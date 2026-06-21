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
#include "../GUI_Elements/GUI_StaticImage.h"
#include "../game.h"

#define TUTORIAL_HINT_CANCEL_EVENTID 1000
#define TUTORIAL_HINT_LABEL_START 1001

extern Engine g_engine;
extern Game g_game;

void tutorialHint_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case TUTORIAL_HINT_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_TUTORIALHINT)
			{
				Uint32 labelId = TUTORIAL_HINT_LABEL_START;
				std::string message;
				while(true)
				{
					GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pWindow->getChild(labelId++));
					if(pLabel)
						message.append(pLabel->getName() + " ");
					else
						break;
				}
				g_game.processTextMessage(MessageTutorialHint, message);
				g_engine.removeWindow(pWindow);
			}
		}
		break;
		default: break;
	}
}

void UTIL_createTutorialHint(Uint32 hintId)
{
	Uint32 labelId = TUTORIAL_HINT_LABEL_START;
	switch(hintId)
	{
		case 1:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 433, 164), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Welcome to Tibia! Use your\x0E\xFF\x7F\x7F arrow keys\x0F or\x0E\xFF\x7F\x7F left-click\x0F with your mouse", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "to walk around.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(186, 74, 43, 30), GUI_TUTORIAL_IMAGE, 233, 340);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(274, 74, 18, 27), GUI_TUTORIAL_IMAGE, 276, 340);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 124, 407, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(377, 135, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(377, 135, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 2:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 428, 259), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Very good! You can also\x0E\xFF\x7F\x7F left-click\x0F on your\x0E\xFF\x7F\x7F automap\x0F to go to a", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "certain spot. Try to click on the\x0E\xFF\x7F\x7F new marker\x0F on your automap after", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "you closed this window.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(160, 89, 110, 110), GUI_TUTORIAL_IMAGE, 233, 230);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 219, 402, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(372, 230, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(372, 230, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 3: UTIL_flashQuestsButton(); break;
		case 4:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 430, 251), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Do you see a\x0E\xFF\x7F\x7F chest\x0F like this in this room?\x0E\xFF\x7F\x7F Right-click\x0F on it and select", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "'\x0E\xFF\x7F\x7FOpen\x0F' from the context menu to take what's inside.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(130, 74, 50, 57), GUI_TUTORIAL_IMAGE, 180, 113);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(162, 102, 128, 89), GUI_TUTORIAL_IMAGE, 212, 141);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 211, 404, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(374, 221, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(374, 221, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 5:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 428, 371), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "You now have a\x0E\xFF\x7F\x7F bag\x0F to carry items. There is also a\x0E\xFF\x7F\x7F coat\x0F inside.\x0E\xFF\x7F\x7F Drag\x0F", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "\x0E\xFF\x7F\x7F" "and drop\x0F it from the\x0E\xFF\x7F\x7F bag\x0F to your\x0E\xFF\x7F\x7F armor slot\x0F to wear it after you", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "closed this window.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(153, 89, 118, 171), GUI_TUTORIAL_IMAGE, 343, 126);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(177, 248, 108, 64), GUI_TUTORIAL_IMAGE, 367, 285);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 331, 402, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(372, 341, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(372, 341, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 6:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 434, 324), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Light your\x0E\xFF\x7F\x7F torch\x0F by\x0E\xFF\x7F\x7F right-clicking\x0F it and selecting '\x0E\xFF\x7F\x7FUse\x0F'. It will slowly", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "burn down as long as it is lit. '\x0E\xFF\x7F\x7FUse\x0F' it again to turn it off once you", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "leave the darkness. You can use many things in Tibia, just explore", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 76, 0, 0), "the room!", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(128, 104, 169, 160), GUI_TUTORIAL_IMAGE, 0, 222);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 284, 408, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(378, 294, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(378, 294, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 7: UTIL_flashBattleWindow(); break;
		case 8:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 426, 245), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Excellent fighting technique! You can loot the\x0E\xFF\x7F\x7F bodies\x0F of creatures", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "which you have killed by\x0E\xFF\x7F\x7F right-clicking\x0F on them and selecting '\x0E\xFF\x7F\x7FOpen\x0F'", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "from the context menu.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(118, 89, 172, 82), GUI_TUTORIAL_IMAGE, 0, 126);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(183, 143, 120, 42), GUI_TUTORIAL_IMAGE, 65, 180);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 205, 400, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(370, 215, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(370, 215, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 9:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 389, 137), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Do you see the\x0E\xFF\x7F\x7F loot window\x0F to your right?\x0E\xFF\x7F\x7F Drag and drop\x0F the", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "\x0E\xFF\x7F\x7F" "cockroach leg\x0F into your\x0E\xFF\x7F\x7F bag\x0F to take it with you! Collect 3", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "cockroach legs for Santiago!", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 97, 363, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(333, 107, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(333, 107, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 10:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 433, 290), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "With your new\x0E\xFF\x7F\x7F shovel\x0F which you can find in your inventory now you", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "can dig holes on certain spots. Follow the path to the east to find", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "such a place! When there\x0E\xFF\x7F\x7F right-click\x0F the shovel, select '\x0E\xFF\x7F\x7FUse with\x0F' and", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 76, 0, 0), "then\x0E\xFF\x7F\x7F left-click\x0F on the\x0E\xFF\x7F\x7F loose stone pile\x0F.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(112, 104, 203, 113), GUI_TUTORIAL_IMAGE, 0, 0);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(144, 217, 148, 13), GUI_TUTORIAL_IMAGE, 32, 113);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 250, 407, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(377, 260, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(377, 260, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 11:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 422, 275), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "With your\x0E\xFF\x7F\x7F rope\x0F you can climb up a rope spot to exit this cave again.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "\x0E\xFF\x7F\x7F" "Right-click\x0F the rope, select '\x0E\xFF\x7F\x7FUse with\x0F' and then\x0E\xFF\x7F\x7F left-click\x0F on the", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "\x0E\xFF\x7F\x7F" "rope spot\x0F.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(102, 89, 190, 80), GUI_TUTORIAL_IMAGE, 203, 0);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(294, 98, 64, 86), GUI_TUTORIAL_IMAGE, 393, 9);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(134, 169, 148, 46), GUI_TUTORIAL_IMAGE, 235, 80);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 235, 396, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(366, 245, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(366, 245, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 12: g_game.sendRequestOutfit(); break;
		case 13:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 426, 403), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Look at the\x0E\xFF\x7F\x7F NPC trade window\x0F to your right. Left-click on the", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "button 'Sell' to see what you can sell to an NPC. The button 'Buy'", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "shows what you can buy from an NPC.\n", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 90, 0, 0), "To sell meat or ham to Carlos,\x0E\xFF\x7F\x7F left-click\x0F on '\x0E\xFF\x7F\x7FSell\x0F' and choose either", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 104, 0, 0), "'\x0E\xFF\x7F\x7Fmeat\x0F' or '\x0E\xFF\x7F\x7Fham\x0F'. If you want to sell more than one piece, left-click", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 118, 0, 0), "and hold the slide bar, then drag it to the desired amount.\x0E\xFF\x7F\x7F Click\x0F '\x0E\xFF\x7F\x7FOk\x0F'", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 132, 0, 0), "to finish the trade.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(128, 161, 175, 161), GUI_TUTORIAL_IMAGE, 300, 348);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(259, 322, 28, 21), GUI_TUTORIAL_IMAGE, 431, 509);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 363, 400, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(370, 373, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(370, 373, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 14:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 427, 182), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Welcome to the village of Rookgaard! You are on your own now, but", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "there are a few new\x0E\xFF\x7F\x7F marks\x0F on your\x0E\xFF\x7F\x7F automap\x0F. Check them out for", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "important locations and NPCs to talk to. Fight monsters to earn", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 76, 0, 0), "money for better equipment and to level up! Once you have reached", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 90, 0, 0), "level 8, you are ready to become a knight, a paladin, a sorcerer or a", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 104, 0, 0), "druid. Have fun!", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 142, 401, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(371, 152, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(371, 152, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 15: /*<Nothing??>*/ break;
		case 16:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 434, 228), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "In this area characters can attack each other, like in most parts of", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "Tibia. Be careful not to attack someone by accident. If you see a", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "character with a\x0E\xFF\x7F\x7F white skull\x0F or with a\x0E\xFF\x7F\x7F red skull\x0F, they have previously", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 76, 0, 0), "attacked or even killed other characters.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(154, 104, 64, 64), GUI_TUTORIAL_IMAGE, 169, 230);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(220, 104, 64, 64), GUI_TUTORIAL_IMAGE, 169, 294);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 188, 408, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(378, 198, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(378, 198, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 17: /*<Nothing??>*/ break;
		case 18: UTIL_flashFollowButton(); break;
		case 19: UTIL_flashHealthPanel(); break;
		case 20:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 423, 384), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "This area is limited to premium citizens only. Buy a premium account", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "to access new areas and unique features in Tibia!", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(62, 74, 300, 250), GUI_TUTORIAL_IMAGE, 0, 382);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 344, 397, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(367, 354, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(367, 354, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 21:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 253, 149), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Use the\x0E\xFF\x7F\x7F arrow keys\x0F to walk on stairs.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(104, 59, 43, 30), GUI_TUTORIAL_IMAGE, 233, 340);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 109, 227, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(197, 119, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(197, 119, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 22:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 242, 242), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Type 'hi' in your\x0E\xFF\x7F\x7F Local Chat\x0F window.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(39, 59, 163, 123), GUI_TUTORIAL_IMAGE, 457, 0);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 202, 216, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(186, 212, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(186, 212, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 23:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 268, 255), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "\x0E\xFF\x7F\x7F" "Right-click exactly\x0F here to use\x0E\xFF\x7F\x7F ladders\x0F.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(38, 59, 71, 91), GUI_TUTORIAL_IMAGE, 461, 123);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(103, 124, 126, 71), GUI_TUTORIAL_IMAGE, 526, 188);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 215, 242, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(212, 225, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(212, 225, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 24:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 402, 358), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Push the branch by\x0E\xFF\x7F\x7F left-clicking\x0F on it, then\x0E\xFF\x7F\x7F hold\x0F the left mouse", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "button\x0E\xFF\x7F\x7F and drag\x0F the branch on its destination.\x0E\xFF\x7F\x7F Right-click\x0F and", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 62, 0, 0), "choose '\x0E\xFF\x7F\x7FUse with\x0F' cart.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(78, 89, 153, 210), GUI_TUTORIAL_IMAGE, 475, 259);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(231, 152, 92, 138), GUI_TUTORIAL_IMAGE, 628, 322);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 318, 376, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(346, 328, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(346, 328, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 25:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 426, 232), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Fight at close distance, but be careful not to let yourself be circled!", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "Always have a health potion ready!", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(162, 74, 108, 98), GUI_TUTORIAL_IMAGE, 620, 0);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 192, 400, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(370, 202, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(370, 202, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 26:
		case 27:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 409, 217), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Keep your distance, your HP are lower, but your attack is higher.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(130, 59, 148, 98), GUI_TUTORIAL_IMAGE, 652, 222);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 177, 383, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(353, 187, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(353, 187, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 28:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 359, 242), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Type the spell right in the chat window and press enter.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(98, 59, 163, 123), GUI_TUTORIAL_IMAGE, 0, 852);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 202, 333, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(303, 212, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(303, 212, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 29:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 289, 328), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Open the '\x0E\xFF\x7F\x7FOptions\x0F' menu and find '\x0E\xFF\x7F\x7FHotkeys\x0F'.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(26, 59, 128, 145), GUI_TUTORIAL_IMAGE, 475, 469);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(149, 89, 113, 179), GUI_TUTORIAL_IMAGE, 598, 499);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 288, 263, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(233, 298, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(233, 298, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 30:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 290, 340), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Enter the right data into the hotkeys menu.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(26, 59, 238, 220), GUI_TUTORIAL_IMAGE, 0, 632);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(171, 279, 8, 1), GUI_TUTORIAL_IMAGE, 145, 852);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 300, 264, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(234, 310, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(234, 310, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 31:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 219, 288), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Press\x0E\xFF\x7F\x7F F1\x0F to cast the new spell.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(18, 59, 183, 169), GUI_TUTORIAL_IMAGE, 720, 320);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 248, 193, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(163, 258, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(163, 258, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 32:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 319, 336), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Find the rune in your inventory,\x0E\xFF\x7F\x7F right-click\x0F on it,", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "choose '\x0E\xFF\x7F\x7FUse with\x0F' and then click on the monster.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(71, 74, 114, 63), GUI_TUTORIAL_IMAGE, 300, 509);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(93, 130, 147, 70), GUI_TUTORIAL_IMAGE, 322, 565);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(72, 189, 175, 87), GUI_TUTORIAL_IMAGE, 301, 624);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 296, 293, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(263, 306, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(263, 306, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 33:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 359, 242), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Type the spell right in the chat window and press enter.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(98, 59, 163, 123), GUI_TUTORIAL_IMAGE, 711, 488);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 202, 293, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(303, 212, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(303, 212, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 34:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 289, 328), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Open the '\x0E\xFF\x7F\x7FOptions\x0F' menu and find '\x0E\xFF\x7F\x7FHotkeys\x0F'.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(26, 59, 128, 145), GUI_TUTORIAL_IMAGE, 475, 469);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(149, 89, 113, 179), GUI_TUTORIAL_IMAGE, 598, 499);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 288, 263, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(233, 298, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(233, 298, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 35:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 290, 355), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Enter the right data into the hotkeys menu.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(26, 59, 238, 236), GUI_TUTORIAL_IMAGE, 238, 711);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 315, 264, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(234, 325, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(234, 325, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 36:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 213, 208), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Press\x0E\xFF\x7F\x7F F2\x0F to cast the new spell.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(60, 59, 92, 89), GUI_TUTORIAL_IMAGE, 800, 222);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 168, 187, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(157, 178, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(157, 178, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 37:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 420, 258), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Click on the\x0E\xFF\x7F\x7F chase opponent\x0F icon in the\x0E\xFF\x7F\x7F combat controls\x0F, then click", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "on a cockroach in the\x0E\xFF\x7F\x7F battle list\x0F.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(101, 74, 51, 78), GUI_TUTORIAL_IMAGE, 620, 98);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(96, 129, 5, 16), GUI_TUTORIAL_IMAGE, 615, 153);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(144, 132, 179, 66), GUI_TUTORIAL_IMAGE, 663, 156);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 218, 394, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(364, 228, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(364, 228, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		case 38:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 415, 358), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Open the\x0E\xFF\x7F\x7F dead cockroach\x0F by right-clicking it and choosing '\x0E\xFF\x7F\x7FOpen\x0F',", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			newLabel = new GUI_Label(iRect(18, 48, 0, 0), "then drag the cockroach leg to your bag.", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_StaticImage* newImage = new GUI_StaticImage(iRect(48, 74, 97, 109), GUI_TUTORIAL_IMAGE, 476, 614);
			newWindow->addChild(newImage);
			newImage = new GUI_StaticImage(iRect(116, 139, 249, 160), GUI_TUTORIAL_IMAGE, 544, 679);
			newWindow->addChild(newImage);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 318, 385, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(359, 328, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(359, 328, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
		default:
		{
			GUI_Window* newWindow = new GUI_Window(iRect(0, 0, 163, 107), "Tutorial Hint", GUI_WINDOW_TUTORIALHINT);
			GUI_Label* newLabel = new GUI_Label(iRect(18, 34, 0, 0), "Unknown tutorial hint", labelId++, 223, 223, 223);
			newLabel->setFont(CLIENT_FONT_OUTLINED);
			newWindow->addChild(newLabel);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(13, 68, 137, 2));
			newWindow->addChild(newSeparator);
			GUI_Button* newButton = new GUI_Button(iRect(107, 78, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			newButton = new GUI_Button(iRect(107, 78, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Close", CLIENT_GUI_ENTER_TRIGGER);
			newButton->setButtonEventCallback(&tutorialHint_Events, TUTORIAL_HINT_CANCEL_EVENTID);
			newButton->startEvents();
			newWindow->addChild(newButton);
			g_engine.addWindow(newWindow);
		}
		break;
	}
}

