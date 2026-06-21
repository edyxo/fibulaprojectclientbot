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
#include "../GUI_Elements/GUI_Label.h"
#include "../game.h"

#define HELP_TITLE "Help"
#define HELP_WIDTH 262
#define HELP_HEIGHT 278
#define HELP_OK_EVENTID 1000
#define HELP_CHELP_BUTTON_TEXT "Client Help"
#define HELP_CHELP_BUTTON_X 18
#define HELP_CHELP_BUTTON_Y 32
#define HELP_CHELP_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define HELP_CHELP_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define HELP_CHELP_EVENTID 1001
#define HELP_CHELP_LABEL_TEXT "Click here to get\ninfo on the client"
#define HELP_CHELP_LABEL_X 117
#define HELP_CHELP_LABEL_Y 31
#define HELP_THINTS_BUTTON_TEXT "Tutorial Hints"
#define HELP_THINTS_BUTTON_X 18
#define HELP_THINTS_BUTTON_Y 65
#define HELP_THINTS_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define HELP_THINTS_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define HELP_THINTS_EVENTID 1002
#define HELP_THINTS_LABEL_TEXT "Show previous\ntutorial hints"
#define HELP_THINTS_LABEL_X 117
#define HELP_THINTS_LABEL_Y 64
#define HELP_THELP_BUTTON_TEXT "Tutor Help"
#define HELP_THELP_BUTTON_X 18
#define HELP_THELP_BUTTON_Y 98
#define HELP_THELP_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define HELP_THELP_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define HELP_THELP_EVENTID 1003
#define HELP_THELP_LABEL_TEXT "Ask a tutor for help"
#define HELP_THELP_LABEL_X 117
#define HELP_THELP_LABEL_Y 102
#define HELP_RVIOLATIONS_BUTTON_TEXT "Rule Violations"
#define HELP_RVIOLATIONS_BUTTON_X 18
#define HELP_RVIOLATIONS_BUTTON_Y 131
#define HELP_RVIOLATIONS_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define HELP_RVIOLATIONS_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define HELP_RVIOLATIONS_EVENTID 1004
#define HELP_RVIOLATIONS_LABEL_TEXT1 "Report rule violations"
#define HELP_RVIOLATIONS_LABEL_TEXT2 "Find out how to report\nrule violations"
#define HELP_RVIOLATIONS_LABEL_X 117
#define HELP_RVIOLATIONS_LABEL_Y1 135
#define HELP_RVIOLATIONS_LABEL_Y2 130
#define HELP_MANUAL_BUTTON_LINK "https://www.tibia.com/gameguides/?subtopic=manual"
#define HELP_MANUAL_BUTTON_TEXT "Manual"
#define HELP_MANUAL_BUTTON_X 18
#define HELP_MANUAL_BUTTON_Y 164
#define HELP_MANUAL_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define HELP_MANUAL_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define HELP_MANUAL_EVENTID 1005
#define HELP_MANUAL_LABEL_TEXT "Read the manual on\nthe website"
#define HELP_MANUAL_LABEL_X 117
#define HELP_MANUAL_LABEL_Y 163
#define HELP_FAQ_BUTTON_LINK "https://www.tibia.com/support/?subtopic=faq"
#define HELP_FAQ_BUTTON_TEXT "FAQ"
#define HELP_FAQ_BUTTON_X 18
#define HELP_FAQ_BUTTON_Y 197
#define HELP_FAQ_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define HELP_FAQ_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define HELP_FAQ_EVENTID 1006
#define HELP_FAQ_LABEL_TEXT "Read the FAQ on\nthe website"
#define HELP_FAQ_LABEL_X 117
#define HELP_FAQ_LABEL_Y 196

extern Engine g_engine;
extern Game g_game;

void help_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case HELP_CHELP_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_HELP)
			{
				g_engine.setAction(CLIENT_ACTION_LENSHELP);
				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case HELP_THINTS_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_HELP)
				UTIL_messageBox("Tutorial Hints", "TODO");
		}
		break;
		case HELP_THELP_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_HELP)
			{
				if(g_engine.isIngame())
					g_game.openHelpChannel();

				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case HELP_RVIOLATIONS_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_HELP)
				UTIL_messageBox("Rule Violations", "TODO");
		}
		break;
		case HELP_MANUAL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
			{
				UTIL_OpenURL(HELP_MANUAL_BUTTON_LINK);
				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case HELP_FAQ_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
			{
				UTIL_OpenURL(HELP_FAQ_BUTTON_LINK);
				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case HELP_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_HELP)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_help()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_HELP);
	if(pWindow)
		return;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, HELP_WIDTH, HELP_HEIGHT), HELP_TITLE, GUI_WINDOW_HELP);
	GUI_Button* newButton = new GUI_Button(iRect(HELP_CHELP_BUTTON_X, HELP_CHELP_BUTTON_Y, HELP_CHELP_BUTTON_W, HELP_CHELP_BUTTON_H), HELP_CHELP_BUTTON_TEXT);
	newButton->setButtonEventCallback(&help_Events, HELP_CHELP_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(HELP_THINTS_BUTTON_X, HELP_THINTS_BUTTON_Y, HELP_THINTS_BUTTON_W, HELP_THINTS_BUTTON_H), HELP_THINTS_BUTTON_TEXT);
	newButton->setButtonEventCallback(&help_Events, HELP_THINTS_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(HELP_THELP_BUTTON_X, HELP_THELP_BUTTON_Y, HELP_THELP_BUTTON_W, HELP_THELP_BUTTON_H), HELP_THELP_BUTTON_TEXT);
	newButton->setButtonEventCallback(&help_Events, HELP_THELP_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(HELP_RVIOLATIONS_BUTTON_X, HELP_RVIOLATIONS_BUTTON_Y, HELP_RVIOLATIONS_BUTTON_W, HELP_RVIOLATIONS_BUTTON_H), HELP_RVIOLATIONS_BUTTON_TEXT);
	newButton->setButtonEventCallback(&help_Events, HELP_RVIOLATIONS_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(HELP_MANUAL_BUTTON_X, HELP_MANUAL_BUTTON_Y, HELP_MANUAL_BUTTON_W, HELP_MANUAL_BUTTON_H), HELP_MANUAL_BUTTON_TEXT);
	newButton->setButtonEventCallback(&help_Events, HELP_MANUAL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(HELP_FAQ_BUTTON_X, HELP_FAQ_BUTTON_Y, HELP_FAQ_BUTTON_W, HELP_FAQ_BUTTON_H), HELP_FAQ_BUTTON_TEXT);
	newButton->setButtonEventCallback(&help_Events, HELP_FAQ_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(HELP_WIDTH - 56, HELP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&help_Events, HELP_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(HELP_WIDTH - 56, HELP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&help_Events, HELP_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Label* newLabel = new GUI_Label(iRect(HELP_CHELP_LABEL_X, HELP_CHELP_LABEL_Y, 0, 0), HELP_CHELP_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(HELP_THINTS_LABEL_X, HELP_THINTS_LABEL_Y, 0, 0), HELP_THINTS_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(HELP_THELP_LABEL_X, HELP_THELP_LABEL_Y, 0, 0), HELP_THELP_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(HELP_RVIOLATIONS_LABEL_X, HELP_RVIOLATIONS_LABEL_Y2, 0, 0), HELP_RVIOLATIONS_LABEL_TEXT2);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(HELP_MANUAL_LABEL_X, HELP_MANUAL_LABEL_Y, 0, 0), HELP_MANUAL_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(HELP_FAQ_LABEL_X, HELP_FAQ_LABEL_Y, 0, 0), HELP_FAQ_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, HELP_HEIGHT - 40, HELP_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}
