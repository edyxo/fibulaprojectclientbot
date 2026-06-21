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

#include "animatedText.h"
#include "engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

AnimatedText::AnimatedText(const Position& pos, Uint8 color, Sint32 additionalOffset, const std::string& text)
{
	setText(text);
	m_position = pos;

	m_startTime = g_frameTime;
	m_additionalOffset = additionalOffset;

	m_color = color;
	colorFrom8bit(m_color, m_red, m_green, m_blue);
}

bool AnimatedText::canBeDeleted()
{
	if((g_frameTime - m_startTime) >= ANIMATEDTEXT_STAY_TIME)
		return true;
	else
		return false;
}

bool AnimatedText::merge(Uint8 color, const std::string& text)
{
	if(m_color != color)
		return false;

	if((g_frameTime - m_startTime) >= (ANIMATEDTEXT_STAY_TIME / 4))
		return false;

	bool currentDmgNegative = false;
	bool newDmgNegative = false;
	Sint32 currentDmg = 0;
	Sint32 newDmg = 0;
	for(size_t i = 0, len = text.length(); i < len; ++i)
	{
		char character = text[i];
		if(character == '-')
			newDmgNegative = true;
		else if(character >= 48 && character <= 57)
		{
			Sint32 v = SDL_static_cast(Sint32, character-'0');
			newDmg *= 10;
			newDmg += v;
		}
		else
			return false;
	}
	for(size_t i = 0, len = m_text.length(); i < len; ++i)
	{
		char character = m_text[i];
		if(character == '-')
			currentDmgNegative = true;
		else if(character >= 48 && character <= 57)
		{
			Sint32 v = SDL_static_cast(Sint32, character-'0');
			currentDmg *= 10;
			currentDmg += v;
		}
		else
			return false;
	}
	if(newDmgNegative)
		newDmg *= -1;
	if(currentDmgNegative)
		currentDmg *= -1;
	Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%d", currentDmg + newDmg);
	setText(std::string(g_buffer, SDL_static_cast(size_t, len)));
	return true;
}

bool AnimatedText::needAdditionalOffset()
{
	if((g_frameTime - m_startTime) >= (ANIMATEDTEXT_STAY_TIME / 4))
		return false;

	return true;
}

void AnimatedText::setText(const std::string text)
{
	m_text = std::move(text);
	m_cachedHalfTextSize = g_engine.calculateFontWidth(CLIENT_FONT_OUTLINED, m_text) / 2;
}

void AnimatedText::render(Sint32 posX, Sint32 posY, Sint32 boundLeft, Sint32 boundTop, Sint32 boundRight, Sint32 boundBottom)
{
	static const Uint32 tf = ANIMATEDTEXT_STAY_TIME;

	Uint32 t = (g_frameTime - m_startTime);
	posY -= (-8 + (48 * t) / tf);
	posY += m_additionalOffset;
	if(posX - SDL_static_cast(Sint32, m_cachedHalfTextSize + 1) <= boundLeft)
		posX = boundLeft + m_cachedHalfTextSize + 1;
	if(posY <= boundTop)
		posY = boundTop + 1;
	if(posX + SDL_static_cast(Sint32, m_cachedHalfTextSize + 1) >= boundRight)
		posX = boundRight - m_cachedHalfTextSize - 1;
	if(posY + 14 >= boundBottom)
		posY = boundBottom - 15;

	g_engine.drawFont(CLIENT_FONT_OUTLINED, posX - m_cachedHalfTextSize, posY, m_text, m_red, m_green, m_blue, CLIENT_FONT_ALIGN_LEFT);
}
