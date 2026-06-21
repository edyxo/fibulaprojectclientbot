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

#ifndef __FILE_GUI_ICON_h_
#define __FILE_GUI_ICON_h_

#include "GUI_Element.h"

class GUI_Icon : public GUI_Element
{
	public:
		GUI_Icon(iRect boxRect, Uint16 picture, Sint32 pictureX, Sint32 pictureY, Sint32 cPictureX, Sint32 cPictureY, Uint32 internalID = 0, const std::string description = "");

		// non-copyable
		GUI_Icon(const GUI_Icon&) = delete;
		GUI_Icon& operator=(const GUI_Icon&) = delete;

		// non-moveable
		GUI_Icon(GUI_Icon&&) = delete;
		GUI_Icon& operator=(GUI_Icon&&) = delete;

		void setButtonEventCallback(void (*eventHandlerFunction)(Uint32,Sint32), Uint32 mEvent);
		void setData(Uint16 picture, Sint32 pictureX, Sint32 pictureY, Sint32 cPictureX, Sint32 cPictureY);

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);

		void render();

	protected:
		void (*m_eventHandlerFunction)(Uint32,Sint32) = NULL;
		std::string m_description;
		Uint32 m_evtParam = 0;
		Sint32 m_sx[2];
		Sint32 m_sy[2];
		Uint16 m_picture;
		Uint8 m_pressed = 0;
};

class GUI_RadioIcon : public GUI_Icon
{
	public:
		GUI_RadioIcon(iRect boxRect, Uint16 picture, Sint32 pictureX, Sint32 pictureY, Sint32 cPictureX, Sint32 cPictureY, Uint32 internalID = 0, const std::string description = "") :
			GUI_Icon(boxRect, picture, pictureX, pictureY, cPictureX, cPictureY, internalID, description) {}

		// non-copyable
		GUI_RadioIcon(const GUI_RadioIcon&) = delete;
		GUI_RadioIcon& operator=(const GUI_RadioIcon&) = delete;

		// non-moveable
		GUI_RadioIcon(GUI_RadioIcon&&) = delete;
		GUI_RadioIcon& operator=(GUI_RadioIcon&&) = delete;

		void setRadioEventCallback(bool (*eventRadioChecked)(void), const std::string description = "");

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		
		void render();

	protected:
		bool (*m_eventRadioChecked)(void) = NULL;
		std::string m_radioDescription;
};

#endif /* __FILE_GUI_ICON_h_ */
