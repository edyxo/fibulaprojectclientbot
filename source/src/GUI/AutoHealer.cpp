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

#define AUTOHEALER_TITLE "Auto Healer"
#define AUTOHEALER_WIDTH 370
#define AUTOHEALER_HEIGHT 306

#define AUTOHEALER_OK_EVENTID 1000
#define AUTOHEALER_CANCEL_EVENTID 1001
#define AUTOHEALER_ENABLED_EVENTID 1002
#define AUTOHEALER_SPELL_EVENTID 1003
#define AUTOHEALER_HP_EVENTID 1004
#define AUTOHEALER_DELAY_EVENTID 1005
#define AUTOHEALER_SELF_UH_EVENTID 1006
#define AUTOHEALER_UH_ID_EVENTID 1007
#define AUTOHEALER_SELF_UH_HP_EVENTID 1008
#define AUTOHEALER_FRIEND_UH_EVENTID 1009
#define AUTOHEALER_FRIEND_NAMES_EVENTID 1010
#define AUTOHEALER_FRIEND_UH_HP_EVENTID 1011

extern Engine g_engine;

static GUI_CheckBox* s_autoHealerEnabled = NULL;
static GUI_TextBox* s_autoHealerSpell = NULL;
static GUI_TextBox* s_autoHealerHP = NULL;
static GUI_CheckBox* s_autoHealerSelfUH = NULL;
static GUI_TextBox* s_autoHealerUHId = NULL;
static GUI_TextBox* s_autoHealerSelfUHHP = NULL;
static GUI_CheckBox* s_autoHealerFriendUH = NULL;
static GUI_TextBox* s_autoHealerFriendNames = NULL;
static GUI_TextBox* s_autoHealerFriendUHHP = NULL;
static GUI_TextBox* s_autoHealerDelay = NULL;

static Uint32 readNumber(GUI_TextBox* textBox, Uint32 fallback)
{
	if(!textBox)
		return fallback;

	std::string value = textBox->getActualText();
	if(value.empty())
		return fallback;

	return SDL_static_cast(Uint32, SDL_strtoul(value.c_str(), NULL, 10));
}

static void autoHealer_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case AUTOHEALER_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_AUTOHEALER)
				break;

			std::string spell = s_autoHealerSpell ? s_autoHealerSpell->getActualText() : g_engine.getAutoHealerSpell();
			Uint32 hpPercent = UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, readNumber(s_autoHealerHP, g_engine.getAutoHealerHPPercent())));
			Uint32 uhId = UTIL_min<Uint32>(65535, readNumber(s_autoHealerUHId, g_engine.getAutoHealerUHItemId()));
			Uint32 selfUHPercent = UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, readNumber(s_autoHealerSelfUHHP, g_engine.getAutoHealerSelfUHPercent())));
			Uint32 friendUHPercent = UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, readNumber(s_autoHealerFriendUHHP, g_engine.getAutoHealerFriendUHPercent())));
			Uint32 delay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, readNumber(s_autoHealerDelay, g_engine.getAutoHealerDelay())));
			bool enabled = (s_autoHealerEnabled ? s_autoHealerEnabled->isChecked() : g_engine.getAutoHealerEnabled());
			bool selfUH = (s_autoHealerSelfUH ? s_autoHealerSelfUH->isChecked() : g_engine.getAutoHealerSelfUHEnabled());
			bool friendUH = (s_autoHealerFriendUH ? s_autoHealerFriendUH->isChecked() : g_engine.getAutoHealerFriendUHEnabled());
			std::string friendNames = s_autoHealerFriendNames ? s_autoHealerFriendNames->getActualText() : g_engine.getAutoHealerFriendNames();

			g_engine.setAutoHealerOptions(enabled, spell, SDL_static_cast(Uint8, hpPercent), delay,
				selfUH, SDL_static_cast(Uint16, uhId), SDL_static_cast(Uint8, selfUHPercent),
				friendUH, friendNames, SDL_static_cast(Uint8, friendUHPercent));
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case AUTOHEALER_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_AUTOHEALER)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_autoHealerOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_AUTOHEALER);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_autoHealerEnabled = NULL;
	s_autoHealerSpell = NULL;
	s_autoHealerHP = NULL;
	s_autoHealerSelfUH = NULL;
	s_autoHealerUHId = NULL;
	s_autoHealerSelfUHHP = NULL;
	s_autoHealerFriendUH = NULL;
	s_autoHealerFriendNames = NULL;
	s_autoHealerFriendUHHP = NULL;
	s_autoHealerDelay = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, AUTOHEALER_WIDTH, AUTOHEALER_HEIGHT), AUTOHEALER_TITLE, GUI_WINDOW_AUTOHEALER);

	s_autoHealerEnabled = new GUI_CheckBox(iRect(18, 24, 280, 16), "Enabled", g_engine.getAutoHealerEnabled(), AUTOHEALER_ENABLED_EVENTID);
	s_autoHealerEnabled->startEvents();
	newWindow->addChild(s_autoHealerEnabled);

	GUI_Label* label = new GUI_Label(iRect(18, 52, 0, 0), "Spell:");
	newWindow->addChild(label);
	s_autoHealerSpell = new GUI_TextBox(iRect(112, 50, 180, 16), g_engine.getAutoHealerSpell(), AUTOHEALER_SPELL_EVENTID);
	s_autoHealerSpell->setMaxLength(64);
	s_autoHealerSpell->startEvents();
	newWindow->addChild(s_autoHealerSpell);

	label = new GUI_Label(iRect(18, 78, 0, 0), "Heal when HP:");
	newWindow->addChild(label);
	s_autoHealerHP = new GUI_TextBox(iRect(112, 76, 54, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getAutoHealerHPPercent())), AUTOHEALER_HP_EVENTID);
	s_autoHealerHP->setOnlyNumbers(true);
	s_autoHealerHP->setMaxLength(3);
	s_autoHealerHP->startEvents();
	newWindow->addChild(s_autoHealerHP);
	label = new GUI_Label(iRect(172, 78, 0, 0), "% or below");
	newWindow->addChild(label);

	s_autoHealerSelfUH = new GUI_CheckBox(iRect(18, 104, 280, 16), "Use UH on yourself first", g_engine.getAutoHealerSelfUHEnabled(), AUTOHEALER_SELF_UH_EVENTID);
	s_autoHealerSelfUH->startEvents();
	newWindow->addChild(s_autoHealerSelfUH);

	label = new GUI_Label(iRect(18, 130, 0, 0), "UH rune ID:");
	newWindow->addChild(label);
	s_autoHealerUHId = new GUI_TextBox(iRect(112, 128, 54, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getAutoHealerUHItemId())), AUTOHEALER_UH_ID_EVENTID);
	s_autoHealerUHId->setOnlyNumbers(true);
	s_autoHealerUHId->setMaxLength(5);
	s_autoHealerUHId->startEvents();
	newWindow->addChild(s_autoHealerUHId);
	label = new GUI_Label(iRect(190, 130, 0, 0), "Self HP:");
	newWindow->addChild(label);
	s_autoHealerSelfUHHP = new GUI_TextBox(iRect(252, 128, 40, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getAutoHealerSelfUHPercent())), AUTOHEALER_SELF_UH_HP_EVENTID);
	s_autoHealerSelfUHHP->setOnlyNumbers(true);
	s_autoHealerSelfUHHP->setMaxLength(3);
	s_autoHealerSelfUHHP->startEvents();
	newWindow->addChild(s_autoHealerSelfUHHP);
	label = new GUI_Label(iRect(298, 130, 0, 0), "%");
	newWindow->addChild(label);

	s_autoHealerFriendUH = new GUI_CheckBox(iRect(18, 154, 280, 16), "Heal friends with UH", g_engine.getAutoHealerFriendUHEnabled(), AUTOHEALER_FRIEND_UH_EVENTID);
	s_autoHealerFriendUH->startEvents();
	newWindow->addChild(s_autoHealerFriendUH);

	label = new GUI_Label(iRect(18, 180, 0, 0), "Friends:");
	newWindow->addChild(label);
	s_autoHealerFriendNames = new GUI_TextBox(iRect(112, 178, 230, 16), g_engine.getAutoHealerFriendNames(), AUTOHEALER_FRIEND_NAMES_EVENTID);
	s_autoHealerFriendNames->setMaxLength(128);
	s_autoHealerFriendNames->startEvents();
	newWindow->addChild(s_autoHealerFriendNames);

	label = new GUI_Label(iRect(18, 206, 0, 0), "Friend HP:");
	newWindow->addChild(label);
	s_autoHealerFriendUHHP = new GUI_TextBox(iRect(112, 204, 54, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getAutoHealerFriendUHPercent())), AUTOHEALER_FRIEND_UH_HP_EVENTID);
	s_autoHealerFriendUHHP->setOnlyNumbers(true);
	s_autoHealerFriendUHHP->setMaxLength(3);
	s_autoHealerFriendUHHP->startEvents();
	newWindow->addChild(s_autoHealerFriendUHHP);
	label = new GUI_Label(iRect(172, 206, 0, 0), "% or below, comma names");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 232, 0, 0), "Delay:");
	newWindow->addChild(label);
	s_autoHealerDelay = new GUI_TextBox(iRect(112, 230, 72, 16), std::to_string(g_engine.getAutoHealerDelay()), AUTOHEALER_DELAY_EVENTID);
	s_autoHealerDelay->setOnlyNumbers(true);
	s_autoHealerDelay->setMaxLength(5);
	s_autoHealerDelay->startEvents();
	newWindow->addChild(s_autoHealerDelay);
	label = new GUI_Label(iRect(190, 232, 0, 0), "ms");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 264, AUTOHEALER_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(AUTOHEALER_WIDTH - 58, 274, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&autoHealer_Events, AUTOHEALER_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(AUTOHEALER_WIDTH - 105, 274, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&autoHealer_Events, AUTOHEALER_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
