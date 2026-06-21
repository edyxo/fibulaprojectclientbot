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

#ifndef __FILE_GUI_ITEMMOVE_h_
#define __FILE_GUI_ITEMMOVE_h_

#include "../GUI_Elements/GUI_Element.h"
#include "itemUI.h"

class GUI_ItemMove : public GUI_Element
{
	public:
		GUI_ItemMove(iRect boxRect, Uint16 itemId, Uint16 itemCount, Uint32 internalID = 0);
		~GUI_ItemMove();

		// non-copyable
		GUI_ItemMove(const GUI_ItemMove&) = delete;
		GUI_ItemMove& operator=(const GUI_ItemMove&) = delete;

		// non-moveable
		GUI_ItemMove(GUI_ItemMove&&) = delete;
		GUI_ItemMove& operator=(GUI_ItemMove&&) = delete;

		void onKeyDown(SDL_Event& event);
		void setItemCount(Uint16 itemCount);
		void render();

	protected:
		ItemUI* m_item;
		Uint32 m_lastKey;
		Uint32 m_calculate = 0;
};

#endif /* __FILE_GUI_ITEMMOVE_h_ */
