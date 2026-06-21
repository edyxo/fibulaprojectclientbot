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

#include "GUI_ScrollBar.h"
#include "../engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

GUI_HScrollBar::GUI_HScrollBar(iRect boxRect, Sint32 sSize, Sint32 sPos, Uint32 internalID)
{
	setRect(boxRect);
	m_lastUpdate = g_frameTime;
	m_internalID = internalID;
	if(sSize != 0)
		setScrollSize(sSize);

	if(sPos != 0)
		setScrollPos(sPos);
}

void GUI_HScrollBar::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	m_tRect = NewRect;
	setScrollSize(m_scrollSize);
}

void GUI_HScrollBar::setBarEventCallback(void(*eventHandlerFunction)(Uint32, Sint32), Uint32 mEvent)
{
	m_evtParam = mEvent;
	m_eventHandlerFunction = eventHandlerFunction;
}

void GUI_HScrollBar::setScrollSize(Sint32 sSize)
{
	if(sSize <= 0)
	{
		m_scrollSize = 0;
		m_scrollPosSize = 0;
		m_scrollScale = 0.f;
		m_scrollInvScale = 0.f;
		return;
	}

	m_scrollSize = sSize;
	m_scrollPosSize = m_tRect.x2 - 36;
	if(m_scrollSize > m_scrollPosSize)
	{
		m_haveHandle = false;
		m_scrollScale = SDL_static_cast(float, m_scrollPosSize) / sSize;
		m_scrollInvScale = SDL_static_cast(float, sSize) / m_scrollPosSize;
	}
	else
		m_haveHandle = true;
	
	if(m_scrollSize < m_scrollPos[1])
		setScrollPos(m_scrollSize);
	else
		setScrollPos(m_scrollPos[1]);
}

void GUI_HScrollBar::setScrollPos(Sint32 sPos, bool notify)
{
	sPos = UTIL_max<Sint32>(0, UTIL_min<Sint32>(m_scrollSize, sPos));

	bool needEvent = (m_scrollPos[1] != sPos);
	if(m_haveHandle)
	{
		m_scrollPos[0] = m_scrollPosSize - m_scrollSize;
		m_scrollPos[1] = sPos;
	}
	else
	{
		m_scrollPos[0] = UTIL_max<Sint32>(0, UTIL_min<Sint32>(m_scrollPosSize, SDL_static_cast(Sint32, sPos * m_scrollScale)));
		m_scrollPos[1] = sPos;
	}
	if(notify && needEvent && m_eventHandlerFunction)
		UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, sPos);
}

void GUI_HScrollBar::onLMouseDown(Sint32 x, Sint32 y)
{
	iRect rect = iRect(m_tRect.x1, m_tRect.y1, 12, 12);
	if(rect.isPointInside(x, y))
	{
		m_buttonDown = 1;
		m_lastUpdate = g_frameTime;
		setScrollPos(m_scrollPos[1] - 1);
		return;
	}

	rect = iRect(m_tRect.x1 + m_tRect.x2 - 12, m_tRect.y1, 12, 12);
	if(rect.isPointInside(x, y))
	{
		m_buttonDown = 2;
		m_lastUpdate = g_frameTime;
		setScrollPos(m_scrollPos[1] + 1);
		return;
	}

	Sint32 xX;
	if(m_haveHandle)
	{
		xX = m_tRect.x1 + 12 + m_scrollPos[1];
		rect = iRect(xX, m_tRect.y1, m_scrollPos[0] + 12, 12);
		if(rect.isPointInside(x, y))
		{
			m_buttonDown = 3;
			m_mouseClickX = x - xX;
			return;
		}
	}
	else
	{
		xX = m_tRect.x1 + 12 + m_scrollPos[0];
		rect = iRect(xX, m_tRect.y1, 12, 12);
		if(rect.isPointInside(x, y))
		{
			m_buttonDown = 3;
			m_mouseClickX = x - xX;
			return;
		}
	}

	if(x <= xX)
	{
		setScrollPos(m_scrollPos[1] - 10);
		m_buttonDown = 4;
		m_lastUpdate = g_frameTime;
		m_mouseClickX = x;
	}
	else
	{
		setScrollPos(m_scrollPos[1] + 10);
		m_buttonDown = 5;
		m_lastUpdate = g_frameTime;
		m_mouseClickX = x;
	}
}

void GUI_HScrollBar::onLMouseUp(Sint32, Sint32)
{
	m_buttonDown = 0;
}

void GUI_HScrollBar::onMouseMove(Sint32 x, Sint32, bool)
{
	if(m_buttonDown == 3)
	{
		if(m_haveHandle)
			setScrollPos(SDL_static_cast(Sint32, x - (m_tRect.x1 + 12 + m_mouseClickX)));
		else
			setScrollPos(SDL_static_cast(Sint32, (x - (m_tRect.x1 + 12 + m_mouseClickX)) * m_scrollInvScale));
	}
}

void GUI_HScrollBar::onWheel(Sint32, Sint32, bool wheelUP)
{
	if(wheelUP)
		setScrollPos(m_scrollPos[1] - 1);
	else
		setScrollPos(m_scrollPos[1] + 1);
}

void GUI_HScrollBar::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HORIZONTAL_TEXTURE_X, GUI_UI_ICON_SCROLLBAR_HORIZONTAL_TEXTURE_Y, GUI_UI_ICON_SCROLLBAR_HORIZONTAL_TEXTURE_W, GUI_UI_ICON_SCROLLBAR_HORIZONTAL_TEXTURE_H, m_tRect.x1 + 12, m_tRect.y1, m_tRect.x2 - 24, 12);
	if(m_buttonDown == 1)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_LEFT_DOWN_X, GUI_UI_ICON_SCROLLBAR_LEFT_DOWN_Y, m_tRect.x1, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_LEFT_DOWN_W, GUI_UI_ICON_SCROLLBAR_LEFT_DOWN_H);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_X, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_Y, m_tRect.x1 + m_tRect.x2 - 12, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_W, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_H);
		if(g_frameTime - m_lastUpdate >= 100)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] - 1);
		}
	}
	else if(m_buttonDown == 2)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_LEFT_UP_X, GUI_UI_ICON_SCROLLBAR_LEFT_UP_Y, m_tRect.x1, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_LEFT_UP_W, GUI_UI_ICON_SCROLLBAR_LEFT_UP_H);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_RIGHT_DOWN_X, GUI_UI_ICON_SCROLLBAR_RIGHT_DOWN_Y, m_tRect.x1 + m_tRect.x2 - 12, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_RIGHT_DOWN_W, GUI_UI_ICON_SCROLLBAR_RIGHT_DOWN_H);
		if(g_frameTime - m_lastUpdate >= 100)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] + 1);
		}
	}
	else
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_LEFT_UP_X, GUI_UI_ICON_SCROLLBAR_LEFT_UP_Y, m_tRect.x1, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_LEFT_UP_W, GUI_UI_ICON_SCROLLBAR_LEFT_UP_H);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_X, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_Y, m_tRect.x1 + m_tRect.x2 - 12, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_W, GUI_UI_ICON_SCROLLBAR_RIGHT_UP_H);
		if(m_buttonDown == 4 && g_frameTime - m_lastUpdate >= 200)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] - 10);
			if(m_tRect.x1 + 12 + m_scrollPos[(m_haveHandle ? 1 : 0)] <= m_mouseClickX)
			{
				m_buttonDown = 3;
				m_mouseClickX -= m_tRect.x1 + 12 + m_scrollPos[(m_haveHandle ? 1 : 0)];
			}
		}
		else if(m_buttonDown == 5 && g_frameTime - m_lastUpdate >= 200)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] + 10);
			if(m_tRect.x1 + 12 + m_scrollPos[(m_haveHandle ? 1 : 0)] >= m_mouseClickX)
			{
				m_buttonDown = 3;
				m_mouseClickX -= m_tRect.x1 + 12 + m_scrollPos[(m_haveHandle ? 1 : 0)];
			}
		}
	}
	if(!m_scrollSize)
		return;

	if(m_haveHandle)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_X, GUI_UI_ICON_SCROLLBAR_HANDLE_Y, m_tRect.x1 + 12 + m_scrollPos[1], m_tRect.y1, 6, GUI_UI_ICON_SCROLLBAR_HANDLE_H);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_X + 6, GUI_UI_ICON_SCROLLBAR_HANDLE_Y, m_tRect.x1 + 12 + m_scrollPos[1] + m_scrollPos[0] + 6, m_tRect.y1, 6, GUI_UI_ICON_SCROLLBAR_HANDLE_H);
		renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_HORIZONTAL_X, GUI_UI_ICON_SCROLLBAR_HANDLE_HORIZONTAL_Y, GUI_UI_ICON_SCROLLBAR_HANDLE_HORIZONTAL_W, GUI_UI_ICON_SCROLLBAR_HANDLE_HORIZONTAL_H, m_tRect.x1 + 12 + m_scrollPos[1] + 6, m_tRect.y1, m_scrollPos[0], GUI_UI_ICON_SCROLLBAR_HANDLE_HORIZONTAL_H);
	}
	else
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_X, GUI_UI_ICON_SCROLLBAR_HANDLE_Y, m_tRect.x1 + 12 + m_scrollPos[0], m_tRect.y1, GUI_UI_ICON_SCROLLBAR_HANDLE_W, GUI_UI_ICON_SCROLLBAR_HANDLE_H);
}

GUI_VScrollBar::GUI_VScrollBar(iRect boxRect, Sint32 sSize, Sint32 sPos, Uint32 internalID)
{
	setRect(boxRect);
	m_lastUpdate = g_frameTime;
	m_internalID = internalID;
	if(sSize != 0)
		setScrollSize(sSize);

	if(sPos != 0)
		setScrollPos(sPos);
}

void GUI_VScrollBar::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	m_tRect = NewRect;
	setScrollSize(m_scrollSize);
}

void GUI_VScrollBar::setBarEventCallback(void (*eventHandlerFunction)(Uint32, Sint32), Uint32 mEvent)
{
	m_evtParam = mEvent;
	m_eventHandlerFunction = eventHandlerFunction;
}

void GUI_VScrollBar::setScrollSize(Sint32 sSize)
{
	if(sSize <= 0)
	{
		m_scrollSize = 0;
		m_scrollPosSize = 0;
		m_scrollScale = 0.f;
		m_scrollInvScale = 0.f;
		return;
	}

	m_scrollSize = sSize;
	m_scrollPosSize = m_tRect.y2 - 36;
	if(m_scrollSize > m_scrollPosSize)
	{
		m_haveHandle = false;
		m_scrollScale = SDL_static_cast(float, m_scrollPosSize) / sSize;
		m_scrollInvScale = SDL_static_cast(float, sSize) / m_scrollPosSize;
	}
	else
		m_haveHandle = true;
	
	if(m_scrollSize < m_scrollPos[1])
		setScrollPos(m_scrollSize);
	else
		setScrollPos(m_scrollPos[1]);
}

void GUI_VScrollBar::setScrollPos(Sint32 sPos, bool notify)
{
	sPos = UTIL_max<Sint32>(0, UTIL_min<Sint32>(m_scrollSize, sPos));

	bool needEvent = (m_scrollPos[1] != sPos);
	if(m_haveHandle)
	{
		m_scrollPos[0] = m_scrollPosSize - m_scrollSize;
		m_scrollPos[1] = sPos;
	}
	else
	{
		m_scrollPos[0] = UTIL_max<Sint32>(0, UTIL_min<Sint32>(m_scrollPosSize, SDL_static_cast(Sint32, sPos * m_scrollScale)));
		m_scrollPos[1] = sPos;
	}
	if(notify && needEvent && m_eventHandlerFunction)
		UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, sPos);
}

void GUI_VScrollBar::onLMouseDown(Sint32 x, Sint32 y)
{
	iRect rect = iRect(m_tRect.x1, m_tRect.y1, 12, 12);
	if(rect.isPointInside(x, y))
	{
		m_buttonDown = 1;
		m_lastUpdate = g_frameTime;
		setScrollPos(m_scrollPos[1] - 1);
		return;
	}

	rect = iRect(m_tRect.x1, m_tRect.y1 + m_tRect.y2 - 12, 12, 12);
	if(rect.isPointInside(x, y))
	{
		m_buttonDown = 2;
		m_lastUpdate = g_frameTime;
		setScrollPos(m_scrollPos[1] + 1);
		return;
	}

	Sint32 yY;
	if(m_haveHandle)
	{
		yY = m_tRect.y1 + 12 + m_scrollPos[1];
		rect = iRect(m_tRect.x1, yY, 12, m_scrollPos[0] + 12);
		if(rect.isPointInside(x, y))
		{
			m_buttonDown = 3;
			m_mouseClickY = y - yY;
			return;
		}
	}
	else
	{
		yY = m_tRect.y1 + 12 + m_scrollPos[0];
		rect = iRect(m_tRect.x1, yY, 12, 12);
		if(rect.isPointInside(x, y))
		{
			m_buttonDown = 3;
			m_mouseClickY = y - yY;
			return;
		}
	}

	if(y <= yY)
	{
		setScrollPos(m_scrollPos[1] - 10);
		m_buttonDown = 4;
		m_lastUpdate = g_frameTime;
		m_mouseClickY = y;
	}
	else
	{
		setScrollPos(m_scrollPos[1] + 10);
		m_buttonDown = 5;
		m_lastUpdate = g_frameTime;
		m_mouseClickY = y;
	}
}

void GUI_VScrollBar::onLMouseUp(Sint32, Sint32)
{
	m_buttonDown = 0;
}

void GUI_VScrollBar::onMouseMove(Sint32, Sint32 y, bool)
{
	if(m_buttonDown == 3)
	{
		if(m_haveHandle)
			setScrollPos(SDL_static_cast(Sint32, y - (m_tRect.y1 + 12 + m_mouseClickY)));
		else
			setScrollPos(SDL_static_cast(Sint32, (y - (m_tRect.y1 + 12 + m_mouseClickY)) * m_scrollInvScale));
	}
}

void GUI_VScrollBar::onWheel(Sint32, Sint32, bool wheelUP)
{
	if(wheelUP)
		setScrollPos(m_scrollPos[1] - 1);
	else
		setScrollPos(m_scrollPos[1] + 1);
}

void GUI_VScrollBar::render()
{
	auto& renderer = g_engine.getRender();
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_VERTICAL_TEXTURE_X, GUI_UI_ICON_SCROLLBAR_VERTICAL_TEXTURE_Y, GUI_UI_ICON_SCROLLBAR_VERTICAL_TEXTURE_W, GUI_UI_ICON_SCROLLBAR_VERTICAL_TEXTURE_H, m_tRect.x1, m_tRect.y1 + 12, 12, m_tRect.y2 - 24);
	if(m_buttonDown == 1)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_UP_DOWN_X, GUI_UI_ICON_SCROLLBAR_UP_DOWN_Y, m_tRect.x1, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_UP_DOWN_W, GUI_UI_ICON_SCROLLBAR_UP_DOWN_H);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_DOWN_UP_X, GUI_UI_ICON_SCROLLBAR_DOWN_UP_Y, m_tRect.x1, m_tRect.y1 + m_tRect.y2 - 12, GUI_UI_ICON_SCROLLBAR_DOWN_UP_W, GUI_UI_ICON_SCROLLBAR_DOWN_UP_H);
		if(g_frameTime - m_lastUpdate >= 100)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] - 1);
		}
	}
	else if(m_buttonDown == 2)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_UP_UP_X, GUI_UI_ICON_SCROLLBAR_UP_UP_Y, m_tRect.x1, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_UP_UP_W, GUI_UI_ICON_SCROLLBAR_UP_UP_H);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_DOWN_DOWN_X, GUI_UI_ICON_SCROLLBAR_DOWN_DOWN_Y, m_tRect.x1, m_tRect.y1 + m_tRect.y2 - 12, GUI_UI_ICON_SCROLLBAR_DOWN_DOWN_W, GUI_UI_ICON_SCROLLBAR_DOWN_DOWN_H);
		if(g_frameTime - m_lastUpdate >= 100)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] + 1);
		}
	}
	else
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_UP_UP_X, GUI_UI_ICON_SCROLLBAR_UP_UP_Y, m_tRect.x1, m_tRect.y1, GUI_UI_ICON_SCROLLBAR_UP_UP_W, GUI_UI_ICON_SCROLLBAR_UP_UP_H);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_DOWN_UP_X, GUI_UI_ICON_SCROLLBAR_DOWN_UP_Y, m_tRect.x1, m_tRect.y1 + m_tRect.y2 - 12, GUI_UI_ICON_SCROLLBAR_DOWN_UP_W, GUI_UI_ICON_SCROLLBAR_DOWN_UP_H);
		if(m_buttonDown == 4 && g_frameTime - m_lastUpdate >= 200)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] - 10);
			if(m_tRect.y1 + 12 + m_scrollPos[0] <= m_mouseClickY)
			{
				m_buttonDown = 3;
				m_mouseClickY -= m_tRect.y1 + 12 + m_scrollPos[(m_haveHandle ? 1 : 0)];
			}
		}
		else if(m_buttonDown == 5 && g_frameTime - m_lastUpdate >= 200)
		{
			m_lastUpdate = g_frameTime;
			setScrollPos(m_scrollPos[1] + 10);
			if(m_tRect.y1 + 12 + m_scrollPos[(m_haveHandle ? 1 : 0)] >= m_mouseClickY)
			{
				m_buttonDown = 3;
				m_mouseClickY -= m_tRect.y1 + 12 + m_scrollPos[(m_haveHandle ? 1 : 0)];
			}
		}
	}
	if(!m_scrollSize)
		return;

	if(m_haveHandle)
	{
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_X, GUI_UI_ICON_SCROLLBAR_HANDLE_Y, m_tRect.x1, m_tRect.y1 + 12 + m_scrollPos[1], GUI_UI_ICON_SCROLLBAR_HANDLE_W, 6);
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_X, GUI_UI_ICON_SCROLLBAR_HANDLE_Y + 6, m_tRect.x1, m_tRect.y1 + 12 + m_scrollPos[1] + m_scrollPos[0] + 6, GUI_UI_ICON_SCROLLBAR_HANDLE_W, 6);
		renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_VERTICAL_X, GUI_UI_ICON_SCROLLBAR_HANDLE_VERTICAL_Y, GUI_UI_ICON_SCROLLBAR_HANDLE_VERTICAL_W, GUI_UI_ICON_SCROLLBAR_HANDLE_VERTICAL_H, m_tRect.x1, m_tRect.y1 + 12 + m_scrollPos[1] + 6, GUI_UI_ICON_SCROLLBAR_HANDLE_VERTICAL_W, m_scrollPos[0]);
	}
	else
		renderer->drawPicture(GUI_UI_IMAGE, GUI_UI_ICON_SCROLLBAR_HANDLE_X, GUI_UI_ICON_SCROLLBAR_HANDLE_Y, m_tRect.x1, m_tRect.y1 + 12 + m_scrollPos[0], GUI_UI_ICON_SCROLLBAR_HANDLE_W, GUI_UI_ICON_SCROLLBAR_HANDLE_H);
}
