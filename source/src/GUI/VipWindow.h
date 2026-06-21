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

#ifndef __FILE_GUI_VIPWINDOW_h_
#define __FILE_GUI_VIPWINDOW_h_

#include "../GUI_Elements/GUI_Container.h"
#include "../GUI_Elements/GUI_Label.h"

class GUI_VipChecker : public GUI_Element
{
	public:
		GUI_VipChecker(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_VipChecker(const GUI_VipChecker&) = delete;
		GUI_VipChecker& operator=(const GUI_VipChecker&) = delete;

		// non-moveable
		GUI_VipChecker(GUI_VipChecker&&) = delete;
		GUI_VipChecker& operator=(GUI_VipChecker&&) = delete;

		void render();
};

class GUI_VipContainer : public GUI_Container
{
	public:
		GUI_VipContainer(iRect boxRect, GUI_PanelWindow* parent, Uint32 internalID = 0) : GUI_Container(boxRect, parent, internalID) {}

		// non-copyable
		GUI_VipContainer(const GUI_VipContainer&) = delete;
		GUI_VipContainer& operator=(const GUI_VipContainer&) = delete;

		// non-moveable
		GUI_VipContainer(GUI_VipContainer&&) = delete;
		GUI_VipContainer& operator=(GUI_VipContainer&&) = delete;

		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);
};

class GUI_VipPlayer : public GUI_Element
{
	public:
		GUI_VipPlayer(iRect boxRect, const std::string& name, Uint32 _GUID, Uint32 iconId, Uint8 status, Uint32 internalID = 0);

		// non-copyable
		GUI_VipPlayer(const GUI_VipPlayer&) = delete;
		GUI_VipPlayer& operator=(const GUI_VipPlayer&) = delete;

		// non-moveable
		GUI_VipPlayer(GUI_VipPlayer&&) = delete;
		GUI_VipPlayer& operator=(GUI_VipPlayer&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);

		void setRect(iRect& NewRect);
		void render();

	protected:
		GUI_DynamicLabel m_name;
		Uint32 m_GUID;
		Uint32 m_iconId;
		Uint32 m_lastClick = 0;
		bool m_hover = false;
};

class GUI_VipGroup : public GUI_Element
{
	public:
		GUI_VipGroup(iRect boxRect, const std::string& name, Uint8 groupId, Uint32 internalID = 0);

		// non-copyable
		GUI_VipGroup(const GUI_VipGroup&) = delete;
		GUI_VipGroup& operator=(const GUI_VipGroup&) = delete;

		// non-moveable
		GUI_VipGroup(GUI_VipGroup&&) = delete;
		GUI_VipGroup& operator=(GUI_VipGroup&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);

		void setRect(iRect& NewRect);
		void render();

	protected:
		GUI_DynamicLabel m_name;
		Uint8 m_groupId;
		bool m_hover = false;
};

class GUI_VipIcon : public GUI_Element
{
	public:
		GUI_VipIcon(iRect boxRect, Uint32 iconId, Uint32 internalID = 0);

		// non-copyable
		GUI_VipIcon(const GUI_VipIcon&) = delete;
		GUI_VipIcon& operator=(const GUI_VipIcon&) = delete;

		// non-moveable
		GUI_VipIcon(GUI_VipIcon&&) = delete;
		GUI_VipIcon& operator=(GUI_VipIcon&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);

		static void renderIcon(Uint32 iconId, Sint32 x, Sint32 y);
		void render();

	protected:
		Uint32 m_iconId;
		Uint8 m_pressed = 0;
};

#endif /* __FILE_GUI_VIPWINDOW_h_ */
