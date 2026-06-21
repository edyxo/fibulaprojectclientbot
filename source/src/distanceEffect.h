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

#ifndef __FILE_DISTANCEEFFECT_h_
#define __FILE_DISTANCEEFFECT_h_

#include "position.h"

class ThingType;
class DistanceEffect
{
	public:
		static Uint32 distanceEffectCount;

		DistanceEffect(const Position& from, const Position& to, Uint16 delay, ThingType* type);
		virtual ~DistanceEffect();

		// non-copyable
		DistanceEffect(const DistanceEffect&) = delete;
		DistanceEffect& operator=(const DistanceEffect&) = delete;

		// non-moveable
		DistanceEffect(DistanceEffect&&) = delete;
		DistanceEffect& operator=(DistanceEffect&&) = delete;

		static DistanceEffect* createDistanceEffect(const Position& pos, const Position& to, Uint16 delay, Uint16 type);

		Uint16 getID();
		SDL_INLINE const Position& getFromPos() {return m_fromPosition;}
		SDL_INLINE const Position& getToPos() {return m_toPosition;}
		float getFlightProgress();
		bool isDelayed();

		virtual void render(Sint32 posX, Sint32 posY);

	protected:
		ThingType* m_thingType;
		Position m_fromPosition;
		Position m_toPosition;

		float m_flightTime;
		Uint32 m_startTime;

		Uint8 m_xPattern;
		Uint8 m_yPattern;
};

class DistanceEffectNULL : public DistanceEffect
{
	public:
		DistanceEffectNULL(const Position& pos, const Position& to, Uint16 delay, ThingType* type) : DistanceEffect(pos, to, delay, type) {}

		// non-copyable
		DistanceEffectNULL(const DistanceEffectNULL&) = delete;
		DistanceEffectNULL& operator=(const DistanceEffectNULL&) = delete;

		// non-moveable
		DistanceEffectNULL(DistanceEffectNULL&&) = delete;
		DistanceEffectNULL& operator=(DistanceEffectNULL&&) = delete;

		virtual void render(Sint32, Sint32) {}
};

class DistanceEffect1X1 : public DistanceEffect
{
	public:
		DistanceEffect1X1(const Position& pos, const Position& to, Uint16 delay, ThingType* type) : DistanceEffect(pos, to, delay, type) {}

		// non-copyable
		DistanceEffect1X1(const DistanceEffect1X1&) = delete;
		DistanceEffect1X1& operator=(const DistanceEffect1X1&) = delete;

		// non-moveable
		DistanceEffect1X1(DistanceEffect1X1&&) = delete;
		DistanceEffect1X1& operator=(DistanceEffect1X1&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_1X1Sprites = 0;
};

class DistanceEffect2X1 : public DistanceEffect
{
	public:
		DistanceEffect2X1(const Position& pos, const Position& to, Uint16 delay, ThingType* type) : DistanceEffect(pos, to, delay, type) {}

		// non-copyable
		DistanceEffect2X1(const DistanceEffect2X1&) = delete;
		DistanceEffect2X1& operator=(const DistanceEffect2X1&) = delete;

		// non-moveable
		DistanceEffect2X1(DistanceEffect2X1&&) = delete;
		DistanceEffect2X1& operator=(DistanceEffect2X1&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_1X1Sprites = 0;
		Uint32 m_2X1Sprites = 0;
};

class DistanceEffect1X2 : public DistanceEffect
{
	public:
		DistanceEffect1X2(const Position& pos, const Position& to, Uint16 delay, ThingType* type) : DistanceEffect(pos, to, delay, type) {}

		// non-copyable
		DistanceEffect1X2(const DistanceEffect1X2&) = delete;
		DistanceEffect1X2& operator=(const DistanceEffect1X2&) = delete;

		// non-moveable
		DistanceEffect1X2(DistanceEffect1X2&&) = delete;
		DistanceEffect1X2& operator=(DistanceEffect1X2&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_1X1Sprites = 0;
		Uint32 m_1X2Sprites = 0;
};

class DistanceEffect2X2 : public DistanceEffect
{
	public:
		DistanceEffect2X2(const Position& pos, const Position& to, Uint16 delay, ThingType* type) : DistanceEffect(pos, to, delay, type) {}

		// non-copyable
		DistanceEffect2X2(const DistanceEffect2X2&) = delete;
		DistanceEffect2X2& operator=(const DistanceEffect2X2&) = delete;

		// non-moveable
		DistanceEffect2X2(DistanceEffect2X2&&) = delete;
		DistanceEffect2X2& operator=(DistanceEffect2X2&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_1X1Sprites = 0;
		Uint32 m_2X1Sprites = 0;
		Uint32 m_1X2Sprites = 0;
		Uint32 m_2X2Sprites = 0;
};

#endif /* __FILE_DISTANCEEFFECT_h_ */
