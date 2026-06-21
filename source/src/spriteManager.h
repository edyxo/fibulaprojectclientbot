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

#ifndef __FILE_SPRITEMANAGER_h_
#define __FILE_SPRITEMANAGER_h_

#include "defines.h"

struct SpriteData
{
	unsigned char data[4096];
	bool bgra;
};

struct SpriteSheet
{
	SpriteSheet(std::string spriteFile, Uint32 firstSpriteId, Uint32 lastSpriteId, Uint32 spriteType) :
		spriteFile(std::move(spriteFile)), firstSpriteId(firstSpriteId), lastSpriteId(lastSpriteId), spriteType(spriteType) {}

	// non-copyable
	SpriteSheet(const SpriteSheet&) = delete;
	SpriteSheet& operator=(const SpriteSheet&) = delete;

	// move-constructible
	SpriteSheet(SpriteSheet&& rhs) noexcept : spriteFile(std::move(rhs.spriteFile)), firstSpriteId(rhs.firstSpriteId), lastSpriteId(rhs.lastSpriteId), spriteType(rhs.spriteType) {}
	SpriteSheet& operator=(SpriteSheet&&) = delete;

	std::string spriteFile;
	Uint32 firstSpriteId;
	Uint32 lastSpriteId;
	Uint32 spriteType;
};

struct SpriteOffset
{
	Uint32 spriteIndex;
	Uint32 spriteType;
};

class SpriteManager
{
	public:
		SpriteManager() = default;
		~SpriteManager();

		// non-copyable
		SpriteManager(const SpriteManager&) = delete;
		SpriteManager& operator=(const SpriteManager&) = delete;

		// non-moveable
		SpriteManager(SpriteManager&&) = delete;
		SpriteManager& operator=(SpriteManager&&) = delete;

		void unloadSprites();
		void manageSprites(std::vector<Uint32>& fromSprites, std::vector<Uint32>& toSprites, Uint8& width, Uint8& height);
		SDL_INLINE bool isSprLoaded() {return m_sprLoaded;}

		unsigned char* LoadSpriteSheet_BMP(const std::string& spriteFile, size_t& outputSize);
		void SplitSpriteSheet(SDL_Surface* sheet, unsigned char* destData, Sint32 x, Sint32 y);
		bool LoadSpriteSheet(Uint32 spriteId, bool bgra);
		unsigned char* LoadSprite_NEW(Uint32 spriteId, bool bgra);

		unsigned char* LoadSprite_RGBA(Uint32 spriteId);
		unsigned char* LoadSprite_BGRA(Uint32 spriteId);

		bool loadSprites(const char* filename);
		bool loadCatalog(const char* filename);

	private:
		std::map<Uint32, SpriteData> m_spriteData;
		std::vector<SpriteSheet> m_spriteSheets;
		std::vector<SpriteOffset> m_spriteOffsets;
		SDL_RWops* m_cachedSprites = NULL;

		bool m_sprLoaded = false;
		bool m_sprCached = false;
};

#endif /* __FILE_SPRITEMANAGER_h_ */
