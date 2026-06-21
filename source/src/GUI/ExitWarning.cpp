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

#define EXIT_WARNING_TITLE "Warning"
#define EXIT_WARNING_WIDTH 518
#define EXIT_WARNING_HEIGHT 134
#define EXIT_WARNING_CANCEL_EVENTID 1000
#define EXIT_WARNING_LOGOUT_EVENTID 1001
#define EXIT_WARNING_EXIT_EVENTID 1002
#define EXIT_WARNING_FIRST_LABEL_TEXT "If you shut down the program, your character might stay in the game."
#define EXIT_WARNING_FIRST_LABEL_X 18
#define EXIT_WARNING_FIRST_LABEL_Y 32
#define EXIT_WARNING_SECOND_LABEL_TEXT "Click on \"Logout\" to ensure that your character leaves the game properly."
#define EXIT_WARNING_SECOND_LABEL_X 18
#define EXIT_WARNING_SECOND_LABEL_Y 46
#define EXIT_WARNING_THIRD_LABEL_TEXT "Click on \"Exit\" if you want to exit the program without logging out your character."
#define EXIT_WARNING_THIRD_LABEL_X 18
#define EXIT_WARNING_THIRD_LABEL_Y 60

extern Engine g_engine;
extern Game g_game;

extern bool g_running;
extern bool g_inited;

void exitWarning_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case EXIT_WARNING_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EXITWARNING)
				g_engine.removeWindow(pWindow);
		}
		break;
		case EXIT_WARNING_LOGOUT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EXITWARNING)
			{
				g_engine.removeWindow(pWindow);
				g_game.sendLogout();
			}
		}
		break;
		case EXIT_WARNING_EXIT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EXITWARNING)
			{
				g_game.sendLogout();
				g_inited = false;
				g_running = false;
			}
		}
		break;
		default: break;
	}
}

void UTIL_exitWarning()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_EXITWARNING);
	if(pWindow)
		return;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, EXIT_WARNING_WIDTH, EXIT_WARNING_HEIGHT), EXIT_WARNING_TITLE, GUI_WINDOW_EXITWARNING);
	GUI_Label* newLabel = new GUI_Label(iRect(EXIT_WARNING_FIRST_LABEL_X, EXIT_WARNING_FIRST_LABEL_Y, 0, 0), EXIT_WARNING_FIRST_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(EXIT_WARNING_SECOND_LABEL_X, EXIT_WARNING_SECOND_LABEL_Y, 0, 0), EXIT_WARNING_SECOND_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(EXIT_WARNING_THIRD_LABEL_X, EXIT_WARNING_THIRD_LABEL_Y, 0, 0), EXIT_WARNING_THIRD_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_Button* newButton = new GUI_Button(iRect(EXIT_WARNING_WIDTH - 56, EXIT_WARNING_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&exitWarning_Events, EXIT_WARNING_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(EXIT_WARNING_WIDTH - 109, EXIT_WARNING_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Logout", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&exitWarning_Events, EXIT_WARNING_LOGOUT_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(EXIT_WARNING_WIDTH - 162, EXIT_WARNING_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Exit");
	newButton->setButtonEventCallback(&exitWarning_Events, EXIT_WARNING_EXIT_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, EXIT_WARNING_HEIGHT - 40, EXIT_WARNING_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow, true);
}
