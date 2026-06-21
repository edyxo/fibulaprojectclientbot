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

#ifndef __FILE_GUI_DESCRIPTION_h_
#define __FILE_GUI_DESCRIPTION_h_

#include "../defines.h"

class GUI_Description
{
	public:
		GUI_Description() = default;

		// non-copyable
		GUI_Description(const GUI_Description&) = delete;
		GUI_Description& operator=(const GUI_Description&) = delete;

		// non-moveable
		GUI_Description(GUI_Description&&) = delete;
		GUI_Description& operator=(GUI_Description&&) = delete;

		void setDisplay(Sint32 mouseX, Sint32 mouseY, const std::string description, Uint32 delay = 500);
		void render();

	protected:
		std::string m_description;
		iRect m_tRect;
		Uint32 m_startDisplay = 0;
};

#endif /* __FILE_GUI_DESCRIPTION_h_ */
