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
#include "../GUI_Elements/GUI_ListBox.h"
#include "../GUI_Elements/GUI_TextBox.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../game.h"
#include "Chat.h"

#define CHANNELS_TITLE "New Channel"
#define CHANNELS_WIDTH 236
#define CHANNELS_HEIGHT 255
#define CHANNELS_CANCEL_EVENTID 1000
#define CHANNELS_OPEN_EVENTID 1001
#define CHANNELS_FIRST_LABEL_TEXT "Select a channel:"
#define CHANNELS_FIRST_LABEL_X 18
#define CHANNELS_FIRST_LABEL_Y 34
#define CHANNELS_SECOND_LABEL_TEXT "Open a private message channel:"
#define CHANNELS_SECOND_LABEL_X 18
#define CHANNELS_SECOND_LABEL_Y 166
#define CHANNELS_LISTBOX_X 18
#define CHANNELS_LISTBOX_Y 47
#define CHANNELS_LISTBOX_W 200
#define CHANNELS_LISTBOX_H 104
#define CHANNELS_LISTBOX_EVENTID 1002
#define CHANNELS_TEXTBOX_X 18
#define CHANNELS_TEXTBOX_Y 179
#define CHANNELS_TEXTBOX_W 200
#define CHANNELS_TEXTBOX_H 16
#define CHANNELS_TEXTBOX_EVENTID 3000

extern Engine g_engine;
extern Game g_game;

std::map<Sint32, Uint32> g_channelsCache;

void channels_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case CHANNELS_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_CHANNELS)
			{
				g_engine.removeWindow(pWindow);
				g_channelsCache.clear();
			}
		}
		break;
		case CHANNELS_OPEN_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_CHANNELS)
			{
				Sint32 select = 0;
				std::string privateChannel;
				GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(CHANNELS_TEXTBOX_EVENTID));
				if(pTextBox)
					privateChannel = pTextBox->getActualText();
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(CHANNELS_LISTBOX_EVENTID));
				if(pListBox)
					select = pListBox->getSelect();
				if(!privateChannel.empty())
					g_game.sendOpenPrivateChannel(privateChannel);
				else
				{
					std::map<Sint32, Uint32>::iterator it = g_channelsCache.find(select);
					if(it != g_channelsCache.end())
					{
						Uint32 channelId = it->second;
						if(channelId == CHANNEL_ID_NPC)
							g_game.openNPCChannel();
						else if(channelId == 0xFFFF)
							g_game.sendCreatePrivateChannel();
						else
							g_game.sendOpenChannel(SDL_static_cast(Uint16, it->second));
					}
				}
				g_engine.removeWindow(pWindow);
				g_channelsCache.clear();
			}
		}
		break;
		case CHANNELS_LISTBOX_EVENTID:
		{
			//Negative status means doubleclick
			GUI_Window* pWindow;
			if(status < 0 && (pWindow = g_engine.getCurrentWindow()) != NULL && pWindow->getInternalID() == GUI_WINDOW_CHANNELS)
			{
				Sint32 select = ~(status);
				std::map<Sint32, Uint32>::iterator it = g_channelsCache.find(select);
				if(it != g_channelsCache.end())
				{
					Uint32 channelId = it->second;
					if(channelId == CHANNEL_ID_NPC)
						g_game.openNPCChannel();
					else if(channelId == 0xFFFF)
						g_game.sendCreatePrivateChannel();
					else
						g_game.sendOpenChannel(SDL_static_cast(Uint16, it->second));
				}

				g_engine.removeWindow(pWindow);
				g_channelsCache.clear();
			}
		}
		break;
		default: break;
	}
}

void UTIL_createChannels(std::vector<ChannelDetail>& channels)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_CHANNELS);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, CHANNELS_WIDTH, CHANNELS_HEIGHT), CHANNELS_TITLE, GUI_WINDOW_CHANNELS);
	GUI_Label* newLabel = new GUI_Label(iRect(CHANNELS_FIRST_LABEL_X, CHANNELS_FIRST_LABEL_Y, 0, 0), CHANNELS_FIRST_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(CHANNELS_SECOND_LABEL_X, CHANNELS_SECOND_LABEL_Y, 0, 0), CHANNELS_SECOND_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_ListBox* newListBox = new GUI_ListBox(iRect(CHANNELS_LISTBOX_X, CHANNELS_LISTBOX_Y, CHANNELS_LISTBOX_W, CHANNELS_LISTBOX_H), CHANNELS_LISTBOX_EVENTID);
	g_channelsCache.clear();
	Sint32 count = 0;
	for(std::vector<ChannelDetail>::iterator it = channels.begin(), end = channels.end(); it != end; ++it)
	{
		ChannelDetail& channel = (*it);
		g_channelsCache[count++] = SDL_static_cast(Uint32, channel.channelId);
		newListBox->add(channel.channelName);
	}
	if(g_game.hasGameFeature(GAME_FEATURE_NPC_INTERFACE))
	{
		g_channelsCache[count++] = CHANNEL_ID_NPC;
		newListBox->add(CHANNEL_NAME_NPC);
	}
	newListBox->setEventCallback(&channels_Events, CHANNELS_LISTBOX_EVENTID);
	newListBox->startEvents();
	newWindow->addChild(newListBox);
	GUI_TextBox* newTextBox = new GUI_TextBox(iRect(CHANNELS_TEXTBOX_X, CHANNELS_TEXTBOX_Y, CHANNELS_TEXTBOX_W, CHANNELS_TEXTBOX_H), "", CHANNELS_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(32);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_Button* newButton = new GUI_Button(iRect(CHANNELS_WIDTH - 56, CHANNELS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&channels_Events, CHANNELS_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(CHANNELS_WIDTH - 109, CHANNELS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Open", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&channels_Events, CHANNELS_OPEN_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, CHANNELS_HEIGHT - 40, CHANNELS_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}