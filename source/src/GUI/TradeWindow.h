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

#ifndef __FILE_GUI_TRADEWINDOW_h_
#define __FILE_GUI_TRADEWINDOW_h_

#include "../GUI_Elements/GUI_Element.h"
#include "../GUI_Elements/GUI_Container.h"

class GUI_TradeContainer : public GUI_Container
{
	public:
		GUI_TradeContainer(iRect boxRect, GUI_PanelWindow* parent, Uint32 internalID = 0) : GUI_Container(boxRect, parent, internalID) {}

		// non-copyable
		GUI_TradeContainer(const GUI_TradeContainer&) = delete;
		GUI_TradeContainer& operator=(const GUI_TradeContainer&) = delete;

		// non-moveable
		GUI_TradeContainer(GUI_TradeContainer&&) = delete;
		GUI_TradeContainer& operator=(GUI_TradeContainer&&) = delete;

		void render();
};

class ItemUI;
class GUI_TradeItem : public GUI_Element
{
	public:
		GUI_TradeItem(iRect boxRect, ItemUI* item, Uint8 index, bool counterOffer, Uint32 internalID = 0);
		~GUI_TradeItem();

		// non-copyable
		GUI_TradeItem(const GUI_TradeItem&) = delete;
		GUI_TradeItem& operator=(const GUI_TradeItem&) = delete;

		// non-moveable
		GUI_TradeItem(GUI_TradeItem&&) = delete;
		GUI_TradeItem& operator=(GUI_TradeItem&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);

		void render();

	protected:
		ItemUI* m_item;
		Uint8 m_index;
		Uint8 m_haveMouse = 0;
		bool m_counterOffer;
};

#endif /* __FILE_GUI_TRADEWINDOW_h_ */
