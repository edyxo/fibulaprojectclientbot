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

#include "GUI_CheckBox.h"
#include "../engine.h"

extern Engine g_engine;

GUI_CheckBox::GUI_CheckBox(iRect boxRect, const std::string labelName, bool checked, Uint32 internalID, Uint8 red, Uint8 green, Uint8 blue) :
	m_Label(std::move(labelName)), m_red(red), m_green(green), m_blue(blue), m_Checked(checked)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_CheckBox::setBoxEventCallback(void (*eventHandlerFunction)(Uint32,Sint32), Uint32 mEvent)
{
	m_evtParam = mEvent;
	m_eventHandlerFunction = eventHandlerFunction;
}

void GUI_CheckBox::setName(const std::string labelName)
{
	m_Label = std::move(labelName);
}

void GUI_CheckBox::setChecked(bool check)
{
    m_Checked = check;
    if(m_eventHandlerFunction)
		UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, (m_Checked ? 1 : 0));
}

void GUI_CheckBox::onLMouseUp(Sint32 x, Sint32 y)
{
	if(m_tRect.isPointInside(x, y))
		setChecked(!m_Checked);
}

void GUI_CheckBox::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_DARK_X, GUI_UI_ICON_HORIZONTAL_LINE_DARK_Y, GUI_UI_ICON_HORIZONTAL_LINE_DARK_W, GUI_UI_ICON_HORIZONTAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1, m_tRect.x2, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_DARK_X, GUI_UI_ICON_VERTICAL_LINE_DARK_Y, GUI_UI_ICON_VERTICAL_LINE_DARK_W, GUI_UI_ICON_VERTICAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1 + 1, 1, m_tRect.y2 - 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_X, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_Y, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_W, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_H, m_tRect.x1 + 1, m_tRect.y1 + m_tRect.y2 - 1, m_tRect.x2 - 1, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_X, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_Y, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_W, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_H, m_tRect.x1 + m_tRect.x2 - 1, m_tRect.y1 + 1, 1, m_tRect.y2 - 2);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_CHECKBOX_UNCHECKED_X, (m_Checked ? GUI_UI_CHECKBOX_CHECKED_Y : GUI_UI_CHECKBOX_UNCHECKED_Y), m_tRect.x1 + 6, m_tRect.y1 + 5, GUI_UI_CHECKBOX_CHECKED_W, GUI_UI_CHECKBOX_CHECKED_H);
	g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 22, m_tRect.y1 + 6, m_Label, m_red, m_green, m_blue, CLIENT_FONT_ALIGN_LEFT);
}
