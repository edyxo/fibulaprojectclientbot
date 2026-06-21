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

#include "GUI_CharacterView.h"
#include "../thingManager.h"
#include "../engine.h"

extern ThingManager g_thingManager;
extern Engine g_engine;
extern Uint32 g_frameTime;

GUI_CharacterView::GUI_CharacterView(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

void GUI_CharacterView::render()
{
	std::vector<CharacterDetail>& accountCharList = g_engine.getAccountCharList();
	Sint32 lastSelectId = g_engine.getCharacterSelectId();

	auto& renderer = g_engine.getRender();
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_DARK_X, GUI_UI_ICON_HORIZONTAL_LINE_DARK_Y, GUI_UI_ICON_HORIZONTAL_LINE_DARK_W, GUI_UI_ICON_HORIZONTAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1, m_tRect.x2, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_DARK_X, GUI_UI_ICON_VERTICAL_LINE_DARK_Y, GUI_UI_ICON_VERTICAL_LINE_DARK_W, GUI_UI_ICON_VERTICAL_LINE_DARK_H, m_tRect.x1, m_tRect.y1 + 1, 1, m_tRect.y2 - 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_X, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_Y, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_W, GUI_UI_ICON_HORIZONTAL_LINE_BRIGHT_H, m_tRect.x1 + 1, m_tRect.y1 + m_tRect.y2 - 1, m_tRect.x2 - 1, 1);
	renderer->drawPictureRepeat(GUI_UI_IMAGE, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_X, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_Y, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_W, GUI_UI_ICON_VERTICAL_LINE_BRIGHT_H, m_tRect.x1 + m_tRect.x2 - 1, m_tRect.y1 + 1, 1, m_tRect.y2 - 2);
	renderer->fillRectangle(m_tRect.x1 + 1, m_tRect.y1 + 1, m_tRect.x2 - 2, m_tRect.y2 - 2, 64, 64, 64, 255);
	renderer->setClipRect(m_tRect.x1 + 1, m_tRect.y1 + 1, m_tRect.x2 - 2, m_tRect.y2 - 2);

	CharacterDetail& character = accountCharList[SDL_static_cast(size_t, lastSelectId)];
	ThingType* thingType = g_thingManager.getThingType(ThingCategory_Creature, character.lookType);
	if(thingType)
	{
		Uint8 currentAnim = 0;
		if(thingType->hasFlag(ThingAttribute_AnimateAlways))
		{
			Sint32 ticks = (1000 / thingType->m_frameGroup[ThingFrameGroup_Idle].m_animCount);
			currentAnim = UTIL_safeMod<Uint8>(SDL_static_cast(Uint8, (g_frameTime / ticks)), thingType->m_frameGroup[ThingFrameGroup_Idle].m_animCount);
		}

		if(thingType->m_frameGroup[ThingFrameGroup_Idle].m_layers > 1)
		{
			Uint32 outfitColors = (character.lookFeet << 24) | (character.lookLegs << 16) | (character.lookBody << 8) | (character.lookHead);
			Sint32 scale = 9 + thingType->m_frameGroup[ThingFrameGroup_Idle].m_realSize / 2;
			Sint32 drawY = m_tRect.y1 + scale;
			for(Uint8 y = 0; y < thingType->m_frameGroup[ThingFrameGroup_Idle].m_height; ++y)
			{
				Sint32 drawX = m_tRect.x1 + scale;
				for(Uint8 x = 0; x < thingType->m_frameGroup[ThingFrameGroup_Idle].m_width; ++x)
				{
					Uint32 sprite = thingType->getSprite(ThingFrameGroup_Idle, x, y, 0, DIRECTION_SOUTH, 0, 0, currentAnim);
					Uint32 spriteMask = thingType->getSprite(ThingFrameGroup_Idle, x, y, 1, DIRECTION_SOUTH, 0, 0, currentAnim);
					if(sprite != 0)
					{
						if(spriteMask != 0)
							renderer->drawSpriteMask(sprite, spriteMask, drawX, drawY, outfitColors);
						else
							renderer->drawSprite(sprite, drawX, drawY);
					}
					if(thingType->m_frameGroup[ThingFrameGroup_Idle].m_patternY > 1)
					{
						if(character.lookAddons & 1)//First addon
						{
							sprite = thingType->getSprite(ThingFrameGroup_Idle, x, y, 0, DIRECTION_SOUTH, 1, 0, currentAnim);
							spriteMask = thingType->getSprite(ThingFrameGroup_Idle, x, y, 1, DIRECTION_SOUTH, 1, 0, currentAnim);
							if(sprite != 0)
							{
								if(spriteMask != 0)
									renderer->drawSpriteMask(sprite, spriteMask, drawX, drawY, outfitColors);
								else
									renderer->drawSprite(sprite, drawX, drawY);
							}
						}
						if(character.lookAddons & 2)//Second addon
						{
							sprite = thingType->getSprite(ThingFrameGroup_Idle, x, y, 0, DIRECTION_SOUTH, 2, 0, currentAnim);
							spriteMask = thingType->getSprite(ThingFrameGroup_Idle, x, y, 1, DIRECTION_SOUTH, 2, 0, currentAnim);
							if(sprite != 0)
							{
								if(spriteMask != 0)
									renderer->drawSpriteMask(sprite, spriteMask, drawX, drawY, outfitColors);
								else
									renderer->drawSprite(sprite, drawX, drawY);
							}
						}
					}
					drawX -= 32;
				}
				drawY -= 32;
			}
		}
		else
		{
			Sint32 scale = 9 + thingType->m_frameGroup[ThingFrameGroup_Idle].m_realSize / 2;
			Sint32 drawY = m_tRect.y1 + scale;
			for(Uint8 y = 0; y < thingType->m_frameGroup[ThingFrameGroup_Idle].m_height; ++y)
			{
				Sint32 drawX = m_tRect.x1 + scale;
				for(Uint8 x = 0; x < thingType->m_frameGroup[ThingFrameGroup_Idle].m_width; ++x)
				{
					Uint32 sprite = thingType->getSprite(ThingFrameGroup_Idle, x, y, 0, DIRECTION_SOUTH, 0, 0, currentAnim);
					if(sprite != 0)
						renderer->drawSprite(sprite, drawX, drawY);
					if(thingType->m_frameGroup[ThingFrameGroup_Idle].m_patternY > 1)
					{
						if(character.lookAddons & 1)//First addon
						{
							sprite = thingType->getSprite(ThingFrameGroup_Idle, x, y, 0, DIRECTION_SOUTH, 1, 0, currentAnim);
							if(sprite != 0)
								renderer->drawSprite(sprite, drawX, drawY);
						}
						if(character.lookAddons & 2)//Second addon
						{
							sprite = thingType->getSprite(ThingFrameGroup_Idle, x, y, 0, DIRECTION_SOUTH, 2, 0, currentAnim);
							if(sprite != 0)
								renderer->drawSprite(sprite, drawX, drawY);
						}
					}
					drawX -= 32;
				}
				drawY -= 32;
			}
		}
	}

	Sint32 len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u Level", character.level);
	g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 140, m_tRect.y1 + 16, std::string(g_buffer, SDL_static_cast(size_t, len)), 180, 180, 180, CLIENT_FONT_ALIGN_CENTER);
	len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u/%u HP", character.health, character.healthMax);
	g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 140, m_tRect.y1 + 30, std::string(g_buffer, SDL_static_cast(size_t, len)), 180, 180, 180, CLIENT_FONT_ALIGN_CENTER);
	len = SDL_snprintf(g_buffer, sizeof(g_buffer), "%u/%u MP", character.mana, character.manaMax);
	g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 140, m_tRect.y1 + 46, std::string(g_buffer, SDL_static_cast(size_t, len)), 180, 180, 180, CLIENT_FONT_ALIGN_CENTER);
	g_engine.drawFont(CLIENT_FONT_NONOUTLINED, m_tRect.x1 + 140, m_tRect.y1 + 62, character.vocName, 180, 180, 180, CLIENT_FONT_ALIGN_CENTER);
	renderer->disableClipRect();
}
