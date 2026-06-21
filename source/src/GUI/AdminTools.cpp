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
#include "../GUI_Elements/GUI_TextBox.h"

#define ADMINTOOLS_TITLE "Admin Tools"
#define ADMINTOOLS_WIDTH 420
#define ADMINTOOLS_HEIGHT 330

#define ADMINTOOLS_CLOSE_EVENTID         1000
#define ADMINTOOLS_ITEM_EVENTID          1001
#define ADMINTOOLS_TELEPORT_EVENTID      1002
#define ADMINTOOLS_GOTO_EVENTID          1003
#define ADMINTOOLS_BRING_EVENTID         1004
#define ADMINTOOLS_BAN_EVENTID           1005
#define ADMINTOOLS_UNBAN_EVENTID         1006
#define ADMINTOOLS_INVISIBLE_ON_EVENTID  1007
#define ADMINTOOLS_INVISIBLE_OFF_EVENTID 1008
#define ADMINTOOLS_INFO_EVENTID          1009
#define ADMINTOOLS_HELP_EVENTID          1010

#define ADMINTOOLS_ITEMID_TEXT_EVENTID   1100
#define ADMINTOOLS_COUNT_TEXT_EVENTID    1101
#define ADMINTOOLS_X_TEXT_EVENTID        1102
#define ADMINTOOLS_Y_TEXT_EVENTID        1103
#define ADMINTOOLS_Z_TEXT_EVENTID        1104
#define ADMINTOOLS_PLAYER_TEXT_EVENTID   1105
#define ADMINTOOLS_DURATION_TEXT_EVENTID 1106
#define ADMINTOOLS_REASON_TEXT_EVENTID   1107

extern Engine g_engine;
extern Game g_game;

static GUI_TextBox* s_itemId = NULL;
static GUI_TextBox* s_count = NULL;
static GUI_TextBox* s_posX = NULL;
static GUI_TextBox* s_posY = NULL;
static GUI_TextBox* s_posZ = NULL;
static GUI_TextBox* s_player = NULL;
static GUI_TextBox* s_duration = NULL;
static GUI_TextBox* s_reason = NULL;

static void showAdminError(const std::string& message)
{
	g_game.processTextMessage(MessageFailure, message);
}

static bool readNumber(GUI_TextBox* textBox, Uint32 minimum, Uint32 maximum, Uint32& value)
{
	if(!textBox || textBox->getActualText().empty())
		return false;

	const std::string& text = textBox->getActualText();
	char* end = NULL;
	unsigned long parsed = SDL_strtoul(text.c_str(), &end, 10);
	if(!end || *end != '\0' || parsed < minimum || parsed > maximum)
		return false;

	value = SDL_static_cast(Uint32, parsed);
	return true;
}

static bool isSafeText(const std::string& text, size_t maximumLength)
{
	if(text.empty() || text.length() > maximumLength)
		return false;

	for(unsigned char character : text)
	{
		if(character < 0x20 || character == 0x7F)
			return false;
	}
	return true;
}

static bool readPlayer(std::string& playerName)
{
	playerName = (s_player ? s_player->getActualText() : std::string());
	if(!isSafeText(playerName, 64) || playerName.find(',') != std::string::npos)
	{
		showAdminError("Enter a valid player name.");
		return false;
	}
	return true;
}

static void sendAdminCommand(const std::string& command)
{
	if(!g_engine.hasDevMode() || !g_engine.isIngame())
	{
		showAdminError("Admin Tools require Dev Mode and an active game session.");
		return;
	}

	if(command.empty() || command.length() > 255)
	{
		showAdminError("Admin command is too long.");
		return;
	}

	// This is intentionally the normal chat path. The server session and group
	// are the only authority for accepting or rejecting the command.
	g_game.sendSay(MessageSay, 0, std::string(), command);
}

static void adminTools_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case ADMINTOOLS_CLOSE_EVENTID:
		{
			GUI_Window* window = g_engine.getCurrentWindow();
			if(window && window->getInternalID() == GUI_WINDOW_ADMINTOOLS)
				g_engine.removeWindow(window);
		}
		break;
		case ADMINTOOLS_ITEM_EVENTID:
		{
			Uint32 itemId = 0;
			Uint32 count = 0;
			if(!readNumber(s_itemId, 1, 65535, itemId) || !readNumber(s_count, 1, 100, count))
			{
				showAdminError("Item id must be 1-65535 and count 1-100.");
				break;
			}
			sendAdminCommand("/i " + std::to_string(itemId) + ", " + std::to_string(count));
		}
		break;
		case ADMINTOOLS_TELEPORT_EVENTID:
		{
			Uint32 x = 0;
			Uint32 y = 0;
			Uint32 z = 0;
			if(!readNumber(s_posX, 0, 65535, x) || !readNumber(s_posY, 0, 65535, y) || !readNumber(s_posZ, 0, 15, z))
			{
				showAdminError("XYZ must be valid map coordinates.");
				break;
			}
			sendAdminCommand("/pos " + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z));
		}
		break;
		case ADMINTOOLS_GOTO_EVENTID:
		case ADMINTOOLS_BRING_EVENTID:
		case ADMINTOOLS_UNBAN_EVENTID:
		case ADMINTOOLS_INFO_EVENTID:
		{
			std::string playerName;
			if(!readPlayer(playerName))
				break;

			const char* command = (event == ADMINTOOLS_GOTO_EVENTID ? "/goto " :
				(event == ADMINTOOLS_BRING_EVENTID ? "/c " :
				(event == ADMINTOOLS_UNBAN_EVENTID ? "/unban " : "/info ")));
			sendAdminCommand(std::string(command) + playerName);
		}
		break;
		case ADMINTOOLS_BAN_EVENTID:
		{
			std::string playerName;
			if(!readPlayer(playerName))
				break;

			Uint32 durationDays = 0;
			if(!readNumber(s_duration, 1, 36500, durationDays))
			{
				showAdminError("Ban duration must be 1-36500 days.");
				break;
			}

			std::string reason = (s_reason ? s_reason->getActualText() : std::string());
			if(!isSafeText(reason, 96) || reason.find(',') != std::string::npos)
			{
				showAdminError("Enter a ban reason up to 96 characters without commas.");
				break;
			}

			sendAdminCommand("/ban " + playerName + ", " + std::to_string(durationDays) + ", " + reason);
		}
		break;
		case ADMINTOOLS_INVISIBLE_ON_EVENTID:
		case ADMINTOOLS_INVISIBLE_OFF_EVENTID:
			sendAdminCommand("/ghost");
			break;
		case ADMINTOOLS_HELP_EVENTID:
			g_game.processTextMessage(MessageLogin,
				"TFS commands: /i, /pos, /goto, /c, /ban, /unban, /ghost, /info.");
			break;
		default:
			break;
	}
}

void UTIL_adminTools()
{
	if(!g_engine.hasDevMode() || !g_engine.isIngame())
	{
		showAdminError("Admin Tools require Dev Mode and an active game session.");
		return;
	}

	GUI_Window* window = g_engine.getWindow(GUI_WINDOW_ADMINTOOLS);
	if(window)
	{
		g_engine.removeWindow(window);
		return;
	}

	s_itemId = s_count = s_posX = s_posY = s_posZ = NULL;
	s_player = s_duration = s_reason = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, ADMINTOOLS_WIDTH, ADMINTOOLS_HEIGHT), ADMINTOOLS_TITLE, GUI_WINDOW_ADMINTOOLS);

	GUI_Label* label = new GUI_Label(iRect(18, 25, 0, 0), "Item:");
	newWindow->addChild(label);
	s_itemId = new GUI_TextBox(iRect(70, 22, 78, 16), "", ADMINTOOLS_ITEMID_TEXT_EVENTID);
	s_itemId->setOnlyNumbers(true);
	s_itemId->setMaxLength(5);
	s_itemId->startEvents();
	newWindow->addChild(s_itemId);
	label = new GUI_Label(iRect(157, 25, 0, 0), "Count:");
	newWindow->addChild(label);
	s_count = new GUI_TextBox(iRect(205, 22, 48, 16), "1", ADMINTOOLS_COUNT_TEXT_EVENTID);
	s_count->setOnlyNumbers(true);
	s_count->setMaxLength(3);
	s_count->startEvents();
	newWindow->addChild(s_count);
	GUI_Button* button = new GUI_Button(iRect(272, 20, 86, 20), "Create Item");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_ITEM_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	label = new GUI_Label(iRect(18, 57, 0, 0), "XYZ:");
	newWindow->addChild(label);
	s_posX = new GUI_TextBox(iRect(70, 54, 72, 16), "", ADMINTOOLS_X_TEXT_EVENTID);
	s_posX->setOnlyNumbers(true);
	s_posX->setMaxLength(5);
	s_posX->startEvents();
	newWindow->addChild(s_posX);
	s_posY = new GUI_TextBox(iRect(150, 54, 72, 16), "", ADMINTOOLS_Y_TEXT_EVENTID);
	s_posY->setOnlyNumbers(true);
	s_posY->setMaxLength(5);
	s_posY->startEvents();
	newWindow->addChild(s_posY);
	s_posZ = new GUI_TextBox(iRect(230, 54, 36, 16), "", ADMINTOOLS_Z_TEXT_EVENTID);
	s_posZ->setOnlyNumbers(true);
	s_posZ->setMaxLength(2);
	s_posZ->startEvents();
	newWindow->addChild(s_posZ);
	button = new GUI_Button(iRect(272, 52, 86, 20), "Teleport");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_TELEPORT_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 84, ADMINTOOLS_WIDTH - 28, 2));
	newWindow->addChild(separator);

	label = new GUI_Label(iRect(18, 99, 0, 0), "Player:");
	newWindow->addChild(label);
	s_player = new GUI_TextBox(iRect(70, 96, 288, 16), "", ADMINTOOLS_PLAYER_TEXT_EVENTID);
	s_player->setMaxLength(64);
	s_player->startEvents();
	newWindow->addChild(s_player);

	button = new GUI_Button(iRect(18, 122, 66, 20), "Goto");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_GOTO_EVENTID);
	button->startEvents();
	newWindow->addChild(button);
	button = new GUI_Button(iRect(91, 122, 66, 20), "Bring");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_BRING_EVENTID);
	button->startEvents();
	newWindow->addChild(button);
	button = new GUI_Button(iRect(164, 122, 66, 20), "Info");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_INFO_EVENTID);
	button->startEvents();
	newWindow->addChild(button);
	button = new GUI_Button(iRect(237, 122, 66, 20), "Unban");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_UNBAN_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	label = new GUI_Label(iRect(18, 160, 0, 0), "Days:");
	newWindow->addChild(label);
	s_duration = new GUI_TextBox(iRect(88, 157, 90, 16), "1", ADMINTOOLS_DURATION_TEXT_EVENTID);
	s_duration->setOnlyNumbers(true);
	s_duration->setMaxLength(5);
	s_duration->startEvents();
	newWindow->addChild(s_duration);
	label = new GUI_Label(iRect(190, 160, 0, 0), "TFS duration in full days");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 190, 0, 0), "Reason:");
	newWindow->addChild(label);
	s_reason = new GUI_TextBox(iRect(88, 187, 270, 16), "", ADMINTOOLS_REASON_TEXT_EVENTID);
	s_reason->setMaxLength(96);
	s_reason->startEvents();
	newWindow->addChild(s_reason);
	button = new GUI_Button(iRect(272, 213, 86, 20), "Ban Player");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_BAN_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	separator = new GUI_Separator(iRect(14, 246, ADMINTOOLS_WIDTH - 28, 2));
	newWindow->addChild(separator);

	label = new GUI_Label(iRect(18, 263, 0, 0), "Invisible:");
	newWindow->addChild(label);
	button = new GUI_Button(iRect(88, 258, 70, 20), "Toggle");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_INVISIBLE_ON_EVENTID);
	button->startEvents();
	newWindow->addChild(button);
	button = new GUI_Button(iRect(170, 258, 70, 20), "GM Help");
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_HELP_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	separator = new GUI_Separator(iRect(14, 292, ADMINTOOLS_WIDTH - 28, 2));
	newWindow->addChild(separator);
	button = new GUI_Button(iRect(ADMINTOOLS_WIDTH - 58, 300, 43, 20), "Close", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&adminTools_Events, ADMINTOOLS_CLOSE_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
