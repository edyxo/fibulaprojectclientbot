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

#define MAGICWALL_TITLE "Magic Wall Auto"
#define MAGICWALL_WIDTH 320
#define MAGICWALL_HEIGHT 205

#define MAGICWALL_OK_EVENTID 1000
#define MAGICWALL_CANCEL_EVENTID 1001
#define MAGICWALL_ENABLED_EVENTID 1002
#define MAGICWALL_SPELL_EVENTID 1003
#define MAGICWALL_DIST_EVENTID 1004
#define MAGICWALL_DELAY_EVENTID 1005

extern Engine g_engine;

static GUI_CheckBox* s_magicWallEnabled = NULL;
static GUI_TextBox* s_magicWallSpell = NULL;
static GUI_TextBox* s_magicWallDist = NULL;
static GUI_TextBox* s_magicWallDelay = NULL;

static Uint32 readNumber(GUI_TextBox* textBox, Uint32 fallback)
{
	if(!textBox)
		return fallback;

	std::string value = textBox->getActualText();
	if(value.empty())
		return fallback;

	return SDL_static_cast(Uint32, SDL_strtoul(value.c_str(), NULL, 10));
}

static void magicWall_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case MAGICWALL_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_MAGICWALL)
				break;

			std::string spell = s_magicWallSpell ? s_magicWallSpell->getActualText() : g_engine.getMagicWallSpell();
			Uint32 dist = UTIL_max<Uint32>(1, UTIL_min<Uint32>(7, readNumber(s_magicWallDist, g_engine.getMagicWallDist())));
			Uint32 delay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, readNumber(s_magicWallDelay, g_engine.getMagicWallDelay())));
			bool enabled = (s_magicWallEnabled ? s_magicWallEnabled->isChecked() : g_engine.getMagicWallEnabled());

			g_engine.setMagicWallOptions(enabled, spell, SDL_static_cast(Uint8, dist), delay);
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case MAGICWALL_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_MAGICWALL)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_magicWallOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_MAGICWALL);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_magicWallEnabled = NULL;
	s_magicWallSpell = NULL;
	s_magicWallDist = NULL;
	s_magicWallDelay = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, MAGICWALL_WIDTH, MAGICWALL_HEIGHT), MAGICWALL_TITLE, GUI_WINDOW_MAGICWALL);

	s_magicWallEnabled = new GUI_CheckBox(iRect(18, 24, 280, 16), "Enabled (casts when target is within distance)", g_engine.getMagicWallEnabled(), MAGICWALL_ENABLED_EVENTID);
	s_magicWallEnabled->startEvents();
	newWindow->addChild(s_magicWallEnabled);

	GUI_Label* label = new GUI_Label(iRect(18, 52, 0, 0), "Spell:");
	newWindow->addChild(label);
	s_magicWallSpell = new GUI_TextBox(iRect(112, 50, 180, 16), g_engine.getMagicWallSpell(), MAGICWALL_SPELL_EVENTID);
	s_magicWallSpell->setMaxLength(64);
	s_magicWallSpell->startEvents();
	newWindow->addChild(s_magicWallSpell);

	label = new GUI_Label(iRect(18, 78, 0, 0), "Max dist:");
	newWindow->addChild(label);
	s_magicWallDist = new GUI_TextBox(iRect(112, 76, 40, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getMagicWallDist())), MAGICWALL_DIST_EVENTID);
	s_magicWallDist->setOnlyNumbers(true);
	s_magicWallDist->setMaxLength(1);
	s_magicWallDist->startEvents();
	newWindow->addChild(s_magicWallDist);
	label = new GUI_Label(iRect(158, 78, 0, 0), "tiles (1-7)");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 104, 0, 0), "Delay:");
	newWindow->addChild(label);
	s_magicWallDelay = new GUI_TextBox(iRect(112, 102, 72, 16), std::to_string(g_engine.getMagicWallDelay()), MAGICWALL_DELAY_EVENTID);
	s_magicWallDelay->setOnlyNumbers(true);
	s_magicWallDelay->setMaxLength(5);
	s_magicWallDelay->startEvents();
	newWindow->addChild(s_magicWallDelay);
	label = new GUI_Label(iRect(190, 104, 0, 0), "ms");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 163, MAGICWALL_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(MAGICWALL_WIDTH - 58, 173, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&magicWall_Events, MAGICWALL_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(MAGICWALL_WIDTH - 105, 173, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&magicWall_Events, MAGICWALL_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
