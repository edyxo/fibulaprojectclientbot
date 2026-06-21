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

#include "distanceEffect.h"
#include "thingManager.h"
#include "engine.h"
#include "thingManager.h"
#include "light.h"

extern Engine g_engine;
extern ThingManager g_thingManager;
extern LightSystem g_light;
extern Uint32 g_frameTime;

Uint32 DistanceEffect::distanceEffectCount = 0;

DistanceEffect::DistanceEffect(const Position& from, const Position& to, Uint16 delay, ThingType* type) : m_thingType(type), m_fromPosition(from), m_toPosition(to)
{
	m_startTime = g_frameTime + delay;

	float tan;
	Sint32 dx = Position::getOffsetX(from, to);
	Sint32 dy = Position::getOffsetY(from, to);
	if(dx != 0)
		tan = SDL_static_cast(float, dy) / SDL_static_cast(float, dx);
	else
		tan = 10.f;

	float absTan = SDL_fabsf(tan);
	if(absTan < 0.4142f)
	{
		if(dx > 0)
		{
			m_xPattern = 0;
			m_yPattern = 1;
		}
		else
		{
			m_xPattern = 2;
			m_yPattern = 1;
		}
	}
	else if(absTan < 2.4142f)
	{
		if(tan > 0.f)
		{
			if(dy > 0)
			{
				m_xPattern = 0;
				m_yPattern = 0;
			}
			else
			{
				m_xPattern = 2;
				m_yPattern = 2;
			}
		}
		else
		{
			if(dx > 0)
			{
				m_xPattern = 0;
				m_yPattern = 2;
			}
			else
			{
				m_xPattern = 2;
				m_yPattern = 0;
			}
		}
	}
	else
	{
		if(dy > 0)
		{
			m_xPattern = 1;
			m_yPattern = 0;
		}
		else
		{
			m_xPattern = 1;
			m_yPattern = 2;
		}
	}
	m_flightTime = SDL_sqrtf(SDL_static_cast(float, (dx * dx) + (dy * dy))) / 10.0f;
	++distanceEffectCount;
}

DistanceEffect::~DistanceEffect()
{
	--distanceEffectCount;
}

DistanceEffect* DistanceEffect::createDistanceEffect(const Position& pos, const Position& to, Uint16 delay, Uint16 type)
{
	if(distanceEffectCount >= DISTANCEEFFECT_MAX_INGAME_DISTANCEEFFECTS)
		return NULL;

	DistanceEffect* newDistanceEffect = NULL;
	ThingType* ttype = g_thingManager.getThingType(ThingCategory_DistanceEffect, type);
	if(ttype)
	{
		Uint8 width = ttype->m_frameGroup[ThingFrameGroup_Default].m_width;
		Uint8 height = ttype->m_frameGroup[ThingFrameGroup_Default].m_height;
		if(width == 1 && height == 1)
		{
			DistanceEffect1X1* newDistanceEffectX = new DistanceEffect1X1(pos, to, delay, ttype);
			newDistanceEffectX->m_1X1Sprites = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffect = newDistanceEffectX;
		}
		else if(width == 2 && height == 1)
		{
			DistanceEffect2X1* newDistanceEffectX = new DistanceEffect2X1(pos, to, delay, ttype);
			newDistanceEffectX->m_1X1Sprites = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffectX->m_2X1Sprites = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffect = newDistanceEffectX;
		}
		else if(width == 1 && height == 2)
		{
			DistanceEffect1X2* newDistanceEffectX = new DistanceEffect1X2(pos, to, delay, ttype);
			newDistanceEffectX->m_1X1Sprites = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffectX->m_1X2Sprites = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffect = newDistanceEffectX;
		}
		else if(width == 2 && height == 2)
		{
			DistanceEffect2X2* newDistanceEffectX = new DistanceEffect2X2(pos, to, delay, ttype);
			newDistanceEffectX->m_1X1Sprites = ttype->getSprite(ThingFrameGroup_Default, 0, 0, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffectX->m_2X1Sprites = ttype->getSprite(ThingFrameGroup_Default, 1, 0, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffectX->m_1X2Sprites = ttype->getSprite(ThingFrameGroup_Default, 0, 1, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffectX->m_2X2Sprites = ttype->getSprite(ThingFrameGroup_Default, 1, 1, 0, newDistanceEffectX->m_xPattern, newDistanceEffectX->m_yPattern, 0, 0);
			newDistanceEffect = newDistanceEffectX;
		}

		if(!newDistanceEffect)
			newDistanceEffect = new DistanceEffect(pos, to, delay, ttype);
	}
	else
		newDistanceEffect = new DistanceEffectNULL(pos, to, delay, ttype);

	return newDistanceEffect;
}

Uint16 DistanceEffect::getID()
{
	if(m_thingType)
		return m_thingType->m_id;

	return 0;
}

float DistanceEffect::getFlightProgress()
{
	return (g_frameTime - m_startTime) / (1000.f * m_flightTime);
}

bool DistanceEffect::isDelayed()
{
	return (g_frameTime < m_startTime);
}

void DistanceEffect::render(Sint32 posX, Sint32 posY)
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
			Uint32 sprite = m_thingType->getSprite(ThingFrameGroup_Default, x, y, 0, m_xPattern, m_yPattern, 0, 0);
			renderer->drawSprite(sprite, posXc, posY);
			posXc -= 32;
		}
		posY -= 32;
	}
}

void DistanceEffect1X1::render(Sint32 posX, Sint32 posY)
{
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_1X1Sprites;
	if(drawSprite != 0)
		g_engine.getRender()->drawSprite(drawSprite, posX, posY);
}

void DistanceEffect2X1::render(Sint32 posX, Sint32 posY)
{
	auto& renderer = g_engine.getRender();
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_1X1Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X1Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}

void DistanceEffect1X2::render(Sint32 posX, Sint32 posY)
{
	auto& renderer = g_engine.getRender();
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_1X1Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_1X2Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY - 32);
}

void DistanceEffect2X2::render(Sint32 posX, Sint32 posY)
{
	auto& renderer = g_engine.getRender();
	if(g_engine.getLightMode() != CLIENT_LIGHT_MODE_NONE)
	{
		Uint16* light = m_thingType->m_light;
		if(light[0] > 0)
			g_light.addLightSource(posX, posY, light);
	}

	Uint32 drawSprite = m_1X1Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X1Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);

	posY -= 32;
	drawSprite = m_1X2Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX, posY);

	drawSprite = m_2X2Sprites;
	if(drawSprite != 0)
		renderer->drawSprite(drawSprite, posX - 32, posY);
}
