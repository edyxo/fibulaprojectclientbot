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

#define HOTKEYS_TITLE   "Hotkey Options"
#define HOTKEYS_WIDTH   460
#define HOTKEYS_HEIGHT  320

#define HOTKEYS_OK_EVENTID      1000
#define HOTKEYS_CANCEL_EVENTID  1001
#define HOTKEYS_LIST_EVENTID    1002
#define HOTKEYS_TEXT_EVENTID    1003
#define HOTKEYS_AUTO_EVENTID    1004
#define HOTKEYS_ACTION_EVENTID  1005
#define HOTKEYS_ITEMID_EVENTID  1006

#define HOTKEYS_COUNT 36
#define HOTKEYS_ACTION_COUNT 6

static const SDL_Keycode s_fKeys[HOTKEYS_COUNT] = {
	SDLK_F1,  SDLK_F2,  SDLK_F3,  SDLK_F4,  SDLK_F5,  SDLK_F6,
	SDLK_F7,  SDLK_F8,  SDLK_F9,  SDLK_F10, SDLK_F11, SDLK_F12,
	SDLK_F1,  SDLK_F2,  SDLK_F3,  SDLK_F4,  SDLK_F5,  SDLK_F6,
	SDLK_F7,  SDLK_F8,  SDLK_F9,  SDLK_F10, SDLK_F11, SDLK_F12,
	SDLK_F1,  SDLK_F2,  SDLK_F3,  SDLK_F4,  SDLK_F5,  SDLK_F6,
	SDLK_F7,  SDLK_F8,  SDLK_F9,  SDLK_F10, SDLK_F11, SDLK_F12
};
static const Uint16 s_fMods[HOTKEYS_COUNT] = {
	KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,
	KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,  KMOD_NONE,
	KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT,
	KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT, KMOD_SHIFT,
	KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,
	KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL,  KMOD_CTRL
};
static const char* const s_fKeyNames[HOTKEYS_COUNT] = {
	"F1",  "F2",  "F3",  "F4",  "F5",  "F6",
	"F7",  "F8",  "F9",  "F10", "F11", "F12",
	"Shift+F1",  "Shift+F2",  "Shift+F3",  "Shift+F4",  "Shift+F5",  "Shift+F6",
	"Shift+F7",  "Shift+F8",  "Shift+F9",  "Shift+F10", "Shift+F11", "Shift+F12",
	"Control+F1",  "Control+F2",  "Control+F3",  "Control+F4",  "Control+F5",  "Control+F6",
	"Control+F7",  "Control+F8",  "Control+F9",  "Control+F10", "Control+F11", "Control+F12"
};

static const Uint8 s_actionTypes[HOTKEYS_ACTION_COUNT] = {
	CLIENT_HOTKEY_ACTION_NONE,
	CLIENT_HOTKEY_ACTION_TEXT,
	CLIENT_HOTKEY_ACTION_USEITEM,
	CLIENT_HOTKEY_ACTION_USEONYOURSELF,
	CLIENT_HOTKEY_ACTION_USEONTARGET,
	CLIENT_HOTKEY_ACTION_WITHCROSSHAIRS
};

static const char* const s_actionNames[HOTKEYS_ACTION_COUNT] = {
	"Clear",
	"Text",
	"Use item",
	"Use on self",
	"Use on target",
	"Use with crosshair"
};

static const char* UTIL_getActionShortName(Uint8 type)
{
	switch(type)
	{
		case CLIENT_HOTKEY_ACTION_TEXT: return "Text";
		case CLIENT_HOTKEY_ACTION_USEITEM: return "Use";
		case CLIENT_HOTKEY_ACTION_USEONYOURSELF: return "Self";
		case CLIENT_HOTKEY_ACTION_USEONTARGET: return "Target";
		case CLIENT_HOTKEY_ACTION_WITHCROSSHAIRS: return "Crosshair";
		default: return "";
	}
}

struct HkEntry
{
	Uint8 actionType = CLIENT_HOTKEY_ACTION_NONE;
	std::string text;
	bool sendAuto = false;
	Uint16 itemId = 0;
	Uint8 subtype = 0;
};

static HkEntry s_entries[HOTKEYS_COUNT];
static Sint32 s_curIdx = 0;

static GUI_ListBox*  s_hkList  = NULL;
static GUI_ListBox*  s_hkAction = NULL;
static GUI_TextBox*  s_hkText  = NULL;
static GUI_CheckBox* s_hkAuto  = NULL;
static GUI_TextBox*  s_hkItemId = NULL;
static bool s_loadingEntry = false;

extern Engine g_engine;

static Sint32 actionTypeToIndex(Uint8 type)
{
	for(Sint32 i = 0; i < HOTKEYS_ACTION_COUNT; ++i)
	{
		if(s_actionTypes[i] == type)
			return i;
	}
	return 0;
}

static Uint8 indexToActionType(Sint32 index)
{
	index = UTIL_max<Sint32>(0, UTIL_min<Sint32>(HOTKEYS_ACTION_COUNT - 1, index));
	return s_actionTypes[index];
}

static Uint32 readNumber(GUI_TextBox* textBox)
{
	if(!textBox)
		return 0;

	const std::string& text = textBox->getActualText();
	if(text.empty())
		return 0;

	return SDL_static_cast(Uint32, SDL_strtoul(text.c_str(), NULL, 10));
}

static std::string makeLabel(int i)
{
	std::string label;
	if(s_entries[i].actionType == CLIENT_HOTKEY_ACTION_TEXT && s_entries[i].sendAuto && !s_entries[i].text.empty())
		label = "\x0E\xFF\xCC\x44"; // bright gold = auto-send active
	if(s_entries[i].actionType == CLIENT_HOTKEY_ACTION_TEXT && !s_entries[i].text.empty())
		label += std::string(s_fKeyNames[i]) + ": " + s_entries[i].text;
	else if(s_entries[i].actionType >= CLIENT_HOTKEY_ACTION_USEITEM && s_entries[i].actionType <= CLIENT_HOTKEY_ACTION_WITHCROSSHAIRS && s_entries[i].itemId != 0)
	{
		char buffer[64];
		SDL_snprintf(buffer, sizeof(buffer), "%s: %s %u", s_fKeyNames[i], UTIL_getActionShortName(s_entries[i].actionType), SDL_static_cast(Uint32, s_entries[i].itemId));
		label += buffer;
	}
	else
		label += std::string(s_fKeyNames[i]) + ":";
	return label;
}

static void saveEntry()
{
	if(s_curIdx < 0 || s_curIdx >= HOTKEYS_COUNT) return;
	if(s_hkAction) s_entries[s_curIdx].actionType = indexToActionType(s_hkAction->getSelect());
	if(s_hkText) s_entries[s_curIdx].text    = s_hkText->getActualText();
	if(s_hkAuto) s_entries[s_curIdx].sendAuto = s_hkAuto->isChecked();
	if(s_hkItemId) s_entries[s_curIdx].itemId = SDL_static_cast(Uint16, UTIL_min<Uint32>(65535, readNumber(s_hkItemId)));
	if(s_hkList) s_hkList->set(s_curIdx, makeLabel(s_curIdx));
}

static void loadEntry(Sint32 idx)
{
	if(idx < 0 || idx >= HOTKEYS_COUNT) return;
	s_loadingEntry = true;
	if(s_hkAction) s_hkAction->setSelect(actionTypeToIndex(s_entries[idx].actionType));
	if(s_hkText) s_hkText->setText(s_entries[idx].text);
	if(s_hkAuto) s_hkAuto->setChecked(s_entries[idx].sendAuto);
	if(s_hkItemId)
		s_hkItemId->setText(s_entries[idx].itemId == 0 ? std::string() : std::to_string(s_entries[idx].itemId));
	s_loadingEntry = false;
}

static void applyAll()
{
	for(int i = 0; i < HOTKEYS_COUNT; ++i)
	{
		HotkeyUsage* hk = g_engine.getHotkey(s_fKeys[i], (SDL_Keymod)s_fMods[i]);
		if(!hk) continue;

		if(hk->action.type == CLIENT_HOTKEY_ACTION_TEXT && hk->action.text.text)
		{
			delete hk->action.text.text;
			hk->action.text.text = NULL;
		}

		if(s_entries[i].actionType == CLIENT_HOTKEY_ACTION_TEXT && !s_entries[i].text.empty())
		{
			hk->action.text.type              = CLIENT_HOTKEY_ACTION_TEXT;
			hk->action.text.sendAutomatically = s_entries[i].sendAuto;
			hk->action.text.text              = new std::string(s_entries[i].text);
		}
		else if(s_entries[i].actionType >= CLIENT_HOTKEY_ACTION_USEITEM && s_entries[i].actionType <= CLIENT_HOTKEY_ACTION_WITHCROSSHAIRS && s_entries[i].itemId != 0)
		{
			hk->action.item.type = s_entries[i].actionType;
			hk->action.item.usageType = 0;
			hk->action.item.itemId = s_entries[i].itemId;
			hk->action.item.itemSubtype = s_entries[i].subtype;
		}
		else
			hk->action.type = CLIENT_HOTKEY_ACTION_NONE;
	}
	g_engine.saveCFG();
}

void hotkey_Events(Uint32 event, Sint32)
{
	if(s_loadingEntry)
		return;

	switch(event)
	{
		case HOTKEYS_OK_EVENTID:
		{
			saveEntry();
			applyAll();
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_HOTKEYS)
				g_engine.removeWindow(pWindow);
		}
		break;
		case HOTKEYS_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_HOTKEYS)
				g_engine.removeWindow(pWindow);
		}
		break;
		case HOTKEYS_LIST_EVENTID:
		{
			if(!s_hkList) break;
			Sint32 sel = s_hkList->getSelect();
			if(sel == s_curIdx) break;
			saveEntry();
			s_curIdx = sel;
			loadEntry(s_curIdx);
		}
		break;
		case HOTKEYS_ACTION_EVENTID:
		case HOTKEYS_TEXT_EVENTID:
		case HOTKEYS_AUTO_EVENTID:
		case HOTKEYS_ITEMID_EVENTID:
		{
			saveEntry();
		}
		break;
		default: break;
	}
}

void UTIL_hotkeyOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_HOTKEYS);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	s_curIdx = 0;
	s_hkList = NULL;
	s_hkAction = NULL;
	s_hkText = NULL;
	s_hkAuto = NULL;
	s_hkItemId = NULL;

	for(int i = 0; i < HOTKEYS_COUNT; ++i)
	{
		s_entries[i] = HkEntry();
		HotkeyUsage* hk = g_engine.getHotkey(s_fKeys[i], (SDL_Keymod)s_fMods[i]);
		if(!hk) continue;
		if(hk->action.type == CLIENT_HOTKEY_ACTION_TEXT)
		{
			s_entries[i].actionType = CLIENT_HOTKEY_ACTION_TEXT;
			s_entries[i].sendAuto = hk->action.text.sendAutomatically;
			if(hk->action.text.text)
				s_entries[i].text = *hk->action.text.text;
		}
		else if(hk->action.type >= CLIENT_HOTKEY_ACTION_USEITEM && hk->action.type <= CLIENT_HOTKEY_ACTION_WITHCROSSHAIRS)
		{
			s_entries[i].actionType = hk->action.item.type;
			s_entries[i].itemId = hk->action.item.itemId;
			s_entries[i].subtype = hk->action.item.itemSubtype;
		}
	}

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, HOTKEYS_WIDTH, HOTKEYS_HEIGHT), HOTKEYS_TITLE, GUI_WINDOW_HOTKEYS);

	GUI_Label* lbl = new GUI_Label(iRect(14, 14, 0, 0), "Available hotkeys:");
	newWindow->addChild(lbl);

	s_hkList = new GUI_ListBox(iRect(14, 28, 126, 250), HOTKEYS_LIST_EVENTID);
	for(int i = 0; i < HOTKEYS_COUNT; ++i)
		s_hkList->add(makeLabel(i));
	s_hkList->setEventCallback(&hotkey_Events, HOTKEYS_LIST_EVENTID);
	s_hkList->startEvents();
	s_hkList->setSelect(0);
	newWindow->addChild(s_hkList);

	lbl = new GUI_Label(iRect(154, 14, 0, 0), "Action:");
	newWindow->addChild(lbl);

	s_hkAction = new GUI_ListBox(iRect(154, 28, 180, 78), HOTKEYS_ACTION_EVENTID);
	for(int i = 0; i < HOTKEYS_ACTION_COUNT; ++i)
		s_hkAction->add(s_actionNames[i]);
	s_hkAction->setEventCallback(&hotkey_Events, HOTKEYS_ACTION_EVENTID);
	s_hkAction->startEvents();
	newWindow->addChild(s_hkAction);

	lbl = new GUI_Label(iRect(154, 116, 0, 0), "Text:");
	newWindow->addChild(lbl);

	s_hkText = new GUI_TextBox(iRect(154, 130, 292, 16), "", HOTKEYS_TEXT_EVENTID);
	s_hkText->setTextEventCallback(&hotkey_Events, HOTKEYS_TEXT_EVENTID);
	s_hkText->startEvents();
	newWindow->addChild(s_hkText);

	s_hkAuto = new GUI_CheckBox(iRect(154, 154, 250, 16), "Send automatically", false, HOTKEYS_AUTO_EVENTID);
	s_hkAuto->setBoxEventCallback(&hotkey_Events, HOTKEYS_AUTO_EVENTID);
	s_hkAuto->startEvents();
	newWindow->addChild(s_hkAuto);

	lbl = new GUI_Label(iRect(154, 184, 0, 0), "Rune/Item ID:");
	newWindow->addChild(lbl);

	s_hkItemId = new GUI_TextBox(iRect(154, 198, 112, 16), "", HOTKEYS_ITEMID_EVENTID);
	s_hkItemId->setOnlyNumbers(true);
	s_hkItemId->setMaxLength(5);
	s_hkItemId->setTextEventCallback(&hotkey_Events, HOTKEYS_ITEMID_EVENTID);
	s_hkItemId->startEvents();
	newWindow->addChild(s_hkItemId);

	lbl = new GUI_Label(iRect(154, 226, 0, 0), "Target uses current attack target.");
	newWindow->addChild(lbl);

	GUI_Separator* sep = new GUI_Separator(iRect(14, 288, HOTKEYS_WIDTH - 28, 2));
	newWindow->addChild(sep);

	GUI_Button* btn = new GUI_Button(iRect(HOTKEYS_WIDTH - 58, 296, 43, 20), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	btn->setButtonEventCallback(&hotkey_Events, HOTKEYS_CANCEL_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	btn = new GUI_Button(iRect(HOTKEYS_WIDTH - 105, 296, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	btn->setButtonEventCallback(&hotkey_Events, HOTKEYS_OK_EVENTID);
	btn->startEvents();
	newWindow->addChild(btn);

	g_engine.addWindow(newWindow, true);

	loadEntry(0);
}
