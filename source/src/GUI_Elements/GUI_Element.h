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

#ifndef __FILE_GUI_ELEMENT_h_
#define __FILE_GUI_ELEMENT_h_

#include "../defines.h"

class GUI_Element
{
	public:
		GUI_Element() = default;
		virtual ~GUI_Element() {}

		// non-copyable
		GUI_Element(const GUI_Element&) = delete;
		GUI_Element& operator=(const GUI_Element&) = delete;

		// non-moveable
		GUI_Element(GUI_Element&&) = delete;
		GUI_Element& operator=(GUI_Element&&) = delete;

		virtual void setRect(iRect& NewRect) {m_tRect = NewRect;}
		iRect& getRect() {return m_tRect;}

		Uint32 getInternalID() {return m_internalID;}

		bool isActive() {return m_bActive;}
		virtual void activate() {m_bActive = true;}
		virtual void deActivate() {m_bActive = false;}

		bool isEventable() {return m_bEvents;}
		void startEvents() {m_bEvents = true;}
		void stopEvents() {m_bEvents = false;}

		virtual void* onAction(Sint32, Sint32) {return NULL;}
		virtual void onTextInput(const char*) {};
		virtual void onKeyDown(SDL_Event&) {};
		virtual void onKeyUp(SDL_Event&) {};
		virtual void onMouseMove(Sint32, Sint32, bool) {};
		virtual void onLMouseDown(Sint32, Sint32) {};
		virtual void onLMouseUp(Sint32, Sint32) {};
		virtual void onRMouseDown(Sint32, Sint32) {};
		virtual void onRMouseUp(Sint32, Sint32) {};
		virtual void onWheel(Sint32, Sint32, bool) {};
		virtual void render() {};
		virtual GUI_Element* getElementAt(Sint32 x, Sint32 y) {return (m_tRect.isPointInside(x, y) ? this : NULL);}

	protected:
		Uint32 m_internalID;
		iRect m_tRect;
		bool m_bActive = false;
		bool m_bEvents = false;
};

#endif /* __FILE_GUI_ELEMENT_h_ */
