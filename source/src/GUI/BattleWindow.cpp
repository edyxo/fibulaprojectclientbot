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
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../GUI_Elements/GUI_Container.h"
#include "../GUI_Elements/GUI_ContextMenu.h"
#include "../GUI_Elements/GUI_Flash.h"
#include "../game.h"
#include "../map.h"
#include "../tile.h"
#include "../creature.h"
#include "BattleWindow.h"

#define BATTLE_TITLE "Battle List"
#define BATTLE_MAXIMINI_EVENTID 1000
#define BATTLE_CLOSE_EVENTID 1001
#define BATTLE_CONFIGURE_EVENTID 1002
#define BATTLE_RESIZE_WIDTH_EVENTID 1003
#define BATTLE_RESIZE_HEIGHT_EVENTID 1004
#define BATTLE_EXIT_WINDOW_EVENTID 1005
#define BATTLE_CONTAINER_EVENTID 1006
#define BATTLE_FLASH_EVENTID 1007

#define SORT_ASCENDING_TIME_EVENTID 1500
#define SORT_DESCENDING_TIME_EVENTID 1501
#define SORT_ASCENDING_DISTANCE_EVENTID 1502
#define SORT_DESCENDING_DISTANCE_EVENTID 1503
#define SORT_ASCENDING_HEALTH_EVENTID 1504
#define SORT_DESCENDING_HEALTH_EVENTID 1505
#define SORT_ASCENDING_NAME_EVENTID 1506
#define SORT_DESCENDING_NAME_EVENTID 1507

#define PARTY_TITLE "Party List"
#define PARTY_MAXIMINI_EVENTID 2000
#define PARTY_CLOSE_EVENTID 2001
#define PARTY_CONFIGURE_EVENTID 2002
#define PARTY_RESIZE_WIDTH_EVENTID 2003
#define PARTY_RESIZE_HEIGHT_EVENTID 2004
#define PARTY_EXIT_WINDOW_EVENTID 2005
#define PARTY_CONTAINER_EVENTID 2006

extern Engine g_engine;
extern Game g_game;
extern Map g_map;

bool g_haveBattleOpen = false;
bool g_havePartyOpen = false;
bool g_needSortCreatures = true;
bool g_recreateBattleWindow = false;
bool g_recreatePartyWindow = false;
Uint8 g_mouseAction = 0;
std::vector<Creature*> g_battleCreatures;

void battle_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case BATTLE_MAXIMINI_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
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

					GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(BATTLE_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeInvisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(BATTLE_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_X, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_Y);

					GUI_Flash* pFlash = SDL_static_cast(GUI_Flash*, pPanel->getChild(BATTLE_FLASH_EVENTID));
					if(pFlash)
					{
						iRect cRect = pFlash->getRect();
						cRect.y2 = 19;
						pFlash->setRect(cRect);
					}
				}
				else
				{
					Sint32 cachedHeight = pPanel->getCachedHeight();
					parent->tryFreeHeight(cachedHeight - pRect.y2);
					pPanel->setSize(pRect.x2, cachedHeight);
					parent->checkPanels();

					GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(BATTLE_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeVisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(BATTLE_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y);

					GUI_Flash* pFlash = SDL_static_cast(GUI_Flash*, pPanel->getChild(BATTLE_FLASH_EVENTID));
					if(pFlash)
					{
						iRect cRect = pFlash->getRect();
						cRect.y2 = pPanel->getCachedHeight();
						pFlash->setRect(cRect);
					}
				}
			}
		}
		break;
		case BATTLE_CLOSE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
			if(pPanel)
			{
				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				parent->removePanel(pPanel);
			}
		}
		break;
		case BATTLE_CONFIGURE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
			if(pPanel)
			{
				GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(BATTLE_CONFIGURE_EVENTID));
				if(pIcon)
				{
					iRect& iconRect = pIcon->getRect();
					UTIL_createBattlePopupMenu(iconRect.x1, iconRect.y1 + 12);
				}
			}
		}
		break;
		case BATTLE_RESIZE_HEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
			if(pPanel)
			{
				GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(BATTLE_CONTAINER_EVENTID));
				if(pContainer)
				{
					iRect cRect = pContainer->getRect();
					cRect.y2 = status - 19;
					pContainer->setRect(cRect);
				}
				
				GUI_Flash* pFlash = SDL_static_cast(GUI_Flash*, pPanel->getChild(BATTLE_FLASH_EVENTID));
				if(pFlash)
				{
					iRect cRect = pFlash->getRect();
					cRect.y2 = status;
					pFlash->setRect(cRect);
				}
			}
		}
		break;
		case BATTLE_EXIT_WINDOW_EVENTID: {g_engine.setContentWindowHeight(GUI_PANEL_WINDOW_BATTLE, status - 19); g_haveBattleOpen = false;} break;
		case BATTLE_FLASH_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
			if(pPanel)
			{
				GUI_Flash* pFlash = SDL_static_cast(GUI_Flash*, pPanel->getChild(BATTLE_FLASH_EVENTID));
				if(pFlash)
					pPanel->removeChild(pFlash);
			}
		}
		break;

		case SORT_ASCENDING_TIME_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Ascending_Time)
			{
				g_engine.setBattleSortMethod(Sort_Ascending_Time);
				UTIL_sortBattleWindow();
			}
		}
		break;
		case SORT_DESCENDING_TIME_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Descending_Time)
			{
				g_engine.setBattleSortMethod(Sort_Descending_Time);
				UTIL_sortBattleWindow();
			}
		}
		break;
		case SORT_ASCENDING_DISTANCE_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Ascending_Distance)
			{
				g_engine.setBattleSortMethod(Sort_Ascending_Distance);
				UTIL_sortBattleWindow();
			}
		}
		break;
		case SORT_DESCENDING_DISTANCE_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Descending_Distance)
			{
				g_engine.setBattleSortMethod(Sort_Descending_Distance);
				UTIL_sortBattleWindow();
			}
		}
		break;
		case SORT_ASCENDING_HEALTH_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Ascending_HP)
			{
				g_engine.setBattleSortMethod(Sort_Ascending_HP);
				UTIL_sortBattleWindow();
			}
		}
		break;
		case SORT_DESCENDING_HEALTH_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Descending_HP)
			{
				g_engine.setBattleSortMethod(Sort_Descending_HP);
				UTIL_sortBattleWindow();
			}
		}
		break;
		case SORT_ASCENDING_NAME_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Ascending_Name)
			{
				g_engine.setBattleSortMethod(Sort_Ascending_Name);
				UTIL_sortBattleWindow();
			}
		}
		break;
		case SORT_DESCENDING_NAME_EVENTID:
		{
			if(g_engine.getBattleSortMethod() != Sort_Descending_Name)
			{
				g_engine.setBattleSortMethod(Sort_Descending_Name);
				UTIL_sortBattleWindow();
			}
		}
		break;

		case PARTY_MAXIMINI_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_PARTY);
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

					GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(PARTY_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeInvisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(PARTY_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_X, GUI_UI_ICON_MAXIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MAXIMIZE_WINDOW_DOWN_Y);
				}
				else
				{
					Sint32 cachedHeight = pPanel->getCachedHeight();
					parent->tryFreeHeight(cachedHeight - pRect.y2);
					pPanel->setSize(pRect.x2, cachedHeight);
					parent->checkPanels();

					GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(PARTY_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeVisible();

					GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(PARTY_MAXIMINI_EVENTID));
					if(pIcon)
						pIcon->setData(GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y);
				}
			}
		}
		break;
		case PARTY_CLOSE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_PARTY);
			if(pPanel)
			{
				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				parent->removePanel(pPanel);
			}
		}
		break;
		case PARTY_CONFIGURE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_PARTY);
			if(pPanel)
			{
				GUI_Icon* pIcon = SDL_static_cast(GUI_Icon*, pPanel->getChild(PARTY_CONFIGURE_EVENTID));
				if(pIcon)
				{
					iRect& iconRect = pIcon->getRect();
					UTIL_createBattlePopupMenu(iconRect.x1, iconRect.y1 + 12);
				}
			}
		}
		break;
		case PARTY_RESIZE_HEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_PARTY);
			if(pPanel)
			{
				GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(PARTY_CONTAINER_EVENTID));
				if(pContainer)
				{
					iRect cRect = pContainer->getRect();
					cRect.y2 = status - 19;
					pContainer->setRect(cRect);
				}
			}
		}
		break;
		case PARTY_EXIT_WINDOW_EVENTID: {g_engine.setContentWindowHeight(GUI_PANEL_WINDOW_PARTY, status - 19); g_havePartyOpen = false;} break;
		default: break;
	}
}

void UTIL_sortWindows()
{
	switch(g_engine.getBattleSortMethod())
	{
		case Sort_Ascending_Time: std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool {return a->getVisibleTime() < b->getVisibleTime();}); break;
		case Sort_Descending_Time: std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool {return a->getVisibleTime() > b->getVisibleTime();}); break;
		case Sort_Ascending_Distance:
		{
			std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool
			{
				Sint32 a_distance = UTIL_max<Sint32>(Position::getDistanceX(a->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(a->getCurrentPosition(), g_map.getCentralPosition()));
				Sint32 b_distance = UTIL_max<Sint32>(Position::getDistanceX(b->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(b->getCurrentPosition(), g_map.getCentralPosition()));
				return a_distance < b_distance;
			});
		}
		break;
		case Sort_Descending_Distance:
		{
			std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool
			{
				Sint32 a_distance = UTIL_max<Sint32>(Position::getDistanceX(a->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(a->getCurrentPosition(), g_map.getCentralPosition()));
				Sint32 b_distance = UTIL_max<Sint32>(Position::getDistanceX(b->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(b->getCurrentPosition(), g_map.getCentralPosition()));
				return a_distance > b_distance;
			});
		}
		break;
		case Sort_Ascending_HP: std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool {return a->getHealth() < b->getHealth();}); break;
		case Sort_Descending_HP: std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool {return a->getHealth() > b->getHealth();}); break;
		case Sort_Ascending_Name: std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool {return a->getName() < b->getName();}); break;
		case Sort_Descending_Name: std::sort(g_battleCreatures.begin(), g_battleCreatures.end(), [](Creature* a, Creature* b) -> bool {return a->getName() > b->getName();}); break;
		default: break;
	}
	g_recreateBattleWindow = true;
	g_recreatePartyWindow = true;
}

void UTIL_sortBattleWindow()
{
	g_needSortCreatures = true;
}

void UTIL_addBattleCreature(void* creature)
{
	//We need to have cache of every creature for Party List widget
	SortMethods sortMethod = g_engine.getBattleSortMethod();
	if(sortMethod != Sort_Ascending_Time && sortMethod != Sort_Descending_Time)
	{
		for(std::vector<Creature*>::iterator it = g_battleCreatures.begin(), end = g_battleCreatures.end(); it != end; ++it)
		{
			Creature* exCreature = (*it);
			switch(sortMethod)
			{
				case Sort_Ascending_Distance:
				{
					Sint32 a_distance = UTIL_max<Sint32>(Position::getDistanceX(SDL_reinterpret_cast(Creature*, creature)->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(SDL_reinterpret_cast(Creature*, creature)->getCurrentPosition(), g_map.getCentralPosition()));
					Sint32 b_distance = UTIL_max<Sint32>(Position::getDistanceX(exCreature->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(exCreature->getCurrentPosition(), g_map.getCentralPosition()));
					if(a_distance < b_distance)
					{
						g_battleCreatures.insert(it, SDL_reinterpret_cast(Creature*, creature));
						goto Skip_Loop;
					}
				}
				break;
				case Sort_Descending_Distance:
				{
					Sint32 a_distance = UTIL_max<Sint32>(Position::getDistanceX(SDL_reinterpret_cast(Creature*, creature)->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(SDL_reinterpret_cast(Creature*, creature)->getCurrentPosition(), g_map.getCentralPosition()));
					Sint32 b_distance = UTIL_max<Sint32>(Position::getDistanceX(exCreature->getCurrentPosition(), g_map.getCentralPosition()), Position::getDistanceY(exCreature->getCurrentPosition(), g_map.getCentralPosition()));
					if(a_distance > b_distance)
					{
						g_battleCreatures.insert(it, SDL_reinterpret_cast(Creature*, creature));
						goto Skip_Loop;
					}
				}
				break;
				case Sort_Ascending_HP:
				{
					if(SDL_reinterpret_cast(Creature*, creature)->getHealth() < exCreature->getHealth())
					{
						g_battleCreatures.insert(it, SDL_reinterpret_cast(Creature*, creature));
						goto Skip_Loop;
					}
				}
				break;
				case Sort_Descending_HP:
				{
					if(SDL_reinterpret_cast(Creature*, creature)->getHealth() > exCreature->getHealth())
					{
						g_battleCreatures.insert(it, SDL_reinterpret_cast(Creature*, creature));
						goto Skip_Loop;
					}
				}
				break;
				case Sort_Ascending_Name:
				{
					if(SDL_reinterpret_cast(Creature*, creature)->getName() < exCreature->getName())
					{
						g_battleCreatures.insert(it, SDL_reinterpret_cast(Creature*, creature));
						goto Skip_Loop;
					}
				}
				break;
				case Sort_Descending_Name:
				{
					if(SDL_reinterpret_cast(Creature*, creature)->getName() > exCreature->getName())
					{
						g_battleCreatures.insert(it, SDL_reinterpret_cast(Creature*, creature));
						goto Skip_Loop;
					}
				}
				break;
				default: break;
			}
		}
	}
	g_battleCreatures.push_back(SDL_reinterpret_cast(Creature*, creature));

	Skip_Loop:
	g_recreateBattleWindow = true;
	g_recreatePartyWindow = true;
}

void UTIL_removeBattleCreature(void* creature)
{
	std::vector<Creature*>::iterator it = std::find(g_battleCreatures.begin(), g_battleCreatures.end(), SDL_reinterpret_cast(Creature*, creature));
	if(it != g_battleCreatures.end())
	{
		g_battleCreatures.erase(it);
		g_recreateBattleWindow = true;
		g_recreatePartyWindow = true;
	}
}

void UTIL_resetBattleCreatures()
{
	g_battleCreatures.clear();
	g_recreateBattleWindow = true;
	g_recreatePartyWindow = true;
}

void UTIL_refreshBattleWindow()
{
	for(std::vector<Creature*>::iterator it = g_battleCreatures.begin(), end = g_battleCreatures.end(); it != end; ++it)
	{
		Creature* creature = (*it);
		if(creature->hasNeedUpdate())
		{
			if(!creature->isVisible())
			{
				creature->setVisible(true);
				g_recreateBattleWindow = true;
			}

			creature->setNeedUpdate(false);
		}
		else if(creature->isVisible())
		{
			if(g_game.getSelectID() == creature->getId())
				g_game.setSelectID(0);

			creature->setVisible(false);
			g_recreateBattleWindow = true;
		}
	}

	if(g_recreateBattleWindow)
	{
		SortMethods sortMethod = g_engine.getBattleSortMethod();
		if(sortMethod == Sort_Ascending_Time || sortMethod == Sort_Descending_Time)
			UTIL_sortBattleWindow();
	}
}

void UTIL_attackNextBattleCreature()
{
	if(g_needSortCreatures)
	{
		UTIL_sortWindows();
		g_needSortCreatures = false;
	}

	std::vector<Creature*> visibleCreatures;
	visibleCreatures.reserve(g_battleCreatures.size());
	for(Creature* creature : g_battleCreatures)
	{
		if(creature && !creature->isLocalCreature() && creature->isVisible())
			visibleCreatures.push_back(creature);
	}

	if(visibleCreatures.empty())
		return;

	const Uint32 currentTarget = g_game.getAttackID();
	size_t nextIndex = 0;
	for(size_t i = 0; i < visibleCreatures.size(); ++i)
	{
		if(visibleCreatures[i]->getId() == currentTarget)
		{
			nextIndex = (i + 1) % visibleCreatures.size();
			break;
		}
	}

	g_game.sendAttack(visibleCreatures[nextIndex]);
}

void UTIL_refreshPartyWindow()
{
	g_recreatePartyWindow = true;
}

bool CanSeeOnBattle(Creature* creature)
{
	return !creature->isLocalCreature() && creature->isVisible();
}

void UTIL_recreateBattleWindow(GUI_Container* container)
{
	if(!container)
	{
		GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
		if(pPanel)
			container = SDL_static_cast(GUI_Container*, pPanel->getChild(BATTLE_CONTAINER_EVENTID));
		if(!container)
			return;
	}
	container->clearChilds(false);
	Sint32 PosY = -9;
	size_t creatures = g_battleCreatures.size();
	for(size_t i = 0; i < creatures; ++i)
	{
		if(CanSeeOnBattle(g_battleCreatures[i]))
		{
			GUI_BattleCreature* newBattleCreature = new GUI_BattleCreature(iRect(1, PosY, 155, 23), i);
			newBattleCreature->startEvents();
			container->addChild(newBattleCreature, false);
			PosY += 23;
		}
	}
	container->validateScrollBar();
}

void UTIL_flashBattleWindow()
{
	if(!g_haveBattleOpen)
		UTIL_toggleBattleWindow();
		
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
	if(pPanel)
	{
		GUI_Flash* pFlash = SDL_static_cast(GUI_Flash*, pPanel->getChild(BATTLE_FLASH_EVENTID));
		if(pFlash)
			pFlash->reset();
		else
		{
			iRect& pRect = pPanel->getOriginalRect();
			pFlash = new GUI_Flash(iRect(0, 0, pRect.x2, pRect.y2), BATTLE_FLASH_EVENTID);
			pFlash->setEndEventCallback(&battle_Events, BATTLE_FLASH_EVENTID);
			pPanel->addChild(pFlash);
		}
	}
}

void UTIL_toggleBattleWindow()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_BATTLE);
	if(pPanel)
	{
		g_engine.removePanelWindow(pPanel);
		return;
	}
	
	if(g_needSortCreatures)
	{
		UTIL_sortWindows();
		g_needSortCreatures = false;
	}
	
	Sint32 savedHeight = g_engine.getContentWindowHeight(GUI_PANEL_WINDOW_BATTLE);
	if(savedHeight < 38)
		savedHeight = 119;
	GUI_PanelWindow* newWindow = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, savedHeight + 19), true, GUI_PANEL_WINDOW_BATTLE, true);
	newWindow->setEventCallback(&battle_Events, BATTLE_RESIZE_WIDTH_EVENTID, BATTLE_RESIZE_HEIGHT_EVENTID, BATTLE_EXIT_WINDOW_EVENTID);
	GUI_BattleChecker* newBattleChecker = new GUI_BattleChecker(iRect(19, 2, 0, 0));
	newWindow->addChild(newBattleChecker);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(2, 0, GUI_UI_ICON_BATTLELIST_W, GUI_UI_ICON_BATTLELIST_H), GUI_UI_IMAGE, GUI_UI_ICON_BATTLELIST_X, GUI_UI_ICON_BATTLELIST_Y);
	newWindow->addChild(newImage);
	GUI_Icon* newIcon = new GUI_Icon(iRect(147, 0, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, BATTLE_MAXIMINI_EVENTID, "Maximise or minimise window");
	newIcon->setButtonEventCallback(&battle_Events, BATTLE_MAXIMINI_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(159, 0, GUI_UI_ICON_CLOSE_WINDOW_UP_W, GUI_UI_ICON_CLOSE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_CLOSE_WINDOW_UP_X, GUI_UI_ICON_CLOSE_WINDOW_UP_Y, GUI_UI_ICON_CLOSE_WINDOW_DOWN_X, GUI_UI_ICON_CLOSE_WINDOW_DOWN_Y, 0, "Close this window");
	newIcon->setButtonEventCallback(&battle_Events, BATTLE_CLOSE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(131, 0, GUI_UI_ICON_SEARCH_WINDOW_UP_W, GUI_UI_ICON_SEARCH_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_SEARCH_WINDOW_UP_X, GUI_UI_ICON_SEARCH_WINDOW_UP_Y, GUI_UI_ICON_SEARCH_WINDOW_DOWN_X, GUI_UI_ICON_SEARCH_WINDOW_DOWN_Y, BATTLE_CONFIGURE_EVENTID, "Click here to configure the battle list");
	newIcon->setButtonEventCallback(&battle_Events, BATTLE_CONFIGURE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(19, 2, 100, 14), BATTLE_TITLE, 0, 144, 144, 144);
	newWindow->addChild(newLabel);
	GUI_Container* newContainer = new GUI_Container(iRect(2, 13, 168, savedHeight), newWindow, BATTLE_CONTAINER_EVENTID);
	UTIL_recreateBattleWindow(newContainer);
	newContainer->startEvents();
	newWindow->addChild(newContainer);

	Sint32 preferredPanel = g_engine.getContentWindowParent(GUI_PANEL_WINDOW_BATTLE);
	bool added = g_engine.addToPanel(newWindow, preferredPanel);
	if(!added && preferredPanel != GUI_PANEL_RANDOM)
		added = g_engine.addToPanel(newWindow, GUI_PANEL_RANDOM);

	if(added)
	{
		g_recreateBattleWindow = false;
		g_haveBattleOpen = true;
	}
	else
	{
		g_recreateBattleWindow = true;
		g_haveBattleOpen = false;
		delete newWindow;
	}
}

void UTIL_createBattlePopupMenu(Sint32 x, Sint32 y)
{
	SortMethods sortMethod = g_engine.getBattleSortMethod();
	GUI_ContextMenu* newMenu = new GUI_ContextMenu();
	newMenu->addContextMenu((sortMethod == Sort_Ascending_Time ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_ASCENDING_TIME_EVENTID, "Sort Ascending by Display Time", "");
	newMenu->addContextMenu((sortMethod == Sort_Descending_Time ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_DESCENDING_TIME_EVENTID, "Sort Descending by Display Time", "");
	newMenu->addContextMenu((sortMethod == Sort_Ascending_Distance ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_ASCENDING_DISTANCE_EVENTID, "Sort Ascending by Distance", "");
	newMenu->addContextMenu((sortMethod == Sort_Descending_Distance ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_DESCENDING_DISTANCE_EVENTID, "Sort Descending by Distance", "");
	newMenu->addContextMenu((sortMethod == Sort_Ascending_HP ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_ASCENDING_HEALTH_EVENTID, "Sort Ascending by Hit Points", "");
	newMenu->addContextMenu((sortMethod == Sort_Descending_HP ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_DESCENDING_HEALTH_EVENTID, "Sort Descending by Hit Points", "");
	newMenu->addContextMenu((sortMethod == Sort_Ascending_Name ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_ASCENDING_NAME_EVENTID, "Sort Ascending by Name", "");
	newMenu->addContextMenu((sortMethod == Sort_Descending_Name ? CONTEXTMENU_STYLE_CHECKED : CONTEXTMENU_STYLE_UNCHECKED), SORT_DESCENDING_NAME_EVENTID, "Sort Descending by Name", "");
	newMenu->setEventCallback(&battle_Events);
	g_engine.showContextMenu(newMenu, x, y);
}

bool CanSeeOnParty(Creature* creature)
{
	return !creature->isLocalCreature() && UTIL_isPartyMember(creature->getShield());
}

void UTIL_recreatePartyWindow(GUI_Container* container)
{
	if(!container)
	{
		GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_PARTY);
		if(pPanel)
			container = SDL_static_cast(GUI_Container*, pPanel->getChild(PARTY_CONTAINER_EVENTID));
		if(!container)
			return;
	}
	container->clearChilds(false);
	Sint32 PosY = -9;
	size_t creatures = g_battleCreatures.size();
	for(size_t i = 0; i < creatures; ++i)
	{
		if(CanSeeOnParty(g_battleCreatures[i]))
		{
			GUI_BattleCreature* newBattleCreature = new GUI_BattleCreature(iRect(1, PosY, 155, 30), i, true);
			newBattleCreature->startEvents();
			container->addChild(newBattleCreature, false);
			PosY += 30;
		}
	}
	container->validateScrollBar();
}

void UTIL_togglePartyWindow()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_PARTY);
	if(pPanel)
	{
		g_engine.removePanelWindow(pPanel);
		return;
	}
	
	if(g_needSortCreatures)
	{
		UTIL_sortWindows();
		g_needSortCreatures = false;
	}
	
	Sint32 savedHeight = g_engine.getContentWindowHeight(GUI_PANEL_WINDOW_PARTY);
	if(savedHeight < 38)
		savedHeight = 119;
	GUI_PanelWindow* newWindow = new GUI_PanelWindow(iRect(0, 0, 172, savedHeight + 19), true, GUI_PANEL_WINDOW_PARTY, true);
	newWindow->setEventCallback(&battle_Events, PARTY_RESIZE_WIDTH_EVENTID, PARTY_RESIZE_HEIGHT_EVENTID, PARTY_EXIT_WINDOW_EVENTID);
	GUI_PartyChecker* newPartyChecker = new GUI_PartyChecker(iRect(19, 2, 0, 0));
	newWindow->addChild(newPartyChecker);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(2, 0, GUI_UI_ICON_BATTLELIST_W, GUI_UI_ICON_BATTLELIST_H), GUI_UI_IMAGE, GUI_UI_ICON_BATTLELIST_X, GUI_UI_ICON_BATTLELIST_Y);
	newWindow->addChild(newImage);
	GUI_Icon* newIcon = new GUI_Icon(iRect(147, 0, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, PARTY_MAXIMINI_EVENTID, "Maximise or minimise window");
	newIcon->setButtonEventCallback(&battle_Events, PARTY_MAXIMINI_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(159, 0, GUI_UI_ICON_CLOSE_WINDOW_UP_W, GUI_UI_ICON_CLOSE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_CLOSE_WINDOW_UP_X, GUI_UI_ICON_CLOSE_WINDOW_UP_Y, GUI_UI_ICON_CLOSE_WINDOW_DOWN_X, GUI_UI_ICON_CLOSE_WINDOW_DOWN_Y, 0, "Close this window");
	newIcon->setButtonEventCallback(&battle_Events, PARTY_CLOSE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(131, 0, GUI_UI_ICON_SEARCH_WINDOW_UP_W, GUI_UI_ICON_SEARCH_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_SEARCH_WINDOW_UP_X, GUI_UI_ICON_SEARCH_WINDOW_UP_Y, GUI_UI_ICON_SEARCH_WINDOW_DOWN_X, GUI_UI_ICON_SEARCH_WINDOW_DOWN_Y, PARTY_CONFIGURE_EVENTID, "Click here to configure the battle list");
	newIcon->setButtonEventCallback(&battle_Events, PARTY_CONFIGURE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_DynamicLabel* newLabel = new GUI_DynamicLabel(iRect(19, 2, 100, 14), PARTY_TITLE, 0, 144, 144, 144);
	newWindow->addChild(newLabel);
	GUI_Container* newContainer = new GUI_Container(iRect(2, 13, 168, savedHeight), newWindow, PARTY_CONTAINER_EVENTID);
	UTIL_recreatePartyWindow(newContainer);
	newContainer->startEvents();
	newWindow->addChild(newContainer);

	Sint32 preferredPanel = g_engine.getContentWindowParent(GUI_PANEL_WINDOW_PARTY);
	bool added = g_engine.addToPanel(newWindow, preferredPanel);
	if(!added && preferredPanel != GUI_PANEL_RANDOM)
		added = g_engine.addToPanel(newWindow, GUI_PANEL_RANDOM);

	if(added)
	{
		g_recreatePartyWindow = false;
		g_havePartyOpen = true;
	}
	else
	{
		g_recreatePartyWindow = true;
		g_havePartyOpen = false;
		delete newWindow;
	}
}

GUI_BattleChecker::GUI_BattleChecker(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_BattleChecker::render()
{
	if(g_needSortCreatures)
	{
		UTIL_sortWindows();
		g_needSortCreatures = false;
	}
	if(g_recreateBattleWindow)
	{
		UTIL_recreateBattleWindow(NULL);
		g_recreateBattleWindow = false;
	}
}

GUI_PartyChecker::GUI_PartyChecker(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_PartyChecker::render()
{
	if(g_needSortCreatures)
	{
		UTIL_sortWindows();
		g_needSortCreatures = false;
	}
	if(g_recreatePartyWindow)
	{
		UTIL_recreatePartyWindow(NULL);
		g_recreatePartyWindow = false;
	}
}

GUI_BattleCreature::GUI_BattleCreature(iRect boxRect, size_t index, bool partyWindow, Uint32 internalID) : m_index(index), m_partyWindow(partyWindow)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_BattleCreature::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	Creature* creature = g_battleCreatures[m_index];
	if(isInsideParent && m_tRect.isPointInside(x, y))
		g_game.setSelectID(creature->getId());
	else if(g_game.getSelectID() == creature->getId())
		g_game.setSelectID(0);
}

void GUI_BattleCreature::onLMouseDown(Sint32, Sint32)
{
	if(g_mouseAction == 1 && g_engine.hasClassicControl())
		g_mouseAction = 2;
	else
		g_mouseAction = 1;
}

void GUI_BattleCreature::onLMouseUp(Sint32 x, Sint32 y)
{
	Uint32 selectCreatureID = g_game.getSelectID();
	if(selectCreatureID != 0 && g_mouseAction != 0)
	{
		Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
		if((mouseState & SDL_BUTTON_RMASK) && g_engine.hasClassicControl())
			return;

		Creature* creature = g_battleCreatures[m_index];
		if(selectCreatureID != creature->getId())
			return;

		Uint16 keyMods = UTIL_parseModifiers(SDL_static_cast(Uint16, SDL_GetModState()));
		if((m_partyWindow && keyMods != KMOD_CTRL) || g_mouseAction == 2 || keyMods == KMOD_SHIFT)
		{
			if(g_game.hasGameFeature(GAME_FEATURE_LOOKATCREATURE))
				g_game.sendLookInBattle(selectCreatureID);
			else
			{
				Position& position = creature->getCurrentPosition();
				Tile* creatureTile = g_map.getTile(position);
				if(creatureTile)
					g_game.sendLookAt(position, 0x62, SDL_static_cast(Uint8, creatureTile->getThingStackPos(creature)));
			}
		}
		else if(keyMods == KMOD_CTRL)
		{
			//set classic control to false so we display client shortcuts
			bool classicControl = g_engine.hasClassicControl();
			g_engine.setClassicControl(false);
			GUI_ContextMenu* newMenu = g_engine.createThingContextMenu(creature, NULL, NULL);
			newMenu->setEventCallback(&Engine::standardThingEvent);
			g_engine.showContextMenu(newMenu, x, y);
			g_engine.setClassicControl(classicControl);
		}
		else if(keyMods == KMOD_ALT || keyMods == KMOD_NONE)
			g_game.sendAttack((g_game.getAttackID() == creature->getId()) ? NULL : creature);

		g_mouseAction = 0;
	}
	else
		g_mouseAction = 0;
}

void GUI_BattleCreature::onRMouseDown(Sint32, Sint32)
{
	if(g_mouseAction == 1 && g_engine.hasClassicControl())
		g_mouseAction = 2;
	else
		g_mouseAction = 1;
}

void GUI_BattleCreature::onRMouseUp(Sint32 x, Sint32 y)
{
	Uint32 selectCreatureID = g_game.getSelectID();
	if(selectCreatureID != 0 && g_mouseAction != 0)
	{
		Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
		if((mouseState & SDL_BUTTON_LMASK) && g_engine.hasClassicControl())
			return;

		Creature* creature = g_battleCreatures[m_index];
		if(selectCreatureID != creature->getId())
			return;

		Uint16 keyMods = UTIL_parseModifiers(SDL_static_cast(Uint16, SDL_GetModState()));
		if((m_partyWindow && keyMods == KMOD_ALT) || g_mouseAction == 2 || keyMods == KMOD_SHIFT)
		{
			if(g_game.hasGameFeature(GAME_FEATURE_LOOKATCREATURE))
				g_game.sendLookInBattle(selectCreatureID);
			else
			{
				Position& position = creature->getCurrentPosition();
				Tile* creatureTile = g_map.getTile(position);
				if(creatureTile)
					g_game.sendLookAt(position, 0x62, SDL_static_cast(Uint8, creatureTile->getThingStackPos(creature)));
			}
		}
		else if(keyMods == KMOD_CTRL || keyMods == KMOD_NONE)
		{
			//set classic control to false so we display client shortcuts
			bool classicControl = g_engine.hasClassicControl();
			g_engine.setClassicControl(false);
			GUI_ContextMenu* newMenu = g_engine.createThingContextMenu(creature, NULL, NULL);
			newMenu->setEventCallback(&Engine::standardThingEvent);
			g_engine.showContextMenu(newMenu, x, y);
			g_engine.setClassicControl(classicControl);
		}
		else if(!m_partyWindow && keyMods == KMOD_ALT)
			g_game.sendAttack((g_game.getAttackID() == creature->getId()) ? NULL : creature);

		g_mouseAction = 0;
	}
	else
		g_mouseAction = 0;
}

void GUI_BattleCreature::render()
{
	Creature* creature = g_battleCreatures[m_index];
	creature->renderOnBattle(m_tRect.x1, m_tRect.y1, m_partyWindow);
}
