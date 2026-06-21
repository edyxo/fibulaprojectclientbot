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

#ifndef __FILE_SOFTWARE_DRAWNING_h_
#define __FILE_SOFTWARE_DRAWNING_h_

#include "defines.h"

Sint32 SDL_FillRect_BLEND(SDL_Surface* dst, SDL_Rect* dstrect, Uint32 RGBA, Uint32 A);
Sint32 SDL_FillRect_MOD(SDL_Surface* dst, SDL_Rect* dstrect, Uint32 R, Uint32 G, Uint32 B);

//Triangle Rasterizer for light
typedef void (*LPSDL_DrawTriangle_MOD)(SDL_Surface* dst, const float colors[3][3], Sint32 vertices[3][2]);
extern LPSDL_DrawTriangle_MOD SDL_DrawTriangle_MOD;

//BiLinear Interpolation
typedef Sint32 (*LPSDL_SmoothStretch)(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect);
extern LPSDL_SmoothStretch SDL_SmoothStretch;

void SDL_DrawLightMap_old_MT(SDL_Surface* src, LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height);
void SDL_DrawLightMap_new_MT(SDL_Surface* src, LightMap* lightmap, Sint32 x, Sint32 y, Sint32 scale, Sint32 width, Sint32 height);
Sint32 SDL_SmoothStretch_MT(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect);

void SDL_SmoothStretch_init();
void SDL_SmoothStretch_shutdown();

#endif /* __FILE_SURFACE_SOFTWARE_h_ */
