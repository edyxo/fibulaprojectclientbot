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

#define MANATRAINER_TITLE "Mana Trainer"
#define MANATRAINER_WIDTH 320
#define MANATRAINER_HEIGHT 178

#define MANATRAINER_OK_EVENTID 1000
#define MANATRAINER_CANCEL_EVENTID 1001
#define MANATRAINER_ENABLED_EVENTID 1002
#define MANATRAINER_SPELL_EVENTID 1003
#define MANATRAINER_MANA_EVENTID 1004
#define MANATRAINER_DELAY_EVENTID 1005

extern Engine g_engine;

static GUI_CheckBox* s_manaTrainerEnabled = NULL;
static GUI_TextBox* s_manaTrainerSpell = NULL;
static GUI_TextBox* s_manaTrainerMana = NULL;
static GUI_TextBox* s_manaTrainerDelay = NULL;

static Uint32 readNumber(GUI_TextBox* textBox, Uint32 fallback)
{
	if(!textBox)
		return fallback;

	std::string value = textBox->getActualText();
	if(value.empty())
		return fallback;

	return SDL_static_cast(Uint32, SDL_strtoul(value.c_str(), NULL, 10));
}

static void manaTrainer_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case MANATRAINER_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_MANATRAINER)
				break;

			std::string spell = s_manaTrainerSpell ? s_manaTrainerSpell->getActualText() : g_engine.getManaTrainerSpell();
			Uint32 manaPercent = UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, readNumber(s_manaTrainerMana, g_engine.getManaTrainerManaPercent())));
			Uint32 delay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, readNumber(s_manaTrainerDelay, g_engine.getManaTrainerDelay())));
			bool enabled = (s_manaTrainerEnabled ? s_manaTrainerEnabled->isChecked() : g_engine.getManaTrainerEnabled());

			g_engine.setManaTrainerOptions(enabled, spell, SDL_static_cast(Uint8, manaPercent), delay);
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case MANATRAINER_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_MANATRAINER)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_manaTrainerOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_MANATRAINER);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_manaTrainerEnabled = NULL;
	s_manaTrainerSpell = NULL;
	s_manaTrainerMana = NULL;
	s_manaTrainerDelay = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, MANATRAINER_WIDTH, MANATRAINER_HEIGHT), MANATRAINER_TITLE, GUI_WINDOW_MANATRAINER);

	s_manaTrainerEnabled = new GUI_CheckBox(iRect(18, 24, 280, 16), "Enabled", g_engine.getManaTrainerEnabled(), MANATRAINER_ENABLED_EVENTID);
	s_manaTrainerEnabled->startEvents();
	newWindow->addChild(s_manaTrainerEnabled);

	GUI_Label* label = new GUI_Label(iRect(18, 52, 0, 0), "Spell:");
	newWindow->addChild(label);
	s_manaTrainerSpell = new GUI_TextBox(iRect(112, 50, 180, 16), g_engine.getManaTrainerSpell(), MANATRAINER_SPELL_EVENTID);
	s_manaTrainerSpell->setMaxLength(64);
	s_manaTrainerSpell->startEvents();
	newWindow->addChild(s_manaTrainerSpell);

	label = new GUI_Label(iRect(18, 78, 0, 0), "Mana at:");
	newWindow->addChild(label);
	s_manaTrainerMana = new GUI_TextBox(iRect(112, 76, 54, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getManaTrainerManaPercent())), MANATRAINER_MANA_EVENTID);
	s_manaTrainerMana->setOnlyNumbers(true);
	s_manaTrainerMana->setMaxLength(3);
	s_manaTrainerMana->startEvents();
	newWindow->addChild(s_manaTrainerMana);
	label = new GUI_Label(iRect(172, 78, 0, 0), "%");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 104, 0, 0), "Delay:");
	newWindow->addChild(label);
	s_manaTrainerDelay = new GUI_TextBox(iRect(112, 102, 72, 16), std::to_string(g_engine.getManaTrainerDelay()), MANATRAINER_DELAY_EVENTID);
	s_manaTrainerDelay->setOnlyNumbers(true);
	s_manaTrainerDelay->setMaxLength(5);
	s_manaTrainerDelay->startEvents();
	newWindow->addChild(s_manaTrainerDelay);
	label = new GUI_Label(iRect(190, 104, 0, 0), "ms");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 136, MANATRAINER_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(MANATRAINER_WIDTH - 58, 146, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&manaTrainer_Events, MANATRAINER_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(MANATRAINER_WIDTH - 105, 146, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&manaTrainer_Events, MANATRAINER_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
