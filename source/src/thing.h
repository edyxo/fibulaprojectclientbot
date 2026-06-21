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

#ifndef __FILE_THING_h_
#define __FILE_THING_h_

#include "position.h"

class Item;
class Creature;
class Thing
{
	public:
		Thing() = default;
		virtual ~Thing() {}

		// non-copyable
		Thing(const Thing&) = delete;
		Thing& operator=(const Thing&) = delete;

		// non-moveable
		Thing(Thing&&) = delete;
		Thing& operator=(Thing&&) = delete;

		void setCurrentPosition(Position& newPosition) {m_position = newPosition;}
		Position& getCurrentPosition() {return m_position;}

		virtual Item* getItem() {return NULL;}
		virtual const Item* getItem() const {return NULL;}

		virtual Creature* getCreature() {return NULL;}
		virtual const Creature* getCreature() const {return NULL;}

		virtual bool isItem() {return false;}
		virtual bool isCreature() {return false;}

	protected:
		Position m_position;
};

#endif /* __FILE_THING_h_ */
