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
#include "../GUI_Elements/GUI_Content.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../game.h"
#include "../container.h"
#include "ContainerWindow.h"
#include "itemUI.h"

#define CONTAINER_MAXIMINI_EVENTID 1000
#define CONTAINER_CLOSE_EVENTID 1001
#define CONTAINER_CLOSED_EVENTID 1002
#define CONTAINER_RESIZE_WIDTH_EVENTID 1003
#define CONTAINER_RESIZE_HEIGHT_EVENTID 1004
#define CONTAINER_EXIT_WINDOW_EVENTID 1005
#define CONTAINER_PARENT_EVENTID 1006
#define CONTAINER_DEPOTSEARCH_EVENTID 1007
#define CONTAINER_CONTAINER_EVENTID 1008
#define CONTAINER_TITLE_EVENTID 1009
#define CONTAINER_PAGE_TEXT_EVENTID 1010
#define CONTAINER_PREV_PAGE_EVENTID 1011
#define CONTAINER_NEXT_PAGE_EVENTID 1012
#define CONTAINER_PAGE_CONTENT_EVENTID 1013

extern Engine g_engine;
extern Game g_game;

Uint32 container_CreateEvent(Uint8 index, Uint32 event)
{
	return SDL_static_cast(Uint32, index) | event << 8;
}

void container_Events(Uint32 event, Sint32 status)
{
	Uint8 index = SDL_static_cast(Uint8, event);
	event >>= 8;
	switch(event)
	{
		case CONTAINER_MAXIMINI_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index));
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

					GUI_ContainerContainer* pContainer = SDL_static_cast(GUI_ContainerContainer*, pPanel->getChild(CONTAINER_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeInvisible();

					GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(CONTAINER_PAGE_CONTENT_EVENTID));
					if(pContent)
						pContent->makeInvisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(CONTAINER_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_X, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_Y);
				}
				else
				{
					Sint32 cachedHeight = pPanel->getCachedHeight();
					parent->tryFreeHeight(cachedHeight - pRect.y2);
					pPanel->setSize(pRect.x2, cachedHeight);
					parent->checkPanels();

					GUI_ContainerContainer* pContainer = SDL_static_cast(GUI_ContainerContainer*, pPanel->getChild(CONTAINER_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeVisible();

					GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(CONTAINER_PAGE_CONTENT_EVENTID));
					if(pContent)
						pContent->makeVisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(CONTAINER_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y);
				}
			}
		}
		break;
		case CONTAINER_CLOSE_EVENTID:
		case CONTAINER_CLOSED_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index));
			if(pPanel)
			{
				if(event == CONTAINER_CLOSE_EVENTID)//If we got closed by server-side we don't need to send this packet
					g_game.sendCloseContainer(index);

				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				parent->removePanel(pPanel);
			}
		}
		break;
		case CONTAINER_RESIZE_HEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index));
			if(pPanel)
			{
				Container* container = g_game.findContainer(index);
				if(container && container->hasPages())
				{
					GUI_ContainerContainer* pContainer = SDL_static_cast(GUI_ContainerContainer*, pPanel->getChild(CONTAINER_CONTAINER_EVENTID));
					if(pContainer)
					{
						iRect cRect = pContainer->getRect();
						cRect.y2 = status - 43;
						pContainer->setRect(cRect);
					}

					GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(CONTAINER_PAGE_CONTENT_EVENTID));
					if(pContent)
					{
						iRect pRect = pPanel->getRect();
						iRect cRect = pContent->getRect();
						cRect.y1 = pRect.y1 + status - 30;
						pContent->setRect(cRect);
					}
				}
				else
				{
					GUI_ContainerContainer* pContainer = SDL_static_cast(GUI_ContainerContainer*, pPanel->getChild(CONTAINER_CONTAINER_EVENTID));
					if(pContainer)
					{
						iRect cRect = pContainer->getRect();
						cRect.y2 = status - 19;
						pContainer->setRect(cRect);
					}
				}
			}
		}
		break;
		case CONTAINER_PARENT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index));
			if(pPanel)
				g_game.sendUpContainer(index);
		}
		break;
		case CONTAINER_PREV_PAGE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index));
			if(pPanel)
			{
				Container* container = g_game.findContainer(index);
				if(container)
				{
					//Just in case let's check if we don't underflow
					if(SDL_static_cast(Uint16, container->getCapacity()) > container->getFirstIndex())
						g_game.sendSeekInContainer(index, 0);
					else
						g_game.sendSeekInContainer(index, container->getFirstIndex() - SDL_static_cast(Uint16, container->getCapacity()));
				}
			}
		}
		break;
		case CONTAINER_NEXT_PAGE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index));
			if(pPanel)
			{
				Container* container = g_game.findContainer(index);
				if(container)
					g_game.sendSeekInContainer(index, container->getFirstIndex() + SDL_static_cast(Uint16, container->getCapacity()));
			}
		}
		break;
		default: break;
	}
}

void UTIL_recreateContainerWindow(Uint8 index, GUI_PanelWindow* pPanel)
{
	Container* container = g_game.findContainer(index);
	if(!container)
	{
		UTIL_SafeEventHandler(&container_Events, container_CreateEvent(index, CONTAINER_CLOSED_EVENTID), 1);
		return;
	}
	if(!pPanel)
		pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index));

	if(pPanel)
	{
		GUI_DynamicLabel* pDynamicLabel = SDL_static_cast(GUI_DynamicLabel*, pPanel->getChild(CONTAINER_TITLE_EVENTID));
		if(pDynamicLabel)
			pDynamicLabel->setName(container->getName());
		else
		{
			GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(19, 2, 90, 14), container->getName(), CONTAINER_TITLE_EVENTID, 144, 144, 144);
			pPanel->addChild(newLabel);
		}

		GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(CONTAINER_PARENT_EVENTID));
		if(pIcon)
		{
			if(!container->hasParent())
				pPanel->removeChild(pIcon);
		}
		else if(container->hasParent())
		{
			GUI_Icon* newIcon = new GUI_Icon(iRect(135, 0, GUI_UI_ICON_PARENT_WINDOW_UP_W, GUI_UI_ICON_PARENT_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_PARENT_WINDOW_UP_X, GUI_UI_ICON_PARENT_WINDOW_UP_Y, GUI_UI_ICON_PARENT_WINDOW_DOWN_X, GUI_UI_ICON_PARENT_WINDOW_DOWN_Y, CONTAINER_PARENT_EVENTID, "Show higher container");
			newIcon->setButtonEventCallback(&container_Events, container_CreateEvent(index, CONTAINER_PARENT_EVENTID));
			newIcon->startEvents();
			pPanel->addChild(newIcon);
		}

		pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(CONTAINER_DEPOTSEARCH_EVENTID));
		if(pIcon)
		{
			if(!container->canUseDepotSearch())
				pPanel->removeChild(pIcon);
		}
		else if(container->canUseDepotSearch())
		{
			GUI_Icon* newIcon = new GUI_Icon(iRect((container->hasParent() ? 119 : 131), 0, GUI_UI_ICON_SEARCH_WINDOW_UP_W, GUI_UI_ICON_SEARCH_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_SEARCH_WINDOW_UP_X, GUI_UI_ICON_SEARCH_WINDOW_UP_Y, GUI_UI_ICON_SEARCH_WINDOW_DOWN_X, GUI_UI_ICON_SEARCH_WINDOW_DOWN_Y, CONTAINER_DEPOTSEARCH_EVENTID, "Search for items stored in your depot, supply stash and your inbox");
			newIcon->setButtonEventCallback(&container_Events, container_CreateEvent(index, CONTAINER_DEPOTSEARCH_EVENTID));
			newIcon->startEvents();
			pPanel->addChild(newIcon);
		}

		GUI_ContainerContainer* pContainer = SDL_static_cast(GUI_ContainerContainer*, pPanel->getChild(CONTAINER_CONTAINER_EVENTID));
		if(pContainer)
		{
			pContainer->clearChilds(false);
			for(size_t i = 0, end = SDL_static_cast(size_t, container->getCapacity()); i != end; ++i)
			{
				GUI_ContainerItem* newContainerItem = new GUI_ContainerItem(iRect(SDL_static_cast(Sint32, ((i % 4) * 37)) + 8, SDL_static_cast(Sint32, ((i / 4) * 37)) - 6, 32, 32), index, i);
				newContainerItem->startEvents();
				pContainer->addChild(newContainerItem, false);
			}

			pPanel->setMinHeight(60 + (container->hasPages() ? 24 : 0));
			pContainer->setAsMaxHeight();
			if(container->hasPages())
				pPanel->setMaxHeight(pPanel->getMaxHeight() + 24);

			pContainer->validateScrollBar();
		}

		if(container->hasPages())
		{
			iRect& panelRect = pPanel->getRect();
			GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(CONTAINER_PAGE_CONTENT_EVENTID));
			if(!pContent)
			{
				pContent = new GUI_Content(iRect(2, panelRect.y2 - 30, 168, 24), CONTAINER_PAGE_CONTENT_EVENTID);
				pContent->startEvents();
				pPanel->addChild(pContent);
			}

			Uint16 currentPage = 1 + (container->getFirstIndex() / SDL_static_cast(Uint16, container->getCapacity()));
			Uint16 totalPages = 1 + ((container->getSize() > 0 ? (container->getSize() - 1) : 0) / SDL_static_cast(Uint16, container->getCapacity()));
			bool hasPrevPage = (currentPage > 1);
			bool hasNextPage = (totalPages > currentPage);

			Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "Page %u of %u", SDL_static_cast(Uint32, currentPage), SDL_static_cast(Uint32, totalPages));
			GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(CONTAINER_PAGE_TEXT_EVENTID));
			if(pLabel)
				pLabel->setName(std::string(g_buffer, SDL_static_cast(size_t, len)));
			else
			{
				GUI_Label* newLabel = new GUI_Label(iRect(88, 9, 0, 0), std::string(g_buffer, SDL_static_cast(size_t, len)), CONTAINER_TITLE_EVENTID, 168, 168, 168);
				newLabel->setFont(CLIENT_FONT_SMALL);
				newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
				pContent->addChild(newLabel);
			}

			pIcon = SDL_static_cast(GUI_Icon*, pContent->getChild(CONTAINER_PREV_PAGE_EVENTID));
			if(pIcon)
			{
				if(!hasPrevPage)
					pContent->removeChild(pIcon);
			}
			else if(hasPrevPage)
			{
				GUI_Icon* newIcon = new GUI_Icon(iRect(6, 3, GUI_UI_ICON_BROWSE_LEFT_UP_W, GUI_UI_ICON_BROWSE_LEFT_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_BROWSE_LEFT_UP_X, GUI_UI_ICON_BROWSE_LEFT_UP_Y, GUI_UI_ICON_BROWSE_LEFT_DOWN_X, GUI_UI_ICON_BROWSE_LEFT_DOWN_Y, CONTAINER_PREV_PAGE_EVENTID);
				newIcon->setButtonEventCallback(&container_Events, container_CreateEvent(index, CONTAINER_PREV_PAGE_EVENTID));
				newIcon->startEvents();
				pContent->addChild(newIcon);
			}

			pIcon = SDL_static_cast(GUI_Icon*, pContent->getChild(CONTAINER_NEXT_PAGE_EVENTID));
			if(pIcon)
			{
				if(!hasNextPage)
					pContent->removeChild(pIcon);
			}
			else if(hasNextPage)
			{
				GUI_Icon* newIcon = new GUI_Icon(iRect(135, 3, GUI_UI_ICON_BROWSE_RIGHT_UP_W, GUI_UI_ICON_BROWSE_RIGHT_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_BROWSE_RIGHT_UP_X, GUI_UI_ICON_BROWSE_RIGHT_UP_Y, GUI_UI_ICON_BROWSE_RIGHT_DOWN_X, GUI_UI_ICON_BROWSE_RIGHT_DOWN_Y, CONTAINER_NEXT_PAGE_EVENTID);
				newIcon->setButtonEventCallback(&container_Events, container_CreateEvent(index, CONTAINER_NEXT_PAGE_EVENTID));
				newIcon->startEvents();
				pContent->addChild(newIcon);
			}
		}
		else
		{
			GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(CONTAINER_PAGE_CONTENT_EVENTID));
			if(pContent)
				pPanel->removeChild(pContent);
		}
	}
}

void UTIL_createContainerWindow(Uint8 index)
{
	Uint32 windowId = GUI_PANEL_WINDOW_CONTAINERS_START + SDL_static_cast(Uint32, index);
	GUI_PanelWindow* pPanel = g_engine.getPanel(windowId);
	if(pPanel)
	{
		//Let's keep our window position :)
		UTIL_recreateContainerWindow(index, pPanel);
		return;
	}

	Container* container = g_game.findContainer(index);
	if(!container)
		return;

	Sint32 defaultHeight = 40 * SDL_static_cast(Sint32, UTIL_max<size_t>(1, ((container->getItems().size() + 3) / 4)));
	GUI_PanelWindow* newWindow = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, 19 + defaultHeight + (container->hasPages() ? 24 : 0)), true, windowId, true);
	newWindow->setEventCallback(&container_Events, container_CreateEvent(index, CONTAINER_RESIZE_WIDTH_EVENTID), container_CreateEvent(index, CONTAINER_RESIZE_HEIGHT_EVENTID), container_CreateEvent(index, CONTAINER_EXIT_WINDOW_EVENTID));
	GUI_ContainerImage* newImage = new GUI_ContainerImage(iRect(2, 0, 12, 12), index);
	newWindow->addChild(newImage);
	GUI_Icon* newIcon = new GUI_Icon(iRect(147, 0, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, CONTAINER_MAXIMINI_EVENTID, "Maximise or minimise window");
	newIcon->setButtonEventCallback(&container_Events, container_CreateEvent(index, CONTAINER_MAXIMINI_EVENTID));
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(159, 0, GUI_UI_ICON_CLOSE_WINDOW_UP_W, GUI_UI_ICON_CLOSE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_CLOSE_WINDOW_UP_X, GUI_UI_ICON_CLOSE_WINDOW_UP_Y, GUI_UI_ICON_CLOSE_WINDOW_DOWN_X, GUI_UI_ICON_CLOSE_WINDOW_DOWN_Y, 0, "Close this window");
	newIcon->setButtonEventCallback(&container_Events, container_CreateEvent(index, CONTAINER_CLOSE_EVENTID));
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_ContainerContainer* newContainer = new GUI_ContainerContainer(iRect(2, 13, 168, defaultHeight), index, newWindow, CONTAINER_CONTAINER_EVENTID);
	newContainer->startEvents();
	newWindow->addChild(newContainer);
	UTIL_recreateContainerWindow(index, newWindow);
	if(!g_engine.addToPanel(newWindow))
		delete newWindow;

	container->resetDirty();
}

GUI_ContainerImage::GUI_ContainerImage(iRect boxRect, Uint8 index, Uint32 internalID) : m_index(index)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_ContainerImage::render()
{
	Container* container = g_game.findContainer(m_index);
	if(!container)
	{
		UTIL_SafeEventHandler(&container_Events, container_CreateEvent(m_index, CONTAINER_CLOSED_EVENTID), 1);
		return;
	}
	if(container->isDirty())
	{
		UTIL_recreateContainerWindow(m_index, NULL);
		container->resetDirty();
	}

	ItemUI* item = container->getItem();
	if(item)
		item->render(m_tRect.x1, m_tRect.y1, m_tRect.y2);
}

void GUI_ContainerContainer::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	GUI_Container::onMouseMove(x, y, isInsideParent);
	if(g_engine.getAction() == CLIENT_ACTION_MOVEITEM)
	{
		if(isInsideParent && m_tRect.isPointInside(x, y))
			m_selected = true;
		else
			m_selected = false;
	}
}

void GUI_ContainerContainer::onLMouseUp(Sint32 x, Sint32 y)
{
	GUI_Container::onLMouseUp(x, y);
	if(g_engine.getAction() == CLIENT_ACTION_MOVEITEM && m_selected)
	{
		Container* container = g_game.findContainer(m_index);
		if(container && container->isUnlocked())
			g_engine.initMove(0xFFFF, (SDL_static_cast(Uint16, m_index) | 0x40), 0xFF);

		g_engine.setAction(CLIENT_ACTION_NONE);
	}
}

void GUI_ContainerContainer::render()
{
	GUI_Container::render();
	if(m_selected)
	{
		if(g_engine.getAction() != CLIENT_ACTION_MOVEITEM)
		{
			m_selected = false;
			return;
		}

		Container* container = g_game.findContainer(m_index);
		if(container)
		{
			auto& renderer = g_engine.getRender();
			if(container->isUnlocked())
				renderer->drawRectangle(m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2, 1, 255, 255, 255, 255);
			else
				renderer->drawRectangle(m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2, 1, 255, 0, 0, 255);
		}
	}
}

GUI_ContainerItem::GUI_ContainerItem(iRect boxRect, Uint8 cid, size_t index, Uint32 internalID) : m_index(index), m_cid(cid)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void* GUI_ContainerItem::onAction(Sint32, Sint32)
{
	Container* container = g_game.findContainer(m_cid);
	if(container)
	{
		ItemUI* item = container->getItem(m_index);
		if(item)
			return SDL_reinterpret_cast(void*, item);
	}

	return NULL;
}

void GUI_ContainerItem::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(g_engine.getAction() == CLIENT_ACTION_MOVEITEM)
	{
		if(isInsideParent && m_tRect.isPointInside(x, y))
			m_selected = true;
		else
			m_selected = false;
	}
}

void GUI_ContainerItem::onLMouseDown(Sint32 x, Sint32 y)
{
	Container* container = g_game.findContainer(m_cid);
	if(container)
	{
		ItemUI* item = container->getItem(m_index);
		if(item)
		{
			Position& position = item->getCurrentPosition();
			g_engine.setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), position.x, position.y, position.z, 0);
			g_engine.setActionData(CLIENT_ACTION_SECOND, 0, item->getItemCount(), 0, 0, 0, 0);
			g_engine.noteManualItemInteraction();
			g_engine.enableMoveItem(x, y);
		}
	}

	g_engine.setAction(CLIENT_ACTION_LEFTMOUSE);
}

void GUI_ContainerItem::onLMouseUp(Sint32, Sint32)
{
	if(g_engine.getAction() == CLIENT_ACTION_MOVEITEM && m_selected)
	{
		g_engine.noteManualItemInteraction();
		Container* container = g_game.findContainer(m_cid);
		if(container && container->isUnlocked())
			g_engine.initMove(0xFFFF, (SDL_static_cast(Uint16, m_cid) | 0x40), SDL_static_cast(Uint8, m_index));

		g_engine.setAction(CLIENT_ACTION_NONE);
	}
}

void GUI_ContainerItem::onRMouseDown(Sint32, Sint32)
{
	g_engine.setAction(CLIENT_ACTION_RIGHTMOUSE);
}

void GUI_ContainerItem::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_INVENTORY_EMPTY_X, GUI_UI_INVENTORY_EMPTY_Y, m_tRect.x1 - 1, m_tRect.y1 - 1, m_tRect.x2 + 2, m_tRect.y2 + 2);

	Container* container = g_game.findContainer(m_cid);
	if(!container)
		return;

	ItemUI* item = container->getItem(m_index);
	if(item)
	{
		item->render(m_tRect.x1, m_tRect.y1, m_tRect.y2);
		if(m_selected)
		{
			if(g_engine.getAction() != CLIENT_ACTION_MOVEITEM)
			{
				m_selected = false;
				return;
			}

			if(container->isUnlocked())
				renderer->drawRectangle(m_tRect.x1 - 1, m_tRect.y1 - 1, m_tRect.x2 + 2, m_tRect.y2 + 2, 1, 255, 255, 255, 255);
			else
				renderer->drawRectangle(m_tRect.x1 - 1, m_tRect.y1 - 1, m_tRect.x2 + 2, m_tRect.y2 + 2, 1, 255, 0, 0, 255);
		}
	}
}
