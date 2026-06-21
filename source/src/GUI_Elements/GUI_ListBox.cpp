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

#include "GUI_ListBox.h"
#include "../engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

GUI_ListBox::GUI_ListBox(iRect boxRect, Uint32 internalID) : m_scrollBar(iRect(0, 0, 0, 0), 0, 0)
{
	setRect(boxRect);
	m_internalID = internalID;
}

GUI_ListBox::~GUI_ListBox()
{
	m_listBox.clear();
}

void GUI_ListBox::setEventCallback(void (*eventHandlerFunction)(Uint32, Sint32), Uint32 mEvent)
{
	m_evtParam = mEvent;
	m_eventHandlerFunction = eventHandlerFunction;
}

void GUI_ListBox::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	iRect nRect = iRect(NewRect.x1 + NewRect.x2 - 13, NewRect.y1 + 1, 12, NewRect.y2 - 2);
	m_scrollBar.setRect(nRect);
	m_tRect = NewRect;
	m_maxDisplay = (m_tRect.y2 - 4) / 12;
	m_scrollBar.setScrollSize(SDL_static_cast(Sint32, m_listBox.size()) - m_maxDisplay);
}

void GUI_ListBox::setSelect(Sint32 select)
{
	Sint32 lastSelect = m_select;
	m_select = UTIL_max<Sint32>(0, UTIL_min<Sint32>(SDL_static_cast(Sint32, m_listBox.size() - 1), select));
	m_scrollBar.setScrollPos(m_select - m_maxDisplay / 2);//Keep the scrollbar in the center
	if(lastSelect != m_select && m_eventHandlerFunction)
		UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, m_select);
}

void GUI_ListBox::add(const std::string data)
{
	m_listBox.push_back(std::move(data));
	m_scrollBar.setScrollSize(SDL_static_cast(Sint32, m_listBox.size()) - m_maxDisplay);
}

void GUI_ListBox::set(Sint32 select, const std::string data)
{
	if(SDL_static_cast(size_t, select) < m_listBox.size())
		m_listBox[select] = std::move(data);
}

void GUI_ListBox::erase(Sint32 select)
{
	if(select == SDL_static_cast(Sint32, m_listBox.size() - 1))
		setSelect(m_select - 1);

	if(SDL_static_cast(size_t, select) < m_listBox.size())
	{
		m_listBox.erase(m_listBox.begin() + select);
		m_scrollBar.setScrollSize(SDL_static_cast(Sint32, m_listBox.size()) - m_maxDisplay);
	}
}

void GUI_ListBox::onKeyDown(SDL_Event& event)
{
	switch(event.key.keysym.sym)
	{
		case SDLK_UP:
		{
			m_select = UTIL_max<Sint32>(0, UTIL_min<Sint32>(SDL_static_cast(Sint32, m_listBox.size() - 1), m_select - 1));

			Sint32 actualPos = m_scrollBar.getScrollPos();
			Sint32 cachePos = m_select;
			if(actualPos > cachePos)
				m_scrollBar.setScrollPos(cachePos);
			if(m_eventHandlerFunction)
				UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, m_select);
			return;
		}

		case SDLK_DOWN:
		{
			m_select = UTIL_max<Sint32>(0, UTIL_min<Sint32>(SDL_static_cast(Sint32, m_listBox.size() - 1), m_select + 1));

			Sint32 actualPos = m_scrollBar.getScrollPos();
			Sint32 cachePos = m_select - m_maxDisplay + 1;
			if(actualPos < cachePos)
				m_scrollBar.setScrollPos(cachePos);
			if(m_eventHandlerFunction)
				UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, m_select);
			return;
		}

		default:
			break;
	}
}

void GUI_ListBox::onLMouseDown(Sint32 x, Sint32 y)
{
	iRect rect = iRect(m_tRect.x1 + 2, m_tRect.y1 + 2, m_tRect.x2 - 16, m_tRect.y2 - 4);
	if(rect.isPointInside(x, y))
	{
		Sint32 count = m_scrollBar.getScrollPos();
		std::vector<std::string>::iterator it = m_listBox.begin();
		std::advance(it, count);

		Sint32 posY = m_tRect.y1 + 2;
		Sint32 endY = m_tRect.y1 + m_tRect.y2 - 16;
		while(it != m_listBox.end())
		{
			if(posY > endY)
				break;

			rect = iRect(m_tRect.x1 + 2, posY, m_tRect.x2 - 16, 12);
			if(rect.isPointInside(x, y))
			{
				if(g_frameTime < m_lastClick && m_select == count)
					m_doubleClicked = true;

				m_select = count;
				m_lastClick = g_frameTime + 1000;
				if(m_eventHandlerFunction)
					UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, m_select);
				return;
			}

			posY += 12;
			++count;
			++it;
		}
		return;
	}
	if(m_scrollBar.getRect().isPointInside(x, y))
		m_scrollBar.onLMouseDown(x, y);
}

void GUI_ListBox::onLMouseUp(Sint32 x, Sint32 y)
{
	m_scrollBar.onLMouseUp(x, y);
	if(m_doubleClicked)
	{
		m_doubleClicked = false;
		if(m_eventHandlerFunction)
			UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, ~(m_select));
	}
}

void GUI_ListBox::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	m_scrollBar.onMouseMove(x, y, isInsideParent);
}

void GUI_ListBox::onWheel(Sint32, Sint32, bool wheelUP)
{
	if(wheelUP)
		m_scrollBar.setScrollPos(m_scrollBar.getScrollPos() - 1);
	else
		m_scrollBar.setScrollPos(m_scrollBar.getScrollPos() + 1);
}

void GUI_ListBox::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_DARK_X, GUI_UI_ICON_HORIZONTAL_LINE_DARK_Y, GUI_UI_ICON_HORIZONTAL_LINE_DARK_W, GUI_UI_ICON_HORIZONTAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1, m_tRect.x2, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_DARK_X, GUI_UI_ICON_VERTICAL_LINE_DARK_Y, GUI_UI_ICON_VERTICAL_LINE_DARK_W, GUI_UI_ICON_VERTICAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1 + 1, 1, m_tRect.y2 - 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_X, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_Y, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_W, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_H, m_tRect.x1 + 1, m_tRect.y1 + m_tRect.y2 - 1, m_tRect.x2 - 1, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_X, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_Y, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_W, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_H, m_tRect.x1 + m_tRect.x2 - 1, m_tRect.y1 + 1, 1, m_tRect.y2 - 2);
	renderer->fillRectangle(m_tRect.x1 + 1, m_tRect.y1 + 1, m_tRect.x2 - 14, m_tRect.y2 - 2, 64, 64, 64, 255);
	renderer->setClipRect(m_tRect.x1 + 2, m_tRect.y1 + 2, m_tRect.x2 - 16, m_tRect.y2 - 4);

	Sint32 count = m_scrollBar.getScrollPos();
	std::vector<std::string>::iterator it = m_listBox.begin();
	std::advance(it, count);

	Sint32 posY = m_tRect.y1 + 2;
	Sint32 endY = m_tRect.y1 + m_tRect.y2 - 16;
	while(it != m_listBox.end())
	{
		if(posY > endY)
			break;

		if(count++ == m_select)
		{
			renderer->fillRectangle(m_tRect.x1 + 2, posY, m_tRect.x2 - 16, 12, 112, 112, 112, 255);
			g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 3, posY + 1, (*it), 255, 255, 255, CLIENT_FONT_ALIGN_LEFT);
		}
		else
			g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 3, posY + 1, (*it), 175, 175, 175, CLIENT_FONT_ALIGN_LEFT);

		posY += 12;
		++it;
	}
	renderer->disableClipRect();
	m_scrollBar.render();
}