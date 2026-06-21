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

#ifndef __FILE_GUI_STATICIMAGE_h_
#define __FILE_GUI_STATICIMAGE_h_

#include "GUI_Element.h"

class GUI_StaticImage : public GUI_Element
{
	public:
		GUI_StaticImage(iRect boxRect, Uint16 pictureId, Sint32 pictureX, Sint32 pictureY, Uint32 internalID = 0, const std::string description = "");

		// non-copyable
		GUI_StaticImage(const GUI_StaticImage&) = delete;
		GUI_StaticImage& operator=(const GUI_StaticImage&) = delete;

		// non-moveable
		GUI_StaticImage(GUI_StaticImage&&) = delete;
		GUI_StaticImage& operator=(GUI_StaticImage&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);

		void render();

	protected:
		std::string m_description;
		Sint32 m_pictureX;
		Sint32 m_pictureY;
		Uint16 m_pictureId;
};

#endif /* __FILE_GUI_STATICIMAGE_h_ */
