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

#include "itemUI.h"
#include "../thingManager.h"
#include "../engine.h"
#include "../game.h"

extern Engine g_engine;
extern Game g_game;
extern ThingManager g_thingManager;
extern Uint32 g_frameTime;

ItemUI* ItemUI::createItemUI(Uint16 itemId, Uint16 count, Sint32 phase)
{
	return ItemUI::createItemUI(g_thingManager.getThingType(ThingCategory_Item, itemId), count, phase);
}

ItemUI* ItemUI::createItemUI(ThingType* type, Uint16 count, Sint32 phase)
{
	if(!type)
		return NULL;

	ItemUI* newItem = new ItemUI(type);
	Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, count));
	if(type->hasFlag(ThingAttribute_Stackable))
	{
		newItem->m_count = count;
		if(type->m_frameGroup[ThingFrameGroup_Default].m_patternX == 4 && type->m_frameGroup[ThingFrameGroup_Default].m_patternY == 2)
		{
			if(count <= 1)
			{
				newItem->m_xPattern = 0;
				newItem->m_yPattern = 0;
			}
			else if(count < 5)
			{
				newItem->m_xPattern = SDL_static_cast(Uint8, count - 1);
				newItem->m_yPattern = 0;
			}
			else
			{
				newItem->m_xPattern = ((count < 10) ? 0 : (count < 25) ? 1 : (count < 50) ? 2 : 3);
				newItem->m_yPattern = 1;
			}
		}
		newItem->m_stringCount = std::string(g_buffer, SDL_static_cast(size_t, len));
		if(count > 1)
			newItem->m_displayCount = true;
	}
	else if(type->hasFlag(ThingAttribute_Chargeable))
	{
		newItem->m_stringCount = std::string(g_buffer, SDL_static_cast(size_t, len));
		if(count > 1)
			newItem->m_displayCount = true;
	}
	else if(type->hasFlag(ThingAttribute_Splash) || type->hasFlag(ThingAttribute_FluidContainer))
	{
		newItem->m_subtype = SDL_static_cast(Uint8, count);
		Sint32 fluid = SDL_static_cast(Sint32, count);
		if(g_game.hasGameFeature(GAME_FEATURE_NEWFLUIDS))
		{
			switch(count)
			{
				case FluidNone:
					fluid = FluidTransparent;
					break;
				case FluidWater:
					fluid = FluidBlue;
					break;
				case FluidMana:
					fluid = FluidPurple;
					break;
				case FluidBeer:
					fluid = FluidBrown;
					break;
				case FluidOil:
					fluid = FluidBrown;
					break;
				case FluidBlood:
					fluid = FluidRed;
					break;
				case FluidSlime:
					fluid = FluidGreen;
					break;
				case FluidMud:
					fluid = FluidBrown;
					break;
				case FluidLemonade:
					fluid = FluidYellow;
					break;
				case FluidMilk:
					fluid = FluidWhite;
					break;
				case FluidWine:
					fluid = FluidPurple;
					break;
				case FluidHealth:
					fluid = FluidRed;
					break;
				case FluidUrine:
					fluid = FluidYellow;
					break;
				case FluidRum:
					fluid = FluidBrown;
					break;
				case FluidFruidJuice:
					fluid = FluidYellow;
					break;
				case FluidCoconutMilk:
					fluid = FluidWhite;
					break;
				case FluidTea:
					fluid = FluidBrown;
					break;
				case FluidMead:
					fluid = FluidBrown;
					break;
				default:
					fluid = FluidTransparent;
					break;
			}
		}
		newItem->m_xPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (fluid % 4)), type->m_frameGroup[ThingFrameGroup_Default].m_patternX);
		newItem->m_yPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (fluid / 4)), type->m_frameGroup[ThingFrameGroup_Default].m_patternY);
	}
	newItem->m_animCount = type->m_frameGroup[ThingFrameGroup_Default].m_animCount;
	newItem->m_animator = type->m_frameGroup[ThingFrameGroup_Default].m_animator;
	if(newItem->m_animator)
		newItem->m_animator->resetAnimation(newItem->m_animation, phase);
	return newItem;
}

void ItemUI::setSubtype(Uint16 count, bool showCount)
{
	if(!m_thingType)
		return;

	Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u", SDL_static_cast(Uint32, count));
	if(m_thingType->hasFlag(ThingAttribute_Stackable))
	{
		m_count = count;
		if(m_thingType->m_frameGroup[ThingFrameGroup_Default].m_patternX == 4 && m_thingType->m_frameGroup[ThingFrameGroup_Default].m_patternY == 2)
		{
			if(count <= 1)
			{
				m_xPattern = 0;
				m_yPattern = 0;
			}
			else if(count < 5)
			{
				m_xPattern = SDL_static_cast(Uint8, count - 1);
				m_yPattern = 0;
			}
			else
			{
				m_xPattern = ((count < 10) ? 0 : (count < 25) ? 1 : (count < 50) ? 2 : 3);
				m_yPattern = 1;
			}
		}
		m_stringCount = std::string(g_buffer, SDL_static_cast(size_t, len));
		m_displayCount = showCount;
	}
	else if(m_thingType->hasFlag(ThingAttribute_Chargeable))
	{
		m_stringCount = std::string(g_buffer, SDL_static_cast(size_t, len));
		m_displayCount = showCount;
	}
	else if(m_thingType->hasFlag(ThingAttribute_Splash) || m_thingType->hasFlag(ThingAttribute_FluidContainer))
	{
		m_subtype = SDL_static_cast(Uint8, count);
		Sint32 fluid = SDL_static_cast(Sint32, count);
		if(g_game.hasGameFeature(GAME_FEATURE_NEWFLUIDS))
		{
			switch(count)
			{
				case FluidNone:
					fluid = FluidTransparent;
					break;
				case FluidWater:
					fluid = FluidBlue;
					break;
				case FluidMana:
					fluid = FluidPurple;
					break;
				case FluidBeer:
					fluid = FluidBrown;
					break;
				case FluidOil:
					fluid = FluidBrown;
					break;
				case FluidBlood:
					fluid = FluidRed;
					break;
				case FluidSlime:
					fluid = FluidGreen;
					break;
				case FluidMud:
					fluid = FluidBrown;
					break;
				case FluidLemonade:
					fluid = FluidYellow;
					break;
				case FluidMilk:
					fluid = FluidWhite;
					break;
				case FluidWine:
					fluid = FluidPurple;
					break;
				case FluidHealth:
					fluid = FluidRed;
					break;
				case FluidUrine:
					fluid = FluidYellow;
					break;
				case FluidRum:
					fluid = FluidBrown;
					break;
				case FluidFruidJuice:
					fluid = FluidYellow;
					break;
				case FluidCoconutMilk:
					fluid = FluidWhite;
					break;
				case FluidTea:
					fluid = FluidBrown;
					break;
				case FluidMead:
					fluid = FluidBrown;
					break;
				default:
					fluid = FluidTransparent;
					break;
			}
		}
		m_xPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (fluid % 4)), m_thingType->m_frameGroup[ThingFrameGroup_Default].m_patternX);
		m_yPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (fluid / 4)), m_thingType->m_frameGroup[ThingFrameGroup_Default].m_patternY);
	}
}

Uint16 ItemUI::getID()
{
	if(m_thingType)
		return m_thingType->m_id;

	return 0;
}

void ItemUI::render(Sint32 posX, Sint32 posY, Sint32 scaled)
{
	Uint8 animation;
	if(m_animCount > 1)
	{
		if(m_animator)
			animation = SDL_static_cast(Uint8, m_animator->getPhase(m_animation));
		else
			animation = (SDL_static_cast(Uint8, (g_frameTime / ITEM_TICKS_PER_FRAME)) % m_animCount);
	}
	else
		animation = 0;

	g_engine.drawItem(m_thingType, posX, posY, scaled, m_xPattern, m_yPattern, m_zPattern, animation);
	if(m_displayCount)
		g_engine.drawFont(CLIENT_FONT_OUTLINED, posX + scaled - 1, posY + scaled - 11, m_stringCount, 192, 192, 192, CLIENT_FONT_ALIGN_RIGHT);
}
