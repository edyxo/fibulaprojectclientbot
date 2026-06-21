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
#include "../GUI_Elements/GUI_ScrollBar.h"
#include "../GUI_Elements/GUI_Content.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_ContextMenu.h"
#include "../GUI_Elements/GUI_ScrollBar.h"
#include "../game.h"
#include "../thingManager.h"
#include "ShopWindow.h"
#include "itemUI.h"

#define SHOP_TITLE "NPC Trade"
#define SHOP_TITLE_EVENTID 2000
#define SHOP_MAXIMINI_EVENTID 1000
#define SHOP_CLOSE_EVENTID 1001
#define SHOP_CONFIGURE_EVENTID 1002
#define SHOP_RESIZE_WIDTH_EVENTID 1003
#define SHOP_RESIZE_HEIGHT_EVENTID 1004
#define SHOP_EXIT_WINDOW_EVENTID 1005
#define SHOP_CONTENT1_EVENTID 1006
#define SHOP_CONTENT2_EVENTID 1007
#define SHOP_CONTAINER_EVENTID 1008
#define SHOP_CURRENCY_LABEL_EVENTID 1009
#define SHOP_CURRENCY_NAME_LABEL_EVENTID 1010
#define SHOP_CURRENCY_ITEM_EVENTID 1011
#define SHOP_BUY_BUTTON_EVENTID 1012
#define SHOP_SELL_BUTTON_EVENTID 1013
#define SHOP_AMOUNT_LABEL_EVENTID 1014
#define SHOP_AMOUNT_NAME_LABEL_EVENTID 1015
#define SHOP_PRICE_LABEL_EVENTID 1016
#define SHOP_PRICE_NAME_LABEL_EVENTID 1017
#define SHOP_MONEY_LABEL_EVENTID 1018
#define SHOP_MONEY_NAME_LABEL_EVENTID 1019
#define SHOP_AMOUNT_SCROLLBAR_EVENTID 1020
#define SHOP_ACCEPT_BUTTON_EVENTID 1021
#define SHOP_ITEM_DISPLAY_EVENTID 1022

#define SHOP_SORT_BY_NAME_EVENTID 1500
#define SHOP_SORT_BY_PRICE_EVENTID 1501
#define SHOP_SORT_BY_WEIGHT_EVENTID 1502
#define SHOP_BUY_IN_BACKPACK_EVENTID 1503
#define SHOP_IGNORE_CAPACITY_EVENTID 1504
#define SHOP_SELL_EQUIPPED_EVENTID 1505

extern Engine g_engine;
extern Game g_game;
extern ThingManager g_thingManager;

std::vector<NPCTradeItems> g_tempNPCItems;
NPCTradeGoods g_tempNPCGoods;
size_t g_NPCActiveOffer = SDL_static_cast(size_t, -1);
Uint64 g_tempNPCMoney = 0;
Uint16 g_tempNPCCurrencyId = 0;
bool g_CanNPCTrade = false;
bool g_buyNPCPage = true;

bool CheckNPCBuyButton() {return g_buyNPCPage;}
bool CheckNPCSellButton() {return !g_buyNPCPage;}

Uint16 UTIL_getShopItemAmount()
{
	if((g_buyNPCPage && g_game.hasGameFeature(GAME_FEATURE_DOUBLE_SHOPBUYAMOUNT)) || (!g_buyNPCPage && g_game.hasGameFeature(GAME_FEATURE_DOUBLE_SHOPSELLAMOUNT)))
		return 10000;

	return 100;
}

Uint16 UTIL_getShopSellQuantity(Uint16 itemId)
{
	Uint16 currentQuantity = 0;
	NPCTradeGoods::iterator it = g_tempNPCGoods.find(itemId);
	if(it != g_tempNPCGoods.end())
		currentQuantity = it->second;

	if(g_engine.getIgnoreEquiped())
	{
		for(Uint8 slot = SLOT_HEAD; slot <= SLOT_AMMO; ++slot)
		{
			ItemUI* inventoryItem = g_game.getInventoryItem(slot);
			if(inventoryItem && inventoryItem->getID() == itemId)
			{
				Uint16 itemCount = inventoryItem->getItemCount();
				if(currentQuantity > itemCount)
					currentQuantity -= itemCount;
				else
					return 0;
			}
		}
	}
	return currentQuantity;
}

Uint64 UTIL_getShopItemPrice(Sint32 quantity)
{
	if(g_NPCActiveOffer == SDL_static_cast(size_t, -1) || quantity == 0)
		return 0;

	NPCTradeItems& tradeItem = g_tempNPCItems[g_NPCActiveOffer];
	Uint64 price = SDL_static_cast(Uint64, (g_buyNPCPage ? tradeItem.itemBuyPrice : tradeItem.itemSellPrice)) * SDL_static_cast(Uint64, quantity);
	if(g_buyNPCPage && g_engine.getBuyWithBackpacks())
	{
		ThingType* itemThing = g_thingManager.getThingType(ThingCategory_Item, tradeItem.itemId);
		if(itemThing && itemThing->hasFlag(ThingAttribute_Stackable))
			quantity = (quantity + 99) / 100;
		
		price += SDL_static_cast(Uint64, ((quantity + 19) / 20) * 20);
	}
	return price;
}

Uint16 UTIL_getShopItemQuantity()
{
	if(g_NPCActiveOffer == SDL_static_cast(size_t, -1))
		return 0;

	NPCTradeItems& tradeItem = g_tempNPCItems[g_NPCActiveOffer];
	if(g_buyNPCPage)
	{
		Uint16 capacityMaxCount = SDL_static_cast(Uint16, UTIL_min<Uint32>(65535, SDL_static_cast(Uint32, g_game.getPlayerCapacity() * 100.0) / tradeItem.itemWeight));
		if(g_engine.getIgnoreCapacity())
			capacityMaxCount = 65535;

		Uint16 priceMaxCount = SDL_static_cast(Uint16, UTIL_min<Uint64>(65535, g_tempNPCMoney / SDL_static_cast(Uint64, tradeItem.itemBuyPrice)));
		Uint16 buyMaxCount = UTIL_max<Uint16>(1, UTIL_min<Uint16>(UTIL_getShopItemAmount(), UTIL_min<Uint16>(capacityMaxCount, priceMaxCount)));
		for(Uint16 i = buyMaxCount; i > 0; --i)
		{
			//Check for additional prices like backpacks cost
			if(UTIL_getShopItemPrice(SDL_static_cast(Sint32, i)) <= g_tempNPCMoney)
				return i;
		}
		return 0;
	}
	else
		return UTIL_min<Uint16>(UTIL_getShopItemAmount(), UTIL_getShopSellQuantity(tradeItem.itemId));
}

void UTIL_sortShopItems()
{
	Uint8 sortMethod = (g_buyNPCPage ? g_engine.getBuySortMethod() : g_engine.getSellSortMethod());
	switch(sortMethod)
	{
		case Shop_Sort_Name: std::sort(g_tempNPCItems.begin(), g_tempNPCItems.end(), [](NPCTradeItems& a, NPCTradeItems& b) -> bool {return a.itemName < b.itemName;}); break;
		case Shop_Sort_Price:
		{
			if(g_buyNPCPage)
				std::sort(g_tempNPCItems.begin(), g_tempNPCItems.end(), [](NPCTradeItems& a, NPCTradeItems& b) -> bool {return a.itemBuyPrice < b.itemBuyPrice;});
			else
				std::sort(g_tempNPCItems.begin(), g_tempNPCItems.end(), [](NPCTradeItems& a, NPCTradeItems& b) -> bool {return a.itemSellPrice < b.itemSellPrice;});
		}
		break;
		case Shop_Sort_Weight: std::sort(g_tempNPCItems.begin(), g_tempNPCItems.end(), [](NPCTradeItems& a, NPCTradeItems& b) -> bool {return a.itemWeight < b.itemWeight;}); break;
		default: break;
	}
}

void UTIL_recreateShopWindow(const std::string& name, Uint16 currencyId, GUI_PanelWindow* pPanel);
void shop_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case SHOP_MAXIMINI_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
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

					GUI_ShopContainer* pContainer = SDL_static_cast(GUI_ShopContainer*, pPanel->getChild(SHOP_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeInvisible();

					GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT1_EVENTID));
					if(pContent)
						pContent->makeInvisible();

					pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT2_EVENTID));
					if(pContent)
						pContent->makeInvisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(SHOP_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_X, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_Y);
				}
				else
				{
					Sint32 cachedHeight = pPanel->getCachedHeight();
					parent->tryFreeHeight(cachedHeight - pRect.y2);
					pPanel->setSize(pRect.x2, cachedHeight);
					parent->checkPanels();

					GUI_ShopContainer* pContainer = SDL_static_cast(GUI_ShopContainer*, pPanel->getChild(SHOP_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeVisible();

					GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT1_EVENTID));
					if(pContent)
						pContent->makeVisible();

					pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT2_EVENTID));
					if(pContent)
						pContent->makeVisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(SHOP_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y);
				}
			}
		}
		break;
		case SHOP_CLOSE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
			if(pPanel)
			{
				g_game.sendNPCClose();

				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				parent->removePanel(pPanel);
			}
		}
		break;
		case SHOP_CONFIGURE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
			if(pPanel)
			{
				GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(SHOP_CONFIGURE_EVENTID));
				if(pIcon)
				{
					iRect& iconRect = pIcon->getRect();
					UTIL_createShopPopupMenu(iconRect.x1, iconRect.y1 + 12);
				}
			}
		}
		break;
		case SHOP_RESIZE_HEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
			if(pPanel)
			{
				GUI_ShopContainer* pContainer = SDL_static_cast(GUI_ShopContainer*, pPanel->getChild(SHOP_CONTAINER_EVENTID));
				if(pContainer)
				{
					iRect cRect = pContainer->getRect();
					cRect.y2 = status - 89 - (g_tempNPCCurrencyId > 0 ? 42 : 30);
					pContainer->setRect(cRect);
				}

				GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT2_EVENTID));
				if(pContent)
				{
					iRect pRect = pPanel->getRect();
					iRect cRect = pContent->getRect();
					cRect.y1 = pRect.y1 + status - 76;
					pContent->setRect(cRect);
				}
			}
		}
		break;
		case SHOP_EXIT_WINDOW_EVENTID:
		{
			g_NPCActiveOffer = SDL_static_cast(size_t, -1);
			g_tempNPCItems.clear();
			g_tempNPCGoods.clear();
			g_tempNPCItems.shrink_to_fit();
		}
		break;
		case SHOP_BUY_BUTTON_EVENTID:
		{
			GUI_PanelWindow* pPanel;
			if(!g_buyNPCPage && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP)) != NULL)
			{
				g_buyNPCPage = true;
				UTIL_recreateShopWindow("", g_tempNPCCurrencyId, pPanel);
			}
		}
		break;
		case SHOP_SELL_BUTTON_EVENTID:
		{
			GUI_PanelWindow* pPanel;
			if(g_buyNPCPage && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP)) != NULL)
			{
				g_buyNPCPage = false;
				UTIL_recreateShopWindow("", g_tempNPCCurrencyId, pPanel);
			}
		}
		break;
		case SHOP_AMOUNT_SCROLLBAR_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
			if(pPanel)
			{
				GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT2_EVENTID));
				if(pContent)
				{
					++status;

					GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(SHOP_AMOUNT_NAME_LABEL_EVENTID));
					if(pLabel)
					{
						Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%d", status);
						std::string result = UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len)));
						pLabel->setName(std::move(result));
					}

					pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(SHOP_PRICE_NAME_LABEL_EVENTID));
					if(pLabel)
					{
						Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%" SDL_PRIu64, UTIL_getShopItemPrice(status));
						std::string result = UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len)));
						pLabel->setName(std::move(result));
					}

					GUI_ShopItem* pShopItem = SDL_static_cast(GUI_ShopItem*, pContent->getChild(SHOP_ITEM_DISPLAY_EVENTID));
					if(pShopItem && g_NPCActiveOffer != SDL_static_cast(size_t, -1))
					{
						NPCTradeItems& tradeItem = g_tempNPCItems[g_NPCActiveOffer];
						ThingType* itemThing = g_thingManager.getThingType(ThingCategory_Item, tradeItem.itemId);
						if(itemThing && itemThing->hasFlag(ThingAttribute_Stackable))
							pShopItem->setItemData(tradeItem.itemId, SDL_static_cast(Uint16, status));
						else
							pShopItem->setItemData(tradeItem.itemId, SDL_static_cast(Uint16, tradeItem.itemSubtype));
					}
				}
			}
		}
		break;
		case SHOP_ACCEPT_BUTTON_EVENTID:
		{
			GUI_PanelWindow* pPanel;
			if(g_CanNPCTrade && g_NPCActiveOffer != SDL_static_cast(size_t, -1) && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP)) != NULL)
			{
				Uint16 amount = 1;

				NPCTradeItems& tradeItem = g_tempNPCItems[g_NPCActiveOffer];
				if(g_buyNPCPage)
					g_game.sendNPCPurchase(tradeItem.itemId, tradeItem.itemSubtype, amount);
				else
					g_game.sendNPCSell(tradeItem.itemId, tradeItem.itemSubtype, amount);
			}
		}
		break;

		case SHOP_SORT_BY_NAME_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
			if(pPanel)
			{
				(g_buyNPCPage ? g_engine.setBuySortMethod(Shop_Sort_Name) : g_engine.setSellSortMethod(Shop_Sort_Name));
				UTIL_recreateShopWindow("", g_tempNPCCurrencyId, pPanel);
			}
		}
		break;
		case SHOP_SORT_BY_PRICE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
			if(pPanel)
			{
				(g_buyNPCPage ? g_engine.setBuySortMethod(Shop_Sort_Price) : g_engine.setSellSortMethod(Shop_Sort_Price));
				UTIL_recreateShopWindow("", g_tempNPCCurrencyId, pPanel);
			}
		}
		break;
		case SHOP_SORT_BY_WEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
			if(pPanel)
			{
				(g_buyNPCPage ? g_engine.setBuySortMethod(Shop_Sort_Weight) : g_engine.setSellSortMethod(Shop_Sort_Weight));
				UTIL_recreateShopWindow("", g_tempNPCCurrencyId, pPanel);
			}
		}
		break;
		case SHOP_BUY_IN_BACKPACK_EVENTID:
		{
			g_engine.setBuyWithBackpacks(!g_engine.getBuyWithBackpacks());
			UTIL_updateShopWindow();
		}
		break;
		case SHOP_IGNORE_CAPACITY_EVENTID:
		{
			g_engine.setIgnoreCapacity(!g_engine.getIgnoreCapacity());
			UTIL_updateShopWindow();
		}
		break;
		case SHOP_SELL_EQUIPPED_EVENTID:
		{
			g_engine.setIgnoreEquiped(!g_engine.getIgnoreEquiped());
			UTIL_updateShopWindow();
		}
		break;
		default: break;
	}
}

void UTIL_createShopPopupMenu(Sint32 x, Sint32 y)
{
	Uint8 sortMethod = (g_buyNPCPage ? g_engine.getBuySortMethod() : g_engine.getSellSortMethod());
	GUI_ContextMenu* newMenu = new GUI_ContextMenu();
	newMenu->addContextMenu((sortMethod == Shop_Sort_Name ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SHOP_SORT_BY_NAME_EVENTID, "Sort by name", "");
	newMenu->addContextMenu((sortMethod == Shop_Sort_Price ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SHOP_SORT_BY_PRICE_EVENTID, "Sort by price", "");
	newMenu->addContextMenu((sortMethod == Shop_Sort_Weight ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED)|CONTEXTMENU_STYLE_SEPARATED, SHOP_SORT_BY_WEIGHT_EVENTID, "Sort by weight", "");
	if(g_buyNPCPage)
	{
		newMenu->addContextMenu((g_engine.getBuyWithBackpacks() ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SHOP_BUY_IN_BACKPACK_EVENTID, "Buy in shopping bags", "");
		newMenu->addContextMenu((g_engine.getIgnoreCapacity() ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SHOP_IGNORE_CAPACITY_EVENTID, "Ignore capacity", "");
	}
	else
		newMenu->addContextMenu((g_engine.getIgnoreEquiped() ? CONTEXTMENU_STYLE_UNCHECKED : CONTEXTMENU_STYLE_CHECKED), SHOP_SELL_EQUIPPED_EVENTID, "Sell equipped", "");

	newMenu->setEventCallback(&shop_Events);
	g_engine.showContextMenu(newMenu, x, y);
}

void UTIL_updateShopWindow()
{
	GUI_PanelWindow* pPanel;
	if(g_NPCActiveOffer != SDL_static_cast(size_t, -1) && (pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP)) != NULL)
	{
		Uint16 currentAmount = 1;

		GUI_Content* pContent = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT2_EVENTID));
		if(pContent)
		{
			GUI_HScrollBar* pScrollBar = SDL_static_cast(GUI_HScrollBar*, pContent->getChild(SHOP_AMOUNT_SCROLLBAR_EVENTID));
			if(g_CanNPCTrade && pScrollBar)
				currentAmount = SDL_static_cast(Uint16, pScrollBar->getScrollPos() + 1);

			GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(SHOP_MONEY_NAME_LABEL_EVENTID));
			if(pLabel)
			{
				Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%" SDL_PRIu64, g_tempNPCMoney);
				std::string result = UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len)));
				pLabel->setName(std::move(result));
			}

			Uint16 amount = UTIL_getShopItemQuantity();
			if(amount == 0)
			{
				if(pScrollBar)
					pScrollBar->setScrollSize(0);

				pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(SHOP_AMOUNT_NAME_LABEL_EVENTID));
				if(pLabel)
					pLabel->setName("0");

				pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(SHOP_PRICE_NAME_LABEL_EVENTID));
				if(pLabel)
					pLabel->setName("0");

				GUI_ShopItem* pShopItem = SDL_static_cast(GUI_ShopItem*, pContent->getChild(SHOP_ITEM_DISPLAY_EVENTID));
				if(pShopItem)
					pShopItem->setItemData(0, 0);

				g_CanNPCTrade = false;
			}
			else
			{
				if(pScrollBar && (amount < currentAmount || pScrollBar->getScrollSize() != amount - 1))
				{
					pScrollBar->setScrollSize(SDL_static_cast(Sint32, amount) - 1);
					if(amount < currentAmount)
						currentAmount = amount;
				}
				
				pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(SHOP_AMOUNT_NAME_LABEL_EVENTID));
				if(pLabel)
				{
					Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%d", currentAmount);
					std::string result = UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len)));
					pLabel->setName(std::move(result));
				}
				
				pLabel = SDL_static_cast(GUI_Label*, pContent->getChild(SHOP_PRICE_NAME_LABEL_EVENTID));
				if(pLabel)
				{
					Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%" SDL_PRIu64, UTIL_getShopItemPrice(SDL_static_cast(Sint32, currentAmount)));
					std::string result = UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len)));
					pLabel->setName(std::move(result));
				}

				GUI_ShopItem* pShopItem = SDL_static_cast(GUI_ShopItem*, pContent->getChild(SHOP_ITEM_DISPLAY_EVENTID));
				if(pShopItem)
				{
					NPCTradeItems& tradeItem = g_tempNPCItems[g_NPCActiveOffer];
					ThingType* itemThing = g_thingManager.getThingType(ThingCategory_Item, tradeItem.itemId);
					if(itemThing && itemThing->hasFlag(ThingAttribute_Stackable))
						pShopItem->setItemData(tradeItem.itemId, SDL_static_cast(Uint16, currentAmount));
					else
						pShopItem->setItemData(tradeItem.itemId, SDL_static_cast(Uint16, tradeItem.itemSubtype));
				}

				g_CanNPCTrade = true;
			}
		}
	}
}

void UTIL_recreateShopWindow(const std::string& name, Uint16 currencyId, GUI_PanelWindow* pPanel)
{
	g_CanNPCTrade = false;
	g_NPCActiveOffer = SDL_static_cast(size_t, -1);
	if(!name.empty())
	{
		GUI_DynamicLabel* pLabel = SDL_static_cast(GUI_DynamicLabel*, pPanel->getChild(SHOP_TITLE_EVENTID));
		if(pLabel)
			pLabel->setName(std::move(std::string(SHOP_TITLE).append(" - ").append(name)));
	}

	GUI_Content* pContent1 = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT1_EVENTID));
	GUI_Content* pContent2 = SDL_static_cast(GUI_Content*, pPanel->getChild(SHOP_CONTENT2_EVENTID));
	GUI_ShopContainer* pContainer = SDL_static_cast(GUI_ShopContainer*, pPanel->getChild(SHOP_CONTAINER_EVENTID));
	if(pContent1)
	{
		if(currencyId > 0)
		{
			GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent1->getChild(SHOP_CURRENCY_LABEL_EVENTID));
			if(!pLabel)
			{
				GUI_Label* newLabel = new GUI_Label(iRect(3, 5, 0, 0), "Currency:", SHOP_CURRENCY_LABEL_EVENTID, 175, 175, 175);
				pContent1->addChild(newLabel);
			}

			std::string currencyName;
			ThingType* currencyThing = g_thingManager.getThingType(ThingCategory_Item, currencyId);
			if(currencyThing)
				currencyName = currencyThing->m_marketData.m_name;

			GUI_DynamicLabel* pDynamicLabel = SDL_static_cast(GUI_DynamicLabel*, pContent1->getChild(SHOP_CURRENCY_NAME_LABEL_EVENTID));
			if(pDynamicLabel)
				pDynamicLabel->setName(std::move(currencyName));
			else
			{
				GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(3, 25, 75, 14), std::move(currencyName), SHOP_CURRENCY_NAME_LABEL_EVENTID, 175, 175, 175);
				pContent1->addChild(newLabel);
			}

			GUI_Button* pButton = SDL_static_cast(GUI_Button*, pContent1->getChild(SHOP_BUY_BUTTON_EVENTID));
			if(pButton)
				pContent1->removeChild(pButton);

			pButton = SDL_static_cast(GUI_Button*, pContent1->getChild(SHOP_SELL_BUTTON_EVENTID));
			if(pButton)
				pContent1->removeChild(pButton);
			
			GUI_RadioButton* newButton = new GUI_RadioButton(iRect(123, 0, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Buy", SHOP_BUY_BUTTON_EVENTID, "Buy items from the NPC");
			newButton->setRadioEventCallback(&CheckNPCBuyButton, "You are currently buying items from the NPC");
			newButton->setButtonEventCallback(&shop_Events, SHOP_BUY_BUTTON_EVENTID);
			newButton->startEvents();
			pContent1->addChild(newButton);
			newButton = new GUI_RadioButton(iRect(123, 20, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Sell", SHOP_SELL_BUTTON_EVENTID, "Sell items to the NPC");
			newButton->setRadioEventCallback(&CheckNPCSellButton, "You are currently selling items to the NPC");
			newButton->setButtonEventCallback(&shop_Events, SHOP_SELL_BUTTON_EVENTID);
			newButton->startEvents();
			pContent1->addChild(newButton);

			GUI_ShopItem* pShopItem = SDL_static_cast(GUI_ShopItem*, pContent1->getChild(SHOP_CURRENCY_ITEM_EVENTID));
			if(pShopItem)
				pShopItem->setItemData(currencyId, 100);
			else
			{
				GUI_ShopItem* newShopItem = new GUI_ShopItem(iRect(88, 4, 32, 32), currencyId, 100, SHOP_CURRENCY_ITEM_EVENTID);
				newShopItem->startEvents();
				pContent1->addChild(newShopItem);
			}
			pPanel->setMinHeight(167);
		}
		else
		{
			GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent1->getChild(SHOP_CURRENCY_LABEL_EVENTID));
			if(pLabel)
				pContent1->removeChild(pLabel);

			GUI_DynamicLabel* pDynamicLabel = SDL_static_cast(GUI_DynamicLabel*, pContent1->getChild(SHOP_CURRENCY_NAME_LABEL_EVENTID));
			if(pDynamicLabel)
				pContent1->removeChild(pLabel);

			GUI_Button* pButton = SDL_static_cast(GUI_Button*, pContent1->getChild(SHOP_BUY_BUTTON_EVENTID));
			if(pButton)
				pContent1->removeChild(pButton);

			pButton = SDL_static_cast(GUI_Button*, pContent1->getChild(SHOP_SELL_BUTTON_EVENTID));
			if(pButton)
				pContent1->removeChild(pButton);

			GUI_RadioButton* newButton = new GUI_RadioButton(iRect(74, 4, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Buy", SHOP_BUY_BUTTON_EVENTID, "Buy items from the NPC");
			newButton->setRadioEventCallback(&CheckNPCBuyButton, "You are currently buying items from the NPC");
			newButton->setButtonEventCallback(&shop_Events, SHOP_BUY_BUTTON_EVENTID);
			newButton->startEvents();
			pContent1->addChild(newButton);
			newButton = new GUI_RadioButton(iRect(121, 4, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Sell", SHOP_SELL_BUTTON_EVENTID, "Sell items to the NPC");
			newButton->setRadioEventCallback(&CheckNPCSellButton, "You are currently selling items to the NPC");
			newButton->setButtonEventCallback(&shop_Events, SHOP_SELL_BUTTON_EVENTID);
			newButton->startEvents();
			pContent1->addChild(newButton);

			GUI_ShopItem* pShopItem = SDL_static_cast(GUI_ShopItem*, pContent1->getChild(SHOP_CURRENCY_ITEM_EVENTID));
			if(pShopItem)
				pContent1->removeChild(pShopItem);

			pPanel->setMinHeight(155);
		}
	}
	if(pContent2)
	{
		GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(SHOP_AMOUNT_LABEL_EVENTID));
		if(!pLabel)
		{
			GUI_Label* newLabel = new GUI_Label(iRect(3, 23, 0, 0), "Amount:", SHOP_AMOUNT_LABEL_EVENTID, 175, 175, 175);
			pContent2->addChild(newLabel);
		}
		
		pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(SHOP_PRICE_LABEL_EVENTID));
		if(!pLabel)
		{
			GUI_Label* newLabel = new GUI_Label(iRect(3, 38, 0, 0), "Price:", SHOP_PRICE_LABEL_EVENTID, 175, 175, 175);
			pContent2->addChild(newLabel);
		}
		
		pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(SHOP_MONEY_LABEL_EVENTID));
		if(!pLabel)
		{
			GUI_Label* newLabel = new GUI_Label(iRect(3, 53, 0, 0), (currencyId > 0 ? "Stock:" : "Money:"), SHOP_MONEY_LABEL_EVENTID, 175, 175, 175);
			pContent2->addChild(newLabel);
		}
		
		pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(SHOP_AMOUNT_NAME_LABEL_EVENTID));
		if(pLabel)
			pLabel->setName("0");
		else
		{
			GUI_Label* newLabel = new GUI_Label(iRect(117, 23, 0, 0), "0", SHOP_AMOUNT_NAME_LABEL_EVENTID, 175, 175, 175);
			newLabel->setAlign(CLIENT_FONT_ALIGN_RIGHT);
			pContent2->addChild(newLabel);
		}
		
		pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(SHOP_PRICE_NAME_LABEL_EVENTID));
		if(pLabel)
			pLabel->setName("0");
		else
		{
			GUI_Label* newLabel = new GUI_Label(iRect(117, 38, 0, 0), "0", SHOP_PRICE_NAME_LABEL_EVENTID, 175, 175, 175);
			newLabel->setAlign(CLIENT_FONT_ALIGN_RIGHT);
			pContent2->addChild(newLabel);
		}

		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%" SDL_PRIu64, g_tempNPCMoney);
		std::string result = UTIL_formatStringCommas(std::string(g_buffer, SDL_static_cast(size_t, len)));
		
		pLabel = SDL_static_cast(GUI_Label*, pContent2->getChild(SHOP_MONEY_NAME_LABEL_EVENTID));
		if(pLabel)
			pLabel->setName(std::move(result));
		else
		{
			GUI_Label* newLabel = new GUI_Label(iRect(117, 53, 0, 0), std::move(result), SHOP_MONEY_NAME_LABEL_EVENTID, 175, 175, 175);
			newLabel->setAlign(CLIENT_FONT_ALIGN_RIGHT);
			pContent2->addChild(newLabel);
		}

		GUI_HScrollBar* pScrollBar = SDL_static_cast(GUI_HScrollBar*, pContent2->getChild(SHOP_AMOUNT_SCROLLBAR_EVENTID));
		if(pScrollBar)
			pScrollBar->setScrollSize(0);
		else
		{
			GUI_HScrollBar* newHScrollBar = new GUI_HScrollBar(iRect(2, 6, 116, 12), 0, 0, SHOP_AMOUNT_SCROLLBAR_EVENTID);
			newHScrollBar->setBarEventCallback(&shop_Events, SHOP_AMOUNT_SCROLLBAR_EVENTID);
			newHScrollBar->startEvents();
			pContent2->addChild(newHScrollBar);
		}

		GUI_Button* pButton = SDL_static_cast(GUI_Button*, pContent2->getChild(SHOP_ACCEPT_BUTTON_EVENTID));
		if(!pButton)
		{
			GUI_Button* newButton = new GUI_Button(iRect(123, 45, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", SHOP_ACCEPT_BUTTON_EVENTID);
			newButton->setButtonEventCallback(&shop_Events, SHOP_ACCEPT_BUTTON_EVENTID);
			newButton->startEvents();
			pContent2->addChild(newButton);
		}

		GUI_ShopItem* pShopItem = SDL_static_cast(GUI_ShopItem*, pContent2->getChild(SHOP_ITEM_DISPLAY_EVENTID));
		if(pShopItem)
			pShopItem->setItemData(0, 0);
		else
		{
			GUI_ShopItem* newShopItem = new GUI_ShopItem(iRect(133, 7, 32, 32), 0, 0, SHOP_ITEM_DISPLAY_EVENTID);
			newShopItem->startEvents();
			pContent2->addChild(newShopItem);
		}
	}
	if(pContainer)
	{
		Sint32 PosY = -9;
		UTIL_sortShopItems();
		pContainer->clearChilds(false);
		for(size_t i = 0, end = g_tempNPCItems.size(); i < end; ++i)
		{
			NPCTradeItems& tradeItem = g_tempNPCItems[i];
			Uint32 price = (g_buyNPCPage ? tradeItem.itemBuyPrice : tradeItem.itemSellPrice);
			bool condition = ((price > 0 && price <= SDL_MAX_UINT32 - 5) ? true : false);//Fix for older versions which use signed int instead of unsigned(it send negative values when the item shouldn't display)
			if(condition)
			{
				Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "Price: %u, %u.%02u oz", price, tradeItem.itemWeight / 100, tradeItem.itemWeight % 100);
				GUI_ShopSelectItem* newShopItem = new GUI_ShopSelectItem(iRect(0, PosY, 156, 36), tradeItem.itemName, std::string(g_buffer, SDL_static_cast(size_t, len)), tradeItem.itemId, tradeItem.itemSubtype, i);
				newShopItem->startEvents();
				pContainer->addChild(newShopItem, false);
				PosY += 36;
			}
		}
		pContainer->validateScrollBar();
	}
}

void UTIL_createShopWindow(const std::string& name, Uint16 currencyId, void* itemsData)
{
	std::vector<NPCTradeItems>& itemVector = *SDL_reinterpret_cast(std::vector<NPCTradeItems>*, itemsData);
	g_tempNPCItems = std::move(itemVector);
	g_tempNPCCurrencyId = currencyId;
	g_buyNPCPage = true;

	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
	if(pPanel)
	{
		UTIL_recreateShopWindow(name, currencyId, pPanel);
		return;
	}

	Sint32 defaultHeight = 72;
	Sint32 contentHeight = (currencyId == 0 ? 30 : 42);
	GUI_PanelWindow* newWindow = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, 89 + contentHeight + defaultHeight), true, GUI_PANEL_WINDOW_SHOP, true);
	newWindow->setEventCallback(&shop_Events, SHOP_RESIZE_WIDTH_EVENTID, SHOP_RESIZE_HEIGHT_EVENTID, SHOP_EXIT_WINDOW_EVENTID);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(2, 0, GUI_UI_ICON_TRADEWIDGET_W, GUI_UI_ICON_TRADEWIDGET_H), GUI_UI_IMAGE, GUI_UI_ICON_TRADEWIDGET_X, GUI_UI_ICON_TRADEWIDGET_Y);
	newWindow->addChild(newImage);
	GUI_Icon* newIcon = new GUI_Icon(iRect(147, 0, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, SHOP_MAXIMINI_EVENTID, "Maximise or minimise window");
	newIcon->setButtonEventCallback(&shop_Events, SHOP_MAXIMINI_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(159, 0, GUI_UI_ICON_CLOSE_WINDOW_UP_W, GUI_UI_ICON_CLOSE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_CLOSE_WINDOW_UP_X, GUI_UI_ICON_CLOSE_WINDOW_UP_Y, GUI_UI_ICON_CLOSE_WINDOW_DOWN_X, GUI_UI_ICON_CLOSE_WINDOW_DOWN_Y, 0, "Close this window");
	newIcon->setButtonEventCallback(&shop_Events, SHOP_CLOSE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(131, 0, GUI_UI_ICON_SEARCH_WINDOW_UP_W, GUI_UI_ICON_SEARCH_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_SEARCH_WINDOW_UP_X, GUI_UI_ICON_SEARCH_WINDOW_UP_Y, GUI_UI_ICON_SEARCH_WINDOW_DOWN_X, GUI_UI_ICON_SEARCH_WINDOW_DOWN_Y, SHOP_CONFIGURE_EVENTID, "Click here to configure the NPC trade");
	newIcon->setButtonEventCallback(&shop_Events, SHOP_CONFIGURE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(19, 2, 100, 14), SHOP_TITLE, SHOP_TITLE_EVENTID, 144, 144, 144);
	newWindow->addChild(newLabel);
	GUI_Content* newContent = new GUI_Content(iRect(2, 13, 168, contentHeight), SHOP_CONTENT1_EVENTID);
	newContent->startEvents();
	newWindow->addChild(newContent);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(0, contentHeight - 2, 168, 2));
	newContent->addChild(newSeparator);
	newContent = new GUI_Content(iRect(2, 13 + contentHeight + defaultHeight, 168, 70), SHOP_CONTENT2_EVENTID);
	newContent->startEvents();
	newWindow->addChild(newContent);
	newSeparator = new GUI_Separator(iRect(0, 0, 168, 2));
	newContent->addChild(newSeparator);
	GUI_ShopContainer* newContainer = new GUI_ShopContainer(iRect(2, 13 + contentHeight, 168, defaultHeight), newWindow, SHOP_CONTAINER_EVENTID);
	newContainer->startEvents();
	newWindow->addChild(newContainer);
	UTIL_recreateShopWindow(name, currencyId, newWindow);
	if(!g_engine.addToPanel(newWindow))
		delete newWindow;
}

void UTIL_goodsShopWindow(Uint64 playerMoney, void* itemsData)
{
	NPCTradeGoods& npcGoods = *SDL_reinterpret_cast(NPCTradeGoods*, itemsData);
	g_tempNPCGoods = std::move(npcGoods);
	g_tempNPCMoney = playerMoney;
	UTIL_updateShopWindow();
}

void UTIL_closeShopWindow()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SHOP);
	if(pPanel)
	{
		GUI_Panel* parent = pPanel->getParent();
		if(!parent)
			return;

		parent->removePanel(pPanel);
	}
}

void GUI_ShopContainer::onRMouseDown(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	if(m_scrollBar.getRect().isPointInside(x, y))
	{
		m_scrollBar.onRMouseDown(x, y);
		return;
	}

	m_rmouse = true;
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

void GUI_ShopContainer::onRMouseUp(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	m_scrollBar.onRMouseUp(x, y);
	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		if((*it)->isEventable())
			(*it)->onRMouseUp(x, y);
	}

	if(m_rmouse && isInsideRect(x, y))
		UTIL_createShopPopupMenu(x, y);;

	m_rmouse = false;
}

void GUI_ShopContainer::render()
{
	g_engine.getRender()->fillRectangle(m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2, 64, 64, 64, 255);
	GUI_Container::render();
}

GUI_ShopItem::GUI_ShopItem(iRect boxRect, Uint16 itemId, Uint16 itemCount, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
	m_item = ItemUI::createItemUI(itemId, 1);
	if(m_item)
		m_item->setSubtype(itemCount, false);
}

GUI_ShopItem::~GUI_ShopItem()
{
	if(m_item)
		delete m_item;
}

void GUI_ShopItem::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
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

void GUI_ShopItem::onLMouseDown(Sint32, Sint32)
{
	m_haveMouse = 1;
}

void GUI_ShopItem::onLMouseUp(Sint32, Sint32)
{
	if(m_haveMouse > 0)
	{
		if(m_haveMouse == 1 && m_item)
			g_game.sendLookInShop(m_item->getID(), m_item->getItemCount());

		m_haveMouse = 0;
	}
}

void GUI_ShopItem::onRMouseDown(Sint32, Sint32)
{
	m_haveMouse = 1;
}

void GUI_ShopItem::onRMouseUp(Sint32, Sint32)
{
	if(m_haveMouse > 0)
	{
		if(m_haveMouse == 1 && m_item)
			g_game.sendLookInShop(m_item->getID(), m_item->getItemCount());

		m_haveMouse = 0;
	}
}

void GUI_ShopItem::setItemData(Uint16 itemId, Uint16 itemCount)
{
	if(m_item)
		delete m_item;

	m_item = ItemUI::createItemUI(itemId, 1);
	if(m_item)
		m_item->setSubtype(itemCount, false);
}

void GUI_ShopItem::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_INVENTORY_EMPTY_X, GUI_UI_INVENTORY_EMPTY_Y, m_tRect.x1 - 1, m_tRect.y1 - 1, m_tRect.x2 + 2, m_tRect.y2 + 2);
	if(m_item)
		m_item->render(m_tRect.x1, m_tRect.y1, m_tRect.y2);
}

GUI_ShopSelectItem::GUI_ShopSelectItem(iRect boxRect, const std::string& itemName, const std::string& itemDescription, Uint16 itemId, Uint8 itemSubtype, size_t index, Uint32 internalID) :
	m_name(iRect(0, 0, 110, 14), itemName), m_description(iRect(0, 0, 110, 14), itemDescription), m_index(index)
{
	m_internalID = internalID;
	m_item = ItemUI::createItemUI(itemId, SDL_static_cast(Uint16, itemSubtype));
	setRect(boxRect);
}

GUI_ShopSelectItem::~GUI_ShopSelectItem()
{
	if(m_item)
		delete m_item;
}

void GUI_ShopSelectItem::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	bool condition = (m_name.isFullDisplay() ? false : true);
	condition = (m_description.isFullDisplay() ? condition : true);
	if(condition)
	{
		bool inside = (isInsideParent && m_tRect.isPointInside(x, y));
		if(inside)
		{
			std::string description;
			description.assign(m_name.getName()).append(1, '\n').append(m_description.getName());
			g_engine.showDescription(x, y, description);
		}
	}
}

void GUI_ShopSelectItem::onLMouseDown(Sint32, Sint32)
{
	g_CanNPCTrade = false;//We don't know if we can buy/sell this item
	g_NPCActiveOffer = m_index;
	UTIL_updateShopWindow();
}

void GUI_ShopSelectItem::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	iRect nRect = iRect(NewRect.x1 + 40, NewRect.y1 + 7, 110, 14);
	m_name.setRect(nRect);
	nRect.y1 = NewRect.y1 + 21;
	m_description.setRect(nRect);
	m_tRect = NewRect;
}

void GUI_ShopSelectItem::render()
{
	auto& renderer = g_engine.getRender();
	if(g_NPCActiveOffer == m_index)
		g_engine.getRender()->fillRectangle(m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2, 112, 112, 112, 255);

	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_INVENTORY_EMPTY_X, GUI_UI_INVENTORY_EMPTY_Y, m_tRect.x1 + 3, m_tRect.y1 + 1, 34, 34);
	if(m_item)
		m_item->render(m_tRect.x1 + 4, m_tRect.y1 + 2, 32);

	m_name.render();
	m_description.render();
}
