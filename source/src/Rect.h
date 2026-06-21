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

#ifndef __FILE_RECT_h_
#define __FILE_RECT_h_

#include "defines.h"

struct iRect
{
	iRect() : x1(0), y1(0), x2(0), y2(0) {}
	iRect(Sint32 x, Sint32 y, Sint32 x2, Sint32 y2) : x1(x), y1(y), x2(x2), y2(y2) {}
	iRect(const iRect& other) : x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2) {}

	iRect operator+(const iRect& other) const
	{
		iRect ret(*this);
		ret.x1 += other.x1;
		ret.y1 += other.y1;
		ret.x2 += other.x2;
		ret.y2 += other.y2;
		return ret;
	}

	const iRect& operator+=(const iRect& other)
	{
		x1 += other.x1;
		y1 += other.y1;
		x2 += other.x2;
		y2 += other.y2;
		return *this;
	}

	iRect operator-(const iRect& other) const
	{
		iRect ret(*this);
		ret.x1 -= other.x1;
		ret.y1 -= other.y1;
		ret.x2 -= other.x2;
		ret.y2 -= other.y2;
		return ret;
	}

	const iRect& operator-=(const iRect& other)
	{
		x1 -= other.x1;
		y1 -= other.y1;
		x2 -= other.x2;
		y2 -= other.y2;
		return *this;
	}

	bool operator==(const iRect& other) const
	{
		return (x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2);
	}

	bool operator!=(const iRect& other) const
	{
		return (x1 != other.x1 && y1 != other.y1 && x2 != other.x2 && y2 != other.y2);
	}

	const iRect& operator=(const iRect& other)
	{
		x1 = other.x1;
		y1 = other.y1;
		x2 = other.x2;
		y2 = other.y2;
		return *this;
	}

	bool isPointInside(const Sint32 posX, const Sint32 posY) const
	{
		#if !defined(__ALLOW_WINDOWS_OUTBOUNDS__)
		return (SDL_static_cast(Uint32, posX - x1) < SDL_static_cast(Uint32, x2) && SDL_static_cast(Uint32, posY - y1) < SDL_static_cast(Uint32, y2));
		#else
		return (x1 <= posX && y1 <= posY && x1 + x2 >= posX && y1 + y2 >= posY);
		#endif
	}

	Sint32 getWidth() const {return x2;}
	Sint32 getHeight() const {return y2;}

	Sint32 x1, y1, x2, y2;
};

#endif
