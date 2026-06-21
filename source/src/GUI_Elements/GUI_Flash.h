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

#ifndef __FILE_GUI_FLASH_h_
#define __FILE_GUI_FLASH_h_

#include "GUI_Element.h"

class GUI_Flash : public GUI_Element
{
	public:
		GUI_Flash(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_Flash(const GUI_Flash&) = delete;
		GUI_Flash& operator=(const GUI_Flash&) = delete;

		// non-moveable
		GUI_Flash(GUI_Flash&&) = delete;
		GUI_Flash& operator=(GUI_Flash&&) = delete;

		void reset();
		void setEndEventCallback(void (*eventHandlerFunction)(Uint32, Sint32), Uint32 mEvent);

		void render();

	protected:
		void (*m_eventHandlerFunction)(Uint32, Sint32) = NULL;
		Uint32 m_startFlash;
		Uint32 m_lastControlFlash;
		Uint32 m_evtParam = 0;
		bool m_controlFlash = true;
};

#endif /* __FILE_GUI_FLASH_h_ */
