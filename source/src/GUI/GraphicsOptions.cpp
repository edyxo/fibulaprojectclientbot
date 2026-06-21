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
#include "../GUI_Elements/GUI_ListBox.h"
#include "../GUI_Elements/GUI_Label.h"

#define GRAPHICS_OPTIONS_TITLE "Graphics Options"
#define GRAPHICS_OPTIONS_WIDTH 236
#define GRAPHICS_OPTIONS_HEIGHT 276
#define GRAPHICS_OPTIONS_CANCEL_EVENTID 1000
#define GRAPHICS_OPTIONS_OK_EVENTID 1001
#define GRAPHICS_OPTIONS_HELP_LINK "http://www.tibia.com/gameguides/?subtopic=manual&section=options"
#define GRAPHICS_OPTIONS_HELP_EVENTID 1002
#define GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_TITLE "Fullscreen"
#define GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_X 18
#define GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_Y 32
#define GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_W 200
#define GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_H 22
#define GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_EVENTID 1003
#define GRAPHICS_OPTIONS_BUTTON_ADVANCED_TITLE "Advanced"
#define GRAPHICS_OPTIONS_BUTTON_ADVANCED_X 132
#define GRAPHICS_OPTIONS_BUTTON_ADVANCED_Y 199
#define GRAPHICS_OPTIONS_BUTTON_ADVANCED_W GUI_UI_BUTTON_86PX_GRAY_UP_W
#define GRAPHICS_OPTIONS_BUTTON_ADVANCED_H GUI_UI_BUTTON_86PX_GRAY_UP_H
#define GRAPHICS_OPTIONS_BUTTON_ADVANCED_EVENTID 1004
#define GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_X 18
#define GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_Y 82
#define GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_W 200
#define GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_H 104
#define GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_EVENTID 1005
#define GRAPHICS_OPTIONS_LABEL_RESOLUTIONS_TITLE "Available Resolutions:"
#define GRAPHICS_OPTIONS_LABEL_RESOLUTIONS_X 18
#define GRAPHICS_OPTIONS_LABEL_RESOLUTIONS_Y 69
#define GRAPHICS_OPTIONS_LABEL_ADVANCED_TITLE "Advanced Settings"
#define GRAPHICS_OPTIONS_LABEL_ADVANCED_X 18
#define GRAPHICS_OPTIONS_LABEL_ADVANCED_Y 205

extern Engine g_engine;
extern bool g_inited;

void graphics_options_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case GRAPHICS_OPTIONS_HELP_EVENTID: UTIL_OpenURL(GRAPHICS_OPTIONS_HELP_LINK); break;
		case GRAPHICS_OPTIONS_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_GRAPHICSOPTIONS)
				g_engine.removeWindow(pWindow);
		}
		break;
		case GRAPHICS_OPTIONS_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_GRAPHICSOPTIONS)
			{
				GUI_CheckBox* pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_EVENTID));
				if(pCheckBox)
				{
					if(pCheckBox->isChecked() != g_engine.hasFullScreen())
					{
						g_engine.setFullScreen(pCheckBox->isChecked());
						g_inited = false;//ReInit engine so renderer will reset FullScreen
					}
				}

				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_EVENTID));
				if(pListBox)
				{
					Sint32 selectedMode = pListBox->getSelect();
					Sint32 displayIndex = SDL_GetWindowDisplayIndex(g_engine.m_window);
					Sint32 displayModes = SDL_GetNumDisplayModes(displayIndex);
					for(Sint32 i = 0, j = 0; i < displayModes; ++i)
					{
						Sint32 reportBits;
						SDL_DisplayMode displayMode;
						SDL_GetDisplayMode(displayIndex, i, &displayMode);
						if(displayMode.format == SDL_PIXELFORMAT_UNKNOWN)
						{
							//If somehow we get unknown pixelformat report it as 32 Bits
							reportBits = 32;
							SDL_snprintf(g_buffer, sizeof(g_buffer), "%d*%d, 32 Bits, %d Hz", displayMode.w, displayMode.h, displayMode.refresh_rate);
						}
						else
						{
							reportBits = SDL_BITSPERPIXEL(displayMode.format);
							if(reportBits == 24)
							{
								//Check if we want to report it as 32 Bits instead of 24 Bits
								reportBits = SDL_BYTESPERPIXEL(displayMode.format) * 8;
							}
							SDL_snprintf(g_buffer, sizeof(g_buffer), "%d*%d, %d Bits, %d Hz", displayMode.w, displayMode.h, reportBits, displayMode.refresh_rate);
						}
						if(reportBits == 24 || reportBits == 32)
						{
							if(j == selectedMode)
							{
								g_engine.setFullScreenMode(displayMode.w, displayMode.h, reportBits, displayMode.refresh_rate);
								if(g_engine.hasFullScreen())
									g_inited = false;//ReInit engine so renderer will reset FullScreen
								break;
							}
							else
								++j;
						}
					}
				}
				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case GRAPHICS_OPTIONS_BUTTON_ADVANCED_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_GRAPHICSOPTIONS)
				UTIL_advancedGraphicsOptions();
		}
		break;
		default: break;
	}
}

void UTIL_graphicsOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_GRAPHICSOPTIONS);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, GRAPHICS_OPTIONS_WIDTH, GRAPHICS_OPTIONS_HEIGHT), GRAPHICS_OPTIONS_TITLE, GUI_WINDOW_GRAPHICSOPTIONS);
	GUI_CheckBox* newCheckBox = new GUI_CheckBox(iRect(GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_X, GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_Y, GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_W, GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_H), GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_TITLE, g_engine.hasFullScreen(), GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_EVENTID);
	newCheckBox->setBoxEventCallback(&graphics_options_Events, GRAPHICS_OPTIONS_CHECKBOX_FSCREEN_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	GUI_Button* newButton = new GUI_Button(iRect(GRAPHICS_OPTIONS_WIDTH - 56, GRAPHICS_OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&graphics_options_Events, GRAPHICS_OPTIONS_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(GRAPHICS_OPTIONS_WIDTH - 109, GRAPHICS_OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&graphics_options_Events, GRAPHICS_OPTIONS_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(GRAPHICS_OPTIONS_WIDTH - 162, GRAPHICS_OPTIONS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Help");
	newButton->setButtonEventCallback(&graphics_options_Events, GRAPHICS_OPTIONS_HELP_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(GRAPHICS_OPTIONS_BUTTON_ADVANCED_X, GRAPHICS_OPTIONS_BUTTON_ADVANCED_Y, GRAPHICS_OPTIONS_BUTTON_ADVANCED_W, GRAPHICS_OPTIONS_BUTTON_ADVANCED_H), GRAPHICS_OPTIONS_BUTTON_ADVANCED_TITLE);
	newButton->setButtonEventCallback(&graphics_options_Events, GRAPHICS_OPTIONS_BUTTON_ADVANCED_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, GRAPHICS_OPTIONS_HEIGHT - 40, GRAPHICS_OPTIONS_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	GUI_Label* newLabel = new GUI_Label(iRect(GRAPHICS_OPTIONS_LABEL_RESOLUTIONS_X, GRAPHICS_OPTIONS_LABEL_RESOLUTIONS_Y, 0, 0), GRAPHICS_OPTIONS_LABEL_RESOLUTIONS_TITLE);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(GRAPHICS_OPTIONS_LABEL_ADVANCED_X, GRAPHICS_OPTIONS_LABEL_ADVANCED_Y, 0, 0), GRAPHICS_OPTIONS_LABEL_ADVANCED_TITLE);
	newWindow->addChild(newLabel);
	GUI_ListBox* newListBox = new GUI_ListBox(iRect(GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_X, GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_Y, GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_W, GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_H), GRAPHICS_OPTIONS_LISTBOX_RESOLUTIONS_EVENTID);
	Sint32 displayIndex = SDL_GetWindowDisplayIndex(g_engine.m_window);
	Sint32 displayModes = SDL_GetNumDisplayModes(displayIndex);
	Sint32 selectedDisplay = 0;
	for(Sint32 i = 0, j = 0; i < displayModes; ++i)
	{
		Sint32 reportBits;
		SDL_DisplayMode displayMode;
		SDL_GetDisplayMode(displayIndex, i, &displayMode);
		if(displayMode.format == SDL_PIXELFORMAT_UNKNOWN)
		{
			//If somehow we get unknown pixelformat report it as 32 Bits
			reportBits = 32;
			SDL_snprintf(g_buffer, sizeof(g_buffer), "%d*%d, 32 Bits, %d Hz", displayMode.w, displayMode.h, displayMode.refresh_rate);
		}
		else
		{
			reportBits = SDL_BITSPERPIXEL(displayMode.format);
			if(reportBits == 24)
			{
				//Check if we want to report it as 32 Bits instead of 24 Bits
				reportBits = SDL_BYTESPERPIXEL(displayMode.format) * 8;
			}
			SDL_snprintf(g_buffer, sizeof(g_buffer), "%d*%d, %d Bits, %d Hz", displayMode.w, displayMode.h, reportBits, displayMode.refresh_rate);
		}
		if(reportBits == 24 || reportBits == 32)
		{
			if(displayMode.w == g_engine.getFullScreenWidth() && displayMode.h == g_engine.getFullScreenHeight() && reportBits == g_engine.getFullScreenBits() && displayMode.refresh_rate == g_engine.getFullScreenHZ())
				selectedDisplay = j;
			else
				++j;
			newListBox->add(g_buffer);
		}
	}
	newListBox->setSelect(selectedDisplay);
	newListBox->startEvents();
	newWindow->addChild(newListBox);
	g_engine.addWindow(newWindow, true);
}
