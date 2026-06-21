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

#ifndef __FILE_THINGMANAGER_h_
#define __FILE_THINGMANAGER_h_

#include "defines.h"

class Animator;
enum ThingCategory : Sint32
{
	ThingCategory_First = 0,
	ThingCategory_Item = ThingCategory_First,
	ThingCategory_Creature,
	ThingCategory_Effect,
	ThingCategory_DistanceEffect,
	ThingCategory_Invalid,
	ThingCategory_Last = ThingCategory_Invalid
};

enum ThingAttribute : Uint64
{
	ThingAttribute_Ground = (1 << 0),
	ThingAttribute_GroundBorder = (1 << 1),
	ThingAttribute_OnBottom = (1 << 2),
	ThingAttribute_OnTop = (1 << 3),
	ThingAttribute_Container = (1 << 4),
	ThingAttribute_Stackable = (1 << 5),
	ThingAttribute_ForceUse = (1 << 6),
	ThingAttribute_MultiUse = (1 << 7),
	ThingAttribute_Writable = (1 << 8),
	ThingAttribute_WritableOnce = (1 << 9),
	ThingAttribute_FluidContainer = (1 << 10),
	ThingAttribute_Splash = (1 << 11),
	ThingAttribute_NotWalkable = (1 << 12),
	ThingAttribute_NotMoveable = (1 << 13),
	ThingAttribute_BlockProjectile = (1 << 14),
	ThingAttribute_NotPathable = (1 << 15),
	ThingAttribute_Pickupable = (1 << 16),
	ThingAttribute_Hangable = (1 << 17),
	ThingAttribute_HookSouth = (1 << 18),
	ThingAttribute_HookEast = (1 << 19),
	ThingAttribute_Rotateable = (1 << 20),
	ThingAttribute_Light = (1 << 21),
	ThingAttribute_DontHide = (1 << 22),
	ThingAttribute_Translucent = (1 << 23),
	ThingAttribute_Displacement = (1 << 24),
	ThingAttribute_Elevation = (1 << 25),
	ThingAttribute_LyingCorpse = (1 << 26),
	ThingAttribute_AnimateAlways = (1 << 27),
	ThingAttribute_MinimapColor = (1 << 28),
	ThingAttribute_LensHelp = (1 << 29),
	ThingAttribute_FullGround = (1 << 30),
	ThingAttribute_LookThrough = (SDL_static_cast(Uint64, 1) << 31),
	ThingAttribute_Cloth = (SDL_static_cast(Uint64, 1) << 32),
	ThingAttribute_Market = (SDL_static_cast(Uint64, 1) << 33),
	ThingAttribute_DefaultAction = (SDL_static_cast(Uint64, 1) << 34),
	ThingAttribute_Wrapable = (SDL_static_cast(Uint64, 1) << 35),
	ThingAttribute_Unwrapable = (SDL_static_cast(Uint64, 1) << 36),
	ThingAttribute_TopEffect = (SDL_static_cast(Uint64, 1) << 37),
	ThingAttribute_NoMoveAnimation = (SDL_static_cast(Uint64, 1) << 38),
	ThingAttribute_Chargeable = (SDL_static_cast(Uint64, 1) << 39),
	ThingAttribute_Usable = (SDL_static_cast(Uint64, 1) << 40),
	ThingAttribute_Corpse = (SDL_static_cast(Uint64, 1) << 41),
	ThingAttribute_PlayerCorpse = (SDL_static_cast(Uint64, 1) << 42),
	ThingAttribute_SaleData = (SDL_static_cast(Uint64, 1) << 43),
	ThingAttribute_ChangeToExpire = (SDL_static_cast(Uint64, 1) << 44),
	ThingAttribute_CyclopediaItem = (SDL_static_cast(Uint64, 1) << 45)
};

enum ThingFrameGroup : Uint8
{
	ThingFrameGroup_Default = 0,
	ThingFrameGroup_Idle = ThingFrameGroup_Default,
	ThingFrameGroup_Moving = 1,
	ThingFrameGroup_Last = 2
};

enum MarketVocations : Uint16
{
	Market_Vocation_Any = 0,
	Market_Vocation_None = (1 << 0),
	Market_Vocation_Knight = (1 << 1),
	Market_Vocation_Paladin = (1 << 2),
	Market_Vocation_Sorcerer = (1 << 3),
	Market_Vocation_Druid = (1 << 4),
	Market_Vocation_Promoted = (1 << 5)
};

enum MarketCategories : Uint16
{
	Market_Category_Any = 0,
	Market_Category_Armors = 1,
	Market_Category_Amulets = 2,
	Market_Category_Boots = 3,
	Market_Category_Containers = 4,
	Market_Category_Decoration = 5,
	Market_Category_Food = 6,
	Market_Category_Helmets = 7,
	Market_Category_Legs = 8,
	Market_Category_Others = 9,
	Market_Category_Potions = 10,
	Market_Category_Rings = 11,
	Market_Category_Runes = 12,
	Market_Category_Shields = 13,
	Market_Category_Tools = 14,
	Market_Category_Valuables = 15,
	Market_Category_Ammunition = 16,
	Market_Category_Axes = 17,
	Market_Category_Clubs = 18,
	Market_Category_Distances = 19,
	Market_Category_Swords = 20,
	Market_Category_Wands_Rods = 21,
	Market_Category_Premium_Scrolls = 22,
	Market_Category_Tibia_Coins = 23
};

struct FrameGroup
{
	FrameGroup() = default;

	// non-copyable
	FrameGroup(const FrameGroup&) = delete;
	FrameGroup& operator=(const FrameGroup&) = delete;

	// moveable
	FrameGroup(FrameGroup&& rhs) noexcept : m_sprites(std::move(rhs.m_sprites)), m_animator(rhs.m_animator), m_width(rhs.m_width), m_height(rhs.m_height),
		m_realSize(rhs.m_realSize), m_layers(rhs.m_layers), m_patternX(rhs.m_patternX), m_patternY(rhs.m_patternY), m_patternZ(rhs.m_patternZ), m_animCount(rhs.m_animCount)
	{
		rhs.m_animator = NULL;
	}
	FrameGroup& operator=(FrameGroup&& rhs) noexcept
	{
		if(this != &rhs)
		{
			m_sprites = std::move(rhs.m_sprites);
			m_animator = rhs.m_animator;
			m_width = rhs.m_width;
			m_height = rhs.m_height;
			m_realSize = rhs.m_realSize;
			m_layers = rhs.m_layers;
			m_patternX = rhs.m_patternX;
			m_patternY = rhs.m_patternY;
			m_patternZ = rhs.m_patternZ;
			m_animCount = rhs.m_animCount;
			rhs.m_animator = NULL;
		}
		return (*this);
	}

	std::vector<Uint32> m_sprites;
	Animator* m_animator = NULL;

	Uint8 m_width = 0;
	Uint8 m_height = 0;
	Uint8 m_realSize = 32;
	Uint8 m_layers = 0;
	Uint8 m_patternX = 0;
	Uint8 m_patternY = 0;
	Uint8 m_patternZ = 0;
	Uint8 m_animCount = 0;
};

struct MarketData
{
	MarketData() = default;

	// non-copyable
	MarketData(const MarketData&) = delete;
	MarketData& operator=(const MarketData&) = delete;

	// moveable
	MarketData(MarketData&& rhs) noexcept : m_name(std::move(rhs.m_name)), m_category(rhs.m_category),
		m_requiredLevel(rhs.m_requiredLevel), m_restrictVocation(rhs.m_restrictVocation), m_showAs(rhs.m_showAs), m_tradeAs(rhs.m_tradeAs) {}
	MarketData& operator=(MarketData&& rhs) noexcept
	{
		if(this != &rhs)
		{
			m_name = std::move(rhs.m_name);
			m_category = rhs.m_category;
			m_requiredLevel = rhs.m_requiredLevel;
			m_restrictVocation = rhs.m_restrictVocation;
			m_showAs = rhs.m_showAs;
			m_tradeAs = rhs.m_tradeAs;
		}
		return (*this);
	}

	std::string m_name;
	Uint16 m_category = 0;
	Uint16 m_requiredLevel = 0;
	Uint16 m_restrictVocation = 0;
	Uint16 m_showAs = 0;
	Uint16 m_tradeAs = 0;
};

struct SaleData
{
	SaleData() : m_moneyType(3031) {}

	// non-copyable
	SaleData(const SaleData&) = delete;
	SaleData& operator=(const SaleData&) = delete;

	// moveable
	SaleData(SaleData&& rhs) noexcept :
		m_name(std::move(rhs.m_name)), m_location(std::move(rhs.m_location)), m_buyPrice(rhs.m_buyPrice), m_sellPrice(rhs.m_sellPrice), m_moneyType(rhs.m_moneyType) {}
	SaleData& operator=(SaleData&& rhs) noexcept
	{
		if(this != &rhs)
		{
			m_name = std::move(rhs.m_name);
			m_location = std::move(rhs.m_location);
			m_buyPrice = rhs.m_buyPrice;
			m_sellPrice = rhs.m_sellPrice;
			m_moneyType = rhs.m_moneyType;
		}
		return (*this);
	}

	std::string m_name;
	std::string m_location;
	Uint32 m_buyPrice = 0;
	Uint32 m_sellPrice = 0;
	Uint32 m_moneyType;
};

class ThingType
{
	public:
		ThingType() = default;

		// non-copyable
		ThingType(const ThingType&) = delete;
		ThingType& operator=(const ThingType&) = delete;

		// moveable
		ThingType(ThingType&& rhs) noexcept :
			m_salesData(std::move(rhs.m_salesData)), m_flags(rhs.m_flags), m_marketData(std::move(rhs.m_marketData)), m_category(rhs.m_category),
			m_groundSpeed(rhs.m_groundSpeed), m_writableSize(rhs.m_writableSize), m_elevation(rhs.m_elevation), m_minimapColor(rhs.m_minimapColor),
			m_lensHelp(rhs.m_lensHelp), m_cloth(rhs.m_cloth), m_defaultAction(rhs.m_defaultAction), m_changeToExpire(rhs.m_changeToExpire),
			m_cyclopediaType(rhs.m_cyclopediaType), m_id(rhs.m_id)
		{
			m_frameGroup[0] = std::move(rhs.m_frameGroup[0]);
			m_frameGroup[1] = std::move(rhs.m_frameGroup[1]);
			m_displacement[0] = rhs.m_displacement[0];
			m_displacement[1] = rhs.m_displacement[1];
			m_light[0] = rhs.m_light[0];
			m_light[1] = rhs.m_light[1];
			rhs.m_category = ThingCategory_Invalid;
			rhs.m_id = 0;
		}
		ThingType& operator=(ThingType&& rhs) noexcept
		{
			if(this != &rhs)
			{
				m_salesData = std::move(rhs.m_salesData);
				m_flags = rhs.m_flags;
				m_frameGroup[0] = std::move(rhs.m_frameGroup[0]);
				m_frameGroup[1] = std::move(rhs.m_frameGroup[1]);
				m_marketData = std::move(rhs.m_marketData);
				m_category = rhs.m_category;
				m_groundSpeed = rhs.m_groundSpeed;
				m_writableSize = rhs.m_writableSize;
				m_displacement[0] = rhs.m_displacement[0];
				m_displacement[1] = rhs.m_displacement[1];
				m_light[0] = rhs.m_light[0];
				m_light[1] = rhs.m_light[1];
				m_elevation = rhs.m_elevation;
				m_minimapColor = rhs.m_minimapColor;
				m_lensHelp = rhs.m_lensHelp;
				m_cloth = rhs.m_cloth;
				m_defaultAction = rhs.m_defaultAction;
				m_changeToExpire = rhs.m_changeToExpire;
				m_cyclopediaType = rhs.m_cyclopediaType;
				m_id = rhs.m_id;
				rhs.m_category = ThingCategory_Invalid;
				rhs.m_id = 0;
			}
			return (*this);
		}

		void clear();

		bool loadType(Uint16 id, ThingCategory category, SDL_RWops* rwops);
		bool loadAppearance(Sint64 offsetLimit, Uint16& things, SDL_RWops* rwops);
		SDL_INLINE bool hasFlag(Uint64 flag) {return (m_flags & flag);}

		Uint32 getSprite(ThingFrameGroup f, Uint8 w, Uint8 h, Uint8 l, Uint8 x, Uint8 y, Uint8 z, Uint8 a);

		std::vector<SaleData> m_salesData;
		Uint64 m_flags = 0;
		FrameGroup m_frameGroup[ThingFrameGroup_Last];
		MarketData m_marketData;
		ThingCategory m_category = ThingCategory_Invalid;

		Uint16 m_groundSpeed = 0;
		Uint16 m_writableSize = 0;
		Uint16 m_displacement[2] = {};
		Uint16 m_light[2] = {};
		Uint16 m_elevation = 0;
		Uint16 m_minimapColor = 0;
		Uint16 m_lensHelp = 0;
		Uint16 m_cloth = 0;
		Uint16 m_defaultAction = 0;
		Uint16 m_changeToExpire = 0;
		Uint16 m_cyclopediaType = 0;

		Uint16 m_id = 0;
};

class ThingManager
{
	public:
		ThingManager() = default;
		~ThingManager();

		// non-copyable
		ThingManager(const ThingManager&) = delete;
		ThingManager& operator=(const ThingManager&) = delete;

		// non-moveable
		ThingManager(ThingManager&&) = delete;
		ThingManager& operator=(ThingManager&&) = delete;

		void clear();
		void unloadDat();

		bool loadDat(const char* filename);
		bool loadAppearances(const char* filename);
		ThingType* getThingType(ThingCategory category, Uint16 id);

		SDL_INLINE bool isDatLoaded() {return m_datLoaded;}
		SDL_INLINE bool isValidDatId(ThingCategory category, Uint16 id) {return (id > 0 && id < m_things[category].size());}

	protected:
		std::vector<ThingType> m_things[ThingCategory_Last];

		Uint16 m_goldCoinId = 0;
		Uint16 m_platinumCoinId = 0;
		Uint16 m_crystalCoinId = 0;
		Uint16 m_tibiaCoinId = 0;
		Uint16 m_stampedLetterId = 0;
		Uint16 m_supplyStashId = 0;

		bool m_datLoaded = false;
};

#endif /* __FILE_THINGMANAGER_h_ */
