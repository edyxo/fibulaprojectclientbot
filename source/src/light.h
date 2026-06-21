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

#ifndef __FILE_LIGHT_h_
#define __FILE_LIGHT_h_

#include "defines.h"

class LightSystem
{
	public:
		LightSystem();

		// non-copyable
		LightSystem(const LightSystem&) = delete;
		LightSystem& operator=(const LightSystem&) = delete;

		// non-moveable
		LightSystem(LightSystem&&) = delete;
		LightSystem& operator=(LightSystem&&) = delete;

		void setGlobalLight(Uint16 intensity, Uint16 color, Uint8 z);
		void changeFloor(Uint8 z);

		void addLightSource(Sint32 x, Sint32 y, Uint16 light[2]);
		void setLightSource(Sint32 x, Sint32 y, float brightness);
		void resetLightSource(Sint32 x, Sint32 y);
		void initLightMap(Sint32 offsetX, Sint32 offsetY, Uint8 floorZ);
		Uint8 getLightSourceAverage(Sint32 x, Sint32 y);

		SDL_INLINE Uint16 getLightIntensity() {return m_light[0];}
		SDL_INLINE Uint16 getLightColor() {return m_light[1];}
		SDL_INLINE LightMap* getLightMap() {return m_lightMap;}

	protected:
		LightMap m_lightMap[GAME_MAP_WIDTH * GAME_MAP_HEIGHT];

		Sint32 m_offsetX;
		Sint32 m_offsetY;
		Uint16 m_light[2];

		Uint8 m_lightAmbient;
		Uint8 m_lightRed;
		Uint8 m_lightGreen;
		Uint8 m_lightBlue;
};

#endif /* __FILE_LIGHT_h_ */
