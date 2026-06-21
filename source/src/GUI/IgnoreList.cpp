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
#include "../GUI_Elements/GUI_TextBox.h"
#include "../GUI_Elements/GUI_ListBox.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../game.h"

#define IGNORELIST_TITLE "Edit Ignore List and White List"
#define IGNORELIST_WIDTH 534
#define IGNORELIST_HEIGHT 398
#define IGNORELIST_CANCEL_EVENTID 1000
#define IGNORELIST_OK_EVENTID 1001
#define IGNORELIST_LABEL_BLACKLIST_TITLE "Characters you wish to ignore:"
#define IGNORELIST_LABEL_BLACKLIST_X 18
#define IGNORELIST_LABEL_BLACKLIST_Y 69
#define IGNORELIST_LABEL_WHITELIST_TITLE "Characters you allow to message you:"
#define IGNORELIST_LABEL_WHITELIST_X 274
#define IGNORELIST_LABEL_WHITELIST_Y 69
#define IGNORELIST_LABEL_GLOBAL1_TITLE "Global settings for ignoring messages:"
#define IGNORELIST_LABEL_GLOBAL1_X 18
#define IGNORELIST_LABEL_GLOBAL1_Y 264
#define IGNORELIST_LABEL_GLOBAL2_TITLE "Global settings for allowing messages:"
#define IGNORELIST_LABEL_GLOBAL2_X 274
#define IGNORELIST_LABEL_GLOBAL2_Y 264
#define IGNORELIST_ACTIVATE_BLACKLIST_TEXT "Activate Ignore List"
#define IGNORELIST_ACTIVATE_BLACKLIST_X 18
#define IGNORELIST_ACTIVATE_BLACKLIST_Y 32
#define IGNORELIST_ACTIVATE_BLACKLIST_W 242
#define IGNORELIST_ACTIVATE_BLACKLIST_H 22
#define IGNORELIST_ACTIVATE_BLACKLIST_EVENTID 1002
#define IGNORELIST_ACTIVATE_WHITELIST_TEXT "Activate White List"
#define IGNORELIST_ACTIVATE_WHITELIST_X 274
#define IGNORELIST_ACTIVATE_WHITELIST_Y 32
#define IGNORELIST_ACTIVATE_WHITELIST_W 242
#define IGNORELIST_ACTIVATE_WHITELIST_H 22
#define IGNORELIST_ACTIVATE_WHITELIST_EVENTID 1003
#define IGNORELIST_IGNORE_PERMANENTLY_TEXT "Ignore Permanently"
#define IGNORELIST_IGNORE_PERMANENTLY_X 18
#define IGNORELIST_IGNORE_PERMANENTLY_Y 223
#define IGNORELIST_IGNORE_PERMANENTLY_W 242
#define IGNORELIST_IGNORE_PERMANENTLY_H 22
#define IGNORELIST_IGNORE_PERMANENTLY_EVENTID 1004
#define IGNORELIST_ALLOW_PERMANENTLY_TEXT "Allow Permanently"
#define IGNORELIST_ALLOW_PERMANENTLY_X 274
#define IGNORELIST_ALLOW_PERMANENTLY_Y 223
#define IGNORELIST_ALLOW_PERMANENTLY_W 242
#define IGNORELIST_ALLOW_PERMANENTLY_H 22
#define IGNORELIST_ALLOW_PERMANENTLY_EVENTID 1005
#define IGNORELIST_IGNORE_YELLING_TEXT "Ignore Yelling"
#define IGNORELIST_IGNORE_YELLING_X 18
#define IGNORELIST_IGNORE_YELLING_Y 280
#define IGNORELIST_IGNORE_YELLING_W 242
#define IGNORELIST_IGNORE_YELLING_H 22
#define IGNORELIST_IGNORE_YELLING_EVENTID 1006
#define IGNORELIST_IGNORE_MESSAGES_TEXT "Ignore Private Messages"
#define IGNORELIST_IGNORE_MESSAGES_X 18
#define IGNORELIST_IGNORE_MESSAGES_Y 315
#define IGNORELIST_IGNORE_MESSAGES_W 242
#define IGNORELIST_IGNORE_MESSAGES_H 22
#define IGNORELIST_IGNORE_MESSAGES_EVENTID 1007
#define IGNORELIST_ALLOW_MESSAGES_TEXT "Allow VIPs to message you"
#define IGNORELIST_ALLOW_MESSAGES_X 274
#define IGNORELIST_ALLOW_MESSAGES_Y 280
#define IGNORELIST_ALLOW_MESSAGES_W 242
#define IGNORELIST_ALLOW_MESSAGES_H 22
#define IGNORELIST_ALLOW_MESSAGES_EVENTID 1008
#define IGNORELIST_ADD_BLACKLIST_TEXT "Add"
#define IGNORELIST_ADD_BLACKLIST_X 171
#define IGNORELIST_ADD_BLACKLIST_Y 191
#define IGNORELIST_ADD_BLACKLIST_W GUI_UI_BUTTON_43PX_GRAY_UP_W
#define IGNORELIST_ADD_BLACKLIST_H GUI_UI_BUTTON_43PX_GRAY_UP_H
#define IGNORELIST_ADD_BLACKLIST_EVENTID 1009
#define IGNORELIST_DELETE_BLACKLIST_TEXT "Delete"
#define IGNORELIST_DELETE_BLACKLIST_X 217
#define IGNORELIST_DELETE_BLACKLIST_Y 191
#define IGNORELIST_DELETE_BLACKLIST_W GUI_UI_BUTTON_43PX_GRAY_UP_W
#define IGNORELIST_DELETE_BLACKLIST_H GUI_UI_BUTTON_43PX_GRAY_UP_H
#define IGNORELIST_DELETE_BLACKLIST_EVENTID 1010
#define IGNORELIST_ADD_WHITELIST_TEXT "Add"
#define IGNORELIST_ADD_WHITELIST_X 427
#define IGNORELIST_ADD_WHITELIST_Y 191
#define IGNORELIST_ADD_WHITELIST_W GUI_UI_BUTTON_43PX_GRAY_UP_W
#define IGNORELIST_ADD_WHITELIST_H GUI_UI_BUTTON_43PX_GRAY_UP_H
#define IGNORELIST_ADD_WHITELIST_EVENTID 1011
#define IGNORELIST_DELETE_WHITELIST_TEXT "Delete"
#define IGNORELIST_DELETE_WHITELIST_X 473
#define IGNORELIST_DELETE_WHITELIST_Y 191
#define IGNORELIST_DELETE_WHITELIST_W GUI_UI_BUTTON_43PX_GRAY_UP_W
#define IGNORELIST_DELETE_WHITELIST_H GUI_UI_BUTTON_43PX_GRAY_UP_H
#define IGNORELIST_DELETE_WHITELIST_EVENTID 1012
#define IGNORELIST_LISTBOX_BLACKLIST_X 18
#define IGNORELIST_LISTBOX_BLACKLIST_Y 82
#define IGNORELIST_LISTBOX_BLACKLIST_W 242
#define IGNORELIST_LISTBOX_BLACKLIST_H 104
#define IGNORELIST_LISTBOX_BLACKLIST_EVENTID 1013
#define IGNORELIST_LISTBOX_WHITELIST_X 274
#define IGNORELIST_LISTBOX_WHITELIST_Y 82
#define IGNORELIST_LISTBOX_WHITELIST_W 242
#define IGNORELIST_LISTBOX_WHITELIST_H 104
#define IGNORELIST_LISTBOX_WHITELIST_EVENTID 1014
#define IGNORELIST_BLACKLIST_TEXTBOX_X 18
#define IGNORELIST_BLACKLIST_TEXTBOX_Y 193
#define IGNORELIST_BLACKLIST_TEXTBOX_W 150
#define IGNORELIST_BLACKLIST_TEXTBOX_H 16
#define IGNORELIST_BLACKLIST_TEXTBOX_EVENTID 3000
#define IGNORELIST_WHITELIST_TEXTBOX_X 274
#define IGNORELIST_WHITELIST_TEXTBOX_Y 193
#define IGNORELIST_WHITELIST_TEXTBOX_W 150
#define IGNORELIST_WHITELIST_TEXTBOX_H 16
#define IGNORELIST_WHITELIST_TEXTBOX_EVENTID 3001

extern Engine g_engine;
extern Game g_game;

struct Black_White_List
{
	Black_White_List(std::string name, bool perm) : playerName(std::move(name)), permanently(perm) {}

	// non-copyable
	Black_White_List(const Black_White_List&) = delete;
	Black_White_List& operator=(const Black_White_List&) = delete;

	// non-moveable
	Black_White_List(Black_White_List&& rhs) noexcept : playerName(std::move(rhs.playerName)), permanently(rhs.permanently) {}
	Black_White_List& operator=(Black_White_List&& rhs) noexcept
	{
		if(this != &rhs)
		{
			playerName = std::move(rhs.playerName);
			permanently = rhs.permanently;
		}
		return (*this);
	}

	bool operator<(const Black_White_List& b) const
	{
		return playerName < b.playerName;
	}

	std::string playerName;
	bool permanently;
};

std::vector<Black_White_List> g_blackListCopy;
std::vector<Black_White_List> g_whiteListCopy;

void ignorelist_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case IGNORELIST_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				g_engine.removeWindow(pWindow);
				g_blackListCopy.clear(); g_blackListCopy.shrink_to_fit();
				g_whiteListCopy.clear(); g_whiteListCopy.shrink_to_fit();
			}
		}
		break;
		case IGNORELIST_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				g_engine.removeWindow(pWindow);
				GUI_CheckBox* pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(IGNORELIST_ACTIVATE_BLACKLIST_EVENTID));
				if(pCheckBox)
					g_engine.setActivatedBlackList(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(IGNORELIST_ACTIVATE_WHITELIST_EVENTID));
				if(pCheckBox)
					g_engine.setActivatedWhiteList(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(IGNORELIST_IGNORE_YELLING_EVENTID));
				if(pCheckBox)
					g_engine.setIgnoreYellingMessages(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(IGNORELIST_IGNORE_MESSAGES_EVENTID));
				if(pCheckBox)
					g_engine.setIgnorePrivateMessages(pCheckBox->isChecked());
				pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(IGNORELIST_ALLOW_MESSAGES_EVENTID));
				if(pCheckBox)
					g_engine.setAllowVipMessages(pCheckBox->isChecked());

				std::unordered_map<std::string, bool>& blackList = g_engine.getBlackList();
				std::unordered_map<std::string, bool>& whiteList = g_engine.getWhiteList();
				blackList.clear(); blackList.reserve(g_blackListCopy.size());
				whiteList.clear(); whiteList.reserve(g_whiteListCopy.size());
				for(std::vector<Black_White_List>::iterator it = g_blackListCopy.begin(), end = g_blackListCopy.end(); it != end; ++it)
				{
					Black_White_List& blackListElement = (*it);
					blackList[blackListElement.playerName] = blackListElement.permanently;
				}
				for(std::vector<Black_White_List>::iterator it = g_whiteListCopy.begin(), end = g_whiteListCopy.end(); it != end; ++it)
				{
					Black_White_List& whiteListElement = (*it);
					whiteList[whiteListElement.playerName] = whiteListElement.permanently;
				}
				g_blackListCopy.clear(); g_blackListCopy.shrink_to_fit();
				g_whiteListCopy.clear(); g_whiteListCopy.shrink_to_fit();
			}
		}
		break;
		case IGNORELIST_IGNORE_PERMANENTLY_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(IGNORELIST_LISTBOX_BLACKLIST_EVENTID));
				if(pListBox)
				{
					Sint32 select = pListBox->getSelect();
					if(SDL_static_cast(size_t, select) >= g_blackListCopy.size())
						break;

					Black_White_List& blackList = g_blackListCopy[select];
					blackList.permanently = (status == 1);
					if(blackList.permanently)
					{
						std::string displayText("\x0E\xC0\x01\x01");
						displayText.append(blackList.playerName);
						pListBox->set(select, std::move(displayText));
					}
					else
						pListBox->set(select, blackList.playerName);
				}
			}
		}
		break;
		case IGNORELIST_ALLOW_PERMANENTLY_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(IGNORELIST_LISTBOX_WHITELIST_EVENTID));
				if(pListBox)
				{
					Sint32 select = pListBox->getSelect();
					if(SDL_static_cast(size_t, select) >= g_whiteListCopy.size())
						break;

					Black_White_List& whiteList = g_whiteListCopy[select];
					whiteList.permanently = (status == 1);
					if(whiteList.permanently)
					{
						std::string displayText("\x0E\xC0\x01\x01");
						displayText.append(whiteList.playerName);
						pListBox->set(select, std::move(displayText));
					}
					else
						pListBox->set(select, whiteList.playerName);
				}
			}
		}
		break;
		case IGNORELIST_ADD_BLACKLIST_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(IGNORELIST_BLACKLIST_TEXTBOX_EVENTID));
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(IGNORELIST_LISTBOX_BLACKLIST_EVENTID));
				if(pTextBox && pListBox)
				{
					std::string playerName = UTIL_formatCreatureName(pTextBox->getActualText());
					g_blackListCopy.emplace_back(playerName, false);
					pListBox->add(std::move(playerName));
					pListBox->setSelect(SDL_MAX_SINT32);
				}
			}
		}
		break;
		case IGNORELIST_DELETE_BLACKLIST_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(IGNORELIST_LISTBOX_BLACKLIST_EVENTID));
				if(pListBox)
				{
					Sint32 select = pListBox->getSelect();
					if(SDL_static_cast(size_t, select) >= g_blackListCopy.size())
						break;

					pListBox->erase(select);
					g_blackListCopy.erase(g_blackListCopy.begin() + select);
				}
			}
		}
		break;
		case IGNORELIST_ADD_WHITELIST_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(IGNORELIST_WHITELIST_TEXTBOX_EVENTID));
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(IGNORELIST_LISTBOX_WHITELIST_EVENTID));
				if(pTextBox && pListBox)
				{
					std::string playerName = UTIL_formatCreatureName(pTextBox->getActualText());
					g_whiteListCopy.emplace_back(playerName, false);
					pListBox->add(std::move(playerName));
					pListBox->setSelect(SDL_MAX_SINT32);
				}
			}
		}
		break;
		case IGNORELIST_DELETE_WHITELIST_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				GUI_ListBox* pListBox = SDL_static_cast(GUI_ListBox*, pWindow->getChild(IGNORELIST_LISTBOX_WHITELIST_EVENTID));
				if(pListBox)
				{
					Sint32 select = pListBox->getSelect();
					if(SDL_static_cast(size_t, select) >= g_whiteListCopy.size())
						break;
					
					pListBox->erase(select);
					g_whiteListCopy.erase(g_whiteListCopy.begin() + select);
				}
			}
		}
		break;
		case IGNORELIST_LISTBOX_BLACKLIST_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				if(status < 0)
				{
					Sint32 select = ~(status);
					GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(IGNORELIST_BLACKLIST_TEXTBOX_EVENTID));
					if(pTextBox)
					{
						pTextBox->setText(g_blackListCopy[select].playerName);
						pWindow->setActiveElement(pTextBox);
					}
				}
				else
				{
					GUI_CheckBox* pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(IGNORELIST_IGNORE_PERMANENTLY_EVENTID));
					if(pCheckBox)
						pCheckBox->setChecked(g_blackListCopy[status].permanently);
				}
			}
		}
		break;
		case IGNORELIST_LISTBOX_WHITELIST_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_IGNORELIST)
			{
				if(status < 0)
				{
					Sint32 select = ~(status);
					GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(IGNORELIST_WHITELIST_TEXTBOX_EVENTID));
					if(pTextBox)
					{
						pTextBox->setText(g_whiteListCopy[select].playerName);
						pWindow->setActiveElement(pTextBox);
					}
				}
				else
				{
					GUI_CheckBox* pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(IGNORELIST_ALLOW_PERMANENTLY_EVENTID));
					if(pCheckBox)
						pCheckBox->setChecked(g_whiteListCopy[status].permanently);
				}
			}
		}
		break;
		default: break;
	}
}

void UTIL_createIgnoreList()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_IGNORELIST);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	std::unordered_map<std::string, bool>& blackList = g_engine.getBlackList();
	std::unordered_map<std::string, bool>& whiteList = g_engine.getWhiteList();
	g_blackListCopy.clear();
	g_whiteListCopy.clear();
	for(std::unordered_map<std::string, bool>::iterator it = blackList.begin(), end = blackList.end(); it != end; ++it)
		g_blackListCopy.emplace_back(it->first, it->second);

	for(std::unordered_map<std::string, bool>::iterator it = whiteList.begin(), end = whiteList.end(); it != end; ++it)
		g_whiteListCopy.emplace_back(it->first, it->second);

	std::sort(g_blackListCopy.begin(), g_blackListCopy.end());
	std::sort(g_whiteListCopy.begin(), g_whiteListCopy.end());

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, IGNORELIST_WIDTH, IGNORELIST_HEIGHT), IGNORELIST_TITLE, GUI_WINDOW_IGNORELIST);
	GUI_ListBox* newListBox = new GUI_ListBox(iRect(IGNORELIST_LISTBOX_BLACKLIST_X, IGNORELIST_LISTBOX_BLACKLIST_Y, IGNORELIST_LISTBOX_BLACKLIST_W, IGNORELIST_LISTBOX_BLACKLIST_H), IGNORELIST_LISTBOX_BLACKLIST_EVENTID);
	newListBox->setEventCallback(&ignorelist_Events, IGNORELIST_LISTBOX_BLACKLIST_EVENTID);
	for(std::vector<Black_White_List>::iterator it = g_blackListCopy.begin(), end = g_blackListCopy.end(); it != end; ++it)
	{
		Black_White_List& blackListElement = (*it);
		if(blackListElement.permanently)
		{
			std::string displayText("\x0E\xC0\x01\x01");
			displayText.append(blackListElement.playerName);
			newListBox->add(std::move(displayText));
		}
		else
			newListBox->add(blackListElement.playerName);
	}
	newListBox->startEvents();
	newWindow->addChild(newListBox);
	newListBox = new GUI_ListBox(iRect(IGNORELIST_LISTBOX_WHITELIST_X, IGNORELIST_LISTBOX_WHITELIST_Y, IGNORELIST_LISTBOX_WHITELIST_W, IGNORELIST_LISTBOX_WHITELIST_H), IGNORELIST_LISTBOX_WHITELIST_EVENTID);
	newListBox->setEventCallback(&ignorelist_Events, IGNORELIST_LISTBOX_WHITELIST_EVENTID);
	for(std::vector<Black_White_List>::iterator it = g_whiteListCopy.begin(), end = g_whiteListCopy.end(); it != end; ++it)
	{
		Black_White_List& whiteListElement = (*it);
		if(whiteListElement.permanently)
		{
			std::string displayText("\x0E\xC0\x01\x01");
			displayText.append(whiteListElement.playerName);
			newListBox->add(std::move(displayText));
		}
		else
			newListBox->add(whiteListElement.playerName);
	}
	newListBox->startEvents();
	newWindow->addChild(newListBox);
	GUI_TextBox* newTextBox = new GUI_TextBox(iRect(IGNORELIST_BLACKLIST_TEXTBOX_X, IGNORELIST_BLACKLIST_TEXTBOX_Y, IGNORELIST_BLACKLIST_TEXTBOX_W, IGNORELIST_BLACKLIST_TEXTBOX_H), "", IGNORELIST_BLACKLIST_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(32);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	newTextBox = new GUI_TextBox(iRect(IGNORELIST_WHITELIST_TEXTBOX_X, IGNORELIST_WHITELIST_TEXTBOX_Y, IGNORELIST_WHITELIST_TEXTBOX_W, IGNORELIST_WHITELIST_TEXTBOX_H), "", IGNORELIST_WHITELIST_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(32);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_Label* newLabel = new GUI_Label(iRect(IGNORELIST_LABEL_BLACKLIST_X, IGNORELIST_LABEL_BLACKLIST_Y, 0, 0), IGNORELIST_LABEL_BLACKLIST_TITLE);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(IGNORELIST_LABEL_WHITELIST_X, IGNORELIST_LABEL_WHITELIST_Y, 0, 0), IGNORELIST_LABEL_WHITELIST_TITLE);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(IGNORELIST_LABEL_GLOBAL1_X, IGNORELIST_LABEL_GLOBAL1_Y, 0, 0), IGNORELIST_LABEL_GLOBAL1_TITLE);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(IGNORELIST_LABEL_GLOBAL2_X, IGNORELIST_LABEL_GLOBAL2_Y, 0, 0), IGNORELIST_LABEL_GLOBAL2_TITLE);
	newWindow->addChild(newLabel);
	GUI_CheckBox* newCheckBox = new GUI_CheckBox(iRect(IGNORELIST_ACTIVATE_BLACKLIST_X, IGNORELIST_ACTIVATE_BLACKLIST_Y, IGNORELIST_ACTIVATE_BLACKLIST_W, IGNORELIST_ACTIVATE_BLACKLIST_H), IGNORELIST_ACTIVATE_BLACKLIST_TEXT, g_engine.getActivatedBlackList(), IGNORELIST_ACTIVATE_BLACKLIST_EVENTID);
	newCheckBox->setBoxEventCallback(&ignorelist_Events, IGNORELIST_ACTIVATE_BLACKLIST_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(IGNORELIST_ACTIVATE_WHITELIST_X, IGNORELIST_ACTIVATE_WHITELIST_Y, IGNORELIST_ACTIVATE_WHITELIST_W, IGNORELIST_ACTIVATE_WHITELIST_H), IGNORELIST_ACTIVATE_WHITELIST_TEXT, g_engine.getActivatedWhiteList(), IGNORELIST_ACTIVATE_WHITELIST_EVENTID);
	newCheckBox->setBoxEventCallback(&ignorelist_Events, IGNORELIST_ACTIVATE_WHITELIST_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(IGNORELIST_IGNORE_PERMANENTLY_X, IGNORELIST_IGNORE_PERMANENTLY_Y, IGNORELIST_IGNORE_PERMANENTLY_W, IGNORELIST_IGNORE_PERMANENTLY_H), IGNORELIST_IGNORE_PERMANENTLY_TEXT, false, IGNORELIST_IGNORE_PERMANENTLY_EVENTID);
	newCheckBox->setBoxEventCallback(&ignorelist_Events, IGNORELIST_IGNORE_PERMANENTLY_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(IGNORELIST_ALLOW_PERMANENTLY_X, IGNORELIST_ALLOW_PERMANENTLY_Y, IGNORELIST_ALLOW_PERMANENTLY_W, IGNORELIST_ALLOW_PERMANENTLY_H), IGNORELIST_ALLOW_PERMANENTLY_TEXT, false, IGNORELIST_ALLOW_PERMANENTLY_EVENTID);
	newCheckBox->setBoxEventCallback(&ignorelist_Events, IGNORELIST_ALLOW_PERMANENTLY_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(IGNORELIST_IGNORE_YELLING_X, IGNORELIST_IGNORE_YELLING_Y, IGNORELIST_IGNORE_YELLING_W, IGNORELIST_IGNORE_YELLING_H), IGNORELIST_IGNORE_YELLING_TEXT, g_engine.getIgnoreYellingMessages(), IGNORELIST_IGNORE_YELLING_EVENTID);
	newCheckBox->setBoxEventCallback(&ignorelist_Events, IGNORELIST_IGNORE_YELLING_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(IGNORELIST_IGNORE_MESSAGES_X, IGNORELIST_IGNORE_MESSAGES_Y, IGNORELIST_IGNORE_MESSAGES_W, IGNORELIST_IGNORE_MESSAGES_H), IGNORELIST_IGNORE_MESSAGES_TEXT, g_engine.getIgnorePrivateMessages(), IGNORELIST_IGNORE_MESSAGES_EVENTID);
	newCheckBox->setBoxEventCallback(&ignorelist_Events, IGNORELIST_IGNORE_MESSAGES_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(IGNORELIST_ALLOW_MESSAGES_X, IGNORELIST_ALLOW_MESSAGES_Y, IGNORELIST_ALLOW_MESSAGES_W, IGNORELIST_ALLOW_MESSAGES_H), IGNORELIST_ALLOW_MESSAGES_TEXT, g_engine.getAllowVipMessages(), IGNORELIST_ALLOW_MESSAGES_EVENTID);
	newCheckBox->setBoxEventCallback(&ignorelist_Events, IGNORELIST_ALLOW_MESSAGES_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	GUI_Button* newButton = new GUI_Button(iRect(IGNORELIST_WIDTH - 56, IGNORELIST_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&ignorelist_Events, IGNORELIST_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(IGNORELIST_WIDTH - 109, IGNORELIST_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&ignorelist_Events, IGNORELIST_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(IGNORELIST_ADD_BLACKLIST_X, IGNORELIST_ADD_BLACKLIST_Y, IGNORELIST_ADD_BLACKLIST_W, IGNORELIST_ADD_BLACKLIST_H), IGNORELIST_ADD_BLACKLIST_TEXT, IGNORELIST_ADD_BLACKLIST_EVENTID);
	newButton->setButtonEventCallback(&ignorelist_Events, IGNORELIST_ADD_BLACKLIST_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(IGNORELIST_DELETE_BLACKLIST_X, IGNORELIST_DELETE_BLACKLIST_Y, IGNORELIST_DELETE_BLACKLIST_W, IGNORELIST_DELETE_BLACKLIST_H), IGNORELIST_DELETE_BLACKLIST_TEXT, IGNORELIST_DELETE_BLACKLIST_EVENTID);
	newButton->setButtonEventCallback(&ignorelist_Events, IGNORELIST_DELETE_BLACKLIST_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(IGNORELIST_ADD_WHITELIST_X, IGNORELIST_ADD_WHITELIST_Y, IGNORELIST_ADD_WHITELIST_W, IGNORELIST_ADD_WHITELIST_H), IGNORELIST_ADD_WHITELIST_TEXT, IGNORELIST_ADD_WHITELIST_EVENTID);
	newButton->setButtonEventCallback(&ignorelist_Events, IGNORELIST_ADD_WHITELIST_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(IGNORELIST_DELETE_WHITELIST_X, IGNORELIST_DELETE_WHITELIST_Y, IGNORELIST_DELETE_WHITELIST_W, IGNORELIST_DELETE_WHITELIST_H), IGNORELIST_DELETE_WHITELIST_TEXT, IGNORELIST_DELETE_WHITELIST_EVENTID);
	newButton->setButtonEventCallback(&ignorelist_Events, IGNORELIST_DELETE_WHITELIST_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, IGNORELIST_HEIGHT - 40, IGNORELIST_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

void UTIL_toggleIgnore(const std::string& playerName)
{
	std::unordered_map<std::string, bool>& blackList = g_engine.getBlackList();
	std::unordered_map<std::string, bool>::iterator it = blackList.find(playerName);
	if(!g_engine.getActivatedBlackList())
	{
		g_engine.setActivatedBlackList(true);
		if(it == blackList.end())
			blackList[playerName] = false;

		SDL_snprintf(g_buffer, sizeof(g_buffer), "You are now ignoring player %s.", playerName.c_str());
	}
	else
	{
		if(it != blackList.end())
		{
			SDL_snprintf(g_buffer, sizeof(g_buffer), "You are no longer ignoring player %s.", playerName.c_str());
			blackList.erase(it);
		}
		else
		{
			SDL_snprintf(g_buffer, sizeof(g_buffer), "You are now ignoring player %s.", playerName.c_str());
			blackList[playerName] = false;
		}
	}
	g_game.processTextMessage(MessageLogin, g_buffer);
}

bool UTIL_onBlackList(const std::string& playerName)
{
	if(!g_engine.getActivatedBlackList())
		return false;

	std::unordered_map<std::string, bool>& blackList = g_engine.getBlackList();
	return (blackList.find(playerName) != blackList.end());
}

bool UTIL_onWhiteList(const std::string& playerName)
{
	if(g_engine.getAllowVipMessages() && UTIL_haveVipPlayer(playerName))
		return true;

	if(!g_engine.getActivatedWhiteList())
		return false;

	std::unordered_map<std::string, bool>& whiteList = g_engine.getWhiteList();
	return (whiteList.find(playerName) != whiteList.end());
}
