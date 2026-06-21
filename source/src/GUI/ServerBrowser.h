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

#ifndef __FILE_GUI_SERVERBROWSER_h_
#define __FILE_GUI_SERVERBROWSER_h_

#include "../GUI_Elements/GUI_Container.h"
#include "../GUI_Elements/GUI_Label.h"

class GUI_ServerBrowserContainer : public GUI_Container
{
	public:
		GUI_ServerBrowserContainer(iRect boxRect, Uint32 internalID = 0) : GUI_Container(boxRect, NULL, internalID) {}

		// non-copyable
		GUI_ServerBrowserContainer(const GUI_ServerBrowserContainer&) = delete;
		GUI_ServerBrowserContainer& operator=(const GUI_ServerBrowserContainer&) = delete;

		// non-moveable
		GUI_ServerBrowserContainer(GUI_ServerBrowserContainer&&) = delete;
		GUI_ServerBrowserContainer& operator=(GUI_ServerBrowserContainer&&) = delete;

		void render();
};

class GUI_ServerBrowserEntry : public GUI_Element
{
	public:
		GUI_ServerBrowserEntry(iRect boxRect, Uint32 internalID = 0);

		// non-copyable
		GUI_ServerBrowserEntry(const GUI_ServerBrowserEntry&) = delete;
		GUI_ServerBrowserEntry& operator=(const GUI_ServerBrowserEntry&) = delete;

		// non-moveable
		GUI_ServerBrowserEntry(GUI_ServerBrowserEntry&&) = delete;
		GUI_ServerBrowserEntry& operator=(GUI_ServerBrowserEntry&&) = delete;

		void setRect(iRect& NewRect);
		void setServerIp(std::string serverIp) {m_serverIp.setName(std::move(serverIp));}
		void setServerName(std::string serverName) {m_serverName.setName(std::move(serverName));}
		void setServerPlayers(std::string serverPlayers) {m_serverPlayers.setName(std::move(serverPlayers));}
		void setServerType(std::string serverType) {m_serverType.setName(std::move(serverType));}
		void setServerExp(std::string serverExp) {m_serverExp.setName(std::move(serverExp));}
		void setServerVersion(std::string serverVersion) {m_serverVersion.setName(std::move(serverVersion));}

		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);

		void render();

	protected:
		GUI_DynamicLabel m_serverIp;
		GUI_DynamicLabel m_serverName;
		GUI_DynamicLabel m_serverPlayers;
		GUI_DynamicLabel m_serverType;
		GUI_DynamicLabel m_serverExp;
		GUI_DynamicLabel m_serverVersion;
		bool m_doubleClicked = false;
};

#endif /* __FILE_GUI_SERVERBROWSER_h_ */
