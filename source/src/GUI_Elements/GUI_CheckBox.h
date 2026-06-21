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

#ifndef __FILE_GUI_CHECKBOX_h_
#define __FILE_GUI_CHECKBOX_h_

#include "GUI_Element.h"

class GUI_CheckBox : public GUI_Element
{
	public:
		GUI_CheckBox(iRect boxRect, const std::string labelName, bool checked = false, Uint32 internalID = 0, Uint8 red = 180, Uint8 green = 180, Uint8 blue = 180);

		// non-copyable
		GUI_CheckBox(const GUI_CheckBox&) = delete;
		GUI_CheckBox& operator=(const GUI_CheckBox&) = delete;

		// non-moveable
		GUI_CheckBox(GUI_CheckBox&&) = delete;
		GUI_CheckBox& operator=(GUI_CheckBox&&) = delete;

		void setBoxEventCallback(void (*eventHandlerFunction)(Uint32,Sint32), Uint32 mEvent);
		void render();

		void onLMouseUp(Sint32 x, Sint32 y);

		void setName(const std::string labelName);
		void setColor(Uint8 red, Uint8 green, Uint8 blue) {m_red = red; m_green = green; m_blue = blue;}
		void setChecked(bool check);

		SDL_INLINE bool isChecked() {return m_Checked;}

	protected:
		void (*m_eventHandlerFunction)(Uint32,Sint32) = NULL;
		std::string m_Label;
		Uint32 m_evtParam = 0;
		Uint8 m_red;
		Uint8 m_green;
		Uint8 m_blue;
		bool m_Checked;
};

#endif /* __FILE_GUI_CHECKBOX_h_ */
