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

#include "GUI_ContextMenu.h"
#include "../engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

void GUI_ContextMenu::addContextMenu(Uint8 style, Uint32 eventId, const std::string text, const std::string shortcut)
{
	m_childs.emplace_back();
	ContextMenuChild& newChild = m_childs.back();
	newChild.text = std::move(text);
	newChild.shortcut = std::move(shortcut);
	newChild.eventId = eventId;
	newChild.childStyle = style;
}

void GUI_ContextMenu::addSeparator()
{
	if(!m_childs.empty())
	{
		ContextMenuChild& child = m_childs.back();
		child.childStyle |= CONTEXTMENU_STYLE_SEPARATED;
	}
}

void GUI_ContextMenu::setEventCallback(void (*eventHandlerFunction)(Uint32, Sint32))
{
	m_eventHandlerFunction = eventHandlerFunction;
}

void GUI_ContextMenu::setDisplay(Sint32 mouseX, Sint32 mouseY)
{
	Uint32 currentWidth = 162, currentHeigth = 12;
	bool nextLineSeparator = false;
	for(std::vector<ContextMenuChild>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		Uint32 cachedMSGsize = g_engine.calculateFontWidth(CLIENT_FONT_OUTLINED, (*it).text) + 12;
		if(nextLineSeparator)
		{
			currentHeigth += 8;
			nextLineSeparator = false;
		}
		if(!(*it).shortcut.empty())
			cachedMSGsize += g_engine.calculateFontWidth(CLIENT_FONT_OUTLINED, (*it).shortcut) + 6;

		if((*it).childStyle & (CONTEXTMENU_STYLE_CHECKED|CONTEXTMENU_STYLE_UNCHECKED))
			cachedMSGsize += 16;

		if(cachedMSGsize > currentWidth)
			currentWidth = cachedMSGsize;

		if((*it).childStyle & CONTEXTMENU_STYLE_SEPARATED)
			nextLineSeparator = true;

		currentHeigth += 19;
	}
	if(!m_childs.empty())
		currentHeigth -= 9;

	m_tRect.x2 = currentWidth;
	m_tRect.y2 = currentHeigth;
	m_tRect.x1 = mouseX;
	m_tRect.y1 = mouseY;

	Sint32 maxX = g_engine.getWindowWidth();
	Sint32 maxY = g_engine.getWindowHeight();
	if(m_tRect.x1 + m_tRect.x2 > maxX)
		m_tRect.x1 = maxX - m_tRect.x2;
	if(m_tRect.y1 + m_tRect.y2 > maxY)
		m_tRect.y1 = maxY - m_tRect.y2;
	if(m_tRect.x1 < 0)
		m_tRect.x1 = 0;
	if(m_tRect.y1 < 0)
		m_tRect.y1 = 0;
}

void GUI_ContextMenu::onLMouseUp(Sint32 x, Sint32 y)
{
	if(!m_tRect.isPointInside(x, y))
		return;

	Sint32 startY = m_tRect.y1 + 6;
	bool nextLineSeparator = false;
	for(std::vector<ContextMenuChild>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		if(nextLineSeparator)
		{
			startY += 8;
			nextLineSeparator = false;
		}

		iRect rect = iRect(m_tRect.x1 + 4, startY - 2, m_tRect.x2 - 8, 16);
		if(rect.isPointInside(x, y))
		{
			if(m_eventHandlerFunction)
				UTIL_SafeEventHandler(m_eventHandlerFunction, (*it).eventId, 1);
			return;
		}
		if((*it).childStyle & CONTEXTMENU_STYLE_SEPARATED)
			nextLineSeparator = true;

		startY += 19;
	}
}

void GUI_ContextMenu::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	m_hoverEvent = -1;
	if(!isInsideParent)
		return;

	Sint32 startY = m_tRect.y1 + 6, index = 0;
	bool nextLineSeparator = false;
	for(std::vector<ContextMenuChild>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it, ++index)
	{
		if(nextLineSeparator)
		{
			startY += 8;
			nextLineSeparator = false;
		}

		iRect rect = iRect(m_tRect.x1 + 4, startY - 2, m_tRect.x2 - 8, 16);
		if(rect.isPointInside(x, y))
		{
			m_hoverEvent = index;
			return;
		}
		if((*it).childStyle & CONTEXTMENU_STYLE_SEPARATED)
			nextLineSeparator = true;

		startY += 19;
	}
}

void GUI_ContextMenu::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_BACKGROUND_GREY_X, GUI_UI_BACKGROUND_GREY_Y, GUI_UI_BACKGROUND_GREY_W, GUI_UI_BACKGROUND_GREY_H, m_tRect.x1 + 3, m_tRect.y1 + 3, m_tRect.x2 - 6, m_tRect.y2 - 6);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_TOPLEFT_BORDER_X, GUI_UI_ICON_TOPLEFT_BORDER_Y, m_tRect.x1, m_tRect.y1, GUI_UI_ICON_TOPLEFT_BORDER_W, GUI_UI_ICON_TOPLEFT_BORDER_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_TOPRIGHT_BORDER_X, GUI_UI_ICON_TOPRIGHT_BORDER_Y, m_tRect.x1 + m_tRect.x2 - 3, m_tRect.y1, GUI_UI_ICON_TOPRIGHT_BORDER_W, GUI_UI_ICON_TOPRIGHT_BORDER_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_BOTLEFT_BORDER_X, GUI_UI_ICON_BOTLEFT_BORDER_Y, m_tRect.x1, m_tRect.y1 + m_tRect.y2 - 3, GUI_UI_ICON_BOTLEFT_BORDER_W, GUI_UI_ICON_BOTLEFT_BORDER_H);
	renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_BOTRIGHT_BORDER_X, GUI_UI_ICON_BOTRIGHT_BORDER_Y, m_tRect.x1 + m_tRect.x2 - 3, m_tRect.y1 + m_tRect.y2 - 3, GUI_UI_ICON_BOTRIGHT_BORDER_W, GUI_UI_ICON_BOTRIGHT_BORDER_H);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_DIVIDER_X, GUI_UI_ICON_HORIZONTAL_DIVIDER_Y, GUI_UI_ICON_HORIZONTAL_DIVIDER_W, GUI_UI_ICON_HORIZONTAL_DIVIDER_H, m_tRect.x1 + 3, m_tRect.y1, m_tRect.x2 - 6, 3);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_DIVIDER_X, GUI_UI_ICON_VERTICAL_DIVIDER_Y, GUI_UI_ICON_VERTICAL_DIVIDER_W, GUI_UI_ICON_VERTICAL_DIVIDER_H, m_tRect.x1, m_tRect.y1 + 3, 3, m_tRect.y2 - 6);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_DIVIDER_X, GUI_UI_ICON_HORIZONTAL_DIVIDER_Y, GUI_UI_ICON_HORIZONTAL_DIVIDER_W, GUI_UI_ICON_HORIZONTAL_DIVIDER_H, m_tRect.x1 + 3, m_tRect.y1 + m_tRect.y2 - 3, m_tRect.x2 - 6, 3);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_DIVIDER_X, GUI_UI_ICON_VERTICAL_DIVIDER_Y, GUI_UI_ICON_VERTICAL_DIVIDER_W, GUI_UI_ICON_VERTICAL_DIVIDER_H, m_tRect.x1 + m_tRect.x2 - 3, m_tRect.y1 + 3, 3, m_tRect.y2 - 6);

	Sint32 startY = m_tRect.y1 + 6, index = 0;
	bool nextLineSeparator = false;
	for(std::vector<ContextMenuChild>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it, ++index)
	{
		if(nextLineSeparator)
		{
			renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_SEPARATOR_X, GUI_UI_ICON_HORIZONTAL_SEPARATOR_Y, GUI_UI_ICON_HORIZONTAL_SEPARATOR_W, GUI_UI_ICON_HORIZONTAL_SEPARATOR_H, m_tRect.x1 + 4, startY - 2, m_tRect.x2 - 8, 2);
			startY += 8;
			nextLineSeparator = false;
		}
		if(m_hoverEvent == index)
			renderer->fillRectangle(m_tRect.x1 + 4, startY - 2, m_tRect.x2 - 8, 16, 128, 128, 128, 255);

		if((*it).childStyle & CONTEXTMENU_STYLE_SEPARATED)
			nextLineSeparator = true;
		if((*it).childStyle & CONTEXTMENU_STYLE_STANDARD)
			g_engine.drawFont(CLIENT_FONT_OUTLINED, m_tRect.x1 + 6, startY, (*it).text, 223, 223, 223, CLIENT_FONT_ALIGN_LEFT);
		if((*it).childStyle & CONTEXTMENU_STYLE_CHECKED)
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_CHECKBOX_CHECKED_X, GUI_UI_CHECKBOX_CHECKED_Y, m_tRect.x1 + 6, startY, GUI_UI_CHECKBOX_CHECKED_W, GUI_UI_CHECKBOX_CHECKED_H);
			g_engine.drawFont(CLIENT_FONT_OUTLINED, m_tRect.x1 + 22, startY, (*it).text, 223, 223, 223, CLIENT_FONT_ALIGN_LEFT);
		}
		if((*it).childStyle & CONTEXTMENU_STYLE_UNCHECKED)
		{
			renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_CHECKBOX_UNCHECKED_X, GUI_UI_CHECKBOX_UNCHECKED_Y, m_tRect.x1 + 6, startY, GUI_UI_CHECKBOX_UNCHECKED_W, GUI_UI_CHECKBOX_UNCHECKED_H);
			g_engine.drawFont(CLIENT_FONT_OUTLINED, m_tRect.x1 + 22, startY, (*it).text, 223, 223, 223, CLIENT_FONT_ALIGN_LEFT);
		}
		if(!(*it).shortcut.empty())
			g_engine.drawFont(CLIENT_FONT_OUTLINED, m_tRect.x1 + m_tRect.x2 - 6, startY, (*it).shortcut, 223, 223, 223, CLIENT_FONT_ALIGN_RIGHT);

		startY += 19;
	}
}
