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

#ifndef __FILE_ITEM_UI_h_
#define __FILE_ITEM_UI_h_

#include "../position.h"
#include "../animator.h"

class ThingType;
class ItemUI
{
	public:
		ItemUI(ThingType* type) : m_thingType(type), m_position(0xFFFF, 0, 0) {}

		// non-copyable
		ItemUI(const ItemUI&) = delete;
		ItemUI& operator=(const ItemUI&) = delete;

		// non-moveable
		ItemUI(ItemUI&&) = delete;
		ItemUI& operator=(ItemUI&&) = delete;

		static ItemUI* createItemUI(Uint16 itemId, Uint16 count, Sint32 phase = AnimationPhase_Automatic);
		static ItemUI* createItemUI(ThingType* type, Uint16 count, Sint32 phase = AnimationPhase_Automatic);
		void setSubtype(Uint16 count, bool showCount);

		SDL_INLINE Uint16 getItemCount() {return m_count;}
		SDL_INLINE Uint8 getItemSubtype() {return m_subtype;}

		Uint16 getID();
		void render(Sint32 posX, Sint32 posY, Sint32 scaled);

		void setCurrentPosition(Position& newPosition) {m_position = newPosition;}
		Position& getCurrentPosition() {return m_position;}

		SDL_INLINE ThingType* getThingType() {return m_thingType;}

	protected:
		ThingType* m_thingType;
		Animator* m_animator = NULL;
		Animation m_animation;

		Position m_position;
		std::string m_stringCount;

		Uint16 m_count = 1;
		Uint8 m_subtype = 0;
		Uint8 m_animCount = 0;
		Uint8 m_xPattern = 0;
		Uint8 m_yPattern = 0;
		Uint8 m_zPattern = 0;
		bool m_displayCount = false;
};

#endif /* __FILE_ITEM_h_ */
