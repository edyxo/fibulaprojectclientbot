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

#include "GUI_MultiTextBox.h"
#include "../engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

GUI_MultiTextBox::GUI_MultiTextBox(iRect boxRect, bool allowEdit, const std::string text, Uint32 internalID, Uint8 red, Uint8 green, Uint8 blue) : 
	m_sText(std::move(text)), m_scrollBar(iRect(0, 0, 0, 0), 0, 0), m_red(red), m_green(green), m_blue(blue), m_allowEdit(allowEdit)
{
	setRect(boxRect);
	m_internalID = internalID;
	m_cursorTimer = g_frameTime;
}

void GUI_MultiTextBox::setRect(iRect& NewRect)
{
	if(m_tRect == NewRect)
		return;

	iRect nRect = iRect(NewRect.x1 + NewRect.x2 - 13, NewRect.y1 + 1, 12, NewRect.y2 - 2);
	m_scrollBar.setRect(nRect);
	m_tRect = NewRect;
	m_maxDisplay = (m_tRect.y2 - 8) / 14;
	m_needUpdate = true;
}

void GUI_MultiTextBox::setTextEventCallback(void (*eventHandlerFunction)(Uint32,Sint32), Uint32 mEvent)
{
	m_eventHandlerFunction = eventHandlerFunction;
	m_evtParam = mEvent;
}

void GUI_MultiTextBox::selectAll()
{
	setSelection(0, SDL_static_cast(Uint32, m_sText.length()));
}

void GUI_MultiTextBox::setSelection(Uint32 start, Uint32 end)
{
	if(start == m_selectionStart && end == m_selectionEnd)
		return;

	if(start > end)
		std::swap(start, end);

	m_selectionStart = UTIL_max<Uint32>(0, UTIL_min<Uint32>(SDL_static_cast(Uint32, m_sText.length()), start));
	m_selectionEnd = UTIL_max<Uint32>(0, UTIL_min<Uint32>(SDL_static_cast(Uint32, m_sText.length()), end));
	m_needUpdateSelection = true;
}

void GUI_MultiTextBox::clearSelection()
{
	if(!hasSelection())
		return;

	m_selectionStart = 0;
	m_selectionEnd = 0;
	m_needUpdateSelection = true;
}

void GUI_MultiTextBox::deleteSelection()
{
	Sint32 n = SDL_static_cast(Sint32, m_selectionEnd - m_selectionStart);
	if(n <= 0)
	{
		clearSelection();
		return;
	}

	removeTextToTextBox(n, m_selectionStart);
	if(m_cursorPosition != m_selectionStart)
		moveCursor(-n);
	clearSelection();
}

void GUI_MultiTextBox::onTextInput(const char* textInput)
{
	if(!m_allowEdit)
		return;
	if(hasSelection())
		deleteSelection();

	size_t len = SDL_strlen(textInput);
	Sint32 n = addTextToTextBox(std::string(textInput, len), m_cursorPosition);
	if(n != 0)
		moveCursor(n);
}

void GUI_MultiTextBox::onKeyDown(SDL_Event& event)
{
	switch(event.key.keysym.sym)
	{
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		{
			if(event.key.keysym.mod == KMOD_NONE && m_allowEdit)
			{
				if(hasSelection())
					deleteSelection();

				Sint32 n = addTextToTextBox(std::string(1, '\n'), m_cursorPosition);
				if(n != 0)
					moveCursor(n);
			}
		}
		break;
		case SDLK_BACKSPACE:
		{
			if(event.key.keysym.mod == KMOD_NONE && m_allowEdit)
			{
				if(hasSelection())
					deleteSelection();
				else if(m_cursorPosition != 0)
				{
					removeTextToTextBox(1, m_cursorPosition - 1);
					moveCursor(-1);
				}
			}
		}
		break;
		case SDLK_DELETE:
		{
			if(event.key.keysym.mod == KMOD_NONE && m_allowEdit)
			{
				if(hasSelection())
					deleteSelection();
				else if(m_cursorPosition != m_sText.length())
					removeTextToTextBox(1, m_cursorPosition);
			}
		}
		break;
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
				{
					UTIL_SetClipboardTextLatin1(UTIL_formatConsoleText(m_sText.substr(SDL_static_cast(size_t, m_selectionStart), SDL_static_cast(size_t, m_selectionEnd - m_selectionStart))).c_str());
					if(m_allowEdit)
						deleteSelection();
				}
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
		case SDLK_v:
		{
			if(event.key.keysym.mod == KMOD_CTRL && m_allowEdit)
			{
				if(hasSelection())
					deleteSelection();

				char* latin1 = UTIL_GetClipboardTextLatin1();
				if(latin1)
				{
					size_t len = SDL_strlen(latin1);
					Sint32 n = addTextToTextBox(std::string(latin1, len), m_cursorPosition);
					if(n != 0)
						moveCursor(n);
				}
			}
		}
		break;
		default: break;
	}
}

void GUI_MultiTextBox::onLMouseDown(Sint32 x, Sint32 y)
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

void GUI_MultiTextBox::onLMouseUp(Sint32 x, Sint32 y)
{
	m_scrollBar.onLMouseUp(x, y);
	m_selecting = false;
}

void GUI_MultiTextBox::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	m_scrollBar.onMouseMove(x, y, isInsideParent);
	if(m_selecting)
	{
		m_cursorPosition = getCursorPosition(x, y);
		setSelection(m_selectionReference, m_cursorPosition);
	}
}

void GUI_MultiTextBox::onWheel(Sint32, Sint32, bool wheelUP)
{
	if(wheelUP)
		m_scrollBar.setScrollPos(m_scrollBar.getScrollPos() - 1);
	else
		m_scrollBar.setScrollPos(m_scrollBar.getScrollPos() + 1);
}

void GUI_MultiTextBox::activate()
{
	m_bActive = true;
	if(m_allowEdit && SDL_HasScreenKeyboardSupport() && !SDL_IsTextInputActive())
		SDL_StartTextInput();

	m_cursorTimer = g_frameTime;
	m_bShowCursor = true;
}

void GUI_MultiTextBox::deActivate()
{
	m_bActive = false;
	if(m_allowEdit && SDL_HasScreenKeyboardSupport() && SDL_IsTextInputActive())
		SDL_StopTextInput();

	clearSelection();
}

void GUI_MultiTextBox::update()
{
	m_lines.clear();
	if(m_sText.empty())
		return;
	
	UTIL_parseSizedText(m_sText, 0, m_font, SDL_static_cast(Uint32, m_tRect.x2 - 18), reinterpret_cast<void*>(this), [](void* __THIS, bool, size_t start, size_t length) -> size_t
	{
		GUI_MultiTextBox* _THIS = reinterpret_cast<GUI_MultiTextBox*>(__THIS);

		MultiLine newLine;
		newLine.selectionWidth = 0;
		newLine.lineStart = start;
		newLine.lineLength = length;
		_THIS->m_lines.push_back(newLine);
		return 0;
	});

	if(m_sText[m_sText.length() - 1] == '\n')
	{
		MultiLine newLine;
		newLine.lineStart = m_sText.length();
		newLine.lineLength = 1;
		newLine.selectionWidth = 0;
		m_lines.push_back(newLine);
	}

	m_scrollBar.setScrollSize(SDL_static_cast(Sint32, m_lines.size()) - m_maxDisplay);
	if(hasSelection())
		m_needUpdateSelection = true;
}

void GUI_MultiTextBox::updateSelection()
{
	Sint32 vectorPos = 0, cursorPos = 0;
	for(std::vector<MultiLine>::iterator it = m_lines.begin(), end = m_lines.end(); it != end; ++it)
	{
		MultiLine& currentLine = (*it);
		Uint32 selectionStart = UTIL_max<Uint32>(SDL_static_cast(Uint32, currentLine.lineStart), m_selectionStart);
		Uint32 selectionLen = UTIL_min<Uint32>(SDL_static_cast(Uint32, currentLine.lineStart + currentLine.lineLength), m_selectionEnd) - selectionStart;
		if(selectionLen <= SDL_static_cast(Uint32, currentLine.lineLength))
		{
			Uint32 selectionWidth = g_engine.calculateFontWidth(m_font, m_sText, selectionStart, selectionLen);
			Uint32 positionOfSelection = m_textStartPosition + g_engine.calculateFontWidth(m_font, m_sText, currentLine.lineStart, selectionStart - currentLine.lineStart);
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

void GUI_MultiTextBox::render()
{
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
	if(g_frameTime - m_cursorTimer >= 333)
	{
		m_cursorTimer = g_frameTime;
		m_bShowCursor = !m_bShowCursor;
	}

	auto& renderer = g_engine.getRender();
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_DARK_X, GUI_UI_ICON_HORIZONTAL_LINE_DARK_Y, GUI_UI_ICON_HORIZONTAL_LINE_DARK_W, GUI_UI_ICON_HORIZONTAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1, m_tRect.x2, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_DARK_X, GUI_UI_ICON_VERTICAL_LINE_DARK_Y, GUI_UI_ICON_VERTICAL_LINE_DARK_W, GUI_UI_ICON_VERTICAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1 + 1, 1, m_tRect.y2 - 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_X, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_Y, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_W, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_H, m_tRect.x1 + 1, m_tRect.y1 + m_tRect.y2 - 1, m_tRect.x2 - 1, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_X, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_Y, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_W, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_H, m_tRect.x1 + m_tRect.x2 - 1, m_tRect.y1 + 1, 1, m_tRect.y2 - 2);
	renderer->fillRectangle(m_tRect.x1 + 1, m_tRect.y1 + 1, m_tRect.x2 - 14, m_tRect.y2 - 2, 54, 54, 54, 255);
	m_scrollBar.render();

	Sint32 count = m_scrollBar.getScrollPos();
	std::vector<MultiLine>::iterator it = m_lines.begin();
	std::advance(it, count);

	Sint32 posY = m_tRect.y1 + 4;
	Sint32 endY = m_tRect.y1 + m_tRect.y2 - 18;
	if(m_lines.empty())
	{
		if(m_allowEdit && m_bActive && m_bShowCursor)
			renderer->fillRectangle(m_tRect.x1 + m_textStartPosition, posY - 1, 1, 10, 255, 255, 255, 255);

		return;
	}
	for(std::vector<MultiLine>::iterator end = m_lines.end(); it != end; ++it)
	{
		if(posY > endY)
			break;

		MultiLine& currentLine = (*it);
		if(currentLine.selectionWidth != 0)
			renderer->fillRectangle(m_tRect.x1 + currentLine.selectionStart, posY - 1, currentLine.selectionWidth, 14, 128, 128, 128, 255);

		g_engine.drawFont(m_font, m_tRect.x1 + m_textStartPosition, posY, m_sText, m_red, m_green, m_blue, CLIENT_FONT_ALIGN_LEFT, currentLine.lineStart, currentLine.lineLength);
		if(m_allowEdit && m_bActive && m_bShowCursor)
		{
			size_t position = SDL_static_cast(size_t, m_cursorPosition) - currentLine.lineStart;
			if(position < currentLine.lineLength)
			{
				Uint32 positionOfCursor = m_textStartPosition + g_engine.calculateFontWidth(m_font, m_sText, currentLine.lineStart, position);
				renderer->fillRectangle(m_tRect.x1 + positionOfCursor, posY - 1, 1, 10, 255, 255, 255, 255);
			}
		}
		posY += 14;
	}
}

Sint32 GUI_MultiTextBox::addTextToTextBox(std::string text, Uint32 position)
{
	Uint32 remainLen = m_maxLength - SDL_static_cast(Uint32, m_sText.length());
	if(remainLen == 0)
		return 0;
	else
	{
		if(SDL_static_cast(Uint32, text.length()) > remainLen)
			text.resize(SDL_static_cast(size_t, remainLen));
	}

	Sint32 addedCharacters = 0;
	for(size_t i = 0, len = text.length(); i < len; ++i)
	{
		char character = text[i];
		if(character >= 32 || character == '\n')
		{
			m_sText.insert(position, 1, character);
			++position;
			++addedCharacters;
		}
	}

	if(m_eventHandlerFunction)
		UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, 1);
	m_needUpdate = true;
	return addedCharacters;
}

void GUI_MultiTextBox::removeTextToTextBox(Sint32 nbr, Uint32 position)
{
	m_sText.erase(position, nbr);
	if(m_eventHandlerFunction)
		UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, 2);
	m_needUpdate = true;
}

void GUI_MultiTextBox::setText(const std::string text)
{
	m_sText.clear();
	for(size_t i = 0, len = text.length(); i < len; ++i)
	{
		char character = text[i];
		if(character >= 32 || character == '\n')
			m_sText.append(1, character);
	}

	if(SDL_static_cast(Uint32, text.length()) > m_maxLength)
		m_sText.resize(SDL_static_cast(size_t, m_maxLength));

	if(m_eventHandlerFunction)
		UTIL_SafeEventHandler(m_eventHandlerFunction, m_evtParam, 0);
	m_needUpdate = true;
}

void GUI_MultiTextBox::setCursor(Uint32 position)
{
	m_cursorPosition = UTIL_max<Uint32>(0, UTIL_min<Uint32>(SDL_static_cast(Uint32, m_sText.length()), position));
	m_cursorTimer = g_frameTime;
	m_bShowCursor = true;
	m_needUpdateSelection = true;
}

void GUI_MultiTextBox::moveCursor(Sint32 n)
{
	Sint32 pos = UTIL_max<Sint32>(0, SDL_static_cast(Sint32, m_cursorPosition) + n);
	setCursor(SDL_static_cast(Uint32, pos));
}

void GUI_MultiTextBox::moveCursorVertically(Sint32 n)
{
	if(m_lines.size() < 2)
		return;

	size_t positionInLine = 0;
	std::vector<MultiLine>::iterator it = m_lines.begin(), end = m_lines.end();
	for(; it != end; ++it)
	{
		MultiLine& currentLine = (*it);
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

Uint32 GUI_MultiTextBox::getCursorPosition(Sint32 x, Sint32 y)
{
	if(m_lines.empty())
		return 0;

	Sint32 count = m_scrollBar.getScrollPos();
	std::vector<MultiLine>::iterator it = m_lines.begin();
	std::advance(it, count);

	Sint32 posY = m_tRect.y1 + 4;
	Sint32 endY = m_tRect.y1 + m_tRect.y2 - 18;
	if(y < posY)
		return SDL_static_cast(Uint32, (*it).lineStart);

	std::vector<MultiLine>::iterator end = m_lines.end();
	for(; it != end; ++it)
	{
		MultiLine& currentLine = (*it);
		if(SDL_static_cast(Uint32, y - posY) < 14)
		{
			Sint32 lastPosition = 0;
			for(size_t i = 0; i <= currentLine.lineLength; ++i)
			{
				Uint32 loopTextWidth = g_engine.calculateFontWidth(m_font, m_sText, currentLine.lineStart, i);
				Sint32 position = m_tRect.x1 + m_textStartPosition + loopTextWidth;
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
