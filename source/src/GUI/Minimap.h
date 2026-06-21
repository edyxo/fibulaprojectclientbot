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

#ifndef __FILE_GUI_MINIMAP_h_
#define __FILE_GUI_MINIMAP_h_

#include "../GUI_Elements/GUI_Element.h"

class GUI_MinimapTime : public GUI_Element
{
	public:
		GUI_MinimapTime(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_MinimapTime(const GUI_MinimapTime&) = delete;
		GUI_MinimapTime& operator=(const GUI_MinimapTime&) = delete;

		// non-moveable
		GUI_MinimapTime(GUI_MinimapTime&&) = delete;
		GUI_MinimapTime& operator=(GUI_MinimapTime&&) = delete;

		void render();
};

class GUI_MinimapFlag : public GUI_Element
{
	public:
		GUI_MinimapFlag(iRect boxRect, Uint8 type, Uint32 internalID = 0);

		// non-copyable
		GUI_MinimapFlag(const GUI_MinimapFlag&) = delete;
		GUI_MinimapFlag& operator=(const GUI_MinimapFlag&) = delete;

		// non-moveable
		GUI_MinimapFlag(GUI_MinimapFlag&&) = delete;
		GUI_MinimapFlag& operator=(GUI_MinimapFlag&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);

		void render();

	protected:
		Uint8 m_pressed = 0;
		Uint8 m_type;
};

class GUI_Minimap : public GUI_Element
{
	public:
		GUI_Minimap(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_Minimap(const GUI_Minimap&) = delete;
		GUI_Minimap& operator=(const GUI_Minimap&) = delete;

		// non-moveable
		GUI_Minimap(GUI_Minimap&&) = delete;
		GUI_Minimap& operator=(GUI_Minimap&&) = delete;

		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);
		void onWheel(Sint32 x, Sint32 y, bool wheelUP);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);

		void render();

	protected:
		iRect m_mouseEvent;
		bool m_haveRMouse = false;
		bool m_bMouseDragging = false;
		bool m_bMouseAutowalk = false;
};

#endif /* __FILE_GUI_MINIMAP_h_ */
