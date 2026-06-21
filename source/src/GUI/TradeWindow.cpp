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
#include "../GUI_Elements/GUI_StaticImage.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_Content.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../game.h"
#include "TradeWindow.h"
#include "itemUI.h"

#define TRADE_TITLE "Trade"
#define TRADE_MAXIMINI_EVENTID 1000
#define TRADE_CLOSE_EVENTID 1001
#define TRADE_RESIZE_WIDTH_EVENTID 1002
#define TRADE_RESIZE_HEIGHT_EVENTID 1003
#define TRADE_EXIT_WINDOW_EVENTID 1004
#define TRADE_CONTENT1_EVENTID 1005
#define TRADE_CONTENT2_EVENTID 1006
#define TRADE_CONTAINER_EVENTID 1007
#define TRADE_OWN_NAME_LABEL_EVENTID 1008
#define TRADE_COUNTER_NAME_LABEL_EVENTID 1009
#define TRADE_ACTION_LABEL_EVENTID 1010
#define TRADE_REJECT_BUTTON_EVENTID 1011
#define TRADE_ACCEPT_BUTTON_EVENTID 1012

extern Engine g_engine;
extern Game g_game;

void trade_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case TRADE_MAXIMINI_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_TRADE);
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

					GUI_TradeContainer* pContainer = SDL_static_cast(GUI_TradeContainer*, pPanel->getChild(TRADE_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeInvisible();

					GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT1_EVENTID));
					if(pContent)
						pContent->makeInvisible();

					pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT2_EVENTID));
					if(pContent)
						pContent->makeInvisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(TRADE_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_X, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_Y);
				}
				else
				{
					Sint32 cachedHeight = pPanel->getCachedHeight();
					parent->tryFreeHeight(cachedHeight - pRect.y2);
					pPanel->setSize(pRect.x2, cachedHeight);
					parent->checkPanels();

					GUI_TradeContainer* pContainer = SDL_static_cast(GUI_TradeContainer*, pPanel->getChild(TRADE_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeVisible();

					GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT1_EVENTID));
					if(pContent)
						pContent->makeVisible();

					pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT2_EVENTID));
					if(pContent)
						pContent->makeVisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(TRADE_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y);
				}
			}
		}
		break;
		case TRADE_CLOSE_EVENTID:
		case TRADE_REJECT_BUTTON_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_TRADE);
			if(pPanel)
			{
				g_game.sendCloseTrade();

				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				parent->removePanel(pPanel);
			}
		}
		break;
		case TRADE_RESIZE_HEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_TRADE);
			if(pPanel)
			{
				GUI_TradeContainer* pContainer = SDL_static_cast(GUI_TradeContainer*, pPanel->getChild(TRADE_CONTAINER_EVENTID));
				if(pContainer)
				{
					iRect cRect = pContainer->getRect();
					cRect.y2 = status - 56;
					pContainer->setRect(cRect);
				}

				GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT2_EVENTID));
				if(pContent)
				{
					iRect pRect = pPanel->getRect();
					iRect cRect = pContent->getRect();
					cRect.y1 = pRect.y1 + status - 31;
					pContent->setRect(cRect);
				}
			}
		}
		break;
		case TRADE_ACCEPT_BUTTON_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_TRADE);
			if(pPanel)
			{
				g_game.sendAcceptTrade();

				GUI_Content* pContent2 = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT2_EVENTID));
				if(pContent2)
				{
					GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(TRADE_ACTION_LABEL_EVENTID));
					if(pLabel)
						pLabel->setName("Please wait for\nyour partner to accept.");
					else
					{
						GUI_Label* newLabel = new GUI_Label(iRect(60, 6, 0, 0), "Please wait for\nyour partner to accept.", TRADE_ACTION_LABEL_EVENTID, 255, 255, 255);
						newLabel->setFont(CLIENT_FONT_SMALL);
						newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
						pContent2->addChild(newLabel);
					}

					GUI_Button* pButton = SDL_static_cast(GUI_Button*, pContent2->getChild(TRADE_ACCEPT_BUTTON_EVENTID));
					if(pButton)
						pContent2->removeChild(pButton);
				}
			}
		}
		break;
		default: break;
	}
}

void UTIL_recreateTradeWindow(bool counter, const std::string& name, std::vector<ItemUI*>& itemVector, GUI_PanelWindow* pPanel)
{
	GUI_Content* pContent1 = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT1_EVENTID));
	GUI_Content* pContent2 = SDL_static_cast(GUI_Content*, pPanel->getChild(TRADE_CONTENT2_EVENTID));
	GUI_TradeContainer* pContainer = SDL_static_cast(GUI_TradeContainer*, pPanel->getChild(TRADE_CONTAINER_EVENTID));
	if(pContainer)
	{
		for(size_t i = 0, end = itemVector.size(); i != end; ++i)
		{
			GUI_TradeItem* newTradeItem = new GUI_TradeItem(iRect(SDL_static_cast(Sint32, ((i % 2) * 37)) + (counter ? 82 : 3), SDL_static_cast(Sint32, ((i / 2) * 37)) - 6, 32, 32), itemVector[i], SDL_static_cast(Uint8, i), counter);
			newTradeItem->startEvents();
			pContainer->addChild(newTradeItem);
		}

		pPanel->setMinHeight(98);
		pContainer->setAsMaxHeight();
		pPanel->setMaxHeight(pPanel->getMaxHeight() + 38);
	}
	if(counter)
	{
		if(pContent1)
		{
			GUI_DynamicLabel* pLabel = SDL_static_cast(GUI_DynamicLabel*, pContent1->getChild(TRADE_COUNTER_NAME_LABEL_EVENTID));
			if(pLabel)
				pLabel->setName(name);
			else
			{
				GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(82, 2, 74, 8), name, TRADE_COUNTER_NAME_LABEL_EVENTID, 255, 255, 255);
				newLabel->setFont(CLIENT_FONT_SMALL);
				pContent1->addChild(newLabel);
			}
		}
		if(pContent2)
		{
			GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(TRADE_ACTION_LABEL_EVENTID));
			if(pLabel)
				pContent2->removeChild(pLabel);
			
			GUI_Button* pButton = SDL_static_cast(GUI_Button*, pContent2->getChild(TRADE_REJECT_BUTTON_EVENTID));
			if(pButton)
				pContent2->removeChild(pButton);

			pButton = SDL_static_cast(GUI_Button*, pContent2->getChild(TRADE_ACCEPT_BUTTON_EVENTID));
			if(pButton)
				pContent2->removeChild(pButton);

			GUI_Button* newButton = new GUI_Button(iRect(121, 3, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Reject", TRADE_REJECT_BUTTON_EVENTID);
			newButton->setButtonEventCallback(&trade_Events, TRADE_REJECT_BUTTON_EVENTID);
			newButton->startEvents();
			pContent2->addChild(newButton);
			newButton = new GUI_Button(iRect(74, 3, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Accept", TRADE_ACCEPT_BUTTON_EVENTID);
			newButton->setButtonEventCallback(&trade_Events, TRADE_ACCEPT_BUTTON_EVENTID);
			newButton->startEvents();
			pContent2->addChild(newButton);
		}
	}
	else
	{
		if(pContent1)
		{
			GUI_DynamicLabel* pLabel = SDL_static_cast(GUI_DynamicLabel*, pContent1->getChild(TRADE_OWN_NAME_LABEL_EVENTID));
			if(pLabel)
				pLabel->setName(name);
			else
			{
				GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(3, 2, 74, 8), name, TRADE_OWN_NAME_LABEL_EVENTID, 255, 255, 255);
				newLabel->setFont(CLIENT_FONT_SMALL);
				pContent1->addChild(newLabel);
			}
		}
		if(pContent2)
		{
			GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(TRADE_ACTION_LABEL_EVENTID));
			if(pLabel)
				pLabel->setName("Please wait for a\ncounteroffer.");
			else
			{
				GUI_Label* newLabel = new GUI_Label(iRect(60, 6, 0, 0), "Please wait for a\ncounteroffer.", TRADE_ACTION_LABEL_EVENTID, 255, 255, 255);
				newLabel->setFont(CLIENT_FONT_SMALL);
				newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
				pContent2->addChild(newLabel);
			}
			
			GUI_Button* pButton = SDL_static_cast(GUI_Button*, pContent2->getChild(TRADE_REJECT_BUTTON_EVENTID));
			if(pButton)
				pContent2->removeChild(pButton);

			pButton = SDL_static_cast(GUI_Button*, pContent2->getChild(TRADE_ACCEPT_BUTTON_EVENTID));
			if(pButton)
				pContent2->removeChild(pButton);

			GUI_Button* newButton = new GUI_Button(iRect(121, 3, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", TRADE_REJECT_BUTTON_EVENTID);
			newButton->setButtonEventCallback(&trade_Events, TRADE_REJECT_BUTTON_EVENTID);
			newButton->startEvents();
			pContent2->addChild(newButton);
		}
	}
}

void UTIL_createTradeWindow(bool counter, const std::string& name, void* itemsData)
{
	std::vector<ItemUI*>& itemVector = *SDL_reinterpret_cast(std::vector<ItemUI*>*, itemsData);

	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_TRADE);
	if(pPanel)
	{
		UTIL_recreateTradeWindow(counter, name, itemVector, pPanel);
		return;
	}

	Sint32 defaultHeight = 40 * SDL_static_cast(Sint32, UTIL_max<size_t>(1, ((itemVector.size() + 1) / 2)));
	GUI_PanelWindow* newWindow = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, 57 + defaultHeight), true, GUI_PANEL_WINDOW_TRADE, true);
	newWindow->setEventCallback(&trade_Events, TRADE_RESIZE_WIDTH_EVENTID, TRADE_RESIZE_HEIGHT_EVENTID, TRADE_EXIT_WINDOW_EVENTID);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(2, 0, GUI_UI_ICON_TRADEWIDGET_W, GUI_UI_ICON_TRADEWIDGET_H), GUI_UI_IMAGE, GUI_UI_ICON_TRADEWIDGET_X, GUI_UI_ICON_TRADEWIDGET_Y);
	newWindow->addChild(newImage);
	GUI_Icon* newIcon = new GUI_Icon(iRect(147, 0, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, TRADE_MAXIMINI_EVENTID, "Maximise or minimise window");
	newIcon->setButtonEventCallback(&trade_Events, TRADE_MAXIMINI_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(159, 0, GUI_UI_ICON_CLOSE_WINDOW_UP_W, GUI_UI_ICON_CLOSE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_CLOSE_WINDOW_UP_X, GUI_UI_ICON_CLOSE_WINDOW_UP_Y, GUI_UI_ICON_CLOSE_WINDOW_DOWN_X, GUI_UI_ICON_CLOSE_WINDOW_DOWN_Y, 0, "Close this window");
	newIcon->setButtonEventCallback(&trade_Events, TRADE_CLOSE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(19, 2, 100, 14), TRADE_TITLE, 0, 144, 144, 144);
	newWindow->addChild(newLabel);
	GUI_Content* newContent = new GUI_Content(iRect(2, 13, 168, 12), TRADE_CONTENT1_EVENTID);
	newContent->startEvents();
	newWindow->addChild(newContent);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(0, 10, 168, 2));
	newContent->addChild(newSeparator);
	newContent = new GUI_Content(iRect(2, 25 + defaultHeight, 168, 25), TRADE_CONTENT2_EVENTID);
	newContent->startEvents();
	newWindow->addChild(newContent);
	newSeparator = new GUI_Separator(iRect(0, 0, 168, 2));
	newContent->addChild(newSeparator);
	GUI_TradeContainer* newContainer = new GUI_TradeContainer(iRect(2, 25, 168, defaultHeight), newWindow, TRADE_CONTAINER_EVENTID);
	newContainer->startEvents();
	newWindow->addChild(newContainer);
	UTIL_recreateTradeWindow(counter, name, itemVector, newWindow);
	if(!g_engine.addToPanel(newWindow))
		delete newWindow;
}

void UTIL_closeTradeWindow()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_TRADE);
	if(pPanel)
	{
		GUI_Panel* parent = pPanel->getParent();
		if(!parent)
			return;

		parent->removePanel(pPanel);
	}
}

void GUI_TradeContainer::render()
{
	GUI_Container::render();
	g_engine.getRender()->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_SEPARATOR_X, GUI_UI_ICON_VERTICAL_SEPARATOR_Y, GUI_UI_ICON_VERTICAL_SEPARATOR_W, GUI_UI_ICON_VERTICAL_SEPARATOR_H, m_tRect.x1 + 77, m_tRect.y1, 2, m_tRect.y2);
}

GUI_TradeItem::GUI_TradeItem(iRect boxRect, ItemUI* item, Uint8 index, bool counterOffer, Uint32 internalID) : m_item(item), m_index(index), m_counterOffer(counterOffer)
{
	setRect(boxRect);
	m_internalID = internalID;
}

GUI_TradeItem::~GUI_TradeItem()
{
	if(m_item)
		delete m_item;
}

void GUI_TradeItem::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(m_haveMouse > 0)
	{
		bool inside = (isInsideParent && m_tRect.isPointInside(x, y));
		if(m_haveMouse == 1 && !inside)
			m_haveMouse = 2;
		else if(m_haveMouse == 2 && inside)
			m_haveMouse = 1;
	}
}

void GUI_TradeItem::onLMouseDown(Sint32, Sint32)
{
	m_haveMouse = 1;
}

void GUI_TradeItem::onLMouseUp(Sint32, Sint32)
{
	if(m_haveMouse > 0)
	{
		if(m_haveMouse == 1)
			g_game.sendLookInTrade(m_counterOffer, m_index);

		m_haveMouse = 0;
	}
}

void GUI_TradeItem::onRMouseDown(Sint32, Sint32)
{
	m_haveMouse = 1;
}

void GUI_TradeItem::onRMouseUp(Sint32, Sint32)
{
	if(m_haveMouse > 0)
	{
		if(m_haveMouse == 1)
			g_game.sendLookInTrade(m_counterOffer, m_index);

		m_haveMouse = 0;
	}
}

void GUI_TradeItem::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_INVENTORY_EMPTY_X, GUI_UI_INVENTORY_EMPTY_Y, m_tRect.x1 - 1, m_tRect.y1 - 1, m_tRect.x2 + 2, m_tRect.y2 + 2);
	if(m_item)
		m_item->render(m_tRect.x1, m_tRect.y1, m_tRect.y2);
}
