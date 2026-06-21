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

#ifndef __FILE_ANIMATEDTEXT_h_
#define __FILE_ANIMATEDTEXT_h_

#include "position.h"

class AnimatedText
{
	public:
		AnimatedText(const Position& pos, Uint8 color, Sint32 additionalOffset, const std::string& text);

		// non-copyable
		AnimatedText(const AnimatedText&) = delete;
		AnimatedText& operator=(const AnimatedText&) = delete;

		// non-moveable
		AnimatedText(AnimatedText&&) = delete;
		AnimatedText& operator=(AnimatedText&&) = delete;

		SDL_INLINE const Position& getPosition() {return m_position;}
		bool canBeDeleted();
		bool needAdditionalOffset();

		bool merge(Uint8 color, const std::string& text);
		void setText(const std::string text);
		void render(Sint32 posX, Sint32 posY, Sint32 boundLeft, Sint32 boundTop, Sint32 boundRight, Sint32 boundBottom);

	protected:
		std::string m_text;
		Position m_position;

		Uint32 m_startTime;
		Uint32 m_cachedHalfTextSize;
		Sint32 m_additionalOffset;

		Uint8 m_color;
		Uint8 m_red;
		Uint8 m_green;
		Uint8 m_blue;
};

#endif /* __FILE_ANIMATEDTEXT_h_ */
