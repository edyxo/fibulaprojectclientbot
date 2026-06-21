/*
  The Forgotten Client
  Copyright (C) 2020 Saiyans King

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../GUI_Elements/GUI_Window.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_ListBox.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../game.h"

#define MODAL_DIALOG_PADDING_WIDTH 32
#define MODAL_DIALOG_MINIMUM_WIDTH 300
#define MODAL_DIALOG_MAXIMUM_WIDTH 600
#define MODAL_DIALOG_HEIGHT 94
#define MODAL_DIALOG_CHOICES_EVENTID 1000
#define MODAL_DIALOG_BUTTONS_EVENTID 10000

extern Engine g_engine;
extern Game g_game;

std::map<Sint32, Uint8> g_choiceMap;
Uint32 g_modalWindowId = 0;
Uint8 g_escapeButtonId = 0;
Uint8 g_enterButtonId = 0;

void modal_Events(Uint32 event, Sint32 status)
{
	if(event == MODAL_DIALOG_CHOICES_EVENTID)
	{
		GUI_Window* pWindow;
		if(status < 0 && (pWindow = g_engine.getCurrentWindow()) != NULL && pWindow->getInternalID() == GUI_WINDOW_MODALDIALOG)
		{
			Sint32 select = ~(status);
			std::map<Sint32, Uint8>::iterator it = g_choiceMap.find(select);
			if(it != g_choiceMap.end())
				g_game.sendAnswerModalDialog(g_modalWindowId, g_enterButtonId, it->second);

			g_engine.removeWindow(pWindow);
			g_choiceMap.clear();
		}
	}
	else if(event >= MODAL_DIALOG_BUTTONS_EVENTID)
	{
		GUI_Window* pWindow = g_engine.getCurrentWindow();
		if(pWindow && pWindow->getInternalID() == GUI_WINDOW_MODALDIALOG)
		{
			GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(MODAL_DIALOG_CHOICES_EVENTID));
			if(pListBox)
			{
				Sint32 select = pListBox->getSelect();
				std::map<Sint32, Uint8>::iterator it = g_choiceMap.find(select);
				if(it != g_choiceMap.end())
					g_game.sendAnswerModalDialog(g_modalWindowId, SDL_static_cast(Uint8, event - MODAL_DIALOG_BUTTONS_EVENTID), it->second);
			}

			g_engine.removeWindow(pWindow);
			g_choiceMap.clear();
		}
	}
}

void UTIL_createModalDialog(Uint32 windowId, bool priority, const std::string& title, const std::string& message, Uint8 enterButtonId, Uint8 escapeButtonId, std::vector<std::pair<std::string, Uint8>>& buttons, std::vector<std::pair<std::string, Uint8>>& choices)
{
	struct PARSE_ModalDialog
	{
		std::string* m_msg;
		StringVector m_modalMsgs;
		Sint32 m_dialogWidth;
		Sint32 m_dialogHeight;
	};

	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_MODALDIALOG);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	g_modalWindowId = windowId;
	g_escapeButtonId = escapeButtonId;
	g_enterButtonId = enterButtonId;

	PARSE_ModalDialog modalDialogParser;
	modalDialogParser.m_msg = SDL_const_cast(std::string*, &message);
	modalDialogParser.m_dialogWidth = MODAL_DIALOG_MINIMUM_WIDTH;
	modalDialogParser.m_dialogHeight = MODAL_DIALOG_HEIGHT;

	UTIL_parseSizedText(message, 0, CLIENT_FONT_NONOUTLINED, MODAL_DIALOG_MAXIMUM_WIDTH, reinterpret_cast<void*>(&modalDialogParser), [](void* __THIS, bool, size_t start, size_t length) -> size_t
	{
		PARSE_ModalDialog* _THIS = reinterpret_cast<PARSE_ModalDialog*>(__THIS);
		_THIS->m_dialogWidth = UTIL_max<Sint32>(_THIS->m_dialogWidth, g_engine.calculateFontWidth(CLIENT_FONT_NONOUTLINED, *_THIS->m_msg, start, length));
		_THIS->m_modalMsgs.push_back(_THIS->m_msg->substr(start, length));
		_THIS->m_dialogHeight += 14;
		return 0;
	});

	modalDialogParser.m_dialogWidth = UTIL_max<Sint32>(modalDialogParser.m_dialogWidth, SDL_static_cast(Sint32, buttons.size() * 53) + 6);
	modalDialogParser.m_dialogWidth = UTIL_max<Sint32>(modalDialogParser.m_dialogWidth, g_engine.calculateFontWidth(CLIENT_FONT_NONOUTLINED, title));

	Sint32 choicesY = 0;
	Sint32 choicesHeight = 0;
	if(!choices.empty())
	{
		choicesY = modalDialogParser.m_dialogHeight - 60;
		choicesHeight = (UTIL_min<Sint32>(10, UTIL_max<Sint32>(3, SDL_static_cast(Sint32, choices.size()))) * 14) + 6;
		modalDialogParser.m_dialogHeight += choicesHeight;
	}
	if(modalDialogParser.m_dialogWidth > MODAL_DIALOG_MAXIMUM_WIDTH)
		modalDialogParser.m_dialogWidth = MODAL_DIALOG_MAXIMUM_WIDTH;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, modalDialogParser.m_dialogWidth + MODAL_DIALOG_PADDING_WIDTH, modalDialogParser.m_dialogHeight), title, GUI_WINDOW_MODALDIALOG);
	GUI_Label* newLabel;
	Sint32 labelY = 34;
	for(size_t i = 0, end = modalDialogParser.m_modalMsgs.size(); i < end; ++i)
	{
		newLabel = new GUI_Label(iRect(16, labelY, 0, 0), modalDialogParser.m_modalMsgs[i]);
		newWindow->addChild(newLabel);
		labelY += 14;
	}
	GUI_Button* newButton;
	Sint32 buttonStart = modalDialogParser.m_dialogWidth + MODAL_DIALOG_PADDING_WIDTH - 59;
	for(std::vector<std::pair<std::string, Uint8>>::reverse_iterator it = buttons.rbegin(); it != buttons.rend(); ++it)
	{
		newButton = new GUI_Button(iRect(buttonStart, modalDialogParser.m_dialogHeight - 30, 43, 20), it->first, (it->second == escapeButtonId ? CLIENT_GUI_ESCAPE_TRIGGER : it->second == enterButtonId ? CLIENT_GUI_ENTER_TRIGGER : 0));
		newButton->setButtonEventCallback(&modal_Events, MODAL_DIALOG_BUTTONS_EVENTID + SDL_static_cast(Uint32, it->second));
		newButton->startEvents();
		newWindow->addChild(newButton);
		buttonStart -= 53;
	}
	if(!choices.empty())
	{
		Sint32 count = 0;
		g_choiceMap.clear();

		GUI_ListBox* newListBox = new GUI_ListBox(iRect(16, choicesY, modalDialogParser.m_dialogWidth, choicesHeight), MODAL_DIALOG_CHOICES_EVENTID);
		for(std::vector<std::pair<std::string, Uint8>>::iterator it = choices.begin(); it != choices.end(); ++it)
		{
			g_choiceMap[count++] = it->second;
			newListBox->add(it->first);
		}
		newListBox->setSelect(0);
		newListBox->setEventCallback(&modal_Events, MODAL_DIALOG_CHOICES_EVENTID);
		newListBox->startEvents();
		newWindow->addChild(newListBox);
	}
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, modalDialogParser.m_dialogHeight - 40, modalDialogParser.m_dialogWidth + MODAL_DIALOG_PADDING_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow, priority);
}
