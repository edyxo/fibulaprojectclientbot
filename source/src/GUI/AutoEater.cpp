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

#define AUTOEATER_TITLE "Auto Eat Food"
#define AUTOEATER_WIDTH 320
#define AUTOEATER_HEIGHT 152

#define AUTOEATER_OK_EVENTID      1000
#define AUTOEATER_CANCEL_EVENTID  1001
#define AUTOEATER_ENABLED_EVENTID 1002
#define AUTOEATER_FOODID_EVENTID  1003
#define AUTOEATER_DELAY_EVENTID   1004

extern Engine g_engine;

static GUI_CheckBox* s_eaterEnabled = NULL;
static GUI_TextBox*  s_eaterFoodId  = NULL;
static GUI_TextBox*  s_eaterDelay   = NULL;

static Uint32 readNum(GUI_TextBox* tb, Uint32 fallback)
{
	if(!tb) return fallback;
	std::string v = tb->getActualText();
	if(v.empty()) return fallback;
	return SDL_static_cast(Uint32, SDL_strtoul(v.c_str(), NULL, 10));
}

static void autoEater_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case AUTOEATER_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_AUTOEATER)
				break;

			bool enabled = s_eaterEnabled ? s_eaterEnabled->isChecked() : g_engine.getAutoEaterEnabled();
			Uint16 foodId = SDL_static_cast(Uint16, UTIL_max<Uint32>(100, UTIL_min<Uint32>(65535, readNum(s_eaterFoodId, SDL_static_cast(Uint32, g_engine.getAutoEaterFoodId())))));
			Uint32 delay = UTIL_max<Uint32>(1000, UTIL_min<Uint32>(300000, readNum(s_eaterDelay, g_engine.getAutoEaterDelay())));

			g_engine.setAutoEaterOptions(enabled, foodId, delay);
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case AUTOEATER_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_AUTOEATER)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_autoEaterOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_AUTOEATER);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_eaterEnabled = NULL;
	s_eaterFoodId  = NULL;
	s_eaterDelay   = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, AUTOEATER_WIDTH, AUTOEATER_HEIGHT), AUTOEATER_TITLE, GUI_WINDOW_AUTOEATER);

	s_eaterEnabled = new GUI_CheckBox(iRect(18, 24, 280, 16), "Enabled", g_engine.getAutoEaterEnabled(), AUTOEATER_ENABLED_EVENTID);
	s_eaterEnabled->startEvents();
	newWindow->addChild(s_eaterEnabled);

	GUI_Label* label = new GUI_Label(iRect(18, 52, 0, 0), "Food ID:");
	newWindow->addChild(label);
	s_eaterFoodId = new GUI_TextBox(iRect(112, 50, 80, 16), std::to_string(SDL_static_cast(Uint32, g_engine.getAutoEaterFoodId())), AUTOEATER_FOODID_EVENTID);
	s_eaterFoodId->setOnlyNumbers(true);
	s_eaterFoodId->setMaxLength(5);
	s_eaterFoodId->startEvents();
	newWindow->addChild(s_eaterFoodId);
	label = new GUI_Label(iRect(198, 52, 0, 0), "(e.g. 3577=Ham)");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 78, 0, 0), "Delay:");
	newWindow->addChild(label);
	s_eaterDelay = new GUI_TextBox(iRect(112, 76, 80, 16), std::to_string(g_engine.getAutoEaterDelay()), AUTOEATER_DELAY_EVENTID);
	s_eaterDelay->setOnlyNumbers(true);
	s_eaterDelay->setMaxLength(6);
	s_eaterDelay->startEvents();
	newWindow->addChild(s_eaterDelay);
	label = new GUI_Label(iRect(198, 78, 0, 0), "ms");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 110, AUTOEATER_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(AUTOEATER_WIDTH - 58, 120, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&autoEater_Events, AUTOEATER_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(AUTOEATER_WIDTH - 105, 120, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&autoEater_Events, AUTOEATER_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
