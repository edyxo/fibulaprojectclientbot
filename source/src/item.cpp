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

#include "item.h"
#include "thingManager.h"
#include "engine.h"
#include "game.h"
#include "light.h"

extern Engine g_engine;
extern Game g_game;
extern ThingManager g_thingManager;
extern LightSystem g_light;
extern Uint32 g_frameTime;

Item* Item::createItem(const Position& pos, Uint16 type, Uint16 count, Sint32 phase)
{
	Item* newItem = NULL;
	ThingType* ttype = g_thingManager.getThingType(ThingCategory_Item, type);
	if(ttype)
	{
		Uint16 itemCount = 1;
		Uint8 itemSubtype = 0;
		Uint8 animCount = ttype->m_frameGroup[ThingFrameGroup_Default].m_animCount;
		Uint8 xPattern = 0;
		Uint8 yPattern = 0;
		Uint8 zPattern = 0;
		if(ttype->hasFlag(ThingAttribute_Stackable))
		{
			itemCount = count;
			if(ttype->m_frameGroup[ThingFrameGroup_Default].m_patternX == 4 && ttype->m_frameGroup[ThingFrameGroup_Default].m_patternY == 2)
			{
				if(count <= 1)
				{
					xPattern = 0;
					yPattern = 0;
				}
				else if(count < 5)
				{
					xPattern = SDL_static_cast(Uint8, count - 1);
					yPattern = 0;
				}
				else
				{
					xPattern = ((count < 10) ? 0 : (count < 25) ? 1 : (count < 50) ? 2 : 3);
					yPattern = 1;
				}
			}
		}
		else if(ttype->hasFlag(ThingAttribute_Hangable))
		{
			if(count == 2)
				xPattern = (ttype->m_frameGroup[ThingFrameGroup_Default].m_patternX >= 2 ? 1 : 0);
			else if(count == 3)
				xPattern = (ttype->m_frameGroup[ThingFrameGroup_Default].m_patternX >= 3 ? 2 : 0);
		}
		else if(ttype->hasFlag(ThingAttribute_Splash) || ttype->hasFlag(ThingAttribute_FluidContainer))
		{
			itemSubtype = SDL_static_cast(Uint8, count);
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
			xPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (fluid % 4)), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternX);
			yPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (fluid / 4)), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternY);
		}
		else
		{
			xPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, pos.x), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternX);
			yPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, pos.y), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternY);
			zPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, pos.z), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternZ);
		}
		Uint8 layers = ttype->m_frameGroup[ThingFrameGroup_Default].m_layers;
		if(layers <= 2 && animCount <= ITEM_MAX_CACHED_ANIMATIONS)
		{
			Uint8 width = ttype->m_frameGroup[ThingFrameGroup_Default].m_width;
			Uint8 height = ttype->m_frameGroup[ThingFrameGroup_Default].m_height;
			if(layers == 2)
			{
				if(width == 1 && height == 1)
				{
					Item1X1X2* newItemX = new Item1X1X2(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
					{
						newItemX->m_1X1X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_1X1X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 1, xPattern, yPattern, zPattern, a);
					}
					newItem = newItemX;
				}
				else if(width == 2 && height == 1)
				{
					Item2X1X2* newItemX = new Item2X1X2(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
					{
						newItemX->m_2X1X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X1X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 1, xPattern, yPattern, zPattern, a);
						newItemX->m_2X1X2Sprites[a][2] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X1X2Sprites[a][3] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 1, xPattern, yPattern, zPattern, a);
					}
					newItem = newItemX;
				}
				else if(width == 1 && height == 2)
				{
					Item1X2X2* newItemX = new Item1X2X2(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
					{
						newItemX->m_1X2X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_1X2X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 1, xPattern, yPattern, zPattern, a);
						newItemX->m_1X2X2Sprites[a][2] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_1X2X2Sprites[a][3] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 1, xPattern, yPattern, zPattern, a);
					}
					newItem = newItemX;
				}
				else if(width == 2 && height == 2)
				{
					Item2X2X2* newItemX = new Item2X2X2(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
					{
						newItemX->m_2X2X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 1, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2X2Sprites[a][2] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2X2Sprites[a][3] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 1, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2X2Sprites[a][4] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2X2Sprites[a][5] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 1, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2X2Sprites[a][6] = ttype->getSprite(ThingFrameGroup_Default, 1, 1, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2X2Sprites[a][7] = ttype->getSprite(ThingFrameGroup_Default, 1, 1, 1, xPattern, yPattern, zPattern, a);
					}
					newItem = newItemX;
				}
			}
			else
			{
				if(width == 1 && height == 1)
				{
					Item1X1* newItemX = new Item1X1(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
						newItemX->m_1X1Sprites[a] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);

					newItem = newItemX;
				}
				else if(width == 2 && height == 1)
				{
					Item2X1* newItemX = new Item2X1(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
					{
						newItemX->m_2X1Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X1Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, xPattern, yPattern, zPattern, a);
					}
					newItem = newItemX;
				}
				else if(width == 1 && height == 2)
				{
					Item1X2* newItemX = new Item1X2(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
					{
						newItemX->m_1X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_1X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, xPattern, yPattern, zPattern, a);
					}
					newItem = newItemX;
				}
				else if(width == 2 && height == 2)
				{
					Item2X2* newItemX = new Item2X2(pos, ttype);
					for(Uint8 a = 0; a < animCount; ++a)
					{
						newItemX->m_2X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2Sprites[a][2] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, xPattern, yPattern, zPattern, a);
						newItemX->m_2X2Sprites[a][3] = ttype->getSprite(ThingFrameGroup_Default, 1, 1, 0, xPattern, yPattern, zPattern, a);
					}
					newItem = newItemX;
				}
			}
		}

		if(!newItem)
			newItem = new Item(pos, ttype);

		newItem->m_count = itemCount;
		newItem->m_subtype = itemSubtype;
		newItem->m_animator = ttype->m_frameGroup[ThingFrameGroup_Default].m_animator;
		newItem->m_animCount = animCount;
		newItem->m_xPattern = xPattern;
		newItem->m_yPattern = yPattern;
		newItem->m_zPattern = zPattern;
		newItem->m_elevation = ttype->m_elevation;
		newItem->m_hasElevation = ttype->hasFlag(ThingAttribute_Elevation);
		newItem->m_topOrder  = (ttype->hasFlag(ThingAttribute_GroundBorder) ? 1 : ttype->hasFlag(ThingAttribute_OnBottom) ? 2 : ttype->hasFlag(ThingAttribute_OnTop) ? 3 : 0);
		if(newItem->m_animator)
			newItem->m_animator->resetAnimation(newItem->m_animation, phase);
	}
	else
		newItem = new ItemNULL(pos, ttype);

	return newItem;
}

Uint16 Item::getID()
{
	if(m_thingType)
		return m_thingType->m_id;

	return 0;
}

Uint8 Item::calculateAnimationPhase()
{
	if(m_animCount > 1)
	{
		if(m_animator)
			return SDL_static_cast(Uint8, m_animator->getPhase(m_animation));

		return (SDL_static_cast(Uint8, (g_frameTime / ITEM_TICKS_PER_FRAME)) % m_animCount);
	}
	return 0;
}

void Item::render(Sint32 posX, Sint32 posY, bool)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint8 animationFrame = calculateAnimationPhase();
	for(Uint8 l = 0; l < m_thingType->m_frameGroup[ThingFrameGroup_Default].m_layers; ++l)
	{
		Sint32 posYc = posY;
		for(Uint8 y = 0; y < m_thingType->m_frameGroup[ThingFrameGroup_Default].m_height; ++y)
		{
			Sint32 posXc = posX;
			for(Uint8 x = 0; x < m_thingType->m_frameGroup[ThingFrameGroup_Default].m_width; ++x)
			{
				Uint32 sprite = m_thingType->getSprite(ThingFrameGroup_Default, x, y, l, m_xPattern, m_yPattern, m_zPattern, animationFrame);
				if(sprite != 0)
					renderer->drawSprite(sprite, posXc, posYc);
				posXc -= 32;
			}
			posYc -= 32;
		}
	}
}

void Item1X1::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	if(!visible_tile)
		return;

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite = m_1X1Sprites[animationFrame];
	if(drawSprite != 0)
		g_engine.getRender()->drawSprite(drawSprite, posX, posY);
}

void Item2X1::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite;
	if(visible_tile)
	{
		drawSprite = m_2X1Sprites[animationFrame][0];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);
	}

	drawSprite = m_2X1Sprites[animationFrame][1];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}

void Item1X2::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite = m_1X2Sprites[animationFrame][0];
	if(visible_tile)
	{
		drawSprite = m_1X2Sprites[animationFrame][0];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);
	}

	drawSprite = m_1X2Sprites[animationFrame][1];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY - 32);
}

void Item2X2::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite;
	if(visible_tile)
	{
		drawSprite = m_2X2Sprites[animationFrame][0];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);
	}

	drawSprite = m_2X2Sprites[animationFrame][1];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);

	posY -= 32;
	drawSprite = m_2X2Sprites[animationFrame][2];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X2Sprites[animationFrame][3];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}

void Item1X1X2::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	if(!visible_tile)
		return;

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite = m_1X1X2Sprites[animationFrame][0];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_1X1X2Sprites[animationFrame][1];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);
}

void Item2X1X2::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite;
	if(visible_tile)
	{
		drawSprite = m_2X1X2Sprites[animationFrame][0];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);

		drawSprite = m_2X1X2Sprites[animationFrame][1];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);
	}

	drawSprite = m_2X1X2Sprites[animationFrame][2];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);

	drawSprite = m_2X1X2Sprites[animationFrame][3];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}

void Item1X2X2::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite;
	if(visible_tile)
	{
		drawSprite = m_1X2X2Sprites[animationFrame][0];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);

		drawSprite = m_1X2X2Sprites[animationFrame][1];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);
	}

	drawSprite = m_1X2X2Sprites[animationFrame][2];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY - 32);

	drawSprite = m_1X2X2Sprites[animationFrame][3];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY - 32);
}

void Item2X2X2::render(Sint32 posX, Sint32 posY, bool visible_tile)
{
	auto& renderer = g_engine.getRender();
	if(m_thingType->hasFlag(ThingAttribute_Displacement))
	{
		posX -= m_thingType->m_displacement[0];
		posY -= m_thingType->m_displacement[1];
	}

	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint8 animationFrame = calculateAnimationPhase();
	Uint32 drawSprite;
	if(visible_tile)
	{
		drawSprite = m_2X2X2Sprites[animationFrame][0];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);

		drawSprite = m_2X2X2Sprites[animationFrame][1];
		if(drawSprite != 0)
			renderer->drawSprite(drawSprite, posX, posY);
	}

	drawSprite = m_2X2X2Sprites[animationFrame][2];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);

	drawSprite = m_2X2X2Sprites[animationFrame][3];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);

	posY -= 32;
	drawSprite = m_2X2X2Sprites[animationFrame][4];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X2X2Sprites[animationFrame][5];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X2X2Sprites[animationFrame][6];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);

	drawSprite = m_2X2X2Sprites[animationFrame][7];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}
