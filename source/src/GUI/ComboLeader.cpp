/*
  The Forgotten Client
  Copyright (C) 2020 Saiyans King
*/

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../GUI_Elements/GUI_Window.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_CheckBox.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_TextBox.h"

#define COMBOLEADER_TITLE "Combo Leader"
#define COMBOLEADER_WIDTH 390
#define COMBOLEADER_HEIGHT 340

#define COMBOLEADER_OK_EVENTID 1000
#define COMBOLEADER_CANCEL_EVENTID 1001
#define COMBOLEADER_ENABLED_EVENTID 1002
#define COMBOLEADER_LEADER_EVENTID 1003
#define COMBOLEADER_DELAY_EVENTID 1008
#define COMBOLEADER_EFFECT_EVENTID_BASE 1100
#define COMBOLEADER_RUNE_EVENTID_BASE 1120

extern Engine g_engine;

static GUI_CheckBox* s_comboLeaderEnabled = NULL;
static GUI_TextBox* s_comboLeaderName = NULL;
static GUI_TextBox* s_comboLeaderEffect[Engine::COMBO_LEADER_MAPPING_COUNT] = {};
static GUI_TextBox* s_comboLeaderRune[Engine::COMBO_LEADER_MAPPING_COUNT] = {};
static GUI_TextBox* s_comboLeaderDelay = NULL;

static Uint32 readNumber(GUI_TextBox* textBox, Uint32 fallback)
{
	if(!textBox)
		return fallback;

	std::string value = textBox->getActualText();
	if(value.empty())
		return fallback;

	return SDL_static_cast(Uint32, SDL_strtoul(value.c_str(), NULL, 10));
}

static void comboLeader_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case COMBOLEADER_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_COMBOLEADER)
				break;

			bool enabled = (s_comboLeaderEnabled ? s_comboLeaderEnabled->isChecked() : g_engine.getComboLeaderEnabled());
			std::string leader = s_comboLeaderName ? s_comboLeaderName->getActualText() : g_engine.getComboLeaderName();
			Uint32 delay = UTIL_max<Uint32>(250, UTIL_min<Uint32>(10000, readNumber(s_comboLeaderDelay, g_engine.getComboLeaderDelay())));

			g_engine.setComboLeaderOptions(enabled, leader, delay);
			for(size_t index = 0; index < g_engine.getComboLeaderMappingCount(); ++index)
			{
				Uint16 effect = SDL_static_cast(Uint16, UTIL_min<Uint32>(65535, readNumber(s_comboLeaderEffect[index], g_engine.getComboLeaderMappingEffect(index))));
				Uint16 rune = SDL_static_cast(Uint16, UTIL_min<Uint32>(65535, readNumber(s_comboLeaderRune[index], g_engine.getComboLeaderMappingRune(index))));
				g_engine.setComboLeaderMapping(index, g_engine.getComboLeaderMappingName(index), effect, rune);
			}
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case COMBOLEADER_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_COMBOLEADER)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_comboLeaderOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_COMBOLEADER);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_comboLeaderEnabled = NULL;
	s_comboLeaderName = NULL;
	for(size_t index = 0; index < g_engine.getComboLeaderMappingCount(); ++index)
	{
		s_comboLeaderEffect[index] = NULL;
		s_comboLeaderRune[index] = NULL;
	}
	s_comboLeaderDelay = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, COMBOLEADER_WIDTH, COMBOLEADER_HEIGHT), COMBOLEADER_TITLE, GUI_WINDOW_COMBOLEADER);

	s_comboLeaderEnabled = new GUI_CheckBox(iRect(18, 24, 300, 16), "Enabled (mirror leader projectiles)", g_engine.getComboLeaderEnabled(), COMBOLEADER_ENABLED_EVENTID);
	s_comboLeaderEnabled->startEvents();
	newWindow->addChild(s_comboLeaderEnabled);

	GUI_Label* label = new GUI_Label(iRect(18, 52, 0, 0), "Leader:");
	newWindow->addChild(label);
	s_comboLeaderName = new GUI_TextBox(iRect(112, 50, 200, 16), g_engine.getComboLeaderName(), COMBOLEADER_LEADER_EVENTID);
	s_comboLeaderName->setMaxLength(32);
	s_comboLeaderName->startEvents();
	newWindow->addChild(s_comboLeaderName);

	label = new GUI_Label(iRect(18, 80, 0, 0), "Rune");
	newWindow->addChild(label);
	label = new GUI_Label(iRect(128, 80, 0, 0), "Effect");
	newWindow->addChild(label);
	label = new GUI_Label(iRect(236, 80, 0, 0), "Rune ID");
	newWindow->addChild(label);

	for(size_t index = 0; index < g_engine.getComboLeaderMappingCount(); ++index)
	{
		Sint32 y = 104 + SDL_static_cast(Sint32, index) * 24;
		label = new GUI_Label(iRect(18, y + 2, 0, 0), g_engine.getComboLeaderMappingName(index) + ":");
		newWindow->addChild(label);

		s_comboLeaderEffect[index] = new GUI_TextBox(iRect(126, y, 54, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getComboLeaderMappingEffect(index))), COMBOLEADER_EFFECT_EVENTID_BASE + SDL_static_cast(Uint32, index));
		s_comboLeaderEffect[index]->setOnlyNumbers(true);
		s_comboLeaderEffect[index]->setMaxLength(5);
		s_comboLeaderEffect[index]->startEvents();
		newWindow->addChild(s_comboLeaderEffect[index]);

		s_comboLeaderRune[index] = new GUI_TextBox(iRect(234, y, 54, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getComboLeaderMappingRune(index))), COMBOLEADER_RUNE_EVENTID_BASE + SDL_static_cast(Uint32, index));
		s_comboLeaderRune[index]->setOnlyNumbers(true);
		s_comboLeaderRune[index]->setMaxLength(5);
		s_comboLeaderRune[index]->startEvents();
		newWindow->addChild(s_comboLeaderRune[index]);
	}

	label = new GUI_Label(iRect(18, 230, 0, 0), "Cooldown:");
	newWindow->addChild(label);
	s_comboLeaderDelay = new GUI_TextBox(iRect(126, 228, 72, 16), std::to_string(g_engine.getComboLeaderDelay()), COMBOLEADER_DELAY_EVENTID);
	s_comboLeaderDelay->setOnlyNumbers(true);
	s_comboLeaderDelay->setMaxLength(5);
	s_comboLeaderDelay->startEvents();
	newWindow->addChild(s_comboLeaderDelay);
	label = new GUI_Label(iRect(204, 230, 0, 0), "ms");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 258, 0, 0), "Known effects: SD=10, HMM=4, GFB/fire=3.");
	newWindow->addChild(label);
	label = new GUI_Label(iRect(18, 274, 0, 0), "Runes: SD=3155, HMM=3198, GFB=3191, Explosion=3200.");
	newWindow->addChild(label);
	label = new GUI_Label(iRect(18, 290, 0, 0), "Set effect 0 to log projectile IDs. Duplicate effects use first row.");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 306, COMBOLEADER_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(COMBOLEADER_WIDTH - 58, 316, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&comboLeader_Events, COMBOLEADER_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(COMBOLEADER_WIDTH - 105, 316, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&comboLeader_Events, COMBOLEADER_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
