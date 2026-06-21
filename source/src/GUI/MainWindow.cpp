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

#define MAIN_WINDOW_WIDTH 117
#define MAIN_WINDOW_HEIGHT 231
#define MAIN_WINDOW_ENTERGAME_BUTTON_TEXT "Enter Game"
#define MAIN_WINDOW_ENTERGAME_BUTTON_X 15
#define MAIN_WINDOW_ENTERGAME_BUTTON_Y 15
#define MAIN_WINDOW_ENTERGAME_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define MAIN_WINDOW_ENTERGAME_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define MAIN_WINDOW_ENTERGAME_EVENTID 1000
#define MAIN_WINDOW_PROTOCOL_BUTTON_TEXT "Protocol Options"
#define MAIN_WINDOW_PROTOCOL_BUTTON_X 15
#define MAIN_WINDOW_PROTOCOL_BUTTON_Y 45
#define MAIN_WINDOW_PROTOCOL_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define MAIN_WINDOW_PROTOCOL_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define MAIN_WINDOW_PROTOCOL_EVENTID 1001
#define MAIN_WINDOW_BROWSER_BUTTON_TEXT "Server Browser"
#define MAIN_WINDOW_BROWSER_BUTTON_X 15
#define MAIN_WINDOW_BROWSER_BUTTON_Y 75
#define MAIN_WINDOW_BROWSER_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define MAIN_WINDOW_BROWSER_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define MAIN_WINDOW_BROWSER_EVENTID 1002
#define MAIN_WINDOW_ACCESSACC_BUTTON_TEXT "Access Account"
#define MAIN_WINDOW_ACCESSACC_BUTTON_LINK "https://secure.tibia.com/account/?subtopic=accountmanagement"
#define MAIN_WINDOW_ACCESSACC_BUTTON_X 15
#define MAIN_WINDOW_ACCESSACC_BUTTON_Y 105
#define MAIN_WINDOW_ACCESSACC_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define MAIN_WINDOW_ACCESSACC_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define MAIN_WINDOW_ACCESSACC_EVENTID 1003
#define MAIN_WINDOW_OPTIONS_BUTTON_TEXT "Options"
#define MAIN_WINDOW_OPTIONS_BUTTON_X 15
#define MAIN_WINDOW_OPTIONS_BUTTON_Y 135
#define MAIN_WINDOW_OPTIONS_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define MAIN_WINDOW_OPTIONS_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define MAIN_WINDOW_OPTIONS_EVENTID 1004
#define MAIN_WINDOW_INFO_BUTTON_TEXT "Info"
#define MAIN_WINDOW_INFO_BUTTON_X 15
#define MAIN_WINDOW_INFO_BUTTON_Y 165
#define MAIN_WINDOW_INFO_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define MAIN_WINDOW_INFO_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define MAIN_WINDOW_INFO_EVENTID 1005
#define MAIN_WINDOW_EXITGAME_BUTTON_TEXT "Exit Game"
#define MAIN_WINDOW_EXITGAME_BUTTON_X 15
#define MAIN_WINDOW_EXITGAME_BUTTON_Y 195
#define MAIN_WINDOW_EXITGAME_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define MAIN_WINDOW_EXITGAME_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define MAIN_WINDOW_EXITGAME_EVENTID 1006

extern Engine g_engine;
extern bool g_running;
extern bool g_inited;

extern GUI_Window* g_mainWindow;

void mainWindow_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case MAIN_WINDOW_ENTERGAME_EVENTID: UTIL_createEnterGame(); break;
		case MAIN_WINDOW_PROTOCOL_EVENTID: UTIL_protocolOptions(); break;
		case MAIN_WINDOW_BROWSER_EVENTID: UTIL_createServerBrowser(); break;
		case MAIN_WINDOW_ACCESSACC_EVENTID: UTIL_OpenURL(MAIN_WINDOW_ACCESSACC_BUTTON_LINK); break;
		case MAIN_WINDOW_OPTIONS_EVENTID: UTIL_options(); break;
		case MAIN_WINDOW_INFO_EVENTID: UTIL_info(); break;
		case MAIN_WINDOW_EXITGAME_EVENTID:
		{
			g_inited = false;
			g_running = false;
		}
		break;
		default: break;
	}
}

void UTIL_createMainWindow()
{
	#if CLIENT_OVVERIDE_VERSION != 0
	g_mainWindow = new GUI_Window(iRect(0, 0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT - 60), "", GUI_WINDOW_MAIN);
	GUI_Button* newButton = new GUI_Button(iRect(MAIN_WINDOW_ENTERGAME_BUTTON_X, MAIN_WINDOW_ENTERGAME_BUTTON_Y, MAIN_WINDOW_ENTERGAME_BUTTON_W, MAIN_WINDOW_ENTERGAME_BUTTON_H), MAIN_WINDOW_ENTERGAME_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_ENTERGAME_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_ACCESSACC_BUTTON_X, MAIN_WINDOW_ACCESSACC_BUTTON_Y - 60, MAIN_WINDOW_ACCESSACC_BUTTON_W, MAIN_WINDOW_ACCESSACC_BUTTON_H), MAIN_WINDOW_ACCESSACC_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_ACCESSACC_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_OPTIONS_BUTTON_X, MAIN_WINDOW_OPTIONS_BUTTON_Y - 60, MAIN_WINDOW_OPTIONS_BUTTON_W, MAIN_WINDOW_OPTIONS_BUTTON_H), MAIN_WINDOW_OPTIONS_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_OPTIONS_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_INFO_BUTTON_X, MAIN_WINDOW_INFO_BUTTON_Y - 60, MAIN_WINDOW_INFO_BUTTON_W, MAIN_WINDOW_INFO_BUTTON_H), MAIN_WINDOW_INFO_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_INFO_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_EXITGAME_BUTTON_X, MAIN_WINDOW_EXITGAME_BUTTON_Y - 60, MAIN_WINDOW_EXITGAME_BUTTON_W, MAIN_WINDOW_EXITGAME_BUTTON_H), MAIN_WINDOW_EXITGAME_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_EXITGAME_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	g_mainWindow->onReshape(g_engine.getWindowWidth(), g_engine.getWindowHeight());
	#else
	g_mainWindow = new GUI_Window(iRect(0, 0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT), "", GUI_WINDOW_MAIN);
	GUI_Button* newButton = new GUI_Button(iRect(MAIN_WINDOW_ENTERGAME_BUTTON_X, MAIN_WINDOW_ENTERGAME_BUTTON_Y, MAIN_WINDOW_ENTERGAME_BUTTON_W, MAIN_WINDOW_ENTERGAME_BUTTON_H), MAIN_WINDOW_ENTERGAME_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_ENTERGAME_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_PROTOCOL_BUTTON_X, MAIN_WINDOW_PROTOCOL_BUTTON_Y, MAIN_WINDOW_PROTOCOL_BUTTON_W, MAIN_WINDOW_PROTOCOL_BUTTON_H), MAIN_WINDOW_PROTOCOL_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_PROTOCOL_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_BROWSER_BUTTON_X, MAIN_WINDOW_BROWSER_BUTTON_Y, MAIN_WINDOW_BROWSER_BUTTON_W, MAIN_WINDOW_BROWSER_BUTTON_H), MAIN_WINDOW_BROWSER_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_BROWSER_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_ACCESSACC_BUTTON_X, MAIN_WINDOW_ACCESSACC_BUTTON_Y, MAIN_WINDOW_ACCESSACC_BUTTON_W, MAIN_WINDOW_ACCESSACC_BUTTON_H), MAIN_WINDOW_ACCESSACC_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_ACCESSACC_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_OPTIONS_BUTTON_X, MAIN_WINDOW_OPTIONS_BUTTON_Y, MAIN_WINDOW_OPTIONS_BUTTON_W, MAIN_WINDOW_OPTIONS_BUTTON_H), MAIN_WINDOW_OPTIONS_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_OPTIONS_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_INFO_BUTTON_X, MAIN_WINDOW_INFO_BUTTON_Y, MAIN_WINDOW_INFO_BUTTON_W, MAIN_WINDOW_INFO_BUTTON_H), MAIN_WINDOW_INFO_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_INFO_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(MAIN_WINDOW_EXITGAME_BUTTON_X, MAIN_WINDOW_EXITGAME_BUTTON_Y, MAIN_WINDOW_EXITGAME_BUTTON_W, MAIN_WINDOW_EXITGAME_BUTTON_H), MAIN_WINDOW_EXITGAME_BUTTON_TEXT);
	newButton->setButtonEventCallback(&mainWindow_Events, MAIN_WINDOW_EXITGAME_EVENTID);
	newButton->startEvents();
	g_mainWindow->addChild(newButton);
	g_mainWindow->onReshape(g_engine.getWindowWidth(), g_engine.getWindowHeight());
	#endif
}
