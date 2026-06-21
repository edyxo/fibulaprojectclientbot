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

#include "effect.h"
#include "engine.h"
#include "thingManager.h"
#include "light.h"

extern Engine g_engine;
extern ThingManager g_thingManager;
extern LightSystem g_light;
extern Uint32 g_frameTime;

std::vector<Effect*> Effect::effects;
Uint32 Effect::effectCount = 0;

Effect::Effect(const Position& pos, Uint16 delay, ThingType* type) : m_thingType(type), m_position(pos)
{
	m_startTime = g_frameTime + delay;
	++effectCount;
	effects.push_back(this);
}

Effect::~Effect()
{
	auto it = std::find(effects.begin(), effects.end(), this);
	if(it != effects.end())
	{
		*it = effects.back();
		effects.pop_back();
	}
	--effectCount;
}

Effect* Effect::createEffect(const Position& pos, Uint16 delay, Uint16 type)
{
	if(effectCount >= EFFECT_MAX_INGAME_EFFECTS)
		return NULL;

	Effect* newEffect = NULL;
	ThingType* ttype = g_thingManager.getThingType(ThingCategory_Effect, type);
	if(ttype)
	{
		Uint8 animCount = ttype->m_frameGroup[ThingFrameGroup_Default].m_animCount;
		Uint8 xPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, pos.x), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternX);
		Uint8 yPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, pos.y), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternY);
		Uint8 zPattern = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, pos.z), ttype->m_frameGroup[ThingFrameGroup_Default].m_patternZ);
		if(animCount <= EFFECT_MAX_CACHED_ANIMATIONS)
		{
			Uint8 width = ttype->m_frameGroup[ThingFrameGroup_Default].m_width;
			Uint8 height = ttype->m_frameGroup[ThingFrameGroup_Default].m_height;
			if(width == 1 && height == 1)
			{
				auto newEffectX = new Effect1X1(pos, delay, ttype);
				for(Uint8 a = 0; a < animCount; ++a)
					newEffectX->m_1X1Sprites[a] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);

				newEffect = newEffectX;
			}
			else if(width == 2 && height == 1)
			{
				auto newEffectX = new Effect2X1(pos, delay, ttype);
				for(Uint8 a = 0; a < animCount; ++a)
				{
					newEffectX->m_2X1Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
					newEffectX->m_2X1Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, xPattern, yPattern, zPattern, a);
				}
				newEffect = newEffectX;
			}
			else if(width == 1 && height == 2)
			{
				auto newEffectX = new Effect1X2(pos, delay, ttype);
				for(Uint8 a = 0; a < animCount; ++a)
				{
					newEffectX->m_1X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
					newEffectX->m_1X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, xPattern, yPattern, zPattern, a);
				}
				newEffect = newEffectX;
			}
			else if(width == 2 && height == 2)
			{
				auto newEffectX = new Effect2X2(pos, delay, ttype);
				for(Uint8 a = 0; a < animCount; ++a)
				{
					newEffectX->m_2X2Sprites[a][0] = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, xPattern, yPattern, zPattern, a);
					newEffectX->m_2X2Sprites[a][1] = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, xPattern, yPattern, zPattern, a);
					newEffectX->m_2X2Sprites[a][2] = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, xPattern, yPattern, zPattern, a);
					newEffectX->m_2X2Sprites[a][3] = ttype->getSprite(ThingFrameGroup_Default, 1, 1, 0, xPattern, yPattern, zPattern, a);
				}
				newEffect = newEffectX;
			}
		}

		if(!newEffect)
			newEffect = new Effect(pos, delay, ttype);

		newEffect->m_animator = ttype->m_frameGroup[ThingFrameGroup_Default].m_animator;
		newEffect->m_animCount = animCount;
		newEffect->m_xPattern = xPattern;
		newEffect->m_yPattern = yPattern;
		newEffect->m_zPattern = zPattern;
		newEffect->m_topEffect = ttype->hasFlag(ThingAttribute_TopEffect);
		if(newEffect->m_animator)
			newEffect->m_animator->resetAnimation(newEffect->m_animation);
	}
	else
		newEffect = new EffectNULL(pos, delay, ttype);

	return newEffect;
}

Uint16 Effect::getID()
{
	if(m_thingType)
		return m_thingType->m_id;

	return 0;
}

bool Effect::canBeDeleted()
{
	if(m_currentAnim >= m_animCount)
		return true;
	else
		return false;
}

bool Effect::isDelayed()
{
	return (g_frameTime < m_startTime);
}

void Effect::update()
{
	for(std::vector<Effect*>::iterator it = effects.begin(), end = effects.end(); it != end; ++it)
		(*it)->m_currentAnim = (*it)->calculateAnimationPhase();
}

void Effect::render(Sint32 posX, Sint32 posY)
{
	auto& renderer = g_engine.getRender();
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	for(Uint8 y = 0; y < m_thingType->m_frameGroup[ThingFrameGroup_Default].m_height; ++y)
	{
		Sint32 posXc = posX;
		for(Uint8 x = 0; x < m_thingType->m_frameGroup[ThingFrameGroup_Default].m_width; ++x)
		{
			Uint32 sprite = m_thingType->getSprite(ThingFrameGroup_Default, x, y, 0, m_xPattern, m_yPattern, m_zPattern, m_currentAnim);
			if(sprite != 0)
				renderer->drawSprite(sprite, posXc, posY);

			posXc -= 32;
		}
		posY -= 32;
	}
}

Uint8 Effect::calculateAnimationPhase()
{
	if(m_animator)
	{
		Uint8 currentAnimation = SDL_static_cast(Uint8, m_animator->getPhase(m_animation));
		return (m_animator->isComplete(m_animation) ? m_animCount : currentAnimation);
	}

	return SDL_static_cast(Uint8, (g_frameTime - m_startTime) / EFFECT_TICKS_PER_FRAME);
}

void Effect1X1::render(Sint32 posX, Sint32 posY)
{
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_1X1Sprites[m_currentAnim];
	if(drawSprite != 0)
		g_engine.getRender()->drawSprite(drawSprite, posX, posY);
}

void Effect2X1::render(Sint32 posX, Sint32 posY)
{
	auto& renderer = g_engine.getRender();
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_2X1Sprites[m_currentAnim][0];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X1Sprites[m_currentAnim][1];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}

void Effect1X2::render(Sint32 posX, Sint32 posY)
{
	auto& renderer = g_engine.getRender();
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_1X2Sprites[m_currentAnim][0];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_1X2Sprites[m_currentAnim][1];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY - 32);
}

void Effect2X2::render(Sint32 posX, Sint32 posY)
{
	auto& renderer = g_engine.getRender();
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_2X2Sprites[m_currentAnim][0];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X2Sprites[m_currentAnim][1];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);

	posY -= 32;
	drawSprite = m_2X2Sprites[m_currentAnim][2];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X2Sprites[m_currentAnim][3];
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}
