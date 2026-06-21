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
#include "../GUI_Elements/GUI_Icon.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_TextBox.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_StaticImage.h"
#include "../GUI_Elements/GUI_Grouper.h"
#include "../GUI_Elements/GUI_ContextMenu.h"
#include "../automap.h"
#include "../game.h"
#include "Minimap.h"

#define MINIMAP_ZOOMIN_EVENTID 1000
#define MINIMAP_ZOOMOUT_EVENTID 1001
#define MINIMAP_FLOORUP_EVENTID 1002
#define MINIMAP_FLOORDOWN_EVENTID 1003
#define MINIMAP_SCROLLNORTH_EVENTID 1004
#define MINIMAP_SCROLLSOUTH_EVENTID 1005
#define MINIMAP_SCROLLEAST_EVENTID 1006
#define MINIMAP_SCROLLWEST_EVENTID 1007
#define MINIMAP_SCROLLNORTHWEST_EVENTID 1008
#define MINIMAP_SCROLLNORTHEAST_EVENTID 1009
#define MINIMAP_SCROLLSOUTHWEST_EVENTID 1010
#define MINIMAP_SCROLLSOUTHEAST_EVENTID 1011
#define MINIMAP_CENTER_EVENTID 1012
#define MINIMAP_EDITMARK_EVENTID 2000
#define MINIMAP_DELETEMARK_EVENTID 2001
#define MINIMAP_SETMARK_EVENTID 2002

#define EDIT_MARK_TITLE "Edit Mark"
#define EDIT_MARK_WIDTH 286
#define EDIT_MARK_HEIGHT 188
#define EDIT_MARK_CANCEL_EVENTID 1500
#define EDIT_MARK_OK_EVENTID 1501
#define EDIT_MARK_DESCRIPTION_LABEL_TEXT "Enter a short description:"
#define EDIT_MARK_DESCRIPTION_LABEL_X 18
#define EDIT_MARK_DESCRIPTION_LABEL_Y 34
#define EDIT_MARK_TYPE_LABEL_TEXT "Select a type:"
#define EDIT_MARK_TYPE_LABEL_X 18
#define EDIT_MARK_TYPE_LABEL_Y 80
#define EDIT_MARK_TEXTBOX_X 18
#define EDIT_MARK_TEXTBOX_Y 47
#define EDIT_MARK_TEXTBOX_W 250
#define EDIT_MARK_TEXTBOX_H 16
#define EDIT_MARK_TEXTBOX_EVENTID 3000

extern Engine g_engine;
extern Automap g_automap;
extern Game g_game;

std::string g_minimapDescription;
Uint8 g_minimapFlag;

void UTIL_createEditMark();
void minimap_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case MINIMAP_ZOOMIN_EVENTID: g_game.minimapZoomIn(); break;
		case MINIMAP_ZOOMOUT_EVENTID: g_game.minimapZoomOut(); break;
		case MINIMAP_FLOORUP_EVENTID: g_game.minimapFloorUp(); break;
		case MINIMAP_FLOORDOWN_EVENTID: g_game.minimapFloorDown(); break;
		case MINIMAP_SCROLLNORTH_EVENTID: g_game.minimapScrollNorth(); break;
		case MINIMAP_SCROLLSOUTH_EVENTID: g_game.minimapScrollSouth(); break;
		case MINIMAP_SCROLLEAST_EVENTID: g_game.minimapScrollEast(); break;
		case MINIMAP_SCROLLWEST_EVENTID: g_game.minimapScrollWest(); break;
		case MINIMAP_SCROLLNORTHWEST_EVENTID: g_game.minimapScrollNorthWest(); break;
		case MINIMAP_SCROLLNORTHEAST_EVENTID: g_game.minimapScrollNorthEast(); break;
		case MINIMAP_SCROLLSOUTHWEST_EVENTID: g_game.minimapScrollSouthWest(); break;
		case MINIMAP_SCROLLSOUTHEAST_EVENTID: g_game.minimapScrollSouthEast(); break;
		case MINIMAP_CENTER_EVENTID: g_game.minimapCenter(); break;

		case MINIMAP_EDITMARK_EVENTID: UTIL_createEditMark(); break;
		case MINIMAP_DELETEMARK_EVENTID:
		{
			ClientActionData& mapData = g_engine.getActionData(CLIENT_ACTION_SECOND);
			g_automap.setMapMark(mapData.posX, mapData.posY, mapData.posZ, MAPMARK_BAG, "", true);
		}
		break;
		case MINIMAP_SETMARK_EVENTID: UTIL_createEditMark(); break;

		case EDIT_MARK_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EDITMARK)
				g_engine.removeWindow(pWindow);
		}
		break;
		case EDIT_MARK_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_EDITMARK)
			{
				g_engine.removeWindow(pWindow);
				GUI_TextBox* pTextBox = SDL_static_cast(GUI_TextBox*, pWindow->getChild(EDIT_MARK_TEXTBOX_EVENTID));
				if(pTextBox)
				{
					ClientActionData& mapData = g_engine.getActionData(CLIENT_ACTION_SECOND);
					g_automap.setMapMark(mapData.posX, mapData.posY, mapData.posZ, g_minimapFlag, pTextBox->getActualText());
				}
			}
		}
		break;
		default: break;
	}
}

void UTIL_createEditMark()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_EDITMARK);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, EDIT_MARK_WIDTH, EDIT_MARK_HEIGHT), EDIT_MARK_TITLE, GUI_WINDOW_EDITMARK);
	GUI_Label* newLabel = new GUI_Label(iRect(EDIT_MARK_DESCRIPTION_LABEL_X, EDIT_MARK_DESCRIPTION_LABEL_Y, 0, 0), EDIT_MARK_DESCRIPTION_LABEL_TEXT);
	newWindow->addChild(newLabel);
	newLabel = new GUI_Label(iRect(EDIT_MARK_TYPE_LABEL_X, EDIT_MARK_TYPE_LABEL_Y, 0, 0), EDIT_MARK_TYPE_LABEL_TEXT);
	newWindow->addChild(newLabel);
	GUI_TextBox* newTextBox = new GUI_TextBox(iRect(EDIT_MARK_TEXTBOX_X, EDIT_MARK_TEXTBOX_Y, EDIT_MARK_TEXTBOX_W, EDIT_MARK_TEXTBOX_H), g_minimapDescription, EDIT_MARK_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(100);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	for(Uint8 i = MAPMARK_TICK, end = MAPMARK_GREENSOUTH; i <= end; ++i)
	{
		Uint8 flag = i;
		if(flag == MAPMARK_REDNORTH)
			flag = 7;
		else if(flag == MAPMARK_REDEAST)
			flag = 8;
		else if(flag == MAPMARK_GREENNORTH)
			flag = 9;
		else if(flag == MAPMARK_REDSOUTH)
			flag = 17;
		else if(flag == MAPMARK_REDWEST)
			flag = 18;
		else if(flag == MAPMARK_GREENSOUTH)
			flag = 19;
		else if(flag >= MAPMARK_SHOVEL)
			flag += 3;

		GUI_MinimapFlag* newMinimapFlag = new GUI_MinimapFlag(iRect((SDL_static_cast(Sint32, flag % 10) * 20) + 18, (SDL_static_cast(Sint32, flag / 10) * 20) + 93, GUI_UI_ICON_TEXTURED_UP_W, GUI_UI_ICON_TEXTURED_UP_H), i);
		newMinimapFlag->startEvents();
		newWindow->addChild(newMinimapFlag);
	}
	GUI_Button* newButton = new GUI_Button(iRect(EDIT_MARK_WIDTH - 56, EDIT_MARK_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&minimap_Events, EDIT_MARK_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(EDIT_MARK_WIDTH - 109, EDIT_MARK_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&minimap_Events, EDIT_MARK_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, EDIT_MARK_HEIGHT - 40, EDIT_MARK_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

void UTIL_createMinimapPanel()
{
	bool newWindow;
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_MINIMAP);
	if(pPanel)
	{
		newWindow = false;
		pPanel->clearChilds();
	}
	else
	{
		newWindow = true;
		pPanel = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, 117), false, GUI_PANEL_WINDOW_MINIMAP);
	}

	GUI_Grouper* newGrouper = new GUI_Grouper(iRect(5, 1, 114, 114));
	pPanel->addChild(newGrouper);
	GUI_Minimap* newMinimap = new GUI_Minimap(iRect(6, 2, 112, 112));
	newMinimap->startEvents();
	pPanel->addChild(newMinimap);
	GUI_MinimapTime* newMinimapTime = new GUI_MinimapTime(iRect(130, 10, 31, GUI_UI_ICON_MAP_TIME_H));
	pPanel->addChild(newMinimapTime);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(124, 4, GUI_UI_ICON_MAP_ROSE_W, GUI_UI_ICON_MAP_ROSE_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSE_X, GUI_UI_ICON_MAP_ROSE_Y);
	pPanel->addChild(newImage);
	GUI_Button* newButton = new GUI_Button(iRect(124, 92, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Centre");
	newButton->setButtonEventCallback(&minimap_Events, MINIMAP_CENTER_EVENTID);
	newButton->startEvents();
	pPanel->addChild(newButton);
	GUI_Icon* newIcon = new GUI_Icon(iRect(139, 4, GUI_UI_ICON_MAP_ROSEN_UP_W, GUI_UI_ICON_MAP_ROSEN_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSEN_UP_X, GUI_UI_ICON_MAP_ROSEN_UP_Y, GUI_UI_ICON_MAP_ROSEN_DOWN_X, GUI_UI_ICON_MAP_ROSEN_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLNORTH_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(139, 40, GUI_UI_ICON_MAP_ROSES_UP_W, GUI_UI_ICON_MAP_ROSES_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSES_UP_X, GUI_UI_ICON_MAP_ROSES_UP_Y, GUI_UI_ICON_MAP_ROSES_DOWN_X, GUI_UI_ICON_MAP_ROSES_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLSOUTH_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(124, 19, GUI_UI_ICON_MAP_ROSEW_UP_W, GUI_UI_ICON_MAP_ROSEW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSEW_UP_X, GUI_UI_ICON_MAP_ROSEW_UP_Y, GUI_UI_ICON_MAP_ROSEW_DOWN_X, GUI_UI_ICON_MAP_ROSEW_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLWEST_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(160, 19, GUI_UI_ICON_MAP_ROSEE_UP_W, GUI_UI_ICON_MAP_ROSEE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSEE_UP_X, GUI_UI_ICON_MAP_ROSEE_UP_Y, GUI_UI_ICON_MAP_ROSEE_DOWN_X, GUI_UI_ICON_MAP_ROSEE_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLEAST_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(126, 6, GUI_UI_ICON_MAP_ROSENW_UP_W, GUI_UI_ICON_MAP_ROSENW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSENW_UP_X, GUI_UI_ICON_MAP_ROSENW_UP_Y, GUI_UI_ICON_MAP_ROSENW_DOWN_X, GUI_UI_ICON_MAP_ROSENW_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLNORTHWEST_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(153, 6, GUI_UI_ICON_MAP_ROSENE_UP_W, GUI_UI_ICON_MAP_ROSENE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSENE_UP_X, GUI_UI_ICON_MAP_ROSENE_UP_Y, GUI_UI_ICON_MAP_ROSENE_DOWN_X, GUI_UI_ICON_MAP_ROSENE_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLNORTHEAST_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(126, 33, GUI_UI_ICON_MAP_ROSESW_UP_W, GUI_UI_ICON_MAP_ROSESW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSESW_UP_X, GUI_UI_ICON_MAP_ROSESW_UP_Y, GUI_UI_ICON_MAP_ROSESW_DOWN_X, GUI_UI_ICON_MAP_ROSESW_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLSOUTHWEST_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(153, 33, GUI_UI_ICON_MAP_ROSESE_UP_W, GUI_UI_ICON_MAP_ROSESE_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ROSESE_UP_X, GUI_UI_ICON_MAP_ROSESE_UP_Y, GUI_UI_ICON_MAP_ROSESE_DOWN_X, GUI_UI_ICON_MAP_ROSESE_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_SCROLLSOUTHEAST_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(124, 71, GUI_UI_ICON_MAP_ZOOMIN_UP_W, GUI_UI_ICON_MAP_ZOOMIN_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ZOOMIN_UP_X, GUI_UI_ICON_MAP_ZOOMIN_UP_Y, GUI_UI_ICON_MAP_ZOOMIN_DOWN_X, GUI_UI_ICON_MAP_ZOOMIN_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_ZOOMIN_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(124, 50, GUI_UI_ICON_MAP_ZOOMOUT_UP_W, GUI_UI_ICON_MAP_ZOOMOUT_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_ZOOMOUT_UP_X, GUI_UI_ICON_MAP_ZOOMOUT_UP_Y, GUI_UI_ICON_MAP_ZOOMOUT_DOWN_X, GUI_UI_ICON_MAP_ZOOMOUT_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_ZOOMOUT_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(147, 50, GUI_UI_ICON_MAP_MOVEUP_UP_W, GUI_UI_ICON_MAP_MOVEUP_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_MOVEUP_UP_X, GUI_UI_ICON_MAP_MOVEUP_UP_Y, GUI_UI_ICON_MAP_MOVEUP_DOWN_X, GUI_UI_ICON_MAP_MOVEUP_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_FLOORUP_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(147, 71, GUI_UI_ICON_MAP_MOVEDOWN_UP_W, GUI_UI_ICON_MAP_MOVEDOWN_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MAP_MOVEDOWN_UP_X, GUI_UI_ICON_MAP_MOVEDOWN_UP_Y, GUI_UI_ICON_MAP_MOVEDOWN_DOWN_X, GUI_UI_ICON_MAP_MOVEDOWN_DOWN_Y);
	newIcon->setButtonEventCallback(&minimap_Events, MINIMAP_FLOORDOWN_EVENTID);
	newIcon->startEvents();
	pPanel->addChild(newIcon);
	if(newWindow)
		g_engine.addToPanel(pPanel, GUI_PANEL_MAIN);
	else
		pPanel->checkPanels();
}

GUI_MinimapTime::GUI_MinimapTime(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_MinimapTime::render()
{
	auto& renderer = g_engine.getRender();
	Sint32 time = SDL_static_cast(Sint32, g_game.getGameTime()) / 10;
	if(time >= 58)
	{
		time -= 58;
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_MAP_TIME_X + time, GUI_UI_ICON_MAP_TIME_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
	}
	else if(time >= 8)
	{
		time = SDL_static_cast(Sint32, (time - 8) * 0.6f);
		time += 94;
		Sint32 width = 124 - time;
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_MAP_TIME_X + time, GUI_UI_ICON_MAP_TIME_Y, m_tRect.x1, m_tRect.y1, width, m_tRect.y2);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_MAP_TIME_X, GUI_UI_ICON_MAP_TIME_Y, m_tRect.x1 + width, m_tRect.y1, 31 - width, m_tRect.y2);
	}
	else
	{
		time += 86;
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_MAP_TIME_X + time, GUI_UI_ICON_MAP_TIME_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
	}
}

GUI_MinimapFlag::GUI_MinimapFlag(iRect boxRect, Uint8 type, Uint32 internalID) : m_type(type)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_MinimapFlag::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
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

void GUI_MinimapFlag::onLMouseDown(Sint32, Sint32)
{
	m_pressed = 1;
}

void GUI_MinimapFlag::onLMouseUp(Sint32, Sint32)
{
	Uint8 pressed = m_pressed;
	if(pressed > 0)
	{
		m_pressed = 0;
		if(pressed == 1)
			g_minimapFlag = m_type;
	}
}

void GUI_MinimapFlag::render()
{
	bool pressed = (m_pressed == 1 ? true : false);
	pressed = (m_type == g_minimapFlag ? true : pressed);

	auto& renderer = g_engine.getRender();
	if(pressed)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_TEXTURED_DOWN_X, GUI_UI_ICON_TEXTURED_DOWN_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
		AutomapArea::renderMark(m_type, m_tRect.x1 + 3, m_tRect.y1 + 3);
	}
	else
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_TEXTURED_UP_X, GUI_UI_ICON_TEXTURED_UP_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
		AutomapArea::renderMark(m_type, m_tRect.x1 + 2, m_tRect.y1 + 2);
	}
}

GUI_Minimap::GUI_Minimap(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_Minimap::onLMouseDown(Sint32 x, Sint32 y)
{
	m_bMouseAutowalk = true;
	m_bMouseDragging = true;
	m_mouseEvent.x1 = x;
	m_mouseEvent.y1 = y;
}

void GUI_Minimap::onLMouseUp(Sint32 x, Sint32 y)
{
	m_bMouseDragging = false;
	if(m_bMouseAutowalk && m_tRect.isPointInside(x, y))
	{
		MapMark* mapMark;
		Position autoWalkPosition = g_automap.getMapDetail(x - m_tRect.x1, y - m_tRect.y1, m_tRect.x2, m_tRect.y2, mapMark);
		g_engine.notePlayerMovementAction(900);
		g_game.startAutoWalk(autoWalkPosition);
	}
}

void GUI_Minimap::onRMouseDown(Sint32, Sint32)
{
	m_haveRMouse = true;
}

void GUI_Minimap::onRMouseUp(Sint32 x, Sint32 y)
{
	if(m_haveRMouse)
	{
		if(m_tRect.isPointInside(x, y))
		{
			MapMark* mapMark;
			Position mapPosition = g_automap.getMapDetail(x - m_tRect.x1, y - m_tRect.y1, m_tRect.x2, m_tRect.y2, mapMark);
			if(mapMark)
				g_engine.setActionData(CLIENT_ACTION_SECOND, 0, 0, mapMark->x, mapMark->y, mapPosition.z, 0);
			else
				g_engine.setActionData(CLIENT_ACTION_SECOND, 0, 0, mapPosition.x, mapPosition.y, mapPosition.z, 0);

			GUI_ContextMenu* newMenu = new GUI_ContextMenu();
			if(mapMark)
			{
				g_minimapDescription.assign(mapMark->text);
				g_minimapFlag = mapMark->type;
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, MINIMAP_EDITMARK_EVENTID, "Edit Mark", "");
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, MINIMAP_DELETEMARK_EVENTID, "Delete Mark", "");
			}
			else
			{
				g_minimapDescription.clear();
				g_minimapFlag = MAPMARK_TICK;
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, MINIMAP_SETMARK_EVENTID, "Set Mark", "");
			}

			newMenu->setEventCallback(&minimap_Events);
			g_engine.showContextMenu(newMenu, x, y);
		}
		m_haveRMouse = false;
	}
}

void GUI_Minimap::onWheel(Sint32, Sint32, bool wheelUP)
{
	if(wheelUP)
		g_game.minimapZoomIn();
	else
		g_game.minimapZoomOut();
}

void GUI_Minimap::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(m_bMouseDragging)
	{
		Sint32 diffX = (x - m_mouseEvent.x1) / g_automap.getZoom();
		Sint32 diffY = (y - m_mouseEvent.y1) / g_automap.getZoom();
		if(diffX != 0 || diffY != 0)
		{
			Position& pos = g_automap.getPosition();
			g_automap.setPosition(Position(SDL_static_cast(Uint16, SDL_static_cast(Sint32, pos.x) - diffX), SDL_static_cast(Uint16, SDL_static_cast(Sint32, pos.y) - diffY), pos.z));

			m_mouseEvent.x1 = x;
			m_mouseEvent.y1 = y;

			//When we move the minimap let's disable autowalk click
			m_bMouseAutowalk = false;
		}
	}

	bool inside = (isInsideParent && m_tRect.isPointInside(x, y));
	if(inside)
	{
		MapMark* mapMark;
		g_automap.getMapDetail(x - m_tRect.x1, y - m_tRect.y1, m_tRect.x2, m_tRect.y2, mapMark);
		if(mapMark && !mapMark->text.empty())
			g_engine.showDescription(x, y, mapMark->text);
	}
}

void GUI_Minimap::render()
{
	g_automap.render(m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
}
