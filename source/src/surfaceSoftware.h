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

#ifndef __FILE_SURFACE_SOFTWARE_h_
#define __FILE_SURFACE_SOFTWARE_h_

#include "engine.h"

struct PictureOptimization
{
	PictureOptimization() : m_surface(NULL) {}
	PictureOptimization(Sint32 x, Sint32 y, Sint32 w, Sint32 h) : m_surface(NULL), m_x(x), m_y(y), m_width(w), m_height(h) {}

	SDL_Surface* m_surface;
	Sint32 m_x;
	Sint32 m_y;
	Sint32 m_width;
	Sint32 m_height;
};

struct SoftwareSpriteData
{
	SoftwareSpriteData() : m_surface(NULL), m_lastUsage(0) {}

	SDL_Surface* m_surface;
	Uint32 m_lastUsage;
};

typedef robin_hood::unordered_map<Uint32, PictureOptimization> U32BOptimizer;
typedef robin_hood::unordered_map<Uint32, SDL_Surface*> U32BSurfaces;
typedef robin_hood::unordered_map<Uint64, SoftwareSpriteData> U64BSurfaces;

class SurfaceSoftware : public Surface
{
	public:
		SurfaceSoftware();
		virtual ~SurfaceSoftware();

		// non-copyable
		SurfaceSoftware(const SurfaceSoftware&) = delete;
		SurfaceSoftware& operator=(const SurfaceSoftware&) = delete;

		// non-moveable
		SurfaceSoftware(const SurfaceSoftware&&) = delete;
		SurfaceSoftware& operator=(const SurfaceSoftware&&) = delete;

		virtual bool isSupported();
		SDL_Surface* convertSurface(SDL_Surface* s);
		virtual const char* getName() {return "Software";}
		virtual const char* getSoftware() {return "Softrast 1.0";}
		virtual const char* getHardware() {return m_hardware;}
		virtual Uint32 getVRAM() {return m_totalVRAM;}

		virtual void init();
		virtual void doResize(Sint32 w, Sint32 h);
		virtual void spriteManagerReset();
		virtual unsigned char* getScreenPixels(Sint32& width, Sint32& height, bool& bgra);

		virtual void beginScene();
		virtual void endScene();

		bool integer_scaling(Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
		virtual void drawLightMap_old(LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height);
		virtual void drawLightMap_new(LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height);
		virtual void drawGameScene(Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
		virtual void beginGameScene();
		virtual void endGameScene();

		virtual void setClipRect(Sint32 x, Sint32 y, Sint32 w, Sint32 h);
		virtual void disableClipRect();
		virtual void drawRectangle(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 lineWidth, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		virtual void fillRectangle(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

		SDL_Surface* loadPicture(SDL_Surface* s, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh);
		SDL_Surface* loadPicture(Uint16 pictureId, SDL_BlendMode blendMode);
		virtual void drawFont(Uint16 pictureId, Sint32 x, Sint32 y, const std::string& text, size_t pos, size_t len, Uint8 r, Uint8 g, Uint8 b, Sint16 cX[256], Sint16 cY[256], Sint16 cW[256], Sint16 cH[256]);
		virtual void drawBackground(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
		virtual void drawPictureRepeat(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
		virtual void drawPicture(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 x, Sint32 y, Sint32 w, Sint32 h);

		SDL_Surface* loadSprite(Uint64 tempPos, Uint32 spriteId);
		SDL_Surface* loadSpriteMask(Uint64 tempPos, Uint32 spriteId, Uint32 maskSpriteId, Uint32 outfitColor);
		virtual void drawSprite(Uint32 spriteId, Sint32 x, Sint32 y);
		virtual void drawSprite(Uint32 spriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh);
		virtual void drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Uint32 outfitColor);
		virtual void drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Uint32 outfitColor);

		SDL_Surface* createAutomapTile(Uint32 currentArea);
		void uploadAutomapTile(SDL_Surface* surface, Uint8 color[256][256]);
		virtual void drawAutomapTile(Uint32 currentArea, bool& recreate, Uint8 color[256][256], Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh);

	protected:
		U32BOptimizer m_pictureOptimizations;
		U32BSurfaces m_automapTiles;
		U64BSurfaces m_sprites;
		std::circular_buffer<Uint32, MAX_AUTOMAPTILES> m_automapTilesBuff;
		std::circular_buffer<Uint64, MAX_SPRITES> m_spritesIds;

		SDL_Surface** m_pictures = NULL;
		char* m_hardware = NULL;

		SDL_Surface* m_convertSurface = NULL;
		SDL_Surface* m_gameWindow = NULL;
		SDL_Surface* m_renderSurface = NULL;
		SDL_Surface* m_background = NULL;
		SDL_Surface* m_scaled_gameWindow = NULL;

		Uint32 m_totalVRAM = 0;
		Uint32 m_spriteChecker = 0;
		Uint32 m_currentFrame = 0;
		Uint32 m_convertFormat = SDL_PIXELFORMAT_UNKNOWN;

		Sint32 m_integer_scaling_width = 0;
		Sint32 m_integer_scaling_height = 0;

		bool m_useConvertSurface = false;
};

#endif /* __FILE_SURFACE_SOFTWARE_h_ */
