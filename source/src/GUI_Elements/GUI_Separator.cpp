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

#include "GUI_Separator.h"
#include "../engine.h"

extern Engine g_engine;

GUI_Separator::GUI_Separator(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_Separator::render()
{
	g_engine.getRender()->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_SEPARATOR_X, GUI_UI_ICON_HORIZONTAL_SEPARATOR_Y, GUI_UI_ICON_HORIZONTAL_SEPARATOR_W, GUI_UI_ICON_HORIZONTAL_SEPARATOR_H, m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2);
}
