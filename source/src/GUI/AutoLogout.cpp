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
#include "../GUI_Elements/GUI_ListBox.h"

#define AUTOLOGOUT_TITLE "Auto Logout"
#define AUTOLOGOUT_WIDTH 340
#define AUTOLOGOUT_HEIGHT 580

#define AUTOLOGOUT_OK_EVENTID       1000
#define AUTOLOGOUT_CANCEL_EVENTID   1001
#define AUTOLOGOUT_ENABLED_EVENTID  1002
#define AUTOLOGOUT_PK_EVENTID       1003
#define AUTOLOGOUT_PLAYER_EVENTID   1004
#define AUTOLOGOUT_TIMER_EVENTID    1005
#define AUTOLOGOUT_POLL_EVENTID     1006
#define AUTOLOGOUT_GMLIST_EVENTID   1007
#define AUTOLOGOUT_ADD_EVENTID      1008
#define AUTOLOGOUT_REMOVE_EVENTID   1009
#define AUTOLOGOUT_GMENTRY_EVENTID  1010
#define AUTOLOGOUT_PLAYERLIST_EVENTID 1011
#define AUTOLOGOUT_PLAYERADD_EVENTID  1012
#define AUTOLOGOUT_PLAYERREMOVE_EVENTID 1013
#define AUTOLOGOUT_PLAYERENTRY_EVENTID 1014
#define AUTOLOGOUT_CAPTURE_EVENTID 1015
#define AUTOLOGOUT_WEBHOOK_EVENTID 1016
#define AUTOLOGOUT_TEST_ALERT_EVENTID 1017
#define AUTOLOGOUT_ON_PM_EVENTID 1018
#define AUTOLOGOUT_ON_BROADCAST_EVENTID 1019
#define AUTOLOGOUT_ALARM_EVENTID 1020

extern Engine g_engine;

static GUI_CheckBox* s_enabled   = NULL;
static GUI_CheckBox* s_checkPK   = NULL;
static GUI_CheckBox* s_checkPlayer = NULL;
static GUI_TextBox*  s_timer     = NULL;
static GUI_TextBox*  s_poll      = NULL;
static GUI_ListBox*  s_gmList    = NULL;
static GUI_TextBox*  s_gmEntry   = NULL;
static GUI_ListBox*  s_playerList = NULL;
static GUI_TextBox*  s_playerEntry = NULL;
static GUI_CheckBox* s_captureAlerts = NULL;
static GUI_TextBox*  s_discordWebhook = NULL;
static GUI_CheckBox* s_alarmOnDetect = NULL;
static GUI_CheckBox* s_logoutOnPM = NULL;
static GUI_CheckBox* s_logoutOnBroadcast = NULL;

static Uint32 readNumber(GUI_TextBox* tb, Uint32 fallback)
{
	if(!tb) return fallback;
	std::string v = tb->getActualText();
	if(v.empty()) return fallback;
	return SDL_static_cast(Uint32, SDL_strtoul(v.c_str(), NULL, 10));
}

static void autoLogout_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case AUTOLOGOUT_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(!pWindow || pWindow->getInternalID() != GUI_WINDOW_AUTOLOGOUT)
				break;

			bool enabled    = s_enabled    ? s_enabled->isChecked()    : g_engine.getAutoLogoutEnabled();
			bool checkPK    = s_checkPK    ? s_checkPK->isChecked()    : g_engine.getAutoLogoutPK();
			bool checkPlayer= s_checkPlayer? s_checkPlayer->isChecked(): g_engine.getAutoLogoutPlayer();
			Uint32 timerMin = readNumber(s_timer, g_engine.getAutoLogoutTimerMin());
			Uint32 pollSec  = UTIL_max<Uint32>(5, UTIL_min<Uint32>(300, readNumber(s_poll, g_engine.getAutoLogoutPollSec())));

			std::vector<std::string> gmList;
			if(s_gmList)
			{
				// Read entries back from listbox internal vector via repeated getSelect trick:
				// We stored them in order, so we can retrieve via existing GM list + any added
				gmList = g_engine.getAutoLogoutGMs();
			}
			std::vector<std::string> playerList = g_engine.getAutoLogoutPlayers();
			bool captureAlerts = s_captureAlerts ? s_captureAlerts->isChecked() : g_engine.getAlertCaptureEnabled();
			std::string webhook = s_discordWebhook ? s_discordWebhook->getActualText() : g_engine.getDiscordWebhook();

			g_engine.setAutoLogoutOptions(enabled, checkPK, checkPlayer, timerMin, pollSec, gmList, playerList);
			g_engine.setAlertOptions(captureAlerts, webhook);
			g_engine.setPlayerAlarmEnabled(s_alarmOnDetect ? s_alarmOnDetect->isChecked() : g_engine.getPlayerAlarmEnabled());
			g_engine.setAutoLogoutOnPM(s_logoutOnPM ? s_logoutOnPM->isChecked() : g_engine.getAutoLogoutOnPM());
			g_engine.setAutoLogoutOnBroadcast(s_logoutOnBroadcast ? s_logoutOnBroadcast->isChecked() : g_engine.getAutoLogoutOnBroadcast());
			g_engine.saveCFG();
			g_engine.removeWindow(pWindow);
		}
		break;
		case AUTOLOGOUT_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_AUTOLOGOUT)
				g_engine.removeWindow(pWindow);
		}
		break;
		case AUTOLOGOUT_ADD_EVENTID:
		{
			if(!s_gmEntry || !s_gmList) break;
			std::string name = s_gmEntry->getActualText();
			if(name.empty()) break;
			s_gmList->add(name);
			// Also update engine's GM list immediately so OK can read it
			std::vector<std::string> gms = g_engine.getAutoLogoutGMs();
			gms.push_back(name);
			g_engine.setAutoLogoutOptions(
				g_engine.getAutoLogoutEnabled(), g_engine.getAutoLogoutPK(),
				g_engine.getAutoLogoutPlayer(), g_engine.getAutoLogoutTimerMin(),
				g_engine.getAutoLogoutPollSec(), gms, g_engine.getAutoLogoutPlayers());
			s_gmEntry->setText(std::string());
		}
		break;
		case AUTOLOGOUT_REMOVE_EVENTID:
		{
			if(!s_gmList) break;
			Sint32 sel = s_gmList->getSelect();
			if(sel < 0) break;
			s_gmList->erase(sel);
			std::vector<std::string> gms = g_engine.getAutoLogoutGMs();
			if(SDL_static_cast(size_t, sel) < gms.size())
			{
				gms.erase(gms.begin() + sel);
				g_engine.setAutoLogoutOptions(
					g_engine.getAutoLogoutEnabled(), g_engine.getAutoLogoutPK(),
					g_engine.getAutoLogoutPlayer(), g_engine.getAutoLogoutTimerMin(),
					g_engine.getAutoLogoutPollSec(), gms, g_engine.getAutoLogoutPlayers());
			}
		}
		break;
		case AUTOLOGOUT_PLAYERADD_EVENTID:
		{
			if(!s_playerEntry || !s_playerList) break;
			std::string name = s_playerEntry->getActualText();
			if(name.empty()) break;
			s_playerList->add(name);
			std::vector<std::string> players = g_engine.getAutoLogoutPlayers();
			players.push_back(name);
			g_engine.setAutoLogoutOptions(
				g_engine.getAutoLogoutEnabled(), g_engine.getAutoLogoutPK(),
				g_engine.getAutoLogoutPlayer(), g_engine.getAutoLogoutTimerMin(),
				g_engine.getAutoLogoutPollSec(), g_engine.getAutoLogoutGMs(), players);
			s_playerEntry->setText(std::string());
		}
		break;
		case AUTOLOGOUT_PLAYERREMOVE_EVENTID:
		{
			if(!s_playerList) break;
			Sint32 sel = s_playerList->getSelect();
			if(sel < 0) break;
			s_playerList->erase(sel);
			std::vector<std::string> players = g_engine.getAutoLogoutPlayers();
			if(SDL_static_cast(size_t, sel) < players.size())
			{
				players.erase(players.begin() + sel);
				g_engine.setAutoLogoutOptions(
					g_engine.getAutoLogoutEnabled(), g_engine.getAutoLogoutPK(),
					g_engine.getAutoLogoutPlayer(), g_engine.getAutoLogoutTimerMin(),
					g_engine.getAutoLogoutPollSec(), g_engine.getAutoLogoutGMs(), players);
			}
		}
		break;
		case AUTOLOGOUT_TEST_ALERT_EVENTID:
		{
			bool captureAlerts = s_captureAlerts ? s_captureAlerts->isChecked() : g_engine.getAlertCaptureEnabled();
			std::string webhook = s_discordWebhook ? s_discordWebhook->getActualText() : g_engine.getDiscordWebhook();
			g_engine.setAlertOptions(captureAlerts, webhook);
			g_engine.saveCFG();
			g_engine.captureAlertEvent("Test notification");
		}
		break;
		default: break;
	}
}

void UTIL_autoLogoutOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_AUTOLOGOUT);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_enabled = s_checkPK = s_checkPlayer = s_captureAlerts = NULL;
	s_timer = s_poll = s_gmEntry = s_playerEntry = s_discordWebhook = NULL;
	s_gmList = s_playerList = NULL;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, AUTOLOGOUT_WIDTH, AUTOLOGOUT_HEIGHT), AUTOLOGOUT_TITLE, GUI_WINDOW_AUTOLOGOUT);

	s_enabled = new GUI_CheckBox(iRect(18, 24, 300, 16), "Enabled", g_engine.getAutoLogoutEnabled(), AUTOLOGOUT_ENABLED_EVENTID);
	s_enabled->startEvents();
	newWindow->addChild(s_enabled);

	// Timer row
	GUI_Label* label = new GUI_Label(iRect(18, 50, 0, 0), "Logout after:");
	newWindow->addChild(label);
	s_timer = new GUI_TextBox(iRect(120, 48, 48, 16), std::to_string(g_engine.getAutoLogoutTimerMin()), AUTOLOGOUT_TIMER_EVENTID);
	s_timer->setOnlyNumbers(true);
	s_timer->setMaxLength(4);
	s_timer->startEvents();
	newWindow->addChild(s_timer);
	label = new GUI_Label(iRect(174, 50, 0, 0), "min  (0 = off)");
	newWindow->addChild(label);

	// PK / Player checkboxes
	s_checkPK = new GUI_CheckBox(iRect(18, 72, 300, 16), "PK skull detected (any skull)", g_engine.getAutoLogoutPK(), AUTOLOGOUT_PK_EVENTID);
	s_checkPK->startEvents();
	newWindow->addChild(s_checkPK);

	s_checkPlayer = new GUI_CheckBox(iRect(18, 92, 300, 16), "Player nearby (visible, not self)", g_engine.getAutoLogoutPlayer(), AUTOLOGOUT_PLAYER_EVENTID);
	s_checkPlayer->startEvents();
	newWindow->addChild(s_checkPlayer);

	GUI_Separator* sep = new GUI_Separator(iRect(14, 114, AUTOLOGOUT_WIDTH - 28, 2));
	newWindow->addChild(sep);

	// Watched players use VIP status first and myAAC green/red as fallback.
	label = new GUI_Label(iRect(18, 122, 0, 0), "Players/VIPs (logout if online):");
	newWindow->addChild(label);

	s_playerList = new GUI_ListBox(iRect(18, 138, AUTOLOGOUT_WIDTH - 36, 58), AUTOLOGOUT_PLAYERLIST_EVENTID);
	s_playerList->startEvents();
	const std::vector<std::string>& players = g_engine.getAutoLogoutPlayers();
	for(const std::string& player : players)
		s_playerList->add(player);
	newWindow->addChild(s_playerList);

	s_playerEntry = new GUI_TextBox(iRect(18, 202, AUTOLOGOUT_WIDTH - 122, 16), "", AUTOLOGOUT_PLAYERENTRY_EVENTID);
	s_playerEntry->setMaxLength(64);
	s_playerEntry->startEvents();
	newWindow->addChild(s_playerEntry);

	GUI_Button* btn = new GUI_Button(iRect(AUTOLOGOUT_WIDTH - 100, 200, 38, 20), "Add", 0);
	btn->setButtonEventCallback(&autoLogout_Events, AUTOLOGOUT_PLAYERADD_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	btn = new GUI_Button(iRect(AUTOLOGOUT_WIDTH - 58, 200, 48, 20), "Remove", 0);
	btn->setButtonEventCallback(&autoLogout_Events, AUTOLOGOUT_PLAYERREMOVE_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	label = new GUI_Label(iRect(18, 228, 0, 0), "Hidden GMs (Last Login changes):");
	newWindow->addChild(label);

	s_gmList = new GUI_ListBox(iRect(18, 244, AUTOLOGOUT_WIDTH - 36, 58), AUTOLOGOUT_GMLIST_EVENTID);
	s_gmList->startEvents();
	const std::vector<std::string>& gms = g_engine.getAutoLogoutGMs();
	for(const std::string& gm : gms)
		s_gmList->add(gm);
	newWindow->addChild(s_gmList);

	// Add / Remove row
	s_gmEntry = new GUI_TextBox(iRect(18, 308, AUTOLOGOUT_WIDTH - 122, 16), "", AUTOLOGOUT_GMENTRY_EVENTID);
	s_gmEntry->setMaxLength(64);
	s_gmEntry->startEvents();
	newWindow->addChild(s_gmEntry);

	btn = new GUI_Button(iRect(AUTOLOGOUT_WIDTH - 100, 306, 38, 20), "Add", 0);
	btn->setButtonEventCallback(&autoLogout_Events, AUTOLOGOUT_ADD_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	btn = new GUI_Button(iRect(AUTOLOGOUT_WIDTH - 58, 306, 48, 20), "Remove", 0);
	btn->setButtonEventCallback(&autoLogout_Events, AUTOLOGOUT_REMOVE_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	// Poll interval
	label = new GUI_Label(iRect(18, 336, 0, 0), "Poll every:");
	newWindow->addChild(label);
	s_poll = new GUI_TextBox(iRect(90, 334, 48, 16), std::to_string(g_engine.getAutoLogoutPollSec()), AUTOLOGOUT_POLL_EVENTID);
	s_poll->setOnlyNumbers(true);
	s_poll->setMaxLength(3);
	s_poll->startEvents();
	newWindow->addChild(s_poll);
	label = new GUI_Label(iRect(144, 336, 0, 0), "sec");
	newWindow->addChild(label);

	sep = new GUI_Separator(iRect(14, 360, AUTOLOGOUT_WIDTH - 28, 2));
	newWindow->addChild(sep);

	s_captureAlerts = new GUI_CheckBox(iRect(18, 370, 300, 16), "Capture logout/death context", g_engine.getAlertCaptureEnabled(), AUTOLOGOUT_CAPTURE_EVENTID);
	s_captureAlerts->startEvents();
	newWindow->addChild(s_captureAlerts);

	label = new GUI_Label(iRect(18, 394, 0, 0), "Discord webhook (optional):");
	newWindow->addChild(label);
	s_discordWebhook = new GUI_TextBox(iRect(18, 410, AUTOLOGOUT_WIDTH - 36, 16), g_engine.getDiscordWebhook(), AUTOLOGOUT_WEBHOOK_EVENTID);
	s_discordWebhook->setMaxLength(512);
	s_discordWebhook->setHideCharacter('*');
	s_discordWebhook->startEvents();
	newWindow->addChild(s_discordWebhook);

	btn = new GUI_Button(iRect(18, 434, 60, 20), "Test Alert", 0);
	btn->setButtonEventCallback(&autoLogout_Events, AUTOLOGOUT_TEST_ALERT_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	sep = new GUI_Separator(iRect(14, 462, AUTOLOGOUT_WIDTH - 28, 2));
	newWindow->addChild(sep);

	s_alarmOnDetect = new GUI_CheckBox(iRect(18, 472, 300, 16), "Sound alarm on player/PK detection", g_engine.getPlayerAlarmEnabled(), AUTOLOGOUT_ALARM_EVENTID);
	s_alarmOnDetect->startEvents();
	newWindow->addChild(s_alarmOnDetect);

	s_logoutOnPM = new GUI_CheckBox(iRect(18, 492, 300, 16), "Logout on private message received", g_engine.getAutoLogoutOnPM(), AUTOLOGOUT_ON_PM_EVENTID);
	s_logoutOnPM->startEvents();
	newWindow->addChild(s_logoutOnPM);

	s_logoutOnBroadcast = new GUI_CheckBox(iRect(18, 512, 300, 16), "Logout on broadcast / GM message", g_engine.getAutoLogoutOnBroadcast(), AUTOLOGOUT_ON_BROADCAST_EVENTID);
	s_logoutOnBroadcast->startEvents();
	newWindow->addChild(s_logoutOnBroadcast);

	sep = new GUI_Separator(iRect(14, 536, AUTOLOGOUT_WIDTH - 28, 2));
	newWindow->addChild(sep);

	btn = new GUI_Button(iRect(AUTOLOGOUT_WIDTH - 58, 548, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	btn->setButtonEventCallback(&autoLogout_Events, AUTOLOGOUT_CANCEL_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	btn = new GUI_Button(iRect(AUTOLOGOUT_WIDTH - 105, 548, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	btn->setButtonEventCallback(&autoLogout_Events, AUTOLOGOUT_OK_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	g_engine.addWindow(newWindow, true);
}
