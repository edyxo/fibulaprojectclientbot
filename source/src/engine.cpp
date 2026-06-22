/*
  The Forgotten Client
  Copyright (C) 2020 Saiyans King

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "engine.h"
#include "surfaceSoftware.h"
#include "surfaceDirect3D9.h"
#include "surfaceDirect3D11.h"
#include "surfaceDirectDraw.h"
#include "surfaceOpenglCore.h"
#include "surfaceOpengl.h"
#include "surfaceOpengles.h"
#include "surfaceOpengles2.h"
#include "surfaceVulkan.h"
#include "item.h"
#include "map.h"
#include "tile.h"
#include "thingManager.h"
#include "spriteManager.h"
#include "animator.h"
#include "protocollogin.h"
#include "protocolgame.h"
#include "creature.h"
#include "game.h"
#include "container.h"
#include "config.h"
#include "http.h"

#include "GUI_Elements/GUI_Window.h"
#include "GUI_Elements/GUI_Panel.h"
#include "GUI_Elements/GUI_PanelWindow.h"
#include "GUI_Elements/GUI_Description.h"
#include "GUI_Elements/GUI_ContextMenu.h"
#include "GUI_Elements/GUI_Log.h"
#include "GUI/itemUI.h"
#include "GUI/Chat.h"
#include "GUI/GUI_UTIL.h"
#include "GUI_Elements/GUI_TextBox.h"

#include <sstream>
#include <typeinfo>
#if defined(__WIN32__)
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#define _mkdir(x) mkdir((x), 0755)
#endif
#include <deque>

extern Engine g_engine;
extern Map g_map;
extern Http g_http;
extern ThingManager g_thingManager;
extern SpriteManager g_spriteManager;
extern Connection* g_connection;
extern GUI_Log g_logger;
extern Game g_game;
extern Chat g_chat;

extern bool g_running;
extern bool g_inited;
extern Sint32 g_actualCursor;
extern Uint32 g_frameTime;
extern Uint32 g_picRevision;
extern Uint32 g_spriteCounts;
extern Uint16 g_pictureCounts;
extern Uint16 g_ping;

GUI_Window* g_mainWindow = NULL;

static const Sint32 HOTKEY_CONFIG_COUNT = 36;
static const SDL_Keycode s_hotkeyConfigKeys[HOTKEY_CONFIG_COUNT] = {
	SDLK_F1,  SDLK_F2,  SDLK_F3,  SDLK_F4,  SDLK_F5,  SDLK_F6,
	SDLK_F7,  SDLK_F8,  SDLK_F9,  SDLK_F10, SDLK_F11, SDLK_F12,
	SDLK_F1,  SDLK_F2,  SDLK_F3,  SDLK_F4,  SDLK_F5,  SDLK_F6,
	SDLK_F7,  SDLK_F8,  SDLK_F9,  SDLK_F10, SDLK_F11, SDLK_F12,
	SDLK_F1,  SDLK_F2,  SDLK_F3,  SDLK_F4,  SDLK_F5,  SDLK_F6,
	SDLK_F7,  SDLK_F8,  SDLK_F9,  SDLK_F10, SDLK_F11, SDLK_F12
};
static const Uint16 s_hotkeyConfigMods[HOTKEY_CONFIG_COUNT] = {
	KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,
	KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,
	KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT,
	KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT,
	KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,
	KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL
};
static const char* const s_hotkeyConfigNames[HOTKEY_CONFIG_COUNT] = {
	"HotkeyF1", "HotkeyF2", "HotkeyF3", "HotkeyF4", "HotkeyF5", "HotkeyF6",
	"HotkeyF7", "HotkeyF8", "HotkeyF9", "HotkeyF10", "HotkeyF11", "HotkeyF12",
	"HotkeyShiftF1", "HotkeyShiftF2", "HotkeyShiftF3", "HotkeyShiftF4", "HotkeyShiftF5", "HotkeyShiftF6",
	"HotkeyShiftF7", "HotkeyShiftF8", "HotkeyShiftF9", "HotkeyShiftF10", "HotkeyShiftF11", "HotkeyShiftF12",
	"HotkeyCtrlF1", "HotkeyCtrlF2", "HotkeyCtrlF3", "HotkeyCtrlF4", "HotkeyCtrlF5", "HotkeyCtrlF6",
	"HotkeyCtrlF7", "HotkeyCtrlF8", "HotkeyCtrlF9", "HotkeyCtrlF10", "HotkeyCtrlF11", "HotkeyCtrlF12"
};

// Manual world actions are buffered separately from movement. Pure inventory /
// container packets use Tibia's virtual item positions and do not touch map
// pathing, so they can be sent while the player is walking.
enum ManualActionType
{
	MANUAL_ACTION_MOVE,
	MANUAL_ACTION_USE_ITEM,
	MANUAL_ACTION_USE_ITEM_EX
};

struct QueuedManualAction
{
	ManualActionType type = MANUAL_ACTION_MOVE;
	Position fromPos;
	Position toPos;
	Uint16 itemId = 0;
	Uint16 toItemId = 0;
	Uint16 count = 0;
	Uint8 stackpos = 0;
	Uint8 toStackpos = 0;
	Uint8 index = 0;
	Uint32 enqueueTime = 0;
	bool safeWhileWalking = false;
};

static const size_t MANUAL_ACTION_MAX_QUEUE = 32;
static const Uint32 MANUAL_ACTION_MIN_DELAY = 250;
static const Uint32 MANUAL_ACTION_MAX_WAIT = 1200;
static const Uint32 MANUAL_ACTION_COOLDOWN = 300;
static std::deque<QueuedManualAction> g_manualActionQueue;

static bool UTIL_isVirtualItemPosition(const Position& pos)
{
	return pos.x == 0xFFFF;
}

// Keeps all queued variants using the exact packet shape that the original
// direct UI paths used, so the scheduler does not change protocol semantics.
static void UTIL_sendQueuedManualAction(const QueuedManualAction& action)
{
	switch(action.type)
	{
		case MANUAL_ACTION_MOVE:
			g_game.sendMove(action.fromPos, action.itemId, action.stackpos, action.toPos, action.count);
			break;
		case MANUAL_ACTION_USE_ITEM:
			g_game.sendUseItem(action.fromPos, action.itemId, action.stackpos, action.index);
			break;
		case MANUAL_ACTION_USE_ITEM_EX:
			g_game.sendUseItemEx(action.fromPos, action.itemId, action.stackpos, action.toPos, action.toItemId, action.toStackpos);
			break;
		default: break;
	}
}

static bool UTIL_isClassicMovementKey(SDL_Keycode key)
{
	switch(key)
	{
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
		case SDLK_KP_1:
		case SDLK_KP_2:
		case SDLK_KP_3:
		case SDLK_KP_4:
		case SDLK_KP_6:
		case SDLK_KP_7:
		case SDLK_KP_8:
		case SDLK_KP_9:
			return true;
		default:
			return false;
	}
}

static bool UTIL_isNumpadMovementKey(SDL_Keycode key)
{
	switch(key)
	{
		case SDLK_KP_1:
		case SDLK_KP_2:
		case SDLK_KP_3:
		case SDLK_KP_4:
		case SDLK_KP_6:
		case SDLK_KP_7:
		case SDLK_KP_8:
		case SDLK_KP_9:
			return true;
		default:
			return false;
	}
}

static bool UTIL_isNumpadChatNumber(SDL_Keycode key, Uint16 mods)
{
	return UTIL_isNumpadMovementKey(key) && (mods & KMOD_NUM);
}

static bool UTIL_tryAutoStackCoin(ItemUI* itemui);

static bool UTIL_getClassicMovementDirection(SDL_Keycode key, Direction& direction)
{
	switch(key)
	{
		case SDLK_UP:
		case SDLK_KP_8: direction = DIRECTION_NORTH; return true;
		case SDLK_DOWN:
		case SDLK_KP_2: direction = DIRECTION_SOUTH; return true;
		case SDLK_LEFT:
		case SDLK_KP_4: direction = DIRECTION_WEST; return true;
		case SDLK_RIGHT:
		case SDLK_KP_6: direction = DIRECTION_EAST; return true;
		case SDLK_KP_7: direction = DIRECTION_NORTHWEST; return true;
		case SDLK_KP_9: direction = DIRECTION_NORTHEAST; return true;
		case SDLK_KP_1: direction = DIRECTION_SOUTHWEST; return true;
		case SDLK_KP_3: direction = DIRECTION_SOUTHEAST; return true;
		default: return false;
	}
}

static bool UTIL_getClassicTurnDirection(SDL_Keycode key, Direction& direction)
{
	switch(key)
	{
		case SDLK_UP:
		case SDLK_KP_8: direction = DIRECTION_NORTH; return true;
		case SDLK_DOWN:
		case SDLK_KP_2: direction = DIRECTION_SOUTH; return true;
		case SDLK_LEFT:
		case SDLK_KP_4: direction = DIRECTION_WEST; return true;
		case SDLK_RIGHT:
		case SDLK_KP_6: direction = DIRECTION_EAST; return true;
		default: return false;
	}
}

static Uint16 UTIL_getNormalizedKeyboardMods(Uint16 mods)
{
	Uint16 normalMods = KMOD_NONE;
	if(mods & KMOD_SHIFT) normalMods |= KMOD_SHIFT;
	if(mods & KMOD_CTRL)  normalMods |= KMOD_CTRL;
	if(mods & KMOD_ALT)   normalMods |= KMOD_ALT;
	return normalMods;
}

Engine::Engine()
{
	m_engines.emplace_back(CLIENT_ENGINE_SOFTWARE);
	#if defined(SDL_VIDEO_VULKAN)
	m_engines.emplace_back(CLIENT_ENGINE_VULKAN);
	#endif
	#if defined(SDL_VIDEO_RENDER_D3D11)
	m_engines.emplace_back(CLIENT_ENGINE_DIRECT3D11);
	#endif
	#if defined(SDL_VIDEO_RENDER_DDRAW)
	m_engines.emplace_back(CLIENT_ENGINE_DIRECT3D7);
	#endif
	#if defined(SDL_VIDEO_RENDER_OGL)
	m_engines.emplace_back(CLIENT_ENGINE_OPENGLCORE);
	m_engines.emplace_back(CLIENT_ENGINE_OPENGL);
	#else
	#if defined(SDL_VIDEO_RENDER_OGL_ES)
	m_engines.emplace_back(CLIENT_ENGINE_OPENGLES);
	#endif
	#if defined(SDL_VIDEO_RENDER_OGL_ES2)
	m_engines.emplace_back(CLIENT_ENGINE_OPENGLES2);
	#endif
	#endif
	#if defined(SDL_VIDEO_RENDER_D3D)
	m_engines.emplace_back(CLIENT_ENGINE_DIRECT3D);
	#endif

	m_engine = m_engines.back();
	for(Sint32 i = Skills_Fist; i < Skills_LastSkill; ++i)
		m_showSkillsBar[i] = true;

	m_leftPanel = GUI_PANEL_RANDOM;
	m_rightPanel = GUI_PANEL_MAIN;
}

void Engine::loadCFG()
{
	Config cfg;
	SDL_snprintf(g_buffer, sizeof(g_buffer), "%sconfig.cfg", g_prefPath.c_str());
	if(cfg.openToRead(g_buffer))
	{
		std::string data = cfg.fetchKey("Version");
		if(data.empty())
			return;
		#if !(CLIENT_OVVERIDE_VERSION > 0)
		else
			g_clientVersion = SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10));

		data = cfg.fetchKey("Host");
		m_clientHost.assign(data.empty() ? "https://amera.fibula.app/" : data);
		data = cfg.fetchKey("Port");
		m_clientPort.assign(data.empty() ? "443" : data);
		data = cfg.fetchKey("Proxy");
		m_clientProxy.assign(data);
		data = cfg.fetchKey("ProxyAuth");
		m_clientProxyAuth.assign(data);
		#endif

		data = cfg.fetchKey("Engine");
		m_engine = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		data = cfg.fetchKey("BasicBrightness");
		m_lightAmbient = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_lightAmbient = (m_lightAmbient > 100 ? 100 : m_lightAmbient);
		data = cfg.fetchKey("LevelSeparator");
		m_levelSeparator = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_levelSeparator = (m_levelSeparator > 100 ? 100 : m_levelSeparator);
		data = cfg.fetchKey("LightMode");
		m_lightMode = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_lightMode = (m_lightMode > CLIENT_LIGHT_MODE_NEW ? CLIENT_LIGHT_MODE_OLD : m_lightMode);

		data = cfg.fetchKey("WindowedMode");
		if(data.size() > 2)
		{
			data.pop_back();
			data.erase(data.begin());

			StringVector windowData = UTIL_explodeString(data, ",");
			if(windowData.size() == 5)
			{
				m_windowX = SDL_static_cast(Sint32, SDL_strtol(windowData[0].c_str(), NULL, 10));
				m_windowY = SDL_static_cast(Sint32, SDL_strtol(windowData[1].c_str(), NULL, 10));
				m_windowW = SDL_static_cast(Sint32, SDL_strtol(windowData[2].c_str(), NULL, 10));
				m_windowH = SDL_static_cast(Sint32, SDL_strtol(windowData[3].c_str(), NULL, 10));
				m_windowCachedW = m_windowW;
				m_windowCachedH = m_windowH;
				m_maximized = (windowData[4] == "yes" ? true : false);
			}
		}
		data = cfg.fetchKey("Fullscreen");
		m_fullscreen = (data == "yes" ? true : false);
		data = cfg.fetchKey("FullscreenMode");
		if(data.size() > 2)
		{
			data.pop_back();
			data.erase(data.begin());

			StringVector windowData = UTIL_explodeString(data, ",");
			if(windowData.size() == 3)
			{
				m_fullScreenWidth = SDL_static_cast(Sint32, SDL_strtol(windowData[0].c_str(), NULL, 10));
				m_fullScreenHeight = SDL_static_cast(Sint32, SDL_strtol(windowData[1].c_str(), NULL, 10));
				m_fullScreenBits = SDL_static_cast(Sint32, SDL_strtol(windowData[2].c_str(), NULL, 10));
			}
			else if(windowData.size() == 4)
			{
				m_fullScreenWidth = SDL_static_cast(Sint32, SDL_strtol(windowData[0].c_str(), NULL, 10));
				m_fullScreenHeight = SDL_static_cast(Sint32, SDL_strtol(windowData[1].c_str(), NULL, 10));
				m_fullScreenBits = SDL_static_cast(Sint32, SDL_strtol(windowData[2].c_str(), NULL, 10));
				m_fullScreenHZ = SDL_static_cast(Sint32, SDL_strtol(windowData[3].c_str(), NULL, 10));
			}
		}

		data = cfg.fetchKey("ConfineFramerate");
		SDL_setFramerate(&g_fpsmanager, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)));
		data = cfg.fetchKey("NolimitFramerate");
		m_unlimitedFPS = (data == "yes" ? true : false);

		data = cfg.fetchKey("VerticalSync");
		m_vsync = (data == "yes" ? true : false);
		data = cfg.fetchKey("Sharpening");
		m_sharpening = (data == "yes" ? true : false);
		data = cfg.fetchKey("Antialiasing");
		m_antialiasing = (data == "yes" ? CLIENT_ANTIALIASING_NORMAL : data == "integer" ? CLIENT_ANTIALIASING_INTEGER : CLIENT_ANTIALIASING_NONE);
		m_antialiasing = (m_antialiasing > CLIENT_ANTIALIASING_INTEGER ? CLIENT_ANTIALIASING_NORMAL : m_antialiasing);

		data = cfg.fetchKey("ClassicControl");
		m_classicControl = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoChaseOff");
		m_autoChaseOff = (data == "yes" ? true : false);
		data = cfg.fetchKey("DevMode");
		m_devMode = (data == "yes" ? true : false);
		data = cfg.fetchKey("ShowDebugCoordinates");
		m_showDebugCoordinates = (m_devMode && data == "yes" ? true : false);
		data = cfg.fetchKey("ManaTrainerEnabled");
		if(!data.empty())
			m_manaTrainer = (data == "yes" ? true : false);
		data = cfg.fetchKey("ManaTrainerSpell");
		if(!data.empty())
			m_manaTrainerSpell = data;
		data = cfg.fetchKey("ManaTrainerManaPercent");
		if(!data.empty())
			m_manaTrainerManaPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))));
		data = cfg.fetchKey("ManaTrainerDelay");
		if(!data.empty())
			m_manaTrainerDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10))));
		data = cfg.fetchKey("AutoFisherEnabled");
		if(!data.empty())
			m_autoFisher = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoFisherDelay");
		if(!data.empty())
		{
			Uint32 delay = UTIL_min<Uint32>(30000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)));
			m_autoFisherDelayMin = delay;
			m_autoFisherDelayMax = delay;
		}
		data = cfg.fetchKey("AutoFisherDelayMin");
		if(!data.empty())
			m_autoFisherDelayMin = UTIL_min<Uint32>(30000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)));
		data = cfg.fetchKey("AutoFisherDelayMax");
		if(!data.empty())
			m_autoFisherDelayMax = UTIL_min<Uint32>(30000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)));
		if(m_autoFisherDelayMax < m_autoFisherDelayMin)
			m_autoFisherDelayMax = m_autoFisherDelayMin;
		if(m_autoFisherDelayMax < 1000)
			m_autoFisherDelayMax = 1000;
		data = cfg.fetchKey("AutoLogoutEnabled");
		if(!data.empty())
			m_autoLogout = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoLogoutPK");
		if(!data.empty())
			m_autoLogoutPK = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoLogoutPlayer");
		if(!data.empty())
			m_autoLogoutPlayer = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoLogoutTimerMin");
		if(!data.empty())
			m_autoLogoutTimerMin = SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10));
		data = cfg.fetchKey("AutoLogoutPollSec");
		if(!data.empty())
			m_gmPollInterval = UTIL_max<Uint32>(5, UTIL_min<Uint32>(300, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))) * 1000;
		data = cfg.fetchKey("AutoLogoutGMs");
		if(!data.empty())
		{
			m_autoLogoutGMs.clear();
			std::string token;
			for(char c : data)
			{
				if(c == ';') { if(!token.empty()) { m_autoLogoutGMs.push_back(token); token.clear(); } }
				else token += c;
			}
			if(!token.empty()) m_autoLogoutGMs.push_back(token);
		}
		data = cfg.fetchKey("AutoLogoutPlayers");
		if(!data.empty())
		{
			m_autoLogoutPlayers.clear();
			std::string token;
			for(char c : data)
			{
				if(c == ';') { if(!token.empty()) { m_autoLogoutPlayers.push_back(token); token.clear(); } }
				else token += c;
			}
			if(!token.empty()) m_autoLogoutPlayers.push_back(token);
		}
		data = cfg.fetchKey("AlertCaptureEnabled");
		if(!data.empty())
			m_alertCaptureEnabled = (data == "yes");
		data = cfg.fetchKey("DiscordWebhook");
		if(!data.empty())
			m_discordWebhook = data;
		data = cfg.fetchKey("AutoLogoutOnPM");
		if(!data.empty()) m_autoLogoutOnPM = (data == "yes");
		data = cfg.fetchKey("AutoLogoutOnBroadcast");
		if(!data.empty()) m_autoLogoutOnBroadcast = (data == "yes");
		data = cfg.fetchKey("PlayerAlarmEnabled");
		if(!data.empty()) m_playerAlarmEnabled = (data == "yes");
		data = cfg.fetchKey("FullBrightness");
		if(!data.empty()) m_fullBrightness = (data == "yes");
		data = cfg.fetchKey("HideWASD");
		if(!data.empty()) m_hideWASD = (data == "yes");
		data = cfg.fetchKey("HideHPManaPercent");
		if(!data.empty()) m_hideHPManaPercent = (data == "yes");
		data = cfg.fetchKey("AutoRunerEnabled");
		if(!data.empty())
			m_autoRuner = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoRunerSpell");
		if(!data.empty())
			m_autoRunerSpell = data;
		data = cfg.fetchKey("AutoRunerManaPercent");
		if(!data.empty())
			m_autoRunerManaPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))));
		data = cfg.fetchKey("AutoRunerDelay");
		if(!data.empty())
			m_autoRunerDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10))));
		data = cfg.fetchKey("AutoEaterEnabled");
		if(!data.empty())
			m_autoEater = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoEaterFoodId");
		if(!data.empty())
			m_autoEaterFoodId = SDL_static_cast(Uint16, UTIL_max<Uint32>(100, UTIL_min<Uint32>(65535, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))));
		data = cfg.fetchKey("AutoEaterDelay");
		if(!data.empty())
			m_autoEaterDelay = UTIL_max<Uint32>(1000, UTIL_min<Uint32>(300000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10))));
		data = cfg.fetchKey("AutoHealerEnabled");
		if(!data.empty())
			m_autoHealer = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoHealerSpell");
		if(!data.empty())
			m_autoHealerSpell = data;
		data = cfg.fetchKey("AutoHealerHPPercent");
		if(!data.empty())
			m_autoHealerHPPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))));
		data = cfg.fetchKey("AutoHealerSelfUH");
		if(!data.empty())
			m_autoHealerSelfUH = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoHealerUHItemId");
		if(!data.empty())
			m_autoHealerUHItemId = SDL_static_cast(Uint16, UTIL_min<Uint32>(65535, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10))));
		data = cfg.fetchKey("AutoHealerSelfUHPercent");
		if(!data.empty())
			m_autoHealerSelfUHPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))));
		data = cfg.fetchKey("AutoHealerFriendUH");
		if(!data.empty())
			m_autoHealerFriendUH = (data == "yes" ? true : false);
		data = cfg.fetchKey("AutoHealerFriendNames");
		if(!data.empty())
			m_autoHealerFriendNames = data;
		data = cfg.fetchKey("AutoHealerFriendUHPercent");
		if(!data.empty())
			m_autoHealerFriendUHPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))));
		data = cfg.fetchKey("AutoHealerDelay");
		if(!data.empty())
			m_autoHealerDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10))));
		data = cfg.fetchKey("ComboLeaderEnabled");
		if(!data.empty())
			m_comboLeader = (data == "yes" ? true : false);
		data = cfg.fetchKey("ComboLeaderName");
		if(!data.empty())
			m_comboLeaderName = data;
		auto readComboLeaderValue = [](const std::string& value) -> Uint16
		{
			return SDL_static_cast(Uint16, UTIL_min<Uint32>(65535, SDL_static_cast(Uint32, SDL_strtoul(value.c_str(), NULL, 10))));
		};
		data = cfg.fetchKey("ComboLeaderSdEffect");
		if(!data.empty())
			m_comboLeaderMappingEffects[0] = readComboLeaderValue(data);
		data = cfg.fetchKey("ComboLeaderSdRune");
		if(!data.empty())
			m_comboLeaderMappingRunes[0] = readComboLeaderValue(data);
		data = cfg.fetchKey("ComboLeaderHmmEffect");
		if(!data.empty())
			m_comboLeaderMappingEffects[1] = readComboLeaderValue(data);
		data = cfg.fetchKey("ComboLeaderHmmRune");
		if(!data.empty())
			m_comboLeaderMappingRunes[1] = readComboLeaderValue(data);
		for(size_t index = 0; index < COMBO_LEADER_MAPPING_COUNT; ++index)
		{
			char comboKey[64];
			SDL_snprintf(comboKey, sizeof(comboKey), "ComboLeaderMap%uName", SDL_static_cast(Uint32, index));
			data = cfg.fetchKey(comboKey);
			if(!data.empty())
				m_comboLeaderMappingNames[index] = data;
			SDL_snprintf(comboKey, sizeof(comboKey), "ComboLeaderMap%uEffect", SDL_static_cast(Uint32, index));
			data = cfg.fetchKey(comboKey);
			if(!data.empty())
				m_comboLeaderMappingEffects[index] = readComboLeaderValue(data);
			SDL_snprintf(comboKey, sizeof(comboKey), "ComboLeaderMap%uRune", SDL_static_cast(Uint32, index));
			data = cfg.fetchKey(comboKey);
			if(!data.empty())
				m_comboLeaderMappingRunes[index] = readComboLeaderValue(data);
		}
		data = cfg.fetchKey("ComboLeaderDelay");
		if(!data.empty())
			m_comboLeaderDelay = UTIL_max<Uint32>(250, UTIL_min<Uint32>(10000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10))));
		data = cfg.fetchKey("MagicWallEnabled");
		if(!data.empty())
			m_magicWall = (data == "yes" ? true : false);
		data = cfg.fetchKey("MagicWallSpell");
		if(!data.empty())
			m_magicWallSpell = data;
		data = cfg.fetchKey("MagicWallDist");
		if(!data.empty())
			m_magicWallDist = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(7, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)))));
		data = cfg.fetchKey("MagicWallDelay");
		if(!data.empty())
			m_magicWallDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10))));
		data = cfg.fetchKey("CreatureInfo");
		m_showNames = (SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)) > 0 ? true : false);
		data = cfg.fetchKey("CreatureMarks");
		m_showMarks = (data == "yes" ? true : false);
		data = cfg.fetchKey("CreaturePvPFrames");
		m_showPvPFrames = (data == "yes" ? true : false);
		data = cfg.fetchKey("CreatureIcons");
		m_showIcons = (data == "yes" ? true : false);
		data = cfg.fetchKey("TextualEffects");
		m_showTextualEffects = (data == "yes" ? true : false);
		data = cfg.fetchKey("CooldownBar");
		m_showCooldown = (data == "yes" ? true : false);

		data = cfg.fetchKey("InfoMessages");
		m_showInfoMessages = (data == "yes" ? true : false);
		data = cfg.fetchKey("EventMessages");
		m_showEventMessages = (data == "yes" ? true : false);
		data = cfg.fetchKey("StatusMessages");
		m_showStatusMessages = (data == "yes" ? true : false);
		data = cfg.fetchKey("StatusMsgOfOthers");
		m_showStatusOthersMessages = (data == "yes" ? true : false);
		data = cfg.fetchKey("TimeStamps");
		m_showTimestamps = (data == "yes" ? true : false);
		data = cfg.fetchKey("Levels");
		m_showLevels = (data == "yes" ? true : false);
		data = cfg.fetchKey("PrivateMessages");
		m_showPrivateMessages = (data == "yes" ? true : false);

		data = cfg.fetchKey("AttackMode");
		m_attackMode = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10)) + 1;
		m_attackMode = (m_attackMode > ATTACKMODE_DEFENSE ? ATTACKMODE_BALANCED : m_attackMode);
		data = cfg.fetchKey("ChaseMode");
		m_chaseMode = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_chaseMode = (m_chaseMode > CHASEMODE_FOLLOW ? CHASEMODE_STAND : m_chaseMode);
		data = cfg.fetchKey("SecureMode");
		m_secureMode = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_secureMode = (m_secureMode > SECUREMODE_UNSECURE ? SECUREMODE_SECURE : m_secureMode);
		data = cfg.fetchKey("PvpMode");
		m_pvpMode = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_pvpMode = (m_pvpMode > PVPMODE_RED_FIST ? PVPMODE_DOVE : m_secureMode);

		data = cfg.fetchKey("LastMotD");
		m_motdNumber = SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10));
		data = cfg.fetchKey("AutomapZoom");
		g_game.minimapSetZoom(SDL_static_cast(Sint32, SDL_strtol(data.c_str(), NULL, 10)));
		data = cfg.fetchKey("ConsoleHeight");
		m_consoleHeight = SDL_static_cast(Sint32, SDL_strtol(data.c_str(), NULL, 10));

		data = cfg.fetchKey("ActivatedIgnoreList");
		m_activatedBlackList = (data == "yes" ? true : false);
		data = cfg.fetchKey("ActivatedWhiteList");
		m_activatedWhiteList = (data == "yes" ? true : false);
		data = cfg.fetchKey("IgnoreYelling");
		m_ignoreYellingMessages = (data == "yes" ? true : false);
		data = cfg.fetchKey("IgnorePrivateMessages");
		m_ignorePrivateMessages = (data == "yes" ? true : false);
		data = cfg.fetchKey("AllowVipMessages");
		m_allowVipMessages = (data == "yes" ? true : false);

		data = cfg.fetchKey("White");
		if(data.size() > 2)
		{
			data.pop_back();
			data.erase(data.begin());
			m_whiteList.clear();

			StringVector whiteListElements = UTIL_explodeString(data, ",");
			for(StringVector::iterator it = whiteListElements.begin(), end = whiteListElements.end(); it != end; ++it)
			{
				std::string& whiteListElement = (*it);
				if(whiteListElement.back() == '"')
					whiteListElement.pop_back();
				if(whiteListElement.front() == '"')
					whiteListElement.erase(whiteListElement.begin());

				m_whiteList[whiteListElement] = true;
			}
		}
		data = cfg.fetchKey("Ignore");
		if(data.size() > 2)
		{
			data.pop_back();
			data.erase(data.begin());
			m_blackList.clear();

			StringVector blackListElements = UTIL_explodeString(data, ",");
			for(StringVector::iterator it = blackListElements.begin(), end = blackListElements.end(); it != end; ++it)
			{
				std::string& blackListElement = (*it);
				if(blackListElement.back() == '"')
					blackListElement.pop_back();
				if(blackListElement.front() == '"')
					blackListElement.erase(blackListElement.begin());

				m_blackList[blackListElement] = true;
			}
		}
		//data = cfg.fetchKey("KnownTutorialHints");

		data = cfg.fetchKey("LevelBar");
		m_showLevelBar = (data == "yes" ? true : false);
		data = cfg.fetchKey("StaminaBar");
		m_showStaminaBar = (data == "yes" ? true : false);
		data = cfg.fetchKey("MagLevelBar");
		m_showMagLevelBar = (data == "yes" ? true : false);
		data = cfg.fetchKey("OfflineTrainingBar");
		m_showTrainingBar = (data == "yes" ? true : false);
		data = cfg.fetchKey("SkillBars");
		if(data.size() > 2)
		{
			data.pop_back();
			data.erase(data.begin());
			for(Sint32 i = Skills_Fist; i < Skills_LastSkill; ++i)
				m_showSkillsBar[i] = false;

			StringVector skillsData = UTIL_explodeString(data, ",");
			for(StringVector::iterator it = skillsData.begin(), end = skillsData.end(); it != end; ++it)
			{
				Uint32 skillId = SDL_static_cast(Uint32, SDL_strtoul((*it).c_str(), NULL, 10));
				if(skillId < Skills_LastSkill)
					m_showSkillsBar[skillId] = true;
			}
		}

		StringVector vectorData = cfg.fetchKeys("ContentWindow");
		for(StringVector::iterator it = vectorData.begin(), end = vectorData.end(); it != end; ++it)
		{
			data = (*it);
			if(data.size() > 2)
			{
				data.pop_back();
				data.erase(data.begin());

				StringVector windowData = UTIL_explodeString(data, ",");
				if(windowData.size() == 2)
					m_contentWindows[SDL_static_cast(Uint32, SDL_strtoul(windowData[0].c_str(), NULL, 10))] = SDL_static_cast(Sint32, SDL_strtol(windowData[1].c_str(), NULL, 10));
				else if(windowData.size() == 3)
				{
					Uint32 windowId = SDL_static_cast(Uint32, SDL_strtoul(windowData[0].c_str(), NULL, 10));
					m_contentWindows[windowId] = SDL_static_cast(Sint32, SDL_strtol(windowData[1].c_str(), NULL, 10));
					m_parentWindows[windowId] = SDL_static_cast(Sint32, SDL_strtol(windowData[2].c_str(), NULL, 10));
				}
			}
		}

		vectorData = cfg.fetchKeys("OpenDialogs");
		for(StringVector::iterator it = vectorData.begin(), end = vectorData.end(); it != end; ++it)
		{
			data = (*it);
			m_openDialogs.push_back(SDL_static_cast(Uint32, SDL_strtoul(data.c_str(), NULL, 10)));
		}

		vectorData = cfg.fetchKeys("Buddy");
		for(StringVector::iterator it = vectorData.begin(), end = vectorData.end(); it != end; ++it)
		{
			data = (*it);
			if(data.size() > 2)
			{
				data.pop_back();
				data.erase(data.begin());

				StringVector windowData = UTIL_explodeString(data, ",");
				if(windowData.size() == 4)
				{
					Uint32 playerGUID = SDL_static_cast(Uint32, SDL_strtoul(windowData[0].c_str(), NULL, 10));
					Uint32 iconId = SDL_static_cast(Uint32, SDL_strtoul(windowData[1].c_str(), NULL, 10));
					if(windowData[2].back() == '"')
						windowData[2].pop_back();
					if(windowData[2].front() == '"')
						windowData[2].erase(windowData[2].begin());

					setVipData(playerGUID, windowData[2], iconId, (windowData[3] == "yes" ? true : false));
				}
			}
		}

		data = cfg.fetchKey("BattleSortmethod");
		m_battleSortMethod = SDL_static_cast(SortMethods, SDL_strtoul(data.c_str(), NULL, 10));
		m_battleSortMethod = (m_battleSortMethod > Sort_Descending_Name ? Sort_Ascending_Time : m_battleSortMethod);
		data = cfg.fetchKey("BuddySortmethods");
		if(data.size() > 2)
		{
			data.pop_back();
			data.erase(data.begin());

			StringVector sortData = UTIL_explodeString(data, ",");
			if(sortData.size() == 3)
				m_buddySortmethod = SDL_static_cast(VipSortMethods, SDL_strtoul(sortData[0].c_str(), NULL, 10));
		}
		data = cfg.fetchKey("BuddyHideOffline");
		m_buddyHideOffline = (data == "yes" ? true : false);
		data = cfg.fetchKey("BuddyHideGroups");
		m_buddyHideGroups = (data == "yes" ? true : false);
		
		data = cfg.fetchKey("SortOrderBuy");
		m_buySortMethod = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_buySortMethod = (m_buySortMethod > Shop_Sort_Weight ? Shop_Sort_Name : m_buySortMethod);
		data = cfg.fetchKey("SortOrderSell");
		m_sellSortMethod = SDL_static_cast(Uint8, SDL_strtoul(data.c_str(), NULL, 10));
		m_sellSortMethod = (m_sellSortMethod > Shop_Sort_Weight ? Shop_Sort_Name : m_sellSortMethod);
		data = cfg.fetchKey("BuyWithBackpacks");
		m_buyWithBackpacks = (data == "yes" ? true : false);
		data = cfg.fetchKey("IgnoreCapacity");
		m_ignoreCapacity = (data == "yes" ? true : false);
		data = cfg.fetchKey("SellEquipped");
		m_ignoreEquiped = (data == "yes" ? false : true);
		
		data = cfg.fetchKey("LeftSidebars");
		m_leftPanel = SDL_static_cast(Sint32, SDL_strtol(data.c_str(), NULL, 10));
		m_leftPanel = (m_leftPanel == 0 ? GUI_PANEL_RANDOM : m_leftPanel + GUI_PANEL_EXTRA_LEFT_START - 1);
		data = cfg.fetchKey("RightSidebars");
		m_rightPanel = SDL_static_cast(Sint32, SDL_strtol(data.c_str(), NULL, 10));
		m_rightPanel = (m_rightPanel == 0 ? GUI_PANEL_MAIN : m_rightPanel + GUI_PANEL_EXTRA_RIGHT_START - 1);
	}
	#if CLIENT_OVVERIDE_VERSION > 0
	g_clientVersion = CLIENT_OVERRIDE_PROTOCOL_VERSION;
	g_game.clientChangeVersion(CLIENT_OVERRIDE_PROTOCOL_VERSION, CLIENT_OVERRIDE_FILE_VERSION);
	#else
	g_game.clientChangeVersion(g_clientVersion, g_clientVersion);
	#endif
}

void Engine::saveCFG()
{
	Config cfg;
	SDL_snprintf(g_buffer, sizeof(g_buffer), "%sconfig.cfg", g_prefPath.c_str());
	if(cfg.openToSave(g_buffer))
	{
		#if CLIENT_OVVERIDE_VERSION > 0
		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", CLIENT_OVVERIDE_VERSION);
		#else
		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", g_clientVersion);
		#endif
		cfg.insertKey("Version", std::string(g_buffer, SDL_static_cast(size_t, len)));

		#if !(CLIENT_OVVERIDE_VERSION > 0)
		cfg.insertKey("Host", m_clientHost);
		cfg.insertKey("Port", m_clientPort);
		cfg.insertKey("Proxy", m_clientProxy);
		cfg.insertKey("ProxyAuth", m_clientProxyAuth);
		#endif

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_engine));
		cfg.insertKey("Engine", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", "yes");
		cfg.insertKey("LightEffects", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_lightAmbient));
		cfg.insertKey("BasicBrightness", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_levelSeparator));
		cfg.insertKey("LevelSeparator", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_lightMode));
		cfg.insertKey("LightMode", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "(%d,%d,%d,%d,%s)", m_windowX, m_windowY, m_windowCachedW, m_windowCachedH, (m_maximized ? "yes" : "no"));
		cfg.insertKey("WindowedMode", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_fullscreen ? "yes" : "no"));
		cfg.insertKey("Fullscreen", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "(%d,%d,%d,%d)", m_fullScreenWidth, m_fullScreenHeight, m_fullScreenBits, m_fullScreenHZ);
		cfg.insertKey("FullscreenMode", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", g_fpsmanager.rate);
		cfg.insertKey("ConfineFramerate", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_unlimitedFPS ? "yes" : "no"));
		cfg.insertKey("NolimitFramerate", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_vsync ? "yes" : "no"));
		cfg.insertKey("VerticalSync", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_sharpening ? "yes" : "no"));
		cfg.insertKey("Sharpening", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_antialiasing == CLIENT_ANTIALIASING_NORMAL ? "yes" : m_antialiasing == CLIENT_ANTIALIASING_INTEGER ? "integer" : "no"));
		cfg.insertKey("Antialiasing", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_classicControl ? "yes" : "no"));
		cfg.insertKey("ClassicControl", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoChaseOff ? "yes" : "no"));
		cfg.insertKey("AutoChaseOff", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_devMode ? "yes" : "no"));
		cfg.insertKey("DevMode", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showDebugCoordinates ? "yes" : "no"));
		cfg.insertKey("ShowDebugCoordinates", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_manaTrainer ? "yes" : "no"));
		cfg.insertKey("ManaTrainerEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("ManaTrainerSpell", m_manaTrainerSpell);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_manaTrainerManaPercent));
		cfg.insertKey("ManaTrainerManaPercent", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_manaTrainerDelay);
		cfg.insertKey("ManaTrainerDelay", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoFisher ? "yes" : "no"));
		cfg.insertKey("AutoFisherEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_autoFisherDelayMin);
		cfg.insertKey("AutoFisherDelay", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("AutoFisherDelayMin", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_autoFisherDelayMax);
		cfg.insertKey("AutoFisherDelayMax", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoLogout ? "yes" : "no"));
		cfg.insertKey("AutoLogoutEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoLogoutPK ? "yes" : "no"));
		cfg.insertKey("AutoLogoutPK", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoLogoutPlayer ? "yes" : "no"));
		cfg.insertKey("AutoLogoutPlayer", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_autoLogoutTimerMin);
		cfg.insertKey("AutoLogoutTimerMin", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_gmPollInterval / 1000);
		cfg.insertKey("AutoLogoutPollSec", std::string(g_buffer, SDL_static_cast(size_t, len)));
		{
			std::string gmStr;
			for(size_t i = 0; i < m_autoLogoutGMs.size(); ++i) { if(i > 0) gmStr += ';'; gmStr += m_autoLogoutGMs[i]; }
			cfg.insertKey("AutoLogoutGMs", gmStr);
		}
		{
			std::string playerStr;
			for(size_t i = 0; i < m_autoLogoutPlayers.size(); ++i) { if(i > 0) playerStr += ';'; playerStr += m_autoLogoutPlayers[i]; }
			cfg.insertKey("AutoLogoutPlayers", playerStr);
		}
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_alertCaptureEnabled ? "yes" : "no"));
		cfg.insertKey("AlertCaptureEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("DiscordWebhook", m_discordWebhook);
		cfg.insertKey("AutoLogoutOnPM", m_autoLogoutOnPM ? "yes" : "no");
		cfg.insertKey("AutoLogoutOnBroadcast", m_autoLogoutOnBroadcast ? "yes" : "no");
		cfg.insertKey("PlayerAlarmEnabled", m_playerAlarmEnabled ? "yes" : "no");
		cfg.insertKey("FullBrightness", m_fullBrightness ? "yes" : "no");
		cfg.insertKey("HideWASD", m_hideWASD ? "yes" : "no");
		cfg.insertKey("HideHPManaPercent", m_hideHPManaPercent ? "yes" : "no");
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoRuner ? "yes" : "no"));
		cfg.insertKey("AutoRunerEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("AutoRunerSpell", m_autoRunerSpell);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_autoRunerManaPercent));
		cfg.insertKey("AutoRunerManaPercent", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_autoRunerDelay);
		cfg.insertKey("AutoRunerDelay", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoEater ? "yes" : "no"));
		cfg.insertKey("AutoEaterEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_autoEaterFoodId));
		cfg.insertKey("AutoEaterFoodId", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_autoEaterDelay);
		cfg.insertKey("AutoEaterDelay", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoHealer ? "yes" : "no"));
		cfg.insertKey("AutoHealerEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("AutoHealerSpell", m_autoHealerSpell);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_autoHealerHPPercent));
		cfg.insertKey("AutoHealerHPPercent", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoHealerSelfUH ? "yes" : "no"));
		cfg.insertKey("AutoHealerSelfUH", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_autoHealerUHItemId));
		cfg.insertKey("AutoHealerUHItemId", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_autoHealerSelfUHPercent));
		cfg.insertKey("AutoHealerSelfUHPercent", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_autoHealerFriendUH ? "yes" : "no"));
		cfg.insertKey("AutoHealerFriendUH", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("AutoHealerFriendNames", m_autoHealerFriendNames);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_autoHealerFriendUHPercent));
		cfg.insertKey("AutoHealerFriendUHPercent", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_autoHealerDelay);
		cfg.insertKey("AutoHealerDelay", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_comboLeader ? "yes" : "no"));
		cfg.insertKey("ComboLeaderEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("ComboLeaderName", m_comboLeaderName);
		for(size_t index = 0; index < COMBO_LEADER_MAPPING_COUNT; ++index)
		{
			char comboKey[64];
			SDL_snprintf(comboKey, sizeof(comboKey), "ComboLeaderMap%uName", SDL_static_cast(Uint32, index));
			cfg.insertKey(comboKey, m_comboLeaderMappingNames[index]);
			SDL_snprintf(comboKey, sizeof(comboKey), "ComboLeaderMap%uEffect", SDL_static_cast(Uint32, index));
			len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_comboLeaderMappingEffects[index]));
			cfg.insertKey(comboKey, std::string(g_buffer, SDL_static_cast(size_t, len)));
			SDL_snprintf(comboKey, sizeof(comboKey), "ComboLeaderMap%uRune", SDL_static_cast(Uint32, index));
			len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_comboLeaderMappingRunes[index]));
			cfg.insertKey(comboKey, std::string(g_buffer, SDL_static_cast(size_t, len)));
		}
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_comboLeaderMappingEffects[0]));
		cfg.insertKey("ComboLeaderSdEffect", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_comboLeaderMappingRunes[0]));
		cfg.insertKey("ComboLeaderSdRune", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_comboLeaderMappingEffects[1]));
		cfg.insertKey("ComboLeaderHmmEffect", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_comboLeaderMappingRunes[1]));
		cfg.insertKey("ComboLeaderHmmRune", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_comboLeaderDelay);
		cfg.insertKey("ComboLeaderDelay", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_magicWall ? "yes" : "no"));
		cfg.insertKey("MagicWallEnabled", std::string(g_buffer, SDL_static_cast(size_t, len)));
		cfg.insertKey("MagicWallSpell", m_magicWallSpell);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_magicWallDist));
		cfg.insertKey("MagicWallDist", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_magicWallDelay);
		cfg.insertKey("MagicWallDelay", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", (m_showNames ? 2 : 0));
		cfg.insertKey("CreatureInfo", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showMarks ? "yes" : "no"));
		cfg.insertKey("CreatureMarks", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showPvPFrames ? "yes" : "no"));
		cfg.insertKey("CreaturePvPFrames", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showIcons ? "yes" : "no"));
		cfg.insertKey("CreatureIcons", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showTextualEffects ? "yes" : "no"));
		cfg.insertKey("TextualEffects", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showCooldown ? "yes" : "no"));
		cfg.insertKey("CooldownBar", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showInfoMessages ? "yes" : "no"));
		cfg.insertKey("InfoMessages", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showEventMessages ? "yes" : "no"));
		cfg.insertKey("EventMessages", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showStatusMessages ? "yes" : "no"));
		cfg.insertKey("StatusMessages", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showStatusOthersMessages ? "yes" : "no"));
		cfg.insertKey("StatusMsgOfOthers", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showTimestamps ? "yes" : "no"));
		cfg.insertKey("TimeStamps", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showLevels ? "yes" : "no"));
		cfg.insertKey("Levels", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showPrivateMessages ? "yes" : "no"));
		cfg.insertKey("PrivateMessages", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_attackMode - 1));
		cfg.insertKey("AttackMode", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_chaseMode));
		cfg.insertKey("ChaseMode", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_secureMode));
		cfg.insertKey("SecureMode", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_pvpMode));
		cfg.insertKey("PvpMode", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", m_motdNumber);
		cfg.insertKey("LastMotD", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", g_game.minimapGetZoom());
		cfg.insertKey("AutomapZoom", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%d", m_consoleHeight);
		cfg.insertKey("ConsoleHeight", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_activatedBlackList ? "yes" : "no"));
		cfg.insertKey("ActivatedIgnoreList", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_activatedWhiteList ? "yes" : "no"));
		cfg.insertKey("ActivatedWhiteList", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_ignoreYellingMessages ? "yes" : "no"));
		cfg.insertKey("IgnoreYelling", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_ignorePrivateMessages ? "yes" : "no"));
		cfg.insertKey("IgnorePrivateMessages", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_allowVipMessages ? "yes" : "no"));
		cfg.insertKey("AllowVipMessages", std::string(g_buffer, SDL_static_cast(size_t, len)));

		bool itFirst = true;
		std::string listString("{");
		for(std::unordered_map<std::string, bool>::iterator it = m_whiteList.begin(), end = m_whiteList.end(); it != end; ++it)
		{
			if(it->second)
			{
				if(itFirst)
					itFirst = false;
				else
					listString.append(1, ',');

				listString.append(1, '"');
				listString.append(it->first);
				listString.append(1, '"');
			}
		}
		listString.append(1, '}');
		cfg.insertKey("White", listString);
		itFirst = true;
		listString.assign("{");
		for(std::unordered_map<std::string, bool>::iterator it = m_blackList.begin(), end = m_blackList.end(); it != end; ++it)
		{
			if(it->second)
			{
				if(itFirst)
					itFirst = false;
				else
					listString.append(1, ',');

				listString.append(1, '"');
				listString.append(it->first);
				listString.append(1, '"');
			}
		}
		listString.append(1, '}');
		cfg.insertKey("Ignore", listString);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "{}");
		cfg.insertKey("KnownTutorialHints", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showLevelBar ? "yes" : "no"));
		cfg.insertKey("LevelBar", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showStaminaBar ? "yes" : "no"));
		cfg.insertKey("StaminaBar", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showMagLevelBar ? "yes" : "no"));
		cfg.insertKey("MagLevelBar", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_showTrainingBar ? "yes" : "no"));
		cfg.insertKey("OfflineTrainingBar", std::string(g_buffer, SDL_static_cast(size_t, len)));

		itFirst = true;
		listString.assign("{");
		for(Sint32 i = Skills_Fist; i < Skills_LastSkill; ++i)
		{
			if(m_showSkillsBar[i])
			{
				if(itFirst)
					itFirst = false;
				else
					listString.append(1, ',');

				listString.append(std::to_string(i));
			}
		}
		listString.append(1, '}');
		cfg.insertKey("SkillBars", listString);
		for(std::map<Uint32, Sint32>::iterator it = m_contentWindows.begin(), end = m_contentWindows.end(); it != end; ++it)
		{
			std::map<Uint32, Sint32>::iterator pit = m_parentWindows.find(it->first);
			if(pit != m_parentWindows.end())
				len = SDL_snprintf(g_buffer, sizeof(g_buffer), "(%u,%d,%d)", it->first, it->second, pit->second);
			else
				len = SDL_snprintf(g_buffer, sizeof(g_buffer), "(%u,%d)", it->first, it->second);

			cfg.insertKey("ContentWindow", std::string(g_buffer, SDL_static_cast(size_t, len)));
		}
		for(std::vector<Uint32>::iterator it = m_openDialogs.begin(), end = m_openDialogs.end(); it != end; ++it)
		{
			len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", (*it));
			cfg.insertKey("OpenDialogs", std::string(g_buffer, SDL_static_cast(size_t, len)));
		}
		for(std::map<Uint32, VipData>::iterator it = m_vipData.begin(), end = m_vipData.end(); it != end; ++it)
		{
			VipData& vip = it->second;
			len = SDL_snprintf(g_buffer, sizeof(g_buffer), "(%u,%u,\"%s\",%s)", it->first, vip.iconId, vip.description.c_str(), (vip.notifyLogin ? "yes" : "no"));
			cfg.insertKey("Buddy", std::string(g_buffer, SDL_static_cast(size_t, len)));
		}

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_battleSortMethod));
		cfg.insertKey("BattleSortmethod", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_buddySortmethod == 0 ? "(0,1,2)" : m_buddySortmethod == 1 ? "(1,2,0)" : "(2,0,1)"));
		cfg.insertKey("BuddySortmethods", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_buddyHideOffline ? "yes" : "no"));
		cfg.insertKey("BuddyHideOffline", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%s", (m_buddyHideGroups ? "yes" : "no"));
		cfg.insertKey("BuddyHideGroups", std::string(g_buffer, SDL_static_cast(size_t, len)));
		
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_buySortMethod));
		cfg.insertKey("SortOrderBuy", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, m_sellSortMethod));
		cfg.insertKey("SortOrderSell", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), (m_buyWithBackpacks ? "yes" : "no"));
		cfg.insertKey("BuyWithBackpacks", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), (m_ignoreCapacity ? "yes" : "no"));
		cfg.insertKey("IgnoreCapacity", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), (m_ignoreEquiped ? "no" : "yes"));
		cfg.insertKey("SellEquipped", std::string(g_buffer, SDL_static_cast(size_t, len)));

		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%d", (m_leftPanel == GUI_PANEL_RANDOM ? 0 : m_leftPanel - GUI_PANEL_EXTRA_LEFT_START + 1));
		cfg.insertKey("LeftSidebars", std::string(g_buffer, SDL_static_cast(size_t, len)));
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%d", (m_rightPanel == GUI_PANEL_MAIN ? 0 : m_rightPanel - GUI_PANEL_EXTRA_RIGHT_START + 1));
		cfg.insertKey("RightSidebars", std::string(g_buffer, SDL_static_cast(size_t, len)));

		cfg.insertNewLine();
		for(Sint32 i = 0; i < HOTKEY_CONFIG_COUNT; ++i)
		{
			const char* keyName = s_hotkeyConfigNames[i];
			HotkeyUsage* hk = getHotkey(s_hotkeyConfigKeys[i], s_hotkeyConfigMods[i]);
			if(!hk || hk->action.type == CLIENT_HOTKEY_ACTION_NONE)
			{
				cfg.insertKey(keyName, "None");
				continue;
			}
			if(hk->action.type == CLIENT_HOTKEY_ACTION_TEXT)
			{
				std::string val = "T:";
				val += (hk->action.text.sendAutomatically ? "1" : "0");
				val += ":";
				if(hk->action.text.text)
					val += *hk->action.text.text;
				cfg.insertKey(keyName, val);
			}
			else
			{
				len = SDL_snprintf(g_buffer, sizeof(g_buffer), "I:%d:%d:%d",
					SDL_static_cast(int, hk->action.item.type),
					SDL_static_cast(int, hk->action.item.itemId),
					SDL_static_cast(int, hk->action.item.itemSubtype));
				cfg.insertKey(keyName, std::string(g_buffer, SDL_static_cast(size_t, len)));
			}
		}
	}
}

void Engine::attachFullScreenInfo()
{
	SDL_DisplayMode displayMode;
	Sint32 displayIndex = SDL_GetWindowDisplayIndex(m_window);
	Sint32 displayModes = SDL_GetNumDisplayModes(displayIndex);
	for(Sint32 i = 0; i < displayModes; ++i)
	{
		Sint32 reportBits;
		SDL_GetDisplayMode(displayIndex, i, &displayMode);
		if(m_fullScreenWidth == displayMode.w && m_fullScreenHeight == displayMode.h && m_fullScreenHZ == displayMode.refresh_rate)
		{
			if(displayMode.format == SDL_PIXELFORMAT_UNKNOWN && m_fullScreenBits == 32)
			{
				SDL_SetWindowDisplayMode(m_window, &displayMode);
				return;
			}
			else
			{
				reportBits = SDL_BITSPERPIXEL(displayMode.format);
				if(reportBits == 24)
					reportBits = SDL_BYTESPERPIXEL(displayMode.format) * 8;
				if(m_fullScreenBits == reportBits)
				{
					SDL_SetWindowDisplayMode(m_window, &displayMode);
					return;
				}
			}
		}
	}
}

void Engine::run()
{
	SDL_DisplayMode displayMode;
	if(SDL_GetDesktopDisplayMode(0, &displayMode) == 0)
	{
		m_fullScreenWidth = displayMode.w;
		m_fullScreenHeight = displayMode.h;
		m_fullScreenHZ = displayMode.refresh_rate;
		if(displayMode.format == SDL_PIXELFORMAT_UNKNOWN)
			m_fullScreenBits = 32;
		else
		{
			m_fullScreenBits = SDL_BITSPERPIXEL(displayMode.format);
			if(m_fullScreenBits == 24)
				m_fullScreenBits = SDL_BYTESPERPIXEL(displayMode.format) * 8;
		}
	}
	loadCFG();

	Uint32 windowflags = (SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	if(m_fullscreen)
	{
		windowflags |= SDL_WINDOW_FULLSCREEN;
		m_windowW = m_fullScreenWidth;
		m_windowH = m_fullScreenHeight;
	}
	else if(m_maximized)
	{
		windowflags |= SDL_WINDOW_MAXIMIZED;
		m_windowW = m_windowCachedW;
		m_windowH = m_windowCachedH;
	}
	
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "101");
	m_window = SDL_CreateWindow(PRODUCT_NAME, m_windowX, m_windowY, m_windowW, m_windowH, windowflags);
	if(!m_window)
	{
		SDL_snprintf(g_buffer, sizeof(g_buffer), "Couldn't create SDL Window: %s", SDL_GetError());
		UTIL_MessageBox(true, g_buffer);
		g_running = false;
		return;
	}

	if(m_fullscreen)
	{
		attachFullScreenInfo();
		SDL_GetWindowDisplayMode(m_window, &displayMode);
		m_windowW = displayMode.w;
		m_windowH = displayMode.h;
	}
	else
	{
		SDL_GetWindowSize(m_window, &m_windowW, &m_windowH);
		SDL_SetWindowMinimumSize(m_window, GAME_MINIMUM_WIDTH, GAME_MINIMUM_HEIGHT);
	}
	m_windowId = SDL_GetWindowID(m_window);
	m_controlFPS = !m_unlimitedFPS;
	
	//createwindow don't generate resize event so send one ourselves
	UTIL_ResizeEvent(m_windowId, m_windowW, m_windowH);

	initFont(CLIENT_FONT_NONOUTLINED, 256, 128, 32, 7, 8, 16, 14);
	initFont(CLIENT_FONT_OUTLINED, 512, 128, 32, 7, 16, 16, 16);
	initFont(CLIENT_FONT_SMALL, 256, 64, 32, 7, 8, 8, 8);

	UTIL_createMainWindow();
	resetToDefaultHotkeys(true);
}

void Engine::terminate()
{
	if(m_window && !m_fullscreen)
	{
		//Some disgusting "hack" to save original window parameters
		//TOOD: do it the proper way
		if(m_maximized)
			SDL_RestoreWindow(m_window);

		SDL_GetWindowPosition(m_window, &m_windowX, &m_windowY);
		SDL_GetWindowSize(m_window, &m_windowW, &m_windowH);
		m_windowCachedW = m_windowW;
		m_windowCachedH = m_windowH;
	}
	clearWindows();
	clearPanels();
	checkReleaseQueue();
	m_surface.reset();

	if(m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = NULL;
	}
	saveCFG();
}

void Engine::parseCommands(int argc, char* argv[])
{
	//Does argv[0] on all platforms is the path to our application?
	for(int i = 1; i < argc; ++i)
	{
		if(SDL_strcasecmp(argv[i], "-force-d3d11") == 0)
			m_engine = CLIENT_ENGINE_DIRECT3D11;
		else if(SDL_strcasecmp(argv[i], "-force-d3d9") == 0)
			m_engine = CLIENT_ENGINE_DIRECT3D;
		else if(SDL_strcasecmp(argv[i], "-force-d3d7") == 0)
			m_engine = CLIENT_ENGINE_DIRECT3D7;
		else if(SDL_strcasecmp(argv[i], "-force-vulkan") == 0)
			m_engine = CLIENT_ENGINE_VULKAN;
		else if(SDL_strcasecmp(argv[i], "-force-opengl-core") == 0)
			m_engine = CLIENT_ENGINE_OPENGLCORE;
		else if(SDL_strcasecmp(argv[i], "-force-opengl") == 0 || SDL_strcasecmp(argv[i], "-force-opengl-legacy") == 0)
			m_engine = CLIENT_ENGINE_OPENGL;
		else if(SDL_strcasecmp(argv[i], "-force-opengles") == 0)
			m_engine = CLIENT_ENGINE_OPENGLES;
		else if(SDL_strcasecmp(argv[i], "-force-opengles2") == 0)
			m_engine = CLIENT_ENGINE_OPENGLES2;
		else if(SDL_strcasecmp(argv[i], "-force-software") == 0)
			m_engine = CLIENT_ENGINE_SOFTWARE;
	}
}

bool Engine::RecreateWindow(bool vulkan, bool opengl)
{
	if(m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = NULL;
	}

	Uint32 windowflags = (SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	if(m_fullscreen)
	{
		windowflags |= SDL_WINDOW_FULLSCREEN;
		m_windowW = m_fullScreenWidth;
		m_windowH = m_fullScreenHeight;
	}
	else if(m_maximized)
	{
		windowflags |= SDL_WINDOW_MAXIMIZED;
		m_windowW = m_windowCachedW;
		m_windowH = m_windowCachedH;
	}

	if(vulkan)
		windowflags |= SDL_WINDOW_VULKAN;
	else if(opengl)
		windowflags |= SDL_WINDOW_OPENGL;

	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "101");
	m_window = SDL_CreateWindow(PRODUCT_NAME, m_windowX, m_windowY, m_windowW, m_windowH, windowflags);
	if(!m_window)
	{
		if(!vulkan && !opengl)
		{
			SDL_snprintf(g_buffer, sizeof(g_buffer), "Couldn't recreate SDL Window: %s", SDL_GetError());
			UTIL_MessageBox(true, g_buffer);
			exit(-1);
		}
		return false;
	}

	if(m_fullscreen)
	{
		attachFullScreenInfo();

		SDL_DisplayMode displayMode;
		SDL_GetWindowDisplayMode(m_window, &displayMode);
		m_windowW = displayMode.w;
		m_windowH = displayMode.h;
	}
	else
	{
		SDL_GetWindowSize(m_window, &m_windowW, &m_windowH);
		SDL_SetWindowMinimumSize(m_window, GAME_MINIMUM_WIDTH, GAME_MINIMUM_HEIGHT);
	}
	m_windowId = SDL_GetWindowID(m_window);

	//createwindow don't generate resize event so send one ourselves
	UTIL_ResizeEvent(m_windowId, m_windowW, m_windowH);
	return true;
}

bool Engine::init()
{
	m_surface.reset();
	if(m_engine == CLIENT_ENGINE_SOFTWARE)
		m_surface = std::move(std::make_unique<SurfaceSoftware>());
	#if defined(SDL_VIDEO_VULKAN)
	else if(m_engine == CLIENT_ENGINE_VULKAN)
		m_surface = std::move(std::make_unique<SurfaceVulkan>());
	#endif
	#if defined(SDL_VIDEO_RENDER_OGL)
	else if(m_engine == CLIENT_ENGINE_OPENGLCORE)
		m_surface = std::move(std::make_unique<SurfaceOpenglCore>());
	else if(m_engine == CLIENT_ENGINE_OPENGL)
		m_surface = std::move(std::make_unique<SurfaceOpengl>());
	#endif
	#if defined(SDL_VIDEO_RENDER_OGL_ES)
	else if(m_engine == CLIENT_ENGINE_OPENGLES)
		m_surface = std::move(std::make_unique<SurfaceOpenglES>());
	#endif
	#if defined(SDL_VIDEO_RENDER_OGL_ES2)
	else if(m_engine == CLIENT_ENGINE_OPENGLES2)
		m_surface = std::move(std::make_unique<SurfaceOpenglES2>());
	#endif
	#if defined(SDL_VIDEO_RENDER_D3D)
	else if(m_engine == CLIENT_ENGINE_DIRECT3D)
		m_surface = std::move(std::make_unique<SurfaceDirect3D9>());
	#endif
	#if defined(SDL_VIDEO_RENDER_D3D11)
	else if(m_engine == CLIENT_ENGINE_DIRECT3D11)
		m_surface = std::move(std::make_unique<SurfaceDirect3D11>());
	#endif
	#if defined(SDL_VIDEO_RENDER_DDRAW)
	else if(m_engine == CLIENT_ENGINE_DIRECT3D7)
		m_surface = std::move(std::make_unique<SurfaceDirectDraw>());
	#endif
	else
	{
		m_engine = m_engines.back();
		return false;
	}
	
	if(!m_surface->isSupported())
	{
		for(std::vector<Uint8>::iterator it = m_engines.begin(), end = m_engines.end(); it != end; ++it)
		{
			if((*it) == m_engine)
			{
				m_engines.erase(it);
				break;
			}
		}
		if(m_engines.empty())
		{
			UTIL_MessageBox(true, "The engine couldn't find any sufficient graphic engine.");
			g_running = false;
			return false;
		}
		m_engine = m_engines.back();
		return false;
	}
	m_surface->doResize(m_windowW, m_windowH);
	m_surface->init();
	m_surface->spriteManagerReset();
	return true;
}

void Engine::initFont(Uint8 font, Sint32 width, Sint32 height, Sint16 hchars, Sint16 vchars, Sint16 maxchw, Sint16 maxchh, Sint16 spaceh)
{
	Uint16 picture = 0;
	switch(font)
	{
		case CLIENT_FONT_NONOUTLINED:
			picture = GUI_FONT_NONOUTLINED_IMAGE;
			break;
		case CLIENT_FONT_OUTLINED:
			picture = GUI_FONT_OUTLINED_IMAGE;
			break;
		case CLIENT_FONT_SMALL:
			picture = GUI_FONT_SMALL_IMAGE;
			break;
		default: break;
	}
	if(!picture)
		return;

	Sint32 w, h;
	unsigned char* pixels = LoadPicture(picture, true, w, h);
	if(!pixels || width != w || height != h)
	{
		SDL_snprintf(g_buffer, sizeof(g_buffer), "Cannot read file '%s'.\n\nPlease re-install the program.", g_picPath.c_str());
		UTIL_MessageBox(true, g_buffer);
		exit(-1);
		return;
	}

	Uint32 protectionSize = (w * h * 4) - 4;
	m_charPicture[font] = picture;
	for(Sint32 i = 1; i < 32; ++i)
	{
		m_charx[font][i] = 0; m_chary[font][i] = 0;
		m_charw[font][i] = 0; m_charh[font][i] = 0;
	}

	for(Sint16 j = 0; j < vchars; ++j)
	{
		for(Sint16 k = 0; k < hchars; ++k)
		{
			Sint32 ch = 32 + (j * hchars) + k;
			if(ch > 255)
				continue;

			m_charx[font][ch] = k * maxchw;
			m_chary[font][ch] = j * maxchh;

			Sint16 chWidth = 0, chHeight = 0;
			for(Sint16 xPos = m_charx[font][ch]; xPos < m_charx[font][ch] + maxchw; ++xPos)
			{
				for(Sint16 yPos = m_chary[font][ch]; yPos < m_chary[font][ch] + maxchh; ++yPos)
				{
					Uint32 offset = (yPos * width + xPos) * 4;
					if(offset <= protectionSize && pixels[offset + 3] == 0xFF)
					{
						if(chWidth < xPos - m_charx[font][ch])
							chWidth = xPos - m_charx[font][ch];
						if(chHeight < yPos - m_chary[font][ch])
							chHeight = yPos - m_chary[font][ch];
					}
				}
			}

			m_charw[font][ch] = chWidth + 1;
			m_charh[font][ch] = chHeight + 1;
		}
	}

	SDL_free(pixels);
	m_charw[font][0] = maxchw;
	m_charh[font][0] = spaceh;
	switch(font)
	{
		case CLIENT_FONT_NONOUTLINED:
			m_charx[font][0] = 1; m_chary[font][0] = 0;
			m_charw[font][32] = m_charw[font][160] = 2;
			break;

		case CLIENT_FONT_OUTLINED:
			m_charx[font][0] = -1; m_chary[font][0] = 1;
			m_charw[font][32] = m_charw[font][160] = 4;
			break;
			
		case CLIENT_FONT_SMALL:
			m_charx[font][0] = m_chary[font][0] = 0;
			m_charw[font][32] = m_charw[font][160] = 2;
			break;

		default: break;
	}
}

Uint32 Engine::calculateFontWidth(Uint8 fontId, const std::string& text, size_t pos, size_t len)
{
	len += pos;
	if(len > text.length())
		len = text.length();

	Sint32 xSpace = m_charx[fontId][0];
	Sint32 ySpace = m_chary[fontId][0];
	Uint32 calculatedWidth = 0;
	Uint8 character;
	for(size_t i = pos; i < len; ++i)
	{
		character = SDL_static_cast(Uint8, text[i]);
		switch(character)
		{
			case '\n':
			case '\r'://return here?
				return calculatedWidth + ySpace;
			case 0x0E://Special case - change rendering color
			{
				if(i + 4 < len)//First check if we have the color bytes
					i += 3;
				else
					i = len;
			}
			break;
			case 0x0F://Special case - change back standard color
				break;
			default:
				calculatedWidth += m_charw[fontId][character] + xSpace;
				break;
		}
	}
	return calculatedWidth + ySpace;
}

Uint32 Engine::calculateFontWidth(Uint8 fontId, const std::string& text)
{
	Sint32 xSpace = m_charx[fontId][0];
	Sint32 ySpace = m_chary[fontId][0];
	Uint32 calculatedWidth = 0;
	Uint8 character;
	for(size_t i = 0, len = text.length(); i < len; ++i)
	{
		character = SDL_static_cast(Uint8, text[i]);
		switch(character)
		{
			case '\n':
			case '\r':
				return calculatedWidth + ySpace;
			case 0x0E://Special case - change rendering color
			{
				if(i + 4 < len)//First check if we have the color bytes
					i += 3;
				else
					i = len;
			}
			break;
			case 0x0F://Special case - change back standard color
				break;
			default:
				calculatedWidth += m_charw[fontId][character] + xSpace;
				break;
		}
	}
	return calculatedWidth + ySpace;
}

void Engine::exitGame()
{
	if(m_ingame)
		UTIL_exitWarning();
	else
	{
		g_inited = false;
		g_running = false;
	}
}

void Engine::checkReleaseQueue()
{
	for(std::vector<GUI_Window*>::iterator it = m_toReleaseWindows.begin(), end = m_toReleaseWindows.end(); it != end; ++it)
		delete (*it);

	m_toReleaseWindows.clear();
	for(std::vector<GUI_Panel*>::iterator it = m_toReleasePanels.begin(), end = m_toReleasePanels.end(); it != end; ++it)
		delete (*it);

	m_toReleasePanels.clear();
}

void Engine::updateThink()
{
	//Event that get called by every 5 seconds
	checkReleaseQueue();
	g_game.updatePlayerExperienceTable();
	g_game.sendPing();
}

void Engine::clearWindows()
{
	if(m_actWindow)
	{
		m_toReleaseWindows.push_back(m_actWindow);
		m_actWindow = NULL;
	}
	for(std::vector<GUI_Window*>::iterator it = m_windows.begin(), end = m_windows.end(); it != end; ++it)
		m_toReleaseWindows.push_back((*it));

	m_windows.clear();
}

void Engine::addWindow(GUI_Window* pWindow, bool topMost)
{
	pWindow->onReshape(m_windowW, m_windowH);
	if(topMost)
	{
		if(m_actWindow)
			m_windows.insert(m_windows.begin(), m_actWindow);
		m_actWindow = pWindow;
	}
	else if(!m_actWindow)
		m_actWindow = pWindow;
	else
		m_windows.push_back(pWindow);
}

void Engine::removeWindow(GUI_Window* pWindow)
{
	m_toReleaseWindows.push_back(pWindow);
	if(pWindow == m_actWindow)
	{
		m_actWindow = NULL;
		if(!m_windows.empty())
		{
			m_actWindow = m_windows.front();
			m_windows.erase(m_windows.begin());
		}
		return;
	}

	for(std::vector<GUI_Window*>::iterator it = m_windows.begin(), end = m_windows.end(); it != end; ++it)
	{
		if((*it) == pWindow)
		{
			m_windows.erase(it);
			return;
		}
	}
}

GUI_Window* Engine::getWindow(Uint32 internalID)
{
	if(m_actWindow && m_actWindow->getInternalID() == internalID)
		return m_actWindow;
	for(std::vector<GUI_Window*>::iterator it = m_windows.begin(), end = m_windows.end(); it != end; ++it)
	{
		if((*it)->getInternalID() == internalID)
			return (*it);
	}
	return NULL;
}

std::string Engine::formatRect(const iRect& rect)
{
	std::ostringstream out;
	out << "x=" << rect.x1 << " y=" << rect.y1 << " w=" << rect.x2 << " h=" << rect.y2;
	return out.str();
}

std::string Engine::getElementClassName(GUI_Element* element)
{
	if(!element)
		return "NULL";

	std::string name = typeid(*element).name();
	const std::string classPrefix = "class ";
	const std::string structPrefix = "struct ";
	if(name.compare(0, classPrefix.size(), classPrefix) == 0)
		name.erase(0, classPrefix.size());
	else if(name.compare(0, structPrefix.size(), structPrefix) == 0)
		name.erase(0, structPrefix.size());
	return name;
}

std::string Engine::describeElement(const char* area, GUI_Element* element)
{
	if(!element)
		return std::string(area).append("\nElement=NULL");

	iRect& rect = element->getRect();
	m_uiInspectorRect = rect;

	std::ostringstream out;
	out << area << "\nElement=" << getElementClassName(element)
		<< " id=" << element->getInternalID()
		<< " rect={" << formatRect(rect) << "}"
		<< " eventable=" << (element->isEventable() ? "yes" : "no")
		<< " active=" << (element->isActive() ? "yes" : "no");
	return out.str();
}

std::string Engine::describePanelWindow(const char* area, GUI_PanelWindow* pWindow, Sint32 x, Sint32 y)
{
	if(!pWindow)
		return std::string(area).append("\nPanelWindow=NULL");

	iRect& windowRect = pWindow->getRect();
	m_uiInspectorRect = windowRect;

	std::ostringstream out;
	out << area << "\nPanelWindow id=" << pWindow->getInternalID()
		<< " rect={" << formatRect(windowRect) << "}";

	if(pWindow->isResizeHandleAt(x, y))
	{
		m_uiInspectorRect = iRect(windowRect.x1, windowRect.y1 + windowRect.y2 - 4, windowRect.x2, 4);
		out << "\nElement=GUI_PanelWindow resize strip rect={" << formatRect(m_uiInspectorRect) << "}";
		return out.str();
	}

	GUI_Element* element = pWindow->getElementAt(x, y);
	if(element)
	{
		iRect& rect = element->getRect();
		m_uiInspectorRect = rect;
		out << "\nElement=" << getElementClassName(element)
			<< " id=" << element->getInternalID()
			<< " rect={" << formatRect(rect) << "}"
			<< " eventable=" << (element->isEventable() ? "yes" : "no")
			<< " active=" << (element->isActive() ? "yes" : "no");
	}
	else
		out << "\nElement=GUI_PanelWindow body/title";

	return out.str();
}

std::string Engine::describeUIAt(Sint32 x, Sint32 y)
{
	m_uiInspectorRect = iRect(0, 0, 0, 0);

	if(m_contextMenu && m_contextMenu->isInsideRect(x, y))
	{
		std::ostringstream out;
		out << "Area=ContextMenu\nMouse x=" << x << " y=" << y;
		return out.str();
	}

	if(m_actWindow && m_actWindow->isInsideRect(x, y))
	{
		iRect& windowRect = m_actWindow->getRect();
		m_uiInspectorRect = windowRect;

		std::ostringstream out;
		out << "Area=ActiveWindow id=" << m_actWindow->getInternalID()
			<< " rect={" << formatRect(windowRect) << "}";

		GUI_Element* element = m_actWindow->getElementAt(x, y);
		if(element)
		{
			iRect& rect = element->getRect();
			m_uiInspectorRect = rect;
			out << "\nElement=" << getElementClassName(element)
				<< " id=" << element->getInternalID()
				<< " rect={" << formatRect(rect) << "}"
				<< " eventable=" << (element->isEventable() ? "yes" : "no")
				<< " active=" << (element->isActive() ? "yes" : "no");
		}
		else
			out << "\nElement=GUI_Window body/title";
		return out.str();
	}

	if(m_ingame)
	{
		if(m_topPanel && m_topPanel->isInsideRect(x, y))
			return describePanelWindow("Area=TopPanel", m_topPanel, x, y);

		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		{
			GUI_Panel* panel = (*it);
			if(!panel->isInsideRect(x, y))
				continue;

			std::vector<GUI_PanelWindow*>& windows = panel->getPanelWindows();
			for(std::vector<GUI_PanelWindow*>::iterator wit = windows.begin(), wend = windows.end(); wit != wend; ++wit)
			{
				if((*wit)->isInsideRect(x, y))
				{
					std::ostringstream area;
					area << "Area=Panel id=" << panel->getInternalID()
						<< " rect={" << formatRect(panel->getRect()) << "}";
					return describePanelWindow(area.str().c_str(), (*wit), x, y);
				}
			}

			m_uiInspectorRect = panel->getRect();
			std::ostringstream out;
			out << "Area=Panel id=" << panel->getInternalID()
				<< "\nElement=GUI_Panel background rect={" << formatRect(m_uiInspectorRect) << "}";
			return out.str();
		}

		if(m_chatWindowRect.isPointInside(x, y))
		{
			m_uiInspectorRect = m_chatWindowRect;
			std::ostringstream out;
			out << "Area=ChatWindow rect={" << formatRect(m_chatWindowRect) << "}"
				<< "\nMouse x=" << x << " y=" << y;
			return out.str();
		}

		if(m_gameWindowRect.isPointInside(x, y))
		{
			m_uiInspectorRect = m_gameWindowRect;

			std::ostringstream out;
			out << "Area=GameWindow rect={" << formatRect(m_gameWindowRect) << "}";
			Creature* topCreature = NULL;
			Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, true);
			if(tile)
			{
				if(m_showDebugCoordinates)
				{
					const Position& tilePos = tile->getPosition();
					out << "\nTile x=" << tilePos.x << " y=" << tilePos.y << " z=" << SDL_static_cast(Uint32, tilePos.z);
				}
				else
				{
					// Keep absolute world coordinates out of clipboard/log text that may be shared publicly.
					out << "\nTile pos=redacted";
				}
				if(topCreature)
					out << " topCreatureId=" << topCreature->getId();

				Thing* thing = tile->getTopLookThing();
				if(thing && thing->isItem())
					out << " topItemId=" << thing->getItem()->getID();
			}
			else
				out << "\nTile=NULL";
			return out.str();
		}

		if(m_leftPanelAddRect.isPointInside(x, y)) {m_uiInspectorRect = m_leftPanelAddRect; return std::string("Area=LeftSidebarAdd rect={").append(formatRect(m_uiInspectorRect)).append("}");}
		if(m_leftPanelRemRect.isPointInside(x, y)) {m_uiInspectorRect = m_leftPanelRemRect; return std::string("Area=LeftSidebarRemove rect={").append(formatRect(m_uiInspectorRect)).append("}");}
		if(m_rightPanelAddRect.isPointInside(x, y)) {m_uiInspectorRect = m_rightPanelAddRect; return std::string("Area=RightSidebarAdd rect={").append(formatRect(m_uiInspectorRect)).append("}");}
		if(m_rightPanelRemRect.isPointInside(x, y)) {m_uiInspectorRect = m_rightPanelRemRect; return std::string("Area=RightSidebarRemove rect={").append(formatRect(m_uiInspectorRect)).append("}");}

		m_uiInspectorRect = m_gameBackgroundRect;
		return std::string("Area=GameBackground rect={").append(formatRect(m_uiInspectorRect)).append("}");
	}

	if(g_mainWindow && g_mainWindow->isInsideRect(x, y))
	{
		GUI_Element* element = g_mainWindow->getElementAt(x, y);
		if(element)
			return describeElement("Area=MainWindow", element);

		m_uiInspectorRect = g_mainWindow->getRect();
		return std::string("Area=MainWindow\nElement=GUI_Window body rect={").append(formatRect(m_uiInspectorRect)).append("}");
	}

	std::ostringstream out;
	out << "Area=ClientWindow\nMouse x=" << x << " y=" << y;
	return out.str();
}

void Engine::updateUIInspector(Sint32 x, Sint32 y)
{
	m_uiInspectorMouseX = x;
	m_uiInspectorMouseY = y;
	m_uiInspectorText = describeUIAt(x, y);
}

void Engine::logUIInspector(Sint32 x, Sint32 y, const char* action)
{
	updateUIInspector(x, y);

	std::string clipboardText = "[UIInspector] ";
	clipboardText.append(action);
	clipboardText.append("\nMouse x=").append(std::to_string(x)).append(" y=").append(std::to_string(y));
	clipboardText.append("\n").append(m_uiInspectorText);
	UTIL_SetClipboardTextLatin1(clipboardText.c_str());

	std::string singleLine = m_uiInspectorText;
	for(size_t i = 0; i < singleLine.length(); ++i)
	{
		if(singleLine[i] == '\n' || singleLine[i] == '\r')
			singleLine[i] = ' ';
	}

	std::string logLine = "[UIInspector] ";
	logLine.append(action).append(" ").append(singleLine);
	logLine.append(" clipboard=copied");
	g_logger.addLog(LOG_CATEGORY_INFO, logLine);

	SDL_snprintf(g_buffer, sizeof(g_buffer), "%sui-inspector.log", g_prefPath.c_str());
	SDL_RWops* logFile = SDL_RWFromFile(g_buffer, "ab");
	if(logFile)
	{
		std::string fileLine = UTIL_formatDate("%Y-%m-%d %H:%M:%S", time(NULL));
		fileLine.append(" ").append(logLine).append("\r\n");
		SDL_RWwrite(logFile, fileLine.c_str(), 1, fileLine.length());
		SDL_RWclose(logFile);
	}
}

std::string Engine::getDevRecorderDirectory()
{
	if(m_devRecorderDirectory.empty())
		startDevRecorderSession(m_devRecorder ? "record" : "snapshot");
	return m_devRecorderDirectory;
}

std::string Engine::startDevRecorderSession(const char* prefix)
{
	// Use SDL's preference path so recorder files survive deploy folder swaps
	// and do not get mixed with runtime assets beside the executable.
	std::string root = g_prefPath;
	root.append("DevRecorder");
	_mkdir(root.c_str());

	std::string stamp = UTIL_formatDate("%Y%m%d-%H%M%S", time(NULL));
	Uint32 ticks = SDL_GetTicks();
	std::string dir;
	for(Uint32 attempt = 0; attempt < 100; ++attempt)
	{
		char folderName[128];
		if(attempt == 0)
			SDL_snprintf(folderName, sizeof(folderName), "%s-%s-%08u", prefix, stamp.c_str(), ticks);
		else
			SDL_snprintf(folderName, sizeof(folderName), "%s-%s-%08u-%u", prefix, stamp.c_str(), ticks, attempt);

		dir = root;
		dir.append(1, PATH_PLATFORM_SLASH).append(folderName);
		if(_mkdir(dir.c_str()) == 0)
			break;
	}

	m_devRecorderCounter = 0;
	m_devRecorderLastDragCapture = 0;
	m_devRecorderDirectory = dir;
	m_devRecorderDirectory.append("\\");
	return m_devRecorderDirectory;
}

void Engine::recordDevEvent(const char* action, Sint32 x, Sint32 y, bool force)
{
	if(!m_devMode)
		return;
	if(!m_devRecorder && !force)
		return;

	std::string dir = getDevRecorderDirectory();
	std::string stamp = UTIL_formatDate("%Y%m%d-%H%M%S", time(NULL));
	char idBuffer[64];
	// Timestamp + monotonic counter avoids overwriting captures from rapid clicks.
	SDL_snprintf(idBuffer, sizeof(idBuffer), "%s-%04u", stamp.c_str(), ++m_devRecorderCounter);
	std::string baseName(idBuffer);
	std::string screenshotFile = dir + baseName + ".bmp";
	bool screenshotSaved = saveScreenshotBMP(screenshotFile.c_str());

	// Refresh inspector text at capture time so the log matches the screenshot.
	updateUIInspector(x, y);

	std::ostringstream out;
	out << "==== " << UTIL_formatDate("%Y-%m-%d %H:%M:%S", time(NULL)) << " " << action << " ====\r\n";
	out << "Mouse x=" << x << " y=" << y << "\r\n";
	out << "Screenshot=" << (screenshotSaved ? screenshotFile : "FAILED") << "\r\n";
	out << "Window w=" << m_windowW << " h=" << m_windowH << " engine=" << SDL_static_cast(Uint32, m_engine)
	    << " ingame=" << (m_ingame ? "yes" : "no") << "\r\n";
	out << "Rects gameWindow={" << formatRect(m_gameWindowRect) << "} gameBackground={" << formatRect(m_gameBackgroundRect)
	    << "} chat={" << formatRect(m_chatWindowRect) << "}\r\n";

	Creature* player = g_map.getLocalCreature();
	if(player)
	{
		const Position& playerPos = player->getCurrentPosition();
		out << "Player ";
		if(m_showDebugCoordinates)
			out << "x=" << playerPos.x << " y=" << playerPos.y << " z=" << SDL_static_cast(Uint32, playerPos.z);
		else
		{
			// Preserve movement state for debugging without leaking absolute hunt coordinates.
			out << "pos=redacted";
		}
		out
		    << " walking=" << (player->isWalking() ? "yes" : "no")
		    << " prewalking=" << (player->isPreWalking() ? "yes" : "no")
		    << " smoothing=" << (player->hasWalkSmoothing() ? "yes" : "no")
		    << " smoothingAnim=" << (player->isWalkSmoothingAnimating() ? "yes" : "no")
		    << " currentFrame=" << SDL_static_cast(Uint32, player->getCurrentFrame())
		    << " outfitAnim=" << SDL_static_cast(Uint32, player->getOutfitAnim())
		    << " walkedPixels=" << player->getWalkedPixels()
		    << " manualQueueSize=" << getManualActionQueueSize()
		    << " manualWalkPaused=" << (isManualActionWalkPaused() ? "yes" : "no") << "\r\n";
	}
	else
		out << "Player=NULL\r\n";

	out << "Bots manaTrainer=" << (m_manaTrainer ? "on" : "off")
	    << " autoHealer=" << (m_autoHealer ? "on" : "off")
	    << " autoFisher=" << (m_autoFisher ? "on" : "off")
	    << " autoRuner=" << (m_autoRuner ? "on" : "off")
	    << " autoEater=" << (m_autoEater ? "on" : "off")
	    << " autoLogout=" << (m_autoLogout ? "on" : "off")
	    << " autoLogoutPending=" << (m_autoLogoutPending ? "yes" : "no")
	    << " pk=" << (m_autoLogoutPK ? "on" : "off")
	    << " player=" << (m_autoLogoutPlayer ? "on" : "off") << "\r\n";
	if(m_autoLogoutPending)
		out << "AutoLogoutPending reason=\"" << m_autoLogoutPendingReason << "\" lastAttemptDelta="
		    << (m_autoLogoutLastAttempt == 0 ? 0 : g_frameTime - m_autoLogoutLastAttempt) << "\r\n";
	out << "ManualItemBotBlock remaining=" << ((m_manualItemBotBlockUntil != 0 && g_frameTime < m_manualItemBotBlockUntil) ? (m_manualItemBotBlockUntil - g_frameTime) : 0)
	    << " action=" << SDL_static_cast(Uint32, m_actionData) << "\r\n";
	out << "BotScheduler lastActionDelta=" << (m_botLastAction == 0 ? 0 : g_frameTime - m_botLastAction)
	    << " cooldown=" << m_botActionCooldown
	    << " lastMoveDelta=" << (m_lastPlayerMovementAction == 0 ? 0 : g_frameTime - m_lastPlayerMovementAction)
	    << " moveBlock=" << m_playerMovementBotBlock << "\r\n";

	out << "VisibleCreatures:\r\n";
	if(player)
	{
		const Position& playerPos = player->getCurrentPosition();
		const knownCreatures& creatures = g_map.getKnownCreatures();
		size_t visibleCreatures = 0;
		for(auto& kv : creatures)
		{
			Creature* creature = kv.second;
			if(!creature || creature == player)
				continue;

			const Position& cpos = creature->getCurrentPosition();
			if(cpos.z != playerPos.z || !Position::areInRange<8, 6>(cpos, playerPos))
				continue;

			out << " id=" << kv.first << " name=\"" << creature->getName() << "\""
			    << " type=" << SDL_static_cast(Uint32, creature->getType())
			    << " player=" << (creature->isPlayer() ? "yes" : "no")
			    << " skull=" << SDL_static_cast(Uint32, creature->getSkull());
			if(m_showDebugCoordinates)
				out << " x=" << cpos.x << " y=" << cpos.y << " z=" << SDL_static_cast(Uint32, cpos.z);
			else
				out << " pos=redacted";
			out << "\r\n";
			++visibleCreatures;
		}
		if(visibleCreatures == 0)
			out << " none\r\n";
	}
	else
		out << " none\r\n";

	// These slots are the common sources of bot/manual conflicts: runes, food,
	// fishing rod, fish, and temporary hand swaps.
	static const Uint8 trackedSlots[] = {SLOT_RIGHT, SLOT_LEFT, SLOT_AMMO};
	static const char* trackedSlotNames[] = {"right", "left", "ammo"};
	for(size_t i = 0; i < SDL_arraysize(trackedSlots); ++i)
	{
		ItemUI* item = g_game.getInventoryItem(trackedSlots[i]);
		out << "Inventory " << trackedSlotNames[i] << "=";
		if(item)
			out << item->getID() << "x" << SDL_static_cast(Uint32, UTIL_max<Uint16>(1, item->getItemCount()));
		else
			out << "empty";
		out << "\r\n";
	}

	out << "Containers:\r\n";
	bool anyContainer = false;
	for(Uint8 cid = 0; cid < GAME_MAX_CONTAINERS; ++cid)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont)
			continue;

		anyContainer = true;
		std::vector<ItemUI*>& items = cont->getItems();
		out << " cid=" << SDL_static_cast(Uint32, cid)
		    << " openOrder=" << cont->getOpenOrder()
		    << " name=\"" << cont->getName() << "\""
		    << " items=" << items.size() << "/" << SDL_static_cast(Uint32, cont->getCapacity())
		    << " unlocked=" << (cont->isUnlocked() ? "yes" : "no")
		    << " ids=[";
		for(size_t i = 0; i < items.size(); ++i)
		{
			ItemUI* item = items[i];
			if(!item)
				continue;
			if(i > 0)
				out << ",";
			out << item->getID() << "x" << SDL_static_cast(Uint32, UTIL_max<Uint16>(1, item->getItemCount()));
		}
		out << "]\r\n";
	}
	if(!anyContainer)
		out << " none\r\n";

	out << "UI:\r\n" << m_uiInspectorText << "\r\n\r\n";

	std::string logFileName = dir + "dev-recorder.log";
	SDL_RWops* logFile = SDL_RWFromFile(logFileName.c_str(), "ab");
	std::string logText = out.str();
	if(logFile)
	{
		SDL_RWwrite(logFile, logText.c_str(), 1, logText.length());
		SDL_RWclose(logFile);
	}

	std::string loggerLine("[DevRecorder] ");
	loggerLine.append(action).append(" ");
	loggerLine.append(screenshotSaved ? screenshotFile : "screenshot failed");
	g_logger.addLog(LOG_CATEGORY_INFO, loggerLine);
}

void Engine::onKeyDown(SDL_Event& event)
{
	if(m_devMode && event.key.repeat == 0 && event.key.keysym.sym == SDLK_i && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		m_uiInspector = !m_uiInspector;
		if(!m_uiInspector)
		{
			m_uiInspectorText.clear();
			m_uiInspectorRect = iRect(0, 0, 0, 0);
		}
		g_logger.addLog(LOG_CATEGORY_INFO, (m_uiInspector ? "[UIInspector] enabled" : "[UIInspector] disabled"));
		return;
	}
	if(m_devMode && event.key.repeat == 0 && event.key.keysym.sym == SDLK_v && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		// Ctrl+Alt+V toggles continuous dev recording for clicks and drag samples.
		m_devRecorder = !m_devRecorder;
		if(m_devRecorder)
		{
			std::string dir = startDevRecorderSession("record");
			std::string logLine("[DevRecorder] enabled folder=");
			logLine.append(dir).append(" | clicks/drag recorded");
			g_logger.addLog(LOG_CATEGORY_INFO, logLine);
			Sint32 mouseX = 0, mouseY = 0;
			SDL_GetMouseState(&mouseX, &mouseY);
			recordDevEvent("RecorderEnabled", mouseX, mouseY, true);
		}
		else
		{
			std::string logLine("[DevRecorder] disabled folder=");
			logLine.append(m_devRecorderDirectory.empty() ? "(none)" : m_devRecorderDirectory);
			g_logger.addLog(LOG_CATEGORY_INFO, logLine);
			m_devRecorderDirectory.clear();
			m_devRecorderCounter = 0;
			m_devRecorderLastDragCapture = 0;
		}
		return;
	}
	if(event.key.repeat == 0 && event.key.keysym.sym == SDLK_m && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		UTIL_manaTrainerOptions();
		return;
	}
	if(event.key.repeat == 0 && event.key.keysym.sym == SDLK_f && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		UTIL_autoFisherOptions();
		return;
	}
	if(event.key.repeat == 0 && event.key.keysym.sym == SDLK_l && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		UTIL_autoLogoutOptions();
		return;
	}
	if(event.key.repeat == 0 && event.key.keysym.sym == SDLK_r && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		UTIL_autoRunerOptions();
		return;
	}
	if(event.key.repeat == 0 && event.key.keysym.sym == SDLK_c && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		UTIL_comboLeaderOptions();
		return;
	}
	if(event.key.repeat == 0 && event.key.keysym.sym == SDLK_h && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		UTIL_autoHealerOptions();
		return;
	}
	if(event.key.repeat == 0 && event.key.keysym.sym == SDLK_e && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		UTIL_autoEaterOptions();
		return;
	}
	if(m_devMode && event.key.repeat == 0 && event.key.keysym.sym == SDLK_p && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_ALT))
	{
		m_lowProfileMode = !m_lowProfileMode;
		if(m_lowProfileMode)
		{
			m_lpSkillsWasOpen = (getPanel(GUI_PANEL_WINDOW_SKILLS) != nullptr);
			m_lpBattleWasOpen = (getPanel(GUI_PANEL_WINDOW_BATTLE) != nullptr);
			m_lpVipWasOpen = (getPanel(GUI_PANEL_WINDOW_VIP) != nullptr);
			if(m_lpSkillsWasOpen) UTIL_toggleSkillsWindow();
			if(m_lpBattleWasOpen) UTIL_toggleBattleWindow();
			if(m_lpVipWasOpen) UTIL_toggleVipWindow();
		}
		else
		{
			if(m_lpSkillsWasOpen) UTIL_toggleSkillsWindow();
			if(m_lpBattleWasOpen) UTIL_toggleBattleWindow();
			if(m_lpVipWasOpen) UTIL_toggleVipWindow();
		}
		return;
	}

	if(m_contextMenu)
		return;

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	HotkeyUsage* hotkey = getHotkey(event.key.keysym.sym, event.key.keysym.mod);
	if(hotkey)
	{
		switch(hotkey->hotkey)
		{
			case CLIENT_HOTKEY_DIALOGS_OPENTERMINAL:
			{
				if(m_devMode && event.key.repeat == 0)
					m_showLogger = !m_showLogger;
			}
			return;
			case CLIENT_HOTKEY_UI_TOGGLEFPSINDICATOR:
			{
				if(event.key.repeat == 0)
					m_showPerformance = !m_showPerformance;
			}
			return;
			case CLIENT_HOTKEY_UI_TOGGLEFULLSCREEN:
			{
				if(event.key.repeat == 0)
				{
					m_windowW = m_windowCachedW;
					m_windowH = m_windowCachedH;
					m_fullscreen = !m_fullscreen;
					g_inited = false;
				}
			}
			return;
			default: break;
		}
	}
	
	if(m_showLogger)
	{
		g_logger.onKeyDown(event);
		return;
	}

	if(m_actWindow)
	{
		m_actWindow->onKeyDown(event);
		return;
	}

	if(m_ingame)
	{
		Uint16 normalMods = UTIL_getNormalizedKeyboardMods(event.key.keysym.mod);
		if(g_chat.isChatInputActive() && normalMods == KMOD_NONE && UTIL_isNumpadChatNumber(event.key.keysym.sym, event.key.keysym.mod))
			return;

		if(event.key.repeat == 0 && m_wasdMode && !g_chat.isChatInputActive() &&
		   normalMods == KMOD_NONE && event.key.keysym.sym == SDLK_SPACE)
		{
			UTIL_attackNextBattleCreature();
			return;
		}

		if(UTIL_isClassicMovementKey(event.key.keysym.sym))
		{
			Direction direction = DIRECTION_NORTH;
			// Classic movement keys stay game controls even while chat is active.
			// WASD letters can be reclaimed by chat, but arrows/numpad are not text.
			if(normalMods == KMOD_NONE && UTIL_getClassicMovementDirection(event.key.keysym.sym, direction))
			{
				if(UTIL_isNumpadMovementKey(event.key.keysym.sym))
					startNumpadDash(event.key.keysym.sym, direction);

				notePlayerMovementAction(450);
				g_game.checkMovement(direction, true);
				return;
			}
			if(normalMods == KMOD_CTRL && UTIL_getClassicTurnDirection(event.key.keysym.sym, direction))
			{
				notePlayerMovementAction(250);
				g_game.sendTurn(direction);
				return;
			}
		}

		if(normalMods == KMOD_NONE)
		{
			if(event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
			{
				if(!m_wasdMode)
					g_chat.sendMessage();
			}
			else if(event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == SDLK_HOME || event.key.keysym.sym == SDLK_END)
				g_chat.onKeyDown(event);
		}
		else if(normalMods == KMOD_SHIFT)
		{
			if(event.key.keysym.sym == SDLK_UP)
				g_chat.navigateHistory(-1);
			else if(event.key.keysym.sym == SDLK_DOWN)
				g_chat.navigateHistory(1);
			else if(event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_HOME || event.key.keysym.sym == SDLK_END)
				g_chat.onKeyDown(event);
		}
		else if(normalMods == KMOD_CTRL)
		{
			if(event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_v)
				g_chat.onKeyDown(event);
		}

		if(hotkey)
		{
			bool allowMovementHotkey = (!g_chat.isChatInputActive() || UTIL_isClassicMovementKey(event.key.keysym.sym));
			switch(hotkey->hotkey)
			{
				case CLIENT_HOTKEY_MOVEMENT_GOEAST: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_EAST, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_GONORTH: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_NORTH, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_GOWEST: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_WEST, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_GOSOUTH: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_SOUTH, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_GONORTHWEST: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_NORTHWEST, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_GONORTHEAST: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_NORTHEAST, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_GOSOUTHWEST: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_SOUTHWEST, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_GOSOUTHEAST: if(allowMovementHotkey) { notePlayerMovementAction(450); g_game.checkMovement(DIRECTION_SOUTHEAST, true); } break;
				case CLIENT_HOTKEY_MOVEMENT_TURNEAST: if(allowMovementHotkey) { notePlayerMovementAction(250); g_game.sendTurn(DIRECTION_EAST); } break;
				case CLIENT_HOTKEY_MOVEMENT_TURNNORTH: if(allowMovementHotkey) { notePlayerMovementAction(250); g_game.sendTurn(DIRECTION_NORTH); } break;
				case CLIENT_HOTKEY_MOVEMENT_TURNWEST: if(allowMovementHotkey) { notePlayerMovementAction(250); g_game.sendTurn(DIRECTION_WEST); } break;
				case CLIENT_HOTKEY_MOVEMENT_TURNSOUTH: if(allowMovementHotkey) { notePlayerMovementAction(250); g_game.sendTurn(DIRECTION_SOUTH); } break;
				case CLIENT_HOTKEY_MOVEMENT_MOUNT:
				{
					if(event.key.repeat == 0 && g_game.hasGameFeature(GAME_FEATURE_MOUNTS))
					{
						Creature* localCreature = g_map.getLocalCreature();
						if(localCreature)
							g_game.sendMount(!localCreature->getMountType());
					}
				}
				break;
				case CLIENT_HOTKEY_MOVEMENT_STOPACTIONS:
				{
					if(event.key.repeat == 0)
					{
						g_engine.onExivaEsc();
						g_game.stopActions();
					}
				}
				break;
				case CLIENT_HOTKEY_DIALOGS_OPENBUGREPORTS:
				{
					if(event.key.repeat == 0)
						UTIL_createBugReport();
				}
				break;
				case CLIENT_HOTKEY_DIALOGS_OPENIGNORELIST:
				{
					//Open ignore list
				}
				break;
				case CLIENT_HOTKEY_DIALOGS_OPENOPTIONS:
				{
					if(event.key.repeat == 0)
						UTIL_options();
				}
				break;
				case CLIENT_HOTKEY_DIALOGS_OPENHOTKEYS:
				{
					if(event.key.repeat == 0)
						UTIL_hotkeyOptions();
				}
				break;
				case CLIENT_HOTKEY_DIALOGS_OPENQUESTLOG:
				{
					if(event.key.repeat == 0)
						g_game.sendOpenQuestLog();
				}
				break;
				case CLIENT_HOTKEY_WINDOWS_OPENVIPWINDOW:
				{
					if(event.key.repeat == 0)
						UTIL_toggleVipWindow();
				}
				break;
				case CLIENT_HOTKEY_WINDOWS_OPENBATTLEWINDOW:
				{
					if(event.key.repeat == 0)
						UTIL_toggleBattleWindow();
				}
				break;
				case CLIENT_HOTKEY_WINDOWS_OPENSKILLSWINDOW:
				{
					if(event.key.repeat == 0)
						UTIL_toggleSkillsWindow();
				}
				break;
				case CLIENT_HOTKEY_CHAT_CLOSECHANNEL:
				{
					if(event.key.repeat == 0)
						g_game.closeCurrentChannel();
				}
				break;
				case CLIENT_HOTKEY_CHAT_NEXTCHANNEL: g_game.switchToNextChannel(); break;
				case CLIENT_HOTKEY_CHAT_PREVIOUSCHANNEL: g_game.switchToPreviousChannel(); break;
				case CLIENT_HOTKEY_CHAT_OPENCHANNELLIST:
				{
					if(event.key.repeat == 0)
						g_game.sendRequestChannels();
				}
				break;
				case CLIENT_HOTKEY_CHAT_OPENHELPCHANNEL:
				{
					if(event.key.repeat == 0)
						g_game.openHelpChannel();
				}
				break;
				case CLIENT_HOTKEY_CHAT_OPENNPCCHANNEL:
				{
					if(event.key.repeat == 0)
						g_game.openNPCChannel();
				}
				break;
				case CLIENT_HOTKEY_CHAT_DEFAULTCHANNEL:
				{
					if(event.key.repeat == 0)
						g_game.switchToDefault();
				}
				break;
				case CLIENT_HOTKEY_CHAT_TOGGLECHAT:
				{
					if(event.key.repeat == 0)
						g_chat.sendMessage();
				}
				break;
				case CLIENT_HOTKEY_MINIMAP_CENTER: g_game.minimapCenter(); break;
				case CLIENT_HOTKEY_MINIMAP_FLOORDOWN: g_game.minimapFloorDown(); break;
				case CLIENT_HOTKEY_MINIMAP_FLOORUP: g_game.minimapFloorUp(); break;
				case CLIENT_HOTKEY_MINIMAP_SCROLLEAST: g_game.minimapScrollEast(); break;
				case CLIENT_HOTKEY_MINIMAP_SCROLLNORTH: g_game.minimapScrollNorth(); break;
				case CLIENT_HOTKEY_MINIMAP_SCROLLSOUTH: g_game.minimapScrollSouth(); break;
				case CLIENT_HOTKEY_MINIMAP_SCROLLWEST: g_game.minimapScrollWest(); break;
				case CLIENT_HOTKEY_MINIMAP_ZOOMIN: g_game.minimapZoomIn(); break;
				case CLIENT_HOTKEY_MINIMAP_ZOOMOUT: g_game.minimapZoomOut(); break;
				case CLIENT_HOTKEY_UI_TOGGLECREATUREINFO:
				{
					//Toggle creature info
				}
				break;
				case CLIENT_HOTKEY_COMBAT_SETOFFENSIVE:
				{
					if(event.key.repeat == 0)
					{
						setAttackMode(ATTACKMODE_ATTACK);
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_COMBAT_SETBALANCED:
				{
					if(event.key.repeat == 0)
					{
						setAttackMode(ATTACKMODE_BALANCED);
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_COMBAT_SETDEFENSIVE:
				{
					if(event.key.repeat == 0)
					{
						setAttackMode(ATTACKMODE_DEFENSE);
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_COMBAT_TOGGLECHASEMODE:
				{
					if(event.key.repeat == 0)
					{
						setChaseMode((getChaseMode() == CHASEMODE_STAND ? CHASEMODE_FOLLOW : CHASEMODE_STAND));
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_COMBAT_TOGGLESECUREMODE:
				{
					if(event.key.repeat == 0)
					{
						setSecureMode((getSecureMode() == SECUREMODE_SECURE ? SECUREMODE_UNSECURE : SECUREMODE_SECURE));
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_PVPMODE_SETDOVE:
				{
					if(event.key.repeat == 0)
					{
						setPvpMode(PVPMODE_DOVE);
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_PVPMODE_SETREDFIST:
				{
					if(event.key.repeat == 0)
					{
						setPvpMode(PVPMODE_RED_FIST);
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_PVPMODE_SETWHITEHAND:
				{
					if(event.key.repeat == 0)
					{
						setPvpMode(PVPMODE_WHITE_HAND);
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_PVPMODE_SETYELLOWHAND:
				{
					if(event.key.repeat == 0)
					{
						setPvpMode(PVPMODE_YELLOW_HAND);
						g_game.sendAttackModes();
					}
				}
				break;
				case CLIENT_HOTKEY_MISC_LENSHELP:
				{
					if(event.key.repeat == 0)
						setAction(CLIENT_ACTION_LENSHELP);
				}
				break;
				case CLIENT_HOTKEY_MISC_CHANGECHARACTER:
				{
					if(event.key.repeat == 0)
						UTIL_createCharacterList();
				}
				break;
				case CLIENT_HOTKEY_MISC_CHANGEOUTFIT:
				{
					if(event.key.repeat == 0)
						g_game.sendRequestOutfit();
				}
				break;
				case CLIENT_HOTKEY_MISC_LOGOUT:
				{
					if(event.key.repeat == 0)
						g_game.sendLogout();
				}
				break;
				case CLIENT_HOTKEY_MISC_TAKESCREENSHOT:
				{
					//Screenshot
				}
				break;
				case CLIENT_HOTKEY_MISC_NEXTPRESET:
				{
					//Switch to next preset
				}
				break;
				case CLIENT_HOTKEY_MISC_PREVIOUSPRESET:
				{
					//Switch to previous preset
				}
				break;
				case CLIENT_HOTKEY_ACTION:
				{
					if(event.key.repeat == 0)
					{
						HotkeyAction& hkAction = hotkey->action;
						switch(hkAction.type)
						{
							case CLIENT_HOTKEY_ACTION_TEXT:
							{
								if(hkAction.text.text && !hkAction.text.text->empty())
								{
									if(hkAction.text.sendAutomatically)
										g_game.sendSay(MessageSay, 0, std::string(), *hkAction.text.text);
									else
									{
										GUI_TextBox* tb = g_chat.getTextBox();
										if(tb) tb->setText(*hkAction.text.text);
										g_chat.setChatInputActive(true);
									}
								}
							}
							break;
							case CLIENT_HOTKEY_ACTION_USEITEM:
							case CLIENT_HOTKEY_ACTION_USEONYOURSELF:
							case CLIENT_HOTKEY_ACTION_USEONTARGET:
							case CLIENT_HOTKEY_ACTION_WITHCROSSHAIRS:
							case CLIENT_HOTKEY_ACTION_EQUIP:
							{
								Uint16 itemId = hkAction.item.itemId;
								if(itemId == 0) break;

								ItemUI* foundItem = NULL;
								for(Uint8 slot = SLOT_HEAD; slot < SLOT_LAST && !foundItem; ++slot)
								{
									ItemUI* invItem = g_game.getInventoryItem(slot);
									if(invItem && invItem->getID() == itemId)
										foundItem = invItem;
								}
								if(!foundItem)
								{
									for(Uint8 cid = 0; cid < GAME_MAX_CONTAINERS && !foundItem; ++cid)
									{
										Container* cont = g_game.findContainer(cid);
										if(!cont) continue;
										std::vector<ItemUI*>& items = cont->getItems();
										for(size_t j = 0; j < items.size() && !foundItem; ++j)
										{
											if(items[j] && items[j]->getID() == itemId)
												foundItem = items[j];
										}
									}
								}
								if(!foundItem) break;

								Position& pos = foundItem->getCurrentPosition();
								switch(hkAction.type)
								{
									case CLIENT_HOTKEY_ACTION_USEITEM:
									{
										Uint8 index = (pos.y & 0x40) ? SDL_static_cast(Uint8, pos.y & 0x0F) : g_game.findEmptyContainerId();
										sendManualUseItem(pos, itemId, 0, index);
									}
									break;
									case CLIENT_HOTKEY_ACTION_USEONYOURSELF:
										g_game.sendUseOnCreature(pos, itemId, 0, g_game.getPlayerID());
										break;
									case CLIENT_HOTKEY_ACTION_USEONTARGET:
									{
										Uint32 targetId = g_game.getAttackID();
										if(targetId != 0)
											g_game.sendUseOnCreature(pos, itemId, 0, targetId);
									}
									break;
									case CLIENT_HOTKEY_ACTION_WITHCROSSHAIRS:
										setActionData(CLIENT_ACTION_FIRST, 0, itemId, pos.x, pos.y, pos.z, 0);
										setAction(CLIENT_ACTION_USEWITH);
										break;
									case CLIENT_HOTKEY_ACTION_EQUIP:
										g_game.sendEquipItem(itemId, hkAction.item.itemSubtype);
										break;
									default: break;
								}
							}
							break;
							default: break;
						}
					}
				}
				break;
				default: break;
			}
			return;
		}
	}
}

void Engine::onKeyUp(SDL_Event& event)
{
	if(m_contextMenu)
		return;

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	if(m_showLogger)
	{
		g_logger.onKeyUp(event);
		return;
	}

	if(m_actWindow)
	{
		m_actWindow->onKeyUp(event);
		return;
	}

	if(m_ingame)
	{
		Uint16 normalMods = UTIL_getNormalizedKeyboardMods(event.key.keysym.mod);
		if(g_chat.isChatInputActive() && normalMods == KMOD_NONE && UTIL_isNumpadChatNumber(event.key.keysym.sym, event.key.keysym.mod))
			return;

		if(normalMods == KMOD_NONE)
		{
			if(event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == SDLK_HOME || event.key.keysym.sym == SDLK_END)
				g_chat.onKeyUp(event);
		}
		else if(normalMods == KMOD_SHIFT)
		{
			if(event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_HOME || event.key.keysym.sym == SDLK_END)
				g_chat.onKeyUp(event);
		}
		else if(normalMods == KMOD_CTRL)
		{
			if(event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_v)
				g_chat.onKeyUp(event);
		}

		if(UTIL_isClassicMovementKey(event.key.keysym.sym))
		{
			Direction direction = DIRECTION_NORTH;
			if(normalMods == KMOD_NONE && UTIL_getClassicMovementDirection(event.key.keysym.sym, direction))
			{
				if(UTIL_isNumpadMovementKey(event.key.keysym.sym))
					stopNumpadDash(event.key.keysym.sym);

				g_game.releaseMovement();
				return;
			}
		}

		HotkeyUsage* hotkey = getHotkey(event.key.keysym.sym, event.key.keysym.mod);
		if(hotkey)
		{
			switch(hotkey->hotkey)
			{
				case CLIENT_HOTKEY_MOVEMENT_GOEAST:
				case CLIENT_HOTKEY_MOVEMENT_GONORTH:
				case CLIENT_HOTKEY_MOVEMENT_GOWEST:
				case CLIENT_HOTKEY_MOVEMENT_GOSOUTH:
				case CLIENT_HOTKEY_MOVEMENT_GONORTHWEST:
				case CLIENT_HOTKEY_MOVEMENT_GONORTHEAST:
				case CLIENT_HOTKEY_MOVEMENT_GOSOUTHWEST:
				case CLIENT_HOTKEY_MOVEMENT_GOSOUTHEAST:
					g_game.releaseMovement();
					break;
				default: break;
			}
			return;
		}
	}
}

void Engine::onMouseMove(Sint32 x, Sint32 y)
{
	if(m_uiInspector)
		updateUIInspector(x, y);
	if(m_devRecorder)
	{
		Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
		if((mouseState & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) && (m_devRecorderLastDragCapture == 0 || g_frameTime - m_devRecorderLastDragCapture >= 500))
		{
			recordDevEvent("MouseMoveDrag", x, y);
			m_devRecorderLastDragCapture = g_frameTime;
		}
	}

	if(m_contextMenu)
	{
		m_contextMenu->onMouseMove(x, y, m_contextMenu->isInsideRect(x, y));
		return;
	}

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	if(m_showLogger)
	{
		g_logger.onMouseMove(x, y, true);
		return;
	}

	if(m_actWindow)
	{
		m_actWindow->onMouseMove(x, y, m_actWindow->isInsideRect(x, y));
		return;
	}

	if(m_ingame)
	{
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			(*it)->onMouseMove(x, y, (*it)->isInsideRect(x, y));

		{
			bool inside = m_leftPanelAddRect.isPointInside(x, y);
			if(m_leftAddPanel > 0)
			{
				if(m_leftAddPanel == 1 && !inside)
					m_leftAddPanel = 2;
				else if(m_leftAddPanel == 2 && inside)
					m_leftAddPanel = 1;
			}
			if(inside)
				showDescription(x, y, (m_canAddLeftPanel ? "Open new sidebar" : "Enlarge client or reduce game window to make room for further sidebar"));

			inside = m_leftPanelRemRect.isPointInside(x, y);
			if(m_leftRemPanel > 0)
			{
				if(m_leftRemPanel == 1 && !inside)
					m_leftRemPanel = 2;
				else if(m_leftRemPanel == 2 && inside)
					m_leftRemPanel = 1;
			}
			if(inside)
				showDescription(x, y, (m_haveExtraLeftPanel ? "Close this sidebar" : "Last sidebar cannot be closed"));
		}
		{
			bool inside = m_rightPanelAddRect.isPointInside(x, y);
			if(m_rightAddPanel > 0)
			{
				if(m_rightAddPanel == 1 && !inside)
					m_rightAddPanel = 2;
				else if(m_rightAddPanel == 2 && inside)
					m_rightAddPanel = 1;
			}
			if(inside)
				showDescription(x, y, (m_canAddRightPanel ? "Open new sidebar" : "Enlarge client or reduce game window to make room for further sidebar"));

			inside = m_rightPanelRemRect.isPointInside(x, y);
			if(m_rightRemPanel > 0)
			{
				if(m_rightRemPanel == 1 && !inside)
					m_rightRemPanel = 2;
				else if(m_rightRemPanel == 2 && inside)
					m_rightRemPanel = 1;
			}
			if(inside)
				showDescription(x, y, (m_haveExtraRightPanel ? "Close this sidebar" : "Last sidebar cannot be closed"));
		}

		g_chat.onMouseMove(m_chatWindowRect, x, y);
		if(m_actionData == CLIENT_ACTION_MOVEITEM || m_actionData == CLIENT_ACTION_USEWITH || m_actionData == CLIENT_ACTION_TRADE || m_actionData == CLIENT_ACTION_SEARCHHOTKEY)
			g_actualCursor = CLIENT_CURSOR_CROSSHAIR;
		else if(m_actionData == CLIENT_ACTION_LENSHELP)
			g_actualCursor = CLIENT_CURSOR_LENSHELP;
		else if(m_actionData == CLIENT_ACTION_LEFTMOUSE && m_moveItemX != SDL_MIN_SINT32 && m_moveItemY != SDL_MIN_SINT32)
		{
			Sint32 distanceX = std::abs(m_moveItemX - x);
			Sint32 distanceY = std::abs(m_moveItemY - y);
			if(distanceX >= 5 || distanceY >= 5)
			{
				m_actionData = CLIENT_ACTION_MOVEITEM;
				g_actualCursor = CLIENT_CURSOR_CROSSHAIR;

				//Disable move item actions
				m_moveItemX = SDL_MIN_SINT32;
				m_moveItemY = SDL_MIN_SINT32;
			}
		}
	}
	else
		g_mainWindow->onMouseMove(x, y, g_mainWindow->isInsideRect(x, y));
}

void Engine::onLMouseDown(Sint32 x, Sint32 y)
{
	if(m_uiInspector)
		logUIInspector(x, y, "LMouseDown");
	if(m_devRecorder)
		recordDevEvent("LMouseDown", x, y);

	if(m_actionData != CLIENT_ACTION_NONE)
	{
		if(m_classicControl && m_actionData == CLIENT_ACTION_RIGHTMOUSE)
			setAction(CLIENT_ACTION_EXTRAMOUSE);

		return;
	}

	if(m_contextMenu)
		return;

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	if(m_showLogger)
	{
		g_logger.onLMouseDown(x, y);
		return;
	}

	if(m_actWindow)
	{
		if(m_actWindow->isInsideRect(x, y))
			m_actWindow->onLMouseDown(x, y);
		return;
	}

	if(m_ingame)
	{
		GUI_Panel* gPanel = NULL;
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		{
			if((*it)->isInsideRect(x, y))
			{
				gPanel = (*it);
				break;
			}
		}
		if(gPanel)
		{
			gPanel->onLMouseDown(x, y);
			return;
		}
		if(m_gameWindowRect.isPointInside(x, y))
		{
			Creature* topCreature;
			Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, true);
			if(tile)
			{
				Thing* thing = tile->getTopMoveThing();
				if(thing)
				{
					Position& position = tile->getPosition();
					setActionData(CLIENT_ACTION_FIRST, (thing->isCreature() ? thing->getCreature()->getId() : 0), (thing->isItem() ? thing->getItem()->getID() : 0x62), position.x, position.y, position.z, SDL_static_cast(Uint8, tile->getThingStackPos(thing)));
					setActionData(CLIENT_ACTION_SECOND, 0, (thing->isItem() ? thing->getItem()->getItemCount() : 1), 0, 0, 0, 0);
					enableMoveItem(x, y);
				}
			}

			setAction(CLIENT_ACTION_LEFTMOUSE);
			return;
		}
		else
			g_chat.onLMouseDown(m_chatWindowRect, x, y);

		{
			if(m_leftPanelAddRect.isPointInside(x, y))
				m_leftAddPanel = 1;
			else if(m_leftPanelRemRect.isPointInside(x, y))
				m_leftRemPanel = 1;
		}
		{
			if(m_rightPanelAddRect.isPointInside(x, y))
				m_rightAddPanel = 1;
			else if(m_rightPanelRemRect.isPointInside(x, y))
				m_rightRemPanel = 1;
		}
	}
	else
	{
		if(g_mainWindow)
		{
			if(g_mainWindow->isInsideRect(x, y))
				g_mainWindow->onLMouseDown(x, y);
		}
	}
}

void Engine::onLMouseUp(Sint32 x, Sint32 y)
{
	if(m_devRecorder)
		recordDevEvent("LMouseUp", x, y);

	if(m_actionData != CLIENT_ACTION_NONE)
	{
		if(!m_ingame || m_contextMenu || m_description || m_showLogger || m_actWindow || m_actionData == CLIENT_ACTION_LENSHELP)
		{
			setAction(CLIENT_ACTION_NONE);
			return;
		}

		Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
		if(m_actionData == CLIENT_ACTION_MOVEITEM)
		{
			GUI_Panel* gPanel = NULL;
			for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			{
				if((*it)->isInsideRect(x, y))
				{
					gPanel = (*it);
					break;
				}
			}
			if(gPanel)
				gPanel->onLMouseUp(x, y);
			else if(m_gameWindowRect.isPointInside(x, y))
			{
				Creature* topCreature;
				Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, true);
				if(tile)
				{
					Position& position = tile->getPosition();
					initMove(position.x, position.y, position.z);
				}
			}

			setAction(CLIENT_ACTION_NONE);
		}
		else if(m_actionData == CLIENT_ACTION_USEWITH)
		{
			Creature* topCreature = NULL;
			ItemUI* itemui = NULL;
			Thing* useThing = NULL;

			GUI_Panel* gPanel = NULL;
			for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			{
				if((*it)->isInsideRect(x, y))
				{
					gPanel = (*it);
					break;
				}
			}
			if(gPanel)
			{
				itemui = SDL_reinterpret_cast(ItemUI*, gPanel->onAction(x, y));
				if(!itemui && g_game.getSelectID() != 0)
					topCreature = g_map.getCreatureById(g_game.getSelectID());
			}
			else if(m_gameWindowRect.isPointInside(x, y))
			{
				Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, true);
				if(tile)
					useThing = tile->getTopUseThing();
			}

			ClientActionData& actionData = g_engine.m_actionDataStructure[CLIENT_ACTION_FIRST];
			if(topCreature)
				g_game.sendUseOnCreature(Position(actionData.posX, actionData.posY, actionData.posZ), actionData.itemId, actionData.posStack, topCreature->getId());
			else if(useThing)
			{
				if(useThing->isItem())
				{
					Item* item = useThing->getItem();
					Position& position = item->getCurrentPosition();
					Tile* itemTile = g_map.getTile(position);
					if(itemTile)
						sendManualUseItemEx(Position(actionData.posX, actionData.posY, actionData.posZ), actionData.itemId, actionData.posStack, position, item->getID(), SDL_static_cast(Uint8, itemTile->getThingStackPos(item)));
				}
			}
			else if(itemui)
			{
				Position& position = itemui->getCurrentPosition();
				sendManualUseItemEx(Position(actionData.posX, actionData.posY, actionData.posZ), actionData.itemId, actionData.posStack, position, itemui->getID(), 0);
			}

			setAction(CLIENT_ACTION_NONE);
		}
		else if(m_actionData == CLIENT_ACTION_TRADE)
		{
			Creature* topCreature = NULL;

			GUI_Panel* gPanel = NULL;
			for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			{
				if((*it)->isInsideRect(x, y))
				{
					gPanel = (*it);
					break;
				}
			}
			if(gPanel)
			{
				if(g_game.getSelectID() != 0)
					topCreature = g_map.getCreatureById(g_game.getSelectID());
			}
			else if(m_gameWindowRect.isPointInside(x, y))
				g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, true);

			ClientActionData& actionData = g_engine.m_actionDataStructure[CLIENT_ACTION_FIRST];
			if(topCreature && topCreature->isPlayer())
				g_game.sendRequestTrade(Position(actionData.posX, actionData.posY, actionData.posZ), actionData.itemId, actionData.posStack, topCreature->getId());
			else
				g_game.processTextMessage(MessageFailure, "Select a player to trade with.");

			setAction(CLIENT_ACTION_NONE);
		}
		else if(m_actionData == CLIENT_ACTION_SEARCHHOTKEY)
		{
			//Make hotkey
			setAction(CLIENT_ACTION_NONE);
		}
		else if((!(mouseState & SDL_BUTTON_RMASK) && m_actionData == CLIENT_ACTION_EXTRAMOUSE) || m_actionData == CLIENT_ACTION_LEFTMOUSE)
		{
			Creature* topCreature = NULL;
			ItemUI* itemui = NULL;
			Thing* lookThing = NULL;
			Thing* useThing = NULL;

			GUI_Panel* gPanel = NULL;
			for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			{
				if((*it)->isInsideRect(x, y))
				{
					gPanel = (*it);
					break;
				}
			}
			if(gPanel)
				itemui = SDL_reinterpret_cast(ItemUI*, gPanel->onAction(x, y));
			else if(m_gameWindowRect.isPointInside(x, y))
			{
				Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, true);
				if(tile)
				{
					lookThing = tile->getTopLookThing();
					useThing = tile->getTopUseThing();
				}
			}

			if(topCreature || itemui || lookThing || useThing)
			{
				Uint16 keyMods = UTIL_parseModifiers(SDL_static_cast(Uint16, SDL_GetModState()));
				if(m_actionData == CLIENT_ACTION_EXTRAMOUSE || keyMods == KMOD_SHIFT)
				{
					if(topCreature)
					{
						if(g_game.hasGameFeature(GAME_FEATURE_LOOKATCREATURE))
							g_game.sendLookInBattle(topCreature->getId());
						else
						{
							Position& position = topCreature->getCurrentPosition();
							Tile* creatureTile = g_map.getTile(position);
							if(creatureTile)
								g_game.sendLookAt(position, 0x62, SDL_static_cast(Uint8, creatureTile->getThingStackPos(topCreature)));
						}
					}
					else if(lookThing)
					{
						if(lookThing->isItem())
						{
							Item* item = lookThing->getItem();
							Position& position = item->getCurrentPosition();
							Tile* itemTile = g_map.getTile(position);
							if(itemTile)
								g_game.sendLookAt(position, item->getID(), SDL_static_cast(Uint8, itemTile->getThingStackPos(item)));
						}
					}
					else if(itemui)
					{
						Position& position = itemui->getCurrentPosition();
						g_game.sendLookAt(position, itemui->getID(), 0);
					}
				}
				else if(m_actionData == CLIENT_ACTION_LEFTMOUSE)
				{
					if(!m_classicControl && keyMods == KMOD_ALT)
					{
						if(topCreature && g_game.getPlayerID() != topCreature->getId())
							g_game.sendAttack((g_game.getAttackID() == topCreature->getId()) ? NULL : topCreature);
						else if(itemui)
						{
							GUI_ContextMenu* newMenu = createThingContextMenu(NULL, itemui, NULL);
							newMenu->setEventCallback(&Engine::standardThingEvent);
							showContextMenu(newMenu, x, y);
						}
					}
					else if(!m_classicControl && keyMods == KMOD_CTRL)
					{
						if(useThing)
						{
							if(useThing->isItem())
							{
								Item* item = useThing->getItem();
								Position& position = item->getCurrentPosition();
								Tile* itemTile = g_map.getTile(position);
								if(itemTile)
								{
									if(item->getThingType() && item->getThingType()->hasFlag(ThingAttribute_MultiUse))
									{
										setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), position.x, position.y, position.z, SDL_static_cast(Uint8, itemTile->getUseStackPos(item)));
										setAction(CLIENT_ACTION_USEWITH);
										return;
									}
									else
										sendManualUseItem(position, item->getID(), SDL_static_cast(Uint8, itemTile->getUseStackPos(item)), g_game.findEmptyContainerId());
								}
							}
						}
						else if(itemui)
						{
							Position& position = itemui->getCurrentPosition();
							if(itemui->getThingType() && itemui->getThingType()->hasFlag(ThingAttribute_MultiUse))
							{
								setActionData(CLIENT_ACTION_FIRST, 0, itemui->getID(), position.x, position.y, position.z, 0);
								setAction(CLIENT_ACTION_USEWITH);
								return;
							}
							else
							{
								if(position.y & 0x40)
									sendManualUseItem(position, itemui->getID(), 0, (position.y & 0x0F));
								else
									sendManualUseItem(position, itemui->getID(), 0, g_game.findEmptyContainerId());
							}
						}
					}
					else if(m_classicControl && keyMods == KMOD_ALT)
					{
						if(topCreature && g_game.getPlayerID() != topCreature->getId())
							g_game.sendAttack((g_game.getAttackID() == topCreature->getId()) ? NULL : topCreature);
						else if(useThing)
						{
							if(useThing->isItem())
							{
								Item* item = useThing->getItem();
								Position& position = item->getCurrentPosition();
								Tile* itemTile = g_map.getTile(position);
								if(itemTile)
								{
									if(item->getThingType() && item->getThingType()->hasFlag(ThingAttribute_MultiUse))
									{
										setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), position.x, position.y, position.z, SDL_static_cast(Uint8, itemTile->getUseStackPos(item)));
										setAction(CLIENT_ACTION_USEWITH);
										return;
									}
									else
										sendManualUseItem(position, item->getID(), SDL_static_cast(Uint8, itemTile->getUseStackPos(item)), g_game.findEmptyContainerId());
								}
							}
						}
						else if(itemui)
						{
							Position& position = itemui->getCurrentPosition();
							if(itemui->getThingType() && itemui->getThingType()->hasFlag(ThingAttribute_MultiUse))
							{
								setActionData(CLIENT_ACTION_FIRST, 0, itemui->getID(), position.x, position.y, position.z, 0);
								setAction(CLIENT_ACTION_USEWITH);
								return;
							}
							else
							{
								if(position.y & 0x40)
									sendManualUseItem(position, itemui->getID(), 0, (position.y & 0x0F));
								else
									sendManualUseItem(position, itemui->getID(), 0, g_game.findEmptyContainerId());
							}
						}
					}
					else if(m_classicControl && keyMods == KMOD_CTRL)
					{
						GUI_ContextMenu* newMenu = createThingContextMenu(topCreature, itemui, (useThing && useThing->isItem() ? useThing->getItem() : NULL));
						newMenu->setEventCallback(&Engine::standardThingEvent);
						showContextMenu(newMenu, x, y);
					}
					else if(keyMods == KMOD_NONE && m_gameWindowRect.isPointInside(x, y))
					{
						Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, false);
						if(tile)
						{
							Position& autoWalkPosition = tile->getPosition();
							notePlayerMovementAction(900);
							g_game.startAutoWalk(autoWalkPosition);
						}
					}
				}
			}
			else
			{
				Uint16 keyMods = UTIL_parseModifiers(SDL_static_cast(Uint16, SDL_GetModState()));
				if(keyMods == KMOD_NONE && m_gameWindowRect.isPointInside(x, y))
				{
					Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, false);
					if(tile)
					{
						Position& autoWalkPosition = tile->getPosition();
						notePlayerMovementAction(900);
						g_game.startAutoWalk(autoWalkPosition);
					}
				}
			}

			setAction(CLIENT_ACTION_NONE);
		}
		return;
	}

	if(m_contextMenu)
	{
		m_contextMenu->onLMouseUp(x, y);
		delete m_contextMenu;
		m_contextMenu = NULL;
		return;
	}

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	if(m_showLogger)
	{
		g_logger.onLMouseUp(x, y);
		return;
	}

	if(m_actWindow)
	{
		m_actWindow->onLMouseUp(x, y);
		return;
	}

	if(m_ingame)
	{
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			(*it)->onLMouseUp(x, y);

		g_chat.onLMouseUp(m_chatWindowRect, x, y);

		{
			if(m_leftAddPanel == 1)
			{
				m_panels.push_back(new GUI_Panel(iRect(0, 0, 0, 0), (m_leftPanel == GUI_PANEL_RANDOM ? GUI_PANEL_EXTRA_LEFT_START : ++m_leftPanel)));
				recalculateGameWindow();
			}
			else if(m_leftRemPanel == 1)
			{
				if(m_leftPanel != GUI_PANEL_RANDOM)
				{
					for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
					{
						if((*it)->getInternalID() == m_leftPanel)
						{
							delete (*it);
							m_panels.erase(it);
							break;
						}
					}
					recalculateGameWindow();
				}
			}
			
			m_leftAddPanel = 0;
			m_leftRemPanel = 0;
		}
		{
			if(m_rightAddPanel == 1)
			{
				m_panels.push_back(new GUI_Panel(iRect(0, 0, 0, 0), (m_rightPanel == GUI_PANEL_MAIN ? GUI_PANEL_EXTRA_RIGHT_START : ++m_rightPanel)));
				recalculateGameWindow();
			}
			else if(m_rightRemPanel == 1)
			{
				if(m_rightPanel != GUI_PANEL_MAIN)
				{
					for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
					{
						if((*it)->getInternalID() == m_rightPanel)
						{
							delete (*it);
							m_panels.erase(it);
							break;
						}
					}
					recalculateGameWindow();
				}
			}

			m_rightAddPanel = 0;
			m_rightRemPanel = 0;
		}
	}
	else
	{
		if(g_mainWindow)
			g_mainWindow->onLMouseUp(x, y);
	}
}

void Engine::onRMouseDown(Sint32 x, Sint32 y)
{
	if(m_uiInspector)
		logUIInspector(x, y, "RMouseDown");
	if(m_devRecorder)
		recordDevEvent("RMouseDown", x, y);

	if(m_actionData != CLIENT_ACTION_NONE)
	{
		if(m_classicControl && m_actionData == CLIENT_ACTION_LEFTMOUSE)
			setAction(CLIENT_ACTION_EXTRAMOUSE);

		return;
	}

	if(m_contextMenu)
		return;

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	if(m_showLogger)
	{
		g_logger.onRMouseDown(x, y);
		return;
	}

	if(m_actWindow)
	{
		if(m_actWindow->isInsideRect(x, y))
			m_actWindow->onRMouseDown(x, y);
		return;
	}

	if(m_ingame)
	{
		GUI_Panel* gPanel = NULL;
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		{
			if((*it)->isInsideRect(x, y))
			{
				gPanel = (*it);
				break;
			}
		}
		if(gPanel)
		{
			gPanel->onRMouseDown(x, y);
			return;
		}
		if(m_gameWindowRect.isPointInside(x, y))
			setAction(CLIENT_ACTION_RIGHTMOUSE);
		else
			g_chat.onRMouseDown(m_chatWindowRect, x, y);
	}
	else
	{
		if(g_mainWindow)
		{
			if(g_mainWindow->isInsideRect(x, y))
				g_mainWindow->onRMouseDown(x, y);
		}
	}
}

void Engine::onRMouseUp(Sint32 x, Sint32 y)
{
	if(m_devRecorder)
		recordDevEvent("RMouseUp", x, y);

	if(m_actionData != CLIENT_ACTION_NONE)
	{
		if(!m_ingame || m_contextMenu || m_description || m_showLogger || m_actWindow || m_actionData == CLIENT_ACTION_USEWITH || m_actionData == CLIENT_ACTION_TRADE || m_actionData == CLIENT_ACTION_LENSHELP || m_actionData == CLIENT_ACTION_SEARCHHOTKEY)
		{
			setAction(CLIENT_ACTION_NONE);
			return;
		}

		Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
		if((!(mouseState & SDL_BUTTON_LMASK) && m_actionData == CLIENT_ACTION_EXTRAMOUSE) || m_actionData == CLIENT_ACTION_RIGHTMOUSE)
		{
			Creature* topCreature = NULL;
			ItemUI* itemui = NULL;
			Thing* lookThing = NULL;
			Thing* useThing = NULL;

			GUI_Panel* gPanel = NULL;
			for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			{
				if((*it)->isInsideRect(x, y))
				{
					gPanel = (*it);
					break;
				}
			}
			if(gPanel)
				itemui = SDL_reinterpret_cast(ItemUI*, gPanel->onAction(x, y));
			else if(m_gameWindowRect.isPointInside(x, y))
			{
				Tile* tile = g_map.findTile(x, y, m_gameWindowRect, m_scaledSize, m_scale, topCreature, true);
				if(tile)
				{
					lookThing = tile->getTopLookThing();
					useThing = tile->getTopUseThing();
				}
			}

			if(topCreature || itemui || lookThing || useThing)
			{
				Uint16 keyMods = UTIL_parseModifiers(SDL_static_cast(Uint16, SDL_GetModState()));
				if(m_actionData == CLIENT_ACTION_EXTRAMOUSE || keyMods == KMOD_SHIFT)
				{
					if(topCreature)
					{
						if(g_game.hasGameFeature(GAME_FEATURE_LOOKATCREATURE))
							g_game.sendLookInBattle(topCreature->getId());
						else
						{
							Position& position = topCreature->getCurrentPosition();
							Tile* creatureTile = g_map.getTile(position);
							if(creatureTile)
								g_game.sendLookAt(position, 0x62, SDL_static_cast(Uint8, creatureTile->getThingStackPos(topCreature)));
						}
					}
					else if(lookThing)
					{
						if(lookThing->isItem())
						{
							Item* item = lookThing->getItem();
							Position& position = item->getCurrentPosition();
							Tile* itemTile = g_map.getTile(position);
							if(itemTile)
								g_game.sendLookAt(position, item->getID(), SDL_static_cast(Uint8, itemTile->getThingStackPos(item)));
						}
					}
					else if(itemui)
					{
						Position& position = itemui->getCurrentPosition();
						g_game.sendLookAt(position, itemui->getID(), 0);
					}
				}
				else if(m_actionData == CLIENT_ACTION_RIGHTMOUSE)
				{
					if(keyMods == KMOD_NONE && itemui && UTIL_tryAutoStackCoin(itemui))
					{
						setAction(CLIENT_ACTION_NONE);
						return;
					}

					if(!m_classicControl && keyMods == KMOD_ALT)
					{
						if(topCreature && g_game.getPlayerID() != topCreature->getId())
							g_game.sendAttack((g_game.getAttackID() == topCreature->getId()) ? NULL : topCreature);
						else if(itemui)
						{
							GUI_ContextMenu* newMenu = createThingContextMenu(NULL, itemui, NULL);
							newMenu->setEventCallback(&Engine::standardThingEvent);
							showContextMenu(newMenu, x, y);
						}
					}
					else if(!m_classicControl && keyMods == KMOD_CTRL)
					{
						if(useThing)
						{
							if(useThing->isItem())
							{
								Item* item = useThing->getItem();
								Position& position = item->getCurrentPosition();
								Tile* itemTile = g_map.getTile(position);
								if(itemTile)
								{
									if(item->getThingType() && item->getThingType()->hasFlag(ThingAttribute_MultiUse))
									{
										setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), position.x, position.y, position.z, SDL_static_cast(Uint8, itemTile->getUseStackPos(item)));
										setAction(CLIENT_ACTION_USEWITH);
										return;
									}
									else
										sendManualUseItem(position, item->getID(), SDL_static_cast(Uint8, itemTile->getUseStackPos(item)), g_game.findEmptyContainerId());
								}
							}
						}
						else if(itemui)
						{
							Position& position = itemui->getCurrentPosition();
							if(itemui->getThingType() && itemui->getThingType()->hasFlag(ThingAttribute_MultiUse))
							{
								setActionData(CLIENT_ACTION_FIRST, 0, itemui->getID(), position.x, position.y, position.z, 0);
								setAction(CLIENT_ACTION_USEWITH);
								return;
							}
							else
							{
								if(position.y & 0x40)
									sendManualUseItem(position, itemui->getID(), 0, (position.y & 0x0F));
								else
									sendManualUseItem(position, itemui->getID(), 0, g_game.findEmptyContainerId());
							}
						}
					}
					else if(m_classicControl && (keyMods == KMOD_NONE || keyMods == KMOD_ALT))
					{
						if(topCreature && g_game.getPlayerID() != topCreature->getId())
							g_game.sendAttack((g_game.getAttackID() == topCreature->getId()) ? NULL : topCreature);
						else if(useThing)
						{
							if(useThing->isItem())
							{
								Item* item = useThing->getItem();
								Position& position = item->getCurrentPosition();
								Tile* itemTile = g_map.getTile(position);
								if(itemTile)
								{
									if(item->getThingType() && item->getThingType()->hasFlag(ThingAttribute_MultiUse))
									{
										setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), position.x, position.y, position.z, SDL_static_cast(Uint8, itemTile->getUseStackPos(item)));
										setAction(CLIENT_ACTION_USEWITH);
										return;
									}
									else
										sendManualUseItem(position, item->getID(), SDL_static_cast(Uint8, itemTile->getUseStackPos(item)), g_game.findEmptyContainerId());
								}
							}
						}
						else if(itemui)
						{
							Position& position = itemui->getCurrentPosition();
							if(itemui->getThingType() && itemui->getThingType()->hasFlag(ThingAttribute_MultiUse))
							{
								setActionData(CLIENT_ACTION_FIRST, 0, itemui->getID(), position.x, position.y, position.z, 0);
								setAction(CLIENT_ACTION_USEWITH);
								return;
							}
							else
							{
								if(position.y & 0x40)
									sendManualUseItem(position, itemui->getID(), 0, (position.y & 0x0F));
								else
									sendManualUseItem(position, itemui->getID(), 0, g_game.findEmptyContainerId());
							}
						}
					}
					else if((m_classicControl && keyMods == KMOD_CTRL) || (!m_classicControl && keyMods == KMOD_NONE))
					{
						GUI_ContextMenu* newMenu = createThingContextMenu(topCreature, itemui, (useThing && useThing->isItem() ? useThing->getItem() : NULL));
						newMenu->setEventCallback(&Engine::standardThingEvent);
						showContextMenu(newMenu, x, y);
					}
				}
			}

			setAction(CLIENT_ACTION_NONE);
		}
		return;
	}

	if(m_contextMenu)
	{
		delete m_contextMenu;
		m_contextMenu = NULL;
		return;
	}

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	if(m_showLogger)
	{
		g_logger.onRMouseUp(x, y);
		return;
	}

	if(m_actWindow)
	{
		m_actWindow->onRMouseUp(x, y);
		return;
	}

	if(m_ingame)
	{
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			(*it)->onRMouseUp(x, y);

		g_chat.onRMouseUp(m_chatWindowRect, x, y);
	}
	else
	{
		if(g_mainWindow)
			g_mainWindow->onRMouseUp(x, y);
	}
}

void Engine::onWheel(Sint32 x, Sint32 y, bool wheelUP)
{
	if(m_contextMenu)
		return;

	if(m_description)
	{
		delete m_description;
		m_description = NULL;
	}

	if(m_showLogger)
	{
		g_logger.onWheel(x, y, wheelUP);
		return;
	}

	if(m_actWindow)
	{
		if(m_actWindow->isInsideRect(x, y))
			m_actWindow->onWheel(x, y, wheelUP);
		return;
	}

	if(m_ingame)
	{
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		{
			if((*it)->isInsideRect(x, y))
			{
				(*it)->onWheel(x, y, wheelUP);
				return;
			}
		}
		g_chat.onWheel(m_chatWindowRect, x, y, wheelUP);
	}
	else
	{
		if(g_mainWindow && g_mainWindow->isInsideRect(x, y))
			g_mainWindow->onWheel(x, y, wheelUP);
	}
}

void Engine::onTextInput(const char* textInput)
{
	if(m_showLogger)
	{
		g_logger.onTextInput(textInput);
		return;
	}

	if(m_actWindow)
	{
		m_actWindow->onTextInput(textInput);
		return;
	}

	if(m_ingame)
		g_chat.onTextInput(textInput);
	else
	{
		if(g_mainWindow)
			g_mainWindow->onTextInput(textInput);
	}
}

void Engine::windowResize(Sint32 width, Sint32 height)
{
	m_windowW = width;
	m_windowH = height;
	if(!m_maximized && !m_fullscreen)
	{
		m_windowCachedW = m_windowW;
		m_windowCachedH = m_windowH;
	}
	m_surface->doResize(m_windowW, m_windowH);
	for(std::vector<GUI_Window*>::iterator it = m_windows.begin(), end = m_windows.end(); it != end; ++it)
		(*it)->onReshape(m_windowW, m_windowH);

	if(m_actWindow)
		m_actWindow->onReshape(m_windowW, m_windowH);

	if(g_mainWindow)
		g_mainWindow->onReshape(m_windowW, m_windowH);

	if(m_ingame)
		recalculateGameWindow();
}

void Engine::windowMoved(Sint32 x, Sint32 y)
{
	if(!m_maximized && !m_fullscreen)
	{
		m_windowX = x;
		m_windowY = y;
	}
}

void Engine::takeScreenshot(void* data1, void* data2)
{
	//TODO: add other popular extensions like png
	char* fileName = SDL_reinterpret_cast(char*, data1);
	Uint32 flags = SDL_static_cast(Uint32, SDL_reinterpret_cast(size_t, data2));
	if(!fileName || !m_surface)
		return;

	if(flags == SCREENSHOT_FLAG_SAVEASBMP)
		saveScreenshotBMP(fileName);

	SDL_free(fileName);
}

bool Engine::saveScreenshotBMP(const char* fileName)
{
	if(!fileName || !m_surface)
		return false;

	Sint32 screenWidth;
	Sint32 screenHeight;
	bool pixelsBGRA;
	unsigned char* pixels = m_surface->getScreenPixels(screenWidth, screenHeight, pixelsBGRA);
	if(!pixels)
		return false;

	Uint32 Rmask, Gmask, Bmask;
	if(pixelsBGRA)
	{
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		Rmask = 0x0000FF00;
		Gmask = 0x00FF0000;
		Bmask = 0xFF000000;
		#else
		Rmask = 0x00FF0000;
		Gmask = 0x0000FF00;
		Bmask = 0x000000FF;
		#endif
	}
	else
	{
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		Rmask = 0xFF000000;
		Gmask = 0x00FF0000;
		Bmask = 0x0000FF00;
		#else
		Rmask = 0x000000FF;
		Gmask = 0x0000FF00;
		Bmask = 0x00FF0000;
		#endif
	}

	bool saved = false;
	SDL_Surface* s = SDL_CreateRGBSurfaceFrom(pixels, screenWidth, screenHeight, 32, 4 * screenWidth, Rmask, Gmask, Bmask, 0x00000000);
	if(s)
	{
		saved = (SDL_SaveBMP(s, fileName) == 0);
		SDL_FreeSurface(s);
	}
	SDL_free(pixels);
	return saved;
}

static std::string alertJsonEscape(const std::string& text)
{
	std::string escaped;
	escaped.reserve(text.size() + 16);
	for(char ch : text)
	{
		switch(ch)
		{
			case '\\': escaped.append("\\\\"); break;
			case '"': escaped.append("\\\""); break;
			case '\r': break;
			case '\n': escaped.append("\\n"); break;
			default:
				if(SDL_static_cast(Uint8, ch) >= 0x20)
					escaped.push_back(ch);
				break;
		}
	}
	return escaped;
}

static void discordAlertCallback(Uint32, Sint32 requestId)
{
	HttpRequest* request = g_http.getRequest(SDL_static_cast(Uint32, requestId));
	if(request && request->result == HTTP_RESULT_SUCCEEDED)
		g_logger.addLog(LOG_CATEGORY_INFO, "[Alert] Discord notification sent.");
	else
		g_logger.addLog(LOG_CATEGORY_ERROR, "[Alert] Discord notification failed. Check the webhook URL.");
}

void Engine::captureAlertEvent(const std::string& reason)
{
	if(!m_alertCaptureEnabled && m_discordWebhook.empty())
		return;

	const std::string stamp = UTIL_formatDate("%Y%m%d-%H%M%S", time(NULL));
	std::string screenshotFile;
	std::string contextFile;
	std::string captureDirectory;
	bool screenshotSaved = false;

	if(m_alertCaptureEnabled)
	{
		std::string root = g_prefPath;
		root.append("AlertCaptures");
		_mkdir(root.c_str());

		std::string token;
		for(char ch : reason)
		{
			if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
				token.push_back(ch);
			else if(!token.empty() && token.back() != '-')
				token.push_back('-');
		}
		if(token.empty())
			token = "event";

		const Uint32 ticks = SDL_GetTicks();
		for(Uint32 attempt = 0; attempt < 100; ++attempt)
		{
			char folderName[160];
			if(attempt == 0)
				SDL_snprintf(folderName, sizeof(folderName), "%s-%s-%08u", token.c_str(), stamp.c_str(), ticks);
			else
				SDL_snprintf(folderName, sizeof(folderName), "%s-%s-%08u-%u", token.c_str(), stamp.c_str(), ticks, attempt);

			captureDirectory = root;
			captureDirectory.append(1, PATH_PLATFORM_SLASH).append(folderName);
			if(_mkdir(captureDirectory.c_str()) == 0)
				break;
		}

		screenshotFile = captureDirectory;
		screenshotFile.append(1, PATH_PLATFORM_SLASH).append("context.bmp");
		contextFile = captureDirectory;
		contextFile.append(1, PATH_PLATFORM_SLASH).append("context.txt");
		screenshotSaved = saveScreenshotBMP(screenshotFile.c_str());
	}

	std::ostringstream context;
	context << "Time: " << UTIL_formatDate("%Y-%m-%d %H:%M:%S", time(NULL)) << "\r\n";
	context << "Reason: " << reason << "\r\n";
	context << "Character: " << (g_game.getPlayerName().empty() ? "(unknown)" : g_game.getPlayerName()) << "\r\n";
	context << "HP: " << g_game.getPlayerHealth() << "/" << g_game.getPlayerMaxHealth()
		<< " (" << SDL_static_cast(Uint32, g_game.getPlayerHealthPercent()) << "%)\r\n";
	context << "Mana: " << g_game.getPlayerMana() << "/" << g_game.getPlayerMaxMana()
		<< " (" << SDL_static_cast(Uint32, g_game.getPlayerManaPercent()) << "%)\r\n";
	context << "Attack target id: " << g_game.getAttackID() << "\r\n";

	Creature* player = g_map.getLocalCreature();
	context << "Visible creatures:";
	if(player)
	{
		const Position& playerPos = player->getCurrentPosition();
		bool anyVisible = false;
		for(const auto& entry : g_map.getKnownCreatures())
		{
			Creature* creature = entry.second;
			if(!creature || creature == player)
				continue;
			const Position& creaturePos = creature->getCurrentPosition();
			if(creaturePos.z != playerPos.z || !Position::areInRange<8, 6>(creaturePos, playerPos))
				continue;
			context << (anyVisible ? ", " : " ") << creature->getName();
			anyVisible = true;
		}
		if(!anyVisible)
			context << " none";
	}
	else
		context << " none";
	context << "\r\n";
	context << "Screenshot: " << (screenshotSaved ? screenshotFile : "not saved") << "\r\n";

	if(m_alertCaptureEnabled && !contextFile.empty())
	{
		SDL_RWops* file = SDL_RWFromFile(contextFile.c_str(), "wb");
		if(file)
		{
			const std::string text = context.str();
			SDL_RWwrite(file, text.c_str(), 1, text.size());
			SDL_RWclose(file);
		}
	}

	if(screenshotSaved)
		g_logger.addLog(LOG_CATEGORY_INFO, std::string("[Alert] Context captured: ").append(captureDirectory));
	else if(m_alertCaptureEnabled)
		g_logger.addLog(LOG_CATEGORY_ERROR, "[Alert] Failed to save context screenshot.");

	if(!m_discordWebhook.empty())
	{
		const bool validWebhook =
			m_discordWebhook.find("https://discord.com/api/webhooks/") == 0 ||
			m_discordWebhook.find("https://discordapp.com/api/webhooks/") == 0;
		if(!validWebhook)
		{
			g_logger.addLog(LOG_CATEGORY_ERROR, "[Alert] Discord webhook must use the official discord.com webhook URL.");
			return;
		}

		std::ostringstream message;
		message << "**Tibia alert: " << reason << "**\n";
		message << "Character: " << (g_game.getPlayerName().empty() ? "(unknown)" : g_game.getPlayerName()) << "\n";
		message << "HP: " << g_game.getPlayerHealth() << "/" << g_game.getPlayerMaxHealth()
			<< " | Mana: " << g_game.getPlayerMana() << "/" << g_game.getPlayerMaxMana();
		const std::string payload = std::string("{\"content\":\"").append(alertJsonEscape(message.str())).append("\"}");
		g_http.addDiscordWebhook(m_discordWebhook, payload, screenshotSaved ? screenshotFile : std::string(), &discordAlertCallback, 0);
	}
}

HotkeyUsage* Engine::getHotkey(SDL_Keycode key, Uint16 mods)
{
	// Normalize mods: collapse L/R variants so KMOD_LSHIFT matches KMOD_SHIFT etc.
	Uint16 normalMods = UTIL_getNormalizedKeyboardMods(mods);

	std::map<Uint16, std::map<SDL_Keycode, size_t>>::iterator mit = m_hotkeyFastAccess.find(normalMods);
	if(mit != m_hotkeyFastAccess.end())
	{
		std::map<SDL_Keycode, size_t>::iterator it = mit->second.find(key);
		if(it != mit->second.end())
			return &m_hotkeys[it->second];
		else
			return NULL;
	}
	return NULL;
}

void Engine::bindHotkey(ClientHotkeyKeys hotKey, SDL_Keycode key, Uint16 mods, ClientHotkeys hotkeyType)
{
	std::map<Uint16, std::map<SDL_Keycode, size_t>>::iterator mit = m_hotkeyFastAccess.find(mods);
	if(mit != m_hotkeyFastAccess.end())
	{
		std::map<SDL_Keycode, size_t>::iterator it = mit->second.find(key);
		if(it != mit->second.end())
			mit->second.erase(it);
	}

	HotkeyUsage newHotkey;
	newHotkey.action.type = CLIENT_HOTKEY_ACTION_NONE;
	newHotkey.keycode = key;
	newHotkey.hotkey = hotkeyType;
	newHotkey.modifers = mods;
	newHotkey.keyid = SDL_static_cast(Uint8, hotKey);
	m_hotkeyFastAccess[mods][key] = m_hotkeys.size();
	m_hotkeys.push_back(newHotkey);
}

void Engine::setWASDMode(bool enable)
{
	m_hotkeys.clear();
	m_hotkeyFastAccess.clear();
	resetToDefaultHotkeys(enable);
	g_chat.setChatInputActive(!enable);
}

void Engine::resetToDefaultHotkeys(bool wasd)
{
	m_wasdMode = wasd;

	// Register F1-F12 text hotkey slots (NONE, SHIFT, CTRL)
	static const SDL_Keycode s_fkeys[12] = {
		SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6,
		SDLK_F7, SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11, SDLK_F12
	};
	for(int i = 0; i < 12; ++i)
	{
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, s_fkeys[i], KMOD_NONE,  CLIENT_HOTKEY_ACTION);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, s_fkeys[i], KMOD_SHIFT, CLIENT_HOTKEY_ACTION);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, s_fkeys[i], KMOD_CTRL,  CLIENT_HOTKEY_ACTION);
	}
	if(wasd)
	{
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_w, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GONORTH);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_a, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOWEST);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_s, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOSOUTH);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_d, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOEAST);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_q, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GONORTHWEST);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_e, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GONORTHEAST);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_z, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOSOUTHWEST);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_c, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOSOUTHEAST);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_RETURN, KMOD_NONE, CLIENT_HOTKEY_CHAT_TOGGLECHAT);
		bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_ENTER, KMOD_NONE, CLIENT_HOTKEY_CHAT_TOGGLECHAT);
	}
	// Arrow keys always work alongside WASD
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_UP, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNNORTH);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_LEFT, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNWEST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_DOWN, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNSOUTH);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_RIGHT, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNEAST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_UP, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GONORTH);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_LEFT, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOWEST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_DOWN, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOSOUTH);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_RIGHT, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOEAST);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_8, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNNORTH);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_4, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNWEST);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_2, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNSOUTH);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_6, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNEAST);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_8, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GONORTH);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_4, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOWEST);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_2, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOSOUTH);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_KP_6, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOEAST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_KP_7, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GONORTHWEST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_KP_9, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GONORTHEAST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_KP_1, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOSOUTHWEST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_KP_3, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_GOSOUTHEAST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_t, (KMOD_CTRL + KMOD_ALT), CLIENT_HOTKEY_DIALOGS_OPENTERMINAL);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_r, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_MOUNT);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_z, KMOD_CTRL, CLIENT_HOTKEY_DIALOGS_OPENBUGREPORTS);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_i, KMOD_CTRL, CLIENT_HOTKEY_DIALOGS_OPENIGNORELIST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_k, KMOD_CTRL, CLIENT_HOTKEY_DIALOGS_OPENHOTKEYS);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_u, KMOD_CTRL, CLIENT_HOTKEY_DIALOGS_OPENQUESTLOG);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_p, KMOD_CTRL, CLIENT_HOTKEY_WINDOWS_OPENVIPWINDOW);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_b, KMOD_CTRL, CLIENT_HOTKEY_WINDOWS_OPENBATTLEWINDOW);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_s, KMOD_CTRL, CLIENT_HOTKEY_WINDOWS_OPENSKILLSWINDOW);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_e, KMOD_CTRL, CLIENT_HOTKEY_CHAT_CLOSECHANNEL);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_o, KMOD_CTRL, CLIENT_HOTKEY_CHAT_OPENCHANNELLIST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_t, KMOD_CTRL, CLIENT_HOTKEY_CHAT_OPENHELPCHANNEL);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_n, KMOD_CTRL, CLIENT_HOTKEY_UI_TOGGLECREATUREINFO);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_f, KMOD_CTRL, CLIENT_HOTKEY_UI_TOGGLEFULLSCREEN);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_h, KMOD_CTRL, CLIENT_HOTKEY_MISC_LENSHELP);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_g, KMOD_CTRL, CLIENT_HOTKEY_MISC_CHANGECHARACTER);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_l, KMOD_CTRL, CLIENT_HOTKEY_MISC_LOGOUT);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_q, KMOD_CTRL, CLIENT_HOTKEY_MISC_LOGOUT);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_TAB, KMOD_SHIFT, CLIENT_HOTKEY_CHAT_PREVIOUSCHANNEL);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_d, KMOD_ALT, CLIENT_HOTKEY_CHAT_DEFAULTCHANNEL);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_PAGEDOWN, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_FLOORDOWN);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_PAGEUP, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_FLOORUP);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_RIGHT, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_SCROLLEAST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_UP, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_SCROLLNORTH);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_DOWN, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_SCROLLSOUTH);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_LEFT, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_SCROLLWEST);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_END, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_ZOOMIN);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_HOME, KMOD_ALT, CLIENT_HOTKEY_MINIMAP_ZOOMOUT);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_F8, KMOD_ALT, CLIENT_HOTKEY_UI_TOGGLEFPSINDICATOR);
	bindHotkey(CLIENT_HOTKEY_SECOND_KEY, SDLK_RETURN, KMOD_ALT, CLIENT_HOTKEY_UI_TOGGLEFULLSCREEN);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_ESCAPE, KMOD_NONE, CLIENT_HOTKEY_MOVEMENT_STOPACTIONS);
	bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_TAB, KMOD_NONE, CLIENT_HOTKEY_CHAT_NEXTCHANNEL);

	// WASD Ctrl+turns must come last so they override any conflicting bindings above
	if(wasd)
	{
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_w, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNNORTH);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_a, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNWEST);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_s, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNSOUTH);
		bindHotkey(CLIENT_HOTKEY_FIRST_KEY, SDLK_d, KMOD_CTRL, CLIENT_HOTKEY_MOVEMENT_TURNEAST);
	}

	Config hkCfg;
	SDL_snprintf(g_buffer, sizeof(g_buffer), "%sconfig.cfg", g_prefPath.c_str());
	if(hkCfg.openToRead(g_buffer))
	{
		for(Sint32 i = 0; i < HOTKEY_CONFIG_COUNT; ++i)
		{
			std::string data = hkCfg.fetchKey(s_hotkeyConfigNames[i]);
			if(data.empty() || data == "None") continue;

			HotkeyUsage* hk = getHotkey(s_hotkeyConfigKeys[i], s_hotkeyConfigMods[i]);
			if(!hk) continue;

			StringVector parts = UTIL_explodeString(data, ":", 3);
			if(parts.empty()) continue;

			if(parts[0] == "I" && parts.size() >= 3)
			{
				Uint8 actionType = SDL_static_cast(Uint8, SDL_strtoul(parts[1].c_str(), NULL, 10));
				Uint16 itemId = SDL_static_cast(Uint16, SDL_strtoul(parts[2].c_str(), NULL, 10));
				Uint8 subtype = (parts.size() >= 4 ? SDL_static_cast(Uint8, SDL_strtoul(parts[3].c_str(), NULL, 10)) : 0);
				hk->action.item.type = actionType;
				hk->action.item.itemId = itemId;
				hk->action.item.itemSubtype = subtype;
			}
			else if(parts[0] == "T" && parts.size() >= 3)
			{
				hk->action.text.type = CLIENT_HOTKEY_ACTION_TEXT;
				hk->action.text.sendAutomatically = (parts[1] == "1");
				hk->action.text.text = new std::string(parts[2]);
			}
		}
	}
}

Sint32 Engine::calculateMainHeight()
{
	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
	{
		if((*it)->getInternalID() == GUI_PANEL_MAIN)
			return (*it)->getRect().y2 - (*it)->getFreeHeight();
	}
	return 0;
}

bool Engine::hasExtraLeftSidebar()
{
	return m_leftPanel != GUI_PANEL_RANDOM;
}

bool Engine::hasExtraRightSidebar()
{
	return m_rightPanel != GUI_PANEL_MAIN;
}

void Engine::setExtraLeftSidebar(bool enabled)
{
	if(!m_ingame)
	{
		m_leftPanel = (enabled ? GUI_PANEL_EXTRA_LEFT_START : GUI_PANEL_RANDOM);
		return;
	}

	if(enabled)
	{
		if(m_leftPanel == GUI_PANEL_RANDOM)
		{
			m_panels.push_back(new GUI_Panel(iRect(0, 0, GAME_PANEL_FIXED_WIDTH, m_windowH), GUI_PANEL_EXTRA_LEFT_START));
			recalculateGameWindow();
		}
		return;
	}

	bool removed = false;
	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(); it != m_panels.end();)
	{
		Sint32 panelId = (*it)->getInternalID();
		if(panelId >= GUI_PANEL_EXTRA_LEFT_START && panelId <= GUI_PANEL_EXTRA_LEFT_END)
		{
			delete (*it);
			it = m_panels.erase(it);
			removed = true;
		}
		else
			++it;
	}

	m_leftPanel = GUI_PANEL_RANDOM;
	if(removed)
		recalculateGameWindow();
}

void Engine::setExtraRightSidebar(bool enabled)
{
	if(!m_ingame)
	{
		m_rightPanel = (enabled ? GUI_PANEL_EXTRA_RIGHT_START : GUI_PANEL_MAIN);
		return;
	}

	if(enabled)
	{
		if(m_rightPanel == GUI_PANEL_MAIN)
		{
			m_panels.push_back(new GUI_Panel(iRect(0, 0, GAME_PANEL_FIXED_WIDTH, m_windowH), GUI_PANEL_EXTRA_RIGHT_START));
			recalculateGameWindow();
		}
		return;
	}

	bool removed = false;
	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(); it != m_panels.end();)
	{
		Sint32 panelId = (*it)->getInternalID();
		if(panelId >= GUI_PANEL_EXTRA_RIGHT_START && panelId <= GUI_PANEL_EXTRA_RIGHT_END)
		{
			delete (*it);
			it = m_panels.erase(it);
			removed = true;
		}
		else
			++it;
	}

	m_rightPanel = GUI_PANEL_MAIN;
	if(removed)
		recalculateGameWindow();
}

void Engine::setDevMode(bool enabled)
{
	m_devMode = enabled;
	if(enabled)
		return;

	m_showLogger = false;
	m_showDebugCoordinates = false;
	m_devRecorder = false;
	m_devRecorderDirectory.clear();
	m_devRecorderCounter = 0;
	m_devRecorderLastDragCapture = 0;
	m_uiInspector = false;
	m_uiInspectorText.clear();
	m_uiInspectorRect = iRect(0, 0, 0, 0);
}

void Engine::recalculateGameWindow()
{
	m_leftPanel = GUI_PANEL_RANDOM;
	m_rightPanel = GUI_PANEL_MAIN;
	m_haveExtraLeftPanel = false;
	m_haveExtraRightPanel = false;

	Sint32 minWidth = 248;
	Sint32 minHeight = 184;

	Sint32 x = 0;
	Sint32 y = 0;
	Sint32 width = m_windowW;
	Sint32 height = UTIL_max<Sint32>(minHeight, UTIL_min<Sint32>(m_windowH - 111, m_windowH - m_consoleHeight));
	Sint32 mainHeight = calculateMainHeight();
	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(); it != m_panels.end(); ++it)
	{
		Sint32 panelId = (*it)->getInternalID();
		switch(panelId)
		{
			case GUI_PANEL_MAIN:
			{
				width -= GAME_PANEL_FIXED_WIDTH;

				iRect panelRect = (*it)->getRect();
				panelRect.x1 = m_windowW - GAME_PANEL_FIXED_WIDTH;
				panelRect.x2 = GAME_PANEL_FIXED_WIDTH;
				panelRect.y1 = y;
				panelRect.y2 = m_windowH;
				(*it)->setRect(panelRect);
				(*it)->tryFreeHeight(0);
			}
			break;
			case GUI_PANEL_RIGHT:
			{
				width -= GAME_PANEL_FIXED_WIDTH;

				iRect panelRect = (*it)->getRect();
				panelRect.x1 = m_windowW - GAME_PANEL_FIXED_WIDTH;
				panelRect.x2 = GAME_PANEL_FIXED_WIDTH;
				panelRect.y1 = y + mainHeight;
				panelRect.y2 = m_windowH - mainHeight;
				(*it)->setRect(panelRect);
				(*it)->tryFreeHeight(0);
			}
			break;
			default:
			{
				if(width - GAME_PANEL_FIXED_WIDTH < minWidth)
				{
					delete (*it);
					it = m_panels.erase(it);
					--it;//We can do this assuming main panel should be first in vector
					break;
				}

				if(panelId >= GUI_PANEL_EXTRA_RIGHT_START && panelId <= GUI_PANEL_EXTRA_RIGHT_END)
				{
					width -= GAME_PANEL_FIXED_WIDTH;
					if(m_rightPanel == GUI_PANEL_MAIN || panelId > m_rightPanel)
						m_rightPanel = panelId;

					iRect panelRect = (*it)->getRect();
					panelRect.x1 = m_windowW - ((panelId - GUI_PANEL_EXTRA_RIGHT_START + 2) * GAME_PANEL_FIXED_WIDTH);
					panelRect.x2 = GAME_PANEL_FIXED_WIDTH;
					panelRect.y1 = y;
					panelRect.y2 = m_windowH;
					(*it)->setRect(panelRect);
					(*it)->tryFreeHeight(0);
					m_haveExtraRightPanel = true;
				}
				else if(panelId >= GUI_PANEL_EXTRA_LEFT_START && panelId <= GUI_PANEL_EXTRA_LEFT_END)
				{
					x += GAME_PANEL_FIXED_WIDTH;
					width -= GAME_PANEL_FIXED_WIDTH;
					if(m_leftPanel == GUI_PANEL_RANDOM || panelId > m_leftPanel)
						m_leftPanel = panelId;

					iRect panelRect = (*it)->getRect();
					panelRect.x1 = (panelId - GUI_PANEL_EXTRA_LEFT_START) * GAME_PANEL_FIXED_WIDTH;
					panelRect.x2 = GAME_PANEL_FIXED_WIDTH;
					panelRect.y1 = y;
					panelRect.y2 = m_windowH;
					(*it)->setRect(panelRect);
					(*it)->tryFreeHeight(0);
					m_haveExtraLeftPanel = true;
				}
			}
			break;
		}
	}

	m_scaledSize = UTIL_min<Sint32>((width - 4) / (GAME_MAP_WIDTH - 3), (height - 4) / (GAME_MAP_HEIGHT - 3));
	m_scale = m_scaledSize * 0.03125f;

	Sint32 windowWidth = m_scaledSize * (GAME_MAP_WIDTH - 3);
	Sint32 windowHeight = m_scaledSize * (GAME_MAP_HEIGHT - 3);
	height = UTIL_min<Sint32>(height, windowHeight + 4);
	m_consoleHeight = m_windowH - height;
	Sint32 windowX = UTIL_max<Sint32>(x + (width / 2 - windowWidth / 2), x + 2);
	Sint32 windowY = UTIL_max<Sint32>(y + (height / 2 - windowHeight / 2), y + 2);
	m_gameBackgroundRect = iRect(x, y, width, height);
	m_gameWindowRect = iRect(windowX, windowY, windowWidth, windowHeight);
	m_chatWindowRect = iRect(x, height, width, m_consoleHeight);

	bool condition = (width - windowWidth - 4 >= GAME_PANEL_FIXED_WIDTH);
	m_canAddLeftPanel = (condition ? true : false);
	m_canAddRightPanel = (condition ? true : false);
}

void Engine::setConsoleHeight(Sint32 height)
{
	m_consoleHeight = height;
	recalculateGameWindow();
}

void Engine::notePlayerMovementAction(Uint32 blockMs)
{
	m_lastPlayerMovementAction = g_frameTime;
	m_playerMovementBotBlock = blockMs;
	if(blockMs <= 450)
	{
		if(m_contextMenu) { delete m_contextMenu; m_contextMenu = NULL; }
		g_manualActionQueue.clear();
	}
}

void Engine::noteManualItemInteraction(Uint32 blockMs)
{
	Uint32 until = g_frameTime + blockMs;
	if(until > m_manualItemBotBlockUntil)
		m_manualItemBotBlockUntil = until;
}

bool Engine::canRunBotAction(Uint32 movementBlockMs)
{
	if(!m_ingame)
		return false;

	if(m_actionData == CLIENT_ACTION_MOVEITEM || m_actionData == CLIENT_ACTION_USEWITH ||
	   m_actionData == CLIENT_ACTION_LEFTMOUSE || m_actionData == CLIENT_ACTION_RIGHTMOUSE)
		return false;

	if(m_manualItemBotBlockUntil != 0 && g_frameTime < m_manualItemBotBlockUntil)
		return false;

	if(m_botLastAction != 0 && g_frameTime - m_botLastAction < m_botActionCooldown)
		return false;

	Creature* player = g_map.getLocalCreature();
	if(player && (player->isWalking() || player->isPreWalking()))
		return false;

	// Manual movement gets a short grace period so automated actions do not
	// collide with autowalk/prewalk packets and cause visible server correction.
	Uint32 blockMs = UTIL_max<Uint32>(m_playerMovementBotBlock, movementBlockMs);
	if(m_lastPlayerMovementAction != 0 && g_frameTime - m_lastPlayerMovementAction < blockMs)
		return false;

	return true;
}

bool Engine::hasPendingManualAction()
{
	return !g_manualActionQueue.empty();
}

bool Engine::shouldPauseWalkForManualAction()
{
	for(std::deque<QueuedManualAction>::const_iterator it = g_manualActionQueue.begin(), end = g_manualActionQueue.end(); it != end; ++it)
	{
		if(!it->safeWhileWalking)
			return true;
	}
	return false;
}

void Engine::markManualActionPausedWalk()
{
	m_manualActionPausedWalk = true;
	m_manualActionResumeWalk = false;
}

size_t Engine::getManualActionQueueSize()
{
	return g_manualActionQueue.size();
}

bool Engine::isManualActionWalkPaused()
{
	return m_manualActionPausedWalk || m_manualActionResumeWalk;
}

bool Engine::shouldDelayManualAction(Uint32 enqueueTime, bool safeWhileWalking)
{
	if(!m_ingame)
		return true;

	if(safeWhileWalking)
		return false;

	if(m_manualLastAction != 0 && g_frameTime - m_manualLastAction < MANUAL_ACTION_COOLDOWN)
		return true;

	Creature* player = g_map.getLocalCreature();
	if(player && (player->isWalking() || player->isPreWalking()))
		return true;

	if(m_lastPlayerMovementAction != 0 && g_frameTime - m_lastPlayerMovementAction < MANUAL_ACTION_MIN_DELAY)
		return true;

	if(g_frameTime - enqueueTime >= MANUAL_ACTION_MAX_WAIT)
		return false;

	return false;
}

void Engine::sendManualMove(const Position& fromPos, Uint16 itemid, Uint8 stackpos, const Position& toPos, Uint16 count)
{
	QueuedManualAction action;
	action.type = MANUAL_ACTION_MOVE;
	action.fromPos = fromPos;
	action.toPos = toPos;
	action.itemId = itemid;
	action.stackpos = stackpos;
	action.count = count;
	action.enqueueTime = g_frameTime;
	action.safeWhileWalking = (UTIL_isVirtualItemPosition(fromPos) && UTIL_isVirtualItemPosition(toPos));
	noteManualItemInteraction();

	// Fast path keeps inventory/container actions as responsive as Look. World
	// moves still queue around prewalk to avoid server correction jitter.
	if(g_manualActionQueue.empty() && !shouldDelayManualAction(action.enqueueTime, action.safeWhileWalking))
	{
		UTIL_sendQueuedManualAction(action);
		m_botLastAction = g_frameTime;
		m_botActionCooldown = MANUAL_ACTION_COOLDOWN;
		if(!action.safeWhileWalking)
		{
			m_manualLastAction = g_frameTime;
		}
		return;
	}

	if(g_manualActionQueue.size() >= MANUAL_ACTION_MAX_QUEUE)
		g_manualActionQueue.pop_front();
	g_manualActionQueue.push_back(action);
}

void Engine::sendManualUseItem(const Position& position, Uint16 itemId, Uint8 stackpos, Uint8 index)
{
	QueuedManualAction action;
	action.type = MANUAL_ACTION_USE_ITEM;
	action.fromPos = position;
	action.itemId = itemId;
	action.stackpos = stackpos;
	action.index = index;
	action.enqueueTime = g_frameTime;
	action.safeWhileWalking = UTIL_isVirtualItemPosition(position);
	noteManualItemInteraction();

	if(g_manualActionQueue.empty() && !shouldDelayManualAction(action.enqueueTime, action.safeWhileWalking))
	{
		UTIL_sendQueuedManualAction(action);
		m_botLastAction = g_frameTime;
		m_botActionCooldown = MANUAL_ACTION_COOLDOWN;
		if(!action.safeWhileWalking)
		{
			m_manualLastAction = g_frameTime;
		}
		return;
	}

	if(g_manualActionQueue.size() >= MANUAL_ACTION_MAX_QUEUE)
		g_manualActionQueue.pop_front();
	g_manualActionQueue.push_back(action);
}

void Engine::sendManualUseItemEx(const Position& fromPos, Uint16 itemId, Uint8 fromStackPos, const Position& toPos, Uint16 toItemId, Uint8 toStackPos)
{
	QueuedManualAction action;
	action.type = MANUAL_ACTION_USE_ITEM_EX;
	action.fromPos = fromPos;
	action.toPos = toPos;
	action.itemId = itemId;
	action.toItemId = toItemId;
	action.stackpos = fromStackPos;
	action.toStackpos = toStackPos;
	action.enqueueTime = g_frameTime;
	action.safeWhileWalking = (UTIL_isVirtualItemPosition(fromPos) && UTIL_isVirtualItemPosition(toPos));
	noteManualItemInteraction();

	if(g_manualActionQueue.empty() && !shouldDelayManualAction(action.enqueueTime, action.safeWhileWalking))
	{
		UTIL_sendQueuedManualAction(action);
		m_botLastAction = g_frameTime;
		m_botActionCooldown = MANUAL_ACTION_COOLDOWN;
		if(!action.safeWhileWalking)
		{
			m_manualLastAction = g_frameTime;
		}
		return;
	}

	if(g_manualActionQueue.size() >= MANUAL_ACTION_MAX_QUEUE)
		g_manualActionQueue.pop_front();
	g_manualActionQueue.push_back(action);
}

void Engine::updateManualActionQueue()
{
	if(m_manualActionResumeWalk)
	{
		Creature* player = g_map.getLocalCreature();
		if((m_manualLastAction == 0 || g_frameTime - m_manualLastAction >= MANUAL_ACTION_COOLDOWN) && (!player || (!player->isWalking() && !player->isPreWalking())))
		{
			m_manualActionPausedWalk = false;
			m_manualActionResumeWalk = false;
			g_game.checkLocalCreatureMovement();
		}
		return;
	}

	if(g_manualActionQueue.empty())
		return;

	QueuedManualAction& action = g_manualActionQueue.front();
	if(shouldDelayManualAction(action.enqueueTime, action.safeWhileWalking))
		return;

	// Drain one action per update to avoid a burst of delayed packets after a
	// walk step finishes.
	bool safeWhileWalking = action.safeWhileWalking;
	UTIL_sendQueuedManualAction(action);
	g_manualActionQueue.pop_front();
	m_botLastAction = g_frameTime;
	m_botActionCooldown = MANUAL_ACTION_COOLDOWN;
	if(!safeWhileWalking)
	{
		m_manualLastAction = g_frameTime;
	}
	if(m_manualActionPausedWalk && !safeWhileWalking)
		m_manualActionResumeWalk = true;
}

bool Engine::sendBotSay(MessageMode mode, Uint16 channelId, const std::string& receiver, const std::string& message, Uint32 cooldownMs, Uint32 movementBlockMs)
{
	if(!canRunBotAction(movementBlockMs))
		return false;

	g_game.sendSay(mode, channelId, receiver, message);
	m_botLastAction = g_frameTime;
	m_botActionCooldown = cooldownMs;
	return true;
}

bool Engine::sendBotMove(const Position& fromPos, Uint16 itemid, Uint8 stackpos, const Position& toPos, Uint16 count, Uint32 cooldownMs, Uint32 movementBlockMs)
{
	if(!canRunBotAction(movementBlockMs))
		return false;

	g_game.sendMove(fromPos, itemid, stackpos, toPos, count);
	m_botLastAction = g_frameTime;
	m_botActionCooldown = cooldownMs;
	return true;
}

bool Engine::sendBotUseItem(const Position& position, Uint16 itemId, Uint8 stackpos, Uint8 index, Uint32 cooldownMs, Uint32 movementBlockMs)
{
	if(!canRunBotAction(movementBlockMs))
		return false;

	g_game.sendUseItem(position, itemId, stackpos, index);
	m_botLastAction = g_frameTime;
	m_botActionCooldown = cooldownMs;
	return true;
}

bool Engine::sendBotUseItemEx(const Position& fromPos, Uint16 itemId, Uint8 fromStackPos, const Position& toPos, Uint16 toItemId, Uint8 toStackPos, Uint32 cooldownMs, Uint32 movementBlockMs)
{
	if(!canRunBotAction(movementBlockMs))
		return false;

	g_game.sendUseItemEx(fromPos, itemId, fromStackPos, toPos, toItemId, toStackPos);
	m_botLastAction = g_frameTime;
	m_botActionCooldown = cooldownMs;
	return true;
}

bool Engine::sendBotUseOnCreature(const Position& position, Uint16 itemId, Uint8 stackpos, Uint32 creatureId, Uint32 cooldownMs, Uint32 movementBlockMs)
{
	if(!canRunBotAction(movementBlockMs))
		return false;

	g_game.sendUseOnCreature(position, itemId, stackpos, creatureId);
	m_botLastAction = g_frameTime;
	m_botActionCooldown = cooldownMs;
	return true;
}

void Engine::update()
{
	if(m_ingame)
	{
		updateNumpadDash();
		g_map.update();
		updateManualActionQueue();
		updateManaTrainer();
		updateAutoHealer();
		updateAutoFisher();
		updateAutoRuner();
		updateAutoLogout();
		updateAutoEater();
	}
}

void Engine::startNumpadDash(SDL_Keycode key, Direction direction)
{
	if(m_numpadDashActive && m_numpadDashKey == key && m_numpadDashDirection == direction)
		return;

	m_numpadDashActive = true;
	m_numpadDashKey = key;
	m_numpadDashDirection = SDL_static_cast(Uint8, direction);
	m_numpadDashLastPulse = 0;
	m_numpadDashLastTileChange = g_frameTime;
	Creature* player = g_map.getLocalCreature();
	if(player)
	{
		const Position& pos = player->getCurrentPosition();
		m_numpadDashLastX = pos.x;
		m_numpadDashLastY = pos.y;
		m_numpadDashLastZ = pos.z;
	}
}

void Engine::stopNumpadDash(SDL_Keycode key)
{
	if(!m_numpadDashActive || m_numpadDashKey != key)
		return;

	m_numpadDashActive = false;
	m_numpadDashKey = SDLK_UNKNOWN;
	m_numpadDashDirection = DIRECTION_INVALID;
	m_numpadDashLastPulse = 0;
	m_numpadDashLastTileChange = 0;
	m_numpadDashLastX = 0;
	m_numpadDashLastY = 0;
	m_numpadDashLastZ = 0;
}

void Engine::updateNumpadDash()
{
	static const Uint32 NUMPAD_DASH_PULSE_MS = 50;
	static const Uint32 NUMPAD_DASH_STAND_TIMEOUT = 2000;
	if(!m_numpadDashActive || m_numpadDashDirection == DIRECTION_INVALID)
		return;

	if(m_contextMenu || m_showLogger || m_actWindow)
		return;

	SDL_Scancode scancode = SDL_GetScancodeFromKey(m_numpadDashKey);
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	if(scancode == SDL_SCANCODE_UNKNOWN || !keyboardState || !keyboardState[scancode])
	{
		stopNumpadDash(m_numpadDashKey);
		g_game.releaseMovement();
		return;
	}

	Creature* player = g_map.getLocalCreature();
	if(!player)
		return;

	const Position& pos = player->getCurrentPosition();
	if(pos.x != m_numpadDashLastX || pos.y != m_numpadDashLastY || pos.z != m_numpadDashLastZ)
	{
		m_numpadDashLastX = pos.x;
		m_numpadDashLastY = pos.y;
		m_numpadDashLastZ = pos.z;
		m_numpadDashLastTileChange = g_frameTime;
	}
	else if(m_numpadDashLastTileChange != 0 && g_frameTime - m_numpadDashLastTileChange > NUMPAD_DASH_STAND_TIMEOUT)
	{
		stopNumpadDash(m_numpadDashKey);
		g_game.releaseMovement();
		return;
	}

	if(m_numpadDashLastPulse != 0 && g_frameTime - m_numpadDashLastPulse < NUMPAD_DASH_PULSE_MS)
		return;

	notePlayerMovementAction(250);
	g_game.checkMovement(SDL_static_cast(Direction, m_numpadDashDirection), false);
	m_numpadDashLastPulse = g_frameTime;
}

void Engine::updateManaTrainer()
{
	if(!m_manaTrainer || m_manaTrainerSpell.empty())
		return;

	if(g_game.getPlayerManaPercent() < m_manaTrainerManaPercent)
		return;

	if(m_manaTrainerLastCast != 0 && g_frameTime - m_manaTrainerLastCast < m_manaTrainerDelay)
		return;

	if(!sendBotSay(MessageSay, 0, std::string(), m_manaTrainerSpell))
		return;

	m_manaTrainerLastCast = g_frameTime;
	g_logger.addLog(LOG_CATEGORY_INFO, std::string("[ManaTrainer] cast ").append(m_manaTrainerSpell));
}

static std::vector<Uint8> UTIL_getOpenContainersByOpenOrder();

static ItemUI* UTIL_findAutoHealerItem(Uint16 itemId)
{
	if(itemId == 0)
		return NULL;

	for(Uint8 slot = SLOT_HEAD; slot < SLOT_LAST; ++slot)
	{
		ItemUI* invItem = g_game.getInventoryItem(slot);
		if(invItem && invItem->getID() == itemId)
			return invItem;
	}

	std::vector<Uint8> containers = UTIL_getOpenContainersByOpenOrder();
	for(Uint8 cid : containers)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont)
			continue;

		std::vector<ItemUI*>& items = cont->getItems();
		for(ItemUI* item : items)
		{
			if(item && item->getID() == itemId)
				return item;
		}
	}
	return NULL;
}

static std::string UTIL_autoHealerNormalizeName(std::string value)
{
	while(!value.empty() && (value.front() == ' ' || value.front() == '\t'))
		value.erase(value.begin());
	while(!value.empty() && (value.back() == ' ' || value.back() == '\t'))
		value.pop_back();

	for(char& ch : value)
		ch = SDL_static_cast(char, SDL_tolower(ch));
	return value;
}

static bool UTIL_autoHealerNameListed(const std::string& list, const std::string& name)
{
	const std::string wanted = UTIL_autoHealerNormalizeName(name);
	size_t start = 0;
	while(start < list.length())
	{
		size_t end = list.find_first_of(",;|", start);
		std::string token = UTIL_autoHealerNormalizeName(list.substr(start, end == std::string::npos ? std::string::npos : end - start));
		if(!token.empty() && token == wanted)
			return true;
		if(end == std::string::npos)
			break;
		start = end + 1;
	}
	return false;
}

static Creature* UTIL_findAutoHealerFriend(const std::string& friendNames, Uint8 hpPercent)
{
	if(friendNames.empty())
		return NULL;

	Creature* localCreature = g_map.getLocalCreature();
	if(!localCreature)
		return NULL;

	const Position& myPos = localCreature->getCurrentPosition();
	Creature* best = NULL;
	Sint32 bestDist = INT_MAX;
	Uint8 bestHp = 101;

	const knownCreatures& creatures = g_map.getKnownCreatures();
	for(auto& kv : creatures)
	{
		Creature* creature = kv.second;
		if(!creature || creature->isLocalCreature() || !creature->isPlayer() || creature->getHealth() == 0)
			continue;
		if(creature->getHealth() > hpPercent || !UTIL_autoHealerNameListed(friendNames, creature->getName()))
			continue;

		const Position& pos = creature->getCurrentPosition();
		if(pos.z != myPos.z || !Position::areInRange<7, 5>(myPos, pos))
			continue;

		Sint32 dist = Position::getDistanceX(myPos, pos) + Position::getDistanceY(myPos, pos);
		if(!best || creature->getHealth() < bestHp || (creature->getHealth() == bestHp && dist < bestDist))
		{
			best = creature;
			bestHp = creature->getHealth();
			bestDist = dist;
		}
	}
	return best;
}

void Engine::updateAutoHealer()
{
	if(!m_autoHealer)
		return;

	if(m_autoHealerLastCast != 0 && g_frameTime - m_autoHealerLastCast < m_autoHealerDelay)
		return;

	if(m_autoHealerSelfUH && g_game.getPlayerHealthPercent() <= m_autoHealerSelfUHPercent)
	{
		ItemUI* uh = UTIL_findAutoHealerItem(m_autoHealerUHItemId);
		if(uh)
		{
			Position uhPos = uh->getCurrentPosition();
			if(sendBotUseOnCreature(uhPos, m_autoHealerUHItemId, 0, g_game.getPlayerID(), m_autoHealerDelay, 350))
			{
				m_autoHealerLastCast = g_frameTime;
				g_logger.addLog(LOG_CATEGORY_INFO, std::string("[AutoHealer] self UH"));
			}
			return;
		}
	}

	if(!m_autoHealerSpell.empty() && g_game.getPlayerHealthPercent() <= m_autoHealerHPPercent)
	{
		if(!sendBotSay(MessageSay, 0, std::string(), m_autoHealerSpell, m_autoHealerDelay, 350))
			return;

		m_autoHealerLastCast = g_frameTime;
		g_logger.addLog(LOG_CATEGORY_INFO, std::string("[AutoHealer] cast ").append(m_autoHealerSpell));
		return;
	}

	if(!m_autoHealerFriendUH)
		return;

	Creature* friendCreature = UTIL_findAutoHealerFriend(m_autoHealerFriendNames, m_autoHealerFriendUHPercent);
	if(!friendCreature)
		return;

	ItemUI* uh = UTIL_findAutoHealerItem(m_autoHealerUHItemId);
	if(!uh)
		return;

	Position uhPos = uh->getCurrentPosition();
	if(sendBotUseOnCreature(uhPos, m_autoHealerUHItemId, 0, friendCreature->getId(), m_autoHealerDelay, 350))
	{
		m_autoHealerLastCast = g_frameTime;
		g_logger.addLog(LOG_CATEGORY_INFO, std::string("[AutoHealer] friend UH ").append(friendCreature->getName()));
	}
}

static std::string UTIL_comboLeaderLower(std::string value)
{
	for(char& ch : value)
		ch = SDL_static_cast(char, SDL_tolower(ch));
	return value;
}

static ItemUI* UTIL_findComboLeaderRune(Uint16 itemId)
{
	for(Uint8 slot = SLOT_HEAD; slot < SLOT_LAST; ++slot)
	{
		ItemUI* invItem = g_game.getInventoryItem(slot);
		if(invItem && invItem->getID() == itemId)
			return invItem;
	}

	for(Uint8 cid = 0; cid < GAME_MAX_CONTAINERS; ++cid)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont)
			continue;

		std::vector<ItemUI*>& items = cont->getItems();
		for(ItemUI* item : items)
		{
			if(item && item->getID() == itemId)
				return item;
		}
	}
	return NULL;
}

static Creature* UTIL_findComboLeaderCreatureAt(const Position& position)
{
	const knownCreatures& creatures = g_map.getKnownCreatures();
	for(auto& kv : creatures)
	{
		Creature* creature = kv.second;
		if(!creature || creature->isLocalCreature())
			continue;

		if(creature->getCurrentPosition() == position)
			return creature;
	}
	return NULL;
}

void Engine::handleComboLeaderDistanceEffect(const Position& fromPos, const Position& toPos, Uint16 effectId)
{
	if(!m_comboLeader || m_comboLeaderName.empty() || fromPos.z != toPos.z)
		return;

	std::string wantedLeader = UTIL_comboLeaderLower(m_comboLeaderName);
	bool fromLeader = false;
	const knownCreatures& creatures = g_map.getKnownCreatures();
	for(auto& kv : creatures)
	{
		Creature* creature = kv.second;
		if(!creature || creature->isLocalCreature())
			continue;

		if(UTIL_comboLeaderLower(creature->getName()) != wantedLeader)
			continue;

		const Position& leaderPos = creature->getCurrentPosition();
		if(leaderPos.z == fromPos.z && Position::areInRange<1, 1>(leaderPos, fromPos))
		{
			fromLeader = true;
			break;
		}
	}
	if(!fromLeader)
		return;

	size_t mappingIndex = COMBO_LEADER_MAPPING_COUNT;
	for(size_t index = 0; index < COMBO_LEADER_MAPPING_COUNT; ++index)
	{
		if(m_comboLeaderMappingEffects[index] != 0 && effectId == m_comboLeaderMappingEffects[index])
		{
			mappingIndex = index;
			break;
		}
	}
	if(mappingIndex >= COMBO_LEADER_MAPPING_COUNT)
	{
		char logBuffer[160];
		SDL_snprintf(logBuffer, sizeof(logBuffer), "[ComboLeader] %s effect=%u not mapped", m_comboLeaderName.c_str(), SDL_static_cast(Uint32, effectId));
		g_logger.addLog(LOG_CATEGORY_INFO, std::string(logBuffer));
		return;
	}

	Uint16 runeId = m_comboLeaderMappingRunes[mappingIndex];
	const char* runeName = m_comboLeaderMappingNames[mappingIndex].c_str();
	if(runeId == 0)
		return;
	if(m_comboLeaderLastCast != 0 && g_frameTime - m_comboLeaderLastCast < m_comboLeaderDelay)
		return;
	if(m_actionData == CLIENT_ACTION_MOVEITEM || m_actionData == CLIENT_ACTION_USEWITH ||
	   m_actionData == CLIENT_ACTION_LEFTMOUSE || m_actionData == CLIENT_ACTION_RIGHTMOUSE)
		return;

	ItemUI* runeItem = UTIL_findComboLeaderRune(runeId);
	if(!runeItem)
	{
		char logBuffer[160];
		SDL_snprintf(logBuffer, sizeof(logBuffer), "[ComboLeader] missing %s rune id=%u", runeName, SDL_static_cast(Uint32, runeId));
		g_logger.addLog(LOG_CATEGORY_INFO, std::string(logBuffer));
		return;
	}

	Position& runePos = runeItem->getCurrentPosition();
	Creature* targetCreature = UTIL_findComboLeaderCreatureAt(toPos);
	if(targetCreature)
		g_game.sendUseOnCreature(runePos, runeId, 0, targetCreature->getId());
	else
	{
		Tile* tile = g_map.getTile(toPos);
		if(!tile)
			return;

		Thing* useThing = tile->getTopUseThing();
		if(!useThing || !useThing->isItem())
			return;

		Item* targetItem = useThing->getItem();
		Position& targetPos = targetItem->getCurrentPosition();
		g_game.sendUseItemEx(runePos, runeId, 0, targetPos, targetItem->getID(), SDL_static_cast(Uint8, tile->getThingStackPos(targetItem)));
	}

	m_comboLeaderLastCast = g_frameTime;
	m_botLastAction = g_frameTime;
	m_botActionCooldown = 250;

	char logBuffer[192];
	SDL_snprintf(logBuffer, sizeof(logBuffer), "[ComboLeader] mirrored %s effect=%u rune=%u", runeName, SDL_static_cast(Uint32, effectId), SDL_static_cast(Uint32, runeId));
	g_logger.addLog(LOG_CATEGORY_INFO, std::string(logBuffer));
}

void Engine::updateMagicWall()
{
	if(!m_magicWall || m_magicWallSpell.empty())
		return;

	if(m_magicWallLastCast != 0 && g_frameTime - m_magicWallLastCast < m_magicWallDelay)
		return;

	Uint32 attackId = g_game.getAttackID();
	if(attackId == 0)
		return;

	Creature* target = g_map.getCreatureById(attackId);
	if(!target)
		return;

	Creature* local = g_map.getLocalCreature();
	if(!local)
		return;

	const Position& myPos = local->getCurrentPosition();
	const Position& tPos = target->getCurrentPosition();

	if(tPos.z != myPos.z)
		return;

	Sint32 dx = SDL_abs(SDL_static_cast(Sint32, tPos.x) - SDL_static_cast(Sint32, myPos.x));
	Sint32 dy = SDL_abs(SDL_static_cast(Sint32, tPos.y) - SDL_static_cast(Sint32, myPos.y));
	Sint32 dist = dx + dy;

	if(dist > SDL_static_cast(Sint32, m_magicWallDist))
		return;

	if(!sendBotSay(MessageSay, 0, std::string(), m_magicWallSpell))
		return;

	m_magicWallLastCast = g_frameTime;
	g_logger.addLog(LOG_CATEGORY_INFO, std::string("[MagicWall] cast ").append(m_magicWallSpell));
}

static bool UTIL_findAutoFisherFishInHand(Uint8& handSlot, Uint16& fishCount)
{
	static const Uint16 fishId = 3578;
	static const Uint8 handSlots[] = {SLOT_RIGHT, SLOT_LEFT};
	for(Uint8 slot : handSlots)
	{
		ItemUI* item = g_game.getInventoryItem(slot);
		if(item && item->getID() == fishId)
		{
			handSlot = slot;
			fishCount = UTIL_max<Uint16>(1, item->getItemCount());
			return true;
		}
	}
	return false;
}

static std::vector<Uint8> UTIL_getOpenContainersByOpenOrder()
{
	std::vector<Uint8> ordered;
	for(Uint8 cid = 0; cid < GAME_MAX_CONTAINERS; ++cid)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont)
			continue;

		std::vector<Uint8>::iterator it = ordered.begin();
		while(it != ordered.end())
		{
			Container* other = g_game.findContainer(*it);
			if(!other || other->getOpenOrder() > cont->getOpenOrder() ||
			   (other->getOpenOrder() == cont->getOpenOrder() && *it > cid))
				break;

			++it;
		}
		ordered.insert(it, cid);
	}
	return ordered;
}

// Right-click coin stacking: move gold/platinum/crystal into an open
// backpack (2854) or bag (2853). This is the manual quality-of-life loot;
// the old automatic corpse-scanning looter was removed.
static const Uint16 AUTO_LOOTER_BACKPACK_ID = 2854;
static const Uint16 AUTO_LOOTER_BAG_ID = 2853;

static bool UTIL_isAutoLooterGoldId(Uint16 itemId)
{
	return itemId == 3031 || itemId == 3035 || itemId == 3043;
}

static bool UTIL_isAutoLooterDestId(Uint16 itemId)
{
	return itemId == AUTO_LOOTER_BACKPACK_ID || itemId == AUTO_LOOTER_BAG_ID;
}

static bool UTIL_findAutoLooterGoldDestination(Uint16 itemId, Uint16 sourceCount, Position& destPos, Uint16& moveCount, const Position* excludedPos = NULL)
{
	std::vector<Uint8> containers = UTIL_getOpenContainersByOpenOrder();

	// First pass: prefer partial stack of same coin type
	for(Uint8 cid : containers)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont || !cont->isUnlocked())
			continue;

		ItemUI* containerItem = cont->getItem();
		if(!containerItem || !UTIL_isAutoLooterDestId(containerItem->getID()))
			continue;

		std::vector<ItemUI*>& items = cont->getItems();
		for(ItemUI* item : items)
		{
			if(!item || item->getID() != itemId || item->getItemCount() >= 100)
				continue;

			if(excludedPos && item->getCurrentPosition() == *excludedPos)
				continue;

			destPos = item->getCurrentPosition();
			moveCount = UTIL_min<Uint16>(UTIL_max<Uint16>(1, sourceCount), SDL_static_cast(Uint16, 100 - item->getItemCount()));
			return true;
		}
	}

	// Second pass: empty slot in any backpack/bag destination
	for(Uint8 cid : containers)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont || !cont->isUnlocked())
			continue;

		ItemUI* containerItem = cont->getItem();
		if(!containerItem || !UTIL_isAutoLooterDestId(containerItem->getID()))
			continue;

		std::vector<ItemUI*>& items = cont->getItems();
		if(items.size() >= SDL_static_cast(size_t, cont->getCapacity()))
			continue;

		Position emptyPos = Position(0xFFFF, SDL_static_cast(Uint16, cid | 0x40), SDL_static_cast(Uint8, items.size()));
		if(excludedPos && *excludedPos == emptyPos)
			continue;

		destPos = emptyPos;
		moveCount = sourceCount;
		return true;
	}

	return false;
}

static bool UTIL_tryAutoStackCoin(ItemUI* itemui)
{
	if(!itemui || !UTIL_isAutoLooterGoldId(itemui->getID()))
		return false;

	Position fromPos = itemui->getCurrentPosition();
	Position destPos;
	Uint16 moveCount = 0;
	if(!UTIL_findAutoLooterGoldDestination(itemui->getID(), itemui->getItemCount(), destPos, moveCount, &fromPos))
		return false;

	g_engine.sendManualMove(fromPos, itemui->getID(), 0, destPos, moveCount);

	char logBuffer[128];
	SDL_snprintf(logBuffer, sizeof(logBuffer), "[AutoStack] moved coin id=%u count=%u", SDL_static_cast(Uint32, itemui->getID()), SDL_static_cast(Uint32, moveCount));
	g_logger.addLog(LOG_CATEGORY_INFO, std::string(logBuffer));
	return true;
}

static bool UTIL_findAutoFisherFishDestination(Uint16 fishCount, Position& destPos, Uint16& moveCount)
{
	static const Uint16 fishId = 3578;
	std::vector<Uint8> containers = UTIL_getOpenContainersByOpenOrder();
	for(Uint8 cid : containers)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont || !cont->isUnlocked())
			continue;

		bool containerHasFish = false;
		std::vector<ItemUI*>& items = cont->getItems();
		for(ItemUI* item : items)
		{
			if(!item || item->getID() != fishId)
				continue;

			containerHasFish = true;
			if(item->getItemCount() < 100)
			{
				destPos = item->getCurrentPosition();
				moveCount = UTIL_min<Uint16>(fishCount, SDL_static_cast(Uint16, 100 - item->getItemCount()));
				return true;
			}
		}

		if(containerHasFish && items.size() < SDL_static_cast(size_t, cont->getCapacity()))
		{
			destPos = Position(0xFFFF, (SDL_static_cast(Uint16, cid) | 0x40), SDL_static_cast(Uint8, items.size()));
			moveCount = fishCount;
			return true;
		}
	}

	for(Uint8 cid : containers)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont || !cont->isUnlocked())
			continue;

		std::vector<ItemUI*>& items = cont->getItems();
		if(items.size() >= SDL_static_cast(size_t, cont->getCapacity()))
			continue;

		destPos = Position(0xFFFF, (SDL_static_cast(Uint16, cid) | 0x40), SDL_static_cast(Uint8, items.size()));
		moveCount = fishCount;
		return true;
	}
	return false;
}

static bool UTIL_moveAutoFisherFishFromHand(Uint8 handSlot, Uint16 fishCount)
{
	static const Uint16 fishId = 3578;
	Position destPos;
	Uint16 moveCount = 1;
	if(!UTIL_findAutoFisherFishDestination(fishCount, destPos, moveCount))
		return false;

	Position fromPos(0xFFFF, SDL_static_cast(Uint16, handSlot) + 1, 0);
	return g_engine.sendBotMove(fromPos, fishId, 0, destPos, moveCount, 1000, 350);
}

static Uint32 UTIL_rollAutoFisherCastDelay(Uint32 minDelay, Uint32 maxDelay)
{
	minDelay = UTIL_min<Uint32>(30000, minDelay);
	maxDelay = UTIL_min<Uint32>(30000, maxDelay);
	if(maxDelay < minDelay)
		maxDelay = minDelay;

	// The UI may store a 0ms lower bound, but the bot must keep a real 1s action gap.
	minDelay = UTIL_max<Uint32>(1000, minDelay);
	maxDelay = UTIL_max<Uint32>(minDelay, maxDelay);
	if(minDelay == maxDelay)
		return minDelay;

	return SDL_static_cast(Uint32, UTIL_random(SDL_static_cast(Sint32, minDelay), SDL_static_cast(Sint32, maxDelay)));
}

void Engine::updateAutoFisher()
{
	if(!m_autoFisher)
		return;

	Uint8 fishHandSlot = SLOT_RIGHT;
	Uint16 fishCount = 1;
	if(UTIL_findAutoFisherFishInHand(fishHandSlot, fishCount))
	{
		if(m_autoFisherLastFishMove == 0 || g_frameTime - m_autoFisherLastFishMove >= 1000)
		{
			if(UTIL_moveAutoFisherFishFromHand(fishHandSlot, fishCount))
				m_autoFisherLastFishMove = g_frameTime;
		}
		return;
	}
	m_autoFisherLastFishMove = 0;

	if(m_autoFisherLastCast == 0)
	{
		m_autoFisherLastCast = g_frameTime;
		m_autoFisherNextCastDelay = UTIL_rollAutoFisherCastDelay(m_autoFisherDelayMin, m_autoFisherDelayMax);
	}
	if(g_frameTime - m_autoFisherLastCast < m_autoFisherNextCastDelay)
		return;

	Creature* localCreature = g_map.getLocalCreature();
	if(!localCreature)
		return;

	ItemUI* rodItem = g_game.getInventoryItem(SLOT_AMMO);
	if(!rodItem || rodItem->getID() != 3483)
		return;
	Uint8 rodSlot = SLOT_AMMO;

	Position playerPos = localCreature->getCurrentPosition();
	static const Sint32 maxFishingRangeX = 6;
	static const Sint32 maxFishingRangeY = 5;
	static const Uint16 waterIds[] = {618, 620, 788, 795, 4597, 4598, 4599, 4600, 4601};
	struct AutoFisherTarget
	{
		Position pos;
		Uint16 itemId;
		Uint8 stackPos;
	};
	std::vector<AutoFisherTarget> targets;

	for(Sint32 range = 1; range <= maxFishingRangeX; ++range)
	{
		for(Sint32 dx = -range; dx <= range; ++dx)
		{
			Sint32 yRange = UTIL_min<Sint32>(range, maxFishingRangeY);
			for(Sint32 dy = -yRange; dy <= yRange; ++dy)
			{
				if(dx == 0 && dy == 0)
					continue;
				if(SDL_abs(dx) != range && SDL_abs(dy) != range)
					continue;

				Sint32 targetX = SDL_static_cast(Sint32, playerPos.x) + dx;
				Sint32 targetY = SDL_static_cast(Sint32, playerPos.y) + dy;
				if(targetX < 0 || targetY < 0)
					continue;

				Position fishPos(SDL_static_cast(Uint16, targetX), SDL_static_cast(Uint16, targetY), playerPos.z);
				Tile* tile = g_map.getTile(fishPos);
				if(!tile)
					continue;

				Thing* ground = tile->getThingByStackPos(0);
				if(!ground || !ground->isItem())
					continue;

				Uint16 groundId = SDL_static_cast(Item*, ground)->getID();
				bool isWater = false;
				for(Uint16 wid : waterIds)
				{
					if(groundId == wid) { isWater = true; break; }
				}
				if(!isWater)
					continue;

				Thing* useThing = tile->getTopUseThing();
				if(!useThing || !useThing->isItem())
					continue;

				Item* useItem = useThing->getItem();
				Uint16 useItemId = useItem->getID();
				bool isFishableUseItem = false;
				for(Uint16 wid : waterIds)
				{
					if(useItemId == wid) { isFishableUseItem = true; break; }
				}
				if(!isFishableUseItem)
					continue;

				AutoFisherTarget target = {fishPos, useItemId, SDL_static_cast(Uint8, tile->getThingStackPos(useItem))};
				targets.push_back(target);
			}
		}
	}

	if(targets.empty())
		return;

	size_t targetIndex = m_autoFisherTargetIndex % targets.size();
	AutoFisherTarget& target = targets[targetIndex];
	m_autoFisherTargetIndex = (targetIndex + 1) % targets.size();

	Position rodPos(0xFFFF, SDL_static_cast(Uint16, rodSlot) + 1, 0);
	if(g_engine.sendBotUseItemEx(rodPos, rodItem->getID(), 0, target.pos, target.itemId, target.stackPos, 1000, 350))
	{
		m_autoFisherLastCast = g_frameTime;
		m_autoFisherNextCastDelay = UTIL_rollAutoFisherCastDelay(m_autoFisherDelayMin, m_autoFisherDelayMax);
	}
}

static bool UTIL_findAutoRunerBlankRuneInHand(Uint8& slot)
{
	static const Uint8 blankSlots[] = {SLOT_RIGHT, SLOT_LEFT};
	for(Uint8 handSlot : blankSlots)
	{
		ItemUI* item = g_game.getInventoryItem(handSlot);
		if(item && item->getID() == 3147)
		{
			slot = handSlot;
			return true;
		}
	}
	return false;
}

static bool UTIL_findAutoRunerFreeHand(Uint8& slot)
{
	static const Uint8 handSlots[] = {SLOT_RIGHT, SLOT_LEFT};
	for(Uint8 handSlot : handSlots)
	{
		if(!g_game.getInventoryItem(handSlot))
		{
			slot = handSlot;
			return true;
		}
	}
	return false;
}

static bool UTIL_moveAutoRunerBlankRuneToHand(Uint8 handSlot, Uint8& sourceCid, Uint8& sourceSlot)
{
	std::vector<Uint8> containers = UTIL_getOpenContainersByOpenOrder();
	for(Uint8 cid : containers)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont || !cont->isUnlocked())
			continue;

		std::vector<ItemUI*>& items = cont->getItems();
		for(size_t j = 0; j < items.size(); ++j)
		{
			ItemUI* item = items[j];
			if(!item || item->getID() != 3147)
				continue;

			Position fromPos = item->getCurrentPosition();
			Position toPos(0xFFFF, SDL_static_cast(Uint16, handSlot) + 1, 0);
			if(g_engine.sendBotMove(fromPos, item->getID(), 0, toPos, 1, 450, 350))
			{
				sourceCid = cid;
				sourceSlot = fromPos.z;
				return true;
			}
			return false;
		}
	}
	return false;
}

static bool UTIL_moveAutoRunerHandItemToContainerSlot(Uint8 handSlot, Uint16 itemId, Uint8 cid, Uint8 slot, bool useSlot)
{
	Container* cont = g_game.findContainer(cid);
	if(!cont || !cont->isUnlocked())
		return false;

	std::vector<ItemUI*>& items = cont->getItems();
	if(items.size() >= SDL_static_cast(size_t, cont->getCapacity()))
		return false;

	Uint8 destSlot = SDL_static_cast(Uint8, items.size());
	if(useSlot && SDL_static_cast(size_t, slot) <= items.size() && slot < cont->getCapacity())
		destSlot = slot;

	Position fromPos(0xFFFF, SDL_static_cast(Uint16, handSlot) + 1, 0);
	Position toPos(0xFFFF, (SDL_static_cast(Uint16, cid) | 0x40), destSlot);
	return g_engine.sendBotMove(fromPos, itemId, 0, toPos, 1, 450, 350);
}

static bool UTIL_moveAutoRunerHandItemToContainer(Uint8 handSlot, Uint16 itemId, bool hasPreferred, Uint8 preferredCid, Uint8 preferredSlot)
{
	if(hasPreferred && UTIL_moveAutoRunerHandItemToContainerSlot(handSlot, itemId, preferredCid, preferredSlot, true))
		return true;

	std::vector<Uint8> containers = UTIL_getOpenContainersByOpenOrder();
	for(Uint8 cid : containers)
	{
		if(hasPreferred && cid == preferredCid)
			continue;

		if(UTIL_moveAutoRunerHandItemToContainerSlot(handSlot, itemId, cid, 0, false))
			return true;
	}
	return false;
}

void Engine::updateAutoRuner()
{
	if(!m_autoRuner || m_autoRunerSpell.empty())
		return;

	if(m_autoRunerPendingMove)
	{
		ItemUI* item = g_game.getInventoryItem(m_autoRunerPendingSlot);
		if(!item)
		{
			m_autoRunerPendingMove = false;
			m_autoRunerPendingMoveTime = 0;
			m_autoRunerHasReturnContainer = false;
			return;
		}

		if(item->getID() == 3147)
		{
			if(g_frameTime - m_autoRunerLastCast < 2000)
				return;

			m_autoRunerPendingMove = false;
			m_autoRunerPendingMoveTime = 0;
		}
		else
		{
			if(m_autoRunerPendingMoveTime != 0 && g_frameTime - m_autoRunerPendingMoveTime < 500)
				return;

			if(UTIL_moveAutoRunerHandItemToContainer(m_autoRunerPendingSlot, item->getID(), m_autoRunerHasReturnContainer, m_autoRunerReturnContainer, m_autoRunerReturnSlot))
				m_autoRunerPendingMoveTime = g_frameTime;

			return;
		}
	}

	if(m_autoRunerPendingBlankMove)
	{
		ItemUI* item = g_game.getInventoryItem(m_autoRunerPendingBlankSlot);
		if(item)
		{
			m_autoRunerPendingBlankMove = false;
			if(item->getID() != 3147)
				m_autoRunerHasReturnContainer = false;
			return;
		}

		if(g_frameTime - m_autoRunerPendingBlankMoveTime < 2000)
			return;

		m_autoRunerPendingBlankMove = false;
		m_autoRunerHasReturnContainer = false;
	}

	if(m_autoRunerLastCast != 0 && g_frameTime - m_autoRunerLastCast < m_autoRunerDelay)
		return;

	Creature* localCreature = g_map.getLocalCreature();
	if(!localCreature)
		return;

	if(g_game.getPlayerManaPercent() < m_autoRunerManaPercent)
		return;

	Uint8 blankSlot = SLOT_RIGHT;
	if(!UTIL_findAutoRunerBlankRuneInHand(blankSlot))
	{
		Uint8 freeSlot = SLOT_RIGHT;
		if(!UTIL_findAutoRunerFreeHand(freeSlot))
			return;

		Uint8 sourceCid = 0;
		Uint8 sourceSlot = 0;
		if(UTIL_moveAutoRunerBlankRuneToHand(freeSlot, sourceCid, sourceSlot))
		{
			m_autoRunerPendingBlankMove = true;
			m_autoRunerPendingBlankSlot = freeSlot;
			m_autoRunerPendingBlankMoveTime = g_frameTime;
			m_autoRunerHasReturnContainer = true;
			m_autoRunerReturnContainer = sourceCid;
			m_autoRunerReturnSlot = sourceSlot;
		}
		return;
	}

	if(!sendBotSay(MessageSay, 0, std::string(), m_autoRunerSpell, 500, 350))
		return;

	m_autoRunerLastCast = g_frameTime;
	m_autoRunerPendingMove = true;
	m_autoRunerPendingSlot = blankSlot;
	m_autoRunerPendingMoveTime = 0;
}

void Engine::updateAutoEater()
{
	if(!m_autoEater || m_autoEaterFoodId == 0)
		return;

	if(m_autoEaterLastUse != 0 && g_frameTime - m_autoEaterLastUse < m_autoEaterDelay)
		return;

	for(Uint8 cid = 0; cid < GAME_MAX_CONTAINERS; ++cid)
	{
		Container* cont = g_game.findContainer(cid);
		if(!cont) continue;
		std::vector<ItemUI*>& items = cont->getItems();
		for(size_t j = 0; j < items.size(); ++j)
		{
			ItemUI* food = items[j];
			if(!food || food->getID() != m_autoEaterFoodId)
				continue;

			Position pos = food->getCurrentPosition();
			Uint8 index = SDL_static_cast(Uint8, pos.y & 0x0F);
			if(g_engine.sendBotUseItem(pos, m_autoEaterFoodId, 0, index, 500, 350))
				m_autoEaterLastUse = g_frameTime;
			return;
		}
	}
}

enum CharacterWebsiteStatus
{
	CHARACTER_WEBSITE_UNKNOWN,
	CHARACTER_WEBSITE_OFFLINE,
	CHARACTER_WEBSITE_ONLINE
};

static std::string lowerAscii(std::string value)
{
	for(char& ch : value)
		if(ch >= 'A' && ch <= 'Z')
			ch = SDL_static_cast(char, ch + ('a' - 'A'));
	return value;
}

static CharacterWebsiteStatus extractCharacterWebsiteStatus(const std::string& body, const std::string& playerName)
{
	const std::string lowerBody = lowerAscii(body);
	const std::string nameTag = "<b>" + lowerAscii(playerName) + "</b>";
	const size_t namePos = lowerBody.find(nameTag);
	if(namePos == std::string::npos)
		return CHARACTER_WEBSITE_UNKNOWN;

	// myAAC puts the character's online state in the color of this exact name.
	const size_t colorPos = lowerBody.rfind("color:", namePos);
	if(colorPos == std::string::npos || namePos - colorPos > 160)
		return CHARACTER_WEBSITE_UNKNOWN;

	const std::string colorFragment = lowerBody.substr(colorPos, namePos - colorPos);
	if(colorFragment.find("green") != std::string::npos)
		return CHARACTER_WEBSITE_ONLINE;
	if(colorFragment.find("red") != std::string::npos)
		return CHARACTER_WEBSITE_OFFLINE;
	return CHARACTER_WEBSITE_UNKNOWN;
}

static std::string extractLastLogin(const std::string& body)
{
	const std::string lower = lowerAscii(body);
	const size_t labelPos = lower.find("last login");
	if(labelPos == std::string::npos)
		return std::string();

	const size_t td = lower.find("<td", labelPos);
	if(td == std::string::npos)
		return std::string();

	const size_t tdClose = lower.find('>', td);
	const size_t tdEnd = lower.find("</td>", td);
	if(tdClose == std::string::npos || tdEnd == std::string::npos || tdEnd <= tdClose + 1)
		return std::string();
	return body.substr(tdClose + 1, tdEnd - tdClose - 1);
}

static bool readHttpResultFile(const char* fileName, std::string& body)
{
	SDL_RWops* fp = SDL_RWFromFile(fileName, "rb");
	if(!fp) return false;
	const Sint64 size = SDL_RWsize(fp);
	if(size <= 0)
	{
		SDL_RWclose(fp);
		return false;
	}

	body.assign(SDL_static_cast(size_t, size), '\0');
	SDL_RWread(fp, &body[0], 1, SDL_static_cast(size_t, size));
	SDL_RWclose(fp);
	return true;
}

static void gmCheckCallback(Uint32 gmIndex, Sint32 requestId)
{
	HttpRequest* request = g_http.getRequest(SDL_static_cast(Uint32, requestId));
	if(!request || request->result != HTTP_RESULT_SUCCEEDED)
		return;

	char fname[32];
	SDL_snprintf(fname, sizeof(fname), "gmcheck_%u.dat", gmIndex);
	std::string body;
	if(!readHttpResultFile(fname, body))
		return;

	const std::vector<std::string>& gms = g_engine.getAutoLogoutGMs();
	size_t idx = SDL_static_cast(size_t, gmIndex);
	if(idx >= gms.size()) return;

	const std::string lastLogin = extractLastLogin(body);
	if(lastLogin.empty())
		return;

	if(!g_engine.isGmBaselineSet(idx))
	{
		g_engine.setGmBaseline(idx, lastLogin);
		g_game.processTextMessage(MessageLook, "[GM Check] " + gms[idx] + ": Last Login baseline captured (HTTP OK)");
		return;
	}

	if(g_engine.getGmBaseline(idx) != lastLogin)
	{
		g_engine.setGmBaseline(idx, lastLogin);
		const std::string reason = "GM Last Login changed: " + gms[idx];
		g_game.processTextMessage(MessageLook, "[GM Check] " + gms[idx] + ": Last Login changed; logging out");
		g_engine.doAutoLogout(reason.c_str());
	}
}

static void playerCheckCallback(Uint32 playerIndex, Sint32 requestId)
{
	HttpRequest* request = g_http.getRequest(SDL_static_cast(Uint32, requestId));
	if(!request || request->result != HTTP_RESULT_SUCCEEDED)
		return;

	char fname[40];
	SDL_snprintf(fname, sizeof(fname), "playercheck_%u.dat", playerIndex);
	std::string body;
	if(!readHttpResultFile(fname, body))
		return;

	const std::vector<std::string>& players = g_engine.getAutoLogoutPlayers();
	const size_t idx = SDL_static_cast(size_t, playerIndex);
	if(idx >= players.size())
		return;

	const CharacterWebsiteStatus status = extractCharacterWebsiteStatus(body, players[idx]);
	if(status == CHARACTER_WEBSITE_UNKNOWN)
		return;

	const std::string statusText = (status == CHARACTER_WEBSITE_ONLINE ? "ONLINE" : "OFFLINE");
	if(!g_engine.isPlayerWebStatusSet(idx) || g_engine.getPlayerWebStatus(idx) != statusText)
	{
		g_engine.setPlayerWebStatus(idx, statusText);
		g_game.processTextMessage(MessageLook, "[Player Check] " + players[idx] + ": " + statusText + " (HTTP OK)");
	}

	if(status == CHARACTER_WEBSITE_ONLINE)
		g_engine.doAutoLogout("Watched player online");
}

static const char* friendlyLogoutReason(const std::string& reason)
{
	if(reason == "PK skull detected")    return "PK detected";
	if(reason == "Player nearby")        return "Player detected";
	if(reason == "GM detected")          return "GM detected";
	if(reason == "Watched player online") return "Watched player online";
	if(reason == "Timer expired")        return "Timer";
	return reason.c_str();
}

void Engine::doAutoLogout(const char* reason)
{
	if(!m_ingame) return;

	std::string reasonText = (reason ? reason : "Auto Logout");
	if(!m_autoLogoutCaptureTaken)
	{
		m_autoLogoutCaptureTaken = true;
		captureAlertEvent(std::string("Auto Logout - ").append(friendlyLogoutReason(reasonText)));
	}

	bool retryUntilAccepted = (reasonText == "PK skull detected" || reasonText == "Player nearby");
	if(retryUntilAccepted)
	{
		if(!m_autoLogoutPending)
		{
			m_autoLogoutPending = true;
			m_autoLogoutPendingReason = reasonText;
			g_logger.addLog(LOG_CATEGORY_INFO, std::string("[AutoLogout] ").append(reasonText).append(" - retrying logout until accepted"));
		}
		else if(m_autoLogoutPendingReason.empty())
			m_autoLogoutPendingReason = reasonText;

		if(m_autoLogoutLastAttempt != 0 && g_frameTime - m_autoLogoutLastAttempt < 1000)
			return;

		if(m_autoLogoutLastLog == 0 || g_frameTime - m_autoLogoutLastLog >= 5000)
		{
			std::string msg = std::string("Logged out, because: ").append(friendlyLogoutReason(m_autoLogoutPendingReason));
			g_game.processTextMessage(MessageWarning, msg);
			g_logger.addLog(LOG_CATEGORY_INFO, std::string("[AutoLogout] logout attempt: ").append(m_autoLogoutPendingReason));
			m_autoLogoutLastLog = g_frameTime;
		}
		m_autoLogoutLastAttempt = g_frameTime;
		g_game.sendLogout();
		return;
	}

	std::string msg = std::string("Logged out, because: ").append(friendlyLogoutReason(reasonText));
	g_game.processTextMessage(MessageWarning, msg);
	g_logger.addLog(LOG_CATEGORY_INFO, std::string("[AutoLogout] ").append(reasonText));
	m_autoLogout = false;
	g_game.sendLogout();
}

void Engine::updateAutoLogout()
{
	if(!m_autoLogout || !m_ingame)
		return;

	// Session start tracking
	if(m_autoLogoutSessionStart == 0)
		m_autoLogoutSessionStart = g_frameTime;

	if(m_autoLogoutPending)
	{
		doAutoLogout(m_autoLogoutPendingReason.c_str());
		return;
	}

	// Timer
	if(m_autoLogoutTimerMin > 0 && g_frameTime - m_autoLogoutSessionStart > m_autoLogoutTimerMin * 60000)
	{
		doAutoLogout("Timer expired");
		return;
	}

	// Player / PK scan
	if(m_autoLogoutPlayer || m_autoLogoutPK)
	{
		Uint32 localId = g_game.getPlayerID();
		Creature* localCreature = g_map.getLocalCreature();
		Position localPos = (localCreature ? localCreature->getCurrentPosition() : Position(0, 0, 0));

		const knownCreatures& creatures = g_map.getKnownCreatures();
		for(auto& kv : creatures)
		{
			Creature* c = kv.second;
			if(!c || kv.first == localId) continue;

			// Nearby means the creature is still in the visible map window, not
			// just present in the known-creature cache from an older packet.
			Position cpos = c->getCurrentPosition();
			Sint32 dz = SDL_static_cast(Sint32, cpos.z) - SDL_static_cast(Sint32, localPos.z);
			if(dz < -2 || dz > 2) continue;
			if(!Position::areInRange<8, 6>(cpos, localPos)) continue;

			bool isPlayerCreature = c->isPlayer();
			if(!isPlayerCreature && kv.first < 0x40000000)
				isPlayerCreature = true;

			if(m_autoLogoutPK && isPlayerCreature && c->getSkull() != SKULL_NONE)
			{
				if(m_playerAlarmEnabled && !m_autoLogoutPending)
					MessageBeep(MB_ICONEXCLAMATION);
				doAutoLogout("PK skull detected");
				return;
			}
			if(m_autoLogoutPlayer && isPlayerCreature)
			{
				if(m_playerAlarmEnabled && !m_autoLogoutPending)
					MessageBeep(MB_ICONEXCLAMATION);
				doAutoLogout("Player nearby");
				return;
			}
		}
	}

	// "on Amera" in creature name — GM in-game detection
	{
		const knownCreatures& creatures = g_map.getKnownCreatures();
		for(const auto& kv : creatures)
		{
			Creature* c = kv.second;
			if(!c) continue;
			const std::string& cname = c->getName();
			std::string lower = cname;
			for(char& ch : lower)
				if(ch >= 'A' && ch <= 'Z') ch = SDL_static_cast(char, ch + ('a' - 'A'));
			if(lower.find("on amera") != std::string::npos)
			{
				doAutoLogout("GM detected");
				return;
			}
		}
	}

	for(const std::string& playerName : m_autoLogoutPlayers)
	{
		if(UTIL_isVipPlayerOnline(playerName))
		{
			doAutoLogout("Watched player online");
			return;
		}
	}

	// Players use VIP/green-red state; hidden GMs retain Last Login change detection.
	if((!m_autoLogoutGMs.empty() || !m_autoLogoutPlayers.empty()) &&
	   (m_lastGmPoll == 0 || g_frameTime - m_lastGmPoll >= m_gmPollInterval))
	{
		m_lastGmPoll = g_frameTime;
		for(size_t i = 0; i < m_autoLogoutGMs.size(); ++i)
		{
			const std::string& gmName = m_autoLogoutGMs[i];
			std::string urlEncoded;
			for(char ch : gmName)
				urlEncoded += (ch == ' ' ? '+' : ch);
			char fname[32];
			SDL_snprintf(fname, sizeof(fname), "gmcheck_%d.dat", SDL_static_cast(Sint32, i));
			std::string url = "http://135.148.27.135/characters?name=" + urlEncoded;
			g_http.addRequest(url, fname, "", &gmCheckCallback, SDL_static_cast(Uint32, i));
		}
		for(size_t i = 0; i < m_autoLogoutPlayers.size(); ++i)
		{
			const std::string& playerName = m_autoLogoutPlayers[i];
			std::string urlEncoded;
			for(char ch : playerName)
				urlEncoded += (ch == ' ' ? '+' : ch);
			char fname[40];
			SDL_snprintf(fname, sizeof(fname), "playercheck_%d.dat", SDL_static_cast(Sint32, i));
			const std::string url = "http://135.148.27.135/characters?name=" + urlEncoded;
			g_http.addRequest(url, fname, "", &playerCheckCallback, SDL_static_cast(Uint32, i));
		}
	}
}

void Engine::redraw()
{
	m_surface->beginScene();
	if(m_ingame)
	{
		g_map.render();
		m_surface->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_BACKGROUND_GREY_X, GUI_UI_BACKGROUND_GREY_Y, GUI_UI_BACKGROUND_GREY_W, GUI_UI_BACKGROUND_GREY_H, m_gameBackgroundRect.x1, m_gameBackgroundRect.y1, m_gameBackgroundRect.x2, m_gameBackgroundRect.y2);
		m_surface->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_DARK_X, GUI_UI_ICON_HORIZONTAL_LINE_DARK_Y, GUI_UI_ICON_HORIZONTAL_LINE_DARK_W, GUI_UI_ICON_HORIZONTAL_LINE_DARK_H, m_gameWindowRect.x1 - 1, m_gameWindowRect.y1 - 1, m_gameWindowRect.x2 + 2, 1);
		m_surface->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_DARK_X, GUI_UI_ICON_VERTICAL_LINE_DARK_Y, GUI_UI_ICON_VERTICAL_LINE_DARK_W, GUI_UI_ICON_VERTICAL_LINE_DARK_H, m_gameWindowRect.x1 - 1, m_gameWindowRect.y1, 1, m_gameWindowRect.y2 + 1);
		m_surface->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_X, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_Y, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_W, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_H, m_gameWindowRect.x1, m_gameWindowRect.y1+ m_gameWindowRect.y2, m_gameWindowRect.x2 + 1, 1);
		m_surface->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_X, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_Y, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_W, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_H, m_gameWindowRect.x1 + m_gameWindowRect.x2, m_gameWindowRect.y1, 1, m_gameWindowRect.y2);
		m_surface->drawGameScene(0, 0, RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, m_gameWindowRect.x1, m_gameWindowRect.y1, m_gameWindowRect.x2, m_gameWindowRect.y2);
		m_surface->setClipRect(m_gameWindowRect.x1, m_gameWindowRect.y1, m_gameWindowRect.x2, m_gameWindowRect.y2);
		g_map.renderInformations(m_gameWindowRect.x1, m_gameWindowRect.y1, m_gameWindowRect.x2, m_gameWindowRect.y2, m_scale, m_scaledSize);
		m_surface->disableClipRect();
		g_chat.render(m_chatWindowRect);
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
			(*it)->render();

		{
			m_leftPanelAddRect = iRect(m_gameBackgroundRect.x1, m_gameBackgroundRect.y1, GUI_UI_SIDEBAR_LEFT_ADD_UP_W, GUI_UI_SIDEBAR_LEFT_ADD_UP_H);
			if(m_leftPanel != GUI_PANEL_RANDOM)
				m_leftPanelAddRect.x1 = m_gameBackgroundRect.x1 - 2;

			if(m_canAddLeftPanel)
			{
				if(m_leftAddPanel == 1)
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_LEFT_ADD_DOWN_X, GUI_UI_SIDEBAR_LEFT_ADD_DOWN_Y, m_leftPanelAddRect.x1, m_leftPanelAddRect.y1, m_leftPanelAddRect.x2, m_leftPanelAddRect.y2);
				else
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_LEFT_ADD_UP_X, GUI_UI_SIDEBAR_LEFT_ADD_UP_Y, m_leftPanelAddRect.x1, m_leftPanelAddRect.y1, m_leftPanelAddRect.x2, m_leftPanelAddRect.y2);
			}
			else
				m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_LEFT_ADD_DISABLED_X, GUI_UI_SIDEBAR_LEFT_ADD_DISABLED_Y, m_leftPanelAddRect.x1, m_leftPanelAddRect.y1, m_leftPanelAddRect.x2, m_leftPanelAddRect.y2);

			m_leftPanelRemRect = iRect(m_leftPanelAddRect.x1, m_leftPanelAddRect.y1 + m_leftPanelAddRect.y2, GUI_UI_SIDEBAR_LEFT_REMOVE_UP_W, GUI_UI_SIDEBAR_LEFT_REMOVE_UP_H);
			if(m_haveExtraLeftPanel)
			{
				if(m_leftRemPanel == 1)
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_LEFT_REMOVE_DOWN_X, GUI_UI_SIDEBAR_LEFT_REMOVE_DOWN_Y, m_leftPanelRemRect.x1, m_leftPanelRemRect.y1, m_leftPanelRemRect.x2, m_leftPanelRemRect.y2);
				else
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_LEFT_REMOVE_UP_X, GUI_UI_SIDEBAR_LEFT_REMOVE_UP_Y, m_leftPanelRemRect.x1, m_leftPanelRemRect.y1, m_leftPanelRemRect.x2, m_leftPanelRemRect.y2);
			}
			else
				m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_LEFT_REMOVE_DISABLED_X, GUI_UI_SIDEBAR_LEFT_REMOVE_DISABLED_Y, m_leftPanelRemRect.x1, m_leftPanelRemRect.y1, m_leftPanelRemRect.x2, m_leftPanelRemRect.y2);
		}
		{
			m_rightPanelAddRect = iRect(m_gameBackgroundRect.x1 + m_gameBackgroundRect.x2 - 7, m_gameBackgroundRect.y1, GUI_UI_SIDEBAR_RIGHT_ADD_UP_W, GUI_UI_SIDEBAR_RIGHT_ADD_UP_H);
			if(m_canAddRightPanel)
			{
				if(m_rightAddPanel == 1)
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_RIGHT_ADD_DOWN_X, GUI_UI_SIDEBAR_RIGHT_ADD_DOWN_Y, m_rightPanelAddRect.x1, m_rightPanelAddRect.y1, m_rightPanelAddRect.x2, m_rightPanelAddRect.y2);
				else
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_RIGHT_ADD_UP_X, GUI_UI_SIDEBAR_RIGHT_ADD_UP_Y, m_rightPanelAddRect.x1, m_rightPanelAddRect.y1, m_rightPanelAddRect.x2, m_rightPanelAddRect.y2);
			}
			else
				m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_RIGHT_ADD_DISABLED_X, GUI_UI_SIDEBAR_RIGHT_ADD_DISABLED_Y, m_rightPanelAddRect.x1, m_rightPanelAddRect.y1, m_rightPanelAddRect.x2, m_rightPanelAddRect.y2);

			m_rightPanelRemRect = iRect(m_rightPanelAddRect.x1, m_rightPanelAddRect.y1 + m_rightPanelAddRect.y2, GUI_UI_SIDEBAR_RIGHT_REMOVE_UP_W, GUI_UI_SIDEBAR_RIGHT_REMOVE_UP_H);
			if(m_haveExtraRightPanel)
			{
				if(m_rightRemPanel == 1)
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_RIGHT_REMOVE_DOWN_X, GUI_UI_SIDEBAR_RIGHT_REMOVE_DOWN_Y, m_rightPanelRemRect.x1, m_rightPanelRemRect.y1, m_rightPanelRemRect.x2, m_rightPanelRemRect.y2);
				else
					m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_RIGHT_REMOVE_UP_X, GUI_UI_SIDEBAR_RIGHT_REMOVE_UP_Y, m_rightPanelRemRect.x1, m_rightPanelRemRect.y1, m_rightPanelRemRect.x2, m_rightPanelRemRect.y2);
			}
			else
				m_surface->drawPicture(GUI_UI_IMAGE, GUI_UI_SIDEBAR_RIGHT_REMOVE_DISABLED_X, GUI_UI_SIDEBAR_RIGHT_REMOVE_DISABLED_Y, m_rightPanelRemRect.x1, m_rightPanelRemRect.y1, m_rightPanelRemRect.x2, m_rightPanelRemRect.y2);
		}

		if(m_topPanel)
			m_topPanel->render();
	}
	else
	{
		m_surface->drawBackground(GUI_BACKGROUND_IMAGE, GUI_BACKGROUND_X, GUI_BACKGROUND_Y, GUI_BACKGROUND_W, GUI_BACKGROUND_H, 0, 0, m_windowW, m_windowH);
		if(g_mainWindow)
			g_mainWindow->render();
	}

	if(m_showPerformance)
	{
		extern Uint32 g_frameDiff;
		extern Uint16 g_lastFrames;

		Sint32 posX;
		if(m_ingame)
			posX = m_gameBackgroundRect.x1 + m_gameBackgroundRect.x2 - 5;
		else
			posX = m_windowW - 5;

		Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u FPS", g_lastFrames);
		drawFont(CLIENT_FONT_OUTLINED, posX, 5, std::string(g_buffer, SDL_static_cast(size_t, len)), 255, 255, 255, CLIENT_FONT_ALIGN_RIGHT);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u ms", g_frameDiff);
		drawFont(CLIENT_FONT_OUTLINED, posX, 19, std::string(g_buffer, SDL_static_cast(size_t, len)), 255, 255, 255, CLIENT_FONT_ALIGN_RIGHT);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "Ping: %u ms", g_ping);
		drawFont(CLIENT_FONT_OUTLINED, posX, 33, std::string(g_buffer, SDL_static_cast(size_t, len)), 255, 255, 255, CLIENT_FONT_ALIGN_RIGHT);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "Software: %s", m_surface->getSoftware());
		drawFont(CLIENT_FONT_OUTLINED, posX, 47, std::string(g_buffer, SDL_static_cast(size_t, len)), 255, 255, 255, CLIENT_FONT_ALIGN_RIGHT);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "Hardware: %s", m_surface->getHardware());
		drawFont(CLIENT_FONT_OUTLINED, posX, 61, std::string(g_buffer, SDL_static_cast(size_t, len)), 255, 255, 255, CLIENT_FONT_ALIGN_RIGHT);
		len = SDL_snprintf(g_buffer, sizeof(g_buffer), "VRAM: %u MB", m_surface->getVRAM());
		drawFont(CLIENT_FONT_OUTLINED, posX, 75, std::string(g_buffer, SDL_static_cast(size_t, len)), 255, 255, 255, CLIENT_FONT_ALIGN_RIGHT);
	}

	if(m_actWindow)
		m_actWindow->render();

	if(m_showLogger)
		g_logger.render(0, 0, m_windowW, m_windowH);

	if(m_description)
		m_description->render();

	if(m_contextMenu)
		m_contextMenu->render();

	renderDevRecorderStatus();
	renderUIInspector();
		
	m_surface->endScene();
}

void Engine::renderDevRecorderStatus()
{
	if(!m_devMode || !m_devRecorder)
		return;

	auto& renderer = getRender();
	std::string text("NOW RECORDING  Ctrl+Alt+V to stop");
	Sint32 textWidth = SDL_static_cast(Sint32, calculateFontWidth(CLIENT_FONT_SMALL, text));
	Sint32 boxWidth = textWidth + 30;
	Sint32 boxHeight = 20;
	Sint32 boxX = 8;
	Sint32 boxY = 8;

	if(m_ingame)
	{
		boxX = m_gameBackgroundRect.x1 + 8;
		boxY = m_gameBackgroundRect.y1 + 8;
	}
	if(boxX + boxWidth > m_windowW - 6)
		boxX = UTIL_max<Sint32>(6, m_windowW - boxWidth - 6);
	if(boxY + boxHeight > m_windowH - 6)
		boxY = UTIL_max<Sint32>(6, m_windowH - boxHeight - 6);

	renderer->fillRectangle(boxX, boxY, boxWidth, boxHeight, 0, 0, 0, 210);
	renderer->drawRectangle(boxX, boxY, boxWidth, boxHeight, 1, 255, 80, 80, 255);
	renderer->fillRectangle(boxX + 7, boxY + 6, 8, 8, 255, 40, 40, 255);
	drawFont(CLIENT_FONT_SMALL, boxX + 22, boxY + 5, text, 255, 180, 180, CLIENT_FONT_ALIGN_LEFT);
}

void Engine::renderUIInspector()
{
	if(!m_devMode || !m_uiInspector)
		return;

	auto& renderer = getRender();
	if(m_uiInspectorRect.x2 > 0 && m_uiInspectorRect.y2 > 0)
		renderer->drawRectangle(m_uiInspectorRect.x1, m_uiInspectorRect.y1, m_uiInspectorRect.x2, m_uiInspectorRect.y2, 1, 255, 220, 0, 255);

	if(m_uiInspectorText.empty())
		return;

	std::string header("[UI Inspector] Ctrl+Alt+I off | click copies/logs");
	std::string text = header;
	text.append("\n").append(m_uiInspectorText);

	Sint32 boxWidth = 0;
	Sint32 lines = 0;
	size_t start = 0;
	while(start <= text.length())
	{
		size_t end = text.find('\n', start);
		if(end == std::string::npos)
			end = text.length();

		boxWidth = UTIL_max<Sint32>(boxWidth, SDL_static_cast(Sint32, calculateFontWidth(CLIENT_FONT_SMALL, text, start, end - start)));
		++lines;
		if(end == text.length())
			break;
		start = end + 1;
	}

	Sint32 boxHeight = lines * 10 + 8;
	boxWidth = UTIL_min<Sint32>(boxWidth + 12, m_windowW - 12);
	Sint32 boxX = UTIL_min<Sint32>(m_uiInspectorMouseX + 14, m_windowW - boxWidth - 6);
	Sint32 boxY = UTIL_min<Sint32>(m_uiInspectorMouseY + 14, m_windowH - boxHeight - 6);
	if(boxX < 6)
		boxX = 6;
	if(boxY < 6)
		boxY = 6;

	renderer->fillRectangle(boxX, boxY, boxWidth, boxHeight, 0, 0, 0, 210);
	renderer->drawRectangle(boxX, boxY, boxWidth, boxHeight, 1, 255, 220, 0, 255);
	drawFont(CLIENT_FONT_SMALL, boxX + 6, boxY + 4, text, 255, 255, 255, CLIENT_FONT_ALIGN_LEFT);
}

void Engine::drawFont(Uint8 fontId, Sint32 x, Sint32 y, const std::string& text, Uint8 r, Uint8 g, Uint8 b, Sint32 align, size_t pos, size_t len)
{
	if(align != CLIENT_FONT_ALIGN_LEFT)
	{
		drawFont(fontId, x, y, text.substr(pos, len), r, g, b, align);
		return;
	}

	len += pos;
	if(len > text.length())
		len = text.length();

	m_surface->drawFont(m_charPicture[fontId], x, y, text, pos, len, r, g, b, m_charx[fontId], m_chary[fontId], m_charw[fontId], m_charh[fontId]);
}

void Engine::drawFont(Uint8 fontId, Sint32 x, Sint32 y, const std::string& text, Uint8 r, Uint8 g, Uint8 b, Sint32 align)
{
	if(align != CLIENT_FONT_ALIGN_LEFT)
	{
		Sint32 rx = x, ry = y;
		size_t pos = 0, start = 0;
		while((pos = text.find('\n', pos)) != std::string::npos)
		{
			Uint32 calculatedWidth = calculateFontWidth(fontId, text, start, pos);
			switch(align)
			{
				case CLIENT_FONT_ALIGN_RIGHT:
					rx -= calculatedWidth;
					break;
				case CLIENT_FONT_ALIGN_CENTER:
					rx -= calculatedWidth / 2;
					break;
				default: break;
			}
			m_surface->drawFont(m_charPicture[fontId], rx, ry, text, start, pos, r, g, b, m_charx[fontId], m_chary[fontId], m_charw[fontId], m_charh[fontId]);
			++pos;
			start = pos;
			rx = x; ry += m_charh[fontId][0];
		}

		pos = text.length();
		if(start >= pos)
			return;

		Uint32 calculatedWidth = calculateFontWidth(fontId, text, start, pos);
		switch(align)
		{
			case CLIENT_FONT_ALIGN_RIGHT:
				rx -= calculatedWidth;
				break;
			case CLIENT_FONT_ALIGN_CENTER:
				rx -= calculatedWidth / 2;
				break;
			default: break;
		}
		m_surface->drawFont(m_charPicture[fontId], rx, ry, text, start, pos, r, g, b, m_charx[fontId], m_chary[fontId], m_charw[fontId], m_charh[fontId]);
		return;
	}
	m_surface->drawFont(m_charPicture[fontId], x, y, text, 0, text.length(), r, g, b, m_charx[fontId], m_chary[fontId], m_charw[fontId], m_charh[fontId]);
}

void Engine::drawItem(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation)
{
	Uint8 fx = thing->m_frameGroup[ThingFrameGroup_Default].m_width;
	Uint8 fy = thing->m_frameGroup[ThingFrameGroup_Default].m_height;
	Uint8 fl = thing->m_frameGroup[ThingFrameGroup_Default].m_layers;
	if(fx == 1 && fy == 1)
	{
		for(Uint8 l = 0; l < fl; ++l)
		{
			Uint32 sprite = thing->getSprite(ThingFrameGroup_Default, 0, 0, l, xPattern, yPattern, zPattern, animation);
			if(sprite != 0)
				m_surface->drawSprite(sprite, x, y, scaled, scaled, 0, 0, 32, 32);
		}
	}
	else
	{
		Uint8 frs = thing->m_frameGroup[ThingFrameGroup_Default].m_realSize;
		Uint8 fmax = SDL_static_cast(Uint8, (SDL_static_cast(Uint32, frs) + 31) / 32);
		fx = (fx > fmax ? fmax : fx);
		fy = (fy > fmax ? fmax : fy);
		Sint32 scale = SDL_static_cast(Sint32, scaled / (frs * 0.03125f));
		for(Uint8 l = 0; l < fl; ++l)
		{
			Sint32 posYc = y + scaled - scale;
			for(Uint8 cy = 0; cy < fy; ++cy)
			{
				Sint32 posXc = x + scaled - scale;
				for(Uint8 cx = 0; cx < fx; ++cx)
				{
					Uint32 sprite = thing->getSprite(ThingFrameGroup_Default, cx, cy, l, xPattern, yPattern, zPattern, animation);
					if(sprite != 0)
					{
						Sint32 dx = posXc, dy = posYc, dw = scale, dh = scale;
						Sint32 sx = 0, sy = 0, sw = 32, sh = 32;
						if(dx < x)
						{
							Sint32 diff = x - dx;
							dx += diff;
							dw -= diff;
							sx = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
							sw -= sx;
						}
						if(dy < y)
						{
							Sint32 diff = y - dy;
							dy += diff;
							dh -= diff;
							sy = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
							sh -= sy;
						}
						m_surface->drawSprite(sprite, dx, dy, dw, dh, sx, sy, sw, sh);
					}
					posXc -= scale;
				}
				posYc -= scale;
			}
		}
	}
}

void Engine::drawOutfit(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation, Uint32 outfitColor)
{
	Uint8 fx = thing->m_frameGroup[ThingFrameGroup_Idle].m_width;
	Uint8 fy = thing->m_frameGroup[ThingFrameGroup_Idle].m_height;
	Uint8 fl = thing->m_frameGroup[ThingFrameGroup_Idle].m_layers;
	if(fx == 1 && fy == 1)
	{
		if(fl > 1)
		{
			Uint32 sprite = thing->getSprite(ThingFrameGroup_Idle, 0, 0, 0, xPattern, yPattern, zPattern, animation);
			Uint32 spriteMask = thing->getSprite(ThingFrameGroup_Idle, 0, 0, 1, xPattern, yPattern, zPattern, animation);
			if(sprite != 0)
			{
				if(spriteMask != 0)
					m_surface->drawSpriteMask(sprite, spriteMask, x, y, scaled, scaled, 0, 0, 32, 32, outfitColor);
				else
					m_surface->drawSprite(sprite, x, y, scaled, scaled, 0, 0, 32, 32);
			}
		}
		else
		{
			Uint32 sprite = thing->getSprite(ThingFrameGroup_Idle, 0, 0, 0, xPattern, yPattern, zPattern, animation);
			if(sprite != 0)
				m_surface->drawSprite(sprite, x, y, scaled, scaled, 0, 0, 32, 32);
		}
	}
	else
	{
		Uint8 frs = thing->m_frameGroup[ThingFrameGroup_Idle].m_realSize;
		Uint8 fmax = SDL_static_cast(Uint8, (SDL_static_cast(Uint32, frs) + 31) / 32);
		fx = (fx > fmax ? fmax : fx);
		fy = (fy > fmax ? fmax : fy);
		Sint32 scale = SDL_static_cast(Sint32, scaled / (frs * 0.03125f));
		if(fl > 1)
		{
			Sint32 posYc = y + scaled - scale;
			for(Uint8 cy = 0; cy < fy; ++cy)
			{
				Sint32 posXc = x + scaled - scale;
				for(Uint8 cx = 0; cx < fx; ++cx)
				{
					Uint32 sprite = thing->getSprite(ThingFrameGroup_Idle, cx, cy, 0, xPattern, yPattern, zPattern, animation);
					Uint32 spriteMask = thing->getSprite(ThingFrameGroup_Idle, cx, cy, 1, xPattern, yPattern, zPattern, animation);
					if(sprite != 0)
					{
						Sint32 dx = posXc, dy = posYc, dw = scale, dh = scale;
						Sint32 sx = 0, sy = 0, sw = 32, sh = 32;
						if(dx < x)
						{
							Sint32 diff = x - dx;
							dx += diff;
							dw -= diff;
							sx = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
							sw -= sx;
						}
						if(dy < y)
						{
							Sint32 diff = y - dy;
							dy += diff;
							dh -= diff;
							sy = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
							sh -= sy;
						}
						if(spriteMask != 0)
							m_surface->drawSpriteMask(sprite, spriteMask, dx, dy, dw, dh, sx, sy, sw, sh, outfitColor);
						else
							m_surface->drawSprite(sprite, dx, dy, dw, dh, sx, sy, sw, sh);
					}
					posXc -= scale;
				}
				posYc -= scale;
			}
		}
		else
		{
			Sint32 posYc = y + scaled - scale;
			for(Uint8 cy = 0; cy < fy; ++cy)
			{
				Sint32 posXc = x + scaled - scale;
				for(Uint8 cx = 0; cx < fx; ++cx)
				{
					Uint32 sprite = thing->getSprite(ThingFrameGroup_Idle, cx, cy, 0, xPattern, yPattern, zPattern, animation);
					if(sprite != 0)
					{
						Sint32 dx = posXc, dy = posYc, dw = scale, dh = scale;
						Sint32 sx = 0, sy = 0, sw = 32, sh = 32;
						if(dx < x)
						{
							Sint32 diff = x - dx;
							dx += diff;
							dw -= diff;
							sx = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
							sw -= sx;
						}
						if(dy < y)
						{
							Sint32 diff = y - dy;
							dy += diff;
							dh -= diff;
							sy = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
							sh -= sy;
						}
						m_surface->drawSprite(sprite, dx, dy, dw, dh, sx, sy, sw, sh);
					}
					posXc -= scale;
				}
				posYc -= scale;
			}
		}
	}
}

void Engine::drawEffect(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation)
{
	Uint8 fx = thing->m_frameGroup[ThingFrameGroup_Default].m_width;
	Uint8 fy = thing->m_frameGroup[ThingFrameGroup_Default].m_height;
	if(fx == 1 && fy == 1)
	{
		Uint32 sprite = thing->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, animation);
		if(sprite != 0)
			m_surface->drawSprite(sprite, x, y, scaled, scaled, 0, 0, 32, 32);
	}
	else
	{
		Uint8 frs = thing->m_frameGroup[ThingFrameGroup_Default].m_realSize;
		Uint8 fmax = SDL_static_cast(Uint8, (SDL_static_cast(Uint32, frs) + 31) / 32);
		fx = (fx > fmax ? fmax : fx);
		fy = (fy > fmax ? fmax : fy);
		Sint32 scale = SDL_static_cast(Sint32, scaled / (frs * 0.03125f));
		Sint32 posYc = y + scaled - scale;
		for(Uint8 cy = 0; cy < fy; ++cy)
		{
			Sint32 posXc = x + scaled - scale;
			for(Uint8 cx = 0; cx < fx; ++cx)
			{
				Uint32 sprite = thing->getSprite(ThingFrameGroup_Default, cx, cy, 0, xPattern, yPattern, zPattern, animation);
				if(sprite != 0)
				{
					Sint32 dx = posXc, dy = posYc, dw = scale, dh = scale;
					Sint32 sx = 0, sy = 0, sw = 32, sh = 32;
					if(dx < x)
					{
						Sint32 diff = x - dx;
						dx += diff;
						dw -= diff;
						sx = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
						sw -= sx;
					}
					if(dy < y)
					{
						Sint32 diff = y - dy;
						dy += diff;
						dh -= diff;
						sy = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
						sh -= sy;
					}
					m_surface->drawSprite(sprite, dx, dy, dw, dh, sx, sy, sw, sh);
				}
				posXc -= scale;
			}
			posYc -= scale;
		}
	}
}

void Engine::drawDistanceEffect(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation)
{
	Uint8 fx = thing->m_frameGroup[ThingFrameGroup_Default].m_width;
	Uint8 fy = thing->m_frameGroup[ThingFrameGroup_Default].m_height;
	if(fx == 1 && fy == 1)
	{
		Uint32 sprite = thing->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, animation);
		if(sprite != 0)
			m_surface->drawSprite(sprite, x, y, scaled, scaled, 0, 0, 32, 32);
	}
	else
	{
		Uint8 frs = thing->m_frameGroup[ThingFrameGroup_Default].m_realSize;
		Uint8 fmax = SDL_static_cast(Uint8, (SDL_static_cast(Uint32, frs) + 31) / 32);
		fx = (fx > fmax ? fmax : fx);
		fy = (fy > fmax ? fmax : fy);
		Sint32 scale = SDL_static_cast(Sint32, scaled / (frs * 0.03125f));
		Sint32 posYc = y + scaled - scale;
		for(Uint8 cy = 0; cy < fy; ++cy)
		{
			Sint32 posXc = x + scaled - scale;
			for(Uint8 cx = 0; cx < fx; ++cx)
			{
				Uint32 sprite = thing->getSprite(ThingFrameGroup_Default, cx, cy, 0, xPattern, yPattern, zPattern, animation);
				if(sprite != 0)
				{
					Sint32 dx = posXc, dy = posYc, dw = scale, dh = scale;
					Sint32 sx = 0, sy = 0, sw = 32, sh = 32;
					if(dx < x)
					{
						Sint32 diff = x - dx;
						dx += diff;
						dw -= diff;
						sx = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
						sw -= sx;
					}
					if(dy < y)
					{
						Sint32 diff = y - dy;
						dy += diff;
						dh -= diff;
						sy = SDL_static_cast(Sint32, SDL_static_cast(float, diff) / SDL_static_cast(float, scale) * 32.f);
						sh -= sy;
					}
					m_surface->drawSprite(sprite, dx, dy, dw, dh, sx, sy, sw, sh);
				}
				posXc -= scale;
			}
			posYc -= scale;
		}
	}
}

unsigned char* Engine::LoadSprite(Uint32 spriteId, bool bgra)
{
	if(spriteId == 0 || spriteId > g_spriteCounts)
		return NULL;

	if(bgra)
		return g_spriteManager.LoadSprite_BGRA(spriteId);
	return g_spriteManager.LoadSprite_RGBA(spriteId);
}

unsigned char* Engine::LoadSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Uint32 outfitColor, bool bgra)
{
	unsigned char* pixels = LoadSprite(spriteId, bgra);
	if(!pixels)
		return NULL;

	unsigned char* tempPixels = LoadSprite(maskSpriteId, bgra);
	if(!tempPixels)
	{
		SDL_free(pixels);
		return NULL;
	}

	float hR, hG, hB, bR, bG, bB, lR, lG, lB, fR, fG, fB;
	getOutfitColorFloat(SDL_static_cast(Uint8, outfitColor), hR, hG, hB);
	getOutfitColorFloat(SDL_static_cast(Uint8, outfitColor >> 8), bR, bG, bB);
	getOutfitColorFloat(SDL_static_cast(Uint8, outfitColor >> 16), lR, lG, lB);
	getOutfitColorFloat(SDL_static_cast(Uint8, outfitColor >> 24), fR, fG, fB);
	if(bgra)
	{
		for(Uint16 i = 0; i <= 4092; i += 4)
		{
			if(tempPixels[i + 3] == SDL_ALPHA_OPAQUE)
			{
				Uint32 U32pixel = *SDL_reinterpret_cast(Uint32*, &tempPixels[i]);
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Head
				if(U32pixel == 0xFFFFFF00)
				#else
				if(U32pixel == 0x00FFFFFF)
				#endif
				{
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * hR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * hG);
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * hB);
				}
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Body
				else if(U32pixel == 0xFFFF0000)
				#else
				else if(U32pixel == 0x0000FFFF)
				#endif
				{
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * bR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * bG);
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * bB);
				}
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Legs
				else if(U32pixel == 0xFF00FF00)
				#else
				else if(U32pixel == 0x00FF00FF)
				#endif
				{
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * lR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * lG);
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * lB);
				}
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Feet
				else if(U32pixel == 0xFF0000FF)
				#else
				else if(U32pixel == 0xFF0000FF)
				#endif
				{
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * fR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * fG);
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * fB);
				}
			}
		}
	}
	else
	{
		for(Uint16 i = 0; i <= 4092; i += 4)
		{
			if(tempPixels[i + 3] == SDL_ALPHA_OPAQUE)
			{
				Uint32 U32pixel = *SDL_reinterpret_cast(Uint32*, &tempPixels[i]);
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Head
				if(U32pixel == 0xFF00FFFF)
				#else
				if(U32pixel == 0xFFFF00FF)
				#endif
				{
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * hR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * hG);
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * hB);
				}
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Body
				else if(U32pixel == 0xFF0000FF)
				#else
				else if(U32pixel == 0xFF0000FF)
				#endif
				{
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * bR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * bG);
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * bB);
				}
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Legs
				else if(U32pixel == 0xFF00FF00)
				#else
				else if(U32pixel == 0x00FF00FF)
				#endif
				{
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * lR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * lG);
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * lB);
				}
				#if SDL_BYTEORDER == SDL_LIL_ENDIAN //Feet
				else if(U32pixel == 0xFFFF0000)
				#else
				else if(U32pixel == 0x0000FFFF)
				#endif
				{
					pixels[i] = SDL_static_cast(Uint8, pixels[i] * fR);
					pixels[i + 1] = SDL_static_cast(Uint8, pixels[i + 1] * fG);
					pixels[i + 2] = SDL_static_cast(Uint8, pixels[i + 2] * fB);
				}
			}
		}
	}
	SDL_free(tempPixels);
	return pixels;
}

unsigned char* Engine::LoadPicture(Uint16 pictureId, bool bgra, Sint32& width, Sint32& height)
{
	if(pictureId >= g_pictureCounts)
		return NULL;

	SDL_RWops* pictures = SDL_RWFromFile(g_picPath.c_str(), "rb");
	if(!pictures)
		return NULL;

	SDL_RWseek(pictures, 6, RW_SEEK_SET);
	for(Uint16 i = 0; i <= pictureId; ++i)
	{
		Uint8 w = SDL_ReadU8(pictures);
		Uint8 h = SDL_ReadU8(pictures);
		SDL_RWseek(pictures, 3, RW_SEEK_CUR);//ignore colorkey
		if(i == pictureId)
		{
			width = SDL_static_cast(Sint32, w * 32);
			height = SDL_static_cast(Sint32, h * 32);

			Uint32 protectionSize = width * height * 4;
			unsigned char* pixels = SDL_reinterpret_cast(unsigned char*, SDL_calloc(protectionSize, sizeof(unsigned char)));
			if(!pixels)
			{
				SDL_RWclose(pictures);
				return NULL;
			}

			Uint32 chunkLoc, oldLoc;
			protectionSize -= 4;
			for(Uint8 y = 0; y < h; ++y)
			{
				for(Uint8 x = 0; x < w; ++x)
				{
					Uint16 pixelSize, chunkSize;
					Uint16 readData = 0, writeData = 0;

					chunkLoc = SDL_ReadLE32(pictures);
					oldLoc = SDL_static_cast(Uint32, SDL_RWtell(pictures));
					SDL_RWseek(pictures, chunkLoc, RW_SEEK_SET);

					pixelSize = SDL_ReadLE16(pictures);
					// Original Tibia .pic uses 3-byte RGB pixels (no alpha); TFC format uses 4-byte RGBA
					bool picAlpha = (g_picRevision == 0x70434654);
					while(readData < pixelSize)
					{
						chunkSize = SDL_ReadLE16(pictures);
						writeData += chunkSize;
						chunkSize = SDL_ReadLE16(pictures);
						readData += 4;
						for(Uint16 j = 0; j < chunkSize; ++j)
						{
							Uint16 xPos = x * 32 + (writeData + j) % 32;
							Uint16 yPos = y * 32 + (writeData + j) / 32;
							Uint32 offset = (yPos * width + xPos) * 4;
							if(offset <= protectionSize)
							{
								if(bgra)
								{
									pixels[offset + 2] = SDL_ReadU8(pictures);
									pixels[offset + 1] = SDL_ReadU8(pictures);
									pixels[offset] = SDL_ReadU8(pictures);
									pixels[offset + 3] = (picAlpha ? SDL_ReadU8(pictures) : 0xFF);
								}
								else
								{
									pixels[offset] = SDL_ReadU8(pictures);
									pixels[offset + 1] = SDL_ReadU8(pictures);
									pixels[offset + 2] = SDL_ReadU8(pictures);
									pixels[offset + 3] = (picAlpha ? SDL_ReadU8(pictures) : 0xFF);
								}
								readData += (picAlpha ? 4 : 3);
							}
						}
						writeData += chunkSize;
					}
					SDL_RWseek(pictures, oldLoc, RW_SEEK_SET);
				}
			}
			SDL_RWclose(pictures);
			return pixels;
		}
		else
			SDL_RWseek(pictures, w * h * 4, RW_SEEK_CUR);
	}
	SDL_RWclose(pictures);
	return NULL;
}

void Engine::issueNewConnection(bool protocolGame)
{
	releaseConnection();

	Protocol* protocol;
	if(protocolGame)
	{
		protocol = new ProtocolGame();

		CharacterDetail& character = m_characters[SDL_static_cast(size_t, m_characterSelectId)];
		#if CLIENT_OVVERIDE_VERSION == 0
		g_connection = new Connection(character.worldIp.c_str(), character.worldPort, m_clientProxy.c_str(), m_clientProxyAuth.c_str(), protocol);
		#else
		g_connection = new Connection(character.worldIp.c_str(), character.worldPort, "", "", protocol);
		#endif
	}
	else
	{
		protocol = new ProtocolLogin();
		#if CLIENT_OVVERIDE_VERSION == 0
		g_connection = new Connection(m_clientHost.c_str(), SDL_static_cast(Uint16, SDL_strtoul(m_clientPort.c_str(), NULL, 10)), m_clientProxy.c_str(), m_clientProxyAuth.c_str(), protocol);
		#else
		g_connection = new Connection(CLIENT_OVERRIDE_LOGIN_HOST, CLIENT_OVERRIDE_LOGIN_PORT, "", "", protocol);
		#endif
	}
}

void Engine::releaseConnection()
{
	if(g_connection)
	{
		delete g_connection;
		g_connection = NULL;
	}
}

void Engine::initMove(Uint16 posX, Uint16 posY, Uint8 posZ)
{
	ClientActionData& actionData = g_engine.m_actionDataStructure[CLIENT_ACTION_FIRST];
	Position fromPos = Position(actionData.posX, actionData.posY, actionData.posZ);
	Position toPos = Position(posX, posY, posZ);
	Uint16 itemCount = m_actionDataStructure[CLIENT_ACTION_SECOND].itemId;
	if(actionData.creatureId != 0)
	{
		// Creature positions and stack positions are short-lived. Delaying a push
		// through the inventory action queue can target a stale tile/stack.
		g_game.sendMove(fromPos, 0x62, actionData.posStack, toPos, 1);
		return;
	}

	if(itemCount <= 1)
		sendManualMove(fromPos, actionData.itemId, actionData.posStack, toPos, 1);
	else
	{
		Uint16 keyMods = UTIL_parseModifiers(SDL_static_cast(Uint16, SDL_GetModState()));
		if(keyMods == KMOD_CTRL)
			sendManualMove(fromPos, actionData.itemId, actionData.posStack, toPos, itemCount);
		else if(keyMods == KMOD_SHIFT)
			sendManualMove(fromPos, actionData.itemId, actionData.posStack, toPos, 1);
		else
		{
			setActionData(CLIENT_ACTION_SECOND, 0, itemCount, posX, posY, posZ, 0);
			UTIL_createItemMove();
		}
	}
}

void Engine::standardThingEvent(Uint32 event, Sint32)
{
	ClientActionData& actionData = g_engine.m_actionDataStructure[CLIENT_ACTION_FIRST];
	switch(event)
	{
		case THING_EVENT_LOOK:
		case THING_EVENT_DEBUGLOOK:
		{
			bool debugLook = (event == THING_EVENT_DEBUGLOOK);
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x == 0xFFFF)
				g_game.sendLookAt(pos, actionData.itemId, actionData.posStack, debugLook);
			else
			{
				Uint32 creatureId = actionData.creatureId;
				Creature* creature = g_map.getCreatureById(creatureId);
				if(creature)
				{
					if(!debugLook && g_game.hasGameFeature(GAME_FEATURE_LOOKATCREATURE))
					{
						g_game.sendLookInBattle(creatureId);
						return;
					}
					else
					{
						Position& position = creature->getCurrentPosition();
						Tile* creatureTile = g_map.getTile(position);
						if(creatureTile)
						{
							g_game.sendLookAt(position, 0x62, SDL_static_cast(Uint8, creatureTile->getThingStackPos(creature)), debugLook);
							return;
						}
					}
				}

				Tile* tile = g_map.getTile(pos);
				if(tile)
				{
					Thing* lookThing = tile->getTopLookThing();
					if(lookThing)
						g_game.sendLookAt(pos, (lookThing->isItem() ? lookThing->getItem()->getID() : 0x62), SDL_static_cast(Uint8, tile->getThingStackPos(lookThing)), debugLook);
				}
			}
		}
		break;
		case THING_EVENT_OPEN:
		{
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x == 0xFFFF)
			{
				if(pos.y & 0x40)
					g_engine.sendManualUseItem(pos, actionData.itemId, actionData.posStack, (pos.y & 0x0F));
				else
					g_engine.sendManualUseItem(pos, actionData.itemId, actionData.posStack, g_game.findEmptyContainerId());
			}
			else
			{
				Tile* tile = g_map.getTile(pos);
				if(tile)
				{
					Thing* useThing = tile->getTopUseThing();
					if(useThing)
						g_engine.sendManualUseItem(pos, (useThing->isItem() ? useThing->getItem()->getID() : 0x62), SDL_static_cast(Uint8, tile->getUseStackPos(useThing)), g_game.findEmptyContainerId());
				}
			}
		}
		break;
		case THING_EVENT_USE:
		{
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x == 0xFFFF)
				g_engine.sendManualUseItem(pos, actionData.itemId, actionData.posStack, g_game.findEmptyContainerId());
			else
			{
				Tile* tile = g_map.getTile(pos);
				if(tile)
				{
					Thing* useThing = tile->getTopUseThing();
					if(useThing)
						g_engine.sendManualUseItem(pos, (useThing->isItem() ? useThing->getItem()->getID() : 0x62), SDL_static_cast(Uint8, tile->getUseStackPos(useThing)), g_game.findEmptyContainerId());
				}
			}
		}
		break;
		case THING_EVENT_USEWITH:
		{
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x == 0xFFFF)
				g_engine.setAction(CLIENT_ACTION_USEWITH);
			else
			{
				Tile* tile = g_map.getTile(pos);
				if(tile)
				{
					Thing* useThing = tile->getTopUseThing();
					if(useThing && useThing->isItem())
					{
						Item* item = useThing->getItem();
						g_engine.setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), pos.x, pos.y, pos.z, SDL_static_cast(Uint8, tile->getUseStackPos(useThing)));
						g_engine.setAction(CLIENT_ACTION_USEWITH);
					}
				}
			}
		}
		break;
		case THING_EVENT_ROTATE:
		{
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x != 0xFFFF)
			{
				Tile* tile = g_map.getTile(pos);
				if(tile)
				{
					Thing* useThing = tile->getTopUseThing();
					if(useThing)
						g_game.sendRotateItem(pos, (useThing->isItem() ? useThing->getItem()->getID() : 0x62), SDL_static_cast(Uint8, tile->getUseStackPos(useThing)));
				}
			}
		}
		break;
		case THING_EVENT_TRADE:
		{
			//Trade
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x == 0xFFFF)
				g_engine.setAction(CLIENT_ACTION_TRADE);
			else
			{
				Tile* tile = g_map.getTile(pos);
				if(tile)
				{
					Thing* useThing = tile->getTopUseThing();
					if(useThing && useThing->isItem())
					{
						Item* item = useThing->getItem();
						g_engine.setActionData(CLIENT_ACTION_FIRST, 0, item->getID(), pos.x, pos.y, pos.z, SDL_static_cast(Uint8, tile->getUseStackPos(useThing)));
						g_engine.setAction(CLIENT_ACTION_TRADE);
					}
				}
			}
		}
		break;
		case THING_EVENT_WRAP:
		{
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x != 0xFFFF)
			{
				Tile* tile = g_map.getTile(pos);
				if(tile)
				{
					Thing* useThing = tile->getTopUseThing();
					if(useThing)
						g_game.sendWrapState(pos, (useThing->isItem() ? useThing->getItem()->getID() : 0x62), SDL_static_cast(Uint8, tile->getUseStackPos(useThing)));
				}
			}
		}
		break;
		case THING_EVENT_MOVEUP:
		{
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x == 0xFFFF)
				g_engine.sendManualMove(pos, actionData.itemId, 0, Position(pos.x, pos.y, 0xFE), g_engine.m_actionDataStructure[CLIENT_ACTION_SECOND].itemId);
		}
		break;
		case THING_EVENT_BROWSEFIELD:
		{
			Position pos = Position(actionData.posX, actionData.posY, actionData.posZ);
			if(pos.x != 0xFFFF)
				g_game.sendBrowseField(pos);
		}
		break;
		case THING_EVENT_REPORTCOORDS:
		{
			//Report Coordinate
		}
		break;
		case THING_EVENT_SETOUTFIT: g_game.sendRequestOutfit(); break;
		case THING_EVENT_MOUNT: g_game.sendMount(true); break;
		case THING_EVENT_DISMOUNT: g_game.sendMount(false); break;
		case THING_EVENT_ATTACK: g_game.sendAttack((g_game.getAttackID() == actionData.creatureId) ? NULL : g_map.getCreatureById(actionData.creatureId)); break;
		case THING_EVENT_FOLLOW: g_game.sendFollow((g_game.getFollowID() == actionData.creatureId) ? NULL : g_map.getCreatureById(actionData.creatureId)); break;
		case THING_EVENT_COPYNAME:
		{
			Creature* creature = g_map.getCreatureById(actionData.creatureId);
			if(creature)
				UTIL_SetClipboardTextLatin1(creature->getName().c_str());
		}
		break;
		case THING_EVENT_INVITETOPARTY: g_game.sendInviteToParty(actionData.creatureId); break;
		case THING_EVENT_JOINTOPARTY: g_game.sendJoinToParty(actionData.creatureId); break;
		case THING_EVENT_REVOKEINVITATION: g_game.sendRevokePartyInvitation(actionData.creatureId); break;
		case THING_EVENT_PASSLEADERSHIP: g_game.sendPassPartyLeadership(actionData.creatureId); break;
		case THING_EVENT_LEAVEPARTY: g_game.sendLeaveParty(); break;
		case THING_EVENT_ENABLESHAREDEXPERIENCE: g_game.sendEnableSharedPartyExperience(true); break;
		case THING_EVENT_DISABLESHAREDEXPERIENCE: g_game.sendEnableSharedPartyExperience(false); break;
		case THING_EVENT_JOINAGGRESSION: g_game.sendJoinAggression(actionData.creatureId); break;
		case THING_EVENT_MESSAGETO:
		{
			Creature* creature = g_map.getCreatureById(actionData.creatureId);
			if(creature)
				g_game.sendOpenPrivateChannel(creature->getName());
		}
		break;
		case THING_EVENT_INVITETOPRIVATECHAT:
		{
			Creature* creature = g_map.getCreatureById(actionData.creatureId);
			if(creature)
				g_game.sendChannelInvite(creature->getName(), SDL_static_cast(Uint16, g_chat.getOwnPrivateChannel()));
		}
		break;
		case THING_EVENT_EXCLUDEFROMPRIVATECHAT:
		{
			Creature* creature = g_map.getCreatureById(actionData.creatureId);
			if(creature)
				g_game.sendChannelExclude(creature->getName(), SDL_static_cast(Uint16, g_chat.getOwnPrivateChannel()));
		}
		break;
		case THING_EVENT_ADDTOVIPLIST:
		{
			Creature* creature = g_map.getCreatureById(actionData.creatureId);
			if(creature)
				g_game.sendAddVip(creature->getName());
		}
		break;
		case THING_EVENT_IGNORE:
		{
			Creature* creature = g_map.getCreatureById(actionData.creatureId);
			if(creature)
				UTIL_toggleIgnore(creature->getName());
		}
		break;
		case THING_EVENT_RULEVIOLATION:
		{
			//Rule Violation
		}
		break;
		case THING_EVENT_REPORTNAME:
		{
			//Report Name
		}
		break;
		case THING_EVENT_REPORTBOTMACRO:
		{
			//Report Bot/Macro
		}
		break;
		default: break;
	}
}

GUI_ContextMenu* Engine::createThingContextMenu(Creature* creature, ItemUI* itemui, Item* item)
{
	GUI_ContextMenu* newMenu = new GUI_ContextMenu();
	bool showDebugLook = (m_devMode && (UTIL_parseModifiers(SDL_static_cast(Uint16, SDL_GetModState())) & KMOD_CTRL));
	if(item)
	{
		Position& position = item->getCurrentPosition();
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_LOOK, "Look", (m_classicControl ? "" : "(Shift)"));

		ThingType* itemType = item->getThingType();
		if(itemType)
		{
			if(itemType->hasFlag(ThingAttribute_Container))
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_OPEN, "Open", (m_classicControl ? "" : "(Ctrl)"));
			else if(itemType->hasFlag(ThingAttribute_MultiUse))
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_USEWITH, "Use with ...", (m_classicControl ? "" : "(Ctrl)"));
			else
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_USE, "Use", (m_classicControl ? "" : "(Ctrl)"));
			if(showDebugLook)
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_DEBUGLOOK, "Look ID", "");

			if(itemType->hasFlag(ThingAttribute_Wrapable))
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_WRAP, "Wrap", "");
			else if(itemType->hasFlag(ThingAttribute_Unwrapable))
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_WRAP, "Unwrap", "");

			if(itemType->hasFlag(ThingAttribute_Rotateable))
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_ROTATE, "Rotate", "");

			if(g_game.hasGameFeature(GAME_FEATURE_BROWSEFIELD))
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_BROWSEFIELD, "Browse Field", "");

			/*
			if(g_game.canReportBugs())
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_REPORTCOORDS, "Report Coordinate", "");
			*/

			if(itemType->hasFlag(ThingAttribute_Pickupable))
			{
				newMenu->addSeparator();
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_TRADE, "Trade with ...", "");
			}
		}
		else
		{
			newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_USE, "Use", (m_classicControl ? "" : "(Ctrl)"));
			if(showDebugLook)
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_DEBUGLOOK, "Look ID", "");
		}

		ClientActionData& actionData = m_actionDataStructure[CLIENT_ACTION_FIRST];
		actionData.itemId = item->getID();
		actionData.posX = position.x;
		actionData.posY = position.y;
		actionData.posZ = position.z;
		actionData.posStack = 0;
	}
	else if(itemui)
	{
		Position& position = itemui->getCurrentPosition();
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_LOOK, "Look", (m_classicControl ? "" : "(Shift)"));

		ThingType* itemType = itemui->getThingType();
		if(itemType)
		{
			if(itemType->hasFlag(ThingAttribute_Container))
			{
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_OPEN, "Open", (m_classicControl ? "" : "(Ctrl)"));
				if(position.y & 0x40)
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_USE, "Open in new window", "");
			}
			else if(itemType->hasFlag(ThingAttribute_MultiUse))
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_USEWITH, "Use with ...", (m_classicControl ? "" : "(Ctrl)"));
			else
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_USE, "Use", (m_classicControl ? "" : "(Ctrl)"));
			if(showDebugLook)
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_DEBUGLOOK, "Look ID", "");

			if(itemType->hasFlag(ThingAttribute_Pickupable))
			{
				newMenu->addSeparator();
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_TRADE, "Trade with ...", "");
			}
			
			if((position.y & 0x40) && g_game.containerHasParent(position.y & 0x0F))
			{
				m_actionDataStructure[CLIENT_ACTION_SECOND].itemId = itemui->getItemCount();
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_MOVEUP, "Move up", "");
			}
		}
		else
		{
			newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_USE, "Use", (m_classicControl ? "" : "(Ctrl)"));
			if(showDebugLook)
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_DEBUGLOOK, "Look ID", "");
		}

		ClientActionData& actionData = m_actionDataStructure[CLIENT_ACTION_FIRST];
		actionData.itemId = itemui->getID();
		actionData.posX = position.x;
		actionData.posY = position.y;
		actionData.posZ = position.z;
		actionData.posStack = 0;
	}
	else
	{
		ClientActionData& actionData = m_actionDataStructure[CLIENT_ACTION_FIRST];
		actionData.itemId = 0;
		actionData.posX = 0xFFFF;
		actionData.posY = 0;
		actionData.posZ = 0;
		actionData.posStack = 0;
	}

	if(creature)
	{
		newMenu->addSeparator();
		if(g_game.getPlayerID() == creature->getId())
		{
			newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_SETOUTFIT, "Set Outfit", "");
			if(g_game.hasGameFeature(GAME_FEATURE_MOUNTS))
			{
				if(!creature->getMountType())
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_MOUNT, "Mount", "");
				else
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_DISMOUNT, "Dismount", "");
			}

			if(!(g_game.getIcons() & ICON_SWORDS))
			{
				Uint8 playerShield = creature->getShield();
				if(UTIL_isPartyMember(playerShield))
				{
					if(UTIL_isPartyLeader(playerShield))
					{
						if(UTIL_isPartySharedEnabled(playerShield))
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_DISABLESHAREDEXPERIENCE, "Disable Shared Experience", "");
						else
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_ENABLESHAREDEXPERIENCE, "Enable Shared Experience", "");
					}

					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_LEAVEPARTY, "Leave Party", "");
				}
			}
		}
		else
		{
			newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_ATTACK, (g_game.getAttackID() == creature->getId() ? "Stop Attack" : "Attack"), (m_classicControl ? "" : "(Alt)"));
			newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_FOLLOW, (g_game.getFollowID() == creature->getId() ? "Stop Follow" : "Follow"), "");
			if(creature->isPlayer())
			{
				std::string& creatureName = creature->getName();
				newMenu->addSeparator();

				SDL_snprintf(g_buffer, sizeof(g_buffer), "Message to %s", creatureName.c_str());
				newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_MESSAGETO, g_buffer, "");
				if(g_chat.getOwnPrivateChannel() != SDL_static_cast(Uint32, -1))
				{
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_INVITETOPRIVATECHAT, "Invite to private chat", "");
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_EXCLUDEFROMPRIVATECHAT, "Exclude from private chat", "");
				}
				if(!UTIL_haveVipPlayer(creatureName))
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_ADDTOVIPLIST, "Add to VIP list", "");

				if(!UTIL_onWhiteList(creatureName))
				{
					SDL_snprintf(g_buffer, sizeof(g_buffer), (UTIL_onBlackList(creatureName) ? "Unignore %s" : "Ignore %s"), creatureName.c_str());
					newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_IGNORE, g_buffer, "");
				}

				Uint8 targetShield = creature->getShield();
				Uint8 playerShield = (g_map.getLocalCreature() ? g_map.getLocalCreature()->getShield() : SHIELD_NONE);
				switch(playerShield)
				{
					case SHIELD_NONE:
					case SHIELD_WHITEBLUE:
					{
						if(targetShield == SHIELD_WHITEYELLOW)
						{
							SDL_snprintf(g_buffer, sizeof(g_buffer), "Join %s's Party", creatureName.c_str());
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_JOINTOPARTY, g_buffer, "");
						}
						else if(targetShield != SHIELD_GRAY)
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_INVITETOPARTY, "Invite to Party", "");
					}
					break;
					case SHIELD_WHITEYELLOW:
					{
						if(targetShield == SHIELD_WHITEBLUE)
						{
							SDL_snprintf(g_buffer, sizeof(g_buffer), "Revoke %s's Invitation", creatureName.c_str());
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_REVOKEINVITATION, g_buffer, "");
						}
					}
					break;
					case SHIELD_BLUE:
					case SHIELD_BLUE_SHAREDEXP:
					case SHIELD_BLUE_NOSHAREDEXP_BLINK:
					case SHIELD_BLUE_NOSHAREDEXP:
					{
						if(targetShield > SHIELD_WHITEBLUE)
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_JOINAGGRESSION, "Join Aggression", "");
					}
					break;
					case SHIELD_YELLOW:
					case SHIELD_YELLOW_SHAREDEXP:
					case SHIELD_YELLOW_NOSHAREDEXP_BLINK:
					case SHIELD_YELLOW_NOSHAREDEXP:
					{
						if(targetShield == SHIELD_WHITEBLUE)
						{
							SDL_snprintf(g_buffer, sizeof(g_buffer), "Revoke %s's Invitation", creatureName.c_str());
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_REVOKEINVITATION, g_buffer, "");
						}
						else if(targetShield == SHIELD_BLUE || targetShield == SHIELD_BLUE_SHAREDEXP || targetShield == SHIELD_BLUE_NOSHAREDEXP_BLINK || targetShield == SHIELD_BLUE_NOSHAREDEXP)
						{
							SDL_snprintf(g_buffer, sizeof(g_buffer), "Pass Leadership to %s", creatureName.c_str());
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_PASSLEADERSHIP, g_buffer, "");
						}
						else if(targetShield == SHIELD_GRAY)
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_JOINAGGRESSION, "Join Aggression", "");
						else
							newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_INVITETOPARTY, "Invite to Party", "");
					}
					break;
					default: break;
				}
			}
		}

		/*
		if(hasRuleViolation())
		{
			newMenu->addSeparator();
			newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_RULEVIOLATION, "Rule Violation", "");
		}
		//Report Name
		//Report Bot/Macro
		*/

		newMenu->addSeparator();
		newMenu->addContextMenu(CONTEXTMENU_STYLE_STANDARD, THING_EVENT_COPYNAME, "Copy Name", "");
		m_actionDataStructure[CLIENT_ACTION_FIRST].creatureId = creature->getId();
	}
	else
		m_actionDataStructure[CLIENT_ACTION_FIRST].creatureId = 0;

	return newMenu;
}

void Engine::enableMoveItem(Sint32 x, Sint32 y)
{
	m_moveItemX = x;
	m_moveItemY = y;
}

void Engine::setActionData(ClientActions data, Uint32 creatureId, Uint16 itemId, Uint16 posX, Uint16 posY, Uint8 posZ, Uint8 posStack)
{
	ClientActionData& actionData = m_actionDataStructure[data];
	actionData.creatureId = creatureId;
	actionData.itemId = itemId;
	actionData.posX = posX;
	actionData.posY = posY;
	actionData.posZ = posZ;
	actionData.posStack = posStack;
}

void Engine::setAction(ClientActions action)
{
	m_actionData = action;
	if(m_actionData == CLIENT_ACTION_MOVEITEM || m_actionData == CLIENT_ACTION_USEWITH || m_actionData == CLIENT_ACTION_TRADE || m_actionData == CLIENT_ACTION_SEARCHHOTKEY)
		setCursor(CLIENT_CURSOR_CROSSHAIR);
	else if(m_actionData == CLIENT_ACTION_LENSHELP)
		setCursor(CLIENT_CURSOR_LENSHELP);
	else
		setCursor(CLIENT_CURSOR_ARROW);
}

void Engine::showContextMenu(GUI_ContextMenu* menu, Sint32 mouseX, Sint32 mouseY)
{
	if(m_contextMenu)
		delete m_contextMenu;
	m_contextMenu = menu;
	m_contextMenu->setDisplay(mouseX, mouseY);
}

void Engine::showDescription(Sint32 mouseX, Sint32 mouseY, const std::string& description, Uint32 delay)
{
	if(!m_description)
		m_description = new GUI_Description();
	m_description->setDisplay(mouseX, mouseY, description, delay);
}

bool Engine::addToPanel(GUI_PanelWindow* pPanel, Sint32 preferredPanel)
{
	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
	{
		if(preferredPanel != GUI_PANEL_RANDOM)
		{
			if((*it)->getInternalID() == preferredPanel)
			{
				if((*it)->getFreeHeight() >= pPanel->getRect().y2 || (*it)->tryFreeHeight(pPanel))
				{
					if(pPanel->getParent())
						pPanel->getParent()->removePanel(pPanel, false);

					(*it)->addPanel(pPanel);
					pPanel->setParent((*it));
					return true;
				}
				return false;
			}
		}
		else
		{
			if((*it)->getFreeHeight() >= pPanel->getRect().y2)
			{
				if(pPanel->getParent())
					pPanel->getParent()->removePanel(pPanel, false);

				(*it)->addPanel(pPanel);
				pPanel->setParent((*it));
				return true;
			}
		}
	}
	if(preferredPanel == GUI_PANEL_RANDOM)
	{
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		{
			if((*it)->tryFreeHeight(pPanel))
			{
				if(pPanel->getParent())
					pPanel->getParent()->removePanel(pPanel, false);

				(*it)->addPanel(pPanel);
				pPanel->setParent((*it));
				return true;
			}
		}
	}
	return false;
}

void Engine::removePanelWindow(GUI_PanelWindow* pPanel)
{
	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		(*it)->removePanel(pPanel, false);
	
	delete pPanel;
}

GUI_PanelWindow* Engine::getPanel(Uint32 internalID)
{
	GUI_PanelWindow* panel = NULL;
	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
	{
		panel = (*it)->getPanel(internalID);
		if(panel)
			return panel;
	}
	return panel;
}

void Engine::clearPanels()
{
	if(!m_panels.empty())
	{
		m_openDialogs.clear();
		for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		{
			std::vector<GUI_PanelWindow*>& windows = (*it)->getPanelWindows();
			for(std::vector<GUI_PanelWindow*>::iterator wit = windows.begin(), wend = windows.end(); wit != wend; ++wit)
			{
				Uint32 windowId = (*wit)->getInternalID();
				switch(windowId)
				{
					//Save only windows that needed saving
					case GUI_PANEL_WINDOW_INVENTORY:
					case GUI_PANEL_WINDOW_INVENTORY_MINIMIZED:
					case GUI_PANEL_WINDOW_MINIMAP:
					case GUI_PANEL_WINDOW_HEALTH:
					case GUI_PANEL_WINDOW_BUTTONS:
					case GUI_PANEL_WINDOW_SKILLS:
					case GUI_PANEL_WINDOW_BATTLE:
					case GUI_PANEL_WINDOW_VIP:
					case GUI_PANEL_WINDOW_SPELL_LIST:
					case GUI_PANEL_WINDOW_UNJUSTIFIED_POINTS:
					case GUI_PANEL_WINDOW_PREY_WIDGET:
					case GUI_PANEL_WINDOW_PARTY:
					case GUI_PANEL_WINDOW_TOURNAMENT_WIDGET:
					case GUI_PANEL_WINDOW_ANALYTICS_SELECTOR:
					case GUI_PANEL_WINDOW_ANALYTICS_HUNTING:
					case GUI_PANEL_WINDOW_ANALYTICS_LOOT:
					case GUI_PANEL_WINDOW_ANALYTICS_SUPPLY:
					case GUI_PANEL_WINDOW_ANALYTICS_IMPACT:
					case GUI_PANEL_WINDOW_ANALYTICS_XP:
					case GUI_PANEL_WINDOW_ANALYTICS_DROP:
					case GUI_PANEL_WINDOW_ANALYTICS_QUEST:
					{
						m_parentWindows[windowId] = (*it)->getInternalID();
						m_openDialogs.push_back(windowId);
					}
					break;
					default: break;
				}
			}
		}
	}

	for(std::vector<GUI_Panel*>::iterator it = m_panels.begin(), end = m_panels.end(); it != end; ++it)
		m_toReleasePanels.push_back((*it));
	m_panels.clear();
}

void Engine::checkPanelWindows(GUI_PanelWindow* pPanel, Sint32 x, Sint32 y)
{
	GUI_Panel* gPanel = NULL;
	iRect currentRect = pPanel->getRect();
	GUI_Panel* parent = pPanel->getParent();
	std::vector<GUI_Panel*>::iterator it = m_panels.begin();
	for(std::vector<GUI_Panel*>::iterator end = m_panels.end(); it != end; ++it)
	{
		if((*it)->isInsideRect(x, y))
		{
			if((*it) == parent)
			{
				parent->checkPanel(pPanel, x, y);
				pPanel->setRect(currentRect);
			}
			else if(pPanel->isParentChangeable())
			{
				if((*it)->getFreeHeight() >= currentRect.y2)
				{
					gPanel = (*it);
					break;
				}
			}
			return;
		}
	}
	if(gPanel)
	{
		if(parent)
			parent->removePanel(pPanel, false);

		gPanel->addPanel(pPanel);
		gPanel->checkPanel(pPanel, x, y);
		pPanel->setRect(currentRect);
		pPanel->setParent(gPanel);
	}
}

void Engine::resizePanel(GUI_PanelWindow* pPanel, Sint32 x, Sint32 y)
{
	GUI_Panel* parent = pPanel->getParent();
	if(parent)
		parent->resizePanel(pPanel, x, y);
}

void Engine::processGameStart()
{
	m_ingame = true;
	g_ping = 0;
	m_botLastAction = 0;
	m_botActionCooldown = 0;
	m_manualLastAction = 0;
	m_lastPlayerMovementAction = 0;
	m_playerMovementBotBlock = 0;
	m_manualItemBotBlockUntil = 0;
	m_numpadDashActive = false;
	m_numpadDashKey = SDLK_UNKNOWN;
	m_numpadDashDirection = DIRECTION_INVALID;
	m_numpadDashLastPulse = 0;
	m_numpadDashLastTileChange = 0;
	m_numpadDashLastX = 0;
	m_numpadDashLastY = 0;
	m_numpadDashLastZ = 0;
	m_autoLogoutPending = false;
	m_autoLogoutCaptureTaken = false;
	m_autoLogoutLastAttempt = 0;
	m_autoLogoutLastLog = 0;
	m_autoLogoutPendingReason.clear();
	m_gmBaselineLogin.assign(m_autoLogoutGMs.size(), std::string());
	m_gmBaselineSet.assign(m_autoLogoutGMs.size(), false);
	m_playerWebStatus.assign(m_autoLogoutPlayers.size(), std::string());
	m_playerWebStatusSet.assign(m_autoLogoutPlayers.size(), false);
	m_lastGmPoll = 0;
	g_manualActionQueue.clear();

	g_chat.gameStart();
	g_game.reset();
	g_game.resetPlayerExperienceTable();
	clearWindows();
	clearPanels();
	checkReleaseQueue();

	g_game.sendPing();
	g_game.sendPingBack();
	g_game.sendAttackModes();

	bool haveMinimap = false, haveHealth = false, haveInventory = false, haveButtons = false;
	m_panels.push_back(new GUI_Panel(iRect(0, 0, GAME_PANEL_FIXED_WIDTH, 1000), GUI_PANEL_MAIN));
	for(std::vector<Uint32>::iterator it = m_openDialogs.begin(), end = m_openDialogs.end(); it != end; ++it)
	{
		Uint32 windowId = (*it);
		if(!haveMinimap && windowId == GUI_PANEL_WINDOW_MINIMAP)
		{
			UTIL_createMinimapPanel();
			haveMinimap = true;
		}
		else if(!haveHealth && windowId == GUI_PANEL_WINDOW_HEALTH)
		{
			UTIL_createHealthPanel();
			haveHealth = true;
		}
		else if(!haveInventory && windowId == GUI_PANEL_WINDOW_INVENTORY)
		{
			UTIL_createInventoryPanel(false);
			haveInventory = true;
		}
		else if(!haveInventory && windowId == GUI_PANEL_WINDOW_INVENTORY_MINIMIZED)
		{
			UTIL_createInventoryPanel(true);
			haveInventory = true;
		}
		else if(!haveButtons && windowId == GUI_PANEL_WINDOW_BUTTONS)
		{
			UTIL_createButtonsPanel();
			haveButtons = true;
		}
	}
	if(!haveMinimap) UTIL_createMinimapPanel();
	if(!haveHealth) UTIL_createHealthPanel();
	if(!haveInventory) UTIL_createInventoryPanel();
	if(!haveButtons) UTIL_createButtonsPanel();

	if(m_rightPanel != GUI_PANEL_MAIN)
	{
		Sint32 panels = m_rightPanel - GUI_PANEL_EXTRA_RIGHT_START;
		for(Sint32 i = 0; i <= panels; ++i)
			m_panels.push_back(new GUI_Panel(iRect(0, 0, GAME_PANEL_FIXED_WIDTH, m_windowH), GUI_PANEL_EXTRA_RIGHT_START + i));
	}
	if(m_leftPanel != GUI_PANEL_RANDOM)
	{
		Sint32 panels = m_leftPanel - GUI_PANEL_EXTRA_LEFT_START;
		for(Sint32 i = 0; i <= panels; ++i)
			m_panels.push_back(new GUI_Panel(iRect(0, 0, GAME_PANEL_FIXED_WIDTH, m_windowH), GUI_PANEL_EXTRA_LEFT_START + i));
	}
	for(std::vector<Uint32>::iterator it = m_openDialogs.begin(), end = m_openDialogs.end(); it != end; ++it)
	{
		Uint32 windowId = (*it);
		switch(windowId)
		{
			case GUI_PANEL_WINDOW_SKILLS: UTIL_toggleSkillsWindow(); break;
			case GUI_PANEL_WINDOW_BATTLE: UTIL_toggleBattleWindow(); break;
			case GUI_PANEL_WINDOW_VIP: UTIL_toggleVipWindow(); break;
			case GUI_PANEL_WINDOW_PARTY: UTIL_togglePartyWindow(); break;
			default: break;
		}
	}

	recalculateGameWindow();
	m_parentWindows.clear();

	std::string hotkeyMessage = "Bot Hotkeys: Ctrl+Alt+M=Mana Trainer | Ctrl+Alt+H=Auto Healer | Ctrl+Alt+C=Combo Leader | Ctrl+Alt+F=Auto Fisher | Ctrl+Alt+R=Auto Runer | Ctrl+Alt+L=Auto Logout | Ctrl+Alt+E=Auto Eat";
	g_game.processTextMessage(MessageLogin, hotkeyMessage);
}

void Engine::processGameEnd()
{
	m_ingame = false;
	g_ping = 0;
	m_botLastAction = 0;
	m_botActionCooldown = 0;
	m_manualLastAction = 0;
	m_lastPlayerMovementAction = 0;
	m_playerMovementBotBlock = 0;
	m_manualItemBotBlockUntil = 0;
	m_numpadDashActive = false;
	m_numpadDashKey = SDLK_UNKNOWN;
	m_numpadDashDirection = DIRECTION_INVALID;
	m_numpadDashLastPulse = 0;
	m_numpadDashLastTileChange = 0;
	m_numpadDashLastX = 0;
	m_numpadDashLastY = 0;
	m_numpadDashLastZ = 0;
	m_autoLogoutPending = false;
	m_autoLogoutCaptureTaken = false;
	m_autoLogoutLastAttempt = 0;
	m_autoLogoutLastLog = 0;
	m_autoLogoutPendingReason.clear();
	m_gmBaselineLogin.assign(m_autoLogoutGMs.size(), std::string());
	m_gmBaselineSet.assign(m_autoLogoutGMs.size(), false);
	m_playerWebStatus.assign(m_autoLogoutPlayers.size(), std::string());
	m_playerWebStatusSet.assign(m_autoLogoutPlayers.size(), false);
	m_lastGmPoll = 0;
	g_manualActionQueue.clear();

	//Reset map and creatures
	g_map.changeMap(DIRECTION_INVALID);
	g_map.resetCreatures();

	clearWindows();
	clearPanels();
	checkReleaseQueue();
}

void Engine::setVipData(Uint32 playerGUID, const std::string& description, Uint32 iconId, bool notifyLogin)
{
	VipData vip;
	vip.iconId = iconId;
	vip.description = description;
	vip.notifyLogin = notifyLogin;
	m_vipData[playerGUID] = vip;
}

VipData* Engine::getVipData(Uint32 playerGUID)
{
	std::map<Uint32, VipData>::iterator it = m_vipData.find(playerGUID);
	if(it != m_vipData.end())
		return &it->second;

	return NULL;
}
