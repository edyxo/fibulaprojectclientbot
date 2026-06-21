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

#ifndef __FILE_GUI_SEPARATOR_h_
#define __FILE_GUI_SEPARATOR_h_

#include "GUI_Element.h"

class GUI_Separator : public GUI_Element
{
	public:
		GUI_Separator(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_Separator(const GUI_Separator&) = delete;
		GUI_Separator& operator=(const GUI_Separator&) = delete;

		// non-moveable
		GUI_Separator(GUI_Separator&&) = delete;
		GUI_Separator& operator=(GUI_Separator&&) = delete;

		void render();
};

#endif /* __FILE_GUI_SEPARATOR_h_ */
