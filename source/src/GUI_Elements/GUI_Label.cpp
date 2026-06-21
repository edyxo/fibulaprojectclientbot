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

#include "GUI_Label.h"
#include "../engine.h"

extern Engine g_engine;

GUI_Label::GUI_Label(iRect boxRect, const std::string labelName, Uint32 internalID, Uint8 red, Uint8 green, Uint8 blue) :
	m_Label(std::move(labelName)), m_red(red), m_green(green), m_blue(blue)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_Label::setName(const std::string labelName)
{
	m_Label = std::move(labelName);
}

void GUI_Label::render()
{
	g_engine.drawFont(m_font, m_tRect.x1, m_tRect.y1, m_Label, m_red, m_green, m_blue, m_align);
}

GUI_DynamicLabel::GUI_DynamicLabel(iRect boxRect, const std::string labelName, Uint32 internalID, Uint8 red, Uint8 green, Uint8 blue) :
	m_Label(std::move(labelName)), m_red(red), m_green(green), m_blue(blue)
{
	m_displayLabel.assign(m_Label);
	m_internalID = internalID;
	setRect(boxRect);
}

void GUI_DynamicLabel::setRect(iRect& NewRect)
{
	m_tRect = NewRect;

	Uint32 allowedWidth = SDL_static_cast(Uint32, m_tRect.x2);
	Uint32 textWidth = g_engine.calculateFontWidth(m_font, m_Label);
	if(textWidth > allowedWidth)
	{
		for(size_t len = m_Label.length(); --len > 0;)
		{
			textWidth = g_engine.calculateFontWidth(m_font, m_Label, 0, len);
			if(textWidth <= allowedWidth)
			{
				m_displayLabel.assign(m_Label.substr(0, len));
				m_displayLabel.back() = '.'; m_displayLabel.append(2, '.');
				m_fullDisplay = false;
				return;
			}
		}
	}
}

void GUI_DynamicLabel::setName(const std::string labelName)
{
	m_Label = std::move(labelName);
	m_displayLabel.assign(m_Label);
	setRect(m_tRect);
}

void GUI_DynamicLabel::setFont(Uint8 font)
{
	m_font = font;
	setRect(m_tRect);
}

void GUI_DynamicLabel::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(!m_fullDisplay)
	{
		if(isInsideParent && m_tRect.isPointInside(x, y))
			g_engine.showDescription(x, y, m_Label);
	}
}

void GUI_DynamicLabel::render()
{
	g_engine.drawFont(m_font, m_tRect.x1, m_tRect.y1, m_displayLabel, m_red, m_green, m_blue, m_align);
}
