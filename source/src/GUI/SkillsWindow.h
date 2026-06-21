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

#ifndef __FILE_GUI_SKILLSWINDOW_h_
#define __FILE_GUI_SKILLSWINDOW_h_

#include "../GUI_Elements/GUI_Element.h"
#include "../GUI_Elements/GUI_Container.h"

class GUI_SkillsContainer : public GUI_Container
{
	public:
		GUI_SkillsContainer(iRect boxRect, GUI_PanelWindow* parent, Uint32 internalID = 0) : GUI_Container(boxRect, parent, internalID) {}

		// non-copyable
		GUI_SkillsContainer(const GUI_SkillsContainer&) = delete;
		GUI_SkillsContainer& operator=(const GUI_SkillsContainer&) = delete;

		// non-moveable
		GUI_SkillsContainer(GUI_SkillsContainer&&) = delete;
		GUI_SkillsContainer& operator=(GUI_SkillsContainer&&) = delete;

		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);

	protected:
		bool m_rmouse = false;
};

class GUI_SkillSeparator : public GUI_Element
{
	public:
		GUI_SkillSeparator(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_SkillSeparator(const GUI_SkillSeparator&) = delete;
		GUI_SkillSeparator& operator=(const GUI_SkillSeparator&) = delete;

		// non-moveable
		GUI_SkillSeparator(GUI_SkillSeparator&&) = delete;
		GUI_SkillSeparator& operator=(GUI_SkillSeparator&&) = delete;

		void render();
};

class GUI_SkillBar : public GUI_Element
{
	public:
		GUI_SkillBar(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_SkillBar(const GUI_SkillBar&) = delete;
		GUI_SkillBar& operator=(const GUI_SkillBar&) = delete;

		// non-moveable
		GUI_SkillBar(GUI_SkillBar&&) = delete;
		GUI_SkillBar& operator=(GUI_SkillBar&&) = delete;

		void setPercent(Uint8 percent);
		void setColor(Uint8 red, Uint8 green, Uint8 blue) {m_red = red; m_green = green; m_blue = blue;}
		void render();

	protected:
		Sint32 m_percent = 0;
		Uint8 m_red = 0;
		Uint8 m_green = 192;
		Uint8 m_blue = 0;
};

class GUI_SkillDescription : public GUI_Element
{
	public:
		GUI_SkillDescription(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_SkillDescription(const GUI_SkillDescription&) = delete;
		GUI_SkillDescription& operator=(const GUI_SkillDescription&) = delete;

		// non-moveable
		GUI_SkillDescription(GUI_SkillDescription&&) = delete;
		GUI_SkillDescription& operator=(GUI_SkillDescription&&) = delete;
		
		void setName(bool value, const std::string labelName);
		void setColor(bool value, Uint8 red, Uint8 green, Uint8 blue);
		void setAlign(bool value, Uint8 align);
		void setFont(bool value, Uint8 font);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void render();

	protected:
		struct
		{
			std::string m_Label;
			Sint32 m_align = CLIENT_FONT_ALIGN_LEFT;
			Uint8 m_font = CLIENT_FONT_NONOUTLINED;
			Uint8 m_red = 175;
			Uint8 m_green = 175;
			Uint8 m_blue = 175;
		} skillName;

		struct
		{
			std::string m_Label;
			Uint32 m_nameLen = 0;
			Sint32 m_align = CLIENT_FONT_ALIGN_LEFT;
			Uint8 m_font = CLIENT_FONT_NONOUTLINED;
			Uint8 m_red = 175;
			Uint8 m_green = 175;
			Uint8 m_blue = 175;
		} skillValue;
};

#endif /* __FILE_GUI_SKILLSWINDOW_h_ */
