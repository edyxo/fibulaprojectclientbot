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

#ifndef __FILE_GUI_SHOPWINDOW_h_
#define __FILE_GUI_SHOPWINDOW_h_

#include "../GUI_Elements/GUI_Container.h"
#include "../GUI_Elements/GUI_Label.h"

class GUI_ShopContainer : public GUI_Container
{
	public:
		GUI_ShopContainer(iRect boxRect, GUI_PanelWindow* parent, Uint32 internalID = 0) : GUI_Container(boxRect, parent, internalID) {}

		// non-copyable
		GUI_ShopContainer(const GUI_ShopContainer&) = delete;
		GUI_ShopContainer& operator=(const GUI_ShopContainer&) = delete;

		// non-moveable
		GUI_ShopContainer(GUI_ShopContainer&&) = delete;
		GUI_ShopContainer& operator=(GUI_ShopContainer&&) = delete;

		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);

		void render();

	protected:
		bool m_rmouse = 0;
};

class ItemUI;
class GUI_ShopItem : public GUI_Element
{
	public:
		GUI_ShopItem(iRect boxRect, Uint16 itemId, Uint16 itemCount, Uint32 internalID = 0);
		~GUI_ShopItem();

		// non-copyable
		GUI_ShopItem(const GUI_ShopItem&) = delete;
		GUI_ShopItem& operator=(const GUI_ShopItem&) = delete;

		// non-moveable
		GUI_ShopItem(GUI_ShopItem&&) = delete;
		GUI_ShopItem& operator=(GUI_ShopItem&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);

		void setItemData(Uint16 itemId, Uint16 itemCount);
		void render();

	protected:
		ItemUI* m_item;
		Uint8 m_haveMouse = 0;
};

class GUI_ShopSelectItem : public GUI_Element
{
	public:
		GUI_ShopSelectItem(iRect boxRect, const std::string& itemName, const std::string& itemDescription, Uint16 itemId, Uint8 itemSubtype, size_t index, Uint32 internalID = 0);
		~GUI_ShopSelectItem();

		// non-copyable
		GUI_ShopSelectItem(const GUI_ShopSelectItem&) = delete;
		GUI_ShopSelectItem& operator=(const GUI_ShopSelectItem&) = delete;

		// non-moveable
		GUI_ShopSelectItem(GUI_ShopSelectItem&&) = delete;
		GUI_ShopSelectItem& operator=(GUI_ShopSelectItem&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);

		void setRect(iRect& NewRect);
		void render();

	protected:
		GUI_DynamicLabel m_name;
		GUI_DynamicLabel m_description;
		ItemUI* m_item;
		size_t m_index;
};

#endif /* __FILE_GUI_SHOPWINDOW_h_ */
