/*
  The Forgotten Client
  Copyright (C) 2020 Saiyans King
*/

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../game.h"
#include "../GUI_Elements/GUI_Window.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_MultiTextBox.h"

#define BUGREPORT_TITLE "Report Bug"
#define BUGREPORT_WIDTH 360
#define BUGREPORT_HEIGHT 220
#define BUGREPORT_CANCEL_EVENTID 1000
#define BUGREPORT_OK_EVENTID 1001
#define BUGREPORT_TEXTBOX_EVENTID 1002

extern Engine g_engine;
extern Game g_game;

static GUI_MultiTextBox* g_bugReportTextBox = NULL;

static void bugReport_Events(Uint32 event, Sint32)
{
	GUI_Window* window = g_engine.getCurrentWindow();
	if(!window || window->getInternalID() != GUI_WINDOW_BUGREPORT)
		return;

	switch(event)
	{
		case BUGREPORT_OK_EVENTID:
		{
			std::string report = (g_bugReportTextBox ? g_bugReportTextBox->getText() : std::string());
			if(report.empty())
			{
				UTIL_messageBox("Report Bug", "Please describe the bug before sending.");
				return;
			}

			g_game.sendBugReport(report);
			g_engine.removeWindow(window);
			g_bugReportTextBox = NULL;
			UTIL_messageBox("Report Bug", "Your bug report was sent.");
		}
		break;
		case BUGREPORT_CANCEL_EVENTID:
			g_engine.removeWindow(window);
			g_bugReportTextBox = NULL;
			break;
		default: break;
	}
}

void UTIL_createBugReport()
{
	if(!g_engine.isIngame())
		return;

	if(!g_game.getCanReportBugs())
	{
		UTIL_messageBox("Report Bug", "This server does not allow your character to send bug reports.");
		return;
	}

	GUI_Window* window = g_engine.getWindow(GUI_WINDOW_BUGREPORT);
	if(window)
	{
		g_engine.removeWindow(window);
		g_bugReportTextBox = NULL;
		return;
	}

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, BUGREPORT_WIDTH, BUGREPORT_HEIGHT), BUGREPORT_TITLE, GUI_WINDOW_BUGREPORT);
	GUI_Label* label = new GUI_Label(iRect(18, 28, 0, 0), "Please describe what happened:");
	newWindow->addChild(label);

	g_bugReportTextBox = new GUI_MultiTextBox(iRect(18, 48, BUGREPORT_WIDTH - 36, 112), true, "", BUGREPORT_TEXTBOX_EVENTID);
	g_bugReportTextBox->setMaxLength(1000);
	g_bugReportTextBox->startEvents();
	newWindow->addChild(g_bugReportTextBox);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 174, BUGREPORT_WIDTH - 28, 2));
	newWindow->addChild(separator);

	GUI_Button* button = new GUI_Button(iRect(BUGREPORT_WIDTH - 58, 186, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&bugReport_Events, BUGREPORT_CANCEL_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(BUGREPORT_WIDTH - 105, 186, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&bugReport_Events, BUGREPORT_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
