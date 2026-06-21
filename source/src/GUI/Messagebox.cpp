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

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../GUI_Elements/GUI_Window.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../protocolloginHttp.h"

#define MESSAGEBOX_OK_EVENTID 1000

extern Engine g_engine;
extern ProtocolLoginHttp g_protocolLoginHttp;

void messagebox_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case MESSAGEBOX_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_MESSAGEBOX)
			{
				g_engine.removeWindow(pWindow);
				if(!g_engine.isIngame())
				{
					g_engine.releaseConnection();
					g_protocolLoginHttp.closeConnection();
				}
			}
		}
		break;
		default: break;
	}
}

void UTIL_messageBox(const std::string& title, const std::string& message)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_MESSAGEBOX);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	Uint32 cacheMSGsize = 274, cacheMSGsize2 = 91;
	StringVector messages = UTIL_explodeString(message, "\n");
	for(size_t i = 0, end = messages.size(); i < end; ++i)
	{
		Uint32 cachedMSGsize = g_engine.calculateFontWidth(CLIENT_FONT_NONOUTLINED, messages[i]);
		if(cachedMSGsize > cacheMSGsize)
			cacheMSGsize = cachedMSGsize;
		cacheMSGsize2 += 14;
	}

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, cacheMSGsize + 36, cacheMSGsize2), title, GUI_WINDOW_MESSAGEBOX);
	GUI_Label* newLabel;
	Sint32 labelY = 34;
	for(size_t i = 0, end = messages.size(); i < end; ++i)
	{
		newLabel = new GUI_Label(iRect(18, labelY, 43, 20), messages[i]);
		newWindow->addChild(newLabel);
		labelY += 14;
	}
	GUI_Button* newButton = new GUI_Button(iRect(cacheMSGsize - 20, cacheMSGsize2 - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&messagebox_Events, MESSAGEBOX_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(cacheMSGsize - 20, cacheMSGsize2 - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&messagebox_Events, MESSAGEBOX_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, cacheMSGsize2 - 40, cacheMSGsize + 10, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}
