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

#include "GUI_StaticImage.h"
#include "../engine.h"

extern Engine g_engine;

GUI_StaticImage::GUI_StaticImage(iRect boxRect, Uint16 pictureId, Sint32 pictureX, Sint32 pictureY, Uint32 internalID, const std::string description) : 
	m_description(std::move(description)), m_pictureX(pictureX), m_pictureY(pictureY), m_pictureId(pictureId)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_StaticImage::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	bool inside = (isInsideParent && m_tRect.isPointInside(x, y));
	if(!m_description.empty() && inside)
		g_engine.showDescription(x, y, m_description);
}

void GUI_StaticImage::render()
{
	g_engine.getRender()->drawPicture(m_pictureId, m_pictureX, m_pictureY, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
}
