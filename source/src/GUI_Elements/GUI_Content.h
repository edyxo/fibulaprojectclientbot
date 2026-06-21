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

#ifndef __FILE_GUI_CONTENT_h_
#define __FILE_GUI_CONTENT_h_

#include "GUI_Element.h"

class GUI_Content : public GUI_Element
{
	public:
		GUI_Content(iRect boxRect, Uint32 internalID = 0);
		~GUI_Content();

		// non-copyable
		GUI_Content(const GUI_Content&) = delete;
		GUI_Content& operator=(const GUI_Content&) = delete;

		// non-moveable
		GUI_Content(GUI_Content&&) = delete;
		GUI_Content& operator=(GUI_Content&&) = delete;

		void setRect(iRect& NewRect);
		bool isInsideRect(Sint32 x, Sint32 y) {return m_tRect.isPointInside(x, y);}
		void makeVisible() {m_visible = true;}
		void makeInvisible() {m_visible = false;}

		GUI_Element* getActiveElement() {return m_actElement;}
		void setActiveElement(GUI_Element* actElement);

		void clearChilds();
		void addChild(GUI_Element* pChild);
		void removeChild(GUI_Element* pChild);
		GUI_Element* getChild(Uint32 internalID);

		void* onAction(Sint32 x, Sint32 y);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onWheel(Sint32 x, Sint32 y, bool wheelUP);
		GUI_Element* getElementAt(Sint32 x, Sint32 y);
		void render();

	protected:
		std::vector<GUI_Element*> m_childs;
		GUI_Element* m_actElement = NULL;
		bool m_visible = true;
};

#endif /* __FILE_GUI_CONTENT_h_ */
