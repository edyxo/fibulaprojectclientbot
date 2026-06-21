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

#define DEATH_NOTIFICATION_TITLE "You are dead"
#define DEATH_NOTIFICATION_CANCEL_EVENTID 1000
#define DEATH_NOTIFICATION_OK_EVENTID 1001

#define DEATH_NOTIFICATION_WIDTH_REGULAR 340
#define DEATH_NOTIFICATION_HEIGHT_REGULAR 162
#define DEATH_NOTIFICATION_LABEL1_TEXT_REGULAR "Alas! Brave adventurer, you have met a sad fate."
#define DEATH_NOTIFICATION_LABEL1_X_REGULAR 18
#define DEATH_NOTIFICATION_LABEL1_Y_REGULAR 32
#define DEATH_NOTIFICATION_LABEL2_TEXT_REGULAR "But do not despair, for the gods will bring you back"
#define DEATH_NOTIFICATION_LABEL2_X_REGULAR 18
#define DEATH_NOTIFICATION_LABEL2_Y_REGULAR 46
#define DEATH_NOTIFICATION_LABEL3_TEXT_REGULAR "into this world in exchange for a small sacrifice."
#define DEATH_NOTIFICATION_LABEL3_X_REGULAR 18
#define DEATH_NOTIFICATION_LABEL3_Y_REGULAR 60
#define DEATH_NOTIFICATION_LABEL4_TEXT_REGULAR "Simply click on 'Ok' to resume your journeys!"
#define DEATH_NOTIFICATION_LABEL4_X_REGULAR 18
#define DEATH_NOTIFICATION_LABEL4_Y_REGULAR 88

#define DEATH_NOTIFICATION_WIDTH_UNFAIR 340
#define DEATH_NOTIFICATION_HEIGHT_UNFAIR 207
#define DEATH_NOTIFICATION_LABEL1_TEXT_UNFAIR "Alas! Brave adventurer, you have met a sad fate."
#define DEATH_NOTIFICATION_LABEL1_X_UNFAIR 18
#define DEATH_NOTIFICATION_LABEL1_Y_UNFAIR 32
#define DEATH_NOTIFICATION_LABEL2_TEXT_UNFAIR "But do not despair, for the gods will bring you back"
#define DEATH_NOTIFICATION_LABEL2_X_UNFAIR 18
#define DEATH_NOTIFICATION_LABEL2_Y_UNFAIR 46
#define DEATH_NOTIFICATION_LABEL3_TEXT_UNFAIR "into this world in exchange for a small sacrifice."
#define DEATH_NOTIFICATION_LABEL3_X_UNFAIR 18
#define DEATH_NOTIFICATION_LABEL3_Y_UNFAIR 60
#define DEATH_NOTIFICATION_LABEL4_TEXT_UNFAIR "This death penalty has been reduced by %u%%"
#define DEATH_NOTIFICATION_LABEL4_X_UNFAIR 18
#define DEATH_NOTIFICATION_LABEL4_Y_UNFAIR 88
#define DEATH_NOTIFICATION_LABEL5_TEXT_UNFAIR "because it was an unfair fight."
#define DEATH_NOTIFICATION_LABEL5_X_UNFAIR 18
#define DEATH_NOTIFICATION_LABEL5_Y_UNFAIR 102
#define DEATH_NOTIFICATION_LABEL6_TEXT_UNFAIR "Simply click on 'Ok' to resume your journeys!"
#define DEATH_NOTIFICATION_LABEL6_X_UNFAIR 18
#define DEATH_NOTIFICATION_LABEL6_Y_UNFAIR 130

#define DEATH_NOTIFICATION_WIDTH_BLESSED 430
#define DEATH_NOTIFICATION_HEIGHT_BLESSED 192
#define DEATH_NOTIFICATION_LABEL1_TEXT_BLESSED "Alas! Brave adventurer, you have met a sad fate."
#define DEATH_NOTIFICATION_LABEL1_X_BLESSED 18
#define DEATH_NOTIFICATION_LABEL1_Y_BLESSED 32
#define DEATH_NOTIFICATION_LABEL2_TEXT_BLESSED "But do not despair, for the gods will bring you back into this world."
#define DEATH_NOTIFICATION_LABEL2_X_BLESSED 18
#define DEATH_NOTIFICATION_LABEL2_Y_BLESSED 46
#define DEATH_NOTIFICATION_LABEL3_TEXT_BLESSED "This death penalty has been reduced by 100%"
#define DEATH_NOTIFICATION_LABEL3_X_BLESSED 18
#define DEATH_NOTIFICATION_LABEL3_Y_BLESSED 74
#define DEATH_NOTIFICATION_LABEL4_TEXT_BLESSED "because you are blessed with the Adventurer's Blessing."
#define DEATH_NOTIFICATION_LABEL4_X_BLESSED 18
#define DEATH_NOTIFICATION_LABEL4_Y_BLESSED 88
#define DEATH_NOTIFICATION_LABEL5_TEXT_BLESSED "Simply click on 'Ok' to resume your journeys!"
#define DEATH_NOTIFICATION_LABEL5_X_BLESSED 18
#define DEATH_NOTIFICATION_LABEL5_Y_BLESSED 116

extern Engine g_engine;
extern Game g_game;

extern Uint32 g_frameTime;
extern Uint32 g_reloginTimer;
extern bool g_reloginAvailable;

void death_notification_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case DEATH_NOTIFICATION_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_DEATHNOTIFICATION)
			{
				g_engine.removeWindow(pWindow);
				g_game.sendLogout();
				g_engine.releaseConnection();//It is fine todo as we are dead
			}
		}
		break;
		case DEATH_NOTIFICATION_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_DEATHNOTIFICATION)
			{
				g_engine.removeWindow(pWindow);
				if(g_engine.isIngame())
				{
					Sint32 characterId = 0;
					std::string currentName = g_game.getPlayerName();
					std::vector<CharacterDetail>& accountCharList = g_engine.getAccountCharList();
					for(std::vector<CharacterDetail>::iterator it = accountCharList.begin(), end = accountCharList.end(); it != end; ++it)
					{
						//Make sure we got selected our character
						CharacterDetail& character = (*it);
						if(character.name == currentName)
						{
							g_engine.setCharacterSelectId(characterId);
							break;
						}
						++characterId;
					}

					if(g_game.hasGameFeature(GAME_FEATURE_KEEP_CONNECTION_AFTER_DEATH))
					{
						g_game.sendEnterGame();
						//On most servers we probably will get disconnected at this point so schedule relogin just in case
						g_reloginAvailable = true;
						g_reloginTimer = g_frameTime + 5000;
					}
					else
					{
						//Send logout + schedule relogin
						g_game.sendLogout();
						g_engine.releaseConnection();//It is fine todo as we are dead
						g_reloginAvailable = true;
						g_reloginTimer = g_frameTime + 5000;
					}
				}
			}
		}
		break;
		default: break;
	}
}

void UTIL_deathWindow(Uint8 deathType, Uint8 penalty)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_DEATHNOTIFICATION);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	Uint8 type = 0;
	if(deathType == DeathType_Regular)
	{
		if(penalty != 100)
			type = 1;
	}
	else if(deathType == DeathType_Blessed)
		type = 2;

	GUI_Window* newWindow;
	switch(type)
	{
		case 1:
		{
			newWindow = new GUI_Window(iRect(0, 0, DEATH_NOTIFICATION_WIDTH_UNFAIR, DEATH_NOTIFICATION_HEIGHT_UNFAIR), DEATH_NOTIFICATION_TITLE, GUI_WINDOW_DEATHNOTIFICATION);
			GUI_Label* newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL1_X_UNFAIR, DEATH_NOTIFICATION_LABEL1_Y_UNFAIR, 0, 0), DEATH_NOTIFICATION_LABEL1_TEXT_UNFAIR);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL2_X_UNFAIR, DEATH_NOTIFICATION_LABEL2_Y_UNFAIR, 0, 0), DEATH_NOTIFICATION_LABEL2_TEXT_UNFAIR);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL3_X_UNFAIR, DEATH_NOTIFICATION_LABEL3_Y_UNFAIR, 0, 0), DEATH_NOTIFICATION_LABEL3_TEXT_UNFAIR);
			newWindow->addChild(newLabel);
			SDL_snprintf(g_buffer, sizeof(g_buffer), DEATH_NOTIFICATION_LABEL4_TEXT_UNFAIR, SDL_static_cast(Uint32, 100 - penalty));
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL4_X_UNFAIR, DEATH_NOTIFICATION_LABEL4_Y_UNFAIR, 0, 0), g_buffer);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL5_X_UNFAIR, DEATH_NOTIFICATION_LABEL5_Y_UNFAIR, 0, 0), DEATH_NOTIFICATION_LABEL5_TEXT_UNFAIR);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL6_X_UNFAIR, DEATH_NOTIFICATION_LABEL6_Y_UNFAIR, 0, 0), DEATH_NOTIFICATION_LABEL6_TEXT_UNFAIR);
			newWindow->addChild(newLabel);
			GUI_Button* newButton = new GUI_Button(iRect(DEATH_NOTIFICATION_WIDTH_UNFAIR - 56, DEATH_NOTIFICATION_HEIGHT_UNFAIR - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&death_notification_Events, DEATH_NOTIFICATION_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(DEATH_NOTIFICATION_WIDTH_UNFAIR - 109, DEATH_NOTIFICATION_HEIGHT_UNFAIR - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&death_notification_Events, DEATH_NOTIFICATION_OK_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, DEATH_NOTIFICATION_HEIGHT_UNFAIR - 40, DEATH_NOTIFICATION_WIDTH_UNFAIR - 26, 2));
			newWindow->addChild(newSeparator);
		}
		break;
		case 2:
		{
			newWindow = new GUI_Window(iRect(0, 0, DEATH_NOTIFICATION_WIDTH_BLESSED, DEATH_NOTIFICATION_HEIGHT_BLESSED), DEATH_NOTIFICATION_TITLE, GUI_WINDOW_DEATHNOTIFICATION);
			GUI_Label* newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL1_X_BLESSED, DEATH_NOTIFICATION_LABEL1_Y_BLESSED, 0, 0), DEATH_NOTIFICATION_LABEL1_TEXT_BLESSED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL2_X_BLESSED, DEATH_NOTIFICATION_LABEL2_Y_BLESSED, 0, 0), DEATH_NOTIFICATION_LABEL2_TEXT_BLESSED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL3_X_BLESSED, DEATH_NOTIFICATION_LABEL3_Y_BLESSED, 0, 0), DEATH_NOTIFICATION_LABEL3_TEXT_BLESSED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL4_X_BLESSED, DEATH_NOTIFICATION_LABEL4_Y_BLESSED, 0, 0), DEATH_NOTIFICATION_LABEL4_TEXT_BLESSED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL5_X_BLESSED, DEATH_NOTIFICATION_LABEL5_Y_BLESSED, 0, 0), DEATH_NOTIFICATION_LABEL5_TEXT_BLESSED);
			newWindow->addChild(newLabel);
			GUI_Button* newButton = new GUI_Button(iRect(DEATH_NOTIFICATION_WIDTH_BLESSED - 56, DEATH_NOTIFICATION_HEIGHT_BLESSED - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&death_notification_Events, DEATH_NOTIFICATION_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(DEATH_NOTIFICATION_WIDTH_BLESSED - 109, DEATH_NOTIFICATION_HEIGHT_BLESSED - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&death_notification_Events, DEATH_NOTIFICATION_OK_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, DEATH_NOTIFICATION_HEIGHT_BLESSED - 40, DEATH_NOTIFICATION_WIDTH_BLESSED - 26, 2));
			newWindow->addChild(newSeparator);
		}
		break;
		default:
		{
			newWindow = new GUI_Window(iRect(0, 0, DEATH_NOTIFICATION_WIDTH_REGULAR, DEATH_NOTIFICATION_HEIGHT_REGULAR), DEATH_NOTIFICATION_TITLE, GUI_WINDOW_DEATHNOTIFICATION);
			GUI_Label* newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL1_X_REGULAR, DEATH_NOTIFICATION_LABEL1_Y_REGULAR, 0, 0), DEATH_NOTIFICATION_LABEL1_TEXT_REGULAR);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL2_X_REGULAR, DEATH_NOTIFICATION_LABEL2_Y_REGULAR, 0, 0), DEATH_NOTIFICATION_LABEL2_TEXT_REGULAR);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL3_X_REGULAR, DEATH_NOTIFICATION_LABEL3_Y_REGULAR, 0, 0), DEATH_NOTIFICATION_LABEL3_TEXT_REGULAR);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(DEATH_NOTIFICATION_LABEL4_X_REGULAR, DEATH_NOTIFICATION_LABEL4_Y_REGULAR, 0, 0), DEATH_NOTIFICATION_LABEL4_TEXT_REGULAR);
			newWindow->addChild(newLabel);
			GUI_Button* newButton = new GUI_Button(iRect(DEATH_NOTIFICATION_WIDTH_REGULAR - 56, DEATH_NOTIFICATION_HEIGHT_REGULAR - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&death_notification_Events, DEATH_NOTIFICATION_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(DEATH_NOTIFICATION_WIDTH_REGULAR - 109, DEATH_NOTIFICATION_HEIGHT_REGULAR - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&death_notification_Events, DEATH_NOTIFICATION_OK_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, DEATH_NOTIFICATION_HEIGHT_REGULAR - 40, DEATH_NOTIFICATION_WIDTH_REGULAR - 26, 2));
			newWindow->addChild(newSeparator);
		}
		break;
	}
	g_engine.addWindow(newWindow);
}
