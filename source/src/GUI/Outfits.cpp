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
#include "../GUI_Elements/GUI_CheckBox.h"
#include "../GUI_Elements/GUI_Grouper.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../thingManager.h"
#include "../game.h"
#include "Outfits.h"

#define OUTFITS_TITLE "Select Outfit"
#define OUTFITS_WIDTH 450
#define OUTFITS_HEIGHT 391
#define OUTFITS_CANCEL_EVENTID 1000
#define OUTFITS_OK_EVENTID 1001
#define OUTFITS_LABEL_PREVIEW_TITLE "Preview:"
#define OUTFITS_LABEL_PREVIEW_X 63
#define OUTFITS_LABEL_PREVIEW_Y 32
#define OUTFITS_LABEL_FILTER_TITLE "Filter:"
#define OUTFITS_LABEL_FILTER_X 71
#define OUTFITS_LABEL_FILTER_Y 164
#define OUTFITS_LABEL_CONFIGURE_TITLE "Configure:"
#define OUTFITS_LABEL_CONFIGURE_X 57
#define OUTFITS_LABEL_CONFIGURE_Y 252
#define OUTFITS_LABEL_OUTFIT_X 304
#define OUTFITS_LABEL_OUTFIT_Y 185
#define OUTFITS_LABEL_OUTFIT_EVENTID 1002
#define OUTFITS_LABEL_MOUNT_X 304
#define OUTFITS_LABEL_MOUNT_Y 210
#define OUTFITS_LABEL_MOUNT_EVENTID 1003
#define OUTFITS_GROUPER_OUTFIT_NAME_X 199
#define OUTFITS_GROUPER_OUTFIT_NAME_Y 179
#define OUTFITS_GROUPER_OUTFIT_NAME_W 210
#define OUTFITS_GROUPER_OUTFIT_NAME_H 20
#define OUTFITS_GROUPER_MOUNT_NAME_X 199
#define OUTFITS_GROUPER_MOUNT_NAME_Y 204
#define OUTFITS_GROUPER_MOUNT_NAME_W 210
#define OUTFITS_GROUPER_MOUNT_NAME_H 20
#define OUTFITS_CHECKBOX_MOVEMENT_TEXT "Movement"
#define OUTFITS_CHECKBOX_MOVEMENT_X 16
#define OUTFITS_CHECKBOX_MOVEMENT_Y 47
#define OUTFITS_CHECKBOX_MOVEMENT_W 148
#define OUTFITS_CHECKBOX_MOVEMENT_H 22
#define OUTFITS_CHECKBOX_MOVEMENT_EVENTID 1004
#define OUTFITS_CHECKBOX_OUTFIT_TEXT "Show Outfit"
#define OUTFITS_CHECKBOX_OUTFIT_X 16
#define OUTFITS_CHECKBOX_OUTFIT_Y 74
#define OUTFITS_CHECKBOX_OUTFIT_W 148
#define OUTFITS_CHECKBOX_OUTFIT_H 22
#define OUTFITS_CHECKBOX_OUTFIT_EVENTID 1005
#define OUTFITS_CHECKBOX_FLOOR_TEXT "Show Floor"
#define OUTFITS_CHECKBOX_FLOOR_X 16
#define OUTFITS_CHECKBOX_FLOOR_Y 101
#define OUTFITS_CHECKBOX_FLOOR_W 148
#define OUTFITS_CHECKBOX_FLOOR_H 22
#define OUTFITS_CHECKBOX_FLOOR_EVENTID 1006
#define OUTFITS_CHECKBOX_OUTFITS_TEXT "Only my Outfits"
#define OUTFITS_CHECKBOX_OUTFITS_X 16
#define OUTFITS_CHECKBOX_OUTFITS_Y 179
#define OUTFITS_CHECKBOX_OUTFITS_W 148
#define OUTFITS_CHECKBOX_OUTFITS_H 22
#define OUTFITS_CHECKBOX_OUTFITS_EVENTID 1007
#define OUTFITS_CHECKBOX_MOUNTS_TEXT "Only my Mounts"
#define OUTFITS_CHECKBOX_MOUNTS_X 16
#define OUTFITS_CHECKBOX_MOUNTS_Y 206
#define OUTFITS_CHECKBOX_MOUNTS_W 148
#define OUTFITS_CHECKBOX_MOUNTS_H 22
#define OUTFITS_CHECKBOX_MOUNTS_EVENTID 1008
#define OUTFITS_CHECKBOX_ADDON1_TEXT "Addon 1"
#define OUTFITS_CHECKBOX_ADDON1_X 16
#define OUTFITS_CHECKBOX_ADDON1_Y 267
#define OUTFITS_CHECKBOX_ADDON1_W 148
#define OUTFITS_CHECKBOX_ADDON1_H 22
#define OUTFITS_CHECKBOX_ADDON1_EVENTID 1009
#define OUTFITS_CHECKBOX_ADDON2_TEXT "Addon 2"
#define OUTFITS_CHECKBOX_ADDON2_X 16
#define OUTFITS_CHECKBOX_ADDON2_Y 294
#define OUTFITS_CHECKBOX_ADDON2_W 148
#define OUTFITS_CHECKBOX_ADDON2_H 22
#define OUTFITS_CHECKBOX_ADDON2_EVENTID 1010
#define OUTFITS_CHECKBOX_MOUNT_TEXT "Mount"
#define OUTFITS_CHECKBOX_MOUNT_X 16
#define OUTFITS_CHECKBOX_MOUNT_Y 321
#define OUTFITS_CHECKBOX_MOUNT_W 148
#define OUTFITS_CHECKBOX_MOUNT_H 22
#define OUTFITS_CHECKBOX_MOUNT_EVENTID 1011
#define OUTFITS_GROUPER_OUTFIT_X 174
#define OUTFITS_GROUPER_OUTFIT_Y 29
#define OUTFITS_GROUPER_OUTFIT_W 260
#define OUTFITS_GROUPER_OUTFIT_H 145
#define OUTFITS_GROUPER_OUTFIT_NAME_X 199
#define OUTFITS_GROUPER_OUTFIT_NAME_Y 179
#define OUTFITS_GROUPER_OUTFIT_NAME_W 210
#define OUTFITS_GROUPER_OUTFIT_NAME_H 20
#define OUTFITS_GROUPER_MOUNT_NAME_X 199
#define OUTFITS_GROUPER_MOUNT_NAME_Y 204
#define OUTFITS_GROUPER_MOUNT_NAME_W 210
#define OUTFITS_GROUPER_MOUNT_NAME_H 20
#define OUTFITS_ICON_PREV_OUTFIT_X 174
#define OUTFITS_ICON_PREV_OUTFIT_Y 179
#define OUTFITS_ICON_PREV_OUTFIT_W GUI_UI_ICON_ARROW_LEFT_UP_W
#define OUTFITS_ICON_PREV_OUTFIT_H GUI_UI_ICON_ARROW_LEFT_UP_H
#define OUTFITS_ICON_PREV_OUTFIT_EVENTID 1012
#define OUTFITS_ICON_PREV_MOUNT_X 174
#define OUTFITS_ICON_PREV_MOUNT_Y 204
#define OUTFITS_ICON_PREV_MOUNT_W GUI_UI_ICON_ARROW_LEFT_UP_W
#define OUTFITS_ICON_PREV_MOUNT_H GUI_UI_ICON_ARROW_LEFT_UP_H
#define OUTFITS_ICON_PREV_MOUNT_EVENTID 1013
#define OUTFITS_ICON_PREV_DIRECTION_X 176
#define OUTFITS_ICON_PREV_DIRECTION_Y 152
#define OUTFITS_ICON_PREV_DIRECTION_W GUI_UI_ICON_ROTATE_LEFT_UP_W
#define OUTFITS_ICON_PREV_DIRECTION_H GUI_UI_ICON_ROTATE_LEFT_UP_H
#define OUTFITS_ICON_PREV_DIRECTION_EVENTID 1014
#define OUTFITS_ICON_NEXT_OUTFIT_X 414
#define OUTFITS_ICON_NEXT_OUTFIT_Y 179
#define OUTFITS_ICON_NEXT_OUTFIT_W GUI_UI_ICON_ARROW_RIGHT_UP_W
#define OUTFITS_ICON_NEXT_OUTFIT_H GUI_UI_ICON_ARROW_RIGHT_UP_H
#define OUTFITS_ICON_NEXT_OUTFIT_EVENTID 1015
#define OUTFITS_ICON_NEXT_MOUNT_X 414
#define OUTFITS_ICON_NEXT_MOUNT_Y 204
#define OUTFITS_ICON_NEXT_MOUNT_W GUI_UI_ICON_ARROW_RIGHT_UP_W
#define OUTFITS_ICON_NEXT_MOUNT_H GUI_UI_ICON_ARROW_RIGHT_UP_H
#define OUTFITS_ICON_NEXT_MOUNT_EVENTID 1016
#define OUTFITS_ICON_NEXT_DIRECTION_X 412
#define OUTFITS_ICON_NEXT_DIRECTION_Y 152
#define OUTFITS_ICON_NEXT_DIRECTION_W GUI_UI_ICON_ROTATE_RIGHT_UP_W
#define OUTFITS_ICON_NEXT_DIRECTION_H GUI_UI_ICON_ROTATE_RIGHT_UP_H
#define OUTFITS_ICON_NEXT_DIRECTION_EVENTID 1017
#define OUTFITS_OUTFIT_VIEW_X 175
#define OUTFITS_OUTFIT_VIEW_Y 30
#define OUTFITS_OUTFIT_VIEW_W 258
#define OUTFITS_OUTFIT_VIEW_H 143
#define OUTFITS_OUTFIT_VIEW_EVENTID 1018
#define OUTFITS_OUTFIT_COLORS_X 177
#define OUTFITS_OUTFIT_COLORS_Y 229
#define OUTFITS_OUTFIT_COLORS_W 254
#define OUTFITS_OUTFIT_COLORS_H 114

extern Engine g_engine;
extern Game g_game;
extern ThingManager g_thingManager;
extern Uint32 g_frameTime;

std::vector<OutfitDetail> g_outfits;
std::vector<MountDetail> g_mounts;
Uint16 g_outfitLookType;
Uint16 g_outfitMount;
Uint8 g_outfitColors[4];
Uint8 g_outfitAddons;

void outfits_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case OUTFITS_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				g_engine.removeWindow(pWindow);
				g_outfits.clear();
				g_mounts.clear();
				g_outfits.shrink_to_fit();
				g_mounts.shrink_to_fit();
			}
		}
		break;
		case OUTFITS_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				g_game.sendSetOutfit(g_outfitLookType, g_outfitColors[0], g_outfitColors[1], g_outfitColors[2], g_outfitColors[3], g_outfitAddons, g_outfitMount);
				g_engine.removeWindow(pWindow);
				g_outfits.clear();
				g_mounts.clear();
				g_outfits.shrink_to_fit();
				g_mounts.shrink_to_fit();
			}
		}
		break;
		case OUTFITS_CHECKBOX_MOVEMENT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
				{
					if(status == 1)
						pOutfitView->startMovement();
					else
						pOutfitView->stopMovement();
				}
			}
		}
		break;
		case OUTFITS_CHECKBOX_OUTFIT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->refresh();
			}
		}
		break;
		case OUTFITS_CHECKBOX_ADDON1_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				if(status == 1)
					g_outfitAddons |= 1;
				else
					g_outfitAddons &= ~1;
			}
		}
		break;
		case OUTFITS_CHECKBOX_ADDON2_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				if(status == 1)
					g_outfitAddons |= 2;
				else
					g_outfitAddons &= ~2;
			}
		}
		break;
		case OUTFITS_CHECKBOX_MOUNT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->refresh();
			}
		}
		break;
		case OUTFITS_ICON_PREV_OUTFIT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS && g_outfits.size() > 1)
			{
				std::vector<OutfitDetail>::iterator it = g_outfits.begin();
				for(std::vector<OutfitDetail>::iterator end = g_outfits.end(); it != end; ++it)
				{
					if((*it).outfitID == g_outfitLookType)
						break;
				}
				if(it == g_outfits.begin())
					it = g_outfits.end();
				--it;
				g_outfitLookType = (*it).outfitID;

				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->refresh();

				GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pWindow->getChild(OUTFITS_LABEL_OUTFIT_EVENTID));
				if(pLabel)
					pLabel->setName((*it).outfitName);

				GUI_CheckBox* pCheckBox1 = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(OUTFITS_CHECKBOX_ADDON1_EVENTID));
				GUI_CheckBox* pCheckBox2 = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(OUTFITS_CHECKBOX_ADDON2_EVENTID));
				if(pCheckBox1 && pCheckBox2)
				{
					Uint8 outfitAddons = (*it).outfitAddons;
					if(outfitAddons & 1)//First addon
					{
						if(pCheckBox1->isChecked())
							g_outfitAddons |= 1;
						else
							g_outfitAddons &= ~1;
						pCheckBox1->setColor(180, 180, 180);
						pCheckBox1->startEvents();
					}
					else
					{
						g_outfitAddons &= ~1;
						pCheckBox1->setColor(112, 112, 112);
						pCheckBox1->stopEvents();
					}
					if(outfitAddons & 2)//Second addon
					{
						if(pCheckBox2->isChecked())
							g_outfitAddons |= 2;
						else
							g_outfitAddons &= ~2;
						pCheckBox2->setColor(180, 180, 180);
						pCheckBox2->startEvents();
					}
					else
					{
						g_outfitAddons &= ~2;
						pCheckBox2->setColor(112, 112, 112);
						pCheckBox2->stopEvents();
					}
				}
			}
		}
		break;
		case OUTFITS_ICON_NEXT_OUTFIT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS && g_outfits.size() > 1)
			{
				std::vector<OutfitDetail>::iterator it = g_outfits.begin();
				for(std::vector<OutfitDetail>::iterator end = g_outfits.end(); it != end; ++it)
				{
					if((*it).outfitID == g_outfitLookType)
						break;
				}
				++it;
				if(it == g_outfits.end())
					it = g_outfits.begin();
				g_outfitLookType = (*it).outfitID;

				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->refresh();

				GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pWindow->getChild(OUTFITS_LABEL_OUTFIT_EVENTID));
				if(pLabel)
					pLabel->setName((*it).outfitName);

				GUI_CheckBox* pCheckBox1 = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(OUTFITS_CHECKBOX_ADDON1_EVENTID));
				GUI_CheckBox* pCheckBox2 = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(OUTFITS_CHECKBOX_ADDON2_EVENTID));
				if(pCheckBox1 && pCheckBox2)
				{
					Uint8 outfitAddons = (*it).outfitAddons;
					if(outfitAddons & 1)//First addon
					{
						if(pCheckBox1->isChecked())
							g_outfitAddons |= 1;
						else
							g_outfitAddons &= ~1;
						pCheckBox1->setColor(180, 180, 180);
						pCheckBox1->startEvents();
					}
					else
					{
						g_outfitAddons &= ~1;
						pCheckBox1->setColor(112, 112, 112);
						pCheckBox1->stopEvents();
					}
					if(outfitAddons & 2)//Second addon
					{
						if(pCheckBox2->isChecked())
							g_outfitAddons |= 2;
						else
							g_outfitAddons &= ~2;
						pCheckBox2->setColor(180, 180, 180);
						pCheckBox2->startEvents();
					}
					else
					{
						g_outfitAddons &= ~2;
						pCheckBox2->setColor(112, 112, 112);
						pCheckBox2->stopEvents();
					}
				}
			}
		}
		break;
		case OUTFITS_ICON_PREV_MOUNT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS && g_mounts.size() > 1)
			{
				std::vector<MountDetail>::iterator it = g_mounts.begin();
				for(std::vector<MountDetail>::iterator end = g_mounts.end(); it != end; ++it)
				{
					if((*it).mountID == g_outfitMount)
						break;
				}
				if(it == g_mounts.begin())
					it = g_mounts.end();
				--it;
				g_outfitMount = (*it).mountID;

				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->refresh();

				GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pWindow->getChild(OUTFITS_LABEL_MOUNT_EVENTID));
				if(pLabel)
					pLabel->setName((*it).mountName);
			}
		}
		break;
		case OUTFITS_ICON_NEXT_MOUNT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS && g_mounts.size() > 1)
			{
				std::vector<MountDetail>::iterator it = g_mounts.begin();
				for(std::vector<MountDetail>::iterator end = g_mounts.end(); it != end; ++it)
				{
					if((*it).mountID == g_outfitMount)
						break;
				}
				++it;
				if(it == g_mounts.end())
					it = g_mounts.begin();
				g_outfitMount = (*it).mountID;

				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->refresh();

				GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pWindow->getChild(OUTFITS_LABEL_MOUNT_EVENTID));
				if(pLabel)
					pLabel->setName((*it).mountName);
			}
		}
		break;
		case OUTFITS_ICON_PREV_DIRECTION_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->previousDirection();
			}
		}
		break;
		case OUTFITS_ICON_NEXT_DIRECTION_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_OUTFITS)
			{
				GUI_Outfit_View* pOutfitView = SDL_static_cast(GUI_Outfit_View*, pWindow->getChild(OUTFITS_OUTFIT_VIEW_EVENTID));
				if(pOutfitView)
					pOutfitView->nextDirection();
			}
		}
		break;
		default: break;
	}
}

void UTIL_createOutfitWindow(Uint16 lookType, Uint8 lookHead, Uint8 lookBody, Uint8 lookLegs, Uint8 lookFeet, Uint8 lookAddons, Uint16 lookMount, std::vector<OutfitDetail>& outfits, std::vector<MountDetail>& mounts)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_OUTFITS);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	if(lookType == 0)
	{
		if(outfits.empty())
			return;
		lookType = (*outfits.begin()).outfitID;
	}

	std::string outfitName = "Your Character";
	std::string mountName = "No Mount";
	Uint8 outfitAddons = 0;
	bool foundOutfit = false;
	bool foundMount = false;
	for(std::vector<OutfitDetail>::iterator it = outfits.begin(), end = outfits.end(); it != end; ++it)
	{
		OutfitDetail& outfit = (*it);
		if(outfit.outfitID == lookType)
		{
			outfitName = outfit.outfitName;
			outfitAddons = outfit.outfitAddons;
			foundOutfit = true;
			break;
		}
	}

	if(lookMount != 0)
	{
		for(std::vector<MountDetail>::iterator it = mounts.begin(), end = mounts.end(); it != end; ++it)
		{
			MountDetail& mount = (*it);
			if(mount.mountID == lookMount)
			{
				mountName = mount.mountName;
				foundMount = true;
				break;
			}
		}

		if(!foundMount)
		{
			MountDetail newMount;
			newMount.mountID = lookMount;
			newMount.mountName = "Your Mount";
			mounts.push_back(newMount);
			mountName = "Your Mount";
			foundMount = true;
		}
	}

	if(!foundOutfit)
	{
		OutfitDetail newOutfit;
		newOutfit.outfitID = lookType;
		newOutfit.outfitName = "Your Character";
		newOutfit.outfitAddons = 0;
		outfits.push_back(newOutfit);
	}

	g_outfits = std::move(outfits);
	g_mounts = std::move(mounts);
	g_outfitLookType = lookType;
	g_outfitMount = lookMount;
	g_outfitColors[0] = lookHead;
	g_outfitColors[1] = lookBody;
	g_outfitColors[2] = lookLegs;
	g_outfitColors[3] = lookFeet;
	g_outfitAddons = lookAddons;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, OUTFITS_WIDTH, OUTFITS_HEIGHT), OUTFITS_TITLE, GUI_WINDOW_OUTFITS);
	GUI_Outfit_View* newView = new GUI_Outfit_View(iRect(OUTFITS_OUTFIT_VIEW_X, OUTFITS_OUTFIT_VIEW_Y, OUTFITS_OUTFIT_VIEW_W, OUTFITS_OUTFIT_VIEW_H), OUTFITS_OUTFIT_VIEW_EVENTID);
	newWindow->addChild(newView);
	GUI_Outfit_Colors* newColors = new GUI_Outfit_Colors(iRect(OUTFITS_OUTFIT_COLORS_X, OUTFITS_OUTFIT_COLORS_Y, OUTFITS_OUTFIT_COLORS_W, OUTFITS_OUTFIT_COLORS_H));
	newColors->startEvents();
	newWindow->addChild(newColors);
	GUI_Icon* newIcon = new GUI_Icon(iRect(OUTFITS_ICON_PREV_OUTFIT_X, OUTFITS_ICON_PREV_OUTFIT_Y, OUTFITS_ICON_PREV_OUTFIT_W, OUTFITS_ICON_PREV_OUTFIT_H), GUI_UI_IMAGE, GUI_UI_ICON_ARROW_LEFT_UP_X, GUI_UI_ICON_ARROW_LEFT_UP_Y, GUI_UI_ICON_ARROW_LEFT_DOWN_X, GUI_UI_ICON_ARROW_LEFT_DOWN_Y);
	newIcon->setButtonEventCallback(&outfits_Events, OUTFITS_ICON_PREV_OUTFIT_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(OUTFITS_ICON_PREV_MOUNT_X, OUTFITS_ICON_PREV_MOUNT_Y, OUTFITS_ICON_PREV_MOUNT_W, OUTFITS_ICON_PREV_MOUNT_H), GUI_UI_IMAGE, GUI_UI_ICON_ARROW_LEFT_UP_X, GUI_UI_ICON_ARROW_LEFT_UP_Y, GUI_UI_ICON_ARROW_LEFT_DOWN_X, GUI_UI_ICON_ARROW_LEFT_DOWN_Y);
	newIcon->setButtonEventCallback(&outfits_Events, OUTFITS_ICON_PREV_MOUNT_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(OUTFITS_ICON_PREV_DIRECTION_X, OUTFITS_ICON_PREV_DIRECTION_Y, OUTFITS_ICON_PREV_DIRECTION_W, OUTFITS_ICON_PREV_DIRECTION_H), GUI_UI_IMAGE, GUI_UI_ICON_ROTATE_LEFT_UP_X, GUI_UI_ICON_ROTATE_LEFT_UP_Y, GUI_UI_ICON_ROTATE_LEFT_DOWN_X, GUI_UI_ICON_ROTATE_LEFT_DOWN_Y);
	newIcon->setButtonEventCallback(&outfits_Events, OUTFITS_ICON_PREV_DIRECTION_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(OUTFITS_ICON_NEXT_OUTFIT_X, OUTFITS_ICON_NEXT_OUTFIT_Y, OUTFITS_ICON_NEXT_OUTFIT_W, OUTFITS_ICON_NEXT_OUTFIT_H), GUI_UI_IMAGE, GUI_UI_ICON_ARROW_RIGHT_UP_X, GUI_UI_ICON_ARROW_RIGHT_UP_Y, GUI_UI_ICON_ARROW_RIGHT_DOWN_X, GUI_UI_ICON_ARROW_RIGHT_DOWN_Y);
	newIcon->setButtonEventCallback(&outfits_Events, OUTFITS_ICON_NEXT_OUTFIT_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(OUTFITS_ICON_NEXT_MOUNT_X, OUTFITS_ICON_NEXT_MOUNT_Y, OUTFITS_ICON_NEXT_MOUNT_W, OUTFITS_ICON_NEXT_MOUNT_H), GUI_UI_IMAGE, GUI_UI_ICON_ARROW_RIGHT_UP_X, GUI_UI_ICON_ARROW_RIGHT_UP_Y, GUI_UI_ICON_ARROW_RIGHT_DOWN_X, GUI_UI_ICON_ARROW_RIGHT_DOWN_Y);
	newIcon->setButtonEventCallback(&outfits_Events, OUTFITS_ICON_NEXT_MOUNT_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(OUTFITS_ICON_NEXT_DIRECTION_X, OUTFITS_ICON_NEXT_DIRECTION_Y, OUTFITS_ICON_NEXT_DIRECTION_W, OUTFITS_ICON_NEXT_DIRECTION_H), GUI_UI_IMAGE, GUI_UI_ICON_ROTATE_RIGHT_UP_X, GUI_UI_ICON_ROTATE_RIGHT_UP_Y, GUI_UI_ICON_ROTATE_RIGHT_DOWN_X, GUI_UI_ICON_ROTATE_RIGHT_DOWN_Y);
	newIcon->setButtonEventCallback(&outfits_Events, OUTFITS_ICON_NEXT_DIRECTION_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_Grouper* newGrouper = new GUI_Grouper(iRect(OUTFITS_GROUPER_OUTFIT_X, OUTFITS_GROUPER_OUTFIT_Y, OUTFITS_GROUPER_OUTFIT_W, OUTFITS_GROUPER_OUTFIT_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(OUTFITS_GROUPER_OUTFIT_NAME_X, OUTFITS_GROUPER_OUTFIT_NAME_Y, OUTFITS_GROUPER_OUTFIT_NAME_W, OUTFITS_GROUPER_OUTFIT_NAME_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(OUTFITS_GROUPER_MOUNT_NAME_X, OUTFITS_GROUPER_MOUNT_NAME_Y, OUTFITS_GROUPER_MOUNT_NAME_W, OUTFITS_GROUPER_MOUNT_NAME_H));
	newWindow->addChild(newGrouper);
	GUI_CheckBox* newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_MOVEMENT_X, OUTFITS_CHECKBOX_MOVEMENT_Y, OUTFITS_CHECKBOX_MOVEMENT_W, OUTFITS_CHECKBOX_MOVEMENT_H), OUTFITS_CHECKBOX_MOVEMENT_TEXT, false, OUTFITS_CHECKBOX_MOVEMENT_EVENTID);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_MOVEMENT_EVENTID);
	newCheckBox->startEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_OUTFIT_X, OUTFITS_CHECKBOX_OUTFIT_Y, OUTFITS_CHECKBOX_OUTFIT_W, OUTFITS_CHECKBOX_OUTFIT_H), OUTFITS_CHECKBOX_OUTFIT_TEXT, true, OUTFITS_CHECKBOX_OUTFIT_EVENTID, 112, 112, 112);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_OUTFIT_EVENTID);
	newCheckBox->stopEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_FLOOR_X, OUTFITS_CHECKBOX_FLOOR_Y, OUTFITS_CHECKBOX_FLOOR_W, OUTFITS_CHECKBOX_FLOOR_H), OUTFITS_CHECKBOX_FLOOR_TEXT, true, OUTFITS_CHECKBOX_FLOOR_EVENTID, 112, 112, 112);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_FLOOR_EVENTID);
	newCheckBox->stopEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_OUTFITS_X, OUTFITS_CHECKBOX_OUTFITS_Y, OUTFITS_CHECKBOX_OUTFITS_W, OUTFITS_CHECKBOX_OUTFITS_H), OUTFITS_CHECKBOX_OUTFITS_TEXT, true, OUTFITS_CHECKBOX_OUTFITS_EVENTID, 112, 112, 112);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_OUTFITS_EVENTID);
	newCheckBox->stopEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_MOUNTS_X, OUTFITS_CHECKBOX_MOUNTS_Y, OUTFITS_CHECKBOX_MOUNTS_W, OUTFITS_CHECKBOX_MOUNTS_H), OUTFITS_CHECKBOX_MOUNTS_TEXT, true, OUTFITS_CHECKBOX_MOUNTS_EVENTID, 112, 112, 112);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_MOUNTS_EVENTID);
	newCheckBox->stopEvents();
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_ADDON1_X, OUTFITS_CHECKBOX_ADDON1_Y, OUTFITS_CHECKBOX_ADDON1_W, OUTFITS_CHECKBOX_ADDON1_H), OUTFITS_CHECKBOX_ADDON1_TEXT, (g_outfitAddons & 1), OUTFITS_CHECKBOX_ADDON1_EVENTID);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_ADDON1_EVENTID);
	if(outfitAddons & 1)//First addon
		newCheckBox->startEvents();
	else
	{
		g_outfitAddons &= ~1;
		newCheckBox->setColor(112, 112, 112);
		newCheckBox->stopEvents();
	}
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_ADDON2_X, OUTFITS_CHECKBOX_ADDON2_Y, OUTFITS_CHECKBOX_ADDON2_W, OUTFITS_CHECKBOX_ADDON2_H), OUTFITS_CHECKBOX_ADDON2_TEXT, (g_outfitAddons & 2), OUTFITS_CHECKBOX_ADDON2_EVENTID);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_ADDON2_EVENTID);
	if(outfitAddons & 2)//Second addon
		newCheckBox->startEvents();
	else
	{
		g_outfitAddons &= ~2;
		newCheckBox->setColor(112, 112, 112);
		newCheckBox->stopEvents();
	}
	newWindow->addChild(newCheckBox);
	newCheckBox = new GUI_CheckBox(iRect(OUTFITS_CHECKBOX_MOUNT_X, OUTFITS_CHECKBOX_MOUNT_Y, OUTFITS_CHECKBOX_MOUNT_W, OUTFITS_CHECKBOX_MOUNT_H), OUTFITS_CHECKBOX_MOUNT_TEXT, true, OUTFITS_CHECKBOX_MOUNT_EVENTID);
	newCheckBox->setBoxEventCallback(&outfits_Events, OUTFITS_CHECKBOX_MOUNT_EVENTID);
	if(!mounts.empty())
		newCheckBox->startEvents();
	else
	{
		newCheckBox->setColor(112, 112, 112);
		newCheckBox->stopEvents();
	}
	newWindow->addChild(newCheckBox);
	GUI_Label* newLabel = new GUI_Label(iRect(OUTFITS_LABEL_PREVIEW_X, OUTFITS_LABEL_PREVIEW_Y, 0, 0), OUTFITS_LABEL_PREVIEW_TITLE);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OUTFITS_LABEL_FILTER_X, OUTFITS_LABEL_FILTER_Y, 0, 0), OUTFITS_LABEL_FILTER_TITLE);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OUTFITS_LABEL_CONFIGURE_X, OUTFITS_LABEL_CONFIGURE_Y, 0, 0), OUTFITS_LABEL_CONFIGURE_TITLE);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OUTFITS_LABEL_OUTFIT_X, OUTFITS_LABEL_OUTFIT_Y, 0, 0), outfitName, OUTFITS_LABEL_OUTFIT_EVENTID);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(OUTFITS_LABEL_MOUNT_X, OUTFITS_LABEL_MOUNT_Y, 0, 0), mountName, OUTFITS_LABEL_MOUNT_EVENTID);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	newWindow->addChild(newLabel);
	GUI_Button* newButton = new GUI_Button(iRect(OUTFITS_WIDTH - 56, OUTFITS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&outfits_Events, OUTFITS_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(OUTFITS_WIDTH - 109, OUTFITS_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&outfits_Events, OUTFITS_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, OUTFITS_HEIGHT - 40, OUTFITS_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

GUI_Outfit_View::GUI_Outfit_View(iRect boxRect, Uint32 internalID) : m_currentFrame(ThingFrameGroup_Idle)
{
	setRect(boxRect);
	m_internalID = internalID;
	m_ground = g_thingManager.getThingType(ThingCategory_Item, 429);
	m_outfit = g_thingManager.getThingType(ThingCategory_Creature, g_outfitLookType);
	m_mount = (g_outfitMount == 0 ? NULL : g_thingManager.getThingType(ThingCategory_Creature, g_outfitMount));
	resetAnimation();
}

void GUI_Outfit_View::previousDirection()
{
	if(m_direction == DIRECTION_WEST)
		m_direction = DIRECTION_NORTH;
	else
		++m_direction;
}

void GUI_Outfit_View::nextDirection()
{
	if(m_direction == DIRECTION_NORTH)
		m_direction = DIRECTION_WEST;
	else
		--m_direction;
}

void GUI_Outfit_View::refresh()
{
	m_outfit = g_thingManager.getThingType(ThingCategory_Creature, g_outfitLookType);
	m_mount = (g_outfitMount == 0 ? NULL : g_thingManager.getThingType(ThingCategory_Creature, g_outfitMount));

	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_OUTFITS);
	if(pWindow)
	{
		GUI_CheckBox* pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(OUTFITS_CHECKBOX_OUTFIT_EVENTID));
		if(pCheckBox)
			m_showOutfit = pCheckBox->isChecked();

		pCheckBox = SDL_static_cast(GUI_CheckBox*, pWindow->getChild(OUTFITS_CHECKBOX_MOUNT_EVENTID));
		if(pCheckBox)
			m_showMount = pCheckBox->isChecked();
	}

	resetAnimation();
}

void GUI_Outfit_View::startMovement()
{
	m_walking = true;
	m_outfitAnim = 0;
	m_mountAnim = 0;
	if(!g_game.hasGameFeature(GAME_FEATURE_FRAMEGROUPS) || (m_outfit && m_outfit->hasFlag(ThingAttribute_AnimateAlways)))
		m_currentFrame = ThingFrameGroup_Idle;
	else
		m_currentFrame = ThingFrameGroup_Moving;

	m_walkStartTime = g_frameTime;
	resetAnimation();
}

void GUI_Outfit_View::stopMovement()
{
	m_walking = false;
	m_outfitAnim = 0;
	m_mountAnim = 0;
	m_currentFrame = ThingFrameGroup_Idle;
	resetAnimation();
}

void GUI_Outfit_View::resetAnimation()
{
	if(m_outfit && m_outfit->m_frameGroup[m_currentFrame].m_animator)
		m_outfit->m_frameGroup[m_currentFrame].m_animator->resetAnimation(m_outfitAnimation[m_currentFrame]);
	
	if(m_mount && m_mount->m_frameGroup[m_currentFrame].m_animator)
		m_mount->m_frameGroup[m_currentFrame].m_animator->resetAnimation(m_mountAnimation[m_currentFrame]);
}

void GUI_Outfit_View::updateMovement()
{
	static const Uint32 walkTime = 500;//500ms per tile
	if(m_walking)
	{
		float walkTicks = (walkTime * 0.03125f);
		m_walkedPixels = SDL_static_cast(Sint32, (g_frameTime - m_walkStartTime) / walkTicks);
	}
	if(m_outfit)
	{
		if(m_outfit->m_frameGroup[m_currentFrame].m_animator)
		{
			//Calculate with new animation
			m_outfitAnim = SDL_static_cast(Uint8, m_outfit->m_frameGroup[m_currentFrame].m_animator->getPhase(m_outfitAnimation[m_currentFrame], (m_currentFrame == ThingFrameGroup_Idle ? 0 : walkTime)));
		}
		else
		{
			//Calculate with old animation
			if(m_outfit->hasFlag(ThingAttribute_AnimateAlways))
				m_outfitAnim = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (g_frameTime / CREATURE_TICKS_PER_FRAME)), m_outfit->m_frameGroup[m_currentFrame].m_animCount);
			else if(m_walking)
			{
				Uint8 animCount = m_outfit->m_frameGroup[m_currentFrame].m_animCount;
				if(animCount > 1)
					m_outfitAnim = (SDL_static_cast(Uint8, (m_walkedPixels % 32) / 8) % (animCount - 1)) + 1;
				else
					m_outfitAnim = 0;
			}
			else
				m_outfitAnim = 0;
		}
	}
	if(m_mount)
	{
		if(m_mount->m_frameGroup[m_currentFrame].m_animator)
		{
			//Calculate with new animation
			m_mountAnim = SDL_static_cast(Uint8, m_mount->m_frameGroup[m_currentFrame].m_animator->getPhase(m_mountAnimation[m_currentFrame], (m_currentFrame == ThingFrameGroup_Idle ? 0 : walkTime)));
		}
		else
		{
			//Calculate with old animation
			if(m_mount->hasFlag(ThingAttribute_AnimateAlways))
				m_mountAnim = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (g_frameTime / CREATURE_TICKS_PER_FRAME)), m_mount->m_frameGroup[m_currentFrame].m_animCount);
			else if(m_walking)
			{
				Uint8 animCount = m_mount->m_frameGroup[m_currentFrame].m_animCount;
				if(animCount > 1)
					m_mountAnim = (SDL_static_cast(Uint8, (m_walkedPixels % 32) / 8) % (animCount - 1)) + 1;
				else
					m_mountAnim = 0;
			}
			else
				m_mountAnim = 0;
		}
	}
}

Sint32 GUI_Outfit_View::getOffsetX()
{
	if(!m_walking) return 0;
	switch(m_direction)
	{
		case DIRECTION_NORTH:
			return 0;
		case DIRECTION_EAST:
			return (m_walkedPixels % 32);
		case DIRECTION_SOUTH:
			return 0;
		case DIRECTION_WEST:
			return -(m_walkedPixels % 32);
		default: return 0;
	}
}

Sint32 GUI_Outfit_View::getOffsetY()
{
	if(!m_walking) return 0;
	switch(m_direction)
	{
		case DIRECTION_NORTH:
			return -(m_walkedPixels % 32);
		case DIRECTION_EAST:
			return 0;
		case DIRECTION_SOUTH:
			return (m_walkedPixels % 32);
		case DIRECTION_WEST:
			return 0;
		default: return 0;
	}
}

void GUI_Outfit_View::render()
{
	Uint32 outfitColor = (g_outfitColors[3] << 24) | (g_outfitColors[2] << 16) | (g_outfitColors[1] << 8) | (g_outfitColors[0]);
	updateMovement();

	auto& renderer = g_engine.getRender();
	renderer->beginGameScene();
	Sint32 startX = -getOffsetX();
	Sint32 startY = -getOffsetY();
	Sint32 posXc;
	Sint32 posYc = -32 + startY;
	for(Uint8 y = 0; y < 5; ++y)
	{
		posXc = -32 + startX;
		for(Uint8 x = 0; x < 7; ++x)
		{
			Uint8 pDiff = SDL_static_cast(Uint8, (m_walkedPixels / 32));
			Uint8 xPattern = UTIL_safeMod<Uint8>((m_direction == DIRECTION_EAST ? (x + pDiff) : m_direction == DIRECTION_WEST ? (x - pDiff) : x), m_ground->m_frameGroup[ThingFrameGroup_Default].m_patternX);
			Uint8 yPattern = UTIL_safeMod<Uint8>((m_direction == DIRECTION_SOUTH ? (y + pDiff) : m_direction == DIRECTION_NORTH ? (y - pDiff) : y), m_ground->m_frameGroup[ThingFrameGroup_Default].m_patternY);
			Uint32 sprite = m_ground->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, 0, 0);
			if(sprite != 0)
				renderer->drawSprite(sprite, posXc, posYc);

			posXc += 32;
		}
		posYc += 32;
	}

	Uint8 zPattern = 0;
	startX = 64;
	startY = 32;
	if(m_mount && m_showMount)
	{
		startX -= m_mount->m_displacement[0];
		startY -= m_mount->m_displacement[1];
	}
	else if(m_outfit && m_showOutfit)
	{
		startX -= m_outfit->m_displacement[0];
		startY -= m_outfit->m_displacement[1];
	}

	if(m_mount && m_showMount)
	{
		posYc = startY;
		for(Uint8 cy = 0; cy < m_mount->m_frameGroup[m_currentFrame].m_height; ++cy)
		{
			posXc = startX;
			for(Uint8 cx = 0; cx < m_mount->m_frameGroup[m_currentFrame].m_width; ++cx)
			{
				Uint32 sprite = m_mount->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 0, m_direction, 0, 0, m_mountAnim);
				if(sprite != 0)
					renderer->drawSprite(sprite, posXc, posYc);

				posXc -= 32;
			}
			posYc -= 32;
		}
		if(m_outfit)
			zPattern = UTIL_min<Uint8>(1, m_outfit->m_frameGroup[m_currentFrame].m_patternZ - 1);
	}
	if(m_outfit && m_showOutfit)
	{
		if(m_outfit->m_frameGroup[m_currentFrame].m_layers > 1)
		{
			posYc = startY;
			for(Uint8 cy = 0; cy < m_outfit->m_frameGroup[m_currentFrame].m_height; ++cy)
			{
				posXc = startX;
				for(Uint8 cx = 0; cx < m_outfit->m_frameGroup[m_currentFrame].m_width; ++cx)
				{
					Uint32 sprite = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 0, m_direction, 0, zPattern, m_outfitAnim);
					Uint32 spriteMask = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 1, m_direction, 0, zPattern, m_outfitAnim);
					if(sprite != 0)
					{
						if(spriteMask != 0)
							renderer->drawSpriteMask(sprite, spriteMask, posXc, posYc, outfitColor);
						else
							renderer->drawSprite(sprite, posXc, posYc);
					}
					if(m_outfit->m_frameGroup[m_currentFrame].m_patternY > 1)
					{
						if(g_outfitAddons & 1)//First addon
						{
							sprite = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 0, m_direction, 1, zPattern, m_outfitAnim);
							spriteMask = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 1, m_direction, 1, zPattern, m_outfitAnim);
							if(sprite != 0)
							{
								if(spriteMask != 0)
									renderer->drawSpriteMask(sprite, spriteMask, posXc, posYc, outfitColor);
								else
									renderer->drawSprite(sprite, posXc, posYc);
							}
						}
						if(g_outfitAddons & 2)//Second addon
						{
							sprite = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 0, m_direction, 2, zPattern, m_outfitAnim);
							spriteMask = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 1, m_direction, 2, zPattern, m_outfitAnim);
							if(sprite != 0)
							{
								if(spriteMask != 0)
									renderer->drawSpriteMask(sprite, spriteMask, posXc, posYc, outfitColor);
								else
									renderer->drawSprite(sprite, posXc, posYc);
							}
						}
					}
					posXc -= 32;
				}
				posYc -= 32;
			}
		}
		else
		{
			posYc = startY;
			for(Uint8 cy = 0; cy < m_outfit->m_frameGroup[m_currentFrame].m_height; ++cy)
			{
				posXc = startX;
				for(Uint8 cx = 0; cx < m_outfit->m_frameGroup[m_currentFrame].m_width; ++cx)
				{
					Uint32 sprite = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 0, m_direction, 0, zPattern, m_outfitAnim);
					if(sprite != 0)
						renderer->drawSprite(sprite, posXc, posYc);

					if(m_outfit->m_frameGroup[m_currentFrame].m_patternY > 1)
					{
						if(g_outfitAddons & 1)//First addon
						{
							sprite = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 0, m_direction, 1, zPattern, m_outfitAnim);
							if(sprite != 0)
								renderer->drawSprite(sprite, posXc, posYc);
						}
						if(g_outfitAddons & 2)//Second addon
						{
							sprite = m_outfit->getSprite(SDL_static_cast(ThingFrameGroup, m_currentFrame), cx, cy, 0, m_direction, 2, zPattern, m_outfitAnim);
							if(sprite != 0)
								renderer->drawSprite(sprite, posXc, posYc);
						}
					}
					posXc -= 32;
				}
				posYc -= 32;
			}
		}
	}
	renderer->endGameScene();
	renderer->drawGameScene(16, 0, 128, 72, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
}

GUI_Outfit_Colors::GUI_Outfit_Colors(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_Outfit_Colors::onMouseMove(Sint32 x, Sint32 y, bool)
{
	if(!m_Pressed)
		return;

	iRect rect = iRect(m_tRect.x1 + 4, m_tRect.y1 + 20, 247, 91);
	if(rect.isPointInside(x, y))
	{
		Sint32 xFactor = (x - m_tRect.x1 - 4) / 13;
		Sint32 yFactor = (y - m_tRect.y1 - 20) / 13;
		Sint32 cFactor = yFactor * 19 + xFactor;
		if(m_hoverColor[1] == cFactor)
			m_hoverColor[0] = cFactor;
		else
			m_hoverColor[0] = -1;
		return;
	}
}

void GUI_Outfit_Colors::onLMouseDown(Sint32 x, Sint32 y)
{
	m_Pressed = true;

	iRect rect = iRect(m_tRect.x1 + 4, m_tRect.y1 + 20, 247, 91);
	if(rect.isPointInside(x, y))
	{
		Sint32 xFactor = (x - m_tRect.x1 - 4) / 13;
		Sint32 yFactor = (y - m_tRect.y1 - 20) / 13;
		Sint32 cFactor = yFactor * 19 + xFactor;
		m_hoverColor[0] = m_hoverColor[1] = cFactor;
		return;
	}
	rect = iRect(m_tRect.x1, m_tRect.y1, 63, 18);
	if(rect.isPointInside(x, y))
	{
		m_selected = 0;
		return;
	}
	rect = iRect(m_tRect.x1 + 63, m_tRect.y1, 63, 18);
	if(rect.isPointInside(x, y))
	{
		m_selected = 1;
		return;
	}
	rect = iRect(m_tRect.x1 + 126, m_tRect.y1, 63, 18);
	if(rect.isPointInside(x, y))
	{
		m_selected = 2;
		return;
	}
	rect = iRect(m_tRect.x1 + 189, m_tRect.y1, 64, 18);
	if(rect.isPointInside(x, y))
	{
		m_selected = 3;
		return;
	}
}

void GUI_Outfit_Colors::onLMouseUp(Sint32 x, Sint32 y)
{
	if(!m_Pressed)
		return;
	m_Pressed = false;

	iRect rect = iRect(m_tRect.x1 + 4, m_tRect.y1 + 20, 247, 91);
	if(rect.isPointInside(x, y))
	{
		Sint32 xFactor = (x - m_tRect.x1 - 4) / 13;
		Sint32 yFactor = (y - m_tRect.y1 - 20) / 13;
		Sint32 cFactor = yFactor * 19 + xFactor;
		if(m_hoverColor[0] == cFactor)
			g_outfitColors[m_selected] = SDL_static_cast(Uint8, cFactor);
	}
	m_hoverColor[0] = m_hoverColor[1] = -1;
}

void GUI_Outfit_Colors::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_EXTRA_BORDER_X, GUI_UI_ICON_EXTRA_BORDER_Y, m_tRect.x1 + m_tRect.x2 - 2, m_tRect.y1 + 16, GUI_UI_ICON_EXTRA_BORDER_W, GUI_UI_ICON_EXTRA_BORDER_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_EXTRA_BORDER_X, GUI_UI_ICON_EXTRA_BORDER_Y, m_tRect.x1, m_tRect.y1 + m_tRect.y2 - 2, GUI_UI_ICON_EXTRA_BORDER_W, GUI_UI_ICON_EXTRA_BORDER_H);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_X, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_Y, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_W, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_H, m_tRect.x1, m_tRect.y1 + 16, m_tRect.x2 - 2, 2);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_X, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_Y, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_W, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_H, m_tRect.x1, m_tRect.y1 + 18, 2, m_tRect.y2 - 20);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_DARK_X, GUI_UI_ICON_HORIZONTAL_LINE_DARK_Y, GUI_UI_ICON_HORIZONTAL_LINE_DARK_W, GUI_UI_ICON_HORIZONTAL_LINE_DARK_H, m_tRect.x1 + 2, m_tRect.y1 + m_tRect.y2 - 2, m_tRect.x2 - 2, 2);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_DARK_X, GUI_UI_ICON_VERTICAL_LINE_DARK_Y, GUI_UI_ICON_VERTICAL_LINE_DARK_W, GUI_UI_ICON_VERTICAL_LINE_DARK_H, m_tRect.x1 + m_tRect.x2 - 2, m_tRect.y1 + 18, 2, m_tRect.y2 - 20);

	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1, m_tRect.y1, 62, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X + GUI_UI_ICON_UNACTIVE_CHANNEL_W - 2, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1 + 62, m_tRect.y1, 2, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X + 1, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1 + 64, m_tRect.y1, 61, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X + GUI_UI_ICON_UNACTIVE_CHANNEL_W - 2, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1 + 125, m_tRect.y1, 2, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X + 1, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1 + 127, m_tRect.y1, 61, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X + GUI_UI_ICON_UNACTIVE_CHANNEL_W - 2, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1 + 188, m_tRect.y1, 2, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X + 1, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1 + 190, m_tRect.y1, 61, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_UNACTIVE_CHANNEL_X + GUI_UI_ICON_UNACTIVE_CHANNEL_W - 2, GUI_UI_ICON_UNACTIVE_CHANNEL_Y, m_tRect.x1 + 251, m_tRect.y1, 2, GUI_UI_ICON_UNACTIVE_CHANNEL_H);
	switch(m_selected)
	{
		case 1:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1 + 63, m_tRect.y1, 62, GUI_UI_ICON_ACTIVE_CHANNEL_H);
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X + GUI_UI_ICON_ACTIVE_CHANNEL_W - 2, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1 + 125, m_tRect.y1, 2, GUI_UI_ICON_ACTIVE_CHANNEL_H);
		}
		break;
		case 2:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1 + 126, m_tRect.y1, 62, GUI_UI_ICON_ACTIVE_CHANNEL_H);
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X + GUI_UI_ICON_ACTIVE_CHANNEL_W - 2, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1 + 188, m_tRect.y1, 2, GUI_UI_ICON_ACTIVE_CHANNEL_H);
		}
		break;
		case 3:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1 + 189, m_tRect.y1, 62, GUI_UI_ICON_ACTIVE_CHANNEL_H);
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X + GUI_UI_ICON_ACTIVE_CHANNEL_W - 2, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1 + 251, m_tRect.y1, 2, GUI_UI_ICON_ACTIVE_CHANNEL_H);
		}
		break;
		default:
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1, m_tRect.y1, 62, GUI_UI_ICON_ACTIVE_CHANNEL_H);
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_ACTIVE_CHANNEL_X + GUI_UI_ICON_ACTIVE_CHANNEL_W - 2, GUI_UI_ICON_ACTIVE_CHANNEL_Y, m_tRect.x1 + 62, m_tRect.y1, 2, GUI_UI_ICON_ACTIVE_CHANNEL_H);
		}
		break;
	}
	g_engine.drawFont(CLIENT_FONT_SMALL, m_tRect.x1 + 22, m_tRect.y1 + 5, "Head", 255, 255, 255, CLIENT_FONT_ALIGN_LEFT);
	g_engine.drawFont(CLIENT_FONT_SMALL, m_tRect.x1 + 78, m_tRect.y1 + 5, "Primary", 255, 255, 255, CLIENT_FONT_ALIGN_LEFT);
	g_engine.drawFont(CLIENT_FONT_SMALL, m_tRect.x1 + 136, m_tRect.y1 + 5, "Secondary", 255, 255, 255, CLIENT_FONT_ALIGN_LEFT);
	g_engine.drawFont(CLIENT_FONT_SMALL, m_tRect.x1 + 208, m_tRect.y1 + 5, "Detail", 255, 255, 255, CLIENT_FONT_ALIGN_LEFT);

	Uint8 hoverC = SDL_static_cast(Uint8, m_hoverColor[0]);
	Uint8 c = 0;
	Sint32 posY = m_tRect.y1 + 20;
	for(Sint32 i = 0; i < 7; ++i)
	{
		Sint32 posX = m_tRect.x1 + 4;
		for(Sint32 j = 0; j < 19; ++j)
		{
			Uint8 red, green, blue;
			getOutfitColorRGB(c, red, green, blue);
			renderer->fillRectangle(posX + 2, posY + 2, 8, 8, red, green, blue, 255);
			if(g_outfitColors[m_selected] == c)
			{
				renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_COLOR_BOX_DOWN_X, GUI_UI_ICON_COLOR_BOX_DOWN_Y, posX, posY, GUI_UI_ICON_COLOR_BOX_DOWN_W, GUI_UI_ICON_COLOR_BOX_DOWN_H);
				renderer->drawRectangle(posX - 1, posY - 1, 14, 14, 1, 255, 255, 255, 255);
			}
			else
				renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_COLOR_BOX_UP_X, (hoverC == c ? GUI_UI_ICON_COLOR_BOX_DOWN_Y : GUI_UI_ICON_COLOR_BOX_UP_Y), posX, posY, GUI_UI_ICON_COLOR_BOX_UP_W, GUI_UI_ICON_COLOR_BOX_UP_H);
			
			++c;
			posX += 13;
		}
		posY += 13;
	}
}
