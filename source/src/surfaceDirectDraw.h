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

#ifndef __FILE_SURFACE_DIRECTDRAW_h_
#define __FILE_SURFACE_DIRECTDRAW_h_

#include "engine.h"

#if defined(SDL_VIDEO_RENDER_DDRAW)

#define DIRECTDRAW_MAX_VERTICES 2048
#define DIRECTDRAW_MAX_INDICES (DIRECTDRAW_MAX_VERTICES * 6 / 4)
#if DIRECTDRAW_MAX_INDICES >= 65500
#error "DirectDraw doesn't support uint indices"
#endif

struct VertexDDRAW
{
	VertexDDRAW(float x, float y, float u, float v, DWORD color) : x(x), y(y), z(0.0f), color(color), u(u), v(v) {}
	VertexDDRAW(float x, float y, DWORD color) : x(x), y(y), z(0.0f), color(color), u(0.0f), v(0.0f) {}

	float x, y, z;
	DWORD color;
	float u, v;
};

typedef struct IDirectDrawSurface7 IDirectDrawSurface7;
struct DirectDrawTexture
{
	DirectDrawTexture() : m_texture(NULL) {}

	operator bool() const {return m_texture;}

	// non-copyable
	DirectDrawTexture(const DirectDrawTexture&) = delete;
	DirectDrawTexture& operator=(const DirectDrawTexture&) = delete;

	// moveable
	DirectDrawTexture(DirectDrawTexture&& rhs) noexcept :
		m_texture(rhs.m_texture), m_width(rhs.m_width), m_height(rhs.m_height), m_scaleW(rhs.m_scaleW), m_scaleH(rhs.m_scaleH), m_linearSample(rhs.m_linearSample)
	{
		rhs.m_texture = NULL;
	}
	DirectDrawTexture& operator=(DirectDrawTexture&& rhs) noexcept
	{
		if(this != &rhs)
		{
			m_texture = rhs.m_texture;
			m_width = rhs.m_width;
			m_height = rhs.m_height;
			m_scaleW = rhs.m_scaleW;
			m_scaleH = rhs.m_scaleH;
			m_linearSample = rhs.m_linearSample;
			rhs.m_texture = NULL;
		}
		return (*this);
	}

	IDirectDrawSurface7* m_texture;
	Uint32 m_width;
	Uint32 m_height;
	float m_scaleW;
	float m_scaleH;
	bool m_linearSample;
};

struct DirectDrawSpriteData
{
	DirectDrawSpriteData() : m_xOffset(0), m_yOffset(0), m_surface(0), m_lastUsage(0) {}

	Uint32 m_xOffset;
	Uint32 m_yOffset;
	Uint32 m_surface;
	Uint32 m_lastUsage;
};

typedef robin_hood::unordered_map<Uint32, DirectDrawTexture> U32BDDRAWTextures;
typedef robin_hood::unordered_map<Uint64, DirectDrawSpriteData> U64BDDRAWTextures;

typedef struct IDirectDraw IDirectDraw;
typedef struct IDirectDraw7 IDirectDraw7;
typedef struct IDirectDrawClipper IDirectDrawClipper;
typedef struct IDirect3D7 IDirect3D7;
typedef struct IDirect3DDevice7 IDirect3DDevice7;
class SurfaceDirectDraw : public Surface
{
	public:
		SurfaceDirectDraw();
		virtual ~SurfaceDirectDraw();

		// non-copyable
		SurfaceDirectDraw(const SurfaceDirectDraw&) = delete;
		SurfaceDirectDraw& operator=(const SurfaceDirectDraw&) = delete;

		// non-moveable
		SurfaceDirectDraw(const SurfaceDirectDraw&&) = delete;
		SurfaceDirectDraw& operator=(const SurfaceDirectDraw&&) = delete;
		
		bool createDirectDrawTexture(DirectDrawTexture& texture, Uint32 width, Uint32 height, bool linearSampler, bool frameBuffer = false);
		bool updateTextureData(DirectDrawTexture& texture, unsigned char* data);
		void releaseDirectDrawTexture(DirectDrawTexture& texture);
		void updateTextureScaling(DirectDrawTexture& texture);

		void resetRenderer();
		void initRenderer();
		void updateRenderer();
		void updateViewport();

		bool testClipper(Sint32& x, Sint32& y, Sint32& w, Sint32& h, Sint32& sx, Sint32& sy, Sint32& sw, Sint32& sh);

		virtual bool isSupported();
		virtual const char* getName() {return "DirectDraw";}
		virtual const char* getSoftware() {return "DirectDraw";}
		virtual const char* getHardware() {return m_hardware;}
		virtual Uint32 getVRAM() {return m_totalVRAM;}

		void generateSpriteAtlases();

		virtual void renderTargetsRecreate();
		virtual void init();
		virtual void doResize(Sint32 w, Sint32 h);
		virtual void spriteManagerReset();
		virtual unsigned char* getScreenPixels(Sint32& width, Sint32& height, bool& bgra);

		virtual void beginScene();
		virtual void endScene();

		DirectDrawTexture* getTextureIndex(DirectDrawTexture* texture);
		void drawQuad(DirectDrawTexture* texture, float vertices[8], float texcoords[8]);
		void drawQuad(DirectDrawTexture* texture, float vertices[8], float texcoords[8], DWORD color);
		void scheduleBatch();

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

		DirectDrawTexture* loadPicture(DirectDrawTexture* s, Uint16 pictureId, bool linear);
		virtual void drawFont(Uint16 pictureId, Sint32 x, Sint32 y, const std::string& text, size_t pos, size_t len, Uint8 r, Uint8 g, Uint8 b, Sint16 cX[256], Sint16 cY[256], Sint16 cW[256], Sint16 cH[256]);
		virtual void drawBackground(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
		virtual void drawPictureRepeat(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
		virtual void drawPicture(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 x, Sint32 y, Sint32 w, Sint32 h);

		bool loadSprite(Uint32 spriteId, DirectDrawTexture* texture, Uint32 xoff, Uint32 yoff);
		bool loadSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Uint32 outfitColor, DirectDrawTexture* texture, Uint32 xoff, Uint32 yoff);
		virtual void drawSprite(Uint32 spriteId, Sint32 x, Sint32 y);
		virtual void drawSprite(Uint32 spriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh);
		virtual void drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Uint32 outfitColor);
		virtual void drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Uint32 outfitColor);

		DirectDrawTexture* createAutomapTile(Uint32 currentArea);
		void uploadAutomapTile(DirectDrawTexture* texture, Uint8 color[256][256]);
		virtual void drawAutomapTile(Uint32 m_currentArea, bool& m_recreate, Uint8 m_color[256][256], Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh);

	protected:
		std::vector<VertexDDRAW> m_vertices;
		std::vector<Uint16> m_indices;
		std::vector<DirectDrawTexture> m_spritesAtlas;
		U32BDDRAWTextures m_automapTiles;
		U64BDDRAWTextures m_sprites;
		std::circular_buffer<Uint32, MAX_AUTOMAPTILES> m_automapTilesBuff;
		std::circular_buffer<Uint64, MAX_SPRITES> m_spritesIds;

		DirectDrawTexture m_stagging;
		DirectDrawTexture* m_pictures = NULL;
		char* m_hardware = NULL;

		DirectDrawTexture m_gameWindow;
		DirectDrawTexture m_scaled_gameWindow;
		DirectDrawTexture* m_binded_texture = NULL;

		void* m_directDrawHandle;
		IDirectDraw7* m_ddraw7 = NULL;
		IDirect3D7* m_renderer = NULL;
		IDirect3DDevice7* m_device = NULL;
		IDirectDrawSurface7* m_primarybuffer = NULL;
		IDirectDrawSurface7* m_backbuffer = NULL;
		IDirectDrawClipper* m_clipper = NULL;

		Sint32 m_maxTextureSize = 1024;
		Sint32 m_integer_scaling_width = 0;
		Sint32 m_integer_scaling_height = 0;

		Uint32 m_clipRectX = 0;
		Uint32 m_clipRectY = 0;
		Uint32 m_clipRectW = 0;
		Uint32 m_clipRectH = 0;

		Uint32 m_viewPortX = 0;
		Uint32 m_viewPortY = 0;
		Uint32 m_viewPortW = 0;
		Uint32 m_viewPortH = 0;

		Uint32 m_totalVRAM = 0;
		Uint32 m_spriteChecker = 0;
		Uint32 m_currentFrame = 0;
		Uint32 m_cachedVertices = 0;

		Uint32 m_spriteAtlases = 0;
		Uint32 m_spritesPerAtlas = 0;
		Uint32 m_spritesPerModulo = 0;

		Sint32 m_px = 0;
		Sint32 m_py = 0;

		bool m_usingLinearSample = false;
		bool m_needReset = true;
		bool m_fullscreen = false;
};
#endif
#endif /* __FILE_SURFACE_DIRECTDRAW_h_ */
