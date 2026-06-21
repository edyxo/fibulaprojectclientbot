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

#ifndef __FILE_GUI_LABEL_h_
#define __FILE_GUI_LABEL_h_

#include "GUI_Element.h"

class GUI_Label : public GUI_Element
{
	public:
		GUI_Label(iRect boxRect, const std::string labelName, Uint32 internalID = 0, Uint8 red = 180, Uint8 green = 180, Uint8 blue = 180);

		// non-copyable
		GUI_Label(const GUI_Label&) = delete;
		GUI_Label& operator=(const GUI_Label&) = delete;

		// non-moveable
		GUI_Label(GUI_Label&&) = delete;
		GUI_Label& operator=(GUI_Label&&) = delete;

		std::string& getName() {return m_Label;}

		void setName(const std::string labelName);
		void setColor(Uint8 red, Uint8 green, Uint8 blue) {m_red = red; m_green = green; m_blue = blue;}
		void setAlign(Uint8 align) {m_align = align;}
		void setFont(Uint8 font) {m_font = font;}
		void render();

	protected:
		std::string m_Label;
		Sint32 m_align = CLIENT_FONT_ALIGN_LEFT;
		Uint8 m_font = CLIENT_FONT_NONOUTLINED;
		Uint8 m_red;
		Uint8 m_green;
		Uint8 m_blue;
};

class GUI_DynamicLabel : public GUI_Element
{
	public:
		GUI_DynamicLabel(iRect boxRect, const std::string labelName, Uint32 internalID = 0, Uint8 red = 180, Uint8 green = 180, Uint8 blue = 180);

		// non-copyable
		GUI_DynamicLabel(const GUI_DynamicLabel&) = delete;
		GUI_DynamicLabel& operator=(const GUI_DynamicLabel&) = delete;

		// non-moveable
		GUI_DynamicLabel(GUI_DynamicLabel&&) = delete;
		GUI_DynamicLabel& operator=(GUI_DynamicLabel&&) = delete;

		std::string& getName() {return m_Label;}
		bool isFullDisplay() {return m_fullDisplay;}

		void setRect(iRect& NewRect);
		void setName(const std::string labelName);
		void setColor(Uint8 red, Uint8 green, Uint8 blue) {m_red = red; m_green = green; m_blue = blue;}
		void setAlign(Uint8 align) {m_align = align;}
		void setFont(Uint8 font);

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);

		void render();

	protected:
		std::string m_displayLabel;
		std::string m_Label;
		Sint32 m_align = CLIENT_FONT_ALIGN_LEFT;
		Uint8 m_font = CLIENT_FONT_NONOUTLINED;
		Uint8 m_red;
		Uint8 m_green;
		Uint8 m_blue;
		bool m_fullDisplay = true;
};

#endif /* __FILE_GUI_LABEL_h_ */
