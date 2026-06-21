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

#ifndef __FILE_POSITION_h_
#define __FILE_POSITION_h_

#include "defines.h"

struct Position
{
	Position() : x(0), y(0), z(0) {}
	Position(Uint16 x, Uint16 y, Uint8 z) : x(x), y(y), z(z) {}
	Position(const Position& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

	template<Sint32 deltax, Sint32 deltay>
	static bool areInRange(const Position& p1, const Position& p2)
	{
		return Position::getDistanceX(p1, p2) <= deltax && Position::getDistanceY(p1, p2) <= deltay;
	}

	template<Sint32 deltax, Sint32 deltay, Sint16 deltaz>
	static bool areInRange(const Position& p1, const Position& p2)
	{
		return Position::getDistanceX(p1, p2) <= deltax && Position::getDistanceY(p1, p2) <= deltay && Position::getDistanceZ(p1, p2) <= deltaz;
	}

	static Sint32 getOffsetX(const Position& p1, const Position& p2)
	{
		return p1.x - p2.x;
	}
	static Sint32 getOffsetY(const Position& p1, const Position& p2)
	{
		return p1.y - p2.y;
	}
	static Sint16 getOffsetZ(const Position& p1, const Position& p2)
	{
		return p1.z - p2.z;
	}

	static Sint32 getDistanceX(const Position& p1, const Position& p2)
	{
		return std::abs(Position::getOffsetX(p1, p2));
	}
	static Sint32 getDistanceY(const Position& p1, const Position& p2)
	{
		return std::abs(Position::getOffsetY(p1, p2));
	}
	static Sint32 getDistanceZ(const Position& p1, const Position& p2)
	{
		return std::abs(Position::getOffsetZ(p1, p2));
	}

	Uint16 x;
	Uint16 y;
	Uint8 z;

	bool operator<(const Position& p) const
	{
		if(z < p.z)
			return true;

		if(z > p.z)
			return false;

		if(y < p.y)
			return true;

		if(y > p.y)
			return false;

		if(x < p.x)
			return true;

		if(x > p.x)
			return false;

		return false;
	}

	bool operator>(const Position& p) const
	{
		if(z > p.z)
			return true;

		if(z < p.z)
			return false;

		if(y > p.y)
			return true;

		if(y < p.y)
			return false;

		if(x > p.x)
			return true;

		if(x < p.x)
			return false;

		return false;
	}

	bool operator==(const Position& p) const
	{
		return (p.x == x && p.y == y && p.z == z);
	}

	bool operator!=(const Position& p) const
	{
		return (p.x != x || p.y != y || p.z != z);
	}

	Position operator+(const Position& p1) const
	{
		return Position(x + p1.x, y + p1.y, z + p1.z);
	}

	Position operator-(const Position& p1) const
	{
		return Position(x - p1.x, y - p1.y, z - p1.z);
	}

	Position translatedToDirection(Direction direction)
	{
        Position pos = *this;
        switch(direction)
		{
			case DIRECTION_NORTH: --pos.y; break;
			case DIRECTION_EAST: ++pos.x; break;
			case DIRECTION_SOUTH: ++pos.y; break;
			case DIRECTION_WEST: --pos.x; break;
			case DIRECTION_NORTHEAST: {++pos.x;--pos.y;} break;
			case DIRECTION_SOUTHEAST: {++pos.x;++pos.y;} break;
			case DIRECTION_SOUTHWEST: {--pos.x;++pos.y;} break;
			case DIRECTION_NORTHWEST: {--pos.x;--pos.y;} break;
			default: break;
        }
        return pos;
    }

	Position translatedToDirections(std::vector<Direction>& directions)
	{
		//In cipbia the max path tiles is 127(probably because of int8_t limit)
		size_t tileSize = UTIL_min<size_t>(0x7F, directions.size());

		Position pos = *this;
		for(size_t i = 1; i <= tileSize; ++i)
		{
			switch(directions[directions.size()-i])
			{
				case DIRECTION_NORTH: --pos.y; break;
				case DIRECTION_EAST: ++pos.x; break;
				case DIRECTION_SOUTH: ++pos.y; break;
				case DIRECTION_WEST: --pos.x; break;
				case DIRECTION_NORTHEAST: {++pos.x; --pos.y;} break;
				case DIRECTION_SOUTHEAST: {++pos.x; ++pos.y;} break;
				case DIRECTION_SOUTHWEST: {--pos.x; ++pos.y;} break;
				case DIRECTION_NORTHWEST: {--pos.x; --pos.y;} break;
				default: break;
			}
		}
		return pos;
	}

	bool up(Sint32 n = 1)
	{
		Sint32 nz = SDL_static_cast(Sint32, z) - n;
		if(nz >= 0)
		{
			z = SDL_static_cast(Uint8, nz);
			return true;
		}
		return false;
	}

	bool down(Sint32 n = 1)
	{
		Sint32 nz = SDL_static_cast(Sint32, z) + n;
		if(nz <= GAME_MAP_FLOORS)
		{
			z = SDL_static_cast(Uint8, nz);
			return true;
		}
		return false;
	}

	bool coveredUp(Sint32 n = 1)
	{
		Sint32 nx = SDL_static_cast(Sint32, x) + n, ny = SDL_static_cast(Sint32, y) + n, nz = SDL_static_cast(Sint32, z) - n;
		if(SDL_static_cast(Uint32, nx) <= 65535 && SDL_static_cast(Uint32, ny) <= 65535 && SDL_static_cast(Uint32, nz) <= GAME_MAP_FLOORS)
		{
			x = SDL_static_cast(Uint16, nx); y = SDL_static_cast(Uint16, ny); z = SDL_static_cast(Uint8, nz);
			return true;
		}
		return false;
	}

	bool coveredDown(Sint32 n = 1)
	{
		Sint32 nx = SDL_static_cast(Sint32, x) - n, ny = SDL_static_cast(Sint32, y) - n, nz = SDL_static_cast(Sint32, z) + n;
		if(SDL_static_cast(Uint32, nx) <= 65535 && SDL_static_cast(Uint32, ny) <= 65535 && SDL_static_cast(Uint32, nz) <= GAME_MAP_FLOORS)
		{
			x = SDL_static_cast(Uint16, nx); y = SDL_static_cast(Uint16, ny); z = SDL_static_cast(Uint8, nz);
			return true;
		}
		return false;
	}
};

#endif /* __FILE_POSITION_h_ */

