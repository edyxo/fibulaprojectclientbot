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
#include "../GUI_Elements/GUI_CheckBox.h"

#define CONSOLE_OPTIONS_TITLE "Console Options"
#define CONSOLE_OPTIONS_WIDTH 316
#define CONSOLE_OPTIONS_HEIGHT 324
#define CONSOLE_OPTIONS_HELP_LINK "http://www.tibia.com/gameguides/?subtopic=manual&section=options"
#define CONSOLE_OPTIONS_HELP_EVENTID 1000
#define CONSOLE_OPTIONS_CANCEL_EVENTID 1001
#define CONSOLE_OPTIONS_OK_EVENTID 1002
#define CONSOLE_OPTIONS_INFO_MSGS_TEXT "Show Info Messages in Console"
#define CONSOLE_OPTIONS_INFO_MSGS_X 18
#define CONSOLE_OPTIONS_INFO_MSGS_Y 32
#define CONSOLE_OPTIONS_INFO_MSGS_W 280
#define CONSOLE_OPTIONS_INFO_MSGS_H 22
#define CONSOLE_OPTIONS_INFO_MSGS_EVENTID 1003
#define CONSOLE_OPTIONS_EVENT_MSGS_TEXT "Show Event Messages in Console"
#define CONSOLE_OPTIONS_EVENT_MSGS_X 18
#define CONSOLE_OPTIONS_EVENT_MSGS_Y 67
#define CONSOLE_OPTIONS_EVENT_MSGS_W 280
#define CONSOLE_OPTIONS_EVENT_MSGS_H 22
#define CONSOLE_OPTIONS_EVENT_MSGS_EVENTID 1004
#define CONSOLE_OPTIONS_STATUS_MSGS_TEXT "Show Status Messages in Console"
#define CONSOLE_OPTIONS_STATUS_MSGS_X 18
#define CONSOLE_OPTIONS_STATUS_MSGS_Y 102
#define CONSOLE_OPTIONS_STATUS_MSGS_W 280
#define CONSOLE_OPTIONS_STATUS_MSGS_H 22
#define CONSOLE_OPTIONS_STATUS_MSGS_EVENTID 1005
#define CONSOLE_OPTIONS_STATUSOTHERS_MSGS_TEXT "Show Status Messages of Others in Console"
#define CONSOLE_OPTIONS_STATUSOTHERS_MSGS_X 18
#define CONSOLE_OPTIONS_STATUSOTHERS_MSGS_Y 137
#define CONSOLE_OPTIONS_STATUSOTHERS_MSGS_W 280
#define CONSOLE_OPTIONS_STATUSOTHERS_MSGS_H 22
#define CONSOLE_OPTIONS_STATUSOTHERS_MSGS_EVENTID 1006
#define CONSOLE_OPTIONS_TIMESTAMPS_TEXT "Show Timestamps in Console"
#define CONSOLE_OPTIONS_TIMESTAMPS_X 18
#define CONSOLE_OPTIONS_TIMESTAMPS_Y 172
#define CONSOLE_OPTIONS_TIMESTAMPS_W 280
#define CONSOLE_OPTIONS_TIMESTAMPS_H 22
#define CONSOLE_OPTIONS_TIMESTAMPS_EVENTID 1007
#define CONSOLE_OPTIONS_LEVELS_TEXT "Show Levels in Console"
#define CONSOLE_OPTIONS_LEVELS_X 18
#define CONSOLE_OPTIONS_LEVELS_Y 207
#define CONSOLE_OPTIONS_LEVELS_W 280
#define CONSOLE_OPTIONS_LEVELS_H 22
#define CONSOLE_OPTIONS_LEVELS_EVENTID 1008
#define CONSOLE_OPTIONS_PRIV_MSGS_TEXT "Show Private Messages in Game Window"
#define CONSOLE_OPTIONS_PRIV_MSGS_X 18
#define CONSOLE_OPTIONS_PRIV_MSGS_Y 242
#define CONSOLE_OPTIONS_PRIV_MSGS_W 280
#define CONSOLE_OPTIONS_PRIV_MSGS_H 22
#define CONSOLE_OPTIONS_PRIV_MSGS_EVENTID 1009

extern Engine g_engine;

void console_options_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case CONSOLE_OPTIONS_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_CONSOLEOPTIONS)
				g_engine.removeWindow(pWindow);
		}
		break;
		case CONSOLE_OPTIONS_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_CONSOLEOPTIONS)
			{
				GUI_CheckBox* pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(CONSOLE_OPTIONS_INFO_MSGS_EVENTID));
				if(pCheckBox)
					g_engine.setShowInfoMessages(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(CONSOLE_OPTIONS_EVENT_MSGS_EVENTID));
				if(pCheckBox)
					g_engine.setShowEventMessages(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(CONSOLE_OPTIONS_STATUS_MSGS_EVENTID));
				if(pCheckBox)
					g_engine.setShowStatusMessages(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(CONSOLE_OPTIONS_STATUSOTHERS_MSGS_EVENTID));
				if(pCheckBox)
					g_engine.setShowStatusOthersMessages(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(CONSOLE_OPTIONS_TIMESTAMPS_EVENTID));
				if(pCheckBox)
					g_engine.setShowTimestamps(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(CONSOLE_OPTIONS_LEVELS_EVENTID));
				if(pCheckBox)
					g_engine.setShowLevels(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(CONSOLE_OPTIONS_PRIV_MSGS_EVENTID));
				if(pCheckBox)
					g_engine.setShowPrivateMessages(pCheckBox->isChecked());

				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case CONSOLE_OPTIONS_HELP_EVENTID: UTIL_OpenURL(CONSOLE_OPTIONS_HELP_LINK); break;
		default: break;
	}
}

void UTIL_consoleOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_CONSOLEOPTIONS);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, CONSOLE_OPTIONS_WIDTH, CONSOLE_OPTIONS_HEIGHT), CONSOLE_OPTIONS_TITLE, GUI_WINDOW_CONSOLEOPTIONS);
	GUI_CheckBox* newCheckBox = new GUI_CheckBox(iRect(CONSOLE_OPTIONS_INFO_MSGS_X, CONSOLE_OPTIONS_INFO_MSGS_Y, CONSOLE_OPTIONS_INFO_MSGS_W, CONSOLE_OPTIONS_INFO_MSGS_H), CONSOLE_OPTIONS_INFO_MSGS_TEXT, g_engine.hasShowInfoMessages(), CONSOLE_OPTIONS_INFO_MSGS_EVENTID);
	newCheckBox->setBoxEventCallback(&console_options_Events, CONSOLE_OPTIONS_INFO_MSGS_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(CONSOLE_OPTIONS_EVENT_MSGS_X, CONSOLE_OPTIONS_EVENT_MSGS_Y, CONSOLE_OPTIONS_EVENT_MSGS_W, CONSOLE_OPTIONS_EVENT_MSGS_H), CONSOLE_OPTIONS_EVENT_MSGS_TEXT, g_engine.hasShowEventMessages(), CONSOLE_OPTIONS_EVENT_MSGS_EVENTID);
	newCheckBox->setBoxEventCallback(&console_options_Events, CONSOLE_OPTIONS_EVENT_MSGS_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(CONSOLE_OPTIONS_STATUS_MSGS_X, CONSOLE_OPTIONS_STATUS_MSGS_Y, CONSOLE_OPTIONS_STATUS_MSGS_W, CONSOLE_OPTIONS_STATUS_MSGS_H), CONSOLE_OPTIONS_STATUS_MSGS_TEXT, g_engine.hasShowStatusMessages(), CONSOLE_OPTIONS_STATUS_MSGS_EVENTID);
	newCheckBox->setBoxEventCallback(&console_options_Events, CONSOLE_OPTIONS_STATUS_MSGS_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(CONSOLE_OPTIONS_STATUSOTHERS_MSGS_X, CONSOLE_OPTIONS_STATUSOTHERS_MSGS_Y, CONSOLE_OPTIONS_STATUSOTHERS_MSGS_W, CONSOLE_OPTIONS_STATUSOTHERS_MSGS_H), CONSOLE_OPTIONS_STATUSOTHERS_MSGS_TEXT, g_engine.hasShowStatusOthersMessages(), CONSOLE_OPTIONS_STATUSOTHERS_MSGS_EVENTID);
	newCheckBox->setBoxEventCallback(&console_options_Events, CONSOLE_OPTIONS_STATUSOTHERS_MSGS_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(CONSOLE_OPTIONS_TIMESTAMPS_X, CONSOLE_OPTIONS_TIMESTAMPS_Y, CONSOLE_OPTIONS_TIMESTAMPS_W, CONSOLE_OPTIONS_TIMESTAMPS_H), CONSOLE_OPTIONS_TIMESTAMPS_TEXT, g_engine.hasShowTimestamps(), CONSOLE_OPTIONS_TIMESTAMPS_EVENTID);
	newCheckBox->setBoxEventCallback(&console_options_Events, CONSOLE_OPTIONS_TIMESTAMPS_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(CONSOLE_OPTIONS_LEVELS_X, CONSOLE_OPTIONS_LEVELS_Y, CONSOLE_OPTIONS_LEVELS_W, CONSOLE_OPTIONS_LEVELS_H), CONSOLE_OPTIONS_LEVELS_TEXT, g_engine.hasShowLevels(), CONSOLE_OPTIONS_LEVELS_EVENTID);
	newCheckBox->setBoxEventCallback(&console_options_Events, CONSOLE_OPTIONS_LEVELS_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(CONSOLE_OPTIONS_PRIV_MSGS_X, CONSOLE_OPTIONS_PRIV_MSGS_Y, CONSOLE_OPTIONS_PRIV_MSGS_W, CONSOLE_OPTIONS_PRIV_MSGS_H), CONSOLE_OPTIONS_PRIV_MSGS_TEXT, g_engine.hasShowPrivateMessages(), CONSOLE_OPTIONS_PRIV_MSGS_EVENTID);
	newCheckBox->setBoxEventCallback(&console_options_Events, CONSOLE_OPTIONS_PRIV_MSGS_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	GUI_Button* newButton = new GUI_Button(iRect(CONSOLE_OPTIONS_WIDTH - 56, CONSOLE_OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&console_options_Events, CONSOLE_OPTIONS_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(CONSOLE_OPTIONS_WIDTH - 109, CONSOLE_OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&console_options_Events, CONSOLE_OPTIONS_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(CONSOLE_OPTIONS_WIDTH - 162, CONSOLE_OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Help");
	newButton->setButtonEventCallback(&console_options_Events, CONSOLE_OPTIONS_HELP_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, CONSOLE_OPTIONS_HEIGHT - 40, CONSOLE_OPTIONS_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow, true);
}
