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
#include "../GUI_Elements/GUI_MultiTextBox.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../game.h"

#define QUEST_LOG_TITLE "Quest Log"
#define QUEST_LOG_WIDTH 336
#define QUEST_LOG_HEIGHT 360
#define QUEST_LOG_CANCEL_EVENTID 1000
#define QUEST_LOG_OPEN_EVENTID 1001
#define QUEST_LOG_LISTBOX_X 18
#define QUEST_LOG_LISTBOX_Y 32
#define QUEST_LOG_LISTBOX_W 300
#define QUEST_LOG_LISTBOX_H 268
#define QUEST_LOG_LISTBOX_EVENTID 1002

#define QUEST_LINE_TITLE "Quest Line"
#define QUEST_LINE_WIDTH 336
#define QUEST_LINE_HEIGHT 350
#define QUEST_LINE_CANCEL_EVENTID 1100
#define QUEST_LINE_LABEL_X 18
#define QUEST_LINE_LABEL_Y 34
#define QUEST_LINE_LISTBOX_X 18
#define QUEST_LINE_LISTBOX_Y 50
#define QUEST_LINE_LISTBOX_W 300
#define QUEST_LINE_LISTBOX_H 118
#define QUEST_LINE_LISTBOX_EVENTID 1101
#define QUEST_LINE_TEXTBOX_X 18
#define QUEST_LINE_TEXTBOX_Y 172
#define QUEST_LINE_TEXTBOX_W 300
#define QUEST_LINE_TEXTBOX_H 118
#define QUEST_LINE_TEXTBOX_EVENTID 1102

extern Engine g_engine;
extern Game g_game;

std::vector<QuestLogDetail> g_questsLogCache;
std::vector<QuestLineDetail> g_questsLineCache;

void quests_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case QUEST_LOG_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_QUESTLOG)
				g_engine.removeWindow(pWindow);
		}
		break;
		case QUEST_LOG_OPEN_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_QUESTLOG)
			{
				Sint32 select = 0;
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(QUEST_LOG_LISTBOX_EVENTID));
				if(pListBox)
					select = pListBox->getSelect();

				if(select != -1)
					g_game.sendOpenQuestLine(g_questsLogCache[select].questId);
			}
		}
		break;
		case QUEST_LOG_LISTBOX_EVENTID:
		{
			//Negative status means doubleclick
			GUI_Window* pWindow;
			if(status < 0 && (pWindow = g_engine.getCurrentWindow()) != NULL && pWindow->getInternalID() == GUI_WINDOW_QUESTLOG)
			{
				status = ~(status);
				g_game.sendOpenQuestLine(g_questsLogCache[status].questId);
			}
		}
		break;
		case QUEST_LINE_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_QUESTLINE)
				g_engine.removeWindow(pWindow);
		}
		break;
		case QUEST_LINE_LISTBOX_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_QUESTLINE)
			{
				if(status < 0)
					status = ~(status);

				GUI_MultiTextBox* pTextBox = SDL_static_cast(GUI_MultiTextBox*, pWindow->getChild(QUEST_LINE_TEXTBOX_EVENTID));
				if(pTextBox)
					pTextBox->setText(g_questsLineCache[status].missionDescription);
			}
		}
		break;
		default: break;
	}
}

void UTIL_createQuestLog(Uint32 questId, std::vector<QuestLogDetail>& questLogElements)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_QUESTLOG);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	g_questsLogCache = std::move(questLogElements);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, QUEST_LOG_WIDTH, QUEST_LOG_HEIGHT), QUEST_LOG_TITLE, GUI_WINDOW_QUESTLOG);
	GUI_ListBox* newListBox = new GUI_ListBox(iRect(QUEST_LOG_LISTBOX_X, QUEST_LOG_LISTBOX_Y, QUEST_LOG_LISTBOX_W, QUEST_LOG_LISTBOX_H), QUEST_LOG_LISTBOX_EVENTID);
	Sint32 count = 0, select = -1;
	for(std::vector<QuestLogDetail>::iterator it = g_questsLogCache.begin(), end = g_questsLogCache.end(); it != end; ++it)
	{
		QuestLogDetail& questLogElement = (*it);
		if(questLogElement.questCompleted)
			newListBox->add(questLogElement.questName + " (completed)");
		else
			newListBox->add(questLogElement.questName);
		if(SDL_static_cast(Uint32, questLogElement.questId) == questId)
			select = count;
		++count;
	}
	if(select != -1)
		newListBox->setSelect(select);
	newListBox->setEventCallback(&quests_Events, QUEST_LOG_LISTBOX_EVENTID);
	newListBox->startEvents();
	newWindow->addChild(newListBox);
	GUI_Button* newButton = new GUI_Button(iRect(QUEST_LOG_WIDTH - 56, QUEST_LOG_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&quests_Events, QUEST_LOG_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(QUEST_LOG_WIDTH - 109, QUEST_LOG_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Show", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&quests_Events, QUEST_LOG_OPEN_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, QUEST_LOG_HEIGHT - 40, QUEST_LOG_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

void UTIL_createQuestine(Uint16 questId, std::vector<QuestLineDetail>& questLineElements)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_QUESTLINE);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	g_questsLineCache = std::move(questLineElements);

	std::string labelName;
	for(std::vector<QuestLogDetail>::iterator it = g_questsLogCache.begin(), end = g_questsLogCache.end(); it != end; ++it)
	{
		QuestLogDetail& questLogElement = (*it);
		if(questLogElement.questId == questId)
			labelName = questLogElement.questName;
	}

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, QUEST_LINE_WIDTH, QUEST_LINE_HEIGHT), QUEST_LINE_TITLE, GUI_WINDOW_QUESTLINE);
	GUI_Label* newLabel = new GUI_Label(iRect(QUEST_LINE_LABEL_X, QUEST_LINE_LABEL_Y, 0, 0), labelName);
	newWindow->addChild(newLabel);
	GUI_ListBox* newListBox = new GUI_ListBox(iRect(QUEST_LINE_LISTBOX_X, QUEST_LINE_LISTBOX_Y, QUEST_LINE_LISTBOX_W, QUEST_LINE_LISTBOX_H), QUEST_LINE_LISTBOX_EVENTID);
	for(std::vector<QuestLineDetail>::iterator it = g_questsLineCache.begin(), end = g_questsLineCache.end(); it != end; ++it)
	{
		QuestLineDetail& questLineElement = (*it);
		newListBox->add(questLineElement.missionName);
	}
	newListBox->setSelect(0);
	newListBox->setEventCallback(&quests_Events, QUEST_LINE_LISTBOX_EVENTID);
	newListBox->startEvents();
	newWindow->addChild(newListBox);
	GUI_MultiTextBox* newTextBox = new GUI_MultiTextBox(iRect(QUEST_LINE_TEXTBOX_X, QUEST_LINE_TEXTBOX_Y, QUEST_LINE_TEXTBOX_W, QUEST_LINE_TEXTBOX_H), false, (!g_questsLineCache.empty() ? g_questsLineCache[0].missionDescription : ""), QUEST_LINE_TEXTBOX_EVENTID, 175, 175, 175);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_Button* newButton = new GUI_Button(iRect(QUEST_LINE_WIDTH - 56, QUEST_LINE_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&quests_Events, QUEST_LINE_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, QUEST_LINE_HEIGHT - 40, QUEST_LINE_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow, true);
}
