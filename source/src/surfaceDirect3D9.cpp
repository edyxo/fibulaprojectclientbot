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

#include "surfaceDirect3D9.h"

#if defined(SDL_VIDEO_RENDER_D3D)
#include <d3d9.h>
#include <SDL2/SDL_syswm.h>

extern Engine g_engine;
extern Uint32 g_spriteCounts;
extern Uint16 g_pictureCounts;

static D3DPRESENT_PARAMETERS d3d9_pparams;

typedef IDirect3D9*(WINAPI *PFN_CREATE_D3D9)(UINT SDKVersion);
typedef HRESULT(WINAPI *PFN_CREATE_D3D9Ex)(UINT SDKVersion, IDirect3D9Ex** d3d9Ex);

static const GUID D3D9_IID_IDirect3D9 = {0x81BDCBCA, 0x64D4, 0x426D,{0xAE, 0x8D, 0xAD, 0x01, 0x47, 0xF4, 0x27, 0x5C}};
static const GUID D3D9_IID_IDirect3DDevice9Ex = {0xB18B10CE, 0x2649, 0x405A,{0x87, 0x0F, 0x95, 0xF7, 0x77, 0xD4, 0x31, 0x3A}};

static const float inv255f = (1.0f / 255.0f);

SurfaceDirect3D9::SurfaceDirect3D9()
{
	g_engine.RecreateWindow(false);

	m_d3d9Handle = SDL_LoadObject("D3D9.DLL");

	m_vertices.reserve(DIRECT3D9_MAX_VERTICES);
	m_sprites.reserve(MAX_SPRITES);
	m_automapTiles.reserve(MAX_AUTOMAPTILES);
}

SurfaceDirect3D9::~SurfaceDirect3D9()
{
	if(m_hardware)
		SDL_free(m_hardware);

	if(m_pixelShader_sharpen)
		IDirect3DPixelShader9_Release(m_pixelShader_sharpen);

	if(m_defaultRenderTarget)
		IDirect3DSurface9_Release(m_defaultRenderTarget);

	if(m_currentRenderTarget)
		IDirect3DSurface9_Release(m_currentRenderTarget);

	if(m_pictures)
	{
		for(Uint16 i = 0; i < g_pictureCounts; ++i)
		{
			if(m_pictures[i])
				releaseDirect3DTexture(m_pictures[i]);
		}

		SDL_free(m_pictures);
	}

	for(U32BD3D9Textures::iterator it = m_automapTiles.begin(), end = m_automapTiles.end(); it != end; ++it)
		releaseDirect3DTexture(it->second);
	
	for(std::vector<Direct3D9Texture>::iterator it = m_spritesAtlas.begin(), end = m_spritesAtlas.end(); it != end; ++it)
		releaseDirect3DTexture((*it));

	m_sprites.clear();
	m_spritesAtlas.clear();
	m_spritesIds.clear();
	m_automapTilesBuff.clear();
	m_automapTiles.clear();
	if(m_scaled_gameWindow)
		releaseDirect3DTexture(m_scaled_gameWindow);

	if(m_gameWindow)
		releaseDirect3DTexture(m_gameWindow);

	if(m_device)
		IDirect3DDevice9_Release(m_device);

	if(m_deviceEx)
		IDirect3DDevice9Ex_Release(m_deviceEx);

	if(m_d3d9)
		IDirect3D9_Release(m_d3d9);

	if(m_d3d9Ex)
		IDirect3D9Ex_Release(m_d3d9Ex);

	if(m_d3d9Handle)
		SDL_UnloadObject(m_d3d9Handle);
}

bool SurfaceDirect3D9::createDirect3DTexture(Direct3D9Texture& texture, Uint32 width, Uint32 height, bool linearSampler, bool frameBuffer)
{
	releaseDirect3DTexture(texture);

	HRESULT result = IDirect3DDevice9_CreateTexture(m_device, width, height, 1, (frameBuffer ? D3DUSAGE_RENDERTARGET : D3DUSAGE_DYNAMIC), D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture.m_texture, NULL);
	if(FAILED(result))
		return false;

	texture.m_width = width;
	texture.m_height = height;
	texture.m_scaleW = 1.0f / width;
	texture.m_scaleH = 1.0f / height;
	texture.m_linearSample = linearSampler;
	return true;
}

bool SurfaceDirect3D9::updateTextureData(Direct3D9Texture& texture, unsigned char* data)
{
	IDirect3DTexture9* stagingTexture;
	HRESULT result = IDirect3DDevice9_CreateTexture(m_device, texture.m_width, texture.m_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &stagingTexture, NULL);
	if(FAILED(result))
		return false;

	D3DLOCKED_RECT locked;
	result = IDirect3DTexture9_LockRect(stagingTexture, 0, &locked, NULL, D3DLOCK_NOSYSLOCK);
	if(FAILED(result))
	{
		IDirect3DTexture9_Release(stagingTexture);
		return false;
	}

	unsigned char* dstData = SDL_reinterpret_cast(unsigned char*, locked.pBits);
	int length = texture.m_width * 4;
	if(length == locked.Pitch)
		UTIL_FastCopy(SDL_reinterpret_cast(Uint8*, dstData), SDL_reinterpret_cast(const Uint8*, data), length * texture.m_height);
	else
	{
		if(length > locked.Pitch)
			length = locked.Pitch;
		for(Uint32 row = 0; row < texture.m_height; ++row)
		{
			UTIL_FastCopy(SDL_reinterpret_cast(Uint8*, dstData), SDL_reinterpret_cast(const Uint8*, data), length);
			data += length;
			dstData += locked.Pitch;
		}
	}
	result = IDirect3DTexture9_UnlockRect(stagingTexture, 0);
	if(FAILED(result))
	{
		IDirect3DTexture9_Release(stagingTexture);
		return false;
	}
	IDirect3DDevice9_UpdateTexture(m_device, SDL_reinterpret_cast(IDirect3DBaseTexture9*, stagingTexture), SDL_reinterpret_cast(IDirect3DBaseTexture9*, texture.m_texture));
	IDirect3DTexture9_Release(stagingTexture);
	return true;
}

void SurfaceDirect3D9::releaseDirect3DTexture(Direct3D9Texture& texture)
{
	if(texture.m_texture)
	{
		IDirect3DTexture9_Release(texture.m_texture);
		texture.m_texture = NULL;
	}
}

void SurfaceDirect3D9::updateTextureScaling(Direct3D9Texture& texture)
{
	if(m_usingLinearSample != texture.m_linearSample)
	{
		m_usingLinearSample = texture.m_linearSample;
		IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MINFILTER, (m_usingLinearSample ? D3DTEXF_LINEAR : D3DTEXF_POINT));
		IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MAGFILTER, (m_usingLinearSample ? D3DTEXF_LINEAR : D3DTEXF_POINT));
		IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
}

void SurfaceDirect3D9::initRenderer()
{
	IDirect3DDevice9_SetVertexShader(m_device, NULL);
	IDirect3DDevice9_SetPixelShader(m_device, NULL);
	IDirect3DDevice9_SetFVF(m_device, (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1));
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ZENABLE, D3DZB_FALSE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_CULLMODE, D3DCULL_NONE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_LIGHTING, FALSE);

	IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	if(m_useAlphaBlending)
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_SEPARATEALPHABLENDENABLE, TRUE);

	IDirect3DDevice9_SetTextureStageState(m_device, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	IDirect3DDevice9_SetTextureStageState(m_device, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	D3DMATRIX matrix;
	matrix._11 = matrix._22 = matrix._33 = matrix._44 = 1.0f;
	matrix._12 = matrix._13 = matrix._14 = matrix._41 = 0.0f;
	matrix._21 = matrix._23 = matrix._24 = matrix._42 = 0.0f;
	matrix._31 = matrix._32 = matrix._34 = matrix._43 = 0.0f;
	IDirect3DDevice9_SetTransform(m_device, D3DTS_WORLD, &matrix);
	IDirect3DDevice9_SetTransform(m_device, D3DTS_VIEW, &matrix);
	IDirect3DDevice9_SetRenderTarget(m_device, 0, m_defaultRenderTarget);

	m_viewPortX = 0;
	m_viewPortY = 0;
	m_viewPortW = SDL_static_cast(Uint32, g_engine.getWindowWidth());
	m_viewPortH = SDL_static_cast(Uint32, g_engine.getWindowHeight());

	updateRenderer();
	updateViewport();
}

bool SurfaceDirect3D9::resetRenderer()
{
	HRESULT cooperativeLevel = IDirect3DDevice9_TestCooperativeLevel(m_device);
	if(cooperativeLevel == D3DERR_DEVICELOST)
		return false;

	releaseTextures();
	if(m_defaultRenderTarget)
	{
		IDirect3DSurface9_Release(m_defaultRenderTarget);
		m_defaultRenderTarget = NULL;
	}

	if(m_currentRenderTarget)
	{
		IDirect3DSurface9_Release(m_currentRenderTarget);
		m_currentRenderTarget = NULL;
	}

	if(m_gameWindow)
		releaseDirect3DTexture(m_gameWindow);

	if(m_scaled_gameWindow)
		releaseDirect3DTexture(m_scaled_gameWindow);

	HRESULT result = IDirect3DDevice9_Reset(m_device, &d3d9_pparams);
	if(FAILED(result))
	{
		//Restore later
		return false;
	}

	IDirect3DDevice9_GetRenderTarget(m_device, 0, &m_defaultRenderTarget);
	initRenderer();
	renderTargetsRecreate();
	return true;
}

void SurfaceDirect3D9::updateRenderer()
{
	IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	m_usingLinearSample = false;

	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHABLENDENABLE, TRUE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	if(m_useAlphaBlending)
	{
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
	}
}

void SurfaceDirect3D9::updateViewport()
{
	D3DVIEWPORT9 viewport = {m_viewPortX, m_viewPortY, m_viewPortW, m_viewPortH, 0.0f, 1.0f};
	IDirect3DDevice9_SetViewport(m_device, &viewport);

	D3DMATRIX matrix;
	matrix._11 = 2.0f / m_viewPortW; matrix._12 = 0.0f; matrix._13 = 0.0f; matrix._14 = 0.0f;
	matrix._21 = 0.0f; matrix._22 = -2.0f / m_viewPortH; matrix._23 = 0.0f; matrix._24 = 0.0f;
	matrix._31 = 0.0f; matrix._32 = 0.0f; matrix._33 = 1.0f; matrix._34 = 0.0f;
	matrix._41 = -1.0f; matrix._42 = 1.0f; matrix._43 = 0.0f; matrix._44 = 1.0f;
	IDirect3DDevice9_SetTransform(m_device, D3DTS_PROJECTION, &matrix);
}

bool SurfaceDirect3D9::isSupported()
{
	PFN_CREATE_D3D9 CreateD3D9Func;
	PFN_CREATE_D3D9Ex CreateD3D9ExFunc;
	if(!m_d3d9Handle)
	{
		UTIL_MessageBox(false, "Direct3D9: Failed to create Direct3D9 interface.");
		return false;
	}

	CreateD3D9Func = SDL_reinterpret_cast(PFN_CREATE_D3D9, SDL_LoadFunction(m_d3d9Handle, "Direct3DCreate9"));
	if(!CreateD3D9Func)
	{
		UTIL_MessageBox(false, "Direct3D9: Failed to fetch 'Direct3DCreate9' function.");
		return false;
	}

	CreateD3D9ExFunc = SDL_reinterpret_cast(PFN_CREATE_D3D9Ex, SDL_LoadFunction(m_d3d9Handle, "Direct3DCreate9Ex"));
	if(CreateD3D9ExFunc)
	{
		HRESULT result = CreateD3D9ExFunc(D3D_SDK_VERSION, &m_d3d9Ex);
		if(SUCCEEDED(result))
			IDirect3D9Ex_QueryInterface(m_d3d9Ex, D3D9_IID_IDirect3D9, SDL_reinterpret_cast(void**, &m_d3d9));
	}
	if(!m_d3d9)
	{
		m_d3d9 = CreateD3D9Func(D3D_SDK_VERSION);
		if(!m_d3d9Handle)
		{
			UTIL_MessageBox(false, "Direct3D9: Failed to create Direct3D9 interface.");
			return false;
		}
	}

	SDL_SysWMinfo windowinfo;
	SDL_VERSION(&windowinfo.version);
	SDL_GetWindowWMInfo(g_engine.m_window, &windowinfo);

	int w_width, w_height;
	SDL_DisplayMode fullscreen_mode;
	Uint32 window_flags = SDL_GetWindowFlags(g_engine.m_window);
	SDL_GetWindowSize(g_engine.m_window, &w_width, &w_height);
	SDL_GetWindowDisplayMode(g_engine.m_window, &fullscreen_mode);

	SDL_zero(d3d9_pparams);
	d3d9_pparams.hDeviceWindow = windowinfo.info.win.window;
	d3d9_pparams.BackBufferWidth = w_width;
	d3d9_pparams.BackBufferHeight = w_height;
	if(m_d3d9Ex)
		d3d9_pparams.BackBufferCount = 2;
	else
		d3d9_pparams.BackBufferCount = 1;
	if(m_d3d9Ex)
		d3d9_pparams.SwapEffect = D3DSWAPEFFECT_FLIPEX;
	else
		d3d9_pparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if(window_flags & SDL_WINDOW_FULLSCREEN && (window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != SDL_WINDOW_FULLSCREEN_DESKTOP)
	{
		d3d9_pparams.Windowed = FALSE;
		if(fullscreen_mode.format == SDL_PIXELFORMAT_ARGB8888)
			d3d9_pparams.BackBufferFormat = D3DFMT_A8R8G8B8;
		else
			d3d9_pparams.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3d9_pparams.FullScreen_RefreshRateInHz = fullscreen_mode.refresh_rate;
	}
	else
	{
		d3d9_pparams.Windowed = TRUE;
		d3d9_pparams.BackBufferFormat = D3DFMT_UNKNOWN;
		d3d9_pparams.FullScreen_RefreshRateInHz = 0;
	}
	if(g_engine.isVsync())
		d3d9_pparams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else
		d3d9_pparams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	int displayIndex = SDL_GetWindowDisplayIndex(g_engine.m_window);
	m_adapter = SDL_Direct3D9GetAdapterIndex(displayIndex);

	D3DCAPS9 caps;
	IDirect3D9_GetDeviceCaps(m_d3d9, m_adapter, D3DDEVTYPE_HAL, &caps);

	DWORD device_flags = (D3DCREATE_FPU_PRESERVE|D3DCREATE_NOWINDOWCHANGES);
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		device_flags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		device_flags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	HRESULT result = IDirect3D9_CreateDevice(m_d3d9, m_adapter, D3DDEVTYPE_HAL, d3d9_pparams.hDeviceWindow, device_flags, &d3d9_pparams, &m_device);
	if(FAILED(result))
	{
		UTIL_MessageBox(false, "Direct3D9: Failed to create Direct3D9 Device Context.");
		return false;
	}
	if(m_d3d9Ex)
		IDirect3DDevice9_QueryInterface(m_device, D3D9_IID_IDirect3DDevice9Ex, SDL_reinterpret_cast(void**, &m_deviceEx));
	IDirect3DDevice9_GetDeviceCaps(m_device, &caps);
	
	m_maxTextureSize = UTIL_min<Sint32>(caps.MaxTextureWidth, caps.MaxTextureHeight);
	if(m_maxTextureSize < 1024)
	{
		UTIL_MessageBox(false, "Direct3D9: Your device doesn't meet minimum requirement for texture dimension.");
		return false;
	}
	if(caps.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND)
		m_useAlphaBlending = true;

	if(caps.PixelShaderVersion <= D3DPS_VERSION(1, 0))
		m_software = "Direct3D 8.0";
	else if(caps.PixelShaderVersion <= D3DPS_VERSION(1, 3))
		m_software = "Direct3D 8.0a";
	else if(caps.PixelShaderVersion <= D3DPS_VERSION(1, 4))
		m_software = "Direct3D 8.1";
	else if(caps.PixelShaderVersion <= D3DPS_VERSION(2, 0))
		m_software = "Direct3D 9.0";
	else if(caps.PixelShaderVersion < D3DPS_VERSION(3, 0))
		m_software = "Direct3D 9.0b";
	else if(caps.PixelShaderVersion <= D3DPS_VERSION(3, 0))
		m_software = "Direct3D 9.0c";

	if(SUCCEEDED(IDirect3DDevice9_CreatePixelShader(m_device, D3D9_sharpen, &m_pixelShader_sharpen)))
		m_haveSharpening = true;

	IDirect3DDevice9_GetRenderTarget(m_device, 0, &m_defaultRenderTarget);
	initRenderer();

	D3DADAPTER_IDENTIFIER9 identifier;
	if(SUCCEEDED(IDirect3D9_GetAdapterIdentifier(m_d3d9, m_adapter, 0, &identifier)))
		m_hardware = SDL_strdup(identifier.Description);
	m_totalVRAM = UTIL_power_of_2(IDirect3DDevice9_GetAvailableTextureMem(m_device) / 1048576);
	return true;
}

void SurfaceDirect3D9::generateSpriteAtlases()
{
	if(m_maxTextureSize >= 16384 && MAX_SPRITES > 65536)
	{
		m_spriteAtlases = (MAX_SPRITES + 262143) / 262144;
		m_spritesPerAtlas = 262144;
		m_spritesPerModulo = 16384;
	}
	else if(m_maxTextureSize >= 8192 && MAX_SPRITES > 16384)
	{
		m_spriteAtlases = (MAX_SPRITES + 65535) / 65536;
		m_spritesPerAtlas = 65536;
		m_spritesPerModulo = 8192;
	}
	else if(m_maxTextureSize >= 4096 && MAX_SPRITES > 4096)
	{
		m_spriteAtlases = (MAX_SPRITES + 16383) / 16384;
		m_spritesPerAtlas = 16384;
		m_spritesPerModulo = 4096;
	}
	else if(m_maxTextureSize >= 2048 && MAX_SPRITES > 1024)
	{
		m_spriteAtlases = (MAX_SPRITES + 4095) / 4096;
		m_spritesPerAtlas = 4096;
		m_spritesPerModulo = 2048;
	}
	else
	{
		m_spriteAtlases = (MAX_SPRITES + 1023) / 1024;
		m_spritesPerAtlas = 1024;
		m_spritesPerModulo = 1024;
	}

	for(Uint32 i = 0; i < m_spriteAtlases; ++i)
	{
		m_spritesAtlas.emplace_back();
		if(!createDirect3DTexture(m_spritesAtlas.back(), m_spritesPerModulo, m_spritesPerModulo, false))
		{
			UTIL_MessageBox(true, "Direct3D9: Out of video memory.");
			exit(-1);
		}
	}
}

void SurfaceDirect3D9::renderTargetsRecreate()
{
	if(!createDirect3DTexture(m_gameWindow, RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, (g_engine.getAntialiasing() == CLIENT_ANTIALIASING_NORMAL), true))
	{
		UTIL_MessageBox(false, "Direct3D9: Out of video memory.");
		exit(-1);
	}
	generateSpriteAtlases();
}

void SurfaceDirect3D9::init()
{
	m_pictures = SDL_reinterpret_cast(Direct3D9Texture*, SDL_calloc(g_pictureCounts, sizeof(Direct3D9Texture)));
	if(!m_pictures)
	{
		SDL_OutOfMemory();
		SDL_snprintf(g_buffer, sizeof(g_buffer), "Surface::Init() Failed: %s", SDL_GetError());
		UTIL_MessageBox(true, g_buffer);
		exit(-1);
	}
	renderTargetsRecreate();

	#if DIRECT3D9_USE_UINT_INDICES > 0
	Uint32 offset = 0;
	#else
	Uint16 offset = 0;
	#endif
	m_indices.resize(DIRECT3D9_MAX_INDICES);
	for(Sint32 i = 0; i < DIRECT3D9_MAX_INDICES; i += 6)
	{
		m_indices[i + 0] = 0 + offset;
		m_indices[i + 1] = 1 + offset;
		m_indices[i + 2] = 2 + offset;

		m_indices[i + 3] = 3 + offset;
		m_indices[i + 4] = 2 + offset;
		m_indices[i + 5] = 1 + offset;

		offset += 4;
	}
}

void SurfaceDirect3D9::doResize(Sint32, Sint32)
{
	m_needReset = true;
}

void SurfaceDirect3D9::spriteManagerReset()
{
	for(U32BD3D9Textures::iterator it = m_automapTiles.begin(), end = m_automapTiles.end(); it != end; ++it)
		releaseDirect3DTexture(it->second);
	
	for(std::vector<Direct3D9Texture>::iterator it = m_spritesAtlas.begin(), end = m_spritesAtlas.end(); it != end; ++it)
		releaseDirect3DTexture((*it));

	m_sprites.clear();
	m_spritesAtlas.clear();
	m_spritesIds.clear();
	m_automapTilesBuff.clear();
	m_automapTiles.clear();
	generateSpriteAtlases();
}

void SurfaceDirect3D9::releaseTextures()
{
	if(m_pictures)
	{
		for(Uint16 i = 0; i < g_pictureCounts; ++i)
		{
			if(m_pictures[i])
				releaseDirect3DTexture(m_pictures[i]);
		}
	}

	for(U32BD3D9Textures::iterator it = m_automapTiles.begin(), end = m_automapTiles.end(); it != end; ++it)
		releaseDirect3DTexture(it->second);
	
	for(std::vector<Direct3D9Texture>::iterator it = m_spritesAtlas.begin(), end = m_spritesAtlas.end(); it != end; ++it)
		releaseDirect3DTexture((*it));

	m_sprites.clear();
	m_spritesAtlas.clear();
	m_spritesIds.clear();
	m_automapTilesBuff.clear();
	m_automapTiles.clear();
}

unsigned char* SurfaceDirect3D9::getScreenPixels(Sint32& width, Sint32& height, bool& bgra)
{
	if(m_needReset)
		return NULL;

	D3DSURFACE_DESC desc;
	HRESULT result = IDirect3DSurface9_GetDesc(m_defaultRenderTarget, &desc);
	if(FAILED(result))
		return NULL;
	IDirect3DSurface9* surface;
	result = IDirect3DDevice9_CreateOffscreenPlainSurface(m_device, desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &surface, NULL);
	if(FAILED(result))
		return NULL;
	result = IDirect3DDevice9_GetRenderTargetData(m_device, m_defaultRenderTarget, surface);
	if(FAILED(result))
	{
		IDirect3DSurface9_Release(surface);
		return NULL;
	}

	width = SDL_static_cast(Sint32, desc.Width);
	height = SDL_static_cast(Sint32, desc.Height);

	unsigned char* pixels = SDL_reinterpret_cast(unsigned char*, SDL_malloc(width * height * 4));
	if(!pixels)
	{
		IDirect3DSurface9_Release(surface);
		return NULL;
	}
	D3DLOCKED_RECT locked;
	result = IDirect3DSurface9_LockRect(surface, &locked, NULL, D3DLOCK_READONLY);
	if(FAILED(result))
	{
		IDirect3DSurface9_Release(surface);
		SDL_free(pixels);
		return NULL;
	}
	if(desc.Format == D3DFMT_X8R8G8B8 || desc.Format == D3DFMT_A8R8G8B8)
	{
		UTIL_FastCopy(SDL_reinterpret_cast(Uint8*, pixels), SDL_reinterpret_cast(const Uint8*, locked.pBits), SDL_static_cast(size_t, width * height * 4));
		bgra = true;
	}
	else
	{
		IDirect3DSurface9_UnlockRect(surface);
		IDirect3DSurface9_Release(surface);
		SDL_free(pixels);
		return NULL;
	}
	IDirect3DSurface9_UnlockRect(surface);
	IDirect3DSurface9_Release(surface);
	return pixels;
}

void SurfaceDirect3D9::beginScene()
{
	m_spriteChecker = 0;
	++m_currentFrame;
	if(m_needReset)
	{
		int w_width, w_height;
		SDL_GetWindowSize(g_engine.m_window, &w_width, &w_height);
		d3d9_pparams.BackBufferWidth = w_width;
		d3d9_pparams.BackBufferHeight = w_height;

		Uint32 window_flags = SDL_GetWindowFlags(g_engine.m_window);
		if(window_flags & SDL_WINDOW_FULLSCREEN && (window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != SDL_WINDOW_FULLSCREEN_DESKTOP)
		{
			SDL_DisplayMode fullscreen_mode;
			SDL_GetWindowDisplayMode(g_engine.m_window, &fullscreen_mode);

			d3d9_pparams.Windowed = FALSE;
			if(fullscreen_mode.format == SDL_PIXELFORMAT_ARGB8888)
				d3d9_pparams.BackBufferFormat = D3DFMT_A8R8G8B8;
			else
				d3d9_pparams.BackBufferFormat = D3DFMT_X8R8G8B8;
			d3d9_pparams.FullScreen_RefreshRateInHz = fullscreen_mode.refresh_rate;
		}
		else
		{
			d3d9_pparams.Windowed = TRUE;
			d3d9_pparams.BackBufferFormat = D3DFMT_UNKNOWN;
			d3d9_pparams.FullScreen_RefreshRateInHz = 0;
		}

		if(!resetRenderer())
			return;

		m_needReset = false;
	}

	HRESULT result = IDirect3DDevice9_BeginScene(m_device);
	if(result == D3DERR_DEVICELOST)
	{
		m_needReset = true;
		return;
	}
	updateRenderer();
}

void SurfaceDirect3D9::endScene()
{
	scheduleBatch();
	IDirect3DDevice9_EndScene(m_device);

	HRESULT result = IDirect3DDevice9_TestCooperativeLevel(m_device);
	if(result == D3DERR_DEVICELOST)
	{
		//Reset later
		m_needReset = true;
		return;
	}
	else if(result == D3DERR_DEVICENOTRESET)
	{
		m_needReset = true;
		return;
	}
	if(m_deviceEx)
		result = IDirect3DDevice9Ex_PresentEx(m_deviceEx, NULL, NULL, NULL, NULL, (g_engine.isVsync() ? 0 : D3DPRESENT_FORCEIMMEDIATE));
	else
		result = IDirect3DDevice9_Present(m_device, NULL, NULL, NULL, NULL);

	if(result == D3DERR_DEVICELOST || result == D3DERR_DEVICENOTRESET)
		m_needReset = true;
}

Direct3D9Texture* SurfaceDirect3D9::getTextureIndex(Direct3D9Texture* texture)
{
	if(texture != m_binded_texture)
	{
		scheduleBatch();
		m_binded_texture = texture;
	}
	return m_binded_texture;
}

void SurfaceDirect3D9::drawQuad(Direct3D9Texture*, float vertices[8], float texcoords[8])
{
	m_vertices.emplace_back(vertices[0], vertices[1], texcoords[0], texcoords[1], 0xFFFFFFFF);
	m_vertices.emplace_back(vertices[2], vertices[3], texcoords[2], texcoords[3], 0xFFFFFFFF);
	m_vertices.emplace_back(vertices[4], vertices[5], texcoords[4], texcoords[5], 0xFFFFFFFF);
	m_vertices.emplace_back(vertices[6], vertices[7], texcoords[6], texcoords[7], 0xFFFFFFFF);
	m_cachedVertices += 6;
}

void SurfaceDirect3D9::drawQuad(Direct3D9Texture*, float vertices[8], float texcoords[8], DWORD color)
{
	m_vertices.emplace_back(vertices[0], vertices[1], texcoords[0], texcoords[1], color);
	m_vertices.emplace_back(vertices[2], vertices[3], texcoords[2], texcoords[3], color);
	m_vertices.emplace_back(vertices[4], vertices[5], texcoords[4], texcoords[5], color);
	m_vertices.emplace_back(vertices[6], vertices[7], texcoords[6], texcoords[7], color);
	m_cachedVertices += 6;
}

void SurfaceDirect3D9::scheduleBatch()
{
	if(m_cachedVertices > 0)
	{
		if(m_binded_texture)
		{
			updateTextureScaling(*m_binded_texture);
			IDirect3DDevice9_SetTexture(m_device, 0, SDL_reinterpret_cast(IDirect3DBaseTexture9*, m_binded_texture->m_texture));
		}
		else
			IDirect3DDevice9_SetTexture(m_device, 0, SDL_reinterpret_cast(IDirect3DBaseTexture9*, NULL));

		if(m_cachedVertices > DIRECT3D9_MAX_INDICES)
		{
			UINT vertices = SDL_static_cast(UINT, m_vertices.size());
			VertexD3D9* verticeData = &m_vertices[0];
			do
			{
				UINT passVertices = UTIL_min<UINT>(vertices, DIRECT3D9_MAX_VERTICES);
				UINT passIndices = UTIL_min<UINT>(SDL_static_cast(UINT, m_cachedVertices), DIRECT3D9_MAX_INDICES);
				#if DIRECT3D9_USE_UINT_INDICES > 0
				IDirect3DDevice9_DrawIndexedPrimitiveUP(m_device, D3DPT_TRIANGLELIST, 0, passVertices, (passIndices / 3), &m_indices[0], D3DFMT_INDEX32, verticeData, sizeof(VertexD3D9));
				#else
				IDirect3DDevice9_DrawIndexedPrimitiveUP(m_device, D3DPT_TRIANGLELIST, 0, passVertices, (passIndices / 3), &m_indices[0], D3DFMT_INDEX16, verticeData, sizeof(VertexD3D9));
				#endif
				vertices -= passVertices;
				verticeData += passVertices;
				m_cachedVertices -= passIndices;
			} while(m_cachedVertices > 0);
		}
		else
		{
			#if DIRECT3D9_USE_UINT_INDICES > 0
			IDirect3DDevice9_DrawIndexedPrimitiveUP(m_device, D3DPT_TRIANGLELIST, 0, SDL_static_cast(UINT, m_vertices.size()), SDL_static_cast(UINT, m_cachedVertices / 3), &m_indices[0], D3DFMT_INDEX32, &m_vertices[0], sizeof(VertexD3D9));
			#else
			IDirect3DDevice9_DrawIndexedPrimitiveUP(m_device, D3DPT_TRIANGLELIST, 0, SDL_static_cast(UINT, m_vertices.size()), SDL_static_cast(UINT, m_cachedVertices / 3), &m_indices[0], D3DFMT_INDEX16, &m_vertices[0], sizeof(VertexD3D9));
			#endif
		}

		m_vertices.clear();
		m_cachedVertices = 0;
	}
}

bool SurfaceDirect3D9::integer_scaling(Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
	Sint32 width = RENDERTARGET_WIDTH;
	Sint32 height = RENDERTARGET_HEIGHT;
	UTIL_integer_scale(width, height, w, h, m_maxTextureSize, m_maxTextureSize);
	if(m_integer_scaling_width < width || m_integer_scaling_height < height || !m_scaled_gameWindow)
	{
		if(!createDirect3DTexture(m_scaled_gameWindow, width, height, true, true))
			return false;

		m_integer_scaling_width = width;
		m_integer_scaling_height = height;
	}

	scheduleBatch();
	if(m_currentRenderTarget)
	{
		IDirect3DSurface9_Release(m_currentRenderTarget);
		m_currentRenderTarget = NULL;
	}
	IDirect3DTexture9_GetSurfaceLevel(m_scaled_gameWindow.m_texture, 0, &m_currentRenderTarget);
	IDirect3DDevice9_SetRenderTarget(m_device, 0, m_currentRenderTarget);

	m_viewPortX = 0;
	m_viewPortY = 0;
	m_viewPortW = m_scaled_gameWindow.m_width;
	m_viewPortH = m_scaled_gameWindow.m_height;
	updateViewport();

	float minx = -0.5f;
	float maxx = minx + SDL_static_cast(float, width);
	float miny = -0.5f;
	float maxy = miny + SDL_static_cast(float, height);

	float minu = sx * m_gameWindow.m_scaleW;
	float maxu = (sx + sw) * m_gameWindow.m_scaleW;
	float minv = sy * m_gameWindow.m_scaleH;
	float maxv = (sy + sh) * m_gameWindow.m_scaleH;

	float vertices[8];
	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	float texcoords[8];
	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;

	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHABLENDENABLE, FALSE);
	drawQuad(getTextureIndex(&m_gameWindow), vertices, texcoords);

	scheduleBatch();
	if(m_currentRenderTarget)
	{
		IDirect3DSurface9_Release(m_currentRenderTarget);
		m_currentRenderTarget = NULL;
	}
	IDirect3DDevice9_SetRenderTarget(m_device, 0, m_defaultRenderTarget);

	m_viewPortX = 0;
	m_viewPortY = 0;
	m_viewPortW = g_engine.getWindowWidth();
	m_viewPortH = g_engine.getWindowHeight();
	updateViewport();

	minx = SDL_static_cast(float, x) - 0.5f;
	maxx = minx + SDL_static_cast(float, w);
	miny = SDL_static_cast(float, y) - 0.5f;
	maxy = miny + SDL_static_cast(float, h);

	minu = 0.0f;
	maxu = width * m_scaled_gameWindow.m_scaleW;
	minv = 0.0f;
	maxv = height * m_scaled_gameWindow.m_scaleH;

	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;
	if(g_engine.isSharpening() && m_haveSharpening)
	{
		float textureDimensions[2] = {1.0f / w * (sw * m_gameWindow.m_scaleW), 1.0f / h * (sh * m_gameWindow.m_scaleH)};
		IDirect3DDevice9_SetPixelShader(m_device, m_pixelShader_sharpen);
		IDirect3DDevice9_SetPixelShaderConstantF(m_device, 0, textureDimensions, 2);
	}

	drawQuad(getTextureIndex(&m_scaled_gameWindow), vertices, texcoords);
	scheduleBatch();
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHABLENDENABLE, TRUE);
	if(g_engine.isSharpening() && m_haveSharpening)
		IDirect3DDevice9_SetPixelShader(m_device, NULL);

	return true;
}

void SurfaceDirect3D9::drawGameScene(Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
	Uint8 antialiasing = g_engine.getAntialiasing();
	if(antialiasing == CLIENT_ANTIALIASING_INTEGER)
	{
		if(integer_scaling(sx, sy, sw, sh, x, y, w, h))
			return;
		else
		{
			antialiasing = CLIENT_ANTIALIASING_NORMAL;
			g_engine.setAntialiasing(antialiasing);
		}
	}

	float minx = SDL_static_cast(float, x) - 0.5f;
	float maxx = minx + SDL_static_cast(float, w);
	float miny = SDL_static_cast(float, y) - 0.5f;
	float maxy = miny + SDL_static_cast(float, h);

	float minu = sx * m_gameWindow.m_scaleW;
	float maxu = (sx + sw) * m_gameWindow.m_scaleW;
	float minv = sy * m_gameWindow.m_scaleH;
	float maxv = (sy + sh) * m_gameWindow.m_scaleH;

	float vertices[8];
	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	float texcoords[8];
	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;

	scheduleBatch();
	if(g_engine.isSharpening() && m_haveSharpening)
	{
		float textureDimensions[2] = {1.0f / w * (sw * m_gameWindow.m_scaleW), 1.0f / h * (sh * m_gameWindow.m_scaleH)};
		IDirect3DDevice9_SetPixelShader(m_device, m_pixelShader_sharpen);
		IDirect3DDevice9_SetPixelShaderConstantF(m_device, 0, textureDimensions, 2);
	}

	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHABLENDENABLE, FALSE);
	drawQuad(getTextureIndex(&m_gameWindow), vertices, texcoords);
	scheduleBatch();
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHABLENDENABLE, TRUE);
	if(g_engine.isSharpening() && m_haveSharpening)
		IDirect3DDevice9_SetPixelShader(m_device, NULL);
}

void SurfaceDirect3D9::beginGameScene()
{
	scheduleBatch();
	if(m_currentRenderTarget)
	{
		IDirect3DSurface9_Release(m_currentRenderTarget);
		m_currentRenderTarget = NULL;
	}
	IDirect3DTexture9_GetSurfaceLevel(m_gameWindow.m_texture, 0, &m_currentRenderTarget);
	IDirect3DDevice9_SetRenderTarget(m_device, 0, m_currentRenderTarget);

	m_viewPortX = 0;
	m_viewPortY = 0;
	m_viewPortW = m_gameWindow.m_width;
	m_viewPortH = m_gameWindow.m_height;
	updateViewport();
	fillRectangle(0, 0, RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, 0x00, 0x00, 0x00, 255);
}

void SurfaceDirect3D9::endGameScene()
{
	scheduleBatch();
	if(m_currentRenderTarget)
	{
		IDirect3DSurface9_Release(m_currentRenderTarget);
		m_currentRenderTarget = NULL;
	}
	IDirect3DDevice9_SetRenderTarget(m_device, 0, m_defaultRenderTarget);

	m_viewPortX = 0;
	m_viewPortY = 0;
	m_viewPortW = g_engine.getWindowWidth();
	m_viewPortH = g_engine.getWindowHeight();
	updateViewport();
}

void SurfaceDirect3D9::drawLightMap_old(LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height)
{
	scheduleBatch();
	m_binded_texture = NULL;

	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLEND, D3DBLEND_ZERO);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
	if(m_useAlphaBlending)
	{
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	}
	IDirect3DDevice9_SetTexture(m_device, 0, SDL_reinterpret_cast(IDirect3DBaseTexture9*, NULL));

	Sint32 drawY = y - scale - (scale / 2);
	height -= 1;
	for(Sint32 j = -1; j < height; ++j)
	{
		Sint32 offset1 = (j + 1) * width;
		Sint32 offset2 = UTIL_max<Sint32>(j, 0) * width;
		Sint32 drawX = x - scale - (scale / 2), verticeCount = 0;
		for(Sint32 k = -1; k < width; ++k)
		{
			Sint32 offset = UTIL_max<Sint32>(k, 0);
			m_vertices.emplace_back(SDL_static_cast(float, drawX) - 0.5f, SDL_static_cast(float, drawY + scale) - 0.5f, MAKE_RGBA_COLOR(lightmap[offset1 + offset].r, lightmap[offset1 + offset].g, lightmap[offset1 + offset].b, 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX) - 0.5f, SDL_static_cast(float, drawY) - 0.5f, MAKE_RGBA_COLOR(lightmap[offset2 + offset].r, lightmap[offset2 + offset].g, lightmap[offset2 + offset].b, 255));
			verticeCount += 2;
			drawX += scale;
		}

		drawY += scale;
		IDirect3DDevice9_DrawPrimitiveUP(m_device, D3DPT_TRIANGLESTRIP, verticeCount - 2, &m_vertices[0], sizeof(VertexD3D9));
		m_vertices.clear();
	}

	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	if(m_useAlphaBlending)
	{
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
	}
}

void SurfaceDirect3D9::drawLightMap_new(LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height)
{
	scheduleBatch();
	m_binded_texture = NULL;

	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLEND, D3DBLEND_ZERO);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
	if(m_useAlphaBlending)
	{
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	}
	IDirect3DDevice9_SetTexture(m_device, 0, SDL_reinterpret_cast(IDirect3DBaseTexture9*, NULL));

	Sint32 halfScale = (scale / 2);
	Sint32 drawY = y - scale - halfScale;
	height -= 1;
	for(Sint32 j = -1; j < height; ++j)
	{
		Sint32 offset1 = (j + 1) * width;
		Sint32 offset2 = UTIL_max<Sint32>(j, 0) * width;
		Sint32 drawX = x - scale - halfScale;
		for(Sint32 k = -1; k < width; ++k)
		{
			Sint32 offset = UTIL_max<Sint32>(k, 0);
			
			//Get the colors of tile corners
			float topCenter[2][3], leftCenter[2][3], bottomCenter[2][3], rightCenter[2][3], center[2][3], topLeft[2][3], topRight[2][3], bottomLeft[2][3], bottomRight[2][3];
			topLeft[0][0] = lightmap[offset2 + offset].r * inv255f;
			topLeft[0][1] = lightmap[offset2 + offset].g * inv255f;
			topLeft[0][2] = lightmap[offset2 + offset].b * inv255f;
			topRight[0][0] = lightmap[offset2 + k + 1].r * inv255f;
			topRight[0][1] = lightmap[offset2 + k + 1].g * inv255f;
			topRight[0][2] = lightmap[offset2 + k + 1].b * inv255f;
			bottomLeft[0][0] = lightmap[offset1 + offset].r * inv255f;
			bottomLeft[0][1] = lightmap[offset1 + offset].g * inv255f;
			bottomLeft[0][2] = lightmap[offset1 + offset].b * inv255f;
			bottomRight[0][0] = lightmap[offset1 + k + 1].r * inv255f;
			bottomRight[0][1] = lightmap[offset1 + k + 1].g * inv255f;
			bottomRight[0][2] = lightmap[offset1 + k + 1].b * inv255f;

			//Get the colors of first triangles
			float v[3][2], p[2];
			v[0][0] = SDL_static_cast(float, drawX);
			v[0][1] = SDL_static_cast(float, drawY);
			v[1][0] = SDL_static_cast(float, drawX + scale);
			v[1][1] = SDL_static_cast(float, drawY);
			v[2][0] = SDL_static_cast(float, drawX);
			v[2][1] = SDL_static_cast(float, drawY + scale);
			p[0] = SDL_static_cast(float, drawX + scale);
			p[1] = SDL_static_cast(float, drawY);
			getTrianglePointFloat(v[0], v[1], v[2], topLeft[0], topRight[0], bottomLeft[0], p, topRight[1]);
			p[0] = SDL_static_cast(float, drawX + halfScale);
			p[1] = SDL_static_cast(float, drawY);
			getTrianglePointFloat(v[0], v[1], v[2], topLeft[0], topRight[0], bottomLeft[0], p, topCenter[0]);
			p[0] = SDL_static_cast(float, drawX);
			p[1] = SDL_static_cast(float, drawY + halfScale);
			getTrianglePointFloat(v[0], v[1], v[2], topLeft[0], topRight[0], bottomLeft[0], p, leftCenter[0]);
			p[0] = SDL_static_cast(float, drawX + halfScale) - 0.5f;
			p[1] = SDL_static_cast(float, drawY + halfScale) - 0.5f;
			getTrianglePointFloat(v[0], v[1], v[2], topLeft[0], topRight[0], bottomLeft[0], p, center[0]);
			v[0][0] = SDL_static_cast(float, drawX + scale);
			v[0][1] = SDL_static_cast(float, drawY + scale);
			p[0] = SDL_static_cast(float, drawX);
			p[1] = SDL_static_cast(float, drawY + scale);
			getTrianglePointFloat(v[0], v[1], v[2], bottomRight[0], topRight[0], bottomLeft[0], p, bottomLeft[1]);
			p[0] = SDL_static_cast(float, drawX + scale);
			p[1] = SDL_static_cast(float, drawY + halfScale);
			getTrianglePointFloat(v[0], v[1], v[2], bottomRight[0], topRight[0], bottomLeft[0], p, rightCenter[0]);
			p[0] = SDL_static_cast(float, drawX + halfScale);
			p[1] = SDL_static_cast(float, drawY + scale);
			getTrianglePointFloat(v[0], v[1], v[2], bottomRight[0], topRight[0], bottomLeft[0], p, bottomCenter[0]);

			//Get the colors of second triangles
			v[0][0] = SDL_static_cast(float, drawX);
			v[0][1] = SDL_static_cast(float, drawY + scale);
			v[1][0] = SDL_static_cast(float, drawX);
			v[1][1] = SDL_static_cast(float, drawY);
			v[2][0] = SDL_static_cast(float, drawX + scale);
			v[2][1] = SDL_static_cast(float, drawY + scale);
			p[0] = SDL_static_cast(float, drawX);
			p[1] = SDL_static_cast(float, drawY);
			getTrianglePointFloat(v[0], v[1], v[2], bottomLeft[0], topLeft[0], bottomRight[0], p, topLeft[1]);
			p[0] = SDL_static_cast(float, drawX);
			p[1] = SDL_static_cast(float, drawY + halfScale);
			getTrianglePointFloat(v[0], v[1], v[2], bottomLeft[0], topLeft[0], bottomRight[0], p, leftCenter[1]);
			p[0] = SDL_static_cast(float, drawX + halfScale);
			p[1] = SDL_static_cast(float, drawY + scale);
			getTrianglePointFloat(v[0], v[1], v[2], bottomLeft[0], topLeft[0], bottomRight[0], p, bottomCenter[1]);
			v[0][0] = SDL_static_cast(float, drawX + scale);
			v[0][1] = SDL_static_cast(float, drawY);
			p[0] = SDL_static_cast(float, drawX + scale);
			p[1] = SDL_static_cast(float, drawY + scale);
			getTrianglePointFloat(v[0], v[1], v[2], topRight[0], topLeft[0], bottomRight[0], p, bottomRight[1]);
			p[0] = SDL_static_cast(float, drawX + scale);
			p[1] = SDL_static_cast(float, drawY + halfScale);
			getTrianglePointFloat(v[0], v[1], v[2], topRight[0], topLeft[0], bottomRight[0], p, rightCenter[1]);
			p[0] = SDL_static_cast(float, drawX + halfScale);
			p[1] = SDL_static_cast(float, drawY);
			getTrianglePointFloat(v[0], v[1], v[2], topRight[0], topLeft[0], bottomRight[0], p, topCenter[1]);
			p[0] = SDL_static_cast(float, drawX + halfScale) + 0.5f;
			p[1] = SDL_static_cast(float, drawY + halfScale) + 0.5f;
			getTrianglePointFloat(v[0], v[1], v[2], topRight[0], topLeft[0], bottomRight[0], p, center[1]);

			//Use brighter color from triangles for our squares
			#define VEC_MAX(a)										\
				do {												\
					a[0][0] = UTIL_max<float>(a[0][0], a[1][0]);	\
					a[0][1] = UTIL_max<float>(a[0][1], a[1][1]);	\
					a[0][2] = UTIL_max<float>(a[0][2], a[1][2]);	\
				} while(0)

			VEC_MAX(topLeft);
			VEC_MAX(topRight);
			VEC_MAX(bottomLeft);
			VEC_MAX(bottomRight);
			VEC_MAX(leftCenter);
			VEC_MAX(bottomCenter);
			VEC_MAX(rightCenter);
			VEC_MAX(topCenter);
			VEC_MAX(center);
			#undef VEC_MAX

			//Draw Top-Left square
			m_vertices.emplace_back(SDL_static_cast(float, drawX) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, leftCenter[0][0] * 255.f), SDL_static_cast(Uint8, leftCenter[0][1] * 255.f), SDL_static_cast(Uint8, leftCenter[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX) - 0.5f, SDL_static_cast(float, drawY) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, topLeft[0][0] * 255.f), SDL_static_cast(Uint8, topLeft[0][1] * 255.f), SDL_static_cast(Uint8, topLeft[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, center[0][0] * 255.f), SDL_static_cast(Uint8, center[0][1] * 255.f), SDL_static_cast(Uint8, center[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, topCenter[0][0] * 255.f), SDL_static_cast(Uint8, topCenter[0][1] * 255.f), SDL_static_cast(Uint8, topCenter[0][2] * 255.f), 255));
			
			//Draw Bottom-Left square
			m_vertices.emplace_back(SDL_static_cast(float, drawX) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, leftCenter[0][0] * 255.f), SDL_static_cast(Uint8, leftCenter[0][1] * 255.f), SDL_static_cast(Uint8, leftCenter[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX) - 0.5f, SDL_static_cast(float, drawY + scale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, bottomLeft[0][0] * 255.f), SDL_static_cast(Uint8, bottomLeft[0][1] * 255.f), SDL_static_cast(Uint8, bottomLeft[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, center[0][0] * 255.f), SDL_static_cast(Uint8, center[0][1] * 255.f), SDL_static_cast(Uint8, center[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY + scale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, bottomCenter[0][0] * 255.f), SDL_static_cast(Uint8, bottomCenter[0][1] * 255.f), SDL_static_cast(Uint8, bottomCenter[0][2] * 255.f), 255));
			
			//Draw Top-Right square
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, topCenter[0][0] * 255.f), SDL_static_cast(Uint8, topCenter[0][1] * 255.f), SDL_static_cast(Uint8, topCenter[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, center[0][0] * 255.f), SDL_static_cast(Uint8, center[0][1] * 255.f), SDL_static_cast(Uint8, center[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + scale) - 0.5f, SDL_static_cast(float, drawY) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, topRight[0][0] * 255.f), SDL_static_cast(Uint8, topRight[0][1] * 255.f), SDL_static_cast(Uint8, topRight[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + scale) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, rightCenter[0][0] * 255.f), SDL_static_cast(Uint8, rightCenter[0][1] * 255.f), SDL_static_cast(Uint8, rightCenter[0][2] * 255.f), 255));
			
			//Draw Bottom-Right square
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY + scale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, bottomCenter[0][0] * 255.f), SDL_static_cast(Uint8, bottomCenter[0][1] * 255.f), SDL_static_cast(Uint8, bottomCenter[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + halfScale) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, center[0][0] * 255.f), SDL_static_cast(Uint8, center[0][1] * 255.f), SDL_static_cast(Uint8, center[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + scale) - 0.5f, SDL_static_cast(float, drawY + scale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, bottomRight[0][0] * 255.f), SDL_static_cast(Uint8, bottomRight[0][1] * 255.f), SDL_static_cast(Uint8, bottomRight[0][2] * 255.f), 255));
			m_vertices.emplace_back(SDL_static_cast(float, drawX + scale) - 0.5f, SDL_static_cast(float, drawY + halfScale) - 0.5f, MAKE_RGBA_COLOR(SDL_static_cast(Uint8, rightCenter[0][0] * 255.f), SDL_static_cast(Uint8, rightCenter[0][1] * 255.f), SDL_static_cast(Uint8, rightCenter[0][2] * 255.f), 255));

			m_cachedVertices += 24;
			drawX += scale;
		}

		drawY += scale;
	}

	scheduleBatch();
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	if(m_useAlphaBlending)
	{
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
	}
}

void SurfaceDirect3D9::setClipRect(Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
	scheduleBatch();
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SCISSORTESTENABLE, TRUE);

	RECT r;
	r.left = m_viewPortX + x;
	r.top = m_viewPortY + y;
	r.right = m_viewPortX + x + w;
	r.bottom = m_viewPortY + y + h;
	IDirect3DDevice9_SetScissorRect(m_device, &r);
}

void SurfaceDirect3D9::disableClipRect()
{
	scheduleBatch();
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SCISSORTESTENABLE, FALSE);
}

void SurfaceDirect3D9::drawRectangle(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 lineWidth, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	if(m_binded_texture)
	{
		scheduleBatch();
		m_binded_texture = NULL;
	}

	//First Quad
	float minx0 = SDL_static_cast(float, x) - 0.5f;
	float maxx0 = minx0 + SDL_static_cast(float, lineWidth);
	float miny0 = SDL_static_cast(float, y) - 0.5f;
	float maxy0 = miny0 + SDL_static_cast(float, h);

	//Second Quad
	float minx1 = SDL_static_cast(float, x + w - lineWidth) - 0.5f;
	float maxx1 = minx1 + SDL_static_cast(float, lineWidth);
	float miny1 = SDL_static_cast(float, y) - 0.5f;
	float maxy1 = miny1 + SDL_static_cast(float, h);

	//Third Quad
	float minx2 = SDL_static_cast(float, x + lineWidth) - 0.5f;
	float maxx2 = minx2 + SDL_static_cast(float, w - (lineWidth << 1));
	float miny2 = SDL_static_cast(float, y) - 0.5f;
	float maxy2 = miny2 + SDL_static_cast(float, lineWidth);

	//Fourth Quad
	float minx3 = SDL_static_cast(float, x + lineWidth) - 0.5f;
	float maxx3 = minx3 + SDL_static_cast(float, w - (lineWidth << 1));
	float miny3 = SDL_static_cast(float, y + h - lineWidth) - 0.5f;
	float maxy3 = miny3 + SDL_static_cast(float, lineWidth);
	
	DWORD texColor = MAKE_RGBA_COLOR(r, g, b, a);
	m_vertices.emplace_back(minx0, miny0, texColor);
	m_vertices.emplace_back(minx0, maxy0, texColor);
	m_vertices.emplace_back(maxx0, miny0, texColor);
	m_vertices.emplace_back(maxx0, maxy0, texColor);
	m_vertices.emplace_back(minx1, miny1, texColor);
	m_vertices.emplace_back(minx1, maxy1, texColor);
	m_vertices.emplace_back(maxx1, miny1, texColor);
	m_vertices.emplace_back(maxx1, maxy1, texColor);
	m_vertices.emplace_back(minx2, miny2, texColor);
	m_vertices.emplace_back(minx2, maxy2, texColor);
	m_vertices.emplace_back(maxx2, miny2, texColor);
	m_vertices.emplace_back(maxx2, maxy2, texColor);
	m_vertices.emplace_back(minx3, miny3, texColor);
	m_vertices.emplace_back(minx3, maxy3, texColor);
	m_vertices.emplace_back(maxx3, miny3, texColor);
	m_vertices.emplace_back(maxx3, maxy3, texColor);
	m_cachedVertices += 24;
}

void SurfaceDirect3D9::fillRectangle(Sint32 x, Sint32 y, Sint32 w, Sint32 h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	if(m_binded_texture)
	{
		scheduleBatch();
		m_binded_texture = NULL;
	}

	float minx = SDL_static_cast(float, x) - 0.5f;
	float maxx = minx + SDL_static_cast(float, w);
	float miny = SDL_static_cast(float, y) - 0.5f;
	float maxy = miny + SDL_static_cast(float, h);

	DWORD texColor = MAKE_RGBA_COLOR(r, g, b, a);
	m_vertices.emplace_back(minx, miny, texColor);
	m_vertices.emplace_back(minx, maxy, texColor);
	m_vertices.emplace_back(maxx, miny, texColor);
	m_vertices.emplace_back(maxx, maxy, texColor);
	m_cachedVertices += 6;
}

Direct3D9Texture* SurfaceDirect3D9::loadPicture(Uint16 pictureId, bool linear)
{
	Sint32 width, height;
	unsigned char* pixels = g_engine.LoadPicture(pictureId, true, width, height);
	if(!pixels)
		return NULL;

	Direct3D9Texture* s = &m_pictures[pictureId];
	if(!createDirect3DTexture(*s, width, height, linear))
	{
		SDL_free(pixels);
		return NULL;
	}

	updateTextureData(*s, pixels);
	SDL_free(pixels);
	return s;
}

void SurfaceDirect3D9::drawFont(Uint16 pictureId, Sint32 x, Sint32 y, const std::string& text, size_t pos, size_t len, Uint8 r, Uint8 g, Uint8 b, Sint16 cX[256], Sint16 cY[256], Sint16 cW[256], Sint16 cH[256])
{
	Direct3D9Texture* tex = &m_pictures[pictureId];
	if(!tex->m_texture)
	{
		tex = loadPicture(pictureId, false);
		if(!tex)
			return;//load failed
	}

	float vertices[8];
	float texcoords[8];
	DWORD texColor = MAKE_RGBA_COLOR(r, g, b, 255);
	tex = getTextureIndex(tex);

	Sint32 rx = x, ry = y;
	Uint8 character;
	for(size_t i = pos; i < len; ++i)
	{
		character = SDL_static_cast(Uint8, text[i]);
		switch(character)
		{
			case '\r'://carriage return
				rx = x;
				break;
			case '\n'://new line
				rx = x; ry += cH[0];
				break;
			case 0x20://space - don't draw
				rx += cW[character] + cX[0];
				break;
			case 0x0E://Special case - change rendering color
			{
				if(i + 4 < len)//First check if we have the color bytes
				{
					Uint8 red = SDL_static_cast(Uint8, text[i + 1]);
					Uint8 green = SDL_static_cast(Uint8, text[i + 2]);
					Uint8 blue = SDL_static_cast(Uint8, text[i + 3]);
					texColor = MAKE_RGBA_COLOR(red, green, blue, 255);
					i += 3;
				}
				else
					i = len;
			}
			break;
			case 0x0F://Special case - change back standard color
				texColor = MAKE_RGBA_COLOR(r, g, b, 255);
				break;
			default:
			{
				float minx = SDL_static_cast(float, rx) - 0.5f;
				float maxx = minx + SDL_static_cast(float, cW[character]);
				float miny = SDL_static_cast(float, ry) - 0.5f;
				float maxy = miny + SDL_static_cast(float, cH[character]);

				float minu = cX[character] * tex->m_scaleW;
				float maxu = (cX[character] + cW[character]) * tex->m_scaleW;
				float minv = cY[character] * tex->m_scaleH;
				float maxv = (cY[character] + cH[character]) * tex->m_scaleH;

				vertices[0] = minx; vertices[1] = miny;
				vertices[2] = minx; vertices[3] = maxy;
				vertices[4] = maxx; vertices[5] = miny;
				vertices[6] = maxx; vertices[7] = maxy;

				texcoords[0] = minu; texcoords[1] = minv;
				texcoords[2] = minu; texcoords[3] = maxv;
				texcoords[4] = maxu; texcoords[5] = minv;
				texcoords[6] = maxu; texcoords[7] = maxv;

				drawQuad(tex, vertices, texcoords, texColor);
				rx += cW[character] + cX[0];
			}
			break;
		}
	}
}

void SurfaceDirect3D9::drawBackground(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
	Direct3D9Texture* tex = &m_pictures[pictureId];
	if(!tex->m_texture)
	{
		tex = loadPicture(pictureId, g_engine.hasAntialiasing());
		if(!tex)
			return;//load failed
	}

	float minx = SDL_static_cast(float, x) - 0.5f;
	float maxx = minx + SDL_static_cast(float, w);
	float miny = SDL_static_cast(float, y) - 0.5f;
	float maxy = miny + SDL_static_cast(float, h);

	float minu = sx * tex->m_scaleW;
	float maxu = (sx + sw) * tex->m_scaleW;
	float minv = sy * tex->m_scaleH;
	float maxv = (sy + sh) * tex->m_scaleH;

	float vertices[8];
	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	float texcoords[8];
	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;
	drawQuad(getTextureIndex(tex), vertices, texcoords);
}

void SurfaceDirect3D9::drawPictureRepeat(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
	Direct3D9Texture* tex = &m_pictures[pictureId];
	if(!tex->m_texture)
	{
		tex = loadPicture(pictureId, false);
		if(!tex)
			return;//load failed
	}

	float vertices[8];
	float texcoords[8];
	tex = getTextureIndex(tex);

	Sint32 curW, curH, cx;
	for(Sint32 j = h; j > 0; j -= sh)
	{
		curH = (j > sh ? sh : j);
		cx = x;
		for(Sint32 k = w; k > 0; k -= sw)
		{
			curW = (k > sw ? sw : k);
			float minx = SDL_static_cast(float, cx) - 0.5f;
			float maxx = minx + SDL_static_cast(float, curW);
			float miny = SDL_static_cast(float, y) - 0.5f;
			float maxy = miny + SDL_static_cast(float, curH);

			float minu = sx * tex->m_scaleW;
			float maxu = (sx + curW) * tex->m_scaleW;
			float minv = sy * tex->m_scaleH;
			float maxv = (sy + curH) * tex->m_scaleH;

			vertices[0] = minx; vertices[1] = miny;
			vertices[2] = minx; vertices[3] = maxy;
			vertices[4] = maxx; vertices[5] = miny;
			vertices[6] = maxx; vertices[7] = maxy;

			texcoords[0] = minu; texcoords[1] = minv;
			texcoords[2] = minu; texcoords[3] = maxv;
			texcoords[4] = maxu; texcoords[5] = minv;
			texcoords[6] = maxu; texcoords[7] = maxv;

			drawQuad(tex, vertices, texcoords);
			cx += sw;
		}
		y += sh;
	}
}

void SurfaceDirect3D9::drawPicture(Uint16 pictureId, Sint32 sx, Sint32 sy, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
	Direct3D9Texture* tex = &m_pictures[pictureId];
	if(!tex->m_texture)
	{
		tex = loadPicture(pictureId, false);
		if(!tex)
			return;//load failed
	}

	float minx = SDL_static_cast(float, x) - 0.5f;
	float maxx = minx + SDL_static_cast(float, w);
	float miny = SDL_static_cast(float, y) - 0.5f;
	float maxy = miny + SDL_static_cast(float, h);

	float minu = sx * tex->m_scaleW;
	float maxu = (sx + w) * tex->m_scaleW;
	float minv = sy * tex->m_scaleH;
	float maxv = (sy + h) * tex->m_scaleH;

	float vertices[8];
	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	float texcoords[8];
	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;
	drawQuad(getTextureIndex(tex), vertices, texcoords);
}

bool SurfaceDirect3D9::loadSprite(Uint32 spriteId, Direct3D9Texture* texture, Uint32 xoff, Uint32 yoff)
{
	unsigned char* pixels = g_engine.LoadSprite(spriteId, true);
	if(!pixels)
		return false;

	IDirect3DTexture9* stagingTexture;
	HRESULT result = IDirect3DDevice9_CreateTexture(m_device, 32, 32, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &stagingTexture, NULL);
	if(FAILED(result))
	{
		SDL_free(pixels);
		return false;
	}

	D3DLOCKED_RECT locked;
	result = IDirect3DTexture9_LockRect(stagingTexture, 0, &locked, NULL, D3DLOCK_NOSYSLOCK);
	if(FAILED(result))
	{
		SDL_free(pixels);
		IDirect3DTexture9_Release(stagingTexture);
		return false;
	}

	unsigned char* dstData = SDL_reinterpret_cast(unsigned char*, locked.pBits);
	unsigned char* data = pixels;
	int length = 32 * 4;
	if(length == locked.Pitch)
		UTIL_FastCopy(SDL_reinterpret_cast(Uint8*, dstData), SDL_reinterpret_cast(const Uint8*, data), length * 32);
	else
	{
		if(length > locked.Pitch)
			length = locked.Pitch;
		for(Uint32 row = 0; row < 32; ++row)
		{
			UTIL_FastCopy(SDL_reinterpret_cast(Uint8*, dstData), SDL_reinterpret_cast(const Uint8*, data), length);
			data += length;
			dstData += locked.Pitch;
		}
	}
	SDL_free(pixels);

	result = IDirect3DTexture9_UnlockRect(stagingTexture, 0);
	if(FAILED(result))
	{
		IDirect3DTexture9_Release(stagingTexture);
		return false;
	}
	IDirect3DSurface9* destSurface;
	IDirect3DSurface9* stagingSurface;
	POINT destPT; destPT.x = xoff; destPT.y = yoff;
	IDirect3DTexture9_GetSurfaceLevel(stagingTexture, 0, &stagingSurface);
	IDirect3DTexture9_GetSurfaceLevel(texture->m_texture, 0, &destSurface);
	IDirect3DDevice9_UpdateSurface(m_device, stagingSurface, NULL, destSurface, &destPT);
	IDirect3DSurface9_Release(destSurface);
	IDirect3DSurface9_Release(stagingSurface);
	IDirect3DTexture9_Release(stagingTexture);
	return true;
}

bool SurfaceDirect3D9::loadSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Uint32 outfitColor, Direct3D9Texture* texture, Uint32 xoff, Uint32 yoff)
{
	unsigned char* pixels = g_engine.LoadSpriteMask(spriteId, maskSpriteId, outfitColor, true);
	if(!pixels)
		return false;

	IDirect3DTexture9* stagingTexture;
	HRESULT result = IDirect3DDevice9_CreateTexture(m_device, 32, 32, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &stagingTexture, NULL);
	if(FAILED(result))
	{
		SDL_free(pixels);
		return false;
	}

	D3DLOCKED_RECT locked;
	result = IDirect3DTexture9_LockRect(stagingTexture, 0, &locked, NULL, D3DLOCK_NOSYSLOCK);
	if(FAILED(result))
	{
		SDL_free(pixels);
		IDirect3DTexture9_Release(stagingTexture);
		return false;
	}

	unsigned char* dstData = SDL_reinterpret_cast(unsigned char*, locked.pBits);
	unsigned char* data = pixels;
	int length = 32 * 4;
	if(length == locked.Pitch)
		UTIL_FastCopy(SDL_reinterpret_cast(Uint8*, dstData), SDL_reinterpret_cast(const Uint8*, data), length * 32);
	else
	{
		if(length > locked.Pitch)
			length = locked.Pitch;
		for(Uint32 row = 0; row < 32; ++row)
		{
			UTIL_FastCopy(SDL_reinterpret_cast(Uint8*, dstData), SDL_reinterpret_cast(const Uint8*, data), length);
			data += length;
			dstData += locked.Pitch;
		}
	}
	SDL_free(pixels);

	result = IDirect3DTexture9_UnlockRect(stagingTexture, 0);
	if(FAILED(result))
	{
		IDirect3DTexture9_Release(stagingTexture);
		return false;
	}
	IDirect3DSurface9* destSurface;
	IDirect3DSurface9* stagingSurface;
	POINT destPT; destPT.x = xoff; destPT.y = yoff;
	IDirect3DTexture9_GetSurfaceLevel(stagingTexture, 0, &stagingSurface);
	IDirect3DTexture9_GetSurfaceLevel(texture->m_texture, 0, &destSurface);
	IDirect3DDevice9_UpdateSurface(m_device, stagingSurface, NULL, destSurface, &destPT);
	IDirect3DSurface9_Release(destSurface);
	IDirect3DSurface9_Release(stagingSurface);
	IDirect3DTexture9_Release(stagingTexture);
	return true;
}

void SurfaceDirect3D9::drawSprite(Uint32 spriteId, Sint32 x, Sint32 y)
{
	drawSprite(spriteId, x, y, 32, 32, 0, 0, 32, 32);
}

void SurfaceDirect3D9::drawSprite(Uint32 spriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh)
{
	if(spriteId > g_spriteCounts)
		return;

	#if SIZEOF_VOIDP == 4
	Uint64 tempPos;
	Uint32* u32p = SDL_reinterpret_cast(Uint32*, &tempPos);
	u32p[0] = spriteId;
	u32p[1] = 0;
	#else
	Uint64 tempPos = SDL_static_cast(Uint64, spriteId);
	#endif
	Uint32 xOffset;
	Uint32 yOffset;
	Direct3D9Texture* tex;
	U64BD3D9Textures::iterator it = m_sprites.find(tempPos);
	if(it == m_sprites.end())
	{
		if(m_sprites.size() >= MAX_SPRITES)
		{
			while(++m_spriteChecker < MAX_SPRITES)
			{
				Uint64 oldSpriteId = m_spritesIds.front();
				it = m_sprites.find(oldSpriteId);
				if(it == m_sprites.end())
				{
					UTIL_MessageBox(true, "Direct3D9: Sprite Manager failure.");
					exit(-1);
				}

				m_spritesIds.pop_front();
				if(it->second.m_lastUsage == m_currentFrame)
					m_spritesIds.push_back(oldSpriteId);
				else
					goto Skip_Search;
			}
			scheduleBatch();

			it = m_sprites.find(m_spritesIds.front());
			if(it == m_sprites.end())
			{
				UTIL_MessageBox(true, "Direct3D9: Sprite Manager failure.");
				exit(-1);
			}
			m_spritesIds.pop_front();

			Skip_Search:
			Direct3D9SpriteData sprData = it->second;
			sprData.m_lastUsage = m_currentFrame;
			m_sprites.erase(it);
			m_sprites[tempPos] = sprData;
			m_spritesIds.push_back(tempPos);

			xOffset = sprData.m_xOffset;
			yOffset = sprData.m_yOffset;
			tex = &m_spritesAtlas[sprData.m_surface];
		}
		else
		{
			Uint32 spriteIndex = SDL_static_cast(Uint32, m_sprites.size());
			Uint32 spriteAtlas = spriteIndex / m_spritesPerAtlas;
			spriteIndex = (spriteIndex % m_spritesPerAtlas) * 32;
			if(spriteAtlas >= m_spritesAtlas.size())
				return;

			Direct3D9SpriteData& sprData = m_sprites[tempPos];
			sprData.m_xOffset = spriteIndex % m_spritesPerModulo;
			sprData.m_yOffset = (spriteIndex / m_spritesPerModulo) * 32;
			sprData.m_surface = spriteAtlas;
			sprData.m_lastUsage = m_currentFrame;
			m_spritesIds.push_back(tempPos);

			xOffset = sprData.m_xOffset;
			yOffset = sprData.m_yOffset;
			tex = &m_spritesAtlas[sprData.m_surface];
		}
		if(!loadSprite(spriteId, tex, xOffset, yOffset))
			return;//load failed
	}
	else
	{
		xOffset = it->second.m_xOffset;
		yOffset = it->second.m_yOffset;
		tex = &m_spritesAtlas[it->second.m_surface];
		it->second.m_lastUsage = m_currentFrame;
		if(m_spritesIds.front() == tempPos)
		{
			m_spritesIds.pop_front();
			m_spritesIds.push_back(tempPos);
		}
	}

	float minx = SDL_static_cast(float, x) - 0.5f;
	float maxx = minx + SDL_static_cast(float, w);
	float miny = SDL_static_cast(float, y) - 0.5f;
	float maxy = miny + SDL_static_cast(float, h);
	
	sx += SDL_static_cast(Sint32, xOffset);
	sy += SDL_static_cast(Sint32, yOffset);

	float minu = sx * tex->m_scaleW;
	float maxu = (sx + sw) * tex->m_scaleW;
	float minv = sy * tex->m_scaleH;
	float maxv = (sy + sh) * tex->m_scaleH;

	float vertices[8];
	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	float texcoords[8];
	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;
	drawQuad(getTextureIndex(tex), vertices, texcoords);
}

void SurfaceDirect3D9::drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Uint32 outfitColor)
{
	drawSpriteMask(spriteId, maskSpriteId, x, y, 32, 32, 0, 0, 32, 32, outfitColor);
}

void SurfaceDirect3D9::drawSpriteMask(Uint32 spriteId, Uint32 maskSpriteId, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Uint32 outfitColor)
{
	if(spriteId > g_spriteCounts || maskSpriteId > g_spriteCounts)
		return;
	
	#if SIZEOF_VOIDP == 4
	Uint64 tempPos;
	Uint32* u32p = SDL_reinterpret_cast(Uint32*, &tempPos);
	u32p[0] = spriteId;
	u32p[1] = outfitColor;
	#else
	Uint64 tempPos = SDL_static_cast(Uint64, spriteId) | (SDL_static_cast(Uint64, outfitColor) << 32);
	#endif
	Uint32 xOffset;
	Uint32 yOffset;
	Direct3D9Texture* tex;
	U64BD3D9Textures::iterator it = m_sprites.find(tempPos);
	if(it == m_sprites.end())
	{
		if(m_sprites.size() >= MAX_SPRITES)
		{
			while(++m_spriteChecker < MAX_SPRITES)
			{
				Uint64 oldSpriteId = m_spritesIds.front();
				it = m_sprites.find(oldSpriteId);
				if(it == m_sprites.end())
				{
					UTIL_MessageBox(true, "Direct3D9: Sprite Manager failure.");
					exit(-1);
				}

				m_spritesIds.pop_front();
				if(it->second.m_lastUsage == m_currentFrame)
					m_spritesIds.push_back(oldSpriteId);
				else
					goto Skip_Search;
			}
			scheduleBatch();

			it = m_sprites.find(m_spritesIds.front());
			if(it == m_sprites.end())
			{
				UTIL_MessageBox(true, "Direct3D9: Sprite Manager failure.");
				exit(-1);
			}
			m_spritesIds.pop_front();

			Skip_Search:
			Direct3D9SpriteData sprData = it->second;
			sprData.m_lastUsage = m_currentFrame;
			m_sprites.erase(it);
			m_sprites[tempPos] = sprData;
			m_spritesIds.push_back(tempPos);

			xOffset = sprData.m_xOffset;
			yOffset = sprData.m_yOffset;
			tex = &m_spritesAtlas[sprData.m_surface];
		}
		else
		{
			Uint32 spriteIndex = SDL_static_cast(Uint32, m_sprites.size());
			Uint32 spriteAtlas = spriteIndex / m_spritesPerAtlas;
			spriteIndex = (spriteIndex % m_spritesPerAtlas) * 32;
			if(spriteAtlas >= m_spritesAtlas.size())
				return;

			Direct3D9SpriteData& sprData = m_sprites[tempPos];
			sprData.m_xOffset = spriteIndex % m_spritesPerModulo;
			sprData.m_yOffset = (spriteIndex / m_spritesPerModulo) * 32;
			sprData.m_surface = spriteAtlas;
			sprData.m_lastUsage = m_currentFrame;
			m_spritesIds.push_back(tempPos);

			xOffset = sprData.m_xOffset;
			yOffset = sprData.m_yOffset;
			tex = &m_spritesAtlas[sprData.m_surface];
		}
		if(!loadSpriteMask(spriteId, maskSpriteId, outfitColor, tex, xOffset, yOffset))
			return;//load failed
	}
	else
	{
		xOffset = it->second.m_xOffset;
		yOffset = it->second.m_yOffset;
		tex = &m_spritesAtlas[it->second.m_surface];
		it->second.m_lastUsage = m_currentFrame;
		if(m_spritesIds.front() == tempPos)
		{
			m_spritesIds.pop_front();
			m_spritesIds.push_back(tempPos);
		}
	}

	float minx = SDL_static_cast(float, x) - 0.5f;
	float maxx = minx + SDL_static_cast(float, w);
	float miny = SDL_static_cast(float, y) - 0.5f;
	float maxy = miny + SDL_static_cast(float, h);
	
	sx += SDL_static_cast(Sint32, xOffset);
	sy += SDL_static_cast(Sint32, yOffset);

	float minu = sx * tex->m_scaleW;
	float maxu = (sx + sw) * tex->m_scaleW;
	float minv = sy * tex->m_scaleH;
	float maxv = (sy + sh) * tex->m_scaleH;

	float vertices[8];
	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	float texcoords[8];
	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;
	drawQuad(getTextureIndex(tex), vertices, texcoords);
}

Direct3D9Texture* SurfaceDirect3D9::createAutomapTile(Uint32 currentArea)
{
	std::unique_ptr<Direct3D9Texture> s;
	if(m_automapTiles.size() >= MAX_AUTOMAPTILES)
	{
		U32BD3D9Textures::iterator it = m_automapTiles.find(m_automapTilesBuff.front());
		if(it == m_automapTiles.end())
		{
			UTIL_MessageBox(true, "Direct3D9: Sprite Manager failure.");
			exit(-1);
		}

		s = std::make_unique<Direct3D9Texture>(std::move(it->second));
		m_automapTiles.erase(it);
		m_automapTilesBuff.pop_front();
	}
	if(!s)
	{
		s = std::make_unique<Direct3D9Texture>();
		if(!createDirect3DTexture(*s, 256, 256, false))
			return NULL;
	}
	else
		scheduleBatch();

	m_automapTiles[currentArea] = std::move(*s.get());
	m_automapTilesBuff.push_back(currentArea);
	return &m_automapTiles[currentArea];
}

void SurfaceDirect3D9::uploadAutomapTile(Direct3D9Texture* texture, Uint8 color[256][256])
{
	unsigned char* pixels = SDL_reinterpret_cast(unsigned char*, SDL_malloc(256 * 256 * 4));
	if(!pixels)
		return;

	unsigned char* ptrpixels = pixels;
	for(Sint32 y = 0; y < 256; ++y)
	{
		for(Sint32 x = 0; x < 256; ++x)
		{
			Uint8 r, g, b;
			Uint8 c = color[x][y];
			colorFrom8bit(c, r, g, b);
			*ptrpixels++ = b;
			*ptrpixels++ = g;
			*ptrpixels++ = r;
			*ptrpixels++ = 255;
		}
	}
	updateTextureData(*texture, pixels);
	SDL_free(pixels);
}

void SurfaceDirect3D9::drawAutomapTile(Uint32 currentArea, bool& recreate, Uint8 color[256][256], Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh)
{
	Direct3D9Texture* tex;
	U32BD3D9Textures::iterator it = m_automapTiles.find(currentArea);
	if(it == m_automapTiles.end())
	{
		tex = createAutomapTile(currentArea);
		if(!tex)
			return;//create failed

		uploadAutomapTile(tex, color);
		recreate = false;
	}
	else
	{
		tex = &it->second;
		if(m_automapTilesBuff.front() == currentArea)
		{
			m_automapTilesBuff.pop_front();
			m_automapTilesBuff.push_back(currentArea);
		}
		if(recreate)
		{
			uploadAutomapTile(tex, color);
			recreate = false;
		}
	}

	float minx = SDL_static_cast(float, x) - 0.5f;
	float maxx = minx + SDL_static_cast(float, w);
	float miny = SDL_static_cast(float, y) - 0.5f;
	float maxy = miny + SDL_static_cast(float, h);

	float minu = sx * tex->m_scaleW;
	float maxu = (sx + sw) * tex->m_scaleW;
	float minv = sy * tex->m_scaleH;
	float maxv = (sy + sh) * tex->m_scaleH;

	float vertices[8];
	vertices[0] = minx; vertices[1] = miny;
	vertices[2] = minx; vertices[3] = maxy;
	vertices[4] = maxx; vertices[5] = miny;
	vertices[6] = maxx; vertices[7] = maxy;

	float texcoords[8];
	texcoords[0] = minu; texcoords[1] = minv;
	texcoords[2] = minu; texcoords[3] = maxv;
	texcoords[4] = maxu; texcoords[5] = minv;
	texcoords[6] = maxu; texcoords[7] = maxv;
	drawQuad(getTextureIndex(tex), vertices, texcoords);
}
#endif
