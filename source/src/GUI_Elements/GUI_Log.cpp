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

#include "GUI_Log.h"
#include "GUI_Console.h"
#include "GUI_TextBox.h"
#include "../LuaRuntime.h"
#include "../engine.h"

GUI_Log g_logger;
extern Engine g_engine;

GUI_Log::GUI_Log()
{
	m_console = new GUI_Console(iRect(0, 0, 0, 0));
	m_input = new GUI_TextBox(iRect(0, 0, 0, 0), "");
	m_input->setMaxLength(2048);
	m_input->startEvents();
	m_input->activate();
	m_luaRuntime = new LuaRuntime();
}

GUI_Log::~GUI_Log()
{
	delete m_luaRuntime;
	delete m_input;
	delete m_console;
}

void GUI_Log::addLog(Sint32 category, const std::string& text)
{
	switch(category)
	{
		case LOG_CATEGORY_INFO: m_console->addMessage(0, time(NULL), "Information", 0, text, 255, 255, 255); break;
		case LOG_CATEGORY_WARNING: m_console->addMessage(0, time(NULL), "Warning", 0, text, 255, 255, 0); break;
		case LOG_CATEGORY_ERROR: m_console->addMessage(0, time(NULL), "Error", 0, text, 255, 0, 0); break;
		default: break;
	}
}

void GUI_Log::onTextInput(const char* textInput)
{
	m_input->onTextInput(textInput);
}

void GUI_Log::onKeyDown(SDL_Event& event)
{
	if((event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) &&
	   (event.key.keysym.mod & (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT)) == 0)
	{
		executeInput();
		return;
	}

	if(event.key.keysym.sym == SDLK_UP &&
	   (event.key.keysym.mod & (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT)) == 0)
	{
		selectHistory(-1);
		return;
	}

	if(event.key.keysym.sym == SDLK_DOWN &&
	   (event.key.keysym.mod & (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT)) == 0)
	{
		selectHistory(1);
		return;
	}

	if(event.key.keysym.sym == SDLK_l && (event.key.keysym.mod & KMOD_CTRL))
	{
		m_console->clearConsole();
		return;
	}

	if(event.key.keysym.sym == SDLK_c && (event.key.keysym.mod & KMOD_CTRL) && m_console->hasSelection())
	{
		m_console->onKeyDown(event);
		return;
	}

	if(event.key.keysym.sym == SDLK_v && (event.key.keysym.mod & KMOD_CTRL))
	{
		executeClipboardCommands();
		return;
	}

	m_input->onKeyDown(event);
}

void GUI_Log::onKeyUp(SDL_Event& event)
{
	m_console->onKeyUp(event);
}

void GUI_Log::onLMouseDown(Sint32 x, Sint32 y)
{
	if(m_input->getRect().isPointInside(x, y))
	{
		m_console->clearSelection();
		m_input->activate();
		m_input->onLMouseDown(x, y);
	}
	else
	{
		m_input->deActivate();
		m_console->onLMouseDown(x, y);
	}
}

void GUI_Log::onLMouseUp(Sint32 x, Sint32 y)
{
	m_console->onLMouseUp(x, y);
	m_input->onLMouseUp(x, y);
}

void GUI_Log::onRMouseDown(Sint32 x, Sint32 y)
{
	if(!m_input->getRect().isPointInside(x, y))
		m_console->onRMouseDown(x, y);
}

void GUI_Log::onRMouseUp(Sint32 x, Sint32 y)
{
	if(!m_input->getRect().isPointInside(x, y))
		m_console->onRMouseUp(x, y);
}

void GUI_Log::onWheel(Sint32 x, Sint32 y, bool wheelUP)
{
	m_console->onWheel(x, y, wheelUP);
}

void GUI_Log::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	bool inputInside = isInsideParent && m_input->getRect().isPointInside(x, y);
	m_console->onMouseMove(x, y, isInsideParent && !inputInside);
	m_input->onMouseMove(x, y, inputInside);
}

void GUI_Log::render(Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
	auto& renderer = g_engine.getRender();
	renderer->fillRectangle(x, y, w - 12, h, 0, 0, 0, 128);

	const Sint32 inputHeight = 18;
	const Sint32 inputY = y + h - inputHeight - 5;
	iRect nRect = iRect(x, y, w, UTIL_max<Sint32>(0, h - inputHeight - 10));
	m_console->setRect(nRect);
	m_console->render();

	g_engine.drawFont(CLIENT_FONT_NONOUTLINED, x + 5, inputY + 3, "Lua>", 80, 220, 120, CLIENT_FONT_ALIGN_LEFT);
	nRect = iRect(x + 38, inputY, UTIL_max<Sint32>(0, w - 55), inputHeight);
	m_input->setRect(nRect);
	m_input->render();
}

void GUI_Log::executeInput()
{
	std::string code = m_input->getActualText();
	if(code.empty())
		return;

	m_input->setText("");
	m_input->activate();
	executeCode(code);
}

void GUI_Log::executeCode(const std::string& code)
{
	if(m_history.empty() || m_history.back() != code)
	{
		m_history.push_back(code);
		if(m_history.size() > 100)
			m_history.erase(m_history.begin());
	}
	m_historyIndex = m_history.size();

	addLog(LOG_CATEGORY_INFO, std::string("> ").append(code));
	std::vector<std::string> output;
	std::string error;
	if(!m_luaRuntime->execute(code, output, error))
	{
		addLog(LOG_CATEGORY_ERROR, std::string("[Lua] ").append(error));
		return;
	}

	for(const std::string& line : output)
		addLog(LOG_CATEGORY_INFO, std::string("[Lua] ").append(line));
}

void GUI_Log::executeClipboardCommands()
{
	char* clipboardText = UTIL_GetClipboardTextLatin1();
	if(!clipboardText)
		return;

	std::string pastedText(clipboardText);
	SDL_free(clipboardText);

	if(pastedText.find_first_of("\r\n") == std::string::npos)
	{
		std::string inputText = m_input->getActualText();
		inputText.append(pastedText);
		if(inputText.length() > 2048)
			inputText.resize(2048);
		m_input->setText(inputText);
		m_input->activate();
		return;
	}

	m_input->setText("");
	m_input->activate();

	size_t start = 0;
	while(start <= pastedText.length())
	{
		size_t end = pastedText.find_first_of("\r\n", start);
		std::string command = pastedText.substr(start, end == std::string::npos ? std::string::npos : end - start);
		if(command.find_first_not_of(" \t") != std::string::npos)
			executeCode(command);

		if(end == std::string::npos)
			break;

		start = end + 1;
		if(pastedText[end] == '\r' && start < pastedText.length() && pastedText[start] == '\n')
			++start;
	}
}

void GUI_Log::selectHistory(Sint32 offset)
{
	if(m_history.empty())
		return;

	Sint32 next = SDL_static_cast(Sint32, m_historyIndex) + offset;
	next = UTIL_max<Sint32>(0, UTIL_min<Sint32>(SDL_static_cast(Sint32, m_history.size()), next));
	m_historyIndex = SDL_static_cast(size_t, next);
	if(m_historyIndex == m_history.size())
		m_input->setText("");
	else
		m_input->setText(m_history[m_historyIndex]);
	m_input->activate();
}
