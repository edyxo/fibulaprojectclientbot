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

#ifndef __FILE_ENGINE_h_
#define __FILE_ENGINE_h_

#include "defines.h"
#include "position.h"

struct Position;

class Surface
{
	public:
		Surface() {}
		virtual ~Surface() {}

		// non-copyable
		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;

		// non-moveable
		Surface(Surface&&) = delete;
		Surface& operator=(Surface&&) = delete;

		virtual bool isSupported() = 0;
		virtual const char* getName() = 0;
		virtual const char* getSoftware() = 0;
		virtual const char* getHardware() = 0;
		virtual Uint32 getVRAM() = 0;

		virtual void init() = 0;
		virtual void doResize(Sint32 w, Sint32 h) = 0;
		virtual void spriteManagerReset() = 0;
		virtual unsigned char* getScreenPixels(Sint32& width, Sint32& height, bool& bgra) = 0;
		//because we rather use raw data from pixel buffer that why we have "bgra" which indicate
		//if the pixels are orderer BGRA or RGBA if false other formats gets conversed to RGBA

		virtual void beginScene() = 0;
		virtual void endScene() = 0;

		virtual void drawLightMap_old(LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height) = 0;
		virtual void drawLightMap_new(LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height) = 0;
		virtual void drawGameScene(Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h) = 0;
		virtual void beginGameScene() = 0;
		virtual void endGameScene() = 0;

		virtual void setClipRect(Sint32 x, Sint32 y, Sint32 w, Sint32 h) = 0;
		virtual void disableClipRect() = 0;
		virtual void drawRectangle(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 lineWidth, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
		virtual void fillRectangle(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;

		virtual void drawFont(Uint16 pictureId, Sint32 x, Sint32 y, const std::string& text, size_t pos, size_t len, Uint8 r, Uint8 g, Uint8 b, Sint16 cX[256], Sint16 cY[256], Sint16 cW[256], Sint16 cH[256]) = 0;
		virtual void drawBackground(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h) = 0;
		virtual void drawPictureRepeat(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h) = 0;
		virtual void drawPicture(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 x, Sint32 y, Sint32 w, Sint32 h) = 0;

		virtual void drawSprite(Uint32 spriteId, Sint32 x, Sint32 y) = 0;
		virtual void drawSprite(Uint32 spriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh) = 0;
		virtual void drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Uint32 outfitColor) = 0;
		virtual void drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Uint32 outfitColor) = 0;

		virtual void drawAutomapTile(Uint32 currentArea, bool& recreate, Uint8 color[256][256], Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh) = 0;
};

class GUI_Window;
class GUI_Panel;
class GUI_PanelWindow;
class GUI_Element;
class GUI_Description;
class GUI_ContextMenu;
class ThingType;
class Creature;
class ItemUI;
class Item;
class Engine
{
	public:
		Engine();

		// non-copyable
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		// non-moveable
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

		void loadCFG();
		void saveCFG();

		void attachFullScreenInfo();
		void run();
		void terminate();
		void parseCommands(int argc, char* argv[]);

		bool RecreateWindow(bool vulkan, bool opengl = false);
		bool init();

		void initFont(Uint8 font, Sint32 width, Sint32 height, Sint16 hchars, Sint16 vchars, Sint16 maxchw, Sint16 maxchh, Sint16 spaceh);
		Uint32 calculateFontWidth(Uint8 fontId, const std::string& text, size_t pos, size_t len);
		Uint32 calculateFontWidth(Uint8 fontId, const std::string& text);
		SDL_INLINE Uint32 calculateFontGlyphWidth(Uint8 fontId, const Uint8 glyph) {return (m_charw[fontId][glyph] + m_charx[fontId][0]);}
		SDL_INLINE Sint32 getFontSpace(Uint8 fontId) {return m_chary[fontId][0];}

		void exitGame();
		void checkReleaseQueue();
		void updateThink();

		void clearWindows();
		void addWindow(GUI_Window* pWindow, bool topMost = false);
		void removeWindow(GUI_Window* pWindow);
		GUI_Window* getCurrentWindow() {return m_actWindow;}
		GUI_Window* getWindow(Uint32 internalID);

		void onKeyDown(SDL_Event& event);
		void onKeyUp(SDL_Event& event);
		void onMouseMove(Sint32 x, Sint32 y);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);
		void onWheel(Sint32 x, Sint32 y, bool wheelUP);
		void onTextInput(const char* textInput);

		void windowResize(Sint32 width, Sint32 height);
		void windowMoved(Sint32 x, Sint32 y);
		void windowMinimized() {m_maximized = false;}
		void windowMaximized() {m_maximized = true;}
		void windowRestored() {m_maximized = false;}
		void takeScreenshot(void* data1, void* data2);

		HotkeyUsage* getHotkey(SDL_Keycode key, Uint16 mods);
		void bindHotkey(ClientHotkeyKeys hotKey, SDL_Keycode key, Uint16 mods, ClientHotkeys hotkeyType);
		void resetToDefaultHotkeys(bool wasd);

		Sint32 calculateMainHeight();
		void recalculateGameWindow();
		Sint32 getConsoleHeight() {return m_consoleHeight;}
		void setConsoleHeight(Sint32 height);

		void update();
		void redraw();
		void renderDevRecorderStatus();
		void renderUIInspector();
		void notePlayerMovementAction(Uint32 blockMs = 450);
		void noteManualItemInteraction(Uint32 blockMs = 1500);
		bool canRunBotAction(Uint32 movementBlockMs = 350);
		bool hasPendingManualAction();
		bool shouldPauseWalkForManualAction();
		void markManualActionPausedWalk();
		size_t getManualActionQueueSize();
		bool isManualActionWalkPaused();
		void updateManualActionQueue();
		bool shouldDelayManualAction(Uint32 enqueueTime, bool safeWhileWalking = false);
		void sendManualMove(const Position& fromPos, Uint16 itemid, Uint8 stackpos, const Position& toPos, Uint16 count);
		void sendManualUseItem(const Position& position, Uint16 itemId, Uint8 stackpos, Uint8 index);
		void sendManualUseItemEx(const Position& fromPos, Uint16 itemId, Uint8 fromStackPos, const Position& toPos, Uint16 toItemId, Uint8 toStackPos);
		bool sendBotSay(MessageMode mode, Uint16 channelId, const std::string& receiver, const std::string& message, Uint32 cooldownMs = 350, Uint32 movementBlockMs = 350);
		bool sendBotMove(const Position& fromPos, Uint16 itemid, Uint8 stackpos, const Position& toPos, Uint16 count, Uint32 cooldownMs = 350, Uint32 movementBlockMs = 350);
		bool sendBotUseItem(const Position& position, Uint16 itemId, Uint8 stackpos, Uint8 index, Uint32 cooldownMs = 350, Uint32 movementBlockMs = 350);
		bool sendBotUseItemEx(const Position& fromPos, Uint16 itemId, Uint8 fromStackPos, const Position& toPos, Uint16 toItemId, Uint8 toStackPos, Uint32 cooldownMs = 350, Uint32 movementBlockMs = 350);
		bool sendBotUseOnCreature(const Position& position, Uint16 itemId, Uint8 stackpos, Uint32 creatureId, Uint32 cooldownMs = 350, Uint32 movementBlockMs = 350);
		SDL_INLINE bool getManaTrainerEnabled() {return m_manaTrainer;}
		SDL_INLINE const std::string& getManaTrainerSpell() {return m_manaTrainerSpell;}
		SDL_INLINE Uint8 getManaTrainerManaPercent() {return m_manaTrainerManaPercent;}
		SDL_INLINE Uint32 getManaTrainerDelay() {return m_manaTrainerDelay;}
		SDL_INLINE void setManaTrainerOptions(bool enabled, const std::string& spell, Uint8 manaPercent, Uint32 delay)
		{
			m_manaTrainer = enabled;
			m_manaTrainerSpell = (spell.empty() ? "utevo lux" : spell);
			m_manaTrainerManaPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, manaPercent))));
			m_manaTrainerDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, delay));
			m_manaTrainerLastCast = 0;
		}

		SDL_INLINE bool getAutoFisherEnabled() {return m_autoFisher;}
		SDL_INLINE Uint32 getAutoFisherDelay() {return m_autoFisherDelayMin;}
		SDL_INLINE Uint32 getAutoFisherDelayMin() {return m_autoFisherDelayMin;}
		SDL_INLINE Uint32 getAutoFisherDelayMax() {return m_autoFisherDelayMax;}
		SDL_INLINE void setAutoFisherOptions(bool enabled, Uint32 delay)
		{
			setAutoFisherOptions(enabled, delay, delay);
		}
		SDL_INLINE void setAutoFisherOptions(bool enabled, Uint32 delayMin, Uint32 delayMax)
		{
			m_autoFisher = enabled;
			m_autoFisherDelayMin = UTIL_min<Uint32>(30000, delayMin);
			m_autoFisherDelayMax = UTIL_min<Uint32>(30000, delayMax);
			if(m_autoFisherDelayMax < m_autoFisherDelayMin)
				m_autoFisherDelayMax = m_autoFisherDelayMin;
			if(m_autoFisherDelayMax < 1000)
				m_autoFisherDelayMax = 1000;
			m_autoFisherLastCast = 0;
			m_autoFisherLastFishMove = 0;
			m_autoFisherNextCastDelay = 0;
			m_autoFisherTargetIndex = 0;
		}

		SDL_INLINE bool getAutoLogoutEnabled() {return m_autoLogout;}
		SDL_INLINE bool getAutoLogoutPK() {return m_autoLogoutPK;}
		SDL_INLINE bool getAutoLogoutPlayer() {return m_autoLogoutPlayer;}
		SDL_INLINE Uint32 getAutoLogoutTimerMin() {return m_autoLogoutTimerMin;}
		SDL_INLINE Uint32 getAutoLogoutPollSec() {return m_gmPollInterval / 1000;}
		SDL_INLINE const std::vector<std::string>& getAutoLogoutGMs() {return m_autoLogoutGMs;}
		SDL_INLINE const std::vector<std::string>& getAutoLogoutPlayers() {return m_autoLogoutPlayers;}
		SDL_INLINE bool getAlertCaptureEnabled() {return m_alertCaptureEnabled;}
		SDL_INLINE const std::string& getDiscordWebhook() {return m_discordWebhook;}
		SDL_INLINE void setAlertOptions(bool captureEnabled, const std::string& discordWebhook)
		{
			m_alertCaptureEnabled = captureEnabled;
			m_discordWebhook = discordWebhook;
		}
		void captureAlertEvent(const std::string& reason);
		SDL_INLINE bool isGmBaselineSet(size_t i) const { return i < m_gmBaselineSet.size() && m_gmBaselineSet[i]; }
		SDL_INLINE const std::string& getGmBaseline(size_t i) const { static std::string empty; return i < m_gmBaselineLogin.size() ? m_gmBaselineLogin[i] : empty; }
		SDL_INLINE void setGmBaseline(size_t i, const std::string& val)
		{
			if(i >= m_gmBaselineLogin.size()) { m_gmBaselineLogin.resize(i + 1); m_gmBaselineSet.resize(i + 1, false); }
			m_gmBaselineLogin[i] = val;
			m_gmBaselineSet[i] = true;
		}
		SDL_INLINE bool isPlayerWebStatusSet(size_t i) const { return i < m_playerWebStatusSet.size() && m_playerWebStatusSet[i]; }
		SDL_INLINE const std::string& getPlayerWebStatus(size_t i) const { static std::string empty; return i < m_playerWebStatus.size() ? m_playerWebStatus[i] : empty; }
		SDL_INLINE void setPlayerWebStatus(size_t i, const std::string& val)
		{
			if(i >= m_playerWebStatus.size()) { m_playerWebStatus.resize(i + 1); m_playerWebStatusSet.resize(i + 1, false); }
			m_playerWebStatus[i] = val;
			m_playerWebStatusSet[i] = true;
		}
		void setAutoLogoutOptions(bool enabled, bool checkPK, bool checkPlayer, Uint32 timerMin, Uint32 pollSec,
			const std::vector<std::string>& gmList, const std::vector<std::string>& playerList)
		{
			m_autoLogout = enabled;
			m_autoLogoutPK = checkPK;
			m_autoLogoutPlayer = checkPlayer;
			m_autoLogoutTimerMin = timerMin;
			m_gmPollInterval = UTIL_max<Uint32>(5, UTIL_min<Uint32>(300, pollSec)) * 1000;
			m_autoLogoutGMs = gmList;
			m_autoLogoutPlayers = playerList;
			m_gmBaselineLogin.assign(gmList.size(), std::string());
			m_gmBaselineSet.assign(gmList.size(), false);
			m_playerWebStatus.assign(playerList.size(), std::string());
			m_playerWebStatusSet.assign(playerList.size(), false);
			m_autoLogoutSessionStart = 0;
			m_lastGmPoll = 0;
			m_autoLogoutPending = false;
			m_autoLogoutCaptureTaken = false;
			m_autoLogoutLastAttempt = 0;
			m_autoLogoutLastLog = 0;
			m_autoLogoutPendingReason.clear();
		}
		void doAutoLogout(const char* reason);
		SDL_INLINE bool getAutoLogoutOnPM() {return m_autoLogoutOnPM;}
		SDL_INLINE bool getAutoLogoutOnBroadcast() {return m_autoLogoutOnBroadcast;}
		SDL_INLINE bool getPlayerAlarmEnabled() {return m_playerAlarmEnabled;}
		SDL_INLINE void setAutoLogoutOnPM(bool v) {m_autoLogoutOnPM = v;}
		SDL_INLINE void setAutoLogoutOnBroadcast(bool v) {m_autoLogoutOnBroadcast = v;}
		SDL_INLINE void setPlayerAlarmEnabled(bool v) {m_playerAlarmEnabled = v;}

		SDL_INLINE bool getAutoRunerEnabled() {return m_autoRuner;}
		SDL_INLINE const std::string& getAutoRunerSpell() {return m_autoRunerSpell;}
		SDL_INLINE Uint8 getAutoRunerManaPercent() {return m_autoRunerManaPercent;}
		SDL_INLINE Uint32 getAutoRunerDelay() {return m_autoRunerDelay;}
		SDL_INLINE void setAutoRunerOptions(bool enabled, const std::string& spell, Uint8 manaPercent, Uint32 delay)
		{
			m_autoRuner = enabled;
			m_autoRunerSpell = (spell.empty() ? "adori vis" : spell);
			m_autoRunerManaPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, manaPercent))));
			m_autoRunerDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, delay));
			m_autoRunerLastCast = 0;
			m_autoRunerPendingMove = false;
			m_autoRunerPendingMoveTime = 0;
			m_autoRunerPendingBlankMove = false;
			m_autoRunerHasReturnContainer = false;
		}

		SDL_INLINE bool getAutoEaterEnabled() {return m_autoEater;}
		SDL_INLINE Uint16 getAutoEaterFoodId() {return m_autoEaterFoodId;}
		SDL_INLINE Uint32 getAutoEaterDelay() {return m_autoEaterDelay;}
		SDL_INLINE void setAutoEaterOptions(bool enabled, Uint16 foodId, Uint32 delay)
		{
			m_autoEater = enabled;
			m_autoEaterFoodId = SDL_static_cast(Uint16, UTIL_max<Uint32>(100, UTIL_min<Uint32>(65535, SDL_static_cast(Uint32, foodId))));
			m_autoEaterDelay = UTIL_max<Uint32>(1000, UTIL_min<Uint32>(300000, delay));
			m_autoEaterLastUse = 0;
		}

		SDL_INLINE bool getAutoHealerEnabled() {return m_autoHealer;}
		SDL_INLINE const std::string& getAutoHealerSpell() {return m_autoHealerSpell;}
		SDL_INLINE Uint8 getAutoHealerHPPercent() {return m_autoHealerHPPercent;}
		SDL_INLINE Uint32 getAutoHealerDelay() {return m_autoHealerDelay;}
		SDL_INLINE bool getAutoHealerSelfUHEnabled() {return m_autoHealerSelfUH;}
		SDL_INLINE Uint16 getAutoHealerUHItemId() {return m_autoHealerUHItemId;}
		SDL_INLINE Uint8 getAutoHealerSelfUHPercent() {return m_autoHealerSelfUHPercent;}
		SDL_INLINE bool getAutoHealerFriendUHEnabled() {return m_autoHealerFriendUH;}
		SDL_INLINE const std::string& getAutoHealerFriendNames() {return m_autoHealerFriendNames;}
		SDL_INLINE Uint8 getAutoHealerFriendUHPercent() {return m_autoHealerFriendUHPercent;}
		SDL_INLINE void setAutoHealerOptions(bool enabled, const std::string& spell, Uint8 hpPercent, Uint32 delay)
		{
			m_autoHealer = enabled;
			m_autoHealerSpell = (spell.empty() ? "exura" : spell);
			m_autoHealerHPPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, hpPercent))));
			m_autoHealerDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, delay));
			m_autoHealerLastCast = 0;
		}
		SDL_INLINE void setAutoHealerOptions(bool enabled, const std::string& spell, Uint8 hpPercent, Uint32 delay,
			bool selfUH, Uint16 uhItemId, Uint8 selfUHPercent, bool friendUH, const std::string& friendNames, Uint8 friendUHPercent)
		{
			setAutoHealerOptions(enabled, spell, hpPercent, delay);
			m_autoHealerSelfUH = selfUH;
			m_autoHealerUHItemId = uhItemId;
			m_autoHealerSelfUHPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, selfUHPercent))));
			m_autoHealerFriendUH = friendUH;
			m_autoHealerFriendNames = friendNames;
			m_autoHealerFriendUHPercent = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(100, SDL_static_cast(Uint32, friendUHPercent))));
		}

		SDL_INLINE bool getComboLeaderEnabled() {return m_comboLeader;}
		SDL_INLINE const std::string& getComboLeaderName() {return m_comboLeaderName;}
		SDL_INLINE Uint32 getComboLeaderDelay() {return m_comboLeaderDelay;}
		static const size_t COMBO_LEADER_MAPPING_COUNT = 5;
		SDL_INLINE size_t getComboLeaderMappingCount() const {return COMBO_LEADER_MAPPING_COUNT;}
		SDL_INLINE const std::string& getComboLeaderMappingName(size_t index) const
		{
			static const std::string empty;
			return (index < COMBO_LEADER_MAPPING_COUNT ? m_comboLeaderMappingNames[index] : empty);
		}
		SDL_INLINE Uint16 getComboLeaderMappingEffect(size_t index) const
		{
			return (index < COMBO_LEADER_MAPPING_COUNT ? m_comboLeaderMappingEffects[index] : 0);
		}
		SDL_INLINE Uint16 getComboLeaderMappingRune(size_t index) const
		{
			return (index < COMBO_LEADER_MAPPING_COUNT ? m_comboLeaderMappingRunes[index] : 0);
		}
		SDL_INLINE Uint16 getComboLeaderSdEffect() const {return getComboLeaderMappingEffect(0);}
		SDL_INLINE Uint16 getComboLeaderSdRune() const {return getComboLeaderMappingRune(0);}
		SDL_INLINE Uint16 getComboLeaderHmmEffect() const {return getComboLeaderMappingEffect(1);}
		SDL_INLINE Uint16 getComboLeaderHmmRune() const {return getComboLeaderMappingRune(1);}
		SDL_INLINE void setComboLeaderOptions(bool enabled, const std::string& leaderName, Uint32 delay)
		{
			m_comboLeader = enabled;
			m_comboLeaderName = leaderName;
			m_comboLeaderDelay = UTIL_max<Uint32>(250, UTIL_min<Uint32>(10000, delay));
			m_comboLeaderLastCast = 0;
		}
		SDL_INLINE void setComboLeaderOptions(bool enabled, const std::string& leaderName, Uint16 sdEffect, Uint16 sdRune,
			Uint16 hmmEffect, Uint16 hmmRune, Uint32 delay)
		{
			setComboLeaderOptions(enabled, leaderName, delay);
			setComboLeaderMapping(0, "SD", sdEffect, sdRune);
			setComboLeaderMapping(1, "HMM", hmmEffect, hmmRune);
		}
		SDL_INLINE void setComboLeaderMapping(size_t index, const std::string& name, Uint16 effect, Uint16 rune)
		{
			if(index >= COMBO_LEADER_MAPPING_COUNT)
				return;

			m_comboLeaderMappingNames[index] = name.empty() ? "Rune" : name;
			m_comboLeaderMappingEffects[index] = effect;
			m_comboLeaderMappingRunes[index] = rune;
		}
		void handleComboLeaderDistanceEffect(const Position& fromPos, const Position& toPos, Uint16 effectId);

		SDL_INLINE bool getMagicWallEnabled() {return m_magicWall;}
		SDL_INLINE const std::string& getMagicWallSpell() {return m_magicWallSpell;}
		SDL_INLINE Uint8 getMagicWallDist() {return m_magicWallDist;}
		SDL_INLINE Uint32 getMagicWallDelay() {return m_magicWallDelay;}
		SDL_INLINE void setMagicWallOptions(bool enabled, const std::string& spell, Uint8 dist, Uint32 delay)
		{
			m_magicWall = enabled;
			m_magicWallSpell = (spell.empty() ? "exevo gran mas vis" : spell);
			m_magicWallDist = SDL_static_cast(Uint8, UTIL_max<Uint32>(1, UTIL_min<Uint32>(7, SDL_static_cast(Uint32, dist))));
			m_magicWallDelay = UTIL_max<Uint32>(500, UTIL_min<Uint32>(60000, delay));
			m_magicWallLastCast = 0;
		}

		SDL_INLINE void recordExhaust(Uint32 creatureId)
		{
			m_exhaustTimers[creatureId] = SDL_GetTicks();
		}
		SDL_INLINE bool isExhausted(Uint32 creatureId)
		{
			auto it = m_exhaustTimers.find(creatureId);
			if(it == m_exhaustTimers.end()) return false;
			return (SDL_GetTicks() - it->second) < 2000;
		}
		SDL_INLINE float getExhaustRatio(Uint32 creatureId)
		{
			auto it = m_exhaustTimers.find(creatureId);
			if(it == m_exhaustTimers.end()) return 0.0f;
			Uint32 elapsed = SDL_GetTicks() - it->second;
			if(elapsed >= 2000) return 0.0f;
			return 1.0f - (elapsed / 2000.0f);
		}

		void drawFont(Uint8 fontId, Sint32 x, Sint32 y, const std::string& text, Uint8 r, Uint8 g, Uint8 b, Sint32 align, size_t pos, size_t len);
		void drawFont(Uint8 fontId, Sint32 x, Sint32 y, const std::string& text, Uint8 r, Uint8 g, Uint8 b, Sint32 align);

		void drawItem(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation);
		void drawOutfit(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation, Uint32 outfitColor);
		void drawEffect(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation);
		void drawDistanceEffect(ThingType* thing, Sint32 x, Sint32 y, Sint32 scaled, Uint8 xPattern, Uint8 yPattern, Uint8 zPattern, Uint8 animation);

		unsigned char* LoadSprite(Uint32 spriteId, bool bgra);
		unsigned char* LoadSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Uint32 outfitColor, bool bgra);
		unsigned char* LoadPicture(Uint16 pictureId, bool bgra, Sint32& width, Sint32& height);

		void issueNewConnection(bool protocolGame);
		void releaseConnection();

		void initMove(Uint16 posX, Uint16 posY, Uint8 posZ);
		static void standardThingEvent(Uint32 event, Sint32 status);
		GUI_ContextMenu* createThingContextMenu(Creature* creature, ItemUI* itemui, Item* item);
		void enableMoveItem(Sint32 x, Sint32 y);
		void setActionData(ClientActions data, Uint32 creatureId, Uint16 itemId, Uint16 posX, Uint16 posY, Uint8 posZ, Uint8 posStack);
		void setAction(ClientActions action);
		SDL_INLINE ClientActionData& getActionData(Uint8 action) {return m_actionDataStructure[action];}
		SDL_INLINE ClientActions getAction() {return m_actionData;}
	
		void showContextMenu(GUI_ContextMenu* menu, Sint32 mouseX, Sint32 mouseY);
		void showDescription(Sint32 mouseX, Sint32 mouseY, const std::string& description, Uint32 delay = 500);
		bool addToPanel(GUI_PanelWindow* pPanel, Sint32 preferredPanel = -1);
		void removePanelWindow(GUI_PanelWindow* pPanel);
		GUI_PanelWindow* getPanel(Uint32 internalID);
		void clearPanels();
		void checkPanelWindows(GUI_PanelWindow* pPanel, Sint32 x, Sint32 y);
		void resizePanel(GUI_PanelWindow* pPanel, Sint32 x, Sint32 y);

		void processGameStart();
		void processGameEnd();

		SDL_INLINE Sint32 getWindowWidth() {return m_windowW;}
		SDL_INLINE Sint32 getWindowHeight() {return m_windowH;}
		SDL_INLINE Sint32 getLeftPanel() {return m_leftPanel;}
		SDL_INLINE Sint32 getRightPanel() {return m_rightPanel;}
		bool hasExtraLeftSidebar();
		bool hasExtraRightSidebar();
		void setExtraLeftSidebar(bool enabled);
		void setExtraRightSidebar(bool enabled);
		void setDevMode(bool enabled);

		SDL_INLINE void setEngineId(Uint8 engine) {m_engine = engine;}
		SDL_INLINE Uint8 getEngineId() {return m_engine;}
		SDL_INLINE std::unique_ptr<Surface>& getRender() {return m_surface;}

		SDL_INLINE void setClientHost(std::string clientHost) {m_clientHost = std::move(clientHost);}
		SDL_INLINE void setClientPort(std::string clientPort) {m_clientPort = std::move(clientPort);}
		SDL_INLINE void setClientProxy(std::string clientProxy) {m_clientProxy = std::move(clientProxy);}
		SDL_INLINE void setClientProxyAuth(std::string clientProxyAuth) {m_clientProxyAuth = std::move(clientProxyAuth);}
		SDL_INLINE std::string& getClientHost() {return m_clientHost;}
		SDL_INLINE std::string& getClientPort() {return m_clientPort;}
		SDL_INLINE std::string& getClientProxy() {return m_clientProxy;}
		SDL_INLINE std::string& getClientProxyAuth() {return m_clientProxyAuth;}
		
		SDL_INLINE void setSharpening(bool vsync) {m_sharpening = vsync;}
		SDL_INLINE bool isSharpening() {return m_sharpening;}
		SDL_INLINE void setVsync(bool vsync) {m_vsync = vsync; m_controlFPS = !m_unlimitedFPS;}
		SDL_INLINE bool isVsync() {return m_vsync;}
		SDL_INLINE void setAntialiasing(Uint8 antialiasing) {m_antialiasing = antialiasing;}
		SDL_INLINE bool hasAntialiasing() {return m_antialiasing >= CLIENT_ANTIALIASING_NORMAL;}
		SDL_INLINE Uint8 getAntialiasing() {return m_antialiasing;}

		SDL_INLINE void setUnlimitedFPS(bool unlimitedFPS) {m_unlimitedFPS = unlimitedFPS; m_controlFPS = !m_unlimitedFPS;}
		SDL_INLINE bool isUnlimitedFPS() {return m_unlimitedFPS;}
		SDL_INLINE bool isControlledFPS() {return m_controlFPS;}

		SDL_INLINE void setFullScreen(bool fullscreen) {m_fullscreen = fullscreen;}
		SDL_INLINE void setFullScreenMode(Sint32 w, Sint32 h, Sint32 bits, Sint32 hz) {m_fullScreenWidth = w; m_fullScreenHeight = h; m_fullScreenBits = bits; m_fullScreenHZ = hz;}
		SDL_INLINE bool hasFullScreen() {return m_fullscreen;}
		SDL_INLINE Sint32 getFullScreenWidth() {return m_fullScreenWidth;}
		SDL_INLINE Sint32 getFullScreenHeight() {return m_fullScreenHeight;}
		SDL_INLINE Sint32 getFullScreenBits() {return m_fullScreenBits;}
		SDL_INLINE Sint32 getFullScreenHZ() {return m_fullScreenHZ;}
		
		SDL_INLINE void setAttackMode(Uint8 attackMode) {m_attackMode = attackMode;}
		SDL_INLINE void setChaseMode(Uint8 chaseMode) {m_chaseMode = chaseMode;}
		SDL_INLINE void setSecureMode(Uint8 secureMode) {m_secureMode = secureMode;}
		SDL_INLINE void setPvpMode(Uint8 pvpMode) {m_pvpMode = pvpMode;}
		SDL_INLINE void setAmbientLight(Uint8 ambientLight) {m_lightAmbient = ambientLight;}
		SDL_INLINE void setLevelSeparator(Uint8 levelSeparator) {m_levelSeparator = levelSeparator;}
		SDL_INLINE void setLightMode(Uint8 lightMode) {m_lightMode = lightMode;}
		SDL_INLINE Uint8 getAttackMode() {return m_attackMode;}
		SDL_INLINE Uint8 getChaseMode() {return m_chaseMode;}
		SDL_INLINE Uint8 getSecureMode() {return m_secureMode;}
		SDL_INLINE Uint8 getPvpMode() {return m_pvpMode;}
		SDL_INLINE Uint8 getAmbientLight() {return m_lightAmbient;}
		SDL_INLINE Uint8 getLevelSeparator() {return m_levelSeparator;}
		SDL_INLINE Uint8 getLightMode() {return (m_fullBrightness ? CLIENT_LIGHT_MODE_NONE : m_lightMode);}
		SDL_INLINE Uint8 getRealLightMode() {return m_lightMode;}
		SDL_INLINE bool getFullBrightness() {return m_fullBrightness;}
		SDL_INLINE void setFullBrightness(bool v) {m_fullBrightness = v;}
		SDL_INLINE bool getHideWASD() {return m_hideWASD;}
		SDL_INLINE void setHideWASD(bool v) {m_hideWASD = v;}
		SDL_INLINE bool getHideHPManaPercent() {return m_hideHPManaPercent;}
		SDL_INLINE void setHideHPManaPercent(bool v) {m_hideHPManaPercent = v;}
		SDL_INLINE bool getLowProfileMode() {return m_lowProfileMode;}
		SDL_INLINE void setLowProfileMode(bool v) {m_lowProfileMode = v;}

		SDL_INLINE Sint8 getExivaDir() {return m_exivaDir;}
		SDL_INLINE const std::string& getExivaName() {return m_exivaName;}
		SDL_INLINE void setExiva(Sint8 dir, const std::string& name) {m_exivaDir = dir; m_exivaName = name; m_exivaTick = SDL_GetTicks(); m_exivaEscCount = 0;}
		SDL_INLINE void clearExiva() {m_exivaDir = -1; m_exivaName.clear();}
		SDL_INLINE bool onExivaEsc() {if(m_exivaDir < 0) return false; if(++m_exivaEscCount >= 2) {clearExiva(); return true;} return false;}
		SDL_INLINE bool isExivaActive() {return m_exivaDir >= 0 && (SDL_GetTicks() - m_exivaTick) < 30000;}

		SDL_INLINE void setBattleSortMethod(SortMethods sortMethod) {m_battleSortMethod = sortMethod;}
		SDL_INLINE void setBuddySortMethod(VipSortMethods sortMethod) {m_buddySortmethod = sortMethod;}
		SDL_INLINE void setBuddyHideOffline(bool hideOffline) {m_buddyHideOffline = hideOffline;}
		SDL_INLINE void setBuddyHideGroups(bool hideGroups) {m_buddyHideGroups = hideGroups;}
		SDL_INLINE SortMethods getBattleSortMethod() {return m_battleSortMethod;}
		SDL_INLINE VipSortMethods getBuddySortMethod() {return m_buddySortmethod;}
		SDL_INLINE bool getBuddyHideOffline() {return m_buddyHideOffline;}
		SDL_INLINE bool getBuddyHideGroups() {return m_buddyHideGroups;}
		
		SDL_INLINE void setMotdNumber(Uint32 motdNumber) {m_motdNumber = motdNumber;}
		SDL_INLINE void setMotdText(std::string motdText) {m_motdText = std::move(motdText);}
		SDL_INLINE Uint32 getMotdNumber() {return m_motdNumber;}
		SDL_INLINE std::string& getMotdText() {return m_motdText;}

		SDL_INLINE void setAccountSessionKey(std::string sessionKey) {m_accountSessionKey = std::move(sessionKey);}
		SDL_INLINE void setAccountName(std::string accountName) {m_accountName = std::move(accountName);}
		SDL_INLINE void setAccountPassword(std::string accountPassword) {m_accountPassword = std::move(accountPassword);}
		SDL_INLINE void setAccountToken(std::string accountToken) {m_accountToken = std::move(accountToken);}
		SDL_INLINE void setAccountCharList(std::vector<CharacterDetail>& characters) {m_characters = std::move(characters);}
		SDL_INLINE void setAccountStatus(Uint8 accountStatus) {m_accountStatus = accountStatus;}
		SDL_INLINE void setAccountSubstatus(Uint8 accountSubstatus) {m_accountSubStatus = accountSubstatus;}
		SDL_INLINE void setAccountPremDays(Uint32 accountPremDays) {m_accountPremDays = accountPremDays;}
		SDL_INLINE void setAccountNewCharList(bool newCharacterList) {m_newCharacterList = newCharacterList;}
		SDL_INLINE void setCharacterSelectId(Sint32 selectId) {m_characterSelectId = selectId;}
		SDL_INLINE std::string& getAccountSessionKey() {return m_accountSessionKey;}
		SDL_INLINE std::string& getAccountName() {return m_accountName;}
		SDL_INLINE std::string& getAccountPassword() {return m_accountPassword;}
		SDL_INLINE std::string& getAccountToken() {return m_accountToken;}
		SDL_INLINE std::string& getCharacterName() {return m_characters[SDL_static_cast(size_t, m_characterSelectId)].name;}
		SDL_INLINE std::string& getCharacterWorldName() {return m_characters[SDL_static_cast(size_t, m_characterSelectId)].worldName;}
		SDL_INLINE std::vector<CharacterDetail>& getAccountCharList() {return m_characters;}
		SDL_INLINE Uint8 getAccountStatus() {return m_accountStatus;}
		SDL_INLINE Uint8 getAccountSubstatus() {return m_accountSubStatus;}
		SDL_INLINE Uint32 getAccountPremDays() {return m_accountPremDays;}
		SDL_INLINE bool getAccountNewCharList() {return m_newCharacterList;}
		SDL_INLINE Sint32 getCharacterSelectId() {return m_characterSelectId;}

		SDL_INLINE iRect& getGameWindowRect() {return m_gameWindowRect;}
		SDL_INLINE float getGameWindowScale() {return m_scale;}
		SDL_INLINE Sint32 getGameWindowScaleSize() {return m_scaledSize;}
		SDL_INLINE bool isIngame() {return m_ingame;}
		SDL_INLINE bool isWASDMode() {return m_wasdMode;}
		void setWASDMode(bool enable);

		SDL_INLINE void setClassicControl(bool classicControl) {m_classicControl = classicControl;}
		SDL_INLINE void setAutoChaseOff(bool chaseOff) {m_autoChaseOff = chaseOff;}
		SDL_INLINE void setShowNames(bool showNames) {m_showNames = showNames;}
		SDL_INLINE void setShowMarks(bool showMarks) {m_showMarks = showMarks;}
		SDL_INLINE void setShowPvPFrames(bool showPvPFrames) {m_showPvPFrames = showPvPFrames;}
		SDL_INLINE void setShowIcons(bool showIcons) {m_showIcons = showIcons;}
		SDL_INLINE void setShowTextualEffects(bool showTextualEffects) {m_showTextualEffects = showTextualEffects;}
		SDL_INLINE void setShowCooldown(bool showCooldown) {m_showCooldown = showCooldown;}
		SDL_INLINE bool hasClassicControl() {return m_classicControl;}
		SDL_INLINE bool hasAutoChaseOff() {return m_autoChaseOff;}
		SDL_INLINE bool hasShowNames() {return m_showNames;}
		SDL_INLINE bool hasShowMarks() {return m_showMarks;}
		SDL_INLINE bool hasShowPvPFrames() {return m_showPvPFrames;}
		SDL_INLINE bool hasShowIcons() {return m_showIcons;}
		SDL_INLINE bool hasShowTextualEffects() {return m_showTextualEffects;}
		SDL_INLINE bool hasShowCooldown() {return m_showCooldown;}
		SDL_INLINE bool hasDevMode() {return m_devMode;}
		SDL_INLINE void setShowDebugCoordinates(bool showDebugCoordinates) {m_showDebugCoordinates = showDebugCoordinates;}
		SDL_INLINE bool hasShowDebugCoordinates() {return m_showDebugCoordinates;}

		SDL_INLINE void setShowInfoMessages(bool infoMessages) {m_showInfoMessages = infoMessages;}
		SDL_INLINE void setShowEventMessages(bool eventMessages) {m_showEventMessages = eventMessages;}
		SDL_INLINE void setShowStatusMessages(bool statusMessages) {m_showStatusMessages = statusMessages;}
		SDL_INLINE void setShowStatusOthersMessages(bool statusMessages) {m_showStatusOthersMessages = statusMessages;}
		SDL_INLINE void setShowTimestamps(bool timestamps) {m_showTimestamps = timestamps;}
		SDL_INLINE void setShowLevels(bool levels) {m_showLevels = levels;}
		SDL_INLINE void setShowPrivateMessages(bool privateMessages) {m_showPrivateMessages = privateMessages;}
		SDL_INLINE bool hasShowInfoMessages() {return m_showInfoMessages;}
		SDL_INLINE bool hasShowEventMessages() {return m_showEventMessages;}
		SDL_INLINE bool hasShowStatusMessages() {return m_showStatusMessages;}
		SDL_INLINE bool hasShowStatusOthersMessages() {return m_showStatusOthersMessages;}
		SDL_INLINE bool hasShowTimestamps() {return m_showTimestamps;}
		SDL_INLINE bool hasShowLevels() {return m_showLevels;}
		SDL_INLINE bool hasShowPrivateMessages() {return m_showPrivateMessages;}

		SDL_INLINE void setShowLevelBar(bool showLevelBar) {m_showLevelBar = showLevelBar;}
		SDL_INLINE void setShowStaminaBar(bool showStaminaBar) {m_showStaminaBar = showStaminaBar;}
		SDL_INLINE void setShowMagLevelBar(bool showMagLevelBar) {m_showMagLevelBar = showMagLevelBar;}
		SDL_INLINE void setShowTrainingBar(bool showTrainingBar) {m_showTrainingBar = showTrainingBar;}
		SDL_INLINE void setShowSkillBar(Skills skillId, bool showSkillBar) {m_showSkillsBar[skillId] = showSkillBar;}
		SDL_INLINE bool getShowLevelBar() {return m_showLevelBar;}
		SDL_INLINE bool getShowStaminaBar() {return m_showStaminaBar;}
		SDL_INLINE bool getShowMagLevelBar() {return m_showMagLevelBar;}
		SDL_INLINE bool getShowTrainingBar() {return m_showTrainingBar;}
		SDL_INLINE bool getShowSkillBar(Skills skillId) {return m_showSkillsBar[skillId];}
		
		SDL_INLINE void setBuySortMethod(Uint8 sortMethod) {m_buySortMethod = sortMethod;}
		SDL_INLINE void setSellSortMethod(Uint8 sortMethod) {m_sellSortMethod = sortMethod;}
		SDL_INLINE void setBuyWithBackpacks(bool buyWithBackpacks) {m_buyWithBackpacks = buyWithBackpacks;}
		SDL_INLINE void setIgnoreCapacity(bool ignoreCapacity) {m_ignoreCapacity = ignoreCapacity;}
		SDL_INLINE void setIgnoreEquiped(bool ignoreEquiped) {m_ignoreEquiped = ignoreEquiped;}
		SDL_INLINE Uint8 getBuySortMethod() {return m_buySortMethod;}
		SDL_INLINE Uint8 getSellSortMethod() {return m_sellSortMethod;}
		SDL_INLINE bool getBuyWithBackpacks() {return m_buyWithBackpacks;}
		SDL_INLINE bool getIgnoreCapacity() {return m_ignoreCapacity;}
		SDL_INLINE bool getIgnoreEquiped() {return m_ignoreEquiped;}

		SDL_INLINE void setContentWindowHeight(Uint32 windowId, Sint32 height) {m_contentWindows[windowId] = height;}
		SDL_INLINE Sint32 getContentWindowHeight(Uint32 windowId) {std::map<Uint32, Sint32>::iterator it = m_contentWindows.find(windowId); if(it != m_contentWindows.end()) return it->second; return 0;}
		SDL_INLINE Sint32 getContentWindowParent(Uint32 windowId) {std::map<Uint32, Sint32>::iterator it = m_parentWindows.find(windowId); if(it != m_parentWindows.end()) {Sint32 panelId = it->second; m_parentWindows.erase(it); return panelId;} return -1;}

		void setVipData(Uint32 playerGUID, const std::string& description, Uint32 iconId, bool notifyLogin);
		VipData* getVipData(Uint32 playerGUID);

		std::unordered_map<std::string, bool>& getBlackList() {return m_blackList;}
		std::unordered_map<std::string, bool>& getWhiteList() {return m_whiteList;}

		SDL_INLINE void setActivatedBlackList(bool activated) {m_activatedBlackList = activated;}
		SDL_INLINE void setActivatedWhiteList(bool activated) {m_activatedWhiteList = activated;}
		SDL_INLINE void setIgnoreYellingMessages(bool ignoreYelling) {m_ignoreYellingMessages = ignoreYelling;}
		SDL_INLINE void setIgnorePrivateMessages(bool ignoreMessages) {m_ignorePrivateMessages = ignoreMessages;}
		SDL_INLINE void setAllowVipMessages(bool allowMessages) {m_allowVipMessages = allowMessages;}
		SDL_INLINE bool getActivatedBlackList() {return m_activatedBlackList;}
		SDL_INLINE bool getActivatedWhiteList() {return m_activatedWhiteList;}
		SDL_INLINE bool getIgnoreYellingMessages() {return m_ignoreYellingMessages;}
		SDL_INLINE bool getIgnorePrivateMessages() {return m_ignorePrivateMessages;}
		SDL_INLINE bool getAllowVipMessages() {return m_allowVipMessages;}

		SDL_INLINE void setTopPanel(GUI_PanelWindow* newTopPanel) {m_topPanel = newTopPanel;}
		SDL_INLINE GUI_PanelWindow* getTopPanel() {return m_topPanel;}

		SDL_Window* m_window = NULL;
		Uint32 m_windowId = 0;

	protected:
		void updateUIInspector(Sint32 x, Sint32 y);
		void logUIInspector(Sint32 x, Sint32 y, const char* action);
		void recordDevEvent(const char* action, Sint32 x, Sint32 y, bool force = false);
		std::string startDevRecorderSession(const char* prefix);
		std::string getDevRecorderDirectory();
		bool saveScreenshotBMP(const char* fileName);
		std::string describeUIAt(Sint32 x, Sint32 y);
		std::string describeElement(const char* area, GUI_Element* element);
		std::string describePanelWindow(const char* area, GUI_PanelWindow* pWindow, Sint32 x, Sint32 y);
		std::string formatRect(const iRect& rect);
		std::string getElementClassName(GUI_Element* element);
		void updateManaTrainer();
		void updateAutoFisher();
		void updateAutoRuner();
		void updateAutoLogout();
		void updateAutoEater();
		void updateAutoHealer();
		void updateMagicWall();
		void startNumpadDash(SDL_Keycode key, Direction direction);
		void stopNumpadDash(SDL_Keycode key);
		void updateNumpadDash();

		std::string m_clientHost = "127.0.0.1";
		std::string m_clientPort = "7171";
		std::string m_clientProxy;
		std::string m_clientProxyAuth;

		std::string m_motdText = "No current information.";
		std::string m_accountSessionKey;
		std::string m_accountName;
		std::string m_accountPassword;
		std::string m_accountToken;
		std::string m_manaTrainerSpell = "utevo lux";
		std::vector<CharacterDetail> m_characters;

		std::vector<Uint8> m_engines;
		std::vector<GUI_Window*> m_toReleaseWindows;
		std::vector<GUI_Window*> m_windows;
		GUI_Window* m_actWindow = NULL;

		GUI_PanelWindow* m_topPanel = NULL;
		GUI_Description* m_description = NULL;
		GUI_ContextMenu* m_contextMenu = NULL;
		std::vector<GUI_Panel*> m_toReleasePanels;
		std::vector<GUI_Panel*> m_panels;
		std::unique_ptr<Surface> m_surface;

		std::map<Uint32, VipData> m_vipData;
		std::map<Uint32, Sint32> m_parentWindows;
		std::map<Uint32, Sint32> m_contentWindows;
		std::vector<Uint32> m_openDialogs;

		std::unordered_map<std::string, bool> m_blackList;
		std::unordered_map<std::string, bool> m_whiteList;
		std::map<Uint16, std::map<SDL_Keycode, size_t>> m_hotkeyFastAccess;
		std::vector<HotkeyUsage> m_hotkeys;

		iRect m_gameBackgroundRect;
		iRect m_gameWindowRect;
		iRect m_chatWindowRect;
		iRect m_uiInspectorRect;
		std::string m_uiInspectorText;
		std::string m_devRecorderDirectory;
		Sint32 m_uiInspectorMouseX = 0;
		Sint32 m_uiInspectorMouseY = 0;
		Uint32 m_devRecorderCounter = 0;
		Uint32 m_devRecorderLastDragCapture = 0;
		Uint32 m_manaTrainerLastCast = 0;
		Uint32 m_manaTrainerDelay = 2000;
		Uint32 m_autoFisherLastCast = 0;
		Uint32 m_autoFisherLastFishMove = 0;
		Uint32 m_autoFisherDelayMin = 1000;
		Uint32 m_autoFisherDelayMax = 10000;
		Uint32 m_autoFisherNextCastDelay = 0;
		size_t m_autoFisherTargetIndex = 0;
		Uint32 m_autoLogoutSessionStart = 0;
		Uint32 m_autoLogoutTimerMin = 0;
		Uint32 m_gmPollInterval = 30000;
		Uint32 m_lastGmPoll = 0;
		Uint32 m_autoLogoutLastAttempt = 0;
		Uint32 m_autoLogoutLastLog = 0;
		Sint32 m_scaledSize = 32;
		float m_scale = 1.0f;

		Uint32 m_motdNumber = 0;
		Uint32 m_accountPremDays = 0;

		Sint32 m_moveItemX = SDL_MIN_SINT32;
		Sint32 m_moveItemY = SDL_MIN_SINT32;

		iRect m_leftPanelAddRect;
		iRect m_leftPanelRemRect;
		iRect m_rightPanelAddRect;
		iRect m_rightPanelRemRect;
		Sint32 m_leftPanel;
		Sint32 m_rightPanel;
		Sint32 m_consoleHeight = 140;

		Sint32 m_fullScreenWidth = 800;
		Sint32 m_fullScreenHeight = 600;
		Sint32 m_fullScreenBits = 32;
		Sint32 m_fullScreenHZ = 60;

		Sint32 m_characterSelectId = 0;
		Sint16 m_charx[CLIENT_FONT_LAST][256], m_chary[CLIENT_FONT_LAST][256], m_charw[CLIENT_FONT_LAST][256], m_charh[CLIENT_FONT_LAST][256];
		Sint32 m_windowX = SDL_WINDOWPOS_CENTERED;
		Sint32 m_windowY = SDL_WINDOWPOS_CENTERED;
		Sint32 m_windowW = 640;
		Sint32 m_windowH = 480;
		Sint32 m_windowCachedW = 640;
		Sint32 m_windowCachedH = 480;
		Uint16 m_charPicture[CLIENT_FONT_LAST];

		ClientActionData m_actionDataStructure[2];
		ClientActions m_actionData = CLIENT_ACTION_NONE;

		Uint8 m_accountStatus = AccountStatus_Ok;
		Uint8 m_accountSubStatus = SubscriptionStatus_Free;

		Uint8 m_attackMode = ATTACKMODE_BALANCED;
		Uint8 m_chaseMode = CHASEMODE_STAND;
		Uint8 m_secureMode = SECUREMODE_SECURE;
		Uint8 m_pvpMode = PVPMODE_DOVE;
		Uint8 m_antialiasing = CLIENT_ANTIALIASING_NORMAL;
		Uint8 m_lightAmbient = 25;
		Uint8 m_levelSeparator = 80;
		Uint8 m_lightMode = CLIENT_LIGHT_MODE_OLD;
		Uint8 m_engine;

		Uint8 m_leftAddPanel = 0;
		Uint8 m_leftRemPanel = 0;
		Uint8 m_rightAddPanel = 0;
		Uint8 m_rightRemPanel = 0;
		bool m_haveExtraLeftPanel = false;
		bool m_haveExtraRightPanel = false;
		bool m_canAddLeftPanel = false;
		bool m_canAddRightPanel = false;

		SortMethods m_battleSortMethod = Sort_Ascending_Time;
		VipSortMethods m_buddySortmethod = Vip_Sort_Name;
		bool m_buddyHideOffline = false;
		bool m_buddyHideGroups = false;

		bool m_maximized = true;
		bool m_fullscreen = false;
		bool m_vsync = true;
		bool m_unlimitedFPS = true;
		bool m_controlFPS = false;
		bool m_sharpening = false;

		bool m_classicControl = true;
		bool m_autoChaseOff = true;
		bool m_showNames = true;
		bool m_showMarks = true;
		bool m_showPvPFrames = true;
		bool m_showIcons = true;
		bool m_showTextualEffects = true;
		bool m_showCooldown = true;
		bool m_manualActionPausedWalk = false;
		bool m_manualActionResumeWalk = false;

		bool m_showInfoMessages = true;
		bool m_showEventMessages = true;
		bool m_showStatusMessages = true;
		bool m_showStatusOthersMessages = true;
		bool m_showTimestamps = true;
		bool m_showLevels = true;
		bool m_showPrivateMessages = true;

		bool m_showLevelBar = true;
		bool m_showStaminaBar = true;
		bool m_showMagLevelBar = true;
		bool m_showTrainingBar = true;
		bool m_showSkillsBar[Skills_LastSkill];

		Uint8 m_buySortMethod = Shop_Sort_Name;
		Uint8 m_sellSortMethod = Shop_Sort_Name;
		bool m_buyWithBackpacks = false;
		bool m_ignoreCapacity = false;
		bool m_ignoreEquiped = true;

		bool m_activatedBlackList = true;
		bool m_activatedWhiteList = true;
		bool m_ignoreYellingMessages = false;
		bool m_ignorePrivateMessages = false;
		bool m_allowVipMessages = false;

		bool m_showActionBar1 = true;
		bool m_showActionBar2 = false;
		bool m_askBeforeBuying = true;

		bool m_newCharacterList = false;
		bool m_ingame = false;
		bool m_showPerformance = false;
		bool m_showLogger = false;
		bool m_devMode = false;
		bool m_showDebugCoordinates = false;
		bool m_uiInspector = false;
		bool m_devRecorder = false;
		bool m_manaTrainer = false;
		Uint8 m_manaTrainerManaPercent = 95;
		bool m_autoFisher = false;
		bool m_autoLogout = false;
		bool m_autoLogoutPK = false;
		bool m_autoLogoutPlayer = false;
		bool m_autoLogoutPending = false;
		bool m_autoLogoutCaptureTaken = false;
		std::string m_autoLogoutPendingReason;
		std::vector<std::string> m_autoLogoutGMs;
		std::vector<std::string> m_autoLogoutPlayers;
		std::vector<std::string> m_gmBaselineLogin;
		std::vector<bool> m_gmBaselineSet;
		std::vector<std::string> m_playerWebStatus;
		std::vector<bool> m_playerWebStatusSet;
		bool m_alertCaptureEnabled = true;
		std::string m_discordWebhook;
		bool m_autoRuner = false;
		Uint8 m_autoRunerManaPercent = 50;
		Uint32 m_autoRunerLastCast = 0;
		Uint32 m_autoRunerDelay = 3000;
		std::string m_autoRunerSpell = "adori vis";
		bool m_autoRunerPendingMove = false;
		Uint8 m_autoRunerPendingSlot = SLOT_RIGHT;
		Uint32 m_autoRunerPendingMoveTime = 0;
		bool m_autoRunerPendingBlankMove = false;
		Uint8 m_autoRunerPendingBlankSlot = SLOT_RIGHT;
		Uint32 m_autoRunerPendingBlankMoveTime = 0;
		bool m_autoRunerHasReturnContainer = false;
		Uint8 m_autoRunerReturnContainer = 0;
		Uint8 m_autoRunerReturnSlot = 0;
		bool m_autoEater = false;
		Uint16 m_autoEaterFoodId = 3577;
		Uint32 m_autoEaterDelay = 60000;
		Uint32 m_autoEaterLastUse = 0;
		bool m_autoHealer = false;
		std::string m_autoHealerSpell = "exura";
		Uint8 m_autoHealerHPPercent = 70;
		bool m_autoHealerSelfUH = false;
		Uint16 m_autoHealerUHItemId = 3160;
		Uint8 m_autoHealerSelfUHPercent = 45;
		bool m_autoHealerFriendUH = false;
		std::string m_autoHealerFriendNames;
		Uint8 m_autoHealerFriendUHPercent = 60;
		Uint32 m_autoHealerDelay = 1000;
		Uint32 m_autoHealerLastCast = 0;
		bool m_comboLeader = false;
		std::string m_comboLeaderName;
		std::string m_comboLeaderMappingNames[COMBO_LEADER_MAPPING_COUNT] = {"SD", "HMM", "GFB", "Explosion", "Custom"};
		Uint16 m_comboLeaderMappingEffects[COMBO_LEADER_MAPPING_COUNT] = {10, 4, 3, 0, 0};
		Uint16 m_comboLeaderMappingRunes[COMBO_LEADER_MAPPING_COUNT] = {3155, 3198, 3191, 3200, 0};
		Uint32 m_comboLeaderDelay = 1000;
		Uint32 m_comboLeaderLastCast = 0;
		bool m_magicWall = false;
		std::string m_magicWallSpell = "exevo gran mas vis";
		Uint8 m_magicWallDist = 2;
		Uint32 m_magicWallDelay = 2000;
		Uint32 m_magicWallLastCast = 0;
		std::unordered_map<Uint32, Uint32> m_exhaustTimers;
		Uint32 m_botLastAction = 0;
		Uint32 m_botActionCooldown = 0;
		Uint32 m_manualItemBotBlockUntil = 0;
		Uint32 m_manualLastAction = 0;
		Uint32 m_lastPlayerMovementAction = 0;
		Uint32 m_playerMovementBotBlock = 0;
		bool m_numpadDashActive = false;
		SDL_Keycode m_numpadDashKey = SDLK_UNKNOWN;
		Uint8 m_numpadDashDirection = DIRECTION_INVALID;
		Uint32 m_numpadDashLastPulse = 0;
		Uint32 m_numpadDashLastTileChange = 0;
		Uint16 m_numpadDashLastX = 0;
		Uint16 m_numpadDashLastY = 0;
		Uint8 m_numpadDashLastZ = 0;
		bool m_wasdMode = false;
		bool m_autoLogoutOnPM = false;
		bool m_autoLogoutOnBroadcast = false;
		bool m_playerAlarmEnabled = false;
		bool m_fullBrightness = false;
		bool m_hideWASD = false;
		bool m_hideHPManaPercent = false;
		bool m_lowProfileMode = false;
		bool m_lpSkillsWasOpen = false;
		bool m_lpBattleWasOpen = false;
		bool m_lpVipWasOpen = false;
		Sint8 m_exivaDir = -1;
		Uint8 m_exivaEscCount = 0;
		Uint32 m_exivaTick = 0;
		std::string m_exivaName;
};

#endif /* __FILE_ENGINE_h_ */
