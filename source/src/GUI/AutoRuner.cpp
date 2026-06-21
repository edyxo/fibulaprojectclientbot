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

#define AUTORUNER_TITLE "Auto Runer"
#define AUTORUNER_WIDTH 320
#define AUTORUNER_HEIGHT 178

#define AUTORUNER_OK_EVENTID     1000
#define AUTORUNER_CANCEL_EVENTID 1001
#define AUTORUNER_ENABLED_EVENTID 1002
#define AUTORUNER_SPELL_EVENTID  1003
#define AUTORUNER_MANA_EVENTID   1004
#define AUTORUNER_DELAY_EVENTID  1005

extern Engine g_engine;

static GUI_CheckBox* s_runerEnabled = NULL;
static GUI_TextBox*  s_runerSpell   = NULL;
static GUI_TextBox*  s_runerMana    = NULL;
static GUI_TextBox*  s_runerDelay   = NULL;

static Uint32 readNum(GUI_TextBox* tb, Uint32 fallback)
{
	if(!tb) return fallback;
	std::string v = tb->getActualText();
	if(v.empty()) return fallback;
	return SDL_static_cast(Uint32, SDL_strtoul(v.c_str(), NULL, 10));
}

static void autoRuner_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case AUTORUNER_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_AUTORUNER)
				break;

			bool enabled = s_runerEnabled ? s_runerEnabled->isChecked() : g_engine.getAutoRunerEnabled();
			std::string spell = s_runerSpell ? s_runerSpell->getActualText() : g_engine.getAutoRunerSpell();
			Uint32 manaPercent = UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, readNum(s_runerMana, g_engine.getAutoRunerManaPercent())));
			Uint32 delay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, readNum(s_runerDelay, g_engine.getAutoRunerDelay())));

			g_engine.setAutoRunerOptions(enabled, spell, SDL_static_cast(Uint8, manaPercent), delay);
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case AUTORUNER_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_AUTORUNER)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_autoRunerOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_AUTORUNER);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_runerEnabled = NULL;
	s_runerSpell   = NULL;
	s_runerMana    = NULL;
	s_runerDelay   = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, AUTORUNER_WIDTH, AUTORUNER_HEIGHT), AUTORUNER_TITLE, GUI_WINDOW_AUTORUNER);

	s_runerEnabled = new GUI_CheckBox(iRect(18, 24, 280, 16), "Enabled (blank rune in hand/open BP)", g_engine.getAutoRunerEnabled(), AUTORUNER_ENABLED_EVENTID);
	s_runerEnabled->startEvents();
	newWindow->addChild(s_runerEnabled);

	GUI_Label* label = new GUI_Label(iRect(18, 52, 0, 0), "Spell:");
	newWindow->addChild(label);
	s_runerSpell = new GUI_TextBox(iRect(112, 50, 180, 16), g_engine.getAutoRunerSpell(), AUTORUNER_SPELL_EVENTID);
	s_runerSpell->setMaxLength(64);
	s_runerSpell->startEvents();
	newWindow->addChild(s_runerSpell);

	label = new GUI_Label(iRect(18, 78, 0, 0), "Min mana:");
	newWindow->addChild(label);
	s_runerMana = new GUI_TextBox(iRect(112, 76, 54, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getAutoRunerManaPercent())), AUTORUNER_MANA_EVENTID);
	s_runerMana->setOnlyNumbers(true);
	s_runerMana->setMaxLength(3);
	s_runerMana->startEvents();
	newWindow->addChild(s_runerMana);
	label = new GUI_Label(iRect(172, 78, 0, 0), "%");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 104, 0, 0), "Delay:");
	newWindow->addChild(label);
	s_runerDelay = new GUI_TextBox(iRect(112, 102, 72, 16), std::to_string(g_engine.getAutoRunerDelay()), AUTORUNER_DELAY_EVENTID);
	s_runerDelay->setOnlyNumbers(true);
	s_runerDelay->setMaxLength(5);
	s_runerDelay->startEvents();
	newWindow->addChild(s_runerDelay);
	label = new GUI_Label(iRect(190, 104, 0, 0), "ms");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 136, AUTORUNER_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(AUTORUNER_WIDTH - 58, 146, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&autoRuner_Events, AUTORUNER_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(AUTORUNER_WIDTH - 105, 146, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&autoRuner_Events, AUTORUNER_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
