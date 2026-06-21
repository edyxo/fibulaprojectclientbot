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

#include "screenText.h"
#include "engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

void ScreenText::addMessage(MessageMode mode, std::string text)
{
	Uint32 extraTime = g_frameTime;
	if(m_messages.size() >= SDL_static_cast(size_t, (m_screenPosition == ONSCREEN_MESSAGE_TOP ? 10 : 1)))
	{
		m_dirty = true;
		m_messages.erase(m_messages.begin());
	}
	else if(m_messages.empty())
		m_dirty = true;
	else
		extraTime = m_messages.back().m_endTime;

	ScreenTextMessage newMessage;
	newMessage.m_message = std::move(text);
	newMessage.m_endTime = extraTime + UTIL_max<Uint32>(STATICTEXT_PER_CHAR_STAY_DURATION * SDL_static_cast(Uint32, text.length()), STATICTEXT_MINIMUM_STAY_DURATION);
	newMessage.m_mode = mode;
	m_messages.push_back(newMessage);
}

void ScreenText::composeText()
{
	ScreenTextMessage& currentMessage = m_messages.front();
	m_mode = currentMessage.m_mode;
	m_text = currentMessage.m_message;
	m_dirty = false;
	m_messagesLine.clear();
	switch(m_mode)
	{
		case MessageWarning:
		case MessageGamemasterBroadcast:
		case MessageGamemasterPrivateFrom:
		{
			m_red = 250;
			m_green = 96;
			m_blue = 96;
		}
		break;
		case MessageFailure:
		case MessageLogin:
		case MessageTutorialHint:
		case MessageBeyondLast:
		case MessageGame:
		case MessageReport:
		case MessageThankyou:
		case MessageStatus:
		{
			m_red = 255;
			m_green = 255;
			m_blue = 255;
		}
		break;
		case MessageLook:
		case MessageLoot:
		case MessageTradeNpc:
		case MessageHotkeyUse:
		{
			m_red = 0;
			m_green = 240;
			m_blue = 0;
		}
		break;
		case MessagePrivateFrom:
		{
			m_red = 96;
			m_green = 250;
			m_blue = 250;
		}
		break;
		default: return;
	}

	m_textWidth = 0;
	UTIL_parseSizedText(m_text, 0, CLIENT_FONT_OUTLINED, m_allowedWidth, reinterpret_cast<void*>(this), [](void* __THIS, bool skipLine, size_t start, size_t length) -> size_t
	{
		ScreenText* _THIS = reinterpret_cast<ScreenText*>(__THIS);
		if(skipLine)
			_THIS->m_text.insert(start + length - 1, 1, '-');

		ScreenTextMessageLine newLine;
		newLine.lineStart = start;
		newLine.lineLength = length;
		newLine.lineWidth = g_engine.calculateFontWidth(CLIENT_FONT_OUTLINED, _THIS->m_text, newLine.lineStart, newLine.lineLength) / 2;
		_THIS->m_textWidth = UTIL_max<Uint32>(_THIS->m_textWidth, newLine.lineWidth);
		_THIS->m_messagesLine.push_back(newLine);
		return (skipLine ? 1 : 0);
	});
}

bool ScreenText::update()
{
	ScreenTextMessage& currentMessage = m_messages.front();
	if(g_frameTime >= currentMessage.m_endTime)
	{
		m_messages.erase(m_messages.begin());
		if(m_messages.empty())
			return false;
		else
			composeText();
	}
	else if(m_dirty)
		composeText();

	return true;
}

void ScreenText::render(Sint32 boundLeft, Sint32 boundTop, Sint32 boundRight, Sint32 boundBottom)
{
	if(m_messages.empty())
		return;

	if(m_screenPosition == ONSCREEN_MESSAGE_BOTTOM)
	{
		Uint32 width = SDL_static_cast(Uint32, boundRight - boundLeft);
		if(m_allowedWidth != width)
		{
			m_allowedWidth = width;
			m_dirty = true;
		}
	}
	if(!update())
		return;

	Sint32 posX, posY;
	switch(m_screenPosition)
	{
		case ONSCREEN_MESSAGE_BOTTOM:
		{
			posX = boundLeft + (boundRight - boundLeft) / 2;
			posY = boundBottom - (SDL_static_cast(Sint32, m_messagesLine.size()) * 14);
		}
		break;
		case ONSCREEN_MESSAGE_CENTER_LOW:
		{
			Sint32 height = SDL_static_cast(Sint32, m_messagesLine.size()) * 14;
			posX = boundLeft + (boundRight - boundLeft) / 2;
			posY = (boundTop + (boundBottom - boundTop) / 2) + height + 14;
			if(posY <= boundTop)
				posY = boundTop + 1;
			if(posY + height >= boundBottom)
				posY = boundBottom - height - 1;
		}
		break;
		case ONSCREEN_MESSAGE_CENTER_HIGH:
		{
			Sint32 height = SDL_static_cast(Sint32, m_messagesLine.size()) * 14;
			posX = boundLeft + (boundRight - boundLeft) / 2;
			posY = (boundTop + (boundBottom - boundTop) / 2) - height - 14;
			if(posY <= boundTop)
				posY = boundTop + 1;
			if(posY + height >= boundBottom)
				posY = boundBottom - height - 1;
		}
		break;
		default:
		{
			Sint32 height = SDL_static_cast(Sint32, m_messagesLine.size()) * 14;
			posX = boundLeft + (boundRight - boundLeft) / 2;
			posY = boundTop + (SDL_static_cast(Sint32, (boundBottom - boundTop) * 0.25f)) + height;
			if(posY <= boundTop)
				posY = boundTop + 1;
			if(posY + height >= boundBottom)
				posY = boundBottom - height - 1;
		}
		break;
	}
	for(size_t i = 0; i < m_messagesLine.size(); ++i)
	{
		ScreenTextMessageLine& currentLine = m_messagesLine[i];
		g_engine.drawFont(CLIENT_FONT_OUTLINED, posX - currentLine.lineWidth, posY, m_text, m_red, m_green, m_blue, CLIENT_FONT_ALIGN_LEFT, currentLine.lineStart, currentLine.lineLength);
		posY += 14;
	}
}
