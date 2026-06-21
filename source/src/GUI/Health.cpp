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
#include "../GUI_Elements/GUI_Flash.h"
#include "../game.h"
#include "Health.h"

#define HEALTH_WIDTH (GAME_PANEL_FIXED_WIDTH - 4)
#define HEALTH_HEIGHT 32
#define HEALTH_FLASH_EVENTID 1000
#define HEALTH_MARK_X 9
#define HEALTH_MARK_Y 4
#define HEALTH_MARK_W GUI_UI_SYMBOL_HEALTH_W
#define HEALTH_MARK_H GUI_UI_SYMBOL_HEALTH_H
#define HEALTH_BAR_X 26
#define HEALTH_BAR_Y 4
#define HEALTH_BAR_W GUI_UI_BAR_HEALTH_W
#define HEALTH_BAR_H GUI_UI_BAR_HEALTH_H
#define HEALTH_BAR_EVENTID 1001
#define HEALTH_LABEL_X 126
#define HEALTH_LABEL_Y 4
#define HEALTH_LABEL_EVENTID 1002
#define HEALTH_PERCENT_X 71
#define HEALTH_PERCENT_Y 4
#define HEALTH_PERCENT_EVENTID 1003
#define MANA_MARK_X 10
#define MANA_MARK_Y 17
#define MANA_MARK_W GUI_UI_SYMBOL_MANA_W
#define MANA_MARK_H GUI_UI_SYMBOL_MANA_H
#define MANA_BAR_X 26
#define MANA_BAR_Y 17
#define MANA_BAR_W GUI_UI_BAR_MANA_W
#define MANA_BAR_H GUI_UI_BAR_MANA_H
#define MANA_BAR_EVENTID 1004
#define MANA_LABEL_X 126
#define MANA_LABEL_Y 17
#define MANA_LABEL_EVENTID 1005
#define MANA_PERCENT_X 71
#define MANA_PERCENT_Y 17
#define MANA_PERCENT_EVENTID 1006

extern Engine g_engine;
extern Game g_game;

void health_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case HEALTH_FLASH_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_HEALTH);
			if(pPanel)
			{
				GUI_Flash* pFlash = SDL_static_cast(GUI_Flash*, pPanel->getChild(HEALTH_FLASH_EVENTID));
				if(pFlash)
					pPanel->removeChild(pFlash);
			}
		}
		break;
		default: break;
	}
}

void UTIL_updateHealthPanel()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_HEALTH);
	if(!pPanel)
		return;

	if(g_game.hasCachedStat(CACHED_STAT_HEALTH))
	{
		GUI_Health* pHealth = SDL_static_cast(GUI_Health*, pPanel->getChild(HEALTH_BAR_EVENTID));
		if(pHealth)
			pHealth->setPercent(SDL_static_cast(Sint32, g_game.getPlayerHealthPercent()) * 90 / 100);

		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", g_game.getPlayerHealth());
		GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pPanel->getChild(HEALTH_LABEL_EVENTID));
		if(pLabel)
			pLabel->setName(std::string(g_buffer, SDL_static_cast(size_t, len)));

		pLabel = SDL_static_cast(GUI_Label*, pPanel->getChild(HEALTH_PERCENT_EVENTID));
		if(pLabel)
		{
			if(!g_engine.getHideHPManaPercent())
			{
				len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u%%", SDL_static_cast(Uint32, g_game.getPlayerHealthPercent()));
				pLabel->setName(std::string(g_buffer, SDL_static_cast(size_t, len)));
			}
			else
				pLabel->setName(std::string());
		}
	}

	if(g_game.hasCachedStat(CACHED_STAT_MANA))
	{
		GUI_Mana* pMana = SDL_static_cast(GUI_Mana*, pPanel->getChild(MANA_BAR_EVENTID));
		if(pMana)
			pMana->setPercent(SDL_static_cast(Sint32, g_game.getPlayerManaPercent()) * 90 / 100);

		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", g_game.getPlayerMana());
		GUI_Label* pLabel = SDL_static_cast(GUI_Label*, pPanel->getChild(MANA_LABEL_EVENTID));
		if(pLabel)
			pLabel->setName(std::string(g_buffer, SDL_static_cast(size_t, len)));

		pLabel = SDL_static_cast(GUI_Label*, pPanel->getChild(MANA_PERCENT_EVENTID));
		if(pLabel)
		{
			if(!g_engine.getHideHPManaPercent())
			{
				len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u%%", SDL_static_cast(Uint32, g_game.getPlayerManaPercent()));
				pLabel->setName(std::string(g_buffer, SDL_static_cast(size_t, len)));
			}
			else
				pLabel->setName(std::string());
		}
	}
}

void UTIL_createHealthPanel()
{
	bool newWindow;
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_HEALTH);
	if(pPanel)
	{
		newWindow = false;
		pPanel->clearChilds();
	}
	else
	{
		newWindow = true;
		pPanel = new GUI_PanelWindow(iRect(0, 0, HEALTH_WIDTH, HEALTH_HEIGHT), false, GUI_PANEL_WINDOW_HEALTH);
	}

	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(HEALTH_MARK_X, HEALTH_MARK_Y, HEALTH_MARK_W, HEALTH_MARK_H), GUI_UI_IMAGE, GUI_UI_SYMBOL_HEALTH_X, GUI_UI_SYMBOL_HEALTH_Y);
	pPanel->addChild(newImage);
	newImage = new GUI_StaticImage(iRect(MANA_MARK_X, MANA_MARK_Y, MANA_MARK_W, MANA_MARK_H), GUI_UI_IMAGE, GUI_UI_SYMBOL_MANA_X, GUI_UI_SYMBOL_MANA_Y);
	pPanel->addChild(newImage);
	GUI_Health* newHP = new GUI_Health(iRect(HEALTH_BAR_X, HEALTH_BAR_Y, HEALTH_BAR_W, HEALTH_BAR_H), HEALTH_BAR_EVENTID);
	newHP->setPercent(SDL_static_cast(Sint32, g_game.getPlayerHealthPercent()) * 90 / 100);
	pPanel->addChild(newHP);
	GUI_Mana* newMP = new GUI_Mana(iRect(MANA_BAR_X, MANA_BAR_Y, MANA_BAR_W, MANA_BAR_H), MANA_BAR_EVENTID);
	newMP->setPercent(SDL_static_cast(Sint32, g_game.getPlayerManaPercent()) * 90 / 100);
	pPanel->addChild(newMP);
	Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", g_game.getPlayerHealth());
	GUI_Label* newLabel = new GUI_Label(iRect(HEALTH_LABEL_X, HEALTH_LABEL_Y, 0, 0), std::string(g_buffer, SDL_static_cast(size_t, len)), HEALTH_LABEL_EVENTID);
	pPanel->addChild(newLabel);
	len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u%%", SDL_static_cast(Uint32, g_game.getPlayerHealthPercent()));
	newLabel = new GUI_Label(iRect(HEALTH_PERCENT_X, HEALTH_PERCENT_Y, 0, 0), std::string(g_buffer, SDL_static_cast(size_t, len)), HEALTH_PERCENT_EVENTID);
	newLabel->setFont(CLIENT_FONT_OUTLINED);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	pPanel->addChild(newLabel);
	len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", g_game.getPlayerMana());
	newLabel = new GUI_Label(iRect(MANA_LABEL_X, MANA_LABEL_Y, 0, 0), std::string(g_buffer, SDL_static_cast(size_t, len)), MANA_LABEL_EVENTID);
	pPanel->addChild(newLabel);
	len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u%%", SDL_static_cast(Uint32, g_game.getPlayerManaPercent()));
	newLabel = new GUI_Label(iRect(MANA_PERCENT_X, MANA_PERCENT_Y, 0, 0), std::string(g_buffer, SDL_static_cast(size_t, len)), MANA_PERCENT_EVENTID);
	newLabel->setFont(CLIENT_FONT_OUTLINED);
	newLabel->setAlign(CLIENT_FONT_ALIGN_CENTER);
	pPanel->addChild(newLabel);
	if(newWindow)
		g_engine.addToPanel(pPanel, GUI_PANEL_MAIN);
	else
		pPanel->checkPanels();
}

void UTIL_flashHealthPanel()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_HEALTH);
	if(pPanel)
	{
		GUI_Flash* pFlash = SDL_static_cast(GUI_Flash*, pPanel->getChild(HEALTH_FLASH_EVENTID));
		if(pFlash)
			pFlash->reset();
		else
		{
			pFlash = new GUI_Flash(iRect(0, 0, HEALTH_WIDTH, HEALTH_HEIGHT), HEALTH_FLASH_EVENTID);
			pFlash->setEndEventCallback(&health_Events, HEALTH_FLASH_EVENTID);
			pPanel->addChild(pFlash);
		}
	}
}

GUI_Health::GUI_Health(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_Health::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_BAR_EMPTY_X, GUI_UI_BAR_EMPTY_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_BAR_HEALTH_X, GUI_UI_BAR_HEALTH_Y, m_tRect.x1, m_tRect.y1, m_percent, m_tRect.y2);
}

GUI_Mana::GUI_Mana(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_Mana::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_BAR_EMPTY_X, GUI_UI_BAR_EMPTY_Y, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_BAR_MANA_X, GUI_UI_BAR_MANA_Y, m_tRect.x1, m_tRect.y1, m_percent, m_tRect.y2);
}
