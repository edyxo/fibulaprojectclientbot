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

#include "light.h"
#include "engine.h"

LightSystem g_light;

extern Engine g_engine;

LightSystem::LightSystem()
{
	m_light[0] = 0;
	m_light[1] = 0;

	m_lightRed = 255;
	m_lightGreen = 255;
	m_lightBlue = 255;
	m_lightAmbient = 0;
}

void LightSystem::setGlobalLight(Uint16 intensity, Uint16 color, Uint8 z)
{
	m_light[0] = intensity;
	m_light[1] = color;
	changeFloor(z);
}

void LightSystem::changeFloor(Uint8 z)
{
	static const float inv255f = (1.0f / 255.0f);
	Sint32 red, green, blue, staticRed, staticGreen, staticBlue;
	if(z <= GAME_PLAYER_FLOOR)
	{
		colorFrom8bit(SDL_static_cast(Uint8, m_light[1]), m_lightRed, m_lightGreen, m_lightBlue);
		float ambientBrightness = (m_light[0] * inv255f);
		red = SDL_static_cast(Sint32, m_lightRed * ambientBrightness);
		green = SDL_static_cast(Sint32, m_lightGreen * ambientBrightness);
		blue = SDL_static_cast(Sint32, m_lightBlue * ambientBrightness);
		staticRed = 200;
		staticGreen = 200;
		staticBlue = 255;
	}
	else
	{
		red = 0;
		green = 0;
		blue = 0;
		staticRed = 255;
		staticGreen = 255;
		staticBlue = 255;
	}

	float ambientLight = g_engine.getAmbientLight() * (1.0f - ((((red + green + blue) * 342) >> 10) * inv255f)) * inv255f;
	red += SDL_static_cast(Sint32, staticRed * ambientLight);
	green += SDL_static_cast(Sint32, staticGreen * ambientLight);
	blue += SDL_static_cast(Sint32, staticBlue * ambientLight);

	m_lightRed = SDL_static_cast(Uint8, UTIL_min<Sint32>(red, 255));
	m_lightGreen = SDL_static_cast(Uint8, UTIL_min<Sint32>(green, 255));
	m_lightBlue = SDL_static_cast(Uint8, UTIL_min<Sint32>(blue, 255));
	m_lightAmbient = g_engine.getAmbientLight();
}

void LightSystem::addLightSource(Sint32 x, Sint32 y, Uint16 light[2])
{
	Sint32 rad = SDL_static_cast(Sint32, light[0]);
	float radius = SDL_static_cast(float, rad);
	Uint8 r, g, b;
	colorFrom8bit(SDL_static_cast(Uint8, light[1]), r, g, b);

	//Adjust with local creature offset
	x -= m_offsetX;
	y -= m_offsetY;

	Sint32 offsetX = (x % 32);
	Sint32 offsetY = (y % 32);
	x = (x / 32) + 1;
	y = (y / 32) + 1;

	float Xinfluence = offsetX * 0.03125f;
	float Yinfluence = offsetY * 0.03125f;

	Sint32 x_start = UTIL_max<Sint32>(x - rad, 0);
	Sint32 x_end = UTIL_min<Sint32>(x + rad + (offsetX > 0 ? 1 : 0), GAME_MAP_WIDTH);
	Sint32 y_start = UTIL_max<Sint32>(y - rad, 0);
	Sint32 y_end = UTIL_min<Sint32>(y + rad + (offsetY > 0 ? 1 : 0), GAME_MAP_HEIGHT);
	Sint32 j = y_start;
	do
	{
		float y_res = (j - y) - Yinfluence;
		y_res *= y_res;

		Sint32 index = ((j * GAME_MAP_WIDTH) + x_start);
		Sint32 i = x_start;
		do
		{
			float x_res = (i - x) - Xinfluence;
			#ifdef __USE_SSE__
			float distance = (radius - _mm_cvtss_f32(_mm_rcp_ss(_mm_rsqrt_ss(_mm_set_ss(SDL_static_cast(float, (x_res * x_res) + y_res)))))) * 0.2f;
			#else
			float distance = (radius - SDL_sqrtf(SDL_static_cast(float, (x_res * x_res) + y_res))) * 0.2f;
			#endif
			if(distance > 0.0f)
			{
				distance = UTIL_min<float>(distance, 1.0f);
				m_lightMap[index].r = UTIL_max<Uint8>(m_lightMap[index].r, SDL_static_cast(Uint8, r * distance));
				m_lightMap[index].g = UTIL_max<Uint8>(m_lightMap[index].g, SDL_static_cast(Uint8, g * distance));
				m_lightMap[index].b = UTIL_max<Uint8>(m_lightMap[index].b, SDL_static_cast(Uint8, b * distance));
			}
			++index;
		} while(++i < x_end);
	} while(++j < y_end);
}

void LightSystem::setLightSource(Sint32 x, Sint32 y, float brightness)
{
	Sint32 index = ((y * GAME_MAP_WIDTH) + x);
	m_lightMap[index].r = SDL_static_cast(Uint8, m_lightMap[index].r * brightness);
	m_lightMap[index].g = SDL_static_cast(Uint8, m_lightMap[index].g * brightness);
	m_lightMap[index].b = SDL_static_cast(Uint8, m_lightMap[index].b * brightness);
}

void LightSystem::resetLightSource(Sint32 x, Sint32 y)
{
	Sint32 index = ((y * GAME_MAP_WIDTH) + x);
	m_lightMap[index].r = m_lightRed;
	m_lightMap[index].g = m_lightGreen;
	m_lightMap[index].b = m_lightBlue;
}

void LightSystem::initLightMap(Sint32 offsetX, Sint32 offsetY, Uint8 floorZ)
{
	if(g_engine.getAmbientLight() != m_lightAmbient)
		changeFloor(floorZ);

	for(Sint32 i = 0; i < (GAME_MAP_WIDTH * GAME_MAP_HEIGHT); ++i)
	{
		m_lightMap[i].r = m_lightRed;
		m_lightMap[i].g = m_lightGreen;
		m_lightMap[i].b = m_lightBlue;
	}
	m_offsetX = offsetX;
	m_offsetY = offsetY;
}

Uint8 LightSystem::getLightSourceAverage(Sint32 x, Sint32 y)
{
	Sint32 index = ((y * GAME_MAP_WIDTH) + x);
	return SDL_static_cast(Uint8, ((SDL_static_cast(Sint32, m_lightMap[index].r) + SDL_static_cast(Sint32, m_lightMap[index].g) + SDL_static_cast(Sint32, m_lightMap[index].b)) * 342) >> 10);
}
