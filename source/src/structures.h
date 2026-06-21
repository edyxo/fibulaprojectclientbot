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

#ifndef __FILE_STRUCTURES_h_
#define __FILE_STRUCTURES_h_

#include "defines.h"

struct Hotkey_Text
{
	Uint8 type;
	bool sendAutomatically;
	std::string* text;
};

struct Hotkey_Item
{
	Uint8 type;
	Uint8 usageType;
	Uint16 itemId;
	Uint8 itemSubtype;
};

typedef union HotkeyAction
{
	Uint8 type;
	Hotkey_Text text;
	Hotkey_Item item;
} HotkeyAction;

struct HotkeyUsage
{
	HotkeyAction action;
	SDL_Keycode keycode;
	ClientHotkeys hotkey;
	Uint16 modifers;
	Uint8 keyid;
};

struct ClientActionData
{
	ClientActionData() : creatureId(0), itemId(0), posX(0xFFFF), posY(0), posZ(0), posStack(0) {}

	Uint32 creatureId;
	Uint16 itemId;
	Uint16 posX;
	Uint16 posY;
	Uint8 posZ;
	Uint8 posStack;
};

struct LightMap
{
	LightMap() : r(0), g(0), b(0) {}

	Uint8 r;
	Uint8 g;
	Uint8 b;
};

struct ContextMenuChild
{
	ContextMenuChild() = default;

	// copyable
	ContextMenuChild(const ContextMenuChild& rhs) : text(rhs.text), shortcut(rhs.shortcut), eventId(rhs.eventId), childStyle(rhs.childStyle) {}
	ContextMenuChild& operator=(const ContextMenuChild& rhs)
	{
		if(this != &rhs)
		{
			text.assign(rhs.text);
			shortcut.assign(rhs.shortcut);
			eventId = rhs.eventId;
			childStyle = rhs.childStyle;
		}
		return (*this);
	}

	// moveable
	ContextMenuChild(ContextMenuChild&& rhs) noexcept : text(std::move(rhs.text)), shortcut(std::move(rhs.shortcut)), eventId(rhs.eventId), childStyle(rhs.childStyle) {}
	ContextMenuChild& operator=(ContextMenuChild&& rhs) noexcept
	{
		if(this != &rhs)
		{
			text = std::move(rhs.text);
			shortcut = std::move(rhs.shortcut);
			eventId = rhs.eventId;
			childStyle = rhs.childStyle;
		}
		return (*this);
	}

	std::string text;
	std::string shortcut;
	Uint32 eventId;
	Uint8 childStyle;
};

struct WorldDetail
{
	WorldDetail() = default;

	// copyable
	WorldDetail(const WorldDetail& rhs) : worldName(rhs.worldName), worldIp(rhs.worldIp), worldPort(rhs.worldPort), previewState(rhs.previewState) {}
	WorldDetail& operator=(const WorldDetail& rhs)
	{
		if(this != &rhs)
		{
			worldName.assign(rhs.worldName);
			worldIp.assign(rhs.worldIp);
			worldPort = rhs.worldPort;
			previewState = rhs.previewState;
		}
		return (*this);
	}

	// moveable
	WorldDetail(WorldDetail&& rhs) noexcept : worldName(std::move(rhs.worldName)), worldIp(std::move(rhs.worldIp)), worldPort(rhs.worldPort), previewState(rhs.previewState) {}
	WorldDetail& operator=(WorldDetail&& rhs) noexcept
	{
		if(this != &rhs)
		{
			worldName = std::move(rhs.worldName);
			worldIp = std::move(rhs.worldIp);
			worldPort = rhs.worldPort;
			previewState = rhs.previewState;
		}
		return (*this);
	}

	std::string worldName;
	std::string worldIp;
	Uint16 worldPort;
	bool previewState;
};

struct CharacterDetail
{
	CharacterDetail() = default;

	// copyable
	CharacterDetail(const CharacterDetail& rhs) : name(rhs.name), vocName(rhs.vocName), worldName(rhs.worldName), worldIp(rhs.worldIp),
		health(rhs.health), healthMax(rhs.healthMax), mana(rhs.mana), manaMax(rhs.manaMax), level(rhs.level), lookType(rhs.lookType), worldPort(rhs.worldPort),
		lookHead(rhs.lookHead), lookBody(rhs.lookBody), lookLegs(rhs.lookLegs), lookFeet(rhs.lookFeet), lookAddons(rhs.lookAddons), previewState(rhs.previewState) {}
	CharacterDetail& operator=(const CharacterDetail& rhs)
	{
		if(this != &rhs)
		{
			name.assign(rhs.name);
			vocName.assign(rhs.vocName);
			worldName.assign(rhs.worldName);
			worldIp.assign(rhs.worldIp);
			health = rhs.health;
			healthMax = rhs.healthMax;
			mana = rhs.mana;
			manaMax = rhs.manaMax;
			level = rhs.level;
			lookType = rhs.lookType;
			worldPort = rhs.worldPort;
			lookHead = rhs.lookHead;
			lookBody = rhs.lookBody;
			lookLegs = rhs.lookLegs;
			lookFeet = rhs.lookFeet;
			lookAddons = rhs.lookAddons;
			previewState = rhs.previewState;
		}
		return (*this);
	}

	// moveable
	CharacterDetail(CharacterDetail&& rhs) noexcept : name(std::move(rhs.name)), vocName(std::move(rhs.vocName)), worldName(std::move(rhs.worldName)), worldIp(std::move(rhs.worldIp)),
		health(rhs.health), healthMax(rhs.healthMax), mana(rhs.mana), manaMax(rhs.manaMax), level(rhs.level), lookType(rhs.lookType), worldPort(rhs.worldPort),
		lookHead(rhs.lookHead), lookBody(rhs.lookBody), lookLegs(rhs.lookLegs), lookFeet(rhs.lookFeet), lookAddons(rhs.lookAddons), previewState(rhs.previewState) {}
	CharacterDetail& operator=(CharacterDetail&& rhs) noexcept
	{
		if(this != &rhs)
		{
			name = std::move(rhs.name);
			vocName = std::move(rhs.vocName);
			worldName = std::move(rhs.worldName);
			worldIp = std::move(rhs.worldIp);
			health = rhs.health;
			healthMax = rhs.healthMax;
			mana = rhs.mana;
			manaMax = rhs.manaMax;
			level = rhs.level;
			lookType = rhs.lookType;
			worldPort = rhs.worldPort;
			lookHead = rhs.lookHead;
			lookBody = rhs.lookBody;
			lookLegs = rhs.lookLegs;
			lookFeet = rhs.lookFeet;
			lookAddons = rhs.lookAddons;
			previewState = rhs.previewState;
		}
		return (*this);
	}

	std::string name;
	std::string vocName;
	std::string worldName;
	std::string worldIp;
	Uint32 health;
	Uint32 healthMax;
	Uint32 mana;
	Uint32 manaMax;
	Uint16 level;
	Uint16 lookType;
	Uint16 worldPort;
	Uint8 lookHead;
	Uint8 lookBody;
	Uint8 lookLegs;
	Uint8 lookFeet;
	Uint8 lookAddons;
	bool previewState;
};

struct ChannelDetail
{
	ChannelDetail() = default;

	// copyable
	ChannelDetail(const ChannelDetail& rhs) : channelName(rhs.channelName), channelId(rhs.channelId) {}
	ChannelDetail& operator=(const ChannelDetail& rhs)
	{
		if(this != &rhs)
		{
			channelName.assign(rhs.channelName);
			channelId = rhs.channelId;
		}
		return (*this);
	}

	// moveable
	ChannelDetail(ChannelDetail&& rhs) noexcept : channelName(std::move(rhs.channelName)), channelId(rhs.channelId) {}
	ChannelDetail& operator=(ChannelDetail&& rhs) noexcept
	{
		if(this != &rhs)
		{
			channelName = std::move(rhs.channelName);
			channelId = rhs.channelId;
		}
		return (*this);
	}

	std::string channelName;
	Uint16 channelId;
};

struct OutfitDetail
{
	OutfitDetail() = default;

	// copyable
	OutfitDetail(const OutfitDetail& rhs) : outfitName(rhs.outfitName), outfitID(rhs.outfitID), outfitAddons(rhs.outfitAddons) {}
	OutfitDetail& operator=(const OutfitDetail& rhs)
	{
		if(this != &rhs)
		{
			outfitName.assign(rhs.outfitName);
			outfitID = rhs.outfitID;
			outfitAddons = rhs.outfitAddons;
		}
		return (*this);
	}

	// moveable
	OutfitDetail(OutfitDetail&& rhs) noexcept : outfitName(std::move(rhs.outfitName)), outfitID(rhs.outfitID), outfitAddons(rhs.outfitAddons) {}
	OutfitDetail& operator=(OutfitDetail&& rhs) noexcept
	{
		if(this != &rhs)
		{
			outfitName = std::move(rhs.outfitName);
			outfitID = rhs.outfitID;
			outfitAddons = rhs.outfitAddons;
		}
		return (*this);
	}

	std::string outfitName;
	Uint16 outfitID;
	Uint8 outfitAddons;
};

struct MountDetail
{
	MountDetail() = default;

	// copyable
	MountDetail(const MountDetail& rhs) : mountName(rhs.mountName), mountID(rhs.mountID) {}
	MountDetail& operator=(const MountDetail& rhs)
	{
		if(this != &rhs)
		{
			mountName.assign(rhs.mountName);
			mountID = rhs.mountID;
		}
		return (*this);
	}

	// moveable
	MountDetail(MountDetail&& rhs) noexcept : mountName(std::move(rhs.mountName)), mountID(rhs.mountID) {}
	MountDetail& operator=(MountDetail&& rhs) noexcept
	{
		if(this != &rhs)
		{
			mountName = std::move(rhs.mountName);
			mountID = rhs.mountID;
		}
		return (*this);
	}

	std::string mountName;
	Uint16 mountID;
};

struct QuestLogDetail
{
	QuestLogDetail() = default;

	// copyable
	QuestLogDetail(const QuestLogDetail& rhs) : questName(rhs.questName), questId(rhs.questId), questCompleted(rhs.questCompleted) {}
	QuestLogDetail& operator=(const QuestLogDetail& rhs)
	{
		if(this != &rhs)
		{
			questName.assign(rhs.questName);
			questId = rhs.questId;
			questCompleted = rhs.questCompleted;
		}
		return (*this);
	}

	// moveable
	QuestLogDetail(QuestLogDetail&& rhs) noexcept : questName(std::move(rhs.questName)), questId(rhs.questId), questCompleted(rhs.questCompleted) {}
	QuestLogDetail& operator=(QuestLogDetail&& rhs) noexcept
	{
		if(this != &rhs)
		{
			questName = std::move(rhs.questName);
			questId = rhs.questId;
			questCompleted = rhs.questCompleted;
		}
		return (*this);
	}

	std::string questName;
	Uint16 questId;
	bool questCompleted;
};

struct QuestLineDetail
{
	QuestLineDetail() = default;

	// copyable
	QuestLineDetail(const QuestLineDetail& rhs) : missionName(rhs.missionName), missionDescription(rhs.missionDescription), missionId(rhs.missionId) {}
	QuestLineDetail& operator=(const QuestLineDetail& rhs)
	{
		if(this != &rhs)
		{
			missionName.assign(rhs.missionName);
			missionDescription.assign(rhs.missionDescription);
			missionId = rhs.missionId;
		}
		return (*this);
	}

	// moveable
	QuestLineDetail(QuestLineDetail&& rhs) noexcept : missionName(std::move(rhs.missionName)), missionDescription(std::move(rhs.missionDescription)), missionId(rhs.missionId) {}
	QuestLineDetail& operator=(QuestLineDetail&& rhs) noexcept
	{
		if(this != &rhs)
		{
			missionName = std::move(rhs.missionName);
			missionDescription = std::move(rhs.missionDescription);
			missionId = rhs.missionId;
		}
		return (*this);
	}

	std::string missionName;
	std::string missionDescription;
	Uint16 missionId;
};

struct RewardCollection
{
	Uint16 itemId;
	Uint8 itemCount;
};

struct RequestItems
{
	Uint16 itemId;
	Uint8 itemCount;
};

using NPCTradeGoods = std::map<Uint16, Uint16>;
struct NPCTradeItems
{
	NPCTradeItems() = default;

	// copyable
	NPCTradeItems(const NPCTradeItems& rhs) :
		itemName(rhs.itemName), itemWeight(rhs.itemWeight), itemBuyPrice(rhs.itemBuyPrice), itemSellPrice(rhs.itemSellPrice), itemId(rhs.itemId), itemSubtype(rhs.itemSubtype) {}
	NPCTradeItems& operator=(const NPCTradeItems& rhs)
	{
		if(this != &rhs)
		{
			itemName.assign(rhs.itemName);
			itemWeight = rhs.itemWeight;
			itemBuyPrice = rhs.itemBuyPrice;
			itemSellPrice = rhs.itemSellPrice;
			itemId = rhs.itemId;
			itemSubtype = rhs.itemSubtype;
		}
		return (*this);
	}

	// moveable
	NPCTradeItems(NPCTradeItems&& rhs) noexcept :
		itemName(std::move(rhs.itemName)), itemWeight(rhs.itemWeight), itemBuyPrice(rhs.itemBuyPrice), itemSellPrice(rhs.itemSellPrice), itemId(rhs.itemId), itemSubtype(rhs.itemSubtype) {}
	NPCTradeItems& operator=(NPCTradeItems&& rhs) noexcept
	{
		if(this != &rhs)
		{
			itemName = std::move(rhs.itemName);
			itemWeight = rhs.itemWeight;
			itemBuyPrice = rhs.itemBuyPrice;
			itemSellPrice = rhs.itemSellPrice;
			itemId = rhs.itemId;
			itemSubtype = rhs.itemSubtype;
		}
		return (*this);
	}

	std::string itemName;
	Uint32 itemWeight;
	Uint32 itemBuyPrice;
	Uint32 itemSellPrice;
	Uint16 itemId;
	Uint8 itemSubtype;
};

struct VipData
{
	VipData() = default;

	// copyable
	VipData(const VipData& rhs) : description(rhs.description), iconId(rhs.iconId), notifyLogin(rhs.notifyLogin) {}
	VipData& operator=(const VipData& rhs)
	{
		if(this != &rhs)
		{
			description.assign(rhs.description);
			iconId = rhs.iconId;
			notifyLogin = rhs.notifyLogin;
		}
		return (*this);
	}

	// moveable
	VipData(VipData&& rhs) noexcept : description(std::move(rhs.description)), iconId(rhs.iconId), notifyLogin(rhs.notifyLogin)	{}
	VipData& operator=(VipData&& rhs) noexcept
	{
		if(this != &rhs)
		{
			description = std::move(rhs.description);
			iconId = rhs.iconId;
			notifyLogin = rhs.notifyLogin;
		}
		return (*this);
	}

	std::string description;
	Uint32 iconId;
	bool notifyLogin;
};

struct VipPlayers
{
	VipPlayers() = default;

	// copyable
	VipPlayers(const VipPlayers& rhs) : groups(rhs.groups), playerName(rhs.playerName), description(rhs.description),
		playerGUID(rhs.playerGUID), iconId(rhs.iconId), status(rhs.status), notifyLogin(rhs.notifyLogin), localOnly(rhs.localOnly) {}
	VipPlayers& operator=(const VipPlayers& rhs)
	{
		if(this != &rhs)
		{
			groups.assign(rhs.groups.begin(), rhs.groups.end());
			playerName.assign(rhs.playerName);
			description.assign(rhs.description);
			playerGUID = rhs.playerGUID;
			iconId = rhs.iconId;
			status = rhs.status;
			notifyLogin = rhs.notifyLogin;
			localOnly = rhs.localOnly;
		}
		return (*this);
	}

	// moveable
	VipPlayers(VipPlayers&& rhs) noexcept : groups(std::move(rhs.groups)), playerName(std::move(rhs.playerName)), description(std::move(rhs.description)),
		playerGUID(rhs.playerGUID), iconId(rhs.iconId), status(rhs.status), notifyLogin(rhs.notifyLogin), localOnly(rhs.localOnly) {}
	VipPlayers& operator=(VipPlayers&& rhs) noexcept
	{
		if(this != &rhs)
		{
			groups = std::move(rhs.groups);
			playerName = std::move(rhs.playerName);
			description = std::move(rhs.description);
			playerGUID = rhs.playerGUID;
			iconId = rhs.iconId;
			status = rhs.status;
			notifyLogin = rhs.notifyLogin;
			localOnly = rhs.localOnly;
		}
		return (*this);
	}

	std::vector<Uint8> groups;
	std::string playerName;
	std::string description;
	Uint32 playerGUID;
	Uint32 iconId;
	Uint8 status;
	bool notifyLogin;
	bool localOnly = false;
};

struct VipGroups
{
	VipGroups() = default;

	// copyable
	VipGroups(const VipGroups& rhs) : groupName(rhs.groupName), groupId(rhs.groupId), groupRemovable(rhs.groupRemovable) {}
	VipGroups& operator=(const VipGroups& rhs)
	{
		if(this != &rhs)
		{
			groupName.assign(rhs.groupName);
			groupId = rhs.groupId;
			groupRemovable = rhs.groupRemovable;
		}
		return (*this);
	}

	// moveable
	VipGroups(VipGroups&& rhs) noexcept : groupName(std::move(rhs.groupName)), groupId(rhs.groupId), groupRemovable(rhs.groupRemovable)	{}
	VipGroups& operator=(VipGroups&& rhs) noexcept
	{
		if(this != &rhs)
		{
			groupName = std::move(rhs.groupName);
			groupId = rhs.groupId;
			groupRemovable = rhs.groupRemovable;
		}
		return (*this);
	}

	std::string groupName;
	Uint8 groupId;
	bool groupRemovable;
};

struct VipActionPlayer
{
	Uint8 type;
	Uint32 playerGUID;
};

struct VipActionGroup
{
	Uint8 type;
	Uint8 groupId;
};

typedef union VipAction
{
	Uint8 type;
	VipActionPlayer player;
	VipActionGroup group;
} VipAction;

struct InspectObject_NormalObject
{
	Uint8 type;
	Uint16 posX;
	Uint16 posY;
	Uint8 posZ;
};

struct InspectObject_NPCTrade
{
	Uint8 type;
	Uint16 itemId;
	Uint8 itemCount;
};

struct InspectObject_Cyclopedia
{
	Uint8 type;
	Uint16 itemId;
};

typedef union InspectObjectData
{
	Uint8 type;
	InspectObject_NormalObject normalObject;
	InspectObject_NPCTrade npcTrade;
	InspectObject_Cyclopedia cyclopedia;
} InspectObjectData;

struct LootContainer_AssignContainer
{
	Uint8 action;
	Uint8 objectCategory;
	Uint16 posX;
	Uint16 posY;
	Uint8 posZ;
	Uint16 itemId;
	Uint8 itemCount;
};

struct LootContainer_ClearContainer
{
	Uint8 action;
	Uint8 objectCategory;
};

struct LootContainer_SetUseFallback
{
	Uint8 action;
	bool useFallback;
};

typedef union LootContainerData
{
	Uint8 action;
	LootContainer_AssignContainer assignContainer;
	LootContainer_ClearContainer clearContainer;
	LootContainer_SetUseFallback setUseFallback;
} LootContainerData;

struct SpellData
{
	SpellData(const char* a1, const char* a2, Uint8 a3, Uint8 a4, Uint32 a5, Uint8 a6, Uint16 a7, bool a8, Uint8 a9, Uint32 a10, Uint8 a11, Uint16 a12, Uint16 a13):
		spellIncantation(a1), spellName(a2), spellGroup(a3), spellSecondGroup(0), spellId(a4), spellPrice(a5), spellType(a6), spellReqLevel(a7),
		spellVocations(a9), spellReqPremium(a8), spellReqMana(a10), spellReqSoul(a11), spellRuneId(0), spellCooldown(a12), spellGroupCooldown(a13), spellSecondGroupCooldown(0) {}
	
	SpellData(const char* a1, const char* a2, Uint8 a3, Uint8 a4, Uint8 a5, Uint32 a6, Uint8 a7, Uint16 a8, bool a9, Uint8 a10, Uint32 a11, Uint8 a12, Uint16 a13, Uint16 a14, Uint16 a15):
		spellIncantation(a1), spellName(a2), spellGroup(a3), spellSecondGroup(a4), spellId(a5), spellPrice(a6), spellType(a7), spellReqLevel(a8),
		spellVocations(a10), spellReqPremium(a9), spellReqMana(a11), spellReqSoul(a12), spellRuneId(0), spellCooldown(a13), spellGroupCooldown(a14), spellSecondGroupCooldown(a15) {}

	SpellData(const char* a1, const char* a2, Uint8 a3, Uint8 a4, Uint32 a5, Uint8 a6, Uint16 a7, bool a8, Uint8 a9, Uint32 a10, Uint8 a11, Uint16 a12, Uint16 a13, Uint16 a14):
		spellIncantation(a1), spellName(a2), spellGroup(a3), spellSecondGroup(0), spellId(a4), spellPrice(a5), spellType(a6), spellReqLevel(a7),
		spellVocations(a9), spellReqPremium(a8), spellReqMana(a10), spellReqSoul(a11), spellRuneId(a12), spellCooldown(a13), spellGroupCooldown(a14), spellSecondGroupCooldown(0) {}

	SpellData(const char* a1, const char* a2, Uint8 a3, Uint8 a4, Uint8 a5, Uint32 a6, Uint8 a7, Uint16 a8, bool a9, Uint8 a10, Uint32 a11, Uint8 a12, Uint16 a13, Uint16 a14, Uint16 a15, Uint16 a16):
		spellIncantation(a1), spellName(a2), spellGroup(a3), spellSecondGroup(a4), spellId(a5), spellPrice(a6), spellType(a7), spellReqLevel(a8),
		spellVocations(a10), spellReqPremium(a9), spellReqMana(a11), spellReqSoul(a12), spellRuneId(a13), spellCooldown(a14), spellGroupCooldown(a15), spellSecondGroupCooldown(a16) {}

	const char* spellIncantation;
	const char* spellName;
	Uint32 spellPrice;
	Uint32 spellReqMana;
	Uint16 spellReqLevel;
	Uint16 spellCooldown;
	Uint16 spellGroupCooldown;
	Uint16 spellSecondGroupCooldown;
	Uint16 spellRuneId;
	Uint8 spellGroup;
	Uint8 spellSecondGroup;
	Uint8 spellId;
	Uint8 spellType;
	Uint8 spellVocations;
	Uint8 spellReqSoul;
	bool spellReqPremium;
};

struct HouseData
{
	HouseData(const char* a1, Uint32 a2, Uint32 a3, Uint32 a4, Uint32 a5, Uint32 a6, Uint32 a7, Uint16 a8, Uint16 a9, Uint8 a10, Uint8 a11) :
		houseName(a1), houseTownId(a2), houseId(a3), houseFloorBits(a4), houseSQMs(a5), houseBeds(a6), houseRent(a7), housePosX(a8), housePosY(a9), housePosZ(a10), houseBits(a11) {}

	const char* houseName;
	Uint32 houseTownId;
	Uint32 houseId;
	Uint32 houseFloorBits;
	Uint32 houseSQMs;
	Uint32 houseBeds;
	Uint32 houseRent;
	Uint16 housePosX;
	Uint16 housePosY;
	Uint8 housePosZ;
	Uint8 houseBits;
};

#endif /* __FILE_STRUCTURES_h_ */

