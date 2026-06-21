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
#include "../GUI_Elements/GUI_Grouper.h"
#include "../GUI_Elements/GUI_Label.h"

#define INFO_TITLE "Info"
#define INFO_WIDTH 264
#define INFO_HEIGHT 248
#define INFO_OK_EVENTID 1000
#define INFO_FIRST_LABEL_TEXT PRODUCT_NAME
#define INFO_FIRST_LABEL_X 132
#define INFO_FIRST_LABEL_Y 41
#define INFO_SECOND_LABEL_TEXT "Version 0.5.7 Pre-Alpha"
#define INFO_SECOND_LABEL_X 132
#define INFO_SECOND_LABEL_Y 55
#define INFO_THIRD_LABEL_TEXT "Tibia-Eddie Client"
#define INFO_THIRD_LABEL_X 132
#define INFO_THIRD_LABEL_Y 70
#define INFO_FOURTH_LABEL_TEXT "�Copyright� 2020"
#define INFO_FOURTH_LABEL_X 132
#define INFO_FOURTH_LABEL_Y 84
#define INFO_FIFTH_LABEL_TEXT "Using: Simple DirectMedia Layer 2.0"
#define INFO_FIFTH_LABEL_X 132
#define INFO_FIFTH_LABEL_Y 98
#define INFO_SIXTH_LABEL_TEXT "Using: libcURL 7.66.0"
#define INFO_SIXTH_LABEL_X 132
#define INFO_SIXTH_LABEL_Y 112
#define INFO_SEVENTH_LABEL_TEXT "Using: 7-Zip LZMA SDK 19.00"
#define INFO_SEVENTH_LABEL_X 132
#define INFO_SEVENTH_LABEL_Y 126
#define INFO_EIGHTTH_LABEL_TEXT "Official Website"
#define INFO_EIGHTTH_LABEL_X 27
#define INFO_EIGHTTH_LABEL_Y 164
#define INFO_WEBSITE_TEXT "Tibia.com"
#define INFO_WEBSITE_LINK "https://tibia.com/"
#define INFO_WEBSITE_X 151
#define INFO_WEBSITE_Y 160
#define INFO_WEBSITE_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define INFO_WEBSITE_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define INFO_WEBSITE_EVENTID 1001

extern Engine g_engine;

void info_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case INFO_WEBSITE_EVENTID: UTIL_OpenURL(INFO_WEBSITE_LINK); break;
		case INFO_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_INFO)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_info()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_INFO);
	if(pWindow)
		return;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, INFO_WIDTH, INFO_HEIGHT), INFO_TITLE, GUI_WINDOW_INFO);
	GUI_Label* newLabel = new GUI_Label(iRect(INFO_FIRST_LABEL_X, INFO_FIRST_LABEL_Y, 0, 0), INFO_FIRST_LABEL_TEXT);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(INFO_SECOND_LABEL_X, INFO_SECOND_LABEL_Y, 0, 0), INFO_SECOND_LABEL_TEXT);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(INFO_THIRD_LABEL_X, INFO_THIRD_LABEL_Y, 0, 0), INFO_THIRD_LABEL_TEXT);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(INFO_FOURTH_LABEL_X, INFO_FOURTH_LABEL_Y, 0, 0), INFO_FOURTH_LABEL_TEXT);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(INFO_FIFTH_LABEL_X, INFO_FIFTH_LABEL_Y, 0, 0), INFO_FIFTH_LABEL_TEXT);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(INFO_SIXTH_LABEL_X, INFO_SIXTH_LABEL_Y, 0, 0), INFO_SIXTH_LABEL_TEXT);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(INFO_SEVENTH_LABEL_X, INFO_SEVENTH_LABEL_Y, 0, 0), INFO_SEVENTH_LABEL_TEXT);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(INFO_EIGHTTH_LABEL_X, INFO_EIGHTTH_LABEL_Y, 0, 0), INFO_EIGHTTH_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_Button* newButton = new GUI_Button(iRect(INFO_WEBSITE_X, INFO_WEBSITE_Y, INFO_WEBSITE_W, INFO_WEBSITE_H), INFO_WEBSITE_TEXT);
	newButton->setButtonEventCallback(&info_Events, INFO_WEBSITE_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(INFO_WIDTH - 56, INFO_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&info_Events, INFO_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(INFO_WIDTH - 56, INFO_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&info_Events, INFO_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Grouper* newGrouper = new GUI_Grouper(iRect(18, 32, INFO_WIDTH - 36, INFO_HEIGHT - 91));
	newWindow->addChild(newGrouper);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(27, INFO_HEIGHT - 105, INFO_WIDTH - 54, 2));
	newWindow->addChild(newSeparator);
	newSeparator = new GUI_Separator(iRect(13, INFO_HEIGHT - 40, INFO_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}
