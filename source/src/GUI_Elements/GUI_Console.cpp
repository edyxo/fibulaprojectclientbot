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

#include "GUI_Console.h"
#include "../engine.h"

#include <chrono>

extern Engine g_engine;
extern Uint32 g_frameTime;

GUI_Console::GUI_Console(iRect boxRect, Uint32 internalID) : m_scrollBar(iRect(0, 0, 0, 0), 0, 0)
{
	setRect(boxRect);
	m_internalID = internalID;
	m_sText.reserve(MAX_CONSOLE_MESSAGES * 255);//Pre-allocate a little space
}

void GUI_Console::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	iRect nRect = iRect(NewRect.x1 + NewRect.x2 - 13, NewRect.y1 + 1, 12, NewRect.y2 - 2);
	m_scrollBar.setRect(nRect);
	m_tRect = NewRect;
	m_maxDisplay = m_tRect.y2 / 14;
	m_needUpdate = true;
	m_keepLastScrollPos = (m_scrollBar.getScrollPos() == m_scrollBar.getScrollSize());
}

void GUI_Console::clearConsole()
{
	m_messageIndex = 0;
	m_scrollBar.setScrollSize(0);
	m_scrollBar.setScrollPos(0);
	m_messages.clear();
	m_needUpdate = true;
}

void GUI_Console::selectAll()
{
	setSelection(0, SDL_static_cast(Uint32, m_sText.length()));
}

void GUI_Console::setSelection(Uint32 start, Uint32 end)
{
	if(start == m_selectionStart && end == m_selectionEnd)
		return;

	if(start > end)
		std::swap(start, end);

	m_selectionStart = UTIL_max<Uint32>(0, UTIL_min<Uint32>(SDL_static_cast(Uint32, m_sText.length()), start));
	m_selectionEnd = UTIL_max<Uint32>(0, UTIL_min<Uint32>(SDL_static_cast(Uint32, m_sText.length()), end));
	m_needUpdateSelection = true;
}

void GUI_Console::clearSelection()
{
	if(!hasSelection())
		return;

	m_selectionStart = 0;
	m_selectionEnd = 0;
	m_needUpdateSelection = true;
}

void GUI_Console::onKeyDown(SDL_Event& event)
{
	switch(event.key.keysym.sym)
	{
		case SDLK_LEFT:
		{
			if(event.key.keysym.mod == KMOD_SHIFT)
			{
				if(!hasSelection())
					m_selectionReference = m_cursorPosition;

				moveCursor(-1);
				setSelection(m_selectionReference, m_cursorPosition);
			}
			else
			{
				clearSelection();
				moveCursor(-1);
			}
		}
		break;
		case SDLK_RIGHT:
		{
			if(event.key.keysym.mod == KMOD_SHIFT)
			{
				if(!hasSelection())
					m_selectionReference = m_cursorPosition;

				moveCursor(1);
				setSelection(m_selectionReference, m_cursorPosition);
			}
			else
			{
				clearSelection();
				moveCursor(1);
			}
		}
		break;
		case SDLK_HOME:
		{
			if(event.key.keysym.mod == KMOD_SHIFT)
			{
				if(m_cursorPosition != 0)
				{
					setSelection(m_cursorPosition, 0);
					setCursor(0);
				}
			}
			else
			{
				if(m_cursorPosition != 0)
				{
					clearSelection();
					setCursor(0);
				}
			}
		}
		break;
		case SDLK_END:
		{
			if(event.key.keysym.mod == KMOD_SHIFT)
			{
				if(m_cursorPosition != SDL_static_cast(Uint32, m_sText.length()))
				{
					setSelection(m_cursorPosition, SDL_static_cast(Uint32, m_sText.length()));
					setCursor(SDL_static_cast(Uint32, m_sText.length()));
				}
			}
			else
			{
				if(m_cursorPosition != SDL_static_cast(Uint32, m_sText.length()))
				{
					clearSelection();
					setCursor(SDL_static_cast(Uint32, m_sText.length()));
				}
			}
		}
		break;
		case SDLK_UP:
		{
			if(event.key.keysym.mod == KMOD_SHIFT)
			{
				if(m_cursorPosition != 0)
				{
					if(!hasSelection())
						m_selectionReference = m_cursorPosition;

					moveCursorVertically(-1);
					setSelection(m_selectionReference, m_cursorPosition);
				}
			}
			else
			{
				if(m_cursorPosition != 0)
				{
					clearSelection();
					moveCursorVertically(-1);
				}
			}
		}
		break;
		case SDLK_DOWN:
		{
			if(event.key.keysym.mod == KMOD_SHIFT)
			{
				if(m_cursorPosition != SDL_static_cast(Uint32, m_sText.length()))
				{
					if(!hasSelection())
						m_selectionReference = m_cursorPosition;

					moveCursorVertically(1);
					setSelection(m_selectionReference, m_cursorPosition);
				}
			}
			else
			{
				if(m_cursorPosition != SDL_static_cast(Uint32, m_sText.length()))
				{
					clearSelection();
					moveCursorVertically(1);
				}
			}
		}
		break;
		case SDLK_a:
		{
			if(event.key.keysym.mod == KMOD_CTRL)
			{
				m_selectionReference = 0;
				setSelection(m_selectionReference, SDL_static_cast(Uint32, m_sText.length()));
				setCursor(SDL_static_cast(Uint32, m_sText.length()));
			}
		}
		break;
		case SDLK_x:
		{
			if(event.key.keysym.mod == KMOD_CTRL)
			{
				if(hasSelection())
					UTIL_SetClipboardTextLatin1(UTIL_formatConsoleText(m_sText.substr(SDL_static_cast(size_t, m_selectionStart), SDL_static_cast(size_t, m_selectionEnd - m_selectionStart))).c_str());
			}
		}
		break;
		case SDLK_c:
		{
			if(event.key.keysym.mod == KMOD_CTRL)
			{
				if(hasSelection())
					UTIL_SetClipboardTextLatin1(UTIL_formatConsoleText(m_sText.substr(SDL_static_cast(size_t, m_selectionStart), SDL_static_cast(size_t, m_selectionEnd - m_selectionStart))).c_str());
			}
		}
		break;
		default: break;
	}
}

void GUI_Console::onLMouseDown(Sint32 x, Sint32 y)
{
	if(m_scrollBar.getRect().isPointInside(x, y))
	{
		m_scrollBar.onLMouseDown(x, y);
		return;
	}
	m_selecting = true;
	m_cursorPosition = getCursorPosition(x, y);

	m_selectionReference = m_cursorPosition;
	setSelection(m_selectionReference, m_cursorPosition);
}

void GUI_Console::onLMouseUp(Sint32 x, Sint32 y)
{
	m_scrollBar.onLMouseUp(x, y);
	m_selecting = false;
}

void GUI_Console::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	m_scrollBar.onMouseMove(x, y, isInsideParent);
	if(m_selecting)
	{
		m_cursorPosition = getCursorPosition(x, y);
		setSelection(m_selectionReference, m_cursorPosition);
	}
}

void GUI_Console::onWheel(Sint32, Sint32, bool wheelUP)
{
	if(wheelUP)
		m_scrollBar.setScrollPos(m_scrollBar.getScrollPos() - 1);
	else
		m_scrollBar.setScrollPos(m_scrollBar.getScrollPos() + 1);
}

void GUI_Console::deActivate()
{
	m_bActive = false;
	clearSelection();
}

void GUI_Console::update()
{
	struct PARSE_Console
	{
		Uint64 m_index;
		GUI_Console* m_console;
		Uint8 m_red;
		Uint8 m_green;
		Uint8 m_blue;
		bool m_firstLine;
	};

	m_sText.clear();
	m_lines.clear();

	bool firstMessage = true;
	for(std::list<ConsoleMessage>::iterator it = m_messages.begin(), end = m_messages.end(); it != end; ++it)
	{
		ConsoleMessage& cMessage = (*it);
		if(firstMessage)
			firstMessage = false;
		else
			m_sText << '\n';

		size_t start = m_sText.length();
		if(g_engine.hasShowTimestamps())
		{
			if(!cMessage.name.empty())
			{
				if(g_engine.hasShowLevels() && cMessage.level > 0)
					m_sText << cMessage.timestamp << ' ' << cMessage.name << '[' << cMessage.level << "]: " << cMessage.message;
				else
					m_sText << cMessage.timestamp << ' ' << cMessage.name << ": " << cMessage.message;
			}
			else
				m_sText << cMessage.timestamp << ' ' << cMessage.message;
		}
		else
		{
			if(!cMessage.name.empty())
			{
				if(g_engine.hasShowLevels() && cMessage.level > 0)
					m_sText << cMessage.name << '[' << cMessage.level << "]: " << cMessage.message;
				else
					m_sText << cMessage.name << ": " << cMessage.message;
			}
			else
				m_sText << cMessage.message;
		}
		
		PARSE_Console consoleParser;
		consoleParser.m_console = this;
		consoleParser.m_index = cMessage.messageId;
		consoleParser.m_red = cMessage.red;
		consoleParser.m_green = cMessage.green;
		consoleParser.m_blue = cMessage.blue;
		consoleParser.m_firstLine = true;

		UTIL_parseSizedText(m_sText, start, m_font, SDL_static_cast(Uint32, m_tRect.x2 - 28), reinterpret_cast<void*>(&consoleParser), [](void* __THIS, bool, size_t start, size_t length) -> size_t
		{
			PARSE_Console* _THIS = reinterpret_cast<PARSE_Console*>(__THIS);

			_THIS->m_console->m_lines.emplace_back(_THIS->m_index, start, length, SDL_static_cast(Uint32, _THIS->m_firstLine ? 4 : 14), _THIS->m_red, _THIS->m_green, _THIS->m_blue);
			_THIS->m_firstLine = false;
			return 0;
		});

		if(m_sText[m_sText.length() - 1] == '\n')
			m_lines.emplace_back(consoleParser.m_index, m_sText.length(), SDL_static_cast(size_t, 1), SDL_static_cast(Uint32, consoleParser.m_firstLine ? 4 : 14), consoleParser.m_red, consoleParser.m_green, consoleParser.m_blue);
	}

	m_scrollBar.setScrollSize(SDL_static_cast(Sint32, m_lines.size()) - m_maxDisplay);
	if(hasSelection())
		m_needUpdateSelection = true;
}

void GUI_Console::updatePartially()
{
	struct PARSE_Console
	{
		Uint64 m_index;
		GUI_Console* m_console;
		Uint8 m_red;
		Uint8 m_green;
		Uint8 m_blue;
		bool m_firstLine;
	};

	if(m_messages.empty())
	{
		//Make sure we don't get crash by accessing front from empty list - shouldn't happen but just in case
		m_messageIndex = 0;
		m_sText.clear();
		m_lines.clear();

		m_scrollBar.setScrollSize(SDL_static_cast(Sint32, m_lines.size()) - m_maxDisplay);
		if(hasSelection())
			m_needUpdateSelection = true;

		return;
	}

	Uint64 startIndex = m_messages.front().messageId, endIndex = 0;
	if(!m_lines.empty())
	{
		endIndex = m_lines.back().messageId;
		if(m_lines.front().messageId != startIndex)
		{
			size_t adjustStartPosition = 0;
			for(std::vector<ConsoleLine>::iterator it = m_lines.begin(), end = m_lines.end(); it != end; ++it)
			{
				ConsoleLine& messageLine = (*it);
				if(messageLine.messageId == startIndex)
				{
					adjustStartPosition = messageLine.lineStart;
					m_lines.erase(m_lines.begin(), it);
					goto Skip_Line_Clear;
				}
			}
			m_lines.clear();

			Skip_Line_Clear:
			for(std::vector<ConsoleLine>::iterator it = m_lines.begin(), end = m_lines.end(); it != end; ++it)
			{
				ConsoleLine& messageLine = (*it);
				messageLine.lineStart -= adjustStartPosition;
			}
		}
	}

	m_sText.clear();

	bool firstMessage = true;
	for(std::list<ConsoleMessage>::iterator it = m_messages.begin(), end = m_messages.end(); it != end; ++it)
	{
		ConsoleMessage& cMessage = (*it);
		if(firstMessage)
			firstMessage = false;
		else
			m_sText << '\n';

		size_t start = m_sText.length();
		if(g_engine.hasShowTimestamps())
		{
			if(!cMessage.name.empty())
			{
				if(g_engine.hasShowLevels() && cMessage.level > 0)
					m_sText << cMessage.timestamp << ' ' << cMessage.name << '[' << cMessage.level << "]: " << cMessage.message;
				else
					m_sText << cMessage.timestamp << ' ' << cMessage.name << ": " << cMessage.message;
			}
			else
				m_sText << cMessage.timestamp << ' ' << cMessage.message;
		}
		else
		{
			if(!cMessage.name.empty())
			{
				if(g_engine.hasShowLevels() && cMessage.level > 0)
					m_sText << cMessage.name << '[' << cMessage.level << "]: " << cMessage.message;
				else
					m_sText << cMessage.name << ": " << cMessage.message;
			}
			else
				m_sText << cMessage.message;
		}
		
		if(cMessage.messageId > endIndex)
		{
			PARSE_Console consoleParser;
			consoleParser.m_console = this;
			consoleParser.m_index = cMessage.messageId;
			consoleParser.m_red = cMessage.red;
			consoleParser.m_green = cMessage.green;
			consoleParser.m_blue = cMessage.blue;
			consoleParser.m_firstLine = true;

			UTIL_parseSizedText(m_sText, start, m_font, SDL_static_cast(Uint32, m_tRect.x2 - 28), reinterpret_cast<void*>(&consoleParser), [](void* __THIS, bool, size_t start, size_t length) -> size_t
			{
				PARSE_Console* _THIS = reinterpret_cast<PARSE_Console*>(__THIS);

				_THIS->m_console->m_lines.emplace_back(_THIS->m_index, start, length, SDL_static_cast(Uint32, _THIS->m_firstLine ? 4 : 14), _THIS->m_red, _THIS->m_green, _THIS->m_blue);
				_THIS->m_firstLine = false;
				return 0;
			});

			if(m_sText[m_sText.length() - 1] == '\n')
				m_lines.emplace_back(consoleParser.m_index, m_sText.length(), SDL_static_cast(size_t, 1), SDL_static_cast(Uint32, consoleParser.m_firstLine ? 4 : 14), consoleParser.m_red, consoleParser.m_green, consoleParser.m_blue);
		}
	}

	m_scrollBar.setScrollSize(SDL_static_cast(Sint32, m_lines.size()) - m_maxDisplay);
	if(hasSelection())
		m_needUpdateSelection = true;
}

void GUI_Console::updateSelection()
{
	Sint32 vectorPos = 0, cursorPos = 0;
	for(std::vector<ConsoleLine>::iterator it = m_lines.begin(), end = m_lines.end(); it != end; ++it)
	{
		ConsoleLine& currentLine = (*it);
		Uint32 selectionStart = UTIL_max<Uint32>(SDL_static_cast(Uint32, currentLine.lineStart), m_selectionStart);
		Uint32 selectionLen = UTIL_min<Uint32>(SDL_static_cast(Uint32, currentLine.lineStart + currentLine.lineLength), m_selectionEnd) - selectionStart;
		if(selectionLen <= SDL_static_cast(Uint32, currentLine.lineLength))
		{
			Uint32 selectionWidth = g_engine.calculateFontWidth(m_font, m_sText, selectionStart, selectionLen);
			Uint32 positionOfSelection = currentLine.startPosition + g_engine.calculateFontWidth(m_font, m_sText, currentLine.lineStart, selectionStart - currentLine.lineStart);
			currentLine.selectionStart = SDL_static_cast(Sint32, positionOfSelection);
			currentLine.selectionWidth = SDL_static_cast(Sint32, selectionWidth);
		}
		else
			currentLine.selectionWidth = 0;

		size_t position = SDL_static_cast(size_t, m_cursorPosition) - currentLine.lineStart;
		if(position < currentLine.lineLength)
			cursorPos = vectorPos;

		++vectorPos;
	}

	//Keep us in the cursor sight
	Sint32 actualPos = m_scrollBar.getScrollPos();
	if(actualPos > cursorPos)
		m_scrollBar.setScrollPos(cursorPos);
	else if(actualPos < cursorPos - m_maxDisplay + 1)
		m_scrollBar.setScrollPos(cursorPos - m_maxDisplay + 1);
}

void GUI_Console::render()
{
	if((g_engine.hasShowTimestamps() != m_showTimestamps) || (g_engine.hasShowLevels() != m_showLevels))
	{
		m_needUpdate = true;
		m_showTimestamps = g_engine.hasShowTimestamps();
		m_showLevels = g_engine.hasShowLevels();
	}
	if(m_needUpdatePartially)
	{
		if(!m_needUpdate)
			updatePartially();

		m_needUpdatePartially = false;
	}
	if(m_needUpdate)
	{
		update();
		m_needUpdate = false;
	}
	if(m_needUpdateSelection)
	{
		updateSelection();
		m_needUpdateSelection = false;
	}
	if(m_keepLastScrollPos)
	{
		m_scrollBar.setScrollPos(m_scrollBar.getScrollSize());
		m_keepLastScrollPos = false;
	}

	auto& renderer = g_engine.getRender();
	m_scrollBar.render();

	Sint32 count = m_scrollBar.getScrollPos();
	std::vector<ConsoleLine>::iterator it = m_lines.begin();
	std::advance(it, count);

	Sint32 messagesVisible = UTIL_min<Sint32>(m_maxDisplay, SDL_static_cast(Sint32, m_lines.size()));
	Sint32 posY = m_tRect.y1 + m_tRect.y2 - (messagesVisible * 14);
	Sint32 endY = m_tRect.y1 + m_tRect.y2 - 14;
	for(std::vector<ConsoleLine>::iterator end = m_lines.end(); it != end; ++it)
	{
		if(posY > endY)
			break;

		ConsoleLine& currentLine = (*it);
		if(currentLine.selectionWidth != 0)
			renderer->fillRectangle(m_tRect.x1 + currentLine.selectionStart, posY - 1, currentLine.selectionWidth, 14, 128, 128, 128, 255);

		g_engine.drawFont(m_font, m_tRect.x1 + currentLine.startPosition, posY, m_sText, currentLine.red, currentLine.green, currentLine.blue, CLIENT_FONT_ALIGN_LEFT, currentLine.lineStart, currentLine.lineLength);
		posY += 14;
	}
}

void GUI_Console::addMessage(Uint32 statementId, time_t timestamp, const std::string name, Uint16 level, const std::string message, Uint8 red, Uint8 green, Uint8 blue, Uint32 flags)
{
	m_messages.emplace_back(++m_messageIndex, std::move(name), std::move(message), std::move(UTIL_formatDate("%H:%M", timestamp)), flags, statementId, level, red, green, blue);
	if(m_messages.size() > MAX_CONSOLE_MESSAGES)
		m_messages.pop_front();
	m_needUpdatePartially = true;
	m_keepLastScrollPos = (m_scrollBar.getScrollPos() == m_scrollBar.getScrollSize());
}

void GUI_Console::setCursor(Uint32 position)
{
	m_cursorPosition = UTIL_max<Uint32>(0, UTIL_min<Uint32>(SDL_static_cast(Uint32, m_sText.length()), position));
	m_needUpdateSelection = true;
}

void GUI_Console::moveCursor(Sint32 n)
{
	Sint32 pos = UTIL_max<Sint32>(0, SDL_static_cast(Sint32, m_cursorPosition) + n);
	setCursor(SDL_static_cast(Uint32, pos));
}

void GUI_Console::moveCursorVertically(Sint32 n)
{
	if(m_lines.size() < 2)
		return;

	size_t positionInLine = 0;
	std::vector<ConsoleLine>::iterator it = m_lines.begin(), end = m_lines.end();
	for(; it != end; ++it)
	{
		ConsoleLine& currentLine = (*it);
		size_t position = SDL_static_cast(size_t, m_cursorPosition) - currentLine.lineStart;
		if(position < currentLine.lineLength)
		{
			positionInLine = position;
			break;
		}
	}
	if(n > 0)
	{
		for(Sint32 i = 0; i < n; ++i)
		{
			if(++it == end)
			{
				--it;
				break;
			}
		}
		if((*it).lineLength <= positionInLine)
			positionInLine = (*it).lineLength - 1;
		setCursor(SDL_static_cast(Uint32, (*it).lineStart + positionInLine));
	}
	else if(n < 0)
	{
		n *= -1;
		end = m_lines.begin();
		if(it == end)
			return;
		for(Sint32 i = 0; i < n; ++i)
		{
			if(--it == end)
				break;
		}
		if((*it).lineLength <= positionInLine)
			positionInLine = (*it).lineLength - 1;
		setCursor(SDL_static_cast(Uint32, (*it).lineStart + positionInLine));
	}
}

Uint32 GUI_Console::getCursorPosition(Sint32 x, Sint32 y)
{
	if(m_lines.empty())
		return 0;

	Sint32 count = m_scrollBar.getScrollPos();
	std::vector<ConsoleLine>::iterator it = m_lines.begin();
	std::advance(it, count);

	Sint32 messagesVisible = UTIL_min<Sint32>(m_maxDisplay, SDL_static_cast(Sint32, m_lines.size()));
	Sint32 posY = m_tRect.y1 + m_tRect.y2 - (messagesVisible * 14);
	Sint32 endY = m_tRect.y1 + m_tRect.y2 - 14;
	if(y < posY)
		return SDL_static_cast(Uint32, (*it).lineStart);

	std::vector<ConsoleLine>::iterator end = m_lines.end();
	for(; it != end; ++it)
	{
		ConsoleLine& currentLine = (*it);
		if(SDL_static_cast(Uint32, y - posY) < 14)
		{
			Sint32 lastPosition = 0;
			for(size_t i = 0; i <= currentLine.lineLength; ++i)
			{
				Uint32 loopTextWidth = g_engine.calculateFontWidth(m_font, m_sText, currentLine.lineStart, i);
				Sint32 position = m_tRect.x1 + currentLine.startPosition + loopTextWidth;
				if(x - position < 0)
				{
					if(SDL_abs(x - position) < SDL_abs(x - lastPosition))
						return SDL_static_cast(Uint32, currentLine.lineStart + i);
					else
						return SDL_static_cast(Uint32, currentLine.lineStart + i - 1);
				}
				lastPosition = position;
			}
			return SDL_static_cast(Uint32, currentLine.lineStart + currentLine.lineLength - 1);
		}
		posY += 14;
		if(posY > endY)
			break;
	}
	if(it == end)
		return SDL_static_cast(Uint32, m_sText.length());
	return SDL_static_cast(Uint32, (*it).lineStart + (*it).lineLength - 1);
}

ConsoleMessage* GUI_Console::getConsoleMessage(Sint32, Sint32 y)
{
	if(m_lines.empty())
		return NULL;

	Sint32 count = m_scrollBar.getScrollPos();
	std::vector<ConsoleLine>::iterator it = m_lines.begin();
	std::advance(it, count);

	Sint32 messagesVisible = UTIL_min<Sint32>(m_maxDisplay, SDL_static_cast(Sint32, m_lines.size()));
	Sint32 posY = m_tRect.y1 + m_tRect.y2 - (messagesVisible * 14);
	Sint32 endY = m_tRect.y1 + m_tRect.y2 - 14;
	if(y < posY)
		return &m_messages.front();

	std::vector<ConsoleLine>::iterator end = m_lines.end();
	for(; it != end; ++it)
	{
		if(SDL_static_cast(Uint32, y - posY) < 14)
			break;

		posY += 14;
		if(posY > endY)
			break;
	}
	if(it == end)
		return &m_messages.back();

	ConsoleLine& currentLine = (*it);
	for(std::list<ConsoleMessage>::iterator mit = m_messages.begin(), mend = m_messages.end(); mit != mend; ++mit)
	{
		ConsoleMessage& message = (*mit);
		if(message.messageId == currentLine.messageId)
			return &message;
	}
	return NULL;
}
