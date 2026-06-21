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
#include "../GUI_Elements/GUI_Panel.h"
#include "../GUI_Elements/GUI_PanelWindow.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_StaticImage.h"
#include "../map.h"
#include "../game.h"
#include "../creature.h"
#include "Inventory.h"
#include "itemUI.h"

#define INVENTORY_OPTIONS_TITLE "Options"
#define INVENTORY_OPTIONS_DESCRIPTION "Open options"
#define INVENTORY_OPTIONS_EVENTID 1000
#define INVENTORY_HELP_TITLE "Help"
#define INVENTORY_HELP_DESCRIPTION "Open help menu"
#define INVENTORY_HELP_EVENTID 1001
#define INVENTORY_STOP_TITLE "Stop"
#define INVENTORY_STOP_DESCRIPTION "Stop current action"
#define INVENTORY_STOP_EVENTID 1002
#define INVENTORY_QUESTS_TITLE "Quests"
#define INVENTORY_QUESTS_DESCRIPTION "Open quest log"
#define INVENTORY_QUESTS_EVENTID 1003
#define INVENTORY_OFFENSIVE_DESCRIPTION "Offensive: Increases your attack strength but reduces\nyour defense. Your magical damage is not affected."
#define INVENTORY_OFFENSIVE_EVENTID 1004
#define INVENTORY_BALANCED_DESCRIPTION "Balanced: Balances out your attack strength as well as your\ndefense strength. Your magical damage is not affected."
#define INVENTORY_BALANCED_EVENTID 1005
#define INVENTORY_DEFENSIVE_DESCRIPTION "Defensive: Increases your defense but reduces your\nattack strength. Your magical damage is not affected."
#define INVENTORY_DEFENSIVE_EVENTID 1006
#define INVENTORY_STAND_DESCRIPTION "Stand while fighting"
#define INVENTORY_STAND_EVENTID 1007
#define INVENTORY_FOLLOW_DESCRIPTION "Chase opponent"
#define INVENTORY_FOLLOW_EVENTID 1008
#define INVENTORY_PVP_DESCRIPTION1 "Secure Mode Off: You are able to attack someone by targeting,\nregardless of your expert mode. You risk white, red and black\nskulls as well as a protection zone block."
#define INVENTORY_PVP_DESCRIPTION2 "Secure Mode On: You are able to attack only those players\nyour expert mode allows. You risk skulls and protection zone\nblocks depending on your active expert mode."
#define INVENTORY_PVP_EVENTID 1009
#define INVENTORY_DOVE_DESCRIPTION "Dove Mode: You are only able to defend yourself against\naggressors. You can't get a skull or a protection zone block."
#define INVENTORY_DOVE_EVENTID 1010
#define INVENTORY_WHITEHAND_DESCRIPTION "White Hand Mode: You are able to defend yourself and your\ngroup against aggressors. You risk getting a yellow skull."
#define INVENTORY_WHITEHAND_EVENTID 1011
#define INVENTORY_YELLOWHAND_DESCRIPTION "Yellow Hand Mode: You are able to attack any skulled player.\nYou risk getting a yellow skull and a protection zone block."
#define INVENTORY_YELLOWHAND_EVENTID 1012
#define INVENTORY_REDFIST_DESCRIPTION "Red Fist Mode: You are able to attack and block anyone. You risk\nwhite, red and black skulls as well as a protection zone block."
#define INVENTORY_REDFIST_EVENTID 1013
#define INVENTORY_SOUL_DESCRIPTION "Soul Points: Necessary for supply spells, creating runes\nand enchanting. You can regain them by killing monsters\nor sleeping in a bed."
#define INVENTORY_SOUL_EVENTID 1014
#define INVENTORY_CAP_DESCRIPTION "Capacity: The amount of weight you are able to carry."
#define INVENTORY_CAP_EVENTID 1015
#define INVENTORY_MINIMIZE_DESCRIPTION "Minimise inventory"
#define INVENTORY_MINIMIZE_EVENTID 1016
#define INVENTORY_MAXIMIZE_DESCRIPTION "Maximise inventory"
#define INVENTORY_MAXIMIZE_EVENTID 1017

extern Engine g_engine;
extern Map g_map;
extern Game g_game;

bool CheckOffensiveIcon() {return g_engine.getAttackMode() == ATTACKMODE_ATTACK;}
bool CheckBalancedIcon() {return g_engine.getAttackMode() == ATTACKMODE_BALANCED;}
bool CheckDefensiveIcon() {return g_engine.getAttackMode() == ATTACKMODE_DEFENSE;}
bool CheckStandIcon() {return g_engine.getChaseMode() == CHASEMODE_STAND;}
bool CheckFollowIcon() {return g_engine.getChaseMode() == CHASEMODE_FOLLOW;}
bool CheckPvPIcon() {return g_engine.getSecureMode() == SECUREMODE_SECURE;}
bool CheckDoveIcon() {return g_engine.getPvpMode() == PVPMODE_DOVE;}
bool CheckWhiteHandIcon() {return g_engine.getPvpMode() == PVPMODE_WHITE_HAND;}
bool CheckYellowHandIcon() {return g_engine.getPvpMode() == PVPMODE_YELLOW_HAND;}
bool CheckRedFistIcon() {return g_engine.getPvpMode() == PVPMODE_RED_FIST;}

void inventory_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case INVENTORY_OPTIONS_EVENTID: UTIL_options(); break;
		case INVENTORY_HELP_EVENTID: UTIL_help(); break;
		case INVENTORY_STOP_EVENTID: g_game.stopActions(); break;
		case INVENTORY_QUESTS_EVENTID: g_game.sendOpenQuestLog(); break;
		case INVENTORY_OFFENSIVE_EVENTID: {g_engine.setAttackMode(ATTACKMODE_ATTACK); g_game.sendAttackModes();} break;
		case INVENTORY_BALANCED_EVENTID: {g_engine.setAttackMode(ATTACKMODE_BALANCED); g_game.sendAttackModes();} break;
		case INVENTORY_DEFENSIVE_EVENTID: {g_engine.setAttackMode(ATTACKMODE_DEFENSE); g_game.sendAttackModes();} break;
		case INVENTORY_STAND_EVENTID: {g_engine.setChaseMode(CHASEMODE_STAND); g_game.sendAttackModes();} break;
		case INVENTORY_FOLLOW_EVENTID: {g_engine.setChaseMode(CHASEMODE_FOLLOW); g_game.sendAttackModes();} break;
		case INVENTORY_PVP_EVENTID: {g_engine.setSecureMode((g_engine.getSecureMode() == SECUREMODE_SECURE ? SECUREMODE_UNSECURE : SECUREMODE_SECURE)); g_game.sendAttackModes();} break;
		case INVENTORY_DOVE_EVENTID: {g_engine.setPvpMode(PVPMODE_DOVE); g_game.sendAttackModes();} break;
		case INVENTORY_WHITEHAND_EVENTID: {g_engine.setPvpMode(PVPMODE_WHITE_HAND); g_game.sendAttackModes();} break;
		case INVENTORY_YELLOWHAND_EVENTID: {g_engine.setPvpMode(PVPMODE_YELLOW_HAND); g_game.sendAttackModes();} break;
		case INVENTORY_REDFIST_EVENTID: {g_engine.setPvpMode(PVPMODE_RED_FIST); g_game.sendAttackModes();} break;
		case INVENTORY_MINIMIZE_EVENTID: {UTIL_createInventoryPanel(true); g_engine.recalculateGameWindow();} break;
		case INVENTORY_MAXIMIZE_EVENTID: {UTIL_createInventoryPanel(false); g_engine.recalculateGameWindow();} break;
		default: break;
	}
}

void UTIL_createInventoryPanel(bool minimized)
{
	bool newWindow;
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_INVENTORY);
	if(!pPanel)
		pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_INVENTORY_MINIMIZED);

	if(pPanel)
	{
		newWindow = false;
		pPanel->clearChilds();
		if(minimized)
		{
			iRect rect = pPanel->getRect(); rect.y2 = 64;
			pPanel->setRect(rect, true);
			pPanel->setInternalID(GUI_PANEL_WINDOW_INVENTORY_MINIMIZED);
		}
		else
		{
			iRect rect = pPanel->getRect(); rect.y2 = 170;
			pPanel->setRect(rect, true);
			pPanel->setInternalID(GUI_PANEL_WINDOW_INVENTORY);
		}
	}
	else
	{
		newWindow = true;
		if(minimized)
			pPanel = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, 64), false, GUI_PANEL_WINDOW_INVENTORY_MINIMIZED);
		else
			pPanel = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, 170), false, GUI_PANEL_WINDOW_INVENTORY);
	}

	if(minimized)
	{
		if(g_clientVersion >= 1000)
		{
			if(g_game.getExpertPvpMode())
			{
				GUI_RadioIcon* newRadioIcon = new GUI_RadioIcon(iRect(124, 4, GUI_UI_ICON_COMBAT_DOVE_UP_W, GUI_UI_ICON_COMBAT_DOVE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_DOVE_UP_X, GUI_UI_ICON_COMBAT_DOVE_UP_Y, GUI_UI_ICON_COMBAT_DOVE_DOWN_X, GUI_UI_ICON_COMBAT_DOVE_DOWN_Y, 0, INVENTORY_DOVE_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckDoveIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_DOVE_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
				newRadioIcon = new GUI_RadioIcon(iRect(147, 4, GUI_UI_ICON_COMBAT_WHITEHAND_UP_W, GUI_UI_ICON_COMBAT_WHITEHAND_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_WHITEHAND_UP_X, GUI_UI_ICON_COMBAT_WHITEHAND_UP_Y, GUI_UI_ICON_COMBAT_WHITEHAND_DOWN_X, GUI_UI_ICON_COMBAT_WHITEHAND_DOWN_Y, 0, INVENTORY_WHITEHAND_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckWhiteHandIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_WHITEHAND_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
				newRadioIcon = new GUI_RadioIcon(iRect(124, 26, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_W, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_X, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_Y, GUI_UI_ICON_COMBAT_YELLOWHAND_DOWN_X, GUI_UI_ICON_COMBAT_YELLOWHAND_DOWN_Y, 0, INVENTORY_YELLOWHAND_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckYellowHandIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_YELLOWHAND_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
				newRadioIcon = new GUI_RadioIcon(iRect(147, 26, GUI_UI_ICON_COMBAT_REDFIST_UP_W, GUI_UI_ICON_COMBAT_REDFIST_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_REDFIST_UP_X, GUI_UI_ICON_COMBAT_REDFIST_UP_Y, GUI_UI_ICON_COMBAT_REDFIST_DOWN_X, GUI_UI_ICON_COMBAT_REDFIST_DOWN_Y, 0, INVENTORY_REDFIST_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckRedFistIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_REDFIST_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
			}
			else
			{
				GUI_StaticImage* newImage = new GUI_StaticImage(iRect(124, 4, GUI_UI_ICON_COMBAT_DOVE_DISABLED_W, GUI_UI_ICON_COMBAT_DOVE_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_DOVE_DISABLED_X, GUI_UI_ICON_COMBAT_DOVE_DISABLED_Y, 0, INVENTORY_DOVE_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
				newImage = new GUI_StaticImage(iRect(147, 4, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_W, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_X, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_Y, 0, INVENTORY_WHITEHAND_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
				newImage = new GUI_StaticImage(iRect(124, 26, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_W, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_X, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_Y, 0, INVENTORY_YELLOWHAND_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
				newImage = new GUI_StaticImage(iRect(147, 26, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_W, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_X, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_Y, 0, INVENTORY_REDFIST_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
			}
		}
		else
		{
			GUI_Button* newButton = new GUI_Button(iRect(124, 4, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_STOP_TITLE, 0, INVENTORY_STOP_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_STOP_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
			GUI_Icon* newIcon = new GUI_Icon(iRect(124, 26, GUI_UI_ICON_OPTIONS_LEGACY_UP_W, GUI_UI_ICON_OPTIONS_LEGACY_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_OPTIONS_LEGACY_UP_X, GUI_UI_ICON_OPTIONS_LEGACY_UP_Y, GUI_UI_ICON_OPTIONS_LEGACY_DOWN_X, GUI_UI_ICON_OPTIONS_LEGACY_DOWN_Y, 0, INVENTORY_OPTIONS_DESCRIPTION);
			newIcon->setButtonEventCallback(&inventory_Events, INVENTORY_OPTIONS_EVENTID);
			newIcon->startEvents();
			pPanel->addChild(newIcon);
			newIcon = new GUI_Icon(iRect(147, 26, GUI_UI_ICON_HELP_UP_W, GUI_UI_ICON_HELP_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_HELP_UP_X, GUI_UI_ICON_HELP_UP_Y, GUI_UI_ICON_HELP_DOWN_X, GUI_UI_ICON_HELP_DOWN_Y, 0, INVENTORY_HELP_DESCRIPTION);
			newIcon->setButtonEventCallback(&inventory_Events, INVENTORY_HELP_EVENTID);
			newIcon->startEvents();
			pPanel->addChild(newIcon);
		}
		GUI_RadioIcon* newRadioIcon = new GUI_RadioIcon(iRect(56, 4, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_W, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_X, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_Y, GUI_UI_ICON_COMBAT_OFFENSIVE_DOWN_X, GUI_UI_ICON_COMBAT_OFFENSIVE_DOWN_Y, 0, INVENTORY_OFFENSIVE_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckOffensiveIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_OFFENSIVE_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(76, 4, GUI_UI_ICON_COMBAT_BALANCED_UP_W, GUI_UI_ICON_COMBAT_BALANCED_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_BALANCED_UP_X, GUI_UI_ICON_COMBAT_BALANCED_UP_Y, GUI_UI_ICON_COMBAT_BALANCED_DOWN_X, GUI_UI_ICON_COMBAT_BALANCED_DOWN_Y, 0, INVENTORY_BALANCED_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckBalancedIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_BALANCED_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(96, 4, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_W, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_X, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_Y, GUI_UI_ICON_COMBAT_DEFENSIVE_DOWN_X, GUI_UI_ICON_COMBAT_DEFENSIVE_DOWN_Y, 0, INVENTORY_DEFENSIVE_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckDefensiveIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_DEFENSIVE_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(56, 26, GUI_UI_ICON_STAND_UP_W, GUI_UI_ICON_STAND_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_STAND_UP_X, GUI_UI_ICON_STAND_UP_Y, GUI_UI_ICON_STAND_DOWN_X, GUI_UI_ICON_STAND_DOWN_Y, 0, INVENTORY_STAND_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckStandIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_STAND_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(76, 26, GUI_UI_ICON_FOLLOW_UP_W, GUI_UI_ICON_FOLLOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_FOLLOW_UP_X, GUI_UI_ICON_FOLLOW_UP_Y, GUI_UI_ICON_FOLLOW_DOWN_X, GUI_UI_ICON_FOLLOW_DOWN_Y, 0, INVENTORY_FOLLOW_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckFollowIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_FOLLOW_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(96, 26, GUI_UI_ICON_EXPERTPVP_UP_W, GUI_UI_ICON_EXPERTPVP_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_EXPERTPVP_UP_X, GUI_UI_ICON_EXPERTPVP_UP_Y, GUI_UI_ICON_EXPERTPVP_DOWN_X, GUI_UI_ICON_EXPERTPVP_DOWN_Y, 0, INVENTORY_PVP_DESCRIPTION1);
		newRadioIcon->setRadioEventCallback(&CheckPvPIcon, INVENTORY_PVP_DESCRIPTION2);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_PVP_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		GUI_Icon* newIcon = new GUI_RadioIcon(iRect(8, 4, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_W, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_X, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_Y, 0, INVENTORY_MAXIMIZE_DESCRIPTION);
		newIcon->setButtonEventCallback(&inventory_Events, INVENTORY_MAXIMIZE_EVENTID);
		newIcon->startEvents();
		pPanel->addChild(newIcon);
		GUI_StaticImage* newImage = new GUI_StaticImage(iRect(22, 4, GUI_UI_STATUS_BACKGROUND_BIG_W, GUI_UI_STATUS_BACKGROUND_H), GUI_UI_IMAGE, GUI_UI_STATUS_BACKGROUND_BIG_X, GUI_UI_STATUS_BACKGROUND_BIG_Y, 0, INVENTORY_SOUL_DESCRIPTION);
		pPanel->addChild(newImage);
		newImage = new GUI_StaticImage(iRect(22, 25, GUI_UI_STATUS_BACKGROUND_BIG_W, GUI_UI_STATUS_BACKGROUND_H), GUI_UI_IMAGE, GUI_UI_STATUS_BACKGROUND_BIG_X, GUI_UI_STATUS_BACKGROUND_BIG_Y + GUI_UI_STATUS_BACKGROUND_H, 0, INVENTORY_CAP_DESCRIPTION);
		pPanel->addChild(newImage);
		GUI_Icons* newIcons = new GUI_Icons(iRect(15, 48, GUI_UI_ICON_STATUS_BAR_W, GUI_UI_ICON_STATUS_BAR_H));
		pPanel->addChild(newIcons);
		GUI_Label* newLabel = new GUI_Label(iRect(38, 6, 0, 0), "Soul:", 0, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, g_game.getPlayerSoul()));
		newLabel = new GUI_Label(iRect(38, 15, 0, 0), UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len))), INVENTORY_SOUL_EVENTID, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
		newLabel = new GUI_Label(iRect(38, 27, 0, 0), "Cap:", 0, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, g_game.getPlayerCapacity()));
		newLabel = new GUI_Label(iRect(38, 36, 0, 0), UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len))), INVENTORY_CAP_EVENTID, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
	}
	else
	{
		if(g_clientVersion >= 1000)
		{
			GUI_Button* newButton = new GUI_Button(iRect(124, 149, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_STOP_TITLE, 0, INVENTORY_STOP_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_STOP_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
			if(g_game.getExpertPvpMode())
			{
				GUI_RadioIcon* newRadioIcon = new GUI_RadioIcon(iRect(124, 94, GUI_UI_ICON_COMBAT_DOVE_UP_W, GUI_UI_ICON_COMBAT_DOVE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_DOVE_UP_X, GUI_UI_ICON_COMBAT_DOVE_UP_Y, GUI_UI_ICON_COMBAT_DOVE_DOWN_X, GUI_UI_ICON_COMBAT_DOVE_DOWN_Y, 0, INVENTORY_DOVE_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckDoveIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_DOVE_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
				newRadioIcon = new GUI_RadioIcon(iRect(147, 94, GUI_UI_ICON_COMBAT_WHITEHAND_UP_W, GUI_UI_ICON_COMBAT_WHITEHAND_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_WHITEHAND_UP_X, GUI_UI_ICON_COMBAT_WHITEHAND_UP_Y, GUI_UI_ICON_COMBAT_WHITEHAND_DOWN_X, GUI_UI_ICON_COMBAT_WHITEHAND_DOWN_Y, 0, INVENTORY_WHITEHAND_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckWhiteHandIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_WHITEHAND_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
				newRadioIcon = new GUI_RadioIcon(iRect(124, 114, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_W, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_X, GUI_UI_ICON_COMBAT_YELLOWHAND_UP_Y, GUI_UI_ICON_COMBAT_YELLOWHAND_DOWN_X, GUI_UI_ICON_COMBAT_YELLOWHAND_DOWN_Y, 0, INVENTORY_YELLOWHAND_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckYellowHandIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_YELLOWHAND_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
				newRadioIcon = new GUI_RadioIcon(iRect(147, 114, GUI_UI_ICON_COMBAT_REDFIST_UP_W, GUI_UI_ICON_COMBAT_REDFIST_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_REDFIST_UP_X, GUI_UI_ICON_COMBAT_REDFIST_UP_Y, GUI_UI_ICON_COMBAT_REDFIST_DOWN_X, GUI_UI_ICON_COMBAT_REDFIST_DOWN_Y, 0, INVENTORY_REDFIST_DESCRIPTION);
				newRadioIcon->setRadioEventCallback(&CheckRedFistIcon);
				newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_REDFIST_EVENTID);
				newRadioIcon->startEvents();
				pPanel->addChild(newRadioIcon);
			}
			else
			{
				GUI_StaticImage* newImage = new GUI_StaticImage(iRect(124, 94, GUI_UI_ICON_COMBAT_DOVE_DISABLED_W, GUI_UI_ICON_COMBAT_DOVE_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_DOVE_DISABLED_X, GUI_UI_ICON_COMBAT_DOVE_DISABLED_Y, 0, INVENTORY_DOVE_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
				newImage = new GUI_StaticImage(iRect(147, 94, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_W, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_X, GUI_UI_ICON_COMBAT_WHITEHAND_DISABLED_Y, 0, INVENTORY_WHITEHAND_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
				newImage = new GUI_StaticImage(iRect(124, 114, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_W, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_X, GUI_UI_ICON_COMBAT_YELLOWHAND_DISABLED_Y, 0, INVENTORY_YELLOWHAND_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
				newImage = new GUI_StaticImage(iRect(147, 114, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_W, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_X, GUI_UI_ICON_COMBAT_REDFIST_DISABLED_Y, 0, INVENTORY_REDFIST_DESCRIPTION);
				newImage->startEvents();
				pPanel->addChild(newImage);
			}
		}
		else if(g_clientVersion >= 790)
		{
			GUI_Button* newButton = new GUI_Button(iRect(124, 81, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_STOP_TITLE, 0, INVENTORY_STOP_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_STOP_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
			newButton = new GUI_Button(iRect(124, 105, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_QUESTS_TITLE, 0, INVENTORY_QUESTS_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_QUESTS_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
			newButton = new GUI_Button(iRect(124, 127, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_OPTIONS_TITLE, 0, INVENTORY_OPTIONS_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_OPTIONS_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
			newButton = new GUI_Button(iRect(124, 149, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_HELP_TITLE, 0, INVENTORY_HELP_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_HELP_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
		}
		else
		{
			GUI_Button* newButton = new GUI_Button(iRect(124, 81, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_STOP_TITLE, 0, INVENTORY_STOP_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_STOP_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
			newButton = new GUI_Button(iRect(124, 105, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_OPTIONS_TITLE, 0, INVENTORY_OPTIONS_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_OPTIONS_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
			newButton = new GUI_Button(iRect(124, 127, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), INVENTORY_HELP_TITLE, 0, INVENTORY_HELP_DESCRIPTION);
			newButton->setButtonEventCallback(&inventory_Events, INVENTORY_HELP_EVENTID);
			newButton->startEvents();
			pPanel->addChild(newButton);
		}
		GUI_RadioIcon* newRadioIcon = new GUI_RadioIcon(iRect(124, 19, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_W, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_X, GUI_UI_ICON_COMBAT_OFFENSIVE_UP_Y, GUI_UI_ICON_COMBAT_OFFENSIVE_DOWN_X, GUI_UI_ICON_COMBAT_OFFENSIVE_DOWN_Y, 0, INVENTORY_OFFENSIVE_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckOffensiveIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_OFFENSIVE_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(124, 39, GUI_UI_ICON_COMBAT_BALANCED_UP_W, GUI_UI_ICON_COMBAT_BALANCED_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_BALANCED_UP_X, GUI_UI_ICON_COMBAT_BALANCED_UP_Y, GUI_UI_ICON_COMBAT_BALANCED_DOWN_X, GUI_UI_ICON_COMBAT_BALANCED_DOWN_Y, 0, INVENTORY_BALANCED_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckBalancedIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_BALANCED_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(124, 59, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_W, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_X, GUI_UI_ICON_COMBAT_DEFENSIVE_UP_Y, GUI_UI_ICON_COMBAT_DEFENSIVE_DOWN_X, GUI_UI_ICON_COMBAT_DEFENSIVE_DOWN_Y, 0, INVENTORY_DEFENSIVE_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckDefensiveIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_DEFENSIVE_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(147, 19, GUI_UI_ICON_STAND_UP_W, GUI_UI_ICON_STAND_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_STAND_UP_X, GUI_UI_ICON_STAND_UP_Y, GUI_UI_ICON_STAND_DOWN_X, GUI_UI_ICON_STAND_DOWN_Y, 0, INVENTORY_STAND_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckStandIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_STAND_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(147, 39, GUI_UI_ICON_FOLLOW_UP_W, GUI_UI_ICON_FOLLOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_FOLLOW_UP_X, GUI_UI_ICON_FOLLOW_UP_Y, GUI_UI_ICON_FOLLOW_DOWN_X, GUI_UI_ICON_FOLLOW_DOWN_Y, 0, INVENTORY_FOLLOW_DESCRIPTION);
		newRadioIcon->setRadioEventCallback(&CheckFollowIcon);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_FOLLOW_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		newRadioIcon = new GUI_RadioIcon(iRect(147, 59, GUI_UI_ICON_EXPERTPVP_UP_W, GUI_UI_ICON_EXPERTPVP_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_EXPERTPVP_UP_X, GUI_UI_ICON_EXPERTPVP_UP_Y, GUI_UI_ICON_EXPERTPVP_DOWN_X, GUI_UI_ICON_EXPERTPVP_DOWN_Y, 0, INVENTORY_PVP_DESCRIPTION1);
		newRadioIcon->setRadioEventCallback(&CheckPvPIcon, INVENTORY_PVP_DESCRIPTION2);
		newRadioIcon->setButtonEventCallback(&inventory_Events, INVENTORY_PVP_EVENTID);
		newRadioIcon->startEvents();
		pPanel->addChild(newRadioIcon);
		GUI_Icon* newIcon = new GUI_RadioIcon(iRect(8, 4, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, 0, INVENTORY_MINIMIZE_DESCRIPTION);
		newIcon->setButtonEventCallback(&inventory_Events, INVENTORY_MINIMIZE_EVENTID);
		newIcon->startEvents();
		pPanel->addChild(newIcon);
		GUI_StaticImage* newImage = new GUI_StaticImage(iRect(8, 128, GUI_UI_STATUS_BACKGROUND_W, GUI_UI_STATUS_BACKGROUND_H), GUI_UI_IMAGE, GUI_UI_STATUS_BACKGROUND_X, GUI_UI_STATUS_BACKGROUND_Y, 0, INVENTORY_SOUL_DESCRIPTION);
		pPanel->addChild(newImage);
		newImage = new GUI_StaticImage(iRect(82, 128, GUI_UI_STATUS_BACKGROUND_W, GUI_UI_STATUS_BACKGROUND_H), GUI_UI_IMAGE, GUI_UI_STATUS_BACKGROUND_X, GUI_UI_STATUS_BACKGROUND_Y, 0, INVENTORY_CAP_DESCRIPTION);
		pPanel->addChild(newImage);
		GUI_InventoryItem* newInventoryItem = new GUI_InventoryItem(iRect(46, 5, 32, 32), GUI_UI_INVENTORY_HEAD_X, GUI_UI_INVENTORY_HEAD_Y, SLOT_HEAD);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(9, 19, 32, 32), GUI_UI_INVENTORY_NECKLACE_X, GUI_UI_INVENTORY_NECKLACE_Y, SLOT_NECKLACE);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(83, 19, 32, 32), GUI_UI_INVENTORY_BACKPACK_X, GUI_UI_INVENTORY_BACKPACK_Y, SLOT_BACKPACK);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(46, 42, 32, 32), GUI_UI_INVENTORY_ARMOR_X, GUI_UI_INVENTORY_ARMOR_Y, SLOT_ARMOR);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(9, 56, 32, 32), GUI_UI_INVENTORY_LEFT_X, GUI_UI_INVENTORY_LEFT_Y, SLOT_LEFT);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(83, 56, 32, 32), GUI_UI_INVENTORY_RIGHT_X, GUI_UI_INVENTORY_RIGHT_Y, SLOT_RIGHT);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(46, 79, 32, 32), GUI_UI_INVENTORY_LEGS_X, GUI_UI_INVENTORY_LEGS_Y, SLOT_LEGS);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(46, 116, 32, 32), GUI_UI_INVENTORY_FEET_X, GUI_UI_INVENTORY_FEET_Y, SLOT_FEET);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(9, 93, 32, 32), GUI_UI_INVENTORY_RING_X, GUI_UI_INVENTORY_RING_Y, SLOT_RING);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		newInventoryItem = new GUI_InventoryItem(iRect(83, 93, 32, 32), GUI_UI_INVENTORY_AMMO_X, GUI_UI_INVENTORY_AMMO_Y, SLOT_AMMO);
		newInventoryItem->startEvents();
		pPanel->addChild(newInventoryItem);
		GUI_Icons* newIcons = new GUI_Icons(iRect(8, 151, GUI_UI_ICON_STATUS_BAR_W, GUI_UI_ICON_STATUS_BAR_H));
		pPanel->addChild(newIcons);
		GUI_Label* newLabel = new GUI_Label(iRect(25, 130, 0, 0), "Soul:", 0, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, g_game.getPlayerSoul()));
		newLabel = new GUI_Label(iRect(25, 140, 0, 0), UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len))), INVENTORY_SOUL_EVENTID, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
		newLabel = new GUI_Label(iRect(99, 130, 0, 0), "Cap:", 0, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, g_game.getPlayerCapacity()));
		newLabel = new GUI_Label(iRect(99, 140, 0, 0), UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len))), INVENTORY_CAP_EVENTID, 255, 255, 255);
		newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
		newLabel->setFont(CLIENT_FONT_SMALL);
		pPanel->addChild(newLabel);
	}
	
	if(newWindow)
		g_engine.addToPanel(pPanel, GUI_PANEL_MAIN);
	else
		pPanel->checkPanels();
}

void UTIL_updateInventoryPanel()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_INVENTORY);
	if(!pPanel)
		pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_INVENTORY_MINIMIZED);

	if(pPanel)
	{
		GUI_Label* pLabel;
		if(g_game.hasCachedStat(CACHED_STAT_SOUL))
		{
			pLabel = SDL_static_cast(GUI_Label*, pPanel->getChild(INVENTORY_SOUL_EVENTID));
			if(pLabel)
			{
				Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, g_game.getPlayerSoul()));
				pLabel->setName(UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len))));
			}
		}
		
		if(g_game.hasCachedStat(CACHED_STAT_CAPACITY))
		{
			pLabel = SDL_static_cast(GUI_Label*, pPanel->getChild(INVENTORY_CAP_EVENTID));
			if(pLabel)
			{
				Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, g_game.getPlayerCapacity()));
				pLabel->setName(UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len))));
			}
		}
	}
}

void UTIL_flashQuestsButton()
{

}

void UTIL_flashFollowButton()
{

}

GUI_Icons::GUI_Icons(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_Icons::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(isInsideParent && m_tRect.isPointInside(x, y))
	{
		Uint32 playerIcons = g_game.getIcons();
		iRect rect = iRect(m_tRect.x1 + 2, m_tRect.y1 + 2, 9, 9);
		if(playerIcons & ICON_POISON)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are poisoned");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_BURN)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are burning");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_ENERGY)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are electrified");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_SWORDS)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You may not logout during a fight");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_DRUNK)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are drunk");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_MANASHIELD)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are protected by a magic shield");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_HASTE)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are hasted");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_PARALYZE)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are paralysed");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_DROWNING)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are drowning");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_FREEZING)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are freezing");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_DAZZLED)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are dazzled");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_CURSED)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are cursed");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_PARTY_BUFF)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are strengthened");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_REDSWORDS)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You may not logout or enter a protection zone");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_PIGEON)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are within a protection zone");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_BLEEDING)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are bleeding");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_LESSERHEX)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are suffering from a lesser hex (reduces received healing)");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_INTENSEHEX)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are suffering from an intense hex (reduces damage output, reduces received healing)");
				return;
			}
			rect.x1 += 10;
		}
		if(playerIcons & ICON_GREATEREHEX)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are suffering from a greater hex (reduces maximum hit points, reduces damage output, reduces received healing)");
				return;
			}
			rect.x1 += 10;
		}

		Creature* localPlayer = g_map.getLocalCreature();
		if(!localPlayer)
			return;

		if(g_game.hasGameFeature(GAME_FEATURE_REGENERATION_TIME) && g_game.getPlayerRegeneration() == 0)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are hungry");
				return;
			}
			rect.x1 += 10;
		}

		Uint8 playerSkull = localPlayer->getSkull();
		switch(playerSkull)
		{
			case SKULL_YELLOW:
			{
				if(rect.isPointInside(x, y))
				{
					g_engine.showDescription(x, y, "You may be attacked by other players");
					return;
				}
				rect.x1 += 10;
			}
			break;
			case SKULL_GREEN:
			{
				if(rect.isPointInside(x, y))
				{
					g_engine.showDescription(x, y, "You are a member of a party");
					return;
				}
				rect.x1 += 10;
			}
			break;
			case SKULL_WHITE:
			{
				if(rect.isPointInside(x, y))
				{
					g_engine.showDescription(x, y, "You have attacked an unmarked player");
					return;
				}
				rect.x1 += 10;
			}
			break;
			case SKULL_RED:
			{
				if(rect.isPointInside(x, y))
				{
					g_engine.showDescription(x, y, "You have killed too many unmarked players");
					return;
				}
				rect.x1 += 10;
			}
			break;
			case SKULL_BLACK:
			{
				if(rect.isPointInside(x, y))
				{
					g_engine.showDescription(x, y, "You have excessively killed unmarked players");
					return;
				}
				rect.x1 += 10;
			}
			break;
			case SKULL_ORANGE:
			{
				if(rect.isPointInside(x, y))
				{
					g_engine.showDescription(x, y, "You may suffer revenge from your former victim");
					return;
				}
				rect.x1 += 10;
			}
			break;
			default: break;
		}

		Uint8 playerEmblem = localPlayer->getEmblem();
		if(playerEmblem != GUILDEMBLEM_NONE && playerEmblem <= GUILDEMBLEM_NEUTRAL)
		{
			if(rect.isPointInside(x, y))
			{
				g_engine.showDescription(x, y, "You are involved in a guild war");
				return;
			}
			rect.x1 += 10;
		}
	}
}

void GUI_Icons::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_STATUS_BAR_X, GUI_UI_ICON_STATUS_BAR_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
	
	Sint32 posX = m_tRect.x1 + 2;
	Sint32 posY = m_tRect.y1 + 2;
	Uint32 playerIcons = g_game.getIcons();
	if(playerIcons & ICON_POISON)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_POISON_X, GUI_UI_STATUS_POISON_Y, posX, posY, GUI_UI_STATUS_POISON_W, GUI_UI_STATUS_POISON_H);
		posX += 10;
	}
	if(playerIcons & ICON_BURN)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_BURN_X, GUI_UI_STATUS_BURN_Y, posX, posY, GUI_UI_STATUS_BURN_W, GUI_UI_STATUS_BURN_H);
		posX += 10;
	}
	if(playerIcons & ICON_ENERGY)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_ENERGY_X, GUI_UI_STATUS_ENERGY_Y, posX, posY, GUI_UI_STATUS_ENERGY_W, GUI_UI_STATUS_ENERGY_H);
		posX += 10;
	}
	if(playerIcons & ICON_SWORDS)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_SWORDS_X, GUI_UI_STATUS_SWORDS_Y, posX, posY, GUI_UI_STATUS_SWORDS_W, GUI_UI_STATUS_SWORDS_H);
		posX += 10;
	}
	if(playerIcons & ICON_DRUNK)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_DRUNK_X, GUI_UI_STATUS_DRUNK_Y, posX, posY, GUI_UI_STATUS_DRUNK_W, GUI_UI_STATUS_DRUNK_H);
		posX += 10;
	}
	if(playerIcons & ICON_MANASHIELD)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_MANASHIELD_X, GUI_UI_STATUS_MANASHIELD_Y, posX, posY, GUI_UI_STATUS_MANASHIELD_W, GUI_UI_STATUS_MANASHIELD_H);
		posX += 10;
	}
	if(playerIcons & ICON_HASTE)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_HASTE_X, GUI_UI_STATUS_HASTE_Y, posX, posY, GUI_UI_STATUS_HASTE_W, GUI_UI_STATUS_HASTE_H);
		posX += 10;
	}
	if(playerIcons & ICON_PARALYZE)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_PARALYZE_X, GUI_UI_STATUS_PARALYZE_Y, posX, posY, GUI_UI_STATUS_PARALYZE_W, GUI_UI_STATUS_PARALYZE_H);
		posX += 10;
	}
	if(playerIcons & ICON_DROWNING)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_DROWNING_X, GUI_UI_STATUS_DROWNING_Y, posX, posY, GUI_UI_STATUS_DROWNING_W, GUI_UI_STATUS_DROWNING_H);
		posX += 10;
	}
	if(playerIcons & ICON_FREEZING)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_FREEZING_X, GUI_UI_STATUS_FREEZING_Y, posX, posY, GUI_UI_STATUS_FREEZING_W, GUI_UI_STATUS_FREEZING_H);
		posX += 10;
	}
	if(playerIcons & ICON_DAZZLED)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_DAZZLED_X, GUI_UI_STATUS_DAZZLED_Y, posX, posY, GUI_UI_STATUS_DAZZLED_W, GUI_UI_STATUS_DAZZLED_H);
		posX += 10;
	}
	if(playerIcons & ICON_CURSED)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_CURSED_X, GUI_UI_STATUS_CURSED_Y, posX, posY, GUI_UI_STATUS_CURSED_W, GUI_UI_STATUS_CURSED_H);
		posX += 10;
	}
	if(playerIcons & ICON_PARTY_BUFF)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_BUFF_X, GUI_UI_STATUS_BUFF_Y, posX, posY, GUI_UI_STATUS_BUFF_W, GUI_UI_STATUS_BUFF_H);
		posX += 10;
	}
	if(playerIcons & ICON_REDSWORDS)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_PZLOCK_X, GUI_UI_STATUS_PZLOCK_Y, posX, posY, GUI_UI_STATUS_PZLOCK_W, GUI_UI_STATUS_PZLOCK_H);
		posX += 10;
	}
	if(playerIcons & ICON_PIGEON)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_PIGEON_X, GUI_UI_STATUS_PIGEON_Y, posX, posY, GUI_UI_STATUS_PIGEON_W, GUI_UI_STATUS_PIGEON_H);
		posX += 10;
	}
	if(playerIcons & ICON_BLEEDING)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_BLEEDING_X, GUI_UI_STATUS_BLEEDING_Y, posX, posY, GUI_UI_STATUS_BLEEDING_W, GUI_UI_STATUS_BLEEDING_H);
		posX += 10;
	}
	if(playerIcons & ICON_LESSERHEX)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_LESSERHEX_X, GUI_UI_STATUS_LESSERHEX_Y, posX, posY, GUI_UI_STATUS_LESSERHEX_W, GUI_UI_STATUS_LESSERHEX_H);
		posX += 10;
	}
	if(playerIcons & ICON_INTENSEHEX)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_INTENSEHEX_X, GUI_UI_STATUS_INTENSEHEX_Y, posX, posY, GUI_UI_STATUS_INTENSEHEX_W, GUI_UI_STATUS_INTENSEHEX_H);
		posX += 10;
	}
	if(playerIcons & ICON_GREATEREHEX)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_GREATEREHEX_X, GUI_UI_STATUS_GREATEREHEX_Y, posX, posY, GUI_UI_STATUS_GREATEREHEX_W, GUI_UI_STATUS_GREATEREHEX_H);
		posX += 10;
	}

	Creature* localPlayer = g_map.getLocalCreature();
	if(!localPlayer)
		return;

	if(g_game.hasGameFeature(GAME_FEATURE_REGENERATION_TIME) && g_game.getPlayerRegeneration() == 0)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_HUNGRY_X, GUI_UI_STATUS_HUNGRY_Y, posX, posY, GUI_UI_STATUS_HUNGRY_W, GUI_UI_STATUS_HUNGRY_H);
		posX += 10;
	}

	Uint8 playerSkull = localPlayer->getSkull();
	switch(playerSkull)
	{
		case SKULL_YELLOW:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_YELLOWSKULL_X, GUI_UI_STATUS_YELLOWSKULL_Y, posX, posY, GUI_UI_STATUS_YELLOWSKULL_W, GUI_UI_STATUS_YELLOWSKULL_H);
			posX += 10;
		}
		break;
		case SKULL_GREEN:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_GREENSKULL_X, GUI_UI_STATUS_GREENSKULL_Y, posX, posY, GUI_UI_STATUS_GREENSKULL_W, GUI_UI_STATUS_GREENSKULL_H);
			posX += 10;
		}
		break;
		case SKULL_WHITE:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_WHITESKULL_X, GUI_UI_STATUS_WHITESKULL_Y, posX, posY, GUI_UI_STATUS_WHITESKULL_W, GUI_UI_STATUS_WHITESKULL_H);
			posX += 10;
		}
		break;
		case SKULL_RED:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_REDSKULL_X, GUI_UI_STATUS_REDSKULL_Y, posX, posY, GUI_UI_STATUS_REDSKULL_W, GUI_UI_STATUS_REDSKULL_H);
			posX += 10;
		}
		break;
		case SKULL_BLACK:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_BLACKSKULL_X, GUI_UI_STATUS_BLACKSKULL_Y, posX, posY, GUI_UI_STATUS_BLACKSKULL_W, GUI_UI_STATUS_BLACKSKULL_H);
			posX += 10;
		}
		break;
		case SKULL_ORANGE:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_ORANGOSKULL_X, GUI_UI_STATUS_ORANGOSKULL_Y, posX, posY, GUI_UI_STATUS_ORANGOSKULL_W, GUI_UI_STATUS_ORANGOSKULL_H);
			posX += 10;
		}
		break;
		default: break;
	}

	Uint8 playerEmblem = localPlayer->getEmblem();
	if(playerEmblem != GUILDEMBLEM_NONE && playerEmblem <= GUILDEMBLEM_NEUTRAL)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_STATUS_GUILDWAR_X, GUI_UI_STATUS_GUILDWAR_Y, posX, posY, GUI_UI_STATUS_GUILDWAR_W, GUI_UI_STATUS_GUILDWAR_H);
		posX += 10;
	}
}

GUI_InventoryItem::GUI_InventoryItem(iRect boxRect, Sint32 skinX, Sint32 skinY, Uint8 slot, Uint32 internalID) : m_skinX(skinX), m_skinY(skinY), m_slot(slot)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void* GUI_InventoryItem::onAction(Sint32, Sint32)
{
	ItemUI* item = g_game.getInventoryItem(m_slot);
	if(item)
		return SDL_reinterpret_cast(void*, item);

	return NULL;
}

void GUI_InventoryItem::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(g_engine.getAction() == CLIENT_ACTION_MOVEITEM)
	{
		if(isInsideParent && m_tRect.isPointInside(x, y))
			m_selected = true;
		else
			m_selected = false;
	}
}

void GUI_InventoryItem::onLMouseDown(Sint32 x, Sint32 y)
{
	ItemUI* item = g_game.getInventoryItem(m_slot);
	if(item)
	{
		Position& position = item->getCurrentPosition();
		g_engine.setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), position.x, position.y, position.z, 0);
		g_engine.setActionData(CLIENT_ACTION_SECOND, 0, item->getItemCount(), 0, 0, 0, 0);
		g_engine.noteManualItemInteraction();
		g_engine.enableMoveItem(x, y);
	}

	g_engine.setAction(CLIENT_ACTION_LEFTMOUSE);
}

void GUI_InventoryItem::onLMouseUp(Sint32, Sint32)
{
	if(g_engine.getAction() == CLIENT_ACTION_MOVEITEM && m_selected)
	{
		g_engine.noteManualItemInteraction();
		g_engine.initMove(0xFFFF, SDL_static_cast(Uint16, m_slot) + 1, 0);
		g_engine.setAction(CLIENT_ACTION_NONE);
	}
}

void GUI_InventoryItem::onRMouseDown(Sint32, Sint32)
{
	g_engine.setAction(CLIENT_ACTION_RIGHTMOUSE);
}

void GUI_InventoryItem::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_INVENTORY_EMPTY_X, GUI_UI_INVENTORY_EMPTY_Y, m_tRect.x1 - 1, m_tRect.y1 - 1, m_tRect.x2 + 2, m_tRect.y2 + 2);

	ItemUI* item = g_game.getInventoryItem(m_slot);
	if(item)
		item->render(m_tRect.x1, m_tRect.y1, m_tRect.y2);
	else
		renderer->drawPicture(GUI_UI_IMAGE, m_skinX, m_skinY, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);

	if(m_selected)
	{
		if(g_engine.getAction() != CLIENT_ACTION_MOVEITEM)
		{
			m_selected = false;
			return;
		}

		renderer->drawRectangle(m_tRect.x1 - 1, m_tRect.y1 - 1, m_tRect.x2 + 2, m_tRect.y2 + 2, 1, 255, 255, 255, 255);
	}
}
