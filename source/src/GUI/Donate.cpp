/*
  The Forgotten Client
  Copyright (C) 2020 Saiyans King
*/

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../GUI_Elements/GUI_Window.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_Element.h"

#define DONATE_TITLE "Support / Donate"
#define DONATE_WIDTH 320
#define DONATE_HEIGHT 380

#define DONATE_OK_EVENTID 1000

extern Engine g_engine;

class GUI_DonateQRCode : public GUI_Element
{
	public:
		GUI_DonateQRCode(iRect boxRect, Uint32 internalID = 0) : m_description("Binance Pay QR")
		{
			m_internalID = internalID;
			setRect(boxRect);
			loadBitmap();
		}

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
		{
			if(isInsideParent && m_tRect.isPointInside(x, y))
				g_engine.showDescription(x, y, m_description);
		}

		void render()
		{
			Surface* renderer = g_engine.getRender().get();
			if(!renderer)
				return;
			renderer->fillRectangle(m_tRect.x1, m_tRect.y1, m_tRect.x2, m_tRect.y2, 255, 255, 255, 255);
			renderer->fillRectangle(m_tRect.x1, m_tRect.y1, m_tRect.x2, 1, 0, 0, 0, 255);
			renderer->fillRectangle(m_tRect.x1, m_tRect.y1 + m_tRect.y2 - 1, m_tRect.x2, 1, 0, 0, 0, 255);
			renderer->fillRectangle(m_tRect.x1, m_tRect.y1, 1, m_tRect.y2, 0, 0, 0, 255);
			renderer->fillRectangle(m_tRect.x1 + m_tRect.x2 - 1, m_tRect.y1, 1, m_tRect.y2, 0, 0, 0, 255);

			if(!m_loaded)
			{
				renderer->fillRectangle(m_tRect.x1 + 8, m_tRect.y1 + 8, m_tRect.x2 - 16, m_tRect.y2 - 16, 224, 224, 224, 255);
				return;
			}

			for(Sint32 y = 0; y < m_tRect.y2; ++y)
			{
				Sint32 srcY = (y * m_height) / m_tRect.y2;
				Sint32 x = 0;
				while(x < m_tRect.x2)
				{
					Sint32 srcX = (x * m_width) / m_tRect.x2;
					if(!m_dark[SDL_static_cast(size_t, srcY * m_width + srcX)])
					{
						++x;
						continue;
					}

					Sint32 startX = x;
					do
					{
						++x;
						if(x >= m_tRect.x2)
							break;

						srcX = (x * m_width) / m_tRect.x2;
					} while(m_dark[SDL_static_cast(size_t, srcY * m_width + srcX)]);

					renderer->fillRectangle(m_tRect.x1 + startX, m_tRect.y1 + y, x - startX, 1, 0, 0, 0, 255);
				}
			}
		}

	protected:
		void loadBitmap()
		{
			SDL_snprintf(g_buffer, sizeof(g_buffer), "%sbinance-qr.bmp", g_basePath.c_str());
			SDL_Surface* surface = SDL_LoadBMP(g_buffer);
			if(!surface)
				return;

			SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, SDL_SWSURFACE);
			SDL_FreeSurface(surface);
			if(!converted)
				return;

			m_width = converted->w;
			m_height = converted->h;
			if(m_width <= 0 || m_height <= 0)
			{
				SDL_FreeSurface(converted);
				return;
			}

			m_dark.assign(SDL_static_cast(size_t, m_width * m_height), 0);
			if(SDL_MUSTLOCK(converted) && SDL_LockSurface(converted) < 0)
			{
				SDL_FreeSurface(converted);
				return;
			}

			for(Sint32 y = 0; y < m_height; ++y)
			{
				Uint8* row = SDL_reinterpret_cast(Uint8*, converted->pixels) + y * converted->pitch;
				for(Sint32 x = 0; x < m_width; ++x)
				{
					Uint32 pixel = *SDL_reinterpret_cast(Uint32*, row + x * 4);
					Uint8 r, g, b, a;
					SDL_GetRGBA(pixel, converted->format, &r, &g, &b, &a);
					Uint32 luma = SDL_static_cast(Uint32, r) + SDL_static_cast(Uint32, g) + SDL_static_cast(Uint32, b);
					m_dark[SDL_static_cast(size_t, y * m_width + x)] = (a > 32 && luma < 384 ? 1 : 0);
				}
			}

			if(SDL_MUSTLOCK(converted))
				SDL_UnlockSurface(converted);

			SDL_FreeSurface(converted);
			m_loaded = true;
		}

		std::string m_description;
		std::vector<Uint8> m_dark;
		Sint32 m_width = 0;
		Sint32 m_height = 0;
		bool m_loaded = false;
};

static void donate_Events(Uint32 event, Sint32)
{
	if(event == DONATE_OK_EVENTID)
	{
		GUI_Window* pWindow = g_engine.getCurrentWindow();
		if(pWindow && pWindow->getInternalID() == GUI_WINDOW_DONATE)
			g_engine.removeWindow(pWindow);
	}
}

void UTIL_donateOptions()
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_DONATE);
	if(pWindow)
	{
		g_engine.removeWindow(pWindow);
		return;
	}

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, DONATE_WIDTH, DONATE_HEIGHT), DONATE_TITLE, GUI_WINDOW_DONATE);

	GUI_Label* label = new GUI_Label(iRect(18, 28, 0, 0), "Thanks for playing! If you enjoy this client,");
	newWindow->addChild(label);
	label = new GUI_Label(iRect(18, 44, 0, 0), "consider supporting the developer:");
	newWindow->addChild(label);

	GUI_Separator* separator = new GUI_Separator(iRect(14, 62, DONATE_WIDTH - 28, 2));
	newWindow->addChild(separator);

	label = new GUI_Label(iRect(18, 76, 0, 0), "Binance Pay:");
	newWindow->addChild(label);
	label = new GUI_Label(iRect(120, 76, 0, 0), "eddiexo");
	newWindow->addChild(label);

	GUI_DonateQRCode* qrImage = new GUI_DonateQRCode(iRect(60, 100, 200, 200));
	newWindow->addChild(qrImage);

	label = new GUI_Label(iRect(18, 316, 0, 0), "Telegram:");
	newWindow->addChild(label);
	label = new GUI_Label(iRect(120, 316, 0, 0), "@edyxo");
	newWindow->addChild(label);

	label = new GUI_Label(iRect(18, 336, 0, 0), "Want the source code? DM me, we can talk $$$");
	newWindow->addChild(label);

	GUI_Separator* separator2 = new GUI_Separator(iRect(14, 350, DONATE_WIDTH - 28, 2));
	newWindow->addChild(separator2);

	GUI_Button* button = new GUI_Button(iRect(DONATE_WIDTH - 58, 358, 43, 20), "Ok", CLIENT_GUI_ENTER_TRIGGER);
	button->setButtonEventCallback(&donate_Events, DONATE_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	button = new GUI_Button(iRect(DONATE_WIDTH - 58, 358, 43, 20), "Ok", CLIENT_GUI_ESCAPE_TRIGGER);
	button->setButtonEventCallback(&donate_Events, DONATE_OK_EVENTID);
	button->startEvents();
	newWindow->addChild(button);

	g_engine.addWindow(newWindow, true);
}
