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

#define LOGOUT_TITLE "Warning"
#define LOGOUT_WIDTH 253
#define LOGOUT_HEIGHT 100
#define LOGOUT_NO_EVENTID 1000
#define LOGOUT_YES_EVENTID 1001
#define LOGOUT_LABEL_TEXT "Are you sure you want to leave Tibia?"
#define LOGOUT_LABEL_X 18
#define LOGOUT_LABEL_Y 32

extern Engine g_engine;
extern Game g_game;

void logout_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case LOGOUT_NO_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_LOGOUT)
				g_engine.removeWindow(pWindow);
		}
		break;
		case LOGOUT_YES_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_LOGOUT)
			{
				g_engine.removeWindow(pWindow);
				g_game.sendLogout();
			}
		}
		break;
		default: break;
	}
}

void UTIL_logout()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_LOGOUT);
	if(pWindow)
		return;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, LOGOUT_WIDTH, LOGOUT_HEIGHT), LOGOUT_TITLE, GUI_WINDOW_LOGOUT);
	GUI_Label* newLabel = new GUI_Label(iRect(LOGOUT_LABEL_X, LOGOUT_LABEL_Y, 0, 0), LOGOUT_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_Button* newButton = new GUI_Button(iRect(LOGOUT_WIDTH - 56, LOGOUT_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "No", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&logout_Events, LOGOUT_NO_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(LOGOUT_WIDTH - 109, LOGOUT_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Yes", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&logout_Events, LOGOUT_YES_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, LOGOUT_HEIGHT - 40, LOGOUT_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow, true);
}
