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

#include "GUI_Description.h"
#include "../engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

void GUI_Description::setDisplay(Sint32 mouseX, Sint32 mouseY, const std::string description, Uint32 delay)
{
	m_description = std::move(description);
	m_startDisplay = g_frameTime + delay;

	Uint32 cacheMSGsizeX = 0, cacheMSGsizeY = 8;
	StringVector messages = UTIL_explodeString(m_description, "\n");
	for(size_t i = 0, end = messages.size(); i < end; ++i)
	{
		Uint32 cachedMSGsize = g_engine.calculateFontWidth(CLIENT_FONT_NONOUTLINED, messages[i]);
		if(cachedMSGsize > cacheMSGsizeX)
			cacheMSGsizeX = cachedMSGsize;
		cacheMSGsizeY += 16;
	}
	m_tRect.x2 = cacheMSGsizeX + 8;
	m_tRect.y2 = cacheMSGsizeY - 6;
	m_tRect.x1 = mouseX;
	m_tRect.y1 = mouseY - m_tRect.y2;

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

void GUI_Description::render()
{
	if(g_frameTime < m_startDisplay)
		return;

	auto& renderer = g_engine.getRender();
	renderer->drawRectangle(m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2, 1, 0, 0, 0, 255);
	renderer->fillRectangle(m_tRect.x1 + 1, m_tRect.y1 + 1, m_tRect.x2 - 2, m_tRect.y2 - 2, 192, 192, 192, 255);
	g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 4, m_tRect.y1 + 4, m_description, 63, 63, 63, CLIENT_FONT_ALIGN_LEFT);
}
