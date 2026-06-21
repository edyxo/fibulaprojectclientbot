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

#ifndef __FILE_GUI_PANEL_h_
#define __FILE_GUI_PANEL_h_

#include "../defines.h"

enum GUIPanelID
{
	GUI_PANEL_MAIN = 1,//Always
	GUI_PANEL_RIGHT = 2,//Always
	GUI_PANEL_EXTRA_RIGHT_START = 1000,//Optional
	GUI_PANEL_EXTRA_RIGHT_END = 1999,//Optional
	GUI_PANEL_EXTRA_LEFT_START = 2000,//Optional
	GUI_PANEL_EXTRA_LEFT_END = 2999,//Optional
	GUI_PANEL_RANDOM = -1
};

class GUI_PanelWindow;
class GUI_Panel
{
	public:
		GUI_Panel(iRect boxRect, Sint32 internalId = 0);
		~GUI_Panel();

		// non-copyable
		GUI_Panel(const GUI_Panel&) = delete;
		GUI_Panel& operator=(const GUI_Panel&) = delete;

		// non-moveable
		GUI_Panel(GUI_Panel&&) = delete;
		GUI_Panel& operator=(GUI_Panel&&) = delete;

		void setRect(iRect& NewRect);
		iRect& getRect() {return m_tRect;}
		bool isInsideRect(Sint32 x, Sint32 y) {return m_tRect.isPointInside(x, y);}
		Sint32 getFreeHeight();

		void clearPanels();
		void resizePanel(GUI_PanelWindow* pPanel, Sint32 x, Sint32 y);
		void checkPanels();
		void checkPanel(GUI_PanelWindow* pPanel, Sint32 x, Sint32 y);
		bool tryFreeHeight(GUI_PanelWindow* pPanel);
		bool tryFreeHeight(Sint32 needFreeHeight);
		void addPanel(GUI_PanelWindow* pPanel);
		void removePanel(GUI_PanelWindow* pPanel, bool deletePanel = true);
		GUI_PanelWindow* getPanel(Uint32 internalID);
		std::vector<GUI_PanelWindow*>& getPanelWindows() {return m_panels;}

		void setInternalID(Sint32 internalId) {m_internalID = internalId;}
		Sint32 getInternalID() {return m_internalID;}

		void* onAction(Sint32 x, Sint32 y);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);
		void onRMouseUp(Sint32 x, Sint32 y);
		void onWheel(Sint32 x, Sint32 y, bool wheelUP);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);

		void render();

	protected:
		std::vector<GUI_PanelWindow*> m_panels;
		iRect m_tRect;
		Sint32 m_lastPosY;
		Sint32 m_freeHeight;
		Sint32 m_internalID;
};

#endif /* __FILE_GUI_PANEL_h_ */
