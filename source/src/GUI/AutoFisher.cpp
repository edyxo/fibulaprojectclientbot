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

#define AUTOFISHER_TITLE "Auto Fisher"
#define AUTOFISHER_WIDTH 340
#define AUTOFISHER_HEIGHT 178

#define AUTOFISHER_OK_EVENTID     1000
#define AUTOFISHER_CANCEL_EVENTID 1001
#define AUTOFISHER_ENABLED_EVENTID 1002
#define AUTOFISHER_DELAY_MIN_EVENTID 1003
#define AUTOFISHER_DELAY_MAX_EVENTID 1004

extern Engine g_engine;

static GUI_CheckBox* s_autoFisherEnabled = NULL;
static GUI_TextBox*  s_autoFisherDelayMin = NULL;
static GUI_TextBox*  s_autoFisherDelayMax = NULL;

static Uint32 readNumber(GUI_TextBox* textBox, Uint32 fallback)
{
	if(!textBox)
		return fallback;
	std::string value = textBox->getActualText();
	if(value.empty())
		return fallback;
	return SDL_static_cast(Uint32, SDL_strtoul(value.c_str(), NULL, 10));
}

static void autoFisher_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case AUTOFISHER_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_AUTOFISHER)
				break;

			bool enabled = (s_autoFisherEnabled ? s_autoFisherEnabled->isChecked() : g_engine.getAutoFisherEnabled());
			Uint32 delayMin = UTIL_min<Uint32>(30000, readNumber(s_autoFisherDelayMin, g_engine.getAutoFisherDelayMin()));
			Uint32 delayMax = UTIL_min<Uint32>(30000, readNumber(s_autoFisherDelayMax, g_engine.getAutoFisherDelayMax()));

			g_engine.setAutoFisherOptions(enabled, delayMin, delayMax);
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case AUTOFISHER_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_AUTOFISHER)
				g_engine.removeWindow(pWindow);
		}
		break;
		default: break;
	}
}

void UTIL_autoFisherOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_AUTOFISHER);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_autoFisherEnabled = NULL;
	s_autoFisherDelayMin = NULL;
	s_autoFisherDelayMax = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, AUTOFISHER_WIDTH, AUTOFISHER_HEIGHT), AUTOFISHER_TITLE, GUI_WINDOW_AUTOFISHER);

	s_autoFisherEnabled = new GUI_CheckBox(iRect(18, 24, 280, 16), "Enabled (rod in ammo slot)", g_engine.getAutoFisherEnabled(), AUTOFISHER_ENABLED_EVENTID);
	s_autoFisherEnabled->startEvents();
	newWindow->addChild(s_autoFisherEnabled);

	GUI_Label* label = new GUI_Label(iRect(18, 56, 0, 0), "Cast delay:");
	newWindow->addChild(label);
	s_autoFisherDelayMin = new GUI_TextBox(iRect(112, 54, 60, 16), std::to_string(g_engine.getAutoFisherDelayMin()), AUTOFISHER_DELAY_MIN_EVENTID);
	s_autoFisherDelayMin->setOnlyNumbers(true);
	s_autoFisherDelayMin->setMaxLength(5);
	s_autoFisherDelayMin->startEvents();
	newWindow->addChild(s_autoFisherDelayMin);
	label = new GUI_Label(iRect(178, 56, 0, 0), "to");
	newWindow->addChild(label);
	s_autoFisherDelayMax = new GUI_TextBox(iRect(202, 54, 60, 16), std::to_string(g_engine.getAutoFisherDelayMax()), AUTOFISHER_DELAY_MAX_EVENTID);
	s_autoFisherDelayMax->setOnlyNumbers(true);
	s_autoFisherDelayMax->setMaxLength(5);
	s_autoFisherDelayMax->startEvents();
	newWindow->addChild(s_autoFisherDelayMax);
	label = new GUI_Label(iRect(268, 56, 0, 0), "ms");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 80, 0, 0), "Minimum action gap: 1000 ms");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 138, AUTOFISHER_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(AUTOFISHER_WIDTH - 58, 148, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&autoFisher_Events, AUTOFISHER_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(AUTOFISHER_WIDTH - 105, 148, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&autoFisher_Events, AUTOFISHER_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
