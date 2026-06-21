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

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../GUI_Elements/GUI_Panel.h"
#include "../GUI_Elements/GUI_PanelWindow.h"
#include "../GUI_Elements/GUI_StaticImage.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_Icon.h"
#include "../GUI_Elements/GUI_Container.h"
#include "../game.h"

#define SPELLS_TITLE "Spell List"
#define SPELLS_MAXIMINI_EVENTID 1000
#define SPELLS_CLOSE_EVENTID 1001
#define SPELLS_CONFIGURE_EVENTID 1002
#define SPELLS_RESIZE_WIDTH_EVENTID 1003
#define SPELLS_RESIZE_HEIGHT_EVENTID 1004
#define SPELLS_EXIT_WINDOW_EVENTID 1005
#define SPELLS_CONTAINER_EVENTID 1006

extern Engine g_engine;
extern Game g_game;

SpellData g_spells[] =
{
	//{incantation, name, group[, secondGroup], id, price, type, reqLvl, reqPrem, vocations, reqMana, reqSoul[, runeId], cooldown, groupCooldown[, secondGroupCooldown]}
	{"exura", "Light Healing", SPELL_GROUP_HEALING, 0x05, 0, SPELL_TYPE_INSTANT, 8, false, SPELL_VOCATION_PALADIN_SORCERER_DRUID, 20, 0, 1, 1},
	{"exura gran", "Intense Healing", SPELL_GROUP_HEALING, 0x06, 350, SPELL_TYPE_INSTANT, 20, false, SPELL_VOCATION_PALADIN_SORCERER_DRUID, 64, 0, 1, 1},
	{"exura vita", "Ultimate Healing", SPELL_GROUP_HEALING, 0x00, 1000, SPELL_TYPE_INSTANT, 30, false, SPELL_VOCATION_SORCERER_DRUID, 160, 0, 1, 1},
	{"adura gran", "Intense Healing Rune", SPELL_GROUP_SUPPORT, 0x49, 600, SPELL_TYPE_RUNE, 15, false, SPELL_VOCATION_DRUID, 120, 2, 0, 2, 2},
	{"adura vita", "Ultimate Healing Rune", SPELL_GROUP_SUPPORT, 0x3D, 1500, SPELL_TYPE_RUNE, 24, false, SPELL_VOCATION_DRUID, 400, 3, 0, 2, 2},
	{"utani hur", "Haste", SPELL_GROUP_SUPPORT, 0x64, 600, SPELL_TYPE_INSTANT, 14, true, SPELL_VOCATION_ALL, 60, 0, 2, 2},
	{"adori min vis", "Light Magic Missile Rune", SPELL_GROUP_SUPPORT, 0x48, 500, SPELL_TYPE_RUNE, 15, false, SPELL_VOCATION_SORCERER_DRUID, 120, 1, 0, 2, 2},
	{"adori vis", "Heavy Magic Missile Rune", SPELL_GROUP_SUPPORT, 0x4C, 1500, SPELL_TYPE_RUNE, 25, false, SPELL_VOCATION_SORCERER_DRUID, 350, 2, 0, 2, 2},
	{"utevo res", "Summon Creature", SPELL_GROUP_SUPPORT, 0x75, 2000, SPELL_TYPE_INSTANT, 25, false, SPELL_VOCATION_SORCERER_DRUID, 0, 0, 2, 2},
	{"utevo lux", "Light", SPELL_GROUP_SUPPORT, 0x74, 0, SPELL_TYPE_INSTANT, 8, false, SPELL_VOCATION_ALL, 20, 0, 2, 2},
	{"utevo gran lux", "Great Light", SPELL_GROUP_SUPPORT, 0x73, 500, SPELL_TYPE_INSTANT, 13, false, SPELL_VOCATION_ALL, 60, 0, 2, 2},
	{"adeta sio", "Convince Creature Rune", SPELL_GROUP_SUPPORT, 0x59, 800, SPELL_TYPE_RUNE, 16, false, SPELL_VOCATION_DRUID, 200, 3, 0, 2, 2},
	{"exevo vis hur", "Energy Wave", SPELL_GROUP_ATTACK, 0x2A, 2500, SPELL_TYPE_INSTANT, 38, false, SPELL_VOCATION_SORCERER, 170, 0, 8, 2},
	{"adevo ina", "Chameleon Rune", SPELL_GROUP_SUPPORT, 0x5A, 1300, SPELL_TYPE_RUNE, 27, false, SPELL_VOCATION_DRUID, 600, 2, 0, 2, 2},
	{"adori flam", "Fireball Rune", SPELL_GROUP_SUPPORT, 0x4E, 1600, SPELL_TYPE_RUNE, 27, true, SPELL_VOCATION_SORCERER, 460, 3, 0, 2, 2},
	{"adori mas flam", "Great Fireball Rune", SPELL_GROUP_SUPPORT, 0x4D, 1200, SPELL_TYPE_RUNE, 30, false, SPELL_VOCATION_SORCERER, 530, 3, 0, 2, 2},
	{"adevo mas flam", "Fire Bomb Rune", SPELL_GROUP_SUPPORT, 0x51, 1500, SPELL_TYPE_RUNE, 27, false, SPELL_VOCATION_SORCERER_DRUID, 600, 4, 0, 2, 2},
	{"adevo mas hur", "Explosion Rune", SPELL_GROUP_SUPPORT, 0x52, 1800, SPELL_TYPE_RUNE, 31, false, SPELL_VOCATION_SORCERER_DRUID, 570, 4, 0, 2, 2},
	{"exevo flam hur", "Fire Wave", SPELL_GROUP_ATTACK, 0x2B, 850, SPELL_TYPE_INSTANT, 18, false, SPELL_VOCATION_SORCERER, 25, 0, 4, 2},
	{"exiva", "Find Person", SPELL_GROUP_SUPPORT, 0x71, 80, SPELL_TYPE_INSTANT, 8, false, SPELL_VOCATION_ALL, 20, 0, 2, 2},
	{"adori gran mort", "Sudden Death Rune", SPELL_GROUP_SUPPORT, 0x3F, 3000, SPELL_TYPE_RUNE, 45, false, SPELL_VOCATION_SORCERER, 985, 5, 0, 2, 2},
	{"exevo vis lux", "Energy Beam", SPELL_GROUP_ATTACK, 0x28, 1000, SPELL_TYPE_INSTANT, 23, false, SPELL_VOCATION_SORCERER, 40, 0, 4, 2},
	{"exevo gran vis lux", "Great Energy Beam", SPELL_GROUP_ATTACK, 0x29, 1800, SPELL_TYPE_INSTANT, 29, false, SPELL_VOCATION_SORCERER, 110, 0, 6, 2},
	{"exevo gran mas flam", "Hell's Core", SPELL_GROUP_ATTACK, 0x30, 8000, SPELL_TYPE_INSTANT, 60, true, SPELL_VOCATION_SORCERER, 1100, 0, 40, 4},
	{"adevo grav flam", "Fire Field Rune", SPELL_GROUP_SUPPORT, 0x50, 500, SPELL_TYPE_RUNE, 15, false, SPELL_VOCATION_SORCERER_DRUID, 240, 1, 0, 2, 2},
	{"adevo grav pox", "Poison Field Rune", SPELL_GROUP_SUPPORT, 0x44, 300, SPELL_TYPE_RUNE, 14, false, SPELL_VOCATION_SORCERER_DRUID, 200, 1, 0, 2, 2},
	{"adevo grav vis", "Energy Field Rune", SPELL_GROUP_SUPPORT, 0x54, 700, SPELL_TYPE_RUNE, 18, false, SPELL_VOCATION_SORCERER_DRUID, 320, 2, 0, 2, 2},
	{"adevo mas grav flam", "Fire Wall Rune", SPELL_GROUP_SUPPORT, 0x4F, 2000, SPELL_TYPE_RUNE, 33, false, SPELL_VOCATION_SORCERER_DRUID, 780, 4, 0, 2, 2},
	{"exana pox", "Cure Poison", SPELL_GROUP_HEALING, 0x09, 150, SPELL_TYPE_INSTANT, 10, false, SPELL_VOCATION_ALL, 30, 0, 6, 1},
	{"adito grav", "Destroy Field Rune", SPELL_GROUP_SUPPORT, 0x56, 700, SPELL_TYPE_RUNE, 17, false, SPELL_VOCATION_PALADIN_SORCERER_DRUID, 120, 2, 0, 2, 2},
	{"adana pox", "Cure Poison Rune", SPELL_GROUP_SUPPORT, 0x58, 600, SPELL_TYPE_RUNE, 15, false, SPELL_VOCATION_SORCERER_DRUID, 200, 1, 0, 2, 2},
	{"adevo mas grav pox", "Poison Wall Rune", SPELL_GROUP_SUPPORT, 0x43, 1600, SPELL_TYPE_RUNE, 29, false, SPELL_VOCATION_SORCERER_DRUID, 640, 3, 0, 2, 2},
	{"adevo mas grav vis", "Energy Wall Rune", SPELL_GROUP_SUPPORT, 0x53, 2500, SPELL_TYPE_RUNE, 41, false, SPELL_VOCATION_SORCERER_DRUID, 1000, 5, 0, 2, 2},
	{"exura gran san", "Salvation", SPELL_GROUP_HEALING, 0x3B, 8000, SPELL_TYPE_INSTANT, 60, true, SPELL_VOCATION_PALADIN, 210, 0, 1, 1},
	{"utevo res ina", "Creature Illusion", SPELL_GROUP_SUPPORT, 0x63, 1000, SPELL_TYPE_INSTANT, 23, false, SPELL_VOCATION_SORCERER_DRUID, 100, 0, 2, 2},
	{"utani gran hur", "Strong Haste", SPELL_GROUP_SUPPORT, 0x65, 1300, SPELL_TYPE_INSTANT, 20, true, SPELL_VOCATION_SORCERER_DRUID, 100, 0, 2, 2},
	{"exevo pan", "Food", SPELL_GROUP_SUPPORT, 0x62, 300, SPELL_TYPE_INSTANT, 14, false, SPELL_VOCATION_DRUID, 120, 1, 2, 2},
	{"exevo gran frigo hur", "Strong Ice Wave", SPELL_GROUP_ATTACK, 0x2D, 7500, SPELL_TYPE_INSTANT, 40, true, SPELL_VOCATION_DRUID, 170, 0, 8, 2},
	{"utamo vita", "Magic Shield", SPELL_GROUP_SUPPORT, 0x7B, 450, SPELL_TYPE_INSTANT, 14, false, SPELL_VOCATION_SORCERER_DRUID, 50, 0, 2, 2},
	{"utana vid", "Invisible", SPELL_GROUP_SUPPORT, 0x5D, 2000, SPELL_TYPE_INSTANT, 35, false, SPELL_VOCATION_SORCERER_DRUID, 440, 0, 2, 2},
	{"exevo con flam", "Conjure Explosive Arrow", SPELL_GROUP_SUPPORT, 0x6C, 1000, SPELL_TYPE_INSTANT, 25, false, SPELL_VOCATION_PALADIN, 290, 3, 2, 2},
	{"adevo res flam", "Soulfire Rune", SPELL_GROUP_SUPPORT, 0x42, 1800, SPELL_TYPE_RUNE, 27, true, SPELL_VOCATION_SORCERER_DRUID, 420, 3, 0, 2, 2},
	{"exevo con", "Conjure Arrow", SPELL_GROUP_SUPPORT, 0x69, 450, SPELL_TYPE_INSTANT, 13, false, SPELL_VOCATION_PALADIN, 100, 1, 2, 2},
	{"adana ani", "Paralyse Rune", SPELL_GROUP_SUPPORT, 0x46, 1900, SPELL_TYPE_RUNE, 54, true, SPELL_VOCATION_DRUID, 1400, 3, 0, 2, 2},
	{"adevo mas vis", "Energy Bomb Rune", SPELL_GROUP_SUPPORT, 0x55, 2300, SPELL_TYPE_RUNE, 37, true, SPELL_VOCATION_SORCERER, 880, 5, 0, 2, 2},
	{"exevo gran mas tera", "Wrath of Nature", SPELL_GROUP_ATTACK, 0x2F, 6000, SPELL_TYPE_INSTANT, 55, true, SPELL_VOCATION_DRUID, 700, 0, 40, 4},
	{"exori gran con", "Strong Ethereal Spear", SPELL_GROUP_ATTACK, 0x3A, 10000, SPELL_TYPE_INSTANT, 90, true, SPELL_VOCATION_PALADIN, 55, 0, 8, 2},
	{"exori min", "Front Sweep", SPELL_GROUP_ATTACK, 0x13, 4000, SPELL_TYPE_INSTANT, 70, true, SPELL_VOCATION_KNIGHT, 200, 0, 6, 2},
	{"exori ico", "Brutal Strike", SPELL_GROUP_ATTACK, 0x16, 1000, SPELL_TYPE_INSTANT, 16, true, SPELL_VOCATION_KNIGHT, 30, 0, 6, 2},
	{"exori gran ico", "Annihilation", SPELL_GROUP_ATTACK, 0x17, 20000, SPELL_TYPE_INSTANT, 110, true, SPELL_VOCATION_KNIGHT, 300, 0, 30, 4},
	{"utevo vis lux", "Ultimate Light", SPELL_GROUP_SUPPORT, 0x72, 1600, SPELL_TYPE_INSTANT, 26, true, SPELL_VOCATION_SORCERER_DRUID, 140, 0, 2, 2},
	{"exani tera", "Magic Rope", SPELL_GROUP_SUPPORT, 0x68, 200, SPELL_TYPE_INSTANT, 9, true, SPELL_VOCATION_ALL, 20, 0, 2, 2},
	{"adori tera", "Stalagmite Rune", SPELL_GROUP_SUPPORT, 0x41, 1400, SPELL_TYPE_RUNE, 24, false, SPELL_VOCATION_SORCERER_DRUID, 350, 2, 0, 2, 2},
	{"adito tera", "Disintegrate Rune", SPELL_GROUP_SUPPORT, 0x57, 900, SPELL_TYPE_RUNE, 21, true, SPELL_VOCATION_PALADIN_SORCERER_DRUID, 200, 3, 0, 2, 2},
	{"exori", "Berserk", SPELL_GROUP_ATTACK, 0x14, 2500, SPELL_TYPE_INSTANT, 35, true, SPELL_VOCATION_KNIGHT, 115, 0, 4, 2},
	{"exani hur", "Levitate", SPELL_GROUP_SUPPORT, 0x7C, 500, SPELL_TYPE_INSTANT, 12, true, SPELL_VOCATION_ALL, 50, 0, 2, 2},
	{"exura gran mas res", "Mass Healing", SPELL_GROUP_HEALING, 0x08, 2200, SPELL_TYPE_INSTANT, 36, true, SPELL_VOCATION_DRUID, 150, 0, 2, 1},
	{"adana mort", "Animate Dead Rune", SPELL_GROUP_SUPPORT, 0x5C, 1200, SPELL_TYPE_RUNE, 27, true, SPELL_VOCATION_SORCERER_DRUID, 600, 5, 0, 2, 2},
	{"exura sio", "Heal Friend", SPELL_GROUP_HEALING, 0x07, 800, SPELL_TYPE_INSTANT, 18, true, SPELL_VOCATION_DRUID, 140, 0, 1, 1},
	{"adevo grav tera", "Magic Wall Rune", SPELL_GROUP_SUPPORT, 0x47, 2100, SPELL_TYPE_RUNE, 32, true, SPELL_VOCATION_SORCERER, 750, 5, 0, 2, 2},
	{"exori mort", "Death Strike", SPELL_GROUP_ATTACK, 0x25, 800, SPELL_TYPE_INSTANT, 16, true, SPELL_VOCATION_SORCERER, 20, 0, 2, 2},
	{"exori vis", "Energy Strike", SPELL_GROUP_ATTACK, 0x1C, 800, SPELL_TYPE_INSTANT, 12, true, SPELL_VOCATION_SORCERER_DRUID, 20, 0, 2, 2},
	{"exori flam", "Flame Strike", SPELL_GROUP_ATTACK, 0x19, 800, SPELL_TYPE_INSTANT, 14, true, SPELL_VOCATION_SORCERER_DRUID, 20, 0, 2, 2},
	{"exana ina", "Cancel Invisibility", SPELL_GROUP_SUPPORT, 0x5E, 1600, SPELL_TYPE_INSTANT, 26, true, SPELL_VOCATION_PALADIN, 200, 0, 2, 2},
	{"adevo mas pox", "Poison Bomb Rune", SPELL_GROUP_SUPPORT, 0x45, 1000, SPELL_TYPE_RUNE, 25, true, SPELL_VOCATION_DRUID, 520, 2, 0, 2, 2},
	{"exevo gran mort", "Conjure Wand of Darkness", SPELL_GROUP_SUPPORT, 0x8D, 5000, SPELL_TYPE_INSTANT, 41, true, SPELL_VOCATION_SORCERER, 250, 0, 1800, 2},
	{"exeta res", "Challenge", SPELL_GROUP_SUPPORT, 0x60, 2000, SPELL_TYPE_INSTANT, 20, true, SPELL_VOCATION_KNIGHT, 40, 0, 2, 2},
	{"adevo grav vita", "Wild Growth Rune", SPELL_GROUP_SUPPORT, 0x3C, 2000, SPELL_TYPE_RUNE, 27, true, SPELL_VOCATION_DRUID, 600, 5, 0, 2, 2},
	{"exori gran", "Fierce Berserk", SPELL_GROUP_ATTACK, 0x15, 7500, SPELL_TYPE_INSTANT, 90, true, SPELL_VOCATION_KNIGHT, 340, 0, 6, 2},
	{"exori mas", "Groundshaker", SPELL_GROUP_ATTACK, 0x18, 1500, SPELL_TYPE_INSTANT, 33, true, SPELL_VOCATION_KNIGHT, 160, 0, 8, 2},
	{"exori hur", "Whirlwind Throw", SPELL_GROUP_ATTACK, 0x12, 1500, SPELL_TYPE_INSTANT, 28, true, SPELL_VOCATION_KNIGHT, 40, 0, 6, 2},
	{"exeta con", "Enchant Spear", SPELL_GROUP_SUPPORT, 0x67, 2000, SPELL_TYPE_INSTANT, 45, true, SPELL_VOCATION_PALADIN, 350, 3, 2, 2},
	{"exori con", "Ethereal Spear", SPELL_GROUP_ATTACK, 0x11, 1100, SPELL_TYPE_INSTANT, 23, true, SPELL_VOCATION_PALADIN, 25, 0, 2, 2},
	{"exori frigo", "Ice Strike", SPELL_GROUP_ATTACK, 0x1F, 800, SPELL_TYPE_INSTANT, 15, true, SPELL_VOCATION_SORCERER_DRUID, 20, 0, 2, 2},
	{"exori tera", "Terra Strike", SPELL_GROUP_ATTACK, 0x22, 800, SPELL_TYPE_INSTANT, 13, true, SPELL_VOCATION_SORCERER_DRUID, 20, 0, 2, 2},
	{"adori frigo", "Icicle Rune", SPELL_GROUP_SUPPORT, 0x4A, 1700, SPELL_TYPE_RUNE, 28, true, SPELL_VOCATION_DRUID, 460, 3, 0, 2, 2},
	{"adori mas frigo", "Avalanche Rune", SPELL_GROUP_SUPPORT, 0x5B, 1200, SPELL_TYPE_RUNE, 30, false, SPELL_VOCATION_DRUID, 530, 3, 0, 2, 2},
	{"adori mas tera", "Stone Shower Rune", SPELL_GROUP_SUPPORT, 0x40, 1100, SPELL_TYPE_RUNE, 28, true, SPELL_VOCATION_DRUID, 430, 3, 0, 2, 2},
	{"adori mas vis", "Thunderstorm Rune", SPELL_GROUP_SUPPORT, 0x3E, 1100, SPELL_TYPE_RUNE, 28, true, SPELL_VOCATION_SORCERER, 430, 3, 0, 2, 2},
	{"exevo gran mas frigo", "Ethernal Winter", SPELL_GROUP_ATTACK, 0x31, 8000, SPELL_TYPE_INSTANT, 60, true, SPELL_VOCATION_DRUID, 1050, 0, 40, 4},
	{"exevo gran mas vis", "Rage of the Skies", SPELL_GROUP_ATTACK, 0x33, 6000, SPELL_TYPE_INSTANT, 55, true, SPELL_VOCATION_SORCERER, 600, 0, 40, 4},
	{"exevo tera hur", "Terra Wave", SPELL_GROUP_ATTACK, 0x2E, 2500, SPELL_TYPE_INSTANT, 38, false, SPELL_VOCATION_DRUID, 210, 0, 4, 2},
	{"exevo frigo hur", "Ice Wave", SPELL_GROUP_ATTACK, 0x2C, 850, SPELL_TYPE_INSTANT, 18, false, SPELL_VOCATION_DRUID, 25, 0, 4, 2},
	{"exori san", "Divine Missile", SPELL_GROUP_ATTACK, 0x26, 1800, SPELL_TYPE_INSTANT, 40, true, SPELL_VOCATION_PALADIN, 20, 0, 2, 2},
	{"exura ico", "Wound Cleansing", SPELL_GROUP_HEALING, 0x02, 0, SPELL_TYPE_INSTANT, 8, false, SPELL_VOCATION_KNIGHT, 40, 0, 1, 1},
	{"exevo mas san", "Divine Caldera", SPELL_GROUP_ATTACK, 0x27, 3000, SPELL_TYPE_INSTANT, 50, true, SPELL_VOCATION_PALADIN, 160, 0, 4, 2},
	{"exura san", "Divine Healing", SPELL_GROUP_HEALING, 0x01, 3000, SPELL_TYPE_INSTANT, 35, false, SPELL_VOCATION_PALADIN, 160, 0, 1, 1},
	{"utito mas sio", "Train Party", SPELL_GROUP_SUPPORT, 0x77, 4000, SPELL_TYPE_INSTANT, 32, true, SPELL_VOCATION_KNIGHT, 0, 0, 2, 2},
	{"utamo mas sio", "Protect Party", SPELL_GROUP_SUPPORT, 0x7A, 4000, SPELL_TYPE_INSTANT, 32, true, SPELL_VOCATION_PALADIN, 0, 0, 2, 2},
	{"utura mas sio", "Heal Party", SPELL_GROUP_SUPPORT, 0x7D, 4000, SPELL_TYPE_INSTANT, 32, true, SPELL_VOCATION_DRUID, 0, 0, 2, 2},
	{"utori mas sio", "Enchant Party", SPELL_GROUP_SUPPORT, 0x70, 4000, SPELL_TYPE_INSTANT, 32, true, SPELL_VOCATION_SORCERER, 0, 0, 2, 2},
	{"adori san", "Holy Missile Rune", SPELL_GROUP_SUPPORT, 0x4B, 1600, SPELL_TYPE_RUNE, 27, true, SPELL_VOCATION_PALADIN, 300, 3, 0, 2, 2},
	{"utani tempo hur", "Charge", SPELL_GROUP_SUPPORT, 0x61, 1300, SPELL_TYPE_INSTANT, 25, true, SPELL_VOCATION_KNIGHT, 100, 0, 2, 2},
	{"utamo tempo", "Protector", SPELL_GROUP_SUPPORT, 0x79, 6000, SPELL_TYPE_INSTANT, 55, true, SPELL_VOCATION_KNIGHT, 200, 0, 2, 2},
	{"utito tempo", "Blood Rage", SPELL_GROUP_SUPPORT, 0x5F, 8000, SPELL_TYPE_INSTANT, 60, true, SPELL_VOCATION_KNIGHT, 290, 0, 2, 2},
	{"utamo tempo san", "Swift Foot", SPELL_GROUP_SUPPORT, 0x76, 6000, SPELL_TYPE_INSTANT, 55, true, SPELL_VOCATION_PALADIN, 400, 0, 2, 2},
	{"utito tempo san", "Sharpshooter", SPELL_GROUP_SUPPORT, 0x78, 8000, SPELL_TYPE_INSTANT, 60, true, SPELL_VOCATION_PALADIN, 450, 0, 2, 2},
	{"utori flam", "Ignite", SPELL_GROUP_ATTACK, 0x36, 1500, SPELL_TYPE_INSTANT, 26, true, SPELL_VOCATION_SORCERER, 30, 0, 30, 2},
	{"utori mort", "Curse", SPELL_GROUP_ATTACK, 0x35, 6000, SPELL_TYPE_INSTANT, 75, true, SPELL_VOCATION_SORCERER, 30, 0, 40, 2},
	{"utori vis", "Electrify", SPELL_GROUP_ATTACK, 0x37, 2500, SPELL_TYPE_INSTANT, 34, true, SPELL_VOCATION_SORCERER, 30, 0, 30, 2},
	{"utori kor", "Inflict Wound", SPELL_GROUP_ATTACK, 0x38, 2500, SPELL_TYPE_INSTANT, 40, true, SPELL_VOCATION_KNIGHT, 30, 0, 30, 2},
	{"utori pox", "Envenom", SPELL_GROUP_ATTACK, 0x39, 6000, SPELL_TYPE_INSTANT, 50, true, SPELL_VOCATION_DRUID, 30, 0, 40, 2},
	{"utori san", "Holy Flash", SPELL_GROUP_ATTACK, 0x34, 7500, SPELL_TYPE_INSTANT, 70, true, SPELL_VOCATION_PALADIN, 30, 0, 40, 2},
	{"exana kor", "Cure Bleeding", SPELL_GROUP_HEALING, 0x0B, 2500, SPELL_TYPE_INSTANT, 45, true, SPELL_VOCATION_KNIGHT_DRUID, 30, 0, 6, 1},
	{"exana flam", "Cure Burning", SPELL_GROUP_HEALING, 0x0C, 2000, SPELL_TYPE_INSTANT, 30, true, SPELL_VOCATION_DRUID, 30, 0, 6, 1},
	{"exana vis", "Cure Electrification", SPELL_GROUP_HEALING, 0x0D, 1000, SPELL_TYPE_INSTANT, 22, true, SPELL_VOCATION_DRUID, 30, 0, 6, 1},
	{"exana mort", "Cure Curse", SPELL_GROUP_HEALING, 0x0A, 6000, SPELL_TYPE_INSTANT, 80, true, SPELL_VOCATION_PALADIN, 40, 0, 6, 1},
	{"exori moe ico", "Physical Strike", SPELL_GROUP_ATTACK, 0x10, 800, SPELL_TYPE_INSTANT, 16, true, SPELL_VOCATION_DRUID, 20, 0, 2, 2},
	{"exori amp vis", "Lightning", SPELL_GROUP_ATTACK, SPELL_GROUP_SPECIAL, 0x32, 5000, SPELL_TYPE_INSTANT, 55, true, SPELL_VOCATION_SORCERER, 60, 0, 8, 2, 8},
	{"exori gran flam", "Strong Flame Strike", SPELL_GROUP_ATTACK, SPELL_GROUP_SPECIAL, 0x1A, 6000, SPELL_TYPE_INSTANT, 70, true, SPELL_VOCATION_SORCERER, 60, 0, 8, 2, 8},
	{"exori gran vis", "Strong Energy Strike", SPELL_GROUP_ATTACK, SPELL_GROUP_SPECIAL, 0x1D, 7500, SPELL_TYPE_INSTANT, 80, true, SPELL_VOCATION_SORCERER, 60, 0, 8, 2, 8},
	{"exori gran frigo", "Strong Ice Strike", SPELL_GROUP_ATTACK, SPELL_GROUP_SPECIAL, 0x20, 6000, SPELL_TYPE_INSTANT, 80, true, SPELL_VOCATION_DRUID, 60, 0, 8, 2, 8},
	{"exori gran tera", "Strong Terra Strike", SPELL_GROUP_ATTACK, SPELL_GROUP_SPECIAL, 0x23, 6000, SPELL_TYPE_INSTANT, 70, true, SPELL_VOCATION_DRUID, 60, 0, 8, 2, 8},
	{"exori max flam", "Ultimate Flame Strike", SPELL_GROUP_ATTACK, 0x1B, 15000, SPELL_TYPE_INSTANT, 90, true, SPELL_VOCATION_SORCERER, 100, 0, 30, 4},
	{"exori max vis", "Ultimate Energy Strike", SPELL_GROUP_ATTACK, 0x1E, 15000, SPELL_TYPE_INSTANT, 100, true, SPELL_VOCATION_SORCERER, 100, 0, 30, 4},
	{"exori max frigo", "Ultimate Ice Strike", SPELL_GROUP_ATTACK, 0x21, 15000, SPELL_TYPE_INSTANT, 100, true, SPELL_VOCATION_DRUID, 100, 0, 30, 4},
	{"exori max tera", "Ultimate Terra Strike", SPELL_GROUP_ATTACK, 0x24, 15000, SPELL_TYPE_INSTANT, 90, true, SPELL_VOCATION_DRUID, 100, 0, 30, 4},
	{"exura gran ico", "Intense Wound Cleansing", SPELL_GROUP_HEALING, 0x03, 6000, SPELL_TYPE_INSTANT, 80, true, SPELL_VOCATION_KNIGHT, 200, 0, 600, 1},
	{"utura", "Recovery", SPELL_GROUP_HEALING, 0x0E, 4000, SPELL_TYPE_INSTANT, 50, true, SPELL_VOCATION_KNIGHT_PALADIN, 75, 0, 60, 1},
	{"utura gran", "Intense Recovery", SPELL_GROUP_HEALING, 0x0F, 10000, SPELL_TYPE_INSTANT, 100, true, SPELL_VOCATION_KNIGHT_PALADIN, 165, 0, 60, 1},
	{"exura dis", "Practise Healing", SPELL_GROUP_HEALING, 0x7F, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_ALL, 5, 0, 1, 1},
	{"exevo dis flam hur", "Practise Fire Wave", SPELL_GROUP_ATTACK, 0x80, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_ALL, 5, 0, 4, 2},
	{"adori dis min vis", "Practise Magic Missile Rune", SPELL_GROUP_SUPPORT, 0x81, 0, SPELL_TYPE_RUNE, 1, false, SPELL_VOCATION_ALL, 5, 0, 0, 2, 2},
	{"exori min flam", "Apprentice's Strike", SPELL_GROUP_ATTACK, 0x7E, 0, SPELL_TYPE_INSTANT, 8, false, SPELL_VOCATION_SORCERER_DRUID, 6, 0, 2, 2},
	{"exori infir tera", "Mud Attack", SPELL_GROUP_ATTACK, 0x88, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_DRUID, 6, 0, 2, 2},
	{"exevo infir frigo hur", "Chill Out", SPELL_GROUP_ATTACK, 0x87, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_DRUID, 8, 0, 4, 2},
	{"exura infir", "Magic Patch", SPELL_GROUP_HEALING, 0x85, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_PALADIN_SORCERER_DRUID, 6, 0, 1, 1},
	{"exura infir ico", "Bruise Bane", SPELL_GROUP_HEALING, 0x86, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_KNIGHT, 10, 0, 1, 1},
	{"exevo infir con", "Arrow Call", SPELL_GROUP_SUPPORT, 0x89, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_PALADIN, 10, 1, 2, 2},
	{"exori infir vis", "Buzz", SPELL_GROUP_ATTACK, 0x84, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_SORCERER, 6, 0, 2, 2},
	{"exevo infir flam hur", "Scorch", SPELL_GROUP_ATTACK, 0x83, 0, SPELL_TYPE_INSTANT, 1, false, SPELL_VOCATION_SORCERER, 8, 0, 4, 2},
	{"exevo gran con hur", "Conjure Diamond Arrow", SPELL_GROUP_SUPPORT, SPELL_GROUP_CONJURE, 0x8B, 15000, SPELL_TYPE_INSTANT, 150, true, SPELL_VOCATION_PALADIN, 1000, 0, 600, 2, 600},
	{"exevo gran con vis", "Conjure Spectral Bolt", SPELL_GROUP_SUPPORT, SPELL_GROUP_CONJURE, 0x8C, 15000, SPELL_TYPE_INSTANT, 150, true, SPELL_VOCATION_PALADIN, 1000, 0, 600, 2, 600},
	{"utevo gran res eq", "Summon Skullfrost", SPELL_GROUP_SUPPORT, 0x8E, 50000, SPELL_TYPE_INSTANT, 200, true, SPELL_VOCATION_KNIGHT, 1000, 0, 1800, 2},
	{"utevo gran res sac", "Summon Emberwing", SPELL_GROUP_SUPPORT, 0x90, 50000, SPELL_TYPE_INSTANT, 200, true, SPELL_VOCATION_PALADIN, 2000, 0, 1800, 2},
	{"utevo gran res ven", "Summon Thundergiant", SPELL_GROUP_SUPPORT, 0x91, 50000, SPELL_TYPE_INSTANT, 200, true, SPELL_VOCATION_SORCERER, 3000, 0, 1800, 2},
	{"utevo gran res dru", "Summon Grovebeast", SPELL_GROUP_SUPPORT, 0x8F, 50000, SPELL_TYPE_INSTANT, 200, true, SPELL_VOCATION_DRUID, 3000, 0, 1800, 2},
};

void spells_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case SPELLS_MAXIMINI_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SPELL_LIST);
			if(pPanel)
			{
				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				iRect& pRect = pPanel->getOriginalRect();
				if(pRect.y2 > 19)
				{
					pPanel->setCachedHeight(pRect.y2);
					pPanel->setSize(pRect.x2, 19);
					parent->checkPanels();

					GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(SPELLS_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeInvisible();
				}
				else
				{
					Sint32 cachedHeight = pPanel->getCachedHeight();
					parent->tryFreeHeight(cachedHeight - pRect.y2);
					pPanel->setSize(pRect.x2, cachedHeight);
					parent->checkPanels();

					GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(SPELLS_CONTAINER_EVENTID));
					if(pContainer)
						pContainer->makeVisible();
				}
			}
		}
		break;
		case SPELLS_CLOSE_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SPELL_LIST);
			if(pPanel)
			{
				GUI_Panel* parent = pPanel->getParent();
				if(!parent)
					break;

				parent->removePanel(pPanel);
			}
		}
		break;
		case SPELLS_CONFIGURE_EVENTID:
		{
			//GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_EXITWARNING);
			//if(pWindow)
				//g_engine.removeWindow(pWindow);
		}
		break;
		case SPELLS_RESIZE_HEIGHT_EVENTID:
		{
			GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SPELL_LIST);
			if(pPanel)
			{
				GUI_Container* pContainer = SDL_static_cast(GUI_Container*, pPanel->getChild(SPELLS_CONTAINER_EVENTID));
				if(pContainer)
				{
					iRect cRect = pContainer->getRect();
					cRect.y2 = status - 19;
					pContainer->setRect(cRect);
				}
			}
		}
		break;
		case SPELLS_EXIT_WINDOW_EVENTID: g_engine.setContentWindowHeight(GUI_PANEL_WINDOW_SPELL_LIST, status); break;
		default: break;
	}
}

void UTIL_toggleSpellsWindow()
{
	GUI_PanelWindow* pPanel = g_engine.getPanel(GUI_PANEL_WINDOW_SPELL_LIST);
	if(pPanel)
		g_engine.removePanelWindow(pPanel);

	GUI_PanelWindow* newWindow = new GUI_PanelWindow(iRect(0, 0, GAME_PANEL_FIXED_WIDTH - 4, 117), true, GUI_PANEL_WINDOW_SPELL_LIST, true);
	newWindow->setEventCallback(&spells_Events, SPELLS_RESIZE_WIDTH_EVENTID, SPELLS_RESIZE_HEIGHT_EVENTID, SPELLS_EXIT_WINDOW_EVENTID);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(2, 0, GUI_UI_ICON_SPELLSLIST_W, GUI_UI_ICON_SPELLSLIST_H), GUI_UI_IMAGE, GUI_UI_ICON_SPELLSLIST_X, GUI_UI_ICON_SPELLSLIST_Y);
	newWindow->addChild(newImage);
	GUI_Icon* newIcon = new GUI_Icon(iRect(147, 0, GUI_UI_ICON_MINIMIZE_WINDOW_UP_W, GUI_UI_ICON_MINIMIZE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_MINIMIZE_WINDOW_UP_X, GUI_UI_ICON_MINIMIZE_WINDOW_UP_Y, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_X, GUI_UI_ICON_MINIMIZE_WINDOW_DOWN_Y, 0, "Maximise or minimise window");
	newIcon->setButtonEventCallback(&spells_Events, SPELLS_MAXIMINI_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(159, 0, GUI_UI_ICON_CLOSE_WINDOW_UP_W, GUI_UI_ICON_CLOSE_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_CLOSE_WINDOW_UP_X, GUI_UI_ICON_CLOSE_WINDOW_UP_Y, GUI_UI_ICON_CLOSE_WINDOW_DOWN_X, GUI_UI_ICON_CLOSE_WINDOW_DOWN_Y, 0, "Close this window");
	newIcon->setButtonEventCallback(&spells_Events, SPELLS_CLOSE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	newIcon = new GUI_Icon(iRect(131, 0, GUI_UI_ICON_SEARCH_WINDOW_UP_W, GUI_UI_ICON_SEARCH_WINDOW_UP_H), GUI_UI_IMAGE, GUI_UI_ICON_SEARCH_WINDOW_UP_X, GUI_UI_ICON_SEARCH_WINDOW_UP_Y, GUI_UI_ICON_SEARCH_WINDOW_DOWN_X, GUI_UI_ICON_SEARCH_WINDOW_DOWN_Y, 0, "Click here to configure the skills window");
	newIcon->setButtonEventCallback(&spells_Events, SPELLS_CONFIGURE_EVENTID);
	newIcon->startEvents();
	newWindow->addChild(newIcon);
	GUI_Label* newLabel = new GUI_Label(iRect(19, 2, 0, 0), SPELLS_TITLE, 0, 144, 144, 144);
	newWindow->addChild(newLabel);
	GUI_Container* newContainer = new GUI_Container(iRect(2, 13, 168, 98), newWindow, SPELLS_CONTAINER_EVENTID);
	newContainer->startEvents();
	newWindow->addChild(newContainer);

	Sint32 preferredPanel = g_engine.getContentWindowParent(GUI_PANEL_WINDOW_SPELL_LIST);
	bool added = g_engine.addToPanel(newWindow, preferredPanel);
	if(!added && preferredPanel != GUI_PANEL_RANDOM)
		added = g_engine.addToPanel(newWindow, GUI_PANEL_RANDOM);

	if(!added)
		delete newWindow;
}
