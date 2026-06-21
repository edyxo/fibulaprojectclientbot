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

#define OPTIONS_TITLE "Options"
#define OPTIONS_WIDTH 286
#define OPTIONS_HEIGHT 656
#define OPTIONS_OK_EVENTID 1000
#define OPTIONS_GENERAL_BUTTON_TEXT "General"
#define OPTIONS_GENERAL_BUTTON_X 18
#define OPTIONS_GENERAL_BUTTON_Y 32
#define OPTIONS_GENERAL_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_GENERAL_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_GENERAL_EVENTID 1001
#define OPTIONS_GENERAL_LABEL_TEXT "Change general\ngame options"
#define OPTIONS_GENERAL_LABEL_X 117
#define OPTIONS_GENERAL_LABEL_Y 31
#define OPTIONS_GRAPHICS_BUTTON_TEXT "Graphics"
#define OPTIONS_GRAPHICS_BUTTON_X 18
#define OPTIONS_GRAPHICS_BUTTON_Y 65
#define OPTIONS_GRAPHICS_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_GRAPHICS_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_GRAPHICS_EVENTID 1002
#define OPTIONS_GRAPHICS_LABEL_TEXT "Change graphics and\nperformance settings"
#define OPTIONS_GRAPHICS_LABEL_X 117
#define OPTIONS_GRAPHICS_LABEL_Y 64
#define OPTIONS_CONSOLE_BUTTON_TEXT "Console"
#define OPTIONS_CONSOLE_BUTTON_X 18
#define OPTIONS_CONSOLE_BUTTON_Y 98
#define OPTIONS_CONSOLE_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_CONSOLE_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_CONSOLE_EVENTID 1003
#define OPTIONS_CONSOLE_LABEL_TEXT "Customize the console"
#define OPTIONS_CONSOLE_LABEL_X 117
#define OPTIONS_CONSOLE_LABEL_Y 102
#define OPTIONS_HOTKEYS_BUTTON_TEXT "Hotkeys"
#define OPTIONS_HOTKEYS_BUTTON_X 18
#define OPTIONS_HOTKEYS_BUTTON_Y 131
#define OPTIONS_HOTKEYS_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_HOTKEYS_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_HOTKEYS_EVENTID 1004
#define OPTIONS_HOTKEYS_LABEL_TEXT "Edit your hotkeys"
#define OPTIONS_HOTKEYS_LABEL_X 117
#define OPTIONS_HOTKEYS_LABEL_Y 135
#define OPTIONS_MANA_BUTTON_TEXT "Mana Trainer"
#define OPTIONS_MANA_BUTTON_X 18
#define OPTIONS_MANA_BUTTON_Y 164
#define OPTIONS_MANA_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_MANA_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_MANA_EVENTID 1008
#define OPTIONS_MANA_LABEL_TEXT "Edit mana training"
#define OPTIONS_MANA_LABEL_X 117
#define OPTIONS_MANA_LABEL_Y 168
#define OPTIONS_FISHER_BUTTON_TEXT "Auto Fisher"
#define OPTIONS_FISHER_BUTTON_X 18
#define OPTIONS_FISHER_BUTTON_Y 197
#define OPTIONS_FISHER_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_FISHER_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_FISHER_EVENTID 1009
#define OPTIONS_FISHER_LABEL_TEXT "Auto fishing"
#define OPTIONS_FISHER_LABEL_X 117
#define OPTIONS_FISHER_LABEL_Y 201
#define OPTIONS_AUTOLOGOUT_BUTTON_TEXT "Auto Logout"
#define OPTIONS_AUTOLOGOUT_BUTTON_X 18
#define OPTIONS_AUTOLOGOUT_BUTTON_Y 230
#define OPTIONS_AUTOLOGOUT_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_AUTOLOGOUT_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_AUTOLOGOUT_EVENTID 1010
#define OPTIONS_AUTOLOGOUT_LABEL_TEXT "Auto logout (GM/PK/Player)"
#define OPTIONS_AUTOLOGOUT_LABEL_X 117
#define OPTIONS_AUTOLOGOUT_LABEL_Y 234
#define OPTIONS_AUTORUNER_BUTTON_TEXT "Auto Runer"
#define OPTIONS_AUTORUNER_BUTTON_X 18
#define OPTIONS_AUTORUNER_BUTTON_Y 263
#define OPTIONS_AUTORUNER_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_AUTORUNER_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_AUTORUNER_EVENTID 1011
#define OPTIONS_AUTORUNER_LABEL_TEXT "Auto rune making"
#define OPTIONS_AUTORUNER_LABEL_X 117
#define OPTIONS_AUTORUNER_LABEL_Y 267
#define OPTIONS_AUTOEATER_BUTTON_TEXT "Auto Eat"
#define OPTIONS_AUTOEATER_BUTTON_X 18
#define OPTIONS_AUTOEATER_BUTTON_Y 296
#define OPTIONS_AUTOEATER_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_AUTOEATER_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_AUTOEATER_EVENTID 1012
#define OPTIONS_AUTOEATER_LABEL_TEXT "Auto eat food"
#define OPTIONS_AUTOEATER_LABEL_X 117
#define OPTIONS_AUTOEATER_LABEL_Y 300
#define OPTIONS_AUTOHEALER_BUTTON_TEXT "Auto Healer"
#define OPTIONS_AUTOHEALER_BUTTON_X 18
#define OPTIONS_AUTOHEALER_BUTTON_Y 329
#define OPTIONS_AUTOHEALER_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_AUTOHEALER_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_AUTOHEALER_EVENTID 1014
#define OPTIONS_AUTOHEALER_LABEL_TEXT "Auto heal when HP low"
#define OPTIONS_AUTOHEALER_LABEL_X 117
#define OPTIONS_AUTOHEALER_LABEL_Y 333
#define OPTIONS_COMBOLEADER_BUTTON_TEXT "Combo Leader"
#define OPTIONS_COMBOLEADER_BUTTON_X 18
#define OPTIONS_COMBOLEADER_BUTTON_Y 362
#define OPTIONS_COMBOLEADER_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_COMBOLEADER_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_COMBOLEADER_EVENTID 1015
#define OPTIONS_COMBOLEADER_LABEL_TEXT "Mirror leader runes"
#define OPTIONS_COMBOLEADER_LABEL_X 117
#define OPTIONS_COMBOLEADER_LABEL_Y 366
#define OPTIONS_HELP_BUTTON_TEXT "Help"
#define OPTIONS_HELP_BUTTON_X 18
#define OPTIONS_HELP_BUTTON_Y 395
#define OPTIONS_HELP_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_HELP_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_HELP_EVENTID 1005
#define OPTIONS_HELP_LABEL_TEXT "Show the help menu"
#define OPTIONS_HELP_LABEL_X 117
#define OPTIONS_HELP_LABEL_Y 399
#define OPTIONS_MOTD_BUTTON_TEXT "Motd"
#define OPTIONS_MOTD_BUTTON_X 18
#define OPTIONS_MOTD_BUTTON_Y 428
#define OPTIONS_MOTD_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_MOTD_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_MOTD_EVENTID 1006
#define OPTIONS_MOTD_LABEL_TEXT "Show the most recent\nMessage of the Day"
#define OPTIONS_MOTD_LABEL_X 117
#define OPTIONS_MOTD_LABEL_Y 432
#define OPTIONS_DONATE_BUTTON_TEXT "Donate"
#define OPTIONS_DONATE_BUTTON_X 18
#define OPTIONS_DONATE_BUTTON_Y 461
#define OPTIONS_DONATE_BUTTON_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define OPTIONS_DONATE_BUTTON_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define OPTIONS_DONATE_EVENTID 1013
#define OPTIONS_DONATE_LABEL_TEXT "Support the developer"
#define OPTIONS_DONATE_LABEL_X 117
#define OPTIONS_DONATE_LABEL_Y 465
#define OPTIONS_BUTTONS_BUTTON_TEXT "Control Buttons"
#define OPTIONS_BUTTONS_BUTTON_X 18
#define OPTIONS_BUTTONS_BUTTON_Y 527
#define OPTIONS_BUTTONS_BUTTON_W GUI_UI_BUTTON_86PX_GOLD_UP_W
#define OPTIONS_BUTTONS_BUTTON_H GUI_UI_BUTTON_86PX_GOLD_UP_H
#define OPTIONS_BUTTONS_EVENTID 1007
#define OPTIONS_BUTTONS_LABEL_TEXT "Change your displayed\nbuttons"
#define OPTIONS_BUTTONS_LABEL_X 117
#define OPTIONS_BUTTONS_LABEL_Y 531
#define OPTIONS_HELP_SEPARATOR_X 18
#define OPTIONS_HELP_SEPARATOR_Y 387
#define OPTIONS_HELP_SEPARATOR_W 250
#define OPTIONS_HELP_SEPARATOR_H 2

extern Engine g_engine;

void options_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case OPTIONS_GENERAL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_generalOptions();
		}
		break;
		case OPTIONS_GRAPHICS_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_graphicsOptions();
		}
		break;
		case OPTIONS_CONSOLE_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_consoleOptions();
		}
		break;
		case OPTIONS_HOTKEYS_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_hotkeyOptions();
		}
		break;
		case OPTIONS_MANA_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_manaTrainerOptions();
		}
		break;
		case OPTIONS_FISHER_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_autoFisherOptions();
		}
		break;
		case OPTIONS_AUTOLOGOUT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_autoLogoutOptions();
		}
		break;
		case OPTIONS_AUTORUNER_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_autoRunerOptions();
		}
		break;
		case OPTIONS_AUTOEATER_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_autoEaterOptions();
		}
		break;
		case OPTIONS_AUTOHEALER_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_autoHealerOptions();
		}
		break;
		case OPTIONS_COMBOLEADER_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_comboLeaderOptions();
		}
		break;
		case OPTIONS_DONATE_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_donateOptions();
		}
		break;
		case OPTIONS_HELP_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
			{
				UTIL_help();
				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case OPTIONS_MOTD_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				UTIL_createMotd();
		}
		break;
		case OPTIONS_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				g_engine.removeWindow(pWindow);
		}
		break;
		case OPTIONS_BUTTONS_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OPTIONS)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_options()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_OPTIONS);
	if(pWindow)
		return;

	GUI_Window* newWindow;
	if(g_clientVersion >= 1215)
		newWindow = new GUI_Window(iRect(0, 0, OPTIONS_WIDTH, OPTIONS_HEIGHT), OPTIONS_TITLE, GUI_WINDOW_OPTIONS);
	else
		newWindow = new GUI_Window(iRect(0, 0, OPTIONS_WIDTH, OPTIONS_HEIGHT - 33), OPTIONS_TITLE, GUI_WINDOW_OPTIONS);

	GUI_Button* newButton = new GUI_Button(iRect(OPTIONS_GENERAL_BUTTON_X, OPTIONS_GENERAL_BUTTON_Y, OPTIONS_GENERAL_BUTTON_W, OPTIONS_GENERAL_BUTTON_H), OPTIONS_GENERAL_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_GENERAL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_GRAPHICS_BUTTON_X, OPTIONS_GRAPHICS_BUTTON_Y, OPTIONS_GRAPHICS_BUTTON_W, OPTIONS_GRAPHICS_BUTTON_H), OPTIONS_GRAPHICS_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_GRAPHICS_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_CONSOLE_BUTTON_X, OPTIONS_CONSOLE_BUTTON_Y, OPTIONS_CONSOLE_BUTTON_W, OPTIONS_CONSOLE_BUTTON_H), OPTIONS_CONSOLE_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_CONSOLE_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_HOTKEYS_BUTTON_X, OPTIONS_HOTKEYS_BUTTON_Y, OPTIONS_HOTKEYS_BUTTON_W, OPTIONS_HOTKEYS_BUTTON_H), OPTIONS_HOTKEYS_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_HOTKEYS_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_MANA_BUTTON_X, OPTIONS_MANA_BUTTON_Y, OPTIONS_MANA_BUTTON_W, OPTIONS_MANA_BUTTON_H), OPTIONS_MANA_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_MANA_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_FISHER_BUTTON_X, OPTIONS_FISHER_BUTTON_Y, OPTIONS_FISHER_BUTTON_W, OPTIONS_FISHER_BUTTON_H), OPTIONS_FISHER_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_FISHER_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_AUTOLOGOUT_BUTTON_X, OPTIONS_AUTOLOGOUT_BUTTON_Y, OPTIONS_AUTOLOGOUT_BUTTON_W, OPTIONS_AUTOLOGOUT_BUTTON_H), OPTIONS_AUTOLOGOUT_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_AUTOLOGOUT_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_AUTORUNER_BUTTON_X, OPTIONS_AUTORUNER_BUTTON_Y, OPTIONS_AUTORUNER_BUTTON_W, OPTIONS_AUTORUNER_BUTTON_H), OPTIONS_AUTORUNER_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_AUTORUNER_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_AUTOEATER_BUTTON_X, OPTIONS_AUTOEATER_BUTTON_Y, OPTIONS_AUTOEATER_BUTTON_W, OPTIONS_AUTOEATER_BUTTON_H), OPTIONS_AUTOEATER_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_AUTOEATER_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_AUTOHEALER_BUTTON_X, OPTIONS_AUTOHEALER_BUTTON_Y, OPTIONS_AUTOHEALER_BUTTON_W, OPTIONS_AUTOHEALER_BUTTON_H), OPTIONS_AUTOHEALER_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_AUTOHEALER_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_COMBOLEADER_BUTTON_X, OPTIONS_COMBOLEADER_BUTTON_Y, OPTIONS_COMBOLEADER_BUTTON_W, OPTIONS_COMBOLEADER_BUTTON_H), OPTIONS_COMBOLEADER_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_COMBOLEADER_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_DONATE_BUTTON_X, OPTIONS_DONATE_BUTTON_Y, OPTIONS_DONATE_BUTTON_W, OPTIONS_DONATE_BUTTON_H), OPTIONS_DONATE_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_DONATE_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_HELP_BUTTON_X, OPTIONS_HELP_BUTTON_Y, OPTIONS_HELP_BUTTON_W, OPTIONS_HELP_BUTTON_H), OPTIONS_HELP_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_HELP_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OPTIONS_MOTD_BUTTON_X, OPTIONS_MOTD_BUTTON_Y, OPTIONS_MOTD_BUTTON_W, OPTIONS_MOTD_BUTTON_H), OPTIONS_MOTD_BUTTON_TEXT);
	newButton->setButtonEventCallback(&options_Events, OPTIONS_MOTD_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	if(g_clientVersion >= 1215)
	{
		newButton = new GUI_Button(iRect(OPTIONS_BUTTONS_BUTTON_X, OPTIONS_BUTTONS_BUTTON_Y, OPTIONS_BUTTONS_BUTTON_W, OPTIONS_BUTTONS_BUTTON_H), OPTIONS_BUTTONS_BUTTON_TEXT);
		newButton->setButtonEventCallback(&options_Events, OPTIONS_BUTTONS_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
	}
	if(g_clientVersion >= 1215)
	{
		newButton = new GUI_Button(iRect(OPTIONS_WIDTH - 56, OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
		newButton->setButtonEventCallback(&options_Events, OPTIONS_OK_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
		newButton = new GUI_Button(iRect(OPTIONS_WIDTH - 56, OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ESCAPE_TRIGGER);
		newButton->setButtonEventCallback(&options_Events, OPTIONS_OK_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
	}
	else
	{
		newButton = new GUI_Button(iRect(OPTIONS_WIDTH - 56, OPTIONS_HEIGHT - 63, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
		newButton->setButtonEventCallback(&options_Events, OPTIONS_OK_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
		newButton = new GUI_Button(iRect(OPTIONS_WIDTH - 56, OPTIONS_HEIGHT - 63, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ESCAPE_TRIGGER);
		newButton->setButtonEventCallback(&options_Events, OPTIONS_OK_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
	}
	GUI_Label* newLabel = new GUI_Label(iRect(OPTIONS_GENERAL_LABEL_X, OPTIONS_GENERAL_LABEL_Y, 0, 0), OPTIONS_GENERAL_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_GRAPHICS_LABEL_X, OPTIONS_GRAPHICS_LABEL_Y, 0, 0), OPTIONS_GRAPHICS_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_CONSOLE_LABEL_X, OPTIONS_CONSOLE_LABEL_Y, 0, 0), OPTIONS_CONSOLE_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_HOTKEYS_LABEL_X, OPTIONS_HOTKEYS_LABEL_Y, 0, 0), OPTIONS_HOTKEYS_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_MANA_LABEL_X, OPTIONS_MANA_LABEL_Y, 0, 0), OPTIONS_MANA_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_FISHER_LABEL_X, OPTIONS_FISHER_LABEL_Y, 0, 0), OPTIONS_FISHER_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_AUTOLOGOUT_LABEL_X, OPTIONS_AUTOLOGOUT_LABEL_Y, 0, 0), OPTIONS_AUTOLOGOUT_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_AUTORUNER_LABEL_X, OPTIONS_AUTORUNER_LABEL_Y, 0, 0), OPTIONS_AUTORUNER_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_AUTOEATER_LABEL_X, OPTIONS_AUTOEATER_LABEL_Y, 0, 0), OPTIONS_AUTOEATER_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_AUTOHEALER_LABEL_X, OPTIONS_AUTOHEALER_LABEL_Y, 0, 0), OPTIONS_AUTOHEALER_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_COMBOLEADER_LABEL_X, OPTIONS_COMBOLEADER_LABEL_Y, 0, 0), OPTIONS_COMBOLEADER_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_DONATE_LABEL_X, OPTIONS_DONATE_LABEL_Y, 0, 0), OPTIONS_DONATE_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_HELP_LABEL_X, OPTIONS_HELP_LABEL_Y, 0, 0), OPTIONS_HELP_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OPTIONS_MOTD_LABEL_X, OPTIONS_MOTD_LABEL_Y, 0, 0), OPTIONS_MOTD_LABEL_TEXT);
	newWindow->addChild(newLabel);
	if(g_clientVersion >= 1215)
	{
		newLabel = new GUI_Label(iRect(OPTIONS_BUTTONS_LABEL_X, OPTIONS_BUTTONS_LABEL_Y, 0, 0), OPTIONS_BUTTONS_LABEL_TEXT);
		newWindow->addChild(newLabel);
	}
	GUI_Separator* newSeparator = new GUI_Separator(iRect(OPTIONS_HELP_SEPARATOR_X, OPTIONS_HELP_SEPARATOR_Y, OPTIONS_HELP_SEPARATOR_W, OPTIONS_HELP_SEPARATOR_H));
	newWindow->addChild(newSeparator);
	if(g_clientVersion >= 1215)
	{
		newSeparator = new GUI_Separator(iRect(13, OPTIONS_HEIGHT - 40, OPTIONS_WIDTH - 26, 2));
		newWindow->addChild(newSeparator);
	}
	else
	{
		newSeparator = new GUI_Separator(iRect(13, OPTIONS_HEIGHT - 73, OPTIONS_WIDTH - 26, 2));
		newWindow->addChild(newSeparator);
	}
	g_engine.addWindow(newWindow);
}
