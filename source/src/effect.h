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

#ifndef __FILE_EFFECT_h_
#define __FILE_EFFECT_h_

#include "position.h"
#include "animator.h"

class ThingType;
class Effect
{
	public:
		static std::vector<Effect*> effects;
		static Uint32 effectCount;

		Effect(const Position& pos, Uint16 delay, ThingType* type);
		virtual ~Effect();

		// non-copyable
		Effect(const Effect&) = delete;
		Effect& operator=(const Effect&) = delete;

		// non-moveable
		Effect(Effect&&) = delete;
		Effect& operator=(Effect&&) = delete;

		static Effect* createEffect(const Position& pos, Uint16 delay, Uint16 type);

		Uint16 getID();
		SDL_INLINE const Position& getPos() {return m_position;}
		bool canBeDeleted();
		bool isDelayed();
		bool isTopEffect() {return m_topEffect;}
		
		SDL_INLINE void setCachedX(Sint32 x) {m_cacheX = x;}
		SDL_INLINE void setCachedY(Sint32 y) {m_cacheY = y;}
		SDL_INLINE Sint32 getCachedX() {return m_cacheX;}
		SDL_INLINE Sint32 getCachedY() {return m_cacheY;}

		static void update();
		virtual void render(Sint32 posX, Sint32 posY);

		Uint8 calculateAnimationPhase();

	protected:
		ThingType* m_thingType;
		Animator* m_animator = NULL;
		Animation m_animation;
		Position m_position;

		Sint32 m_cacheX = 0;
		Sint32 m_cacheY = 0;

		Uint32 m_startTime;
		Uint8 m_currentAnim = 0;
		Uint8 m_animCount = 0;
		Uint8 m_xPattern = 0;
		Uint8 m_yPattern = 0;
		Uint8 m_zPattern = 0;
		bool m_topEffect = false;
};

class EffectNULL : public Effect
{
	public:
		EffectNULL(const Position& pos, Uint16 delay, ThingType* type) : Effect(pos, delay, type) {}

		// non-copyable
		EffectNULL(const EffectNULL&) = delete;
		EffectNULL& operator=(const EffectNULL&) = delete;

		// non-moveable
		EffectNULL(EffectNULL&&) = delete;
		EffectNULL& operator=(EffectNULL&&) = delete;

		virtual void render(Sint32, Sint32) {}
};

class Effect1X1 : public Effect
{
	public:
		Effect1X1(const Position& pos, Uint16 delay, ThingType* type) : Effect(pos, delay, type) {}

		// non-copyable
		Effect1X1(const Effect1X1&) = delete;
		Effect1X1& operator=(const Effect1X1&) = delete;

		// non-moveable
		Effect1X1(Effect1X1&&) = delete;
		Effect1X1& operator=(Effect1X1&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_1X1Sprites[EFFECT_MAX_CACHED_ANIMATIONS] = {};
};

class Effect2X1 : public Effect
{
	public:
		Effect2X1(const Position& pos, Uint16 delay, ThingType* type) : Effect(pos, delay, type) {}

		// non-copyable
		Effect2X1(const Effect2X1&) = delete;
		Effect2X1& operator=(const Effect2X1&) = delete;

		// non-moveable
		Effect2X1(Effect2X1&&) = delete;
		Effect2X1& operator=(Effect2X1&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_2X1Sprites[EFFECT_MAX_CACHED_ANIMATIONS][2] = {};
};

class Effect1X2 : public Effect
{
	public:
		Effect1X2(const Position& pos, Uint16 delay, ThingType* type) : Effect(pos, delay, type) {}

		// non-copyable
		Effect1X2(Effect1X2&) = delete;
		Effect1X2& operator=(Effect1X2&) = delete;

		// non-moveable
		Effect1X2(const Effect1X2&&) = delete;
		Effect1X2& operator=(const Effect1X2&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_1X2Sprites[EFFECT_MAX_CACHED_ANIMATIONS][2] = {};
};

class Effect2X2 : public Effect
{
	public:
		Effect2X2(const Position& pos, Uint16 delay, ThingType* type) : Effect(pos, delay, type) {}

		// non-copyable
		Effect2X2(const Effect2X2&) = delete;
		Effect2X2& operator=(const Effect2X2&) = delete;

		// non-moveable
		Effect2X2(Effect2X2&&) = delete;
		Effect2X2& operator=(Effect2X2&&) = delete;

		virtual void render(Sint32 posX, Sint32 posY);

		Uint32 m_2X2Sprites[EFFECT_MAX_CACHED_ANIMATIONS][4] = {};
};

#endif /* __FILE_EFFECT_h_ */
