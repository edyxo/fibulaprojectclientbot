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

#ifndef __FILE_AUTOMAP_h_
#define __FILE_AUTOMAP_h_

#include "position.h"

//1024 max tiles = ~128MB of ram
#define AUTOMAP_MAXTILES 1024

struct MapMark
{
	MapMark(Uint16 x, Uint16 y, Uint8 type, std::string text) : text(std::move(text)), x(x), y(y), type(type) {}

	// non-copyable
	MapMark(const MapMark&) = delete;
	MapMark& operator=(const MapMark&) = delete;

	// moveable
	MapMark(MapMark&& rhs) noexcept : text(std::move(rhs.text)), x(rhs.x), y(rhs.y), type(rhs.type) {}
	MapMark& operator=(MapMark&& rhs) noexcept
	{
		if(this != &rhs)
		{
			text = std::move(rhs.text);
			x = rhs.x;
			y = rhs.y;
			type = rhs.type;
		}
		return (*this);
	}

	std::string text;
	Uint16 x, y;
	Uint8 type;
};

class AutomapArea
{
	public:
		AutomapArea(Uint16 x, Uint16 y, Uint8 z, Uint32 area);
		~AutomapArea();

		// non-copyable
		AutomapArea(const AutomapArea&) = delete;
		AutomapArea& operator=(const AutomapArea&) = delete;

		// moveable
		AutomapArea(AutomapArea&& rhs) noexcept :
			m_fileName(std::move(rhs.m_fileName)), m_marks(std::move(rhs.m_marks)), m_basepos(std::move(rhs.m_basepos)), m_currentArea(rhs.m_currentArea), m_recreate(rhs.m_recreate)
		{
			memcpy(m_color, rhs.m_color, sizeof(m_color));
			memcpy(m_speed, rhs.m_speed, sizeof(m_speed));
		}
		AutomapArea& operator=(AutomapArea&& rhs) noexcept
		{
			if(this != &rhs)
			{
				m_fileName = std::move(rhs.m_fileName);
				m_marks = std::move(rhs.m_marks);
				m_basepos = std::move(rhs.m_basepos);
				m_currentArea = rhs.m_currentArea;
				m_recreate = rhs.m_recreate;
				memcpy(m_color, rhs.m_color, sizeof(m_color));
				memcpy(m_speed, rhs.m_speed, sizeof(m_speed));
			}
			return (*this);
		}
		
		MapMark* getMark(Sint32 zoom, Sint32 diff, Sint32 x, Sint32 y, Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2);
		void render(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh);
		static void renderMark(Uint8 type, Sint32 x, Sint32 y);
		void renderMarks(Sint32 zoom, Sint32 diff, Sint32 x, Sint32 y, Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2);
		void setMapMark(Uint16 x, Uint16 y, Uint8 type, const std::string& description, bool remove = false);
		void setTileDetail(Uint16 x, Uint16 y, Uint8 color, Uint8 speed);

		SDL_INLINE Position& getBasePosition() {return m_basepos;}
		SDL_INLINE Uint8 getColor(Uint16 x, Uint16 y) {return m_color[x & 0xFF][y & 0xFF];}
		SDL_INLINE Uint8 getSpeed(Uint16 x, Uint16 y) {return m_speed[x & 0xFF][y & 0xFF];}

		bool load();
		bool save();

	protected:
		std::string m_fileName;
		std::vector<MapMark> m_marks;
		Position m_basepos;

		Uint32 m_currentArea;
		bool m_recreate;

		Uint8 m_color[256][256];
		Uint8 m_speed[256][256];
};

typedef robin_hood::unordered_map<Uint32, AutomapArea> AutomapAreas;

class Automap
{
	public:
		Automap();

		// non-copyable
		Automap(const Automap&) = delete;
		Automap& operator=(const Automap&) = delete;

		// non-moveable
		Automap(Automap&&) = delete;
		Automap& operator=(Automap&&) = delete;

		AutomapArea* getArea(Uint16 x, Uint16 y, Uint8 z);

		void setCentralPosition(const Position& pos);
		void setPosition(const Position& pos) {m_position = pos;}

		Position& getCentralPosition() {return m_centerPosition;}
		Position& getPosition() {return m_position;}

		Position getMapDetail(Sint32 x, Sint32 y, Sint32 w, Sint32 h, MapMark*& mark);
		void setMapMark(Uint16 x, Uint16 y, Uint8 z, Uint8 type, const std::string& description, bool remove = false);
		void setTileDetail(Uint16 x, Uint16 y, Uint8 z, Uint8 color, Uint8 speed);
		void render(Sint32 x, Sint32 y, Sint32 w, Sint32 h);

		Uint8 getColor(const Position& pos);
		Uint8 getSpeed(const Position& pos);

		Sint32 getZoom() {return m_zoom;}
		void setZoom(Sint32 zoom);
		void zoomOut();
		void zoomIn();

	protected:
		std::vector<AutomapArea*> cachedAreas;
		AutomapAreas m_areas;
		Position m_centerPosition;
		Position m_position;
		Uint32 m_currentArea;
		Sint32 m_zoom;
		Sint32 m_diff;
};

#endif /* __FILE_AUTOMAP_h_ */
