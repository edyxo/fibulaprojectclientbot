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
#include "../GUI_Elements/GUI_Panel.h"
#include "../GUI_Elements/GUI_PanelWindow.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_StaticImage.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../GUI_Elements/GUI_TextBox.h"
#include "../GUI_Elements/GUI_MultiTextBox.h"
#include "../GUI_Elements/GUI_ContextMenu.h"
#include "../GUI_Elements/GUI_ScrollBar.h"
#include "../GUI_Elements/GUI_CheckBox.h"
#include "../game.h"
#include "Chat.h"
#include "VipWindow.h"

extern Uint32 g_frameTime;

#define VIP_TITLE "VIP"
#define VIP_MAXIMINI_EVENTID 1000
#define VIP_CLOSE_EVENTID 1001
#define VIP_CONFIGURE_EVENTID 1002
#define VIP_RESIZE_WIDTH_EVENTID 1003
#define VIP_RESIZE_HEIGHT_EVENTID 1004
#define VIP_EXIT_WINDOW_EVENTID 1005
#define VIP_CONTAINER_EVENTID 1006

#define ADD_VIP_TITLE "Add to VIP list"
#define ADD_VIP_WIDTH 360
#define ADD_VIP_HEIGHT 121
#define ADD_VIP_CANCEL_EVENTID 1200
#define ADD_VIP_OK_EVENTID 1201
#define ADD_VIP_DESCRIPTION_LABEL_TEXT "Please enter a character name:"
#define ADD_VIP_DESCRIPTION_LABEL_X 18
#define ADD_VIP_DESCRIPTION_LABEL_Y 32
#define ADD_VIP_TEXTBOX_X 16
#define ADD_VIP_TEXTBOX_Y 46
#define ADD_VIP_TEXTBOX_W 328
#define ADD_VIP_TEXTBOX_H 16
#define ADD_VIP_TEXTBOX_EVENTID 3000

#define EDIT_VIP_TITLE "Edit VIP"
#define EDIT_VIP_WIDTH 285
#define EDIT_VIP_HEIGHT 261
#define EDIT_VIP_CANCEL_EVENTID 1400
#define EDIT_VIP_OK_EVENTID 1401
#define EDIT_VIP_NAME_LABEL_X 18
#define EDIT_VIP_NAME_LABEL_Y 32
#define EDIT_VIP_NAME_SEPARATOR_X 16
#define EDIT_VIP_NAME_SEPARATOR_Y 48
#define EDIT_VIP_NAME_SEPARATOR_W 253
#define EDIT_VIP_NAME_SEPARATOR_H 2
#define EDIT_VIP_TYPE_LABEL_TEXT "Select a type:"
#define EDIT_VIP_TYPE_LABEL_X 18
#define EDIT_VIP_TYPE_LABEL_Y 62
#define EDIT_VIP_DESCRIPTION_LABEL_TEXT "Enter a short description:"
#define EDIT_VIP_DESCRIPTION_LABEL_X 18
#define EDIT_VIP_DESCRIPTION_LABEL_Y 106
#define EDIT_VIP_TEXTBOX_X 16
#define EDIT_VIP_TEXTBOX_Y 122
#define EDIT_VIP_TEXTBOX_W 253
#define EDIT_VIP_TEXTBOX_H 50
#define EDIT_VIP_TEXTBOX_EVENTID 3000
#define EDIT_VIP_NOTIFY_LOGIN_TEXT "Notify on login"
#define EDIT_VIP_NOTIFY_LOGIN_X 16
#define EDIT_VIP_NOTIFY_LOGIN_Y 182
#define EDIT_VIP_NOTIFY_LOGIN_W 253
#define EDIT_VIP_NOTIFY_LOGIN_H 22
#define EDIT_VIP_NOTIFY_LOGIN_EVENTID 1402
#define EDIT_VIP_GROUP_START_EVENTID 10000

#define ADD_GROUP_WIDTH 360
#define ADD_GROUP_HEIGHT 121
#define ADD_GROUP_CANCEL_EVENTID 1600
#define ADD_GROUP_OK_EVENTID 1601
#define ADD_GROUP_DESCRIPTION_LABEL_TEXT "Please enter a group name:"
#define ADD_GROUP_DESCRIPTION_LABEL_X 18
#define ADD_GROUP_DESCRIPTION_LABEL_Y 32
#define ADD_GROUP_TEXTBOX_X 16
#define ADD_GROUP_TEXTBOX_Y 46
#define ADD_GROUP_TEXTBOX_W 328
#define ADD_GROUP_TEXTBOX_H 16
#define ADD_GROUP_TEXTBOX_EVENTID 3000

#define EDIT_GROUP_TITLE "Edit VIP group"
#define EDIT_GROUP_WIDTH 360
#define EDIT_GROUP_HEIGHT 121
#define EDIT_GROUP_CANCEL_EVENTID 1800
#define EDIT_GROUP_OK_EVENTID 1801
#define EDIT_GROUP_DESCRIPTION_LABEL_TEXT "Please enter a character name:"
#define EDIT_GROUP_DESCRIPTION_LABEL_X 18
#define EDIT_GROUP_DESCRIPTION_LABEL_Y 32
#define EDIT_GROUP_TEXTBOX_X 16
#define EDIT_GROUP_TEXTBOX_Y 46
#define EDIT_GROUP_TEXTBOX_W 328
#define EDIT_GROUP_TEXTBOX_H 16
#define EDIT_GROUP_TEXTBOX_EVENTID 3000

#define VIP_POPUP_EDIT_PLAYER 2000
#define VIP_POPUP_REMOVE_PLAYER 2001
#define VIP_POPUP_MESSAGE_PLAYER 2002
#define VIP_POPUP_ADD_PLAYER 2003
#define VIP_POPUP_INVITE_PLAYER 2004
#define VIP_POPUP_EXCLUDE_PLAYER 2005
#define VIP_POPUP_EDIT_GROUP 2006
#define VIP_POPUP_REMOVE_GROUP 2007
#define VIP_POPUP_ADD_GROUP 2008
#define VIP_POPUP_SORT_NAME 2009
#define VIP_POPUP_SORT_TYPE 2010
#define VIP_POPUP_SORT_STATUS 2011
#define VIP_POPUP_HIDE_PLAYER 2012
#define VIP_POPUP_HIDE_GROUP 2013
#define VIP_POPUP_REPORT_NAME 2014
#define VIP_POPUP_COPY_NAME 2015
#define VIP_POPUP_RULE_VIOLATION 2016

extern Engine g_engine;
extern Game g_game;
extern Chat g_chat;

std::unordered_set<std::string> g_vips;
std::vector<VipPlayers> g_vipPlayers;
std::vector<VipGroups> g_vipGroups;
VipAction g_lastVipAction;
Uint32 g_vipIconId = 0;
Uint8 g_createGroupsLeft = 0;

bool g_haveVIPOpen = false;
bool g_needSortVips = true;
bool g_recreateVipWindow = false;
bool g_vipRMouse = false;

void UTIL_createAddVip();
void UTIL_createEditVip(VipPlayers& vipPlayer);
void UTIL_createAddGroup();
void UTIL_createEditGroup(VipGroups& vipGroup);
void vip_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case VIP_MAXIMINI_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP);
			if(pPanel)
			{
				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				iRect& pRect = pPanel->getOriginalRect();
				if(pRect.y2 > 19)
				{
					pPanel->setCachedHeight(pRect.y2);
					pPanel->setSize(pRect.x2, 19);
					parent->checkPanels();

					GUI_VipContainer* pContainer = SDL_static_cast(GUI_VipContainer*, pPanel->getChild(VIP_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeInvisible();
				}
				else
				{
					Sint32 cachedHeight = pPanel->getCachedHeight();
					parent->tryFreeHeight(cachedHeight - pRect.y2);
					pPanel->setSize(pRect.x2, cachedHeight);
					parent->checkPanels();

					GUI_VipContainer* pContainer = SDL_static_cast(GUI_VipContainer*, pPanel->getChild(VIP_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeVisible();
				}
			}
		}
		break;
		case VIP_CLOSE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP);
			if(pPanel)
			{
				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				parent->removePanel(pPanel);
			}
		}
		break;
		case VIP_CONFIGURE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP);
			if(pPanel)
			{
				GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(VIP_CONFIGURE_EVENTID));
				if(pIcon)
				{
					iRect& iconRect = pIcon->getRect();
					VipAction action;
					action.type = VIP_ACTION_NONE;
					UTIL_createVipPopupMenu(iconRect.x1, iconRect.y1 + 12, action);
				}
			}
		}
		break;
		case VIP_RESIZE_HEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP);
			if(pPanel)
			{
				GUI_VipContainer* pContainer = SDL_static_cast(GUI_VipContainer*, pPanel->getChild(VIP_CONTAINER_EVENTID));
				if(pContainer)
				{
					iRect cRect = pContainer->getRect();
					cRect.y2 = status - 19;
					pContainer->setRect(cRect);
				}
			}
		}
		break;
		case VIP_EXIT_WINDOW_EVENTID: {g_engine.setContentWindowHeight(GUI_PANEL_WINDOW_VIP, status); g_haveVIPOpen = false;} break;

		case ADD_VIP_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_ADDVIP)
				g_engine.removeWindow(pWindow);
		}
		break;
		case ADD_VIP_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_ADDVIP)
			{
				g_engine.removeWindow(pWindow);
				GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(ADD_VIP_TEXTBOX_EVENTID));
				if(pTextBox)
					g_game.sendAddVip(pTextBox->getActualText());
			}
		}
		break;

		case EDIT_VIP_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EDITVIP)
				g_engine.removeWindow(pWindow);
		}
		break;
		case EDIT_VIP_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EDITVIP)
			{
				std::vector<Uint8> groups;
				std::string description;
				bool notifyLogin = false;
				g_engine.removeWindow(pWindow);

				GUI_MultiTextBox* pTextBox = SDL_static_cast(GUI_MultiTextBox*, pWindow->getChild(EDIT_VIP_TEXTBOX_EVENTID));
				if(pTextBox)
					description = pTextBox->getText();

				GUI_CheckBox* pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(EDIT_VIP_NOTIFY_LOGIN_EVENTID));
				if(pCheckBox)
					notifyLogin = pCheckBox->isChecked();

				if(g_game.hasGameFeature(GAME_FEATURE_VIP_GROUPS))
				{
					for(Uint8 i = SDL_MIN_UINT8; i <= SDL_MAX_UINT8; ++i)
					{
						pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(EDIT_VIP_GROUP_START_EVENTID + SDL_static_cast(Uint32, i)));
						if(pCheckBox && pCheckBox->isChecked())
							groups.emplace_back(i);
					}
				}

				if(g_game.hasGameFeature(GAME_FEATURE_ADDITIONAL_VIPINFO))
					g_game.sendEditVip(g_lastVipAction.player.playerGUID, description, g_vipIconId, notifyLogin, groups);
				else
					g_engine.setVipData(g_lastVipAction.player.playerGUID, description, g_vipIconId, notifyLogin);

				for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
				{
					VipPlayers& vipPlayer = (*it);
					if(vipPlayer.playerGUID == g_lastVipAction.player.playerGUID)
					{
						vipPlayer.description = std::move(description);
						vipPlayer.iconId = g_vipIconId;
						vipPlayer.notifyLogin = notifyLogin;
						vipPlayer.groups = std::move(groups);
						g_recreateVipWindow = true;
						if(g_engine.getBuddySortMethod() == Vip_Sort_Type)
							g_needSortVips = true;

						break;
					}
				}
			}
		}
		break;

		case ADD_GROUP_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_ADDGROUP)
				g_engine.removeWindow(pWindow);
		}
		break;
		case ADD_GROUP_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_ADDGROUP)
			{
				g_engine.removeWindow(pWindow);
				GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(ADD_VIP_TEXTBOX_EVENTID));
				if(pTextBox)
					g_game.sendVipGroupAction(VIP_GROUP_ACTION_ADD, 0, pTextBox->getActualText());
			}
		}
		break;

		case EDIT_GROUP_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EDITGROUP)
				g_engine.removeWindow(pWindow);
		}
		break;
		case EDIT_GROUP_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EDITGROUP)
			{
				g_engine.removeWindow(pWindow);
				GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(ADD_VIP_TEXTBOX_EVENTID));
				if(pTextBox)
					g_game.sendVipGroupAction(VIP_GROUP_ACTION_EDIT, g_lastVipAction.group.groupId, pTextBox->getActualText());
			}
		}
		break;

		case VIP_POPUP_EDIT_PLAYER:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_PLAYER && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
				{
					VipPlayers& vipPlayer = (*it);
					if(vipPlayer.playerGUID == g_lastVipAction.player.playerGUID)
					{
						UTIL_createEditVip(vipPlayer);
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_REMOVE_PLAYER:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_PLAYER && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
				{
					VipPlayers& vipPlayer = (*it);
					if(vipPlayer.playerGUID == g_lastVipAction.player.playerGUID)
					{
						g_game.sendRemoveVip(vipPlayer.playerGUID);
						g_vips.erase(vipPlayer.playerName);
						g_vipPlayers.erase(it);
						g_recreateVipWindow = true;
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_MESSAGE_PLAYER:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_PLAYER && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
				{
					VipPlayers& vipPlayer = (*it);
					if(vipPlayer.playerGUID == g_lastVipAction.player.playerGUID)
					{
						g_game.sendOpenPrivateChannel(vipPlayer.playerName);
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_ADD_PLAYER:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP);
			if(pPanel)
				UTIL_createAddVip();
		}
		break;
		case VIP_POPUP_INVITE_PLAYER:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_PLAYER && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
				{
					VipPlayers& vipPlayer = (*it);
					if(vipPlayer.playerGUID == g_lastVipAction.player.playerGUID)
					{
						g_game.sendChannelInvite(vipPlayer.playerName, SDL_static_cast(Uint16, g_chat.getOwnPrivateChannel()));
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_EXCLUDE_PLAYER:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_PLAYER && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
				{
					VipPlayers& vipPlayer = (*it);
					if(vipPlayer.playerGUID == g_lastVipAction.player.playerGUID)
					{
						g_game.sendChannelExclude(vipPlayer.playerName, SDL_static_cast(Uint16, g_chat.getOwnPrivateChannel()));
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_EDIT_GROUP:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_GROUP && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipGroups>::iterator it = g_vipGroups.begin(), end = g_vipGroups.end(); it != end; ++it)
				{
					VipGroups& vipGroup = (*it);
					if(vipGroup.groupId == g_lastVipAction.group.groupId)
					{
						UTIL_createEditGroup(vipGroup);
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_REMOVE_GROUP:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_GROUP && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipGroups>::iterator it = g_vipGroups.begin(), end = g_vipGroups.end(); it != end; ++it)
				{
					VipGroups& vipGroup = (*it);
					if(vipGroup.groupId == g_lastVipAction.group.groupId)
					{
						g_game.sendVipGroupAction(VIP_GROUP_ACTION_REMOVE, vipGroup.groupId, "");
						g_vipGroups.erase(it);
						g_recreateVipWindow = true;
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_ADD_GROUP:
		{
			GUI_PanelWindow* pPanel;
			if(g_game.hasGameFeature(GAME_FEATURE_VIP_GROUPS) && g_createGroupsLeft > 0 && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
				UTIL_createAddGroup();
		}
		break;
		case VIP_POPUP_SORT_NAME:
		{
			if(g_engine.getBuddySortMethod() != Vip_Sort_Name)
			{
				g_engine.setBuddySortMethod(Vip_Sort_Name);
				g_needSortVips = true;
			}
		}
		break;
		case VIP_POPUP_SORT_TYPE:
		{
			if(g_engine.getBuddySortMethod() != Vip_Sort_Type)
			{
				g_engine.setBuddySortMethod(Vip_Sort_Type);
				g_needSortVips = true;
			}
		}
		break;
		case VIP_POPUP_SORT_STATUS:
		{
			if(g_engine.getBuddySortMethod() != Vip_Sort_Status)
			{
				g_engine.setBuddySortMethod(Vip_Sort_Status);
				g_needSortVips = true;
			}
		}
		break;
		case VIP_POPUP_HIDE_PLAYER:
		{
			g_engine.setBuddyHideOffline(!g_engine.getBuddyHideOffline());
			g_recreateVipWindow = true;
		}
		break;
		case VIP_POPUP_HIDE_GROUP:
		{
			g_engine.setBuddyHideGroups(!g_engine.getBuddyHideGroups());
			g_recreateVipWindow = true;
		}
		break;
		case VIP_POPUP_REPORT_NAME:
		{
			//
		}
		break;
		case VIP_POPUP_COPY_NAME:
		{
			GUI_PanelWindow* pPanel;
			if(g_lastVipAction.type == VIP_ACTION_PLAYER && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP)) != NULL)
			{
				for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
				{
					VipPlayers& vipPlayer = (*it);
					if(vipPlayer.playerGUID == g_lastVipAction.player.playerGUID)
					{
						UTIL_SetClipboardTextLatin1(vipPlayer.playerName.c_str());
						break;
					}
				}
			}
		}
		break;
		case VIP_POPUP_RULE_VIOLATION:
		{
			//
		}
		break;
		default: break;
	}
}

void UTIL_createAddVip()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_ADDVIP);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, ADD_VIP_WIDTH, ADD_VIP_HEIGHT), ADD_VIP_TITLE, GUI_WINDOW_ADDVIP);
	GUI_Label* newLabel = new GUI_Label(iRect(ADD_VIP_DESCRIPTION_LABEL_X, ADD_VIP_DESCRIPTION_LABEL_Y, 0, 0), ADD_VIP_DESCRIPTION_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_TextBox* newTextBox = new GUI_TextBox(iRect(ADD_VIP_TEXTBOX_X, ADD_VIP_TEXTBOX_Y, ADD_VIP_TEXTBOX_W, ADD_VIP_TEXTBOX_H), "", ADD_VIP_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(32);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_Button* newButton = new GUI_Button(iRect(ADD_VIP_WIDTH - 56, ADD_VIP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&vip_Events, ADD_VIP_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(ADD_VIP_WIDTH - 109, ADD_VIP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&vip_Events, ADD_VIP_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, ADD_VIP_HEIGHT - 40, ADD_VIP_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

void UTIL_createEditVip(VipPlayers& vipPlayer)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_EDITVIP);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	Sint32 windowHeight = EDIT_VIP_HEIGHT;
	if(g_game.hasGameFeature(GAME_FEATURE_VIP_GROUPS) && !g_vipGroups.empty())
		windowHeight += 32 + SDL_static_cast(Sint32, g_vipGroups.size() * 24);

	g_vipIconId = vipPlayer.iconId;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, EDIT_VIP_WIDTH, windowHeight), EDIT_VIP_TITLE, GUI_WINDOW_EDITVIP);
	GUI_Label* newLabel = new GUI_Label(iRect(EDIT_VIP_NAME_LABEL_X, EDIT_VIP_NAME_LABEL_Y, 0, 0), vipPlayer.playerName);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(EDIT_VIP_TYPE_LABEL_X, EDIT_VIP_TYPE_LABEL_Y, 0, 0), EDIT_VIP_TYPE_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(EDIT_VIP_DESCRIPTION_LABEL_X, EDIT_VIP_DESCRIPTION_LABEL_Y, 0, 0), EDIT_VIP_DESCRIPTION_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_MultiTextBox* newTextBox = new GUI_MultiTextBox(iRect(EDIT_VIP_TEXTBOX_X, EDIT_VIP_TEXTBOX_Y, EDIT_VIP_TEXTBOX_W, EDIT_VIP_TEXTBOX_H), true, vipPlayer.description, EDIT_VIP_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(132);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_CheckBox* newCheckBox = new GUI_CheckBox(iRect(EDIT_VIP_NOTIFY_LOGIN_X, EDIT_VIP_NOTIFY_LOGIN_Y, EDIT_VIP_NOTIFY_LOGIN_W, EDIT_VIP_NOTIFY_LOGIN_H), EDIT_VIP_NOTIFY_LOGIN_TEXT, vipPlayer.notifyLogin, EDIT_VIP_NOTIFY_LOGIN_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	for(Uint32 i = 0, end = 10; i <= end; ++i)
	{
		GUI_VipIcon* newVipIcon = new GUI_VipIcon(iRect((SDL_static_cast(Sint32, i) * 21) + 16, 78, GUI_UI_ICON_TEXTURED_UP_W, GUI_UI_ICON_TEXTURED_UP_H), i);
		newVipIcon->startEvents();
		newWindow->addChild(newVipIcon);
	}
	GUI_Button* newButton = new GUI_Button(iRect(EDIT_VIP_WIDTH - 56, windowHeight - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&vip_Events, EDIT_VIP_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(EDIT_VIP_WIDTH - 109, windowHeight - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok");
	newButton->setButtonEventCallback(&vip_Events, EDIT_VIP_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, windowHeight - 40, EDIT_VIP_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	newSeparator = new GUI_Separator(iRect(EDIT_VIP_NAME_SEPARATOR_X, EDIT_VIP_NAME_SEPARATOR_Y, EDIT_VIP_NAME_SEPARATOR_W, EDIT_VIP_NAME_SEPARATOR_H));
	newWindow->addChild(newSeparator);
	if(g_game.hasGameFeature(GAME_FEATURE_VIP_GROUPS) && !g_vipGroups.empty())
	{
		Sint32 groupPosY = 213;
		newSeparator = new GUI_Separator(iRect(16, groupPosY, 253, 2));
		newWindow->addChild(newSeparator);
		groupPosY += 14;
		newLabel = new GUI_Label(iRect(18, groupPosY, 0, 0), "Member of the following groups:");
		newWindow->addChild(newLabel);
		groupPosY += 14;
		for(std::vector<VipGroups>::iterator it = g_vipGroups.begin(), end = g_vipGroups.end(); it != end; ++it)
		{
			VipGroups& vipGroup = (*it);
			std::vector<Uint8>::iterator vit = std::find(vipPlayer.groups.begin(), vipPlayer.groups.end(), vipGroup.groupId);
			newCheckBox = new GUI_CheckBox(iRect(16, groupPosY, 253, 22), vipGroup.groupName, (vit != vipPlayer.groups.end()), EDIT_VIP_GROUP_START_EVENTID + SDL_static_cast(Uint32, vipGroup.groupId));
			newCheckBox->startEvents();
			newWindow->addChild(newCheckBox);
			groupPosY += 24;
		}
	}
	g_engine.addWindow(newWindow);
}

void UTIL_createAddGroup()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_ADDGROUP);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	SDL_snprintf(g_buffer, sizeof(g_buffer), "Add VIP Group(User-Created groups left: %u)", SDL_static_cast(Uint32, g_createGroupsLeft));
	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, ADD_GROUP_WIDTH, ADD_GROUP_HEIGHT), g_buffer, GUI_WINDOW_ADDGROUP);
	GUI_Label* newLabel = new GUI_Label(iRect(ADD_GROUP_DESCRIPTION_LABEL_X, ADD_GROUP_DESCRIPTION_LABEL_Y, 0, 0), ADD_GROUP_DESCRIPTION_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_TextBox* newTextBox = new GUI_TextBox(iRect(ADD_GROUP_TEXTBOX_X, ADD_GROUP_TEXTBOX_Y, ADD_GROUP_TEXTBOX_W, ADD_GROUP_TEXTBOX_H), "", ADD_GROUP_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(32);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_Button* newButton = new GUI_Button(iRect(ADD_GROUP_WIDTH - 56, ADD_GROUP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&vip_Events, ADD_GROUP_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(ADD_GROUP_WIDTH - 109, ADD_GROUP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&vip_Events, ADD_GROUP_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, ADD_GROUP_HEIGHT - 40, ADD_GROUP_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

void UTIL_createEditGroup(VipGroups& vipGroup)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_EDITGROUP);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, EDIT_GROUP_WIDTH, EDIT_GROUP_HEIGHT), EDIT_GROUP_TITLE, GUI_WINDOW_EDITGROUP);
	GUI_Label* newLabel = new GUI_Label(iRect(EDIT_GROUP_DESCRIPTION_LABEL_X, EDIT_GROUP_DESCRIPTION_LABEL_Y, 0, 0), EDIT_GROUP_DESCRIPTION_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_TextBox* newTextBox = new GUI_TextBox(iRect(EDIT_GROUP_TEXTBOX_X, EDIT_GROUP_TEXTBOX_Y, EDIT_GROUP_TEXTBOX_W, EDIT_GROUP_TEXTBOX_H), vipGroup.groupName, EDIT_GROUP_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(32);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_Button* newButton = new GUI_Button(iRect(EDIT_GROUP_WIDTH - 56, EDIT_GROUP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&vip_Events, EDIT_GROUP_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(EDIT_GROUP_WIDTH - 109, EDIT_GROUP_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&vip_Events, EDIT_GROUP_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, EDIT_GROUP_HEIGHT - 40, EDIT_GROUP_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

void UTIL_sortVipWindow()
{
	VipSortMethods sortMethod = g_engine.getBuddySortMethod();
	switch(sortMethod)
	{
		case Vip_Sort_Name: std::sort(g_vipPlayers.begin(), g_vipPlayers.end(), [](VipPlayers& a, VipPlayers& b) -> bool {return a.playerName < b.playerName;}); break;
		case Vip_Sort_Status: std::sort(g_vipPlayers.begin(), g_vipPlayers.end(), [](VipPlayers& a, VipPlayers& b) -> bool {return a.status < b.status;}); break;
		case Vip_Sort_Type: std::sort(g_vipPlayers.begin(), g_vipPlayers.end(), [](VipPlayers& a, VipPlayers& b) -> bool {return a.iconId < b.iconId;}); break;
		default: break;
	}
	g_recreateVipWindow = true;
}

void UTIL_recreateVipWindow(GUI_VipContainer* container)
{
	if(!container)
	{
		GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP);
		if(pPanel)
			container = SDL_static_cast(GUI_VipContainer*, pPanel->getChild(VIP_CONTAINER_EVENTID));
		if(!container)
			return;
	}
	container->clearChilds(false);
	if(g_game.hasGameFeature(GAME_FEATURE_VIP_GROUPS) && !g_engine.getBuddyHideGroups())
	{
		size_t vipPlayers = 0;
		Sint32 PosY = -1;
		for(std::vector<VipGroups>::iterator git = g_vipGroups.begin(), gend = g_vipGroups.end(); git != gend; ++git)
		{
			VipGroups& vipGroup = (*git);
			GUI_VipGroup* newGroup = new GUI_VipGroup(iRect(3, PosY, 150, 14), vipGroup.groupName, vipGroup.groupId);
			newGroup->startEvents();
			container->addChild(newGroup, false);
			GUI_Separator* newSeparator = new GUI_Separator(iRect(2, PosY + 14, 152, 2));
			container->addChild(newSeparator, false);
			PosY += 20;
			for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
			{
				VipPlayers& vipPlayer = (*it);
				if(!g_engine.getBuddyHideOffline() || vipPlayer.status != VIP_STATUS_OFFLINE)
				{
					std::vector<Uint8>::iterator vit = std::find(vipPlayer.groups.begin(), vipPlayer.groups.end(), vipGroup.groupId);
					if(vit != vipPlayer.groups.end())
					{
						GUI_VipPlayer* newPlayer = new GUI_VipPlayer(iRect(2, PosY, 152, 14), vipPlayer.playerName, vipPlayer.playerGUID, vipPlayer.iconId, vipPlayer.status);
						newPlayer->startEvents();
						container->addChild(newPlayer, false);
						PosY += 16;
						++vipPlayers;
					}
				}
			}
		}
		if(vipPlayers != g_vipPlayers.size())
		{
			bool firstDisplay = true;
			for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
			{
				VipPlayers& vipPlayer = (*it);
				if(vipPlayer.groups.empty() && (!g_engine.getBuddyHideOffline() || vipPlayer.status != VIP_STATUS_OFFLINE))
				{
					if(firstDisplay)
					{
						GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(3, PosY, 150, 14), "No Group", 0, 248, 248, 248);
						container->addChild(newLabel, false);
						GUI_Separator* newSeparator = new GUI_Separator(iRect(2, PosY + 14, 152, 2));
						container->addChild(newSeparator, false);
						PosY += 20;
						firstDisplay = false;
					}
					GUI_VipPlayer* newPlayer = new GUI_VipPlayer(iRect(2, PosY, 152, 14), vipPlayer.playerName, vipPlayer.playerGUID, vipPlayer.iconId, vipPlayer.status);
					newPlayer->startEvents();
					container->addChild(newPlayer, false);
					PosY += 16;
				}
			}
		}
	}
	else
	{
		Sint32 PosY = -6;
		for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
		{
			VipPlayers& vipPlayer = (*it);
			if(!g_engine.getBuddyHideOffline() || vipPlayer.status != VIP_STATUS_OFFLINE)
			{
				GUI_VipPlayer* newPlayer = new GUI_VipPlayer(iRect(2, PosY, 152, 14), vipPlayer.playerName, vipPlayer.playerGUID, vipPlayer.iconId, vipPlayer.status);
				newPlayer->startEvents();
				container->addChild(newPlayer, false);
				PosY += 16;
			}
		}
	}
	container->validateScrollBar();
}

void UTIL_toggleVipWindow()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_VIP);
	if(pPanel)
	{
		g_engine.removePanelWindow(pPanel);
		return;
	}

	if(g_needSortVips)
	{
		UTIL_sortVipWindow();
		g_needSortVips = false;
	}
	
	Sint32 savedHeight = g_engine.getContentWindowHeight(GUI_PANEL_WINDOW_VIP);
	if(savedHeight < 38)
		savedHeight = 119;
	GUI_PanelWindow* newWindow = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, savedHeight + 19), true, GUI_PANEL_WINDOW_VIP, true);
	newWindow->setEventCallback(&vip_Events, VIP_RESIZE_WIDTH_EVENTID, VIP_RESIZE_HEIGHT_EVENTID, VIP_EXIT_WINDOW_EVENTID);
	GUI_VipChecker* newVipChecker = new GUI_VipChecker(iRect(19, 2, 0, 0));
	newWindow->addChild(newVipChecker);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(2, 0, GUI_UI_ICON_VIPLIST_W, GUI_UI_ICON_VIPLIST_H), GUI_UI_IMAGE, GUI_UI_ICON_VIPLIST_X, GUI_UI_ICON_VIPLIST_Y);
	newWindow->addChild(newImage);
	GUI_Icon* newIcon = new GUI_Icon(iRect(147, 0, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, VIP_MAXIMINI_EVENTID, "Maximise or minimise window");
	newIcon->setButtonEventCallback(&vip_Events, VIP_MAXIMINI_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(159, 0, GUI_UI_ICON_CLOSE_WINDOW_UP_W, GUI_UI_ICON_CLOSE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_CLOSE_WINDOW_UP_X, GUI_UI_ICON_CLOSE_WINDOW_UP_Y, GUI_UI_ICON_CLOSE_WINDOW_DOWN_X, GUI_UI_ICON_CLOSE_WINDOW_DOWN_Y, 0, "Close this window");
	newIcon->setButtonEventCallback(&vip_Events, VIP_CLOSE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(131, 0, GUI_UI_ICON_SEARCH_WINDOW_UP_W, GUI_UI_ICON_SEARCH_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_SEARCH_WINDOW_UP_X, GUI_UI_ICON_SEARCH_WINDOW_UP_Y, GUI_UI_ICON_SEARCH_WINDOW_DOWN_X, GUI_UI_ICON_SEARCH_WINDOW_DOWN_Y, VIP_CONFIGURE_EVENTID, "Click here to configure the vip window");
	newIcon->setButtonEventCallback(&vip_Events, VIP_CONFIGURE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(19, 2, 100, 14), VIP_TITLE, 0, 144, 144, 144);
	newWindow->addChild(newLabel);
	GUI_VipContainer* newContainer = new GUI_VipContainer(iRect(2, 13, 168, savedHeight), newWindow, VIP_CONTAINER_EVENTID);
	newContainer->startEvents();
	UTIL_recreateVipWindow(newContainer);
	newWindow->addChild(newContainer);

	Sint32 preferredPanel = g_engine.getContentWindowParent(GUI_PANEL_WINDOW_VIP);
	bool added = g_engine.addToPanel(newWindow, preferredPanel);
	if(!added && preferredPanel != GUI_PANEL_RANDOM)
		added = g_engine.addToPanel(newWindow, GUI_PANEL_RANDOM);

	if(added)
	{
		g_recreateVipWindow = false;
		g_haveVIPOpen = true;
	}
	else
	{
		g_recreateVipWindow = true;
		g_haveVIPOpen = false;
		delete newWindow;
	}
}

void UTIL_createVipPopupMenu(Sint32 x, Sint32 y, VipAction& action)
{
	g_lastVipAction = action;

	VipSortMethods sortMethod = g_engine.getBuddySortMethod();
	GUI_ContextMenu* newMenu = new GUI_ContextMenu();
	if(action.type == VIP_ACTION_PLAYER)
	{
		for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
		{
			VipPlayers& vipPlayer = (*it);
			if(vipPlayer.playerGUID == action.player.playerGUID)
			{
				if(!vipPlayer.localOnly)
				{
					SDL_snprintf(g_buffer, sizeof(g_buffer), "Edit %s", vipPlayer.playerName.c_str());
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_EDIT_PLAYER, g_buffer, "");
					SDL_snprintf(g_buffer, sizeof(g_buffer), "Remove %s", vipPlayer.playerName.c_str());
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_REMOVE_PLAYER, g_buffer, "");
				}
				SDL_snprintf(g_buffer, sizeof(g_buffer), "Message to %s", vipPlayer.playerName.c_str());
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_MESSAGE_PLAYER, g_buffer, "");
				break;
			}
		}
	}
	newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD|CONTEXTMENU_STYLE_SEPARATED, VIP_POPUP_ADD_PLAYER, "Add new VIP", "");
	if(action.type == VIP_ACTION_PLAYER && g_chat.getOwnPrivateChannel() != SDL_static_cast(Uint32, -1))
	{
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_INVITE_PLAYER, "Invite to private chat", "");
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_EXCLUDE_PLAYER, "Exclude from private chat", "");
	}
	if(action.type == VIP_ACTION_GROUP)
	{
		for(std::vector<VipGroups>::iterator it = g_vipGroups.begin(), end = g_vipGroups.end(); it != end; ++it)
		{
			VipGroups& vipGroup = (*it);
			if(vipGroup.groupId == action.group.groupId)
			{
				newMenu->addSeparator();
				SDL_snprintf(g_buffer, sizeof(g_buffer), "Edit group %s", vipGroup.groupName.c_str());
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_EDIT_GROUP, g_buffer, "");
				SDL_snprintf(g_buffer, sizeof(g_buffer), "Remove group %s", vipGroup.groupName.c_str());
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_REMOVE_GROUP, g_buffer, "");
				break;
			}
		}
	}
	if(g_game.hasGameFeature(GAME_FEATURE_VIP_GROUPS) && g_createGroupsLeft > 0)
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_ADD_GROUP, "Add new group", "");

	newMenu->addSeparator();
	newMenu->addContextMenu((sortMethod == Vip_Sort_Name ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), VIP_POPUP_SORT_NAME, "Sort by name", "");
	newMenu->addContextMenu((sortMethod == Vip_Sort_Type ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), VIP_POPUP_SORT_TYPE, "Sort by type", "");
	newMenu->addContextMenu((sortMethod == Vip_Sort_Status ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), VIP_POPUP_SORT_STATUS, "Sort by status", "");
	newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_HIDE_PLAYER, (g_engine.getBuddyHideOffline() ? "Show offline VIPs" : "Hide offline VIPs"), "");
	if(g_game.hasGameFeature(GAME_FEATURE_VIP_GROUPS))
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, VIP_POPUP_HIDE_GROUP, (g_engine.getBuddyHideGroups() ? "Show groups" : "Hide groups"), "");

	if(action.type == VIP_ACTION_PLAYER)
	{
		newMenu->addSeparator();
		//newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD|CONTEXTMENU_STYLE_SEPARATED, VIP_POPUP_REPORT_NAME, "Report Name", "");
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD|CONTEXTMENU_STYLE_SEPARATED, VIP_POPUP_COPY_NAME, "Copy Name", "");
		//newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD|CONTEXTMENU_STYLE_SEPARATED, VIP_POPUP_RULE_VIOLATION, "Rule Violation", "");
	}
	newMenu->setEventCallback(&vip_Events);
	g_engine.showContextMenu(newMenu, x, y);
}

static void UTIL_addLocalVipFallbacks()
{
	static const char* localVipNames[] =
	{
		"GM Teddy on Amera",
		"GM Eric on Amera",
		"GM Luna on Amera",
		"Mr. Teddy",
		"Santa Claus on Amera"
	};

	for(size_t i = 0; i < sizeof(localVipNames) / sizeof(localVipNames[0]); ++i)
	{
		g_vips.emplace(localVipNames[i]);
		g_vipPlayers.emplace_back();
		VipPlayers& vipPlayer = g_vipPlayers.back();
		vipPlayer.playerGUID = 0xF0000000u + SDL_static_cast(Uint32, i);
		vipPlayer.playerName = localVipNames[i];
		vipPlayer.description = "Local fallback; server status unavailable.";
		vipPlayer.iconId = 10;
		vipPlayer.notifyLogin = false;
		vipPlayer.status = VIP_STATUS_PENDING;
		vipPlayer.localOnly = true;
	}
}

void UTIL_resetVipPlayers()
{
	g_vips.clear();
	g_vipPlayers.clear();
	UTIL_addLocalVipFallbacks();
	g_needSortVips = true;
	g_recreateVipWindow = true;
}

void UTIL_addVipPlayer(Uint32 playerGUID, const std::string& playerName, const std::string& description, Uint32 iconId, bool notifyLogin, Uint8 status, std::vector<Uint8>& groups)
{
	for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
	{
		VipPlayers& vipPlayer = (*it);
		if(vipPlayer.localOnly && vipPlayer.playerName == playerName)
		{
			vipPlayer.playerGUID = playerGUID;
			vipPlayer.description = description;
			vipPlayer.iconId = iconId;
			vipPlayer.notifyLogin = notifyLogin;
			vipPlayer.status = status;
			vipPlayer.groups = groups;
			vipPlayer.localOnly = false;
			g_needSortVips = true;
			g_recreateVipWindow = true;
			return;
		}
	}

	g_vips.emplace(playerName);
	g_vipPlayers.emplace_back();
	VipPlayers& vipPlayer = g_vipPlayers.back();
	vipPlayer.playerGUID = playerGUID;
	vipPlayer.playerName = playerName;
	vipPlayer.description = description;
	vipPlayer.iconId = iconId;
	vipPlayer.notifyLogin = notifyLogin;
	vipPlayer.status = status;
	vipPlayer.groups = groups;
	g_needSortVips = true;
	g_recreateVipWindow = true;
}

void UTIL_changeVipStatus(Uint32 playerGUID, Uint8 status)
{
	for(std::vector<VipPlayers>::iterator it = g_vipPlayers.begin(), end = g_vipPlayers.end(); it != end; ++it)
	{
		VipPlayers& vipPlayer = (*it);
		if(vipPlayer.playerGUID == playerGUID)
		{
			if(g_engine.getBuddySortMethod() == Vip_Sort_Status)
				g_needSortVips = true;

			vipPlayer.status = status;
			g_recreateVipWindow = true;
			if(vipPlayer.notifyLogin && status != VIP_STATUS_PENDING)
			{
				if(status == VIP_STATUS_ONLINE)
					SDL_snprintf(g_buffer, sizeof(g_buffer), "%s has logged in.", vipPlayer.playerName.c_str());
				else
					SDL_snprintf(g_buffer, sizeof(g_buffer), "%s has logged out", vipPlayer.playerName.c_str());

				g_game.processTextMessage(MessageFailure, g_buffer);
			}
			return;
		}
	}
}

bool UTIL_isVipPlayerOnline(const std::string& playerName)
{
	for(const VipPlayers& vipPlayer : g_vipPlayers)
	{
		if(!vipPlayer.localOnly && vipPlayer.status == VIP_STATUS_ONLINE &&
		   SDL_strcasecmp(vipPlayer.playerName.c_str(), playerName.c_str()) == 0)
			return true;
	}
	return false;
}

void UTIL_changeVipGroups(std::vector<VipGroups>& groups, Uint8 createGroupsLeft)
{
	g_vipGroups = std::move(groups);
	g_createGroupsLeft = createGroupsLeft;
	g_recreateVipWindow = true;
}

bool UTIL_haveVipPlayer(const std::string& name)
{
	return (g_vips.find(name) != g_vips.end());
}

GUI_VipChecker::GUI_VipChecker(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_VipChecker::render()
{
	if(g_needSortVips)
	{
		UTIL_sortVipWindow();
		g_needSortVips = false;
	}
	if(g_recreateVipWindow)
	{
		UTIL_recreateVipWindow(NULL);
		g_recreateVipWindow = false;
	}
}

void GUI_VipContainer::onRMouseDown(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	if(m_scrollBar.getRect().isPointInside(x, y))
	{
		m_scrollBar.onRMouseDown(x, y);
		return;
	}

	g_vipRMouse = true;
	for(std::vector<GUI_Element*>::reverse_iterator it = m_childs.rbegin(), end = m_childs.rend(); it != end; ++it)
	{
		if((*it)->isEventable() && (*it)->getRect().isPointInside(x, y))
		{
			setActiveElement((*it));
			(*it)->onRMouseDown(x, y);
			return;
		}
	}
}

void GUI_VipContainer::onRMouseUp(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	m_scrollBar.onRMouseUp(x, y);
	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		if((*it)->isEventable())
			(*it)->onRMouseUp(x, y);
	}

	if(g_vipRMouse && isInsideRect(x, y))
	{
		VipAction action;
		action.type = VIP_ACTION_NONE;
		UTIL_createVipPopupMenu(x, y, action);
	}

	g_vipRMouse = false;
}

GUI_VipPlayer::GUI_VipPlayer(iRect boxRect, const std::string& name, Uint32 _GUID, Uint32 iconId, Uint8 status, Uint32 internalID) :
	m_name(iRect(0, 0, boxRect.x2 - 20, boxRect.y2), name), m_GUID(_GUID), m_iconId(iconId)
{
	Uint8 red, green, blue;
	if(status == VIP_STATUS_ONLINE)
	{
		red = 96;
		green = 248;
		blue = 96;
	}
	else if(status == VIP_STATUS_PENDING)
	{
		red = 255;
		green = 152;
		blue = 84;
	}
	else
	{
		red = 248;
		green = 96;
		blue = 96;
	}

	m_internalID = internalID;
	m_name.setColor(red, green, blue);
	setRect(boxRect);
}

void GUI_VipPlayer::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(!m_name.isFullDisplay())
	{
		bool inside = (isInsideParent && m_tRect.isPointInside(x, y));
		if(inside)
			g_engine.showDescription(x, y, m_name.getName());
	}
	if(g_vipRMouse)
		m_hover = (isInsideParent && m_tRect.isPointInside(x, y));
}

void GUI_VipPlayer::onLMouseDown(Sint32, Sint32)
{
	if(g_frameTime < m_lastClick)
	{
		g_game.sendOpenPrivateChannel(m_name.getName());
		m_lastClick = 0;
		return;
	}

	m_lastClick = g_frameTime + 1000;
}

void GUI_VipPlayer::onRMouseDown(Sint32, Sint32)
{
	m_hover = true;
}

void GUI_VipPlayer::onRMouseUp(Sint32 x, Sint32 y)
{
	if(g_vipRMouse && m_hover)
	{
		VipAction action;
		action.player.type = VIP_ACTION_PLAYER;
		action.player.playerGUID = m_GUID;
		UTIL_createVipPopupMenu(x, y, action);
		g_vipRMouse = false;
	}
}

void GUI_VipPlayer::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	iRect nRect = iRect(NewRect.x1 + 17, NewRect.y1 + 3, NewRect.x2 - 20, NewRect.y2);
	m_name.setRect(nRect);
	m_tRect = NewRect;
}

void GUI_VipPlayer::render()
{
	GUI_VipIcon::renderIcon(m_iconId, m_tRect.x1, m_tRect.y1 + 1);
	m_name.render();
}

GUI_VipGroup::GUI_VipGroup(iRect boxRect, const std::string& name, Uint8 groupId, Uint32 internalID) :
	m_name(iRect(0, 0, boxRect.x2, boxRect.y2), name, 0, 255, 255, 255), m_groupId(groupId)
{
	m_internalID = internalID;
	setRect(boxRect);
}

void GUI_VipGroup::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(!m_name.isFullDisplay())
	{
		bool inside = (isInsideParent && m_tRect.isPointInside(x, y));
		if(inside)
			g_engine.showDescription(x, y, m_name.getName());
	}
	if(g_vipRMouse)
		m_hover = (isInsideParent && m_tRect.isPointInside(x, y));
}

void GUI_VipGroup::onRMouseDown(Sint32, Sint32)
{
	m_hover = true;
}

void GUI_VipGroup::onRMouseUp(Sint32 x, Sint32 y)
{
	if(g_vipRMouse && m_hover)
	{
		VipAction action;
		action.group.type = VIP_ACTION_GROUP;
		action.group.groupId = m_groupId;
		UTIL_createVipPopupMenu(x, y, action);
		g_vipRMouse = false;
	}
}

void GUI_VipGroup::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	m_name.setRect(NewRect);
	m_tRect = NewRect;
}

void GUI_VipGroup::render()
{
	m_name.render();
}

GUI_VipIcon::GUI_VipIcon(iRect boxRect, Uint32 iconId, Uint32 internalID) : m_iconId(iconId)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_VipIcon::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(m_pressed > 0)
	{
		bool inside = (isInsideParent && m_tRect.isPointInside(x, y));
		if(m_pressed == 1 && !inside)
			m_pressed = 2;
		else if(m_pressed == 2 && inside)
			m_pressed = 1;
	}
}

void GUI_VipIcon::onLMouseDown(Sint32, Sint32)
{
	m_pressed = 1;
}

void GUI_VipIcon::onLMouseUp(Sint32, Sint32)
{
	Uint8 pressed = m_pressed;
	if(pressed > 0)
	{
		m_pressed = 0;
		if(pressed == 1)
			g_vipIconId = m_iconId;
	}
}

void GUI_VipIcon::renderIcon(Uint32 iconId, Sint32 x, Sint32 y)
{
	auto& renderer = g_engine.getRender();
	switch(iconId)
	{
		case 1: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_001_X, GUI_UI_VIP_001_Y, x, y, GUI_UI_VIP_001_W, GUI_UI_VIP_001_H); break;
		case 2: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_002_X, GUI_UI_VIP_002_Y, x, y, GUI_UI_VIP_002_W, GUI_UI_VIP_002_H); break;
		case 3: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_003_X, GUI_UI_VIP_003_Y, x, y, GUI_UI_VIP_003_W, GUI_UI_VIP_003_H); break;
		case 4: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_004_X, GUI_UI_VIP_004_Y, x, y, GUI_UI_VIP_004_W, GUI_UI_VIP_004_H); break;
		case 5: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_005_X, GUI_UI_VIP_005_Y, x, y, GUI_UI_VIP_005_W, GUI_UI_VIP_005_H); break;
		case 6: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_006_X, GUI_UI_VIP_006_Y, x, y, GUI_UI_VIP_006_W, GUI_UI_VIP_006_H); break;
		case 7: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_007_X, GUI_UI_VIP_007_Y, x, y, GUI_UI_VIP_007_W, GUI_UI_VIP_007_H); break;
		case 8: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_008_X, GUI_UI_VIP_008_Y, x, y, GUI_UI_VIP_008_W, GUI_UI_VIP_008_H); break;
		case 9: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_009_X, GUI_UI_VIP_009_Y, x, y, GUI_UI_VIP_009_W, GUI_UI_VIP_009_H); break;
		case 10: renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_VIP_010_X, GUI_UI_VIP_010_Y, x, y, GUI_UI_VIP_010_W, GUI_UI_VIP_010_H); break;
		default: break;
	}
}

void GUI_VipIcon::render()
{
	bool pressed = (m_pressed == 1 ? true : false);
	pressed = (m_iconId == g_vipIconId ? true : pressed);

	auto& renderer = g_engine.getRender();
	if(pressed)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_TEXTURED_DOWN_X, GUI_UI_ICON_TEXTURED_DOWN_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
		GUI_VipIcon::renderIcon(m_iconId, m_tRect.x1 + 3, m_tRect.y1 + 3);
	}
	else
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_TEXTURED_UP_X, GUI_UI_ICON_TEXTURED_UP_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
		GUI_VipIcon::renderIcon(m_iconId, m_tRect.x1 + 2, m_tRect.y1 + 2);
	}
}
