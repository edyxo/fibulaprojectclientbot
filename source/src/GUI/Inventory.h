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

#ifndef __FILE_GUI_INVENTORY_h_
#define __FILE_GUI_INVENTORY_h_

#include "../GUI_Elements/GUI_Element.h"

class GUI_Icons : public GUI_Element
{
	public:
		GUI_Icons(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_Icons(const GUI_Icons&) = delete;
		GUI_Icons& operator=(const GUI_Icons&) = delete;

		// non-moveable
		GUI_Icons(GUI_Icons&&) = delete;
		GUI_Icons& operator=(GUI_Icons&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);

		void render();
};

class GUI_InventoryItem : public GUI_Element
{
	public:
		GUI_InventoryItem(iRect boxRect, Sint32 skinX, Sint32 skinY, Uint8 slot, Uint32 internalID = 0);

		// non-copyable
		GUI_InventoryItem(const GUI_InventoryItem&) = delete;
		GUI_InventoryItem& operator=(const GUI_InventoryItem&) = delete;

		// non-moveable
		GUI_InventoryItem(GUI_InventoryItem&&) = delete;
		GUI_InventoryItem& operator=(GUI_InventoryItem&&) = delete;

		void* onAction(Sint32 x, Sint32 y);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);

		void render();

	protected:
		Sint32 m_skinX;
		Sint32 m_skinY;
		Uint8 m_slot;
		bool m_selected = false;
};

#endif /* __FILE_GUI_INVENTORY_h_ */
