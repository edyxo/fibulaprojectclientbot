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

#ifndef __FILE_GUI_CONTEXTMENU_h_
#define __FILE_GUI_CONTEXTMENU_h_

#include "../defines.h"

class GUI_ContextMenu
{
	public:
		GUI_ContextMenu() = default;

		// non-copyable
		GUI_ContextMenu(const GUI_ContextMenu&) = delete;
		GUI_ContextMenu& operator=(const GUI_ContextMenu&) = delete;

		// non-moveable
		GUI_ContextMenu(GUI_ContextMenu&&) = delete;
		GUI_ContextMenu& operator=(GUI_ContextMenu&&) = delete;
		
		bool isInsideRect(Sint32 x, Sint32 y) {return m_tRect.isPointInside(x, y);}
		void addContextMenu(Uint8 style, Uint32 eventId, const std::string text, const std::string shortcut);
		void addSeparator();
		void setEventCallback(void (*eventHandlerFunction)(Uint32, Sint32));
		void setDisplay(Sint32 mouseX, Sint32 mouseY);

		void onLMouseUp(Sint32 x, Sint32 y);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void render();

	protected:
		std::vector<ContextMenuChild> m_childs;
		void (*m_eventHandlerFunction)(Uint32, Sint32) = NULL;
		iRect m_tRect;
		Sint32 m_hoverEvent = -1;
};

#endif /* __FILE_GUI_CONTEXTMENU_h_ */
