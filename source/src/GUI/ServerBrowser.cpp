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

// If some owner of otservlist wants to cooperate and will send me api then I can reimplement it

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../GUI_Elements/GUI_Window.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_Grouper.h"
#include "../game.h"
#include "../http.h"
#include "ServerBrowser.h"

#define SERVERBROWSER_TITLE "Server Browser (Experimental)"
#define SERVERBROWSER_WIDTH 636
#define SERVERBROWSER_HEIGHT 398
#define SERVERBROWSER_CANCEL_EVENTID 1000
#define SERVERBROWSER_OK_EVENTID 1001
#define SERVERBROWSER_LIST_EVENTID 1002
#define SERVERBROWSER_IP_TEXT "IP"
#define SERVERBROWSER_IP_X 18
#define SERVERBROWSER_IP_Y 32
#define SERVERBROWSER_IP_W 100
#define SERVERBROWSER_IP_H 20
#define SERVERBROWSER_NAME_TEXT "Server Name"
#define SERVERBROWSER_NAME_X 118
#define SERVERBROWSER_NAME_Y 32
#define SERVERBROWSER_NAME_W 100
#define SERVERBROWSER_NAME_H 20
#define SERVERBROWSER_PLAYERS_TEXT "Players/Max"
#define SERVERBROWSER_PLAYERS_X 218
#define SERVERBROWSER_PLAYERS_Y 32
#define SERVERBROWSER_PLAYERS_W 100
#define SERVERBROWSER_PLAYERS_H 20
#define SERVERBROWSER_PVP_TEXT "PvP Type"
#define SERVERBROWSER_PVP_X 318
#define SERVERBROWSER_PVP_Y 32
#define SERVERBROWSER_PVP_W 100
#define SERVERBROWSER_PVP_H 20
#define SERVERBROWSER_EXP_TEXT "EXP Ratio"
#define SERVERBROWSER_EXP_X 418
#define SERVERBROWSER_EXP_Y 32
#define SERVERBROWSER_EXP_W 100
#define SERVERBROWSER_EXP_H 20
#define SERVERBROWSER_CLIENT_TEXT "Client Version"
#define SERVERBROWSER_CLIENT_X 518
#define SERVERBROWSER_CLIENT_Y 32
#define SERVERBROWSER_CLIENT_W 100
#define SERVERBROWSER_CLIENT_H 20
#define SERVERBROWSER_BROWSER_EVENTID 1003
#define SERVERBROWSER_BROWSER_X 18
#define SERVERBROWSER_BROWSER_Y 52
#define SERVERBROWSER_BROWSER_W 600
#define SERVERBROWSER_BROWSER_H 300

extern Engine g_engine;
extern Game g_game;
extern Http g_http;
extern Uint32 g_frameTime;

struct Server_List
{
	Server_List(std::string ip, std::string name, std::string type, Uint32 players, Uint32 playersMax, Uint32 expRatio, Uint32 version) : serverIp(std::move(ip)),
		serverName(std::move(name)), serverType(std::move(type)), serverPlayers(players), serverPlayersMax(playersMax), serverExpRatio(expRatio), serverVersion(version) {}

	// non-copyable
	Server_List(const Server_List&) = delete;
	Server_List& operator=(const Server_List&) = delete;

	// non-moveable
	Server_List(Server_List&& rhs) noexcept : serverIp(std::move(rhs.serverIp)), serverName(std::move(rhs.serverName)), serverType(std::move(rhs.serverType)),
		serverPlayers(rhs.serverPlayers), serverPlayersMax(rhs.serverPlayersMax), serverExpRatio(rhs.serverExpRatio), serverVersion(rhs.serverVersion) {}
	Server_List& operator=(Server_List&& rhs) noexcept
	{
		if(this != &rhs)
		{
			serverIp = std::move(rhs.serverIp);
			serverName = std::move(rhs.serverName);
			serverType = std::move(rhs.serverType);
			serverPlayers = rhs.serverPlayers;
			serverPlayersMax = rhs.serverPlayersMax;
			serverExpRatio = rhs.serverExpRatio;
			serverVersion = rhs.serverVersion;
		}
		return (*this);
	}

	std::string serverIp;
	std::string serverName;
	std::string serverType;
	Uint32 serverPlayers;
	Uint32 serverPlayersMax;
	Uint32 serverExpRatio;
	Uint32 serverVersion;
};

std::vector<Server_List> g_serverList;

Uint32 g_requestPage = 0;
Uint32 g_requestId = 0;
Uint32 g_selectedServer = SDL_static_cast(Uint32, -1);
Uint32 g_lastServerClick = 0;

void ServerBrowser_Recreate(GUI_ServerBrowserContainer* container);
void serverbrowser_Events(Uint32 event, Sint32 status)
{
	switch(event)
	{
		case SERVERBROWSER_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_SERVERBROWSER)
			{
				g_engine.removeWindow(pWindow);
				g_serverList.clear(); g_serverList.shrink_to_fit();
				if(g_requestId != 0)
				{
					g_http.removeRequest(g_requestId);
					g_requestId = 0;
				}
			}
		}
		break;
		case SERVERBROWSER_OK_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_SERVERBROWSER)
			{
				g_engine.removeWindow(pWindow);
				if(g_selectedServer != SDL_static_cast(Uint32, -1))
				{
					if(g_selectedServer < SDL_static_cast(Uint32, g_serverList.size()))
					{
						Server_List& serverList = g_serverList[g_selectedServer];
						g_engine.setClientHost(serverList.serverIp);
						g_engine.setClientPort("7171");//TODO - detect port
						g_clientVersion = serverList.serverVersion;
						g_game.clientChangeVersion(g_clientVersion, g_clientVersion);
					}
				}

				g_serverList.clear(); g_serverList.shrink_to_fit();
				if(g_requestId != 0)
				{
					g_http.removeRequest(g_requestId);
					g_requestId = 0;
				}
			}
		}
		break;
		case SERVERBROWSER_LIST_EVENTID:
		{
			if(g_requestId == SDL_static_cast(Uint32, status))
			{
				HttpRequest* request = g_http.getRequest(SDL_static_cast(Uint32, status));
				if(request) //Don't depend on result here because for some reason curl return recv error for otservlist.org even when we actually received data
				{
					SDL_snprintf(g_buffer, sizeof(g_buffer), "%sbrowser.dat", g_prefPath.c_str());
					SDL_RWops* fp = SDL_RWFromFile(g_buffer, "rb");
					if(fp)
					{
						size_t sizeData = SDL_static_cast(size_t, SDL_RWsize(fp));
						if(sizeData <= 0)
						{
							SDL_RWclose(fp);
							#ifdef SDL_FILESYSTEM_WINDOWS
							DeleteFileA(g_buffer);
							#elif HAVE_STDIO_H
							remove(g_buffer);
							#endif
							return;
						}

						std::vector<char> msgData(sizeData);
						SDL_RWread(fp, &msgData[0], 1, sizeData);
						SDL_RWclose(fp);
						#ifdef SDL_FILESYSTEM_WINDOWS
						DeleteFileA(g_buffer);
						#elif HAVE_STDIO_H
						remove(g_buffer);
						#endif

						char* readData = &msgData[0];

						//Check whether the page we are in exist
						Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "class=\"b highlight\">%u</a>", g_requestPage);
						if(UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len)) != std::string::npos)
						{
							std::string serverIp;
							std::string serverName;
							std::string serverType;
							Uint32 serverPlayers = 0;
							Uint32 serverPlayersMax = 0;
							Uint32 serverExpRatio = 0;
							Uint32 serverVersion = 0;
							while(true)
							{
								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "<th class=\"pl-15\"><a href");
								size_t searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "\">");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</a></th>");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								serverIp = std::string(readData, searchData);
								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</th><th>");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</th><th>");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								serverName = std::string(readData, searchData);
								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), " / ");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								serverPlayers = SDL_static_cast(Uint32, SDL_strtoul(readData, NULL, 10));
								readData += searchData + len;
								sizeData -= searchData + len;
								serverPlayersMax = SDL_static_cast(Uint32, SDL_strtoul(readData, NULL, 10));

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</th><th>");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</th><th>");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</th><th>");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								readData += searchData + len;
								sizeData -= searchData + len;
								serverExpRatio = SDL_static_cast(Uint32, SDL_strtoul(readData + 1, NULL, 10));

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</th><th>");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								readData += searchData + len;
								sizeData -= searchData + len;

								len = SDL_snprintf(g_buffer, sizeof(g_buffer), "</th><th>[ ");
								searchData = UTIL_Faststrstr(readData, sizeData, g_buffer, SDL_static_cast(size_t, len));
								if(searchData == std::string::npos)
									break;

								serverType = std::string(readData, searchData);
								readData += searchData + len;
								sizeData -= searchData + len;
								serverVersion = SDL_static_cast(Uint32, SDL_strtoul(readData, &readData, 10)) * 100;

								Uint32 lowerVersion = SDL_static_cast(Uint32, SDL_strtoul(readData + 1, NULL, 10));
								if(lowerVersion < 10)
									lowerVersion *= 10;

								serverVersion += lowerVersion;
								if(serverVersion != 0) // Only non-custom servers
									g_serverList.emplace_back(std::move(serverIp), std::move(serverName), std::move(serverType), serverPlayers, serverPlayersMax, serverExpRatio, serverVersion);
							}

							ServerBrowser_Recreate(NULL);
							void ServerBrowser_RequestPage(); ServerBrowser_RequestPage();
						}
					}
				}
			}
		}
		break;
		default: break;
	}
}

void ServerBrowser_RequestPage()
{
	std::stringExtended website(1024);
	website << "https://otservlist.org/list-server_players_online-desc-" << (++g_requestPage) << ".html";

	SDL_snprintf(g_buffer, sizeof(g_buffer), "%sbrowser.dat", g_prefPath.c_str());
	g_requestId = g_http.addRequest(website, g_buffer, std::string(), &serverbrowser_Events, SERVERBROWSER_LIST_EVENTID);
}

void ServerBrowser_Recreate(GUI_ServerBrowserContainer* container)
{
	std::sort(g_serverList.begin(), g_serverList.end(), [](Server_List& a, Server_List& b) -> bool {return a.serverPlayers > b.serverPlayers;});

	if(!container)
	{
		GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_SERVERBROWSER);
		if(pWindow)
			container = SDL_static_cast(GUI_ServerBrowserContainer*, pWindow->getChild(SERVERBROWSER_BROWSER_EVENTID));
		if(!container)
			return;
	}
	container->clearChilds(false);
	Sint32 PosY = -9;
	for(std::vector<Server_List>::iterator it = g_serverList.begin(), end = g_serverList.end(); it != end; ++it)
	{
		Server_List& serverList = (*it);
		GUI_ServerBrowserEntry* newServerBrowserEntry = new GUI_ServerBrowserEntry(iRect(SERVERBROWSER_IP_X - 13, PosY, SERVERBROWSER_BROWSER_W - 12, 20), SDL_static_cast(Uint32, std::distance(g_serverList.begin(), it)));
		newServerBrowserEntry->setServerIp(serverList.serverIp);
		newServerBrowserEntry->setServerName(serverList.serverName);
		SDL_snprintf(g_buffer, sizeof(g_buffer), "%u / %u", serverList.serverPlayers, serverList.serverPlayersMax);
		newServerBrowserEntry->setServerPlayers(g_buffer);
		newServerBrowserEntry->setServerType(serverList.serverType);
		SDL_snprintf(g_buffer, sizeof(g_buffer), "X%u", serverList.serverExpRatio);
		newServerBrowserEntry->setServerExp(g_buffer);
		SDL_snprintf(g_buffer, sizeof(g_buffer), "%u.%02u", (serverList.serverVersion / 100), (serverList.serverVersion % 100));
		newServerBrowserEntry->setServerVersion(g_buffer);
		newServerBrowserEntry->startEvents();
		container->addChild(newServerBrowserEntry, false);

		PosY += 20;
	}
	container->validateScrollBar();
}

void UTIL_createServerBrowser()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_SERVERBROWSER);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	g_serverList.clear();
	g_requestPage = 0;
	g_requestId = 0;
	g_selectedServer = SDL_static_cast(Uint32, -1);

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, SERVERBROWSER_WIDTH, SERVERBROWSER_HEIGHT), SERVERBROWSER_TITLE, GUI_WINDOW_SERVERBROWSER);
	GUI_Grouper* newGrouper = new GUI_Grouper(iRect(SERVERBROWSER_IP_X, SERVERBROWSER_IP_Y, SERVERBROWSER_IP_W, SERVERBROWSER_IP_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(SERVERBROWSER_NAME_X, SERVERBROWSER_NAME_Y, SERVERBROWSER_NAME_W, SERVERBROWSER_NAME_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(SERVERBROWSER_PLAYERS_X, SERVERBROWSER_PLAYERS_Y, SERVERBROWSER_PLAYERS_W, SERVERBROWSER_PLAYERS_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(SERVERBROWSER_PVP_X, SERVERBROWSER_PVP_Y, SERVERBROWSER_PVP_W, SERVERBROWSER_PVP_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(SERVERBROWSER_EXP_X, SERVERBROWSER_EXP_Y, SERVERBROWSER_EXP_W, SERVERBROWSER_EXP_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(SERVERBROWSER_CLIENT_X, SERVERBROWSER_CLIENT_Y, SERVERBROWSER_CLIENT_W, SERVERBROWSER_CLIENT_H));
	newWindow->addChild(newGrouper);
	newGrouper = new GUI_Grouper(iRect(SERVERBROWSER_BROWSER_X, SERVERBROWSER_BROWSER_Y, SERVERBROWSER_BROWSER_W, SERVERBROWSER_BROWSER_H));
	newWindow->addChild(newGrouper);
	GUI_DynamicLabel* newDynamicLabel = new GUI_DynamicLabel(iRect(SERVERBROWSER_IP_X + 5, SERVERBROWSER_IP_Y + 5, SERVERBROWSER_IP_W - 10, 12), SERVERBROWSER_IP_TEXT);
	newWindow->addChild(newDynamicLabel);
	newDynamicLabel = new GUI_DynamicLabel(iRect(SERVERBROWSER_NAME_X + 5, SERVERBROWSER_NAME_Y + 5, SERVERBROWSER_NAME_W - 10, 12), SERVERBROWSER_NAME_TEXT);
	newWindow->addChild(newDynamicLabel);
	newDynamicLabel = new GUI_DynamicLabel(iRect(SERVERBROWSER_PLAYERS_X + 5, SERVERBROWSER_PLAYERS_Y + 5, SERVERBROWSER_PLAYERS_W - 10, 12), SERVERBROWSER_PLAYERS_TEXT);
	newWindow->addChild(newDynamicLabel);
	newDynamicLabel = new GUI_DynamicLabel(iRect(SERVERBROWSER_PVP_X + 5, SERVERBROWSER_PVP_Y + 5, SERVERBROWSER_PVP_W - 10, 12), SERVERBROWSER_PVP_TEXT);
	newWindow->addChild(newDynamicLabel);
	newDynamicLabel = new GUI_DynamicLabel(iRect(SERVERBROWSER_EXP_X + 5, SERVERBROWSER_EXP_Y + 5, SERVERBROWSER_EXP_W - 10, 12), SERVERBROWSER_EXP_TEXT);
	newWindow->addChild(newDynamicLabel);
	newDynamicLabel = new GUI_DynamicLabel(iRect(SERVERBROWSER_CLIENT_X + 5, SERVERBROWSER_CLIENT_Y + 5, SERVERBROWSER_CLIENT_W - 10, 12), SERVERBROWSER_CLIENT_TEXT);
	newWindow->addChild(newDynamicLabel);
	GUI_ServerBrowserContainer* newContainer = new GUI_ServerBrowserContainer(iRect(SERVERBROWSER_BROWSER_X + 1, SERVERBROWSER_BROWSER_Y + 1, SERVERBROWSER_BROWSER_W - 2, SERVERBROWSER_BROWSER_H - 2), SERVERBROWSER_BROWSER_EVENTID);
	newContainer->startEvents();
	newWindow->addChild(newContainer);
	GUI_Button* newButton = new GUI_Button(iRect(SERVERBROWSER_WIDTH - 56, SERVERBROWSER_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&serverbrowser_Events, SERVERBROWSER_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(SERVERBROWSER_WIDTH - 109, SERVERBROWSER_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	newButton->setButtonEventCallback(&serverbrowser_Events, SERVERBROWSER_OK_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, SERVERBROWSER_HEIGHT - 40, SERVERBROWSER_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);

	ServerBrowser_RequestPage();
}

void GUI_ServerBrowserContainer::render()
{
	GUI_Container::render();

	Sint32 startX = m_tRect.x1 + SERVERBROWSER_IP_W - 2;
	auto& render = g_engine.getRender();
	render->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_SEPARATOR_X, GUI_UI_ICON_VERTICAL_SEPARATOR_Y, GUI_UI_ICON_VERTICAL_SEPARATOR_W, GUI_UI_ICON_VERTICAL_SEPARATOR_H, startX, m_tRect.y1, 2, m_tRect.y2);
	startX += SERVERBROWSER_NAME_W;
	render->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_SEPARATOR_X, GUI_UI_ICON_VERTICAL_SEPARATOR_Y, GUI_UI_ICON_VERTICAL_SEPARATOR_W, GUI_UI_ICON_VERTICAL_SEPARATOR_H, startX, m_tRect.y1, 2, m_tRect.y2);
	startX += SERVERBROWSER_PLAYERS_W;
	render->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_SEPARATOR_X, GUI_UI_ICON_VERTICAL_SEPARATOR_Y, GUI_UI_ICON_VERTICAL_SEPARATOR_W, GUI_UI_ICON_VERTICAL_SEPARATOR_H, startX, m_tRect.y1, 2, m_tRect.y2);
	startX += SERVERBROWSER_PVP_W;
	render->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_SEPARATOR_X, GUI_UI_ICON_VERTICAL_SEPARATOR_Y, GUI_UI_ICON_VERTICAL_SEPARATOR_W, GUI_UI_ICON_VERTICAL_SEPARATOR_H, startX, m_tRect.y1, 2, m_tRect.y2);
	startX += SERVERBROWSER_EXP_W;
	render->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_SEPARATOR_X, GUI_UI_ICON_VERTICAL_SEPARATOR_Y, GUI_UI_ICON_VERTICAL_SEPARATOR_W, GUI_UI_ICON_VERTICAL_SEPARATOR_H, startX, m_tRect.y1, 2, m_tRect.y2);
}

GUI_ServerBrowserEntry::GUI_ServerBrowserEntry(iRect boxRect, Uint32 internalID) :
	m_serverIp(iRect(0, 0, 0, 0), std::string()), m_serverName(iRect(0, 0, 0, 0), std::string()),
	m_serverPlayers(iRect(0, 0, 0, 0), std::string()), m_serverType(iRect(0, 0, 0, 0), std::string()),
	m_serverExp(iRect(0, 0, 0, 0), std::string()), m_serverVersion(iRect(0, 0, 0, 0), std::string())
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_ServerBrowserEntry::setRect(iRect& NewRect)
{
	m_tRect = NewRect;

	iRect nRect = iRect(NewRect.x1 + SERVERBROWSER_IP_X - 18, NewRect.y1 + 5, SERVERBROWSER_IP_W - 10, NewRect.y2 - 6);
	m_serverIp.setRect(nRect);
	nRect = iRect(NewRect.x1 + SERVERBROWSER_NAME_X - 18, NewRect.y1 + 5, SERVERBROWSER_NAME_W - 10, NewRect.y2 - 6);
	m_serverName.setRect(nRect);
	nRect = iRect(NewRect.x1 + SERVERBROWSER_PLAYERS_X - 18, NewRect.y1 + 5, SERVERBROWSER_PLAYERS_W - 10, NewRect.y2 - 6);
	m_serverPlayers.setRect(nRect);
	nRect = iRect(NewRect.x1 + SERVERBROWSER_PVP_X - 18, NewRect.y1 + 5, SERVERBROWSER_PVP_W - 10, NewRect.y2 - 6);
	m_serverType.setRect(nRect);
	nRect = iRect(NewRect.x1 + SERVERBROWSER_EXP_X - 18, NewRect.y1 + 5, SERVERBROWSER_EXP_W - 10, NewRect.y2 - 6);
	m_serverExp.setRect(nRect);
	nRect = iRect(NewRect.x1 + SERVERBROWSER_CLIENT_X - 18, NewRect.y1 + 5, SERVERBROWSER_CLIENT_W - 10, NewRect.y2 - 6);
	m_serverVersion.setRect(nRect);
}

void GUI_ServerBrowserEntry::onLMouseDown(Sint32, Sint32)
{
	if(g_frameTime < g_lastServerClick && g_selectedServer == m_internalID)
		m_doubleClicked = true;

	g_lastServerClick = g_frameTime + 1000;
	g_selectedServer = m_internalID;
}

void GUI_ServerBrowserEntry::onLMouseUp(Sint32, Sint32)
{
	if(m_doubleClicked)
	{
		m_doubleClicked = false;
		
		SDL_snprintf(g_buffer, sizeof(g_buffer), "http://%s/", m_serverIp.getName().c_str());
		UTIL_OpenURL(g_buffer);
	}
}

void GUI_ServerBrowserEntry::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(m_serverIp.getRect().isPointInside(x, y))
		m_serverIp.onMouseMove(x, y, isInsideParent);
	else if(m_serverName.getRect().isPointInside(x, y))
		m_serverName.onMouseMove(x, y, isInsideParent);
	else if(m_serverPlayers.getRect().isPointInside(x, y))
		m_serverPlayers.onMouseMove(x, y, isInsideParent);
	else if(m_serverType.getRect().isPointInside(x, y))
		m_serverType.onMouseMove(x, y, isInsideParent);
	else if(m_serverExp.getRect().isPointInside(x, y))
		m_serverExp.onMouseMove(x, y, isInsideParent);
	else if(m_serverVersion.getRect().isPointInside(x, y))
		m_serverVersion.onMouseMove(x, y, isInsideParent);
}

void GUI_ServerBrowserEntry::render()
{
	if(g_selectedServer == m_internalID)
		g_engine.getRender()->fillRectangle(m_tRect.x1 - 4, m_tRect.y1, m_tRect.x2 - 4, m_tRect.y2, 112, 112, 112, 255);

	m_serverIp.render();
	m_serverName.render();
	m_serverPlayers.render();
	m_serverType.render();
	m_serverExp.render();
	m_serverVersion.render();
}
