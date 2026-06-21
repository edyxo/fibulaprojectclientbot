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

#include "GUI_UTIL.h"
#include "../engine.h"
#include "../GUI_Elements/GUI_Window.h"
#include "../GUI_Elements/GUI_Button.h"
#include "../GUI_Elements/GUI_Separator.h"
#include "../GUI_Elements/GUI_Label.h"
#include "../GUI_Elements/GUI_MultiTextBox.h"
#include "../GUI_Elements/GUI_StaticImage.h"
#include "../game.h"
#include "../thingManager.h"
#include "ReadWrite.h"

#define TEXT_WINDOW_TITLE1 "Show Text"
#define TEXT_WINDOW_TITLE2 "Edit Text"
#define TEXT_WINDOW_TITLE3 "Edit List"
#define TEXT_WINDOW_WIDTH 286
#define TEXT_WINDOW_HEIGHT 284
#define TEXT_WINDOW_CANCEL_EVENTID 1000
#define TEXT_WINDOW_WRITE_EVENTID 1001
#define TEXT_WINDOW_EDIT_EVENTID 1002
#define TEXT_WINDOW_DESCRIPTION_X 62
#define TEXT_WINDOW_DESCRIPTION_Y 32
#define TEXT_WINDOW_ITEM_X 18
#define TEXT_WINDOW_ITEM_Y 32
#define TEXT_WINDOW_ITEM_W GUI_UI_ICON_EDITLIST_W
#define TEXT_WINDOW_ITEM_H GUI_UI_ICON_EDITLIST_H
#define TEXT_WINDOW_TEXTBOX_X 18
#define TEXT_WINDOW_TEXTBOX_Y 92
#define TEXT_WINDOW_TEXTBOX_W 250
#define TEXT_WINDOW_TEXTBOX_H 132
#define TEXT_WINDOW_TEXTBOX_EVENTID 1003

extern Engine g_engine;
extern Game g_game;

Uint32 g_windowId = 0;
Uint8 g_doorId = 0;

void readwrite_Events(Uint32 event, Sint32)
{
	switch(event)
	{
		case TEXT_WINDOW_CANCEL_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_READWRITE)
				g_engine.removeWindow(pWindow);
		}
		break;
		case TEXT_WINDOW_WRITE_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_READWRITE)
			{
				GUI_MultiTextBox* pTextBox = SDL_static_cast(GUI_MultiTextBox*, pWindow->getChild(TEXT_WINDOW_TEXTBOX_EVENTID));
				if(pTextBox)
					g_game.sendTextWindow(g_windowId, pTextBox->getText());
				
				g_engine.removeWindow(pWindow);
			}
		}
		break;
		case TEXT_WINDOW_EDIT_EVENTID:
		{
			GUI_Window* pWindow = g_engine.getCurrentWindow();
			if(pWindow && pWindow->getInternalID() == GUI_WINDOW_READWRITE)
			{
				GUI_MultiTextBox* pTextBox = SDL_static_cast(GUI_MultiTextBox*, pWindow->getChild(TEXT_WINDOW_TEXTBOX_EVENTID));
				if(pTextBox)
					g_game.sendHouseWindow(g_windowId, g_doorId, pTextBox->getText());

				g_engine.removeWindow(pWindow);
			}
		}
		break;
		default: break;
	}
}

void UTIL_createReadWriteWindow(Uint32 windowId, void* item, Uint16 maxLen, const std::string& text, const std::string& writer, const std::string& date)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_READWRITE);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	g_windowId = windowId;

	Uint16 maxLength = maxLen;
	bool writeAble = false;
	ItemUI* itemUI = SDL_reinterpret_cast(ItemUI*, item);
	if(itemUI && itemUI->getThingType())
	{
		ThingType* itemType = itemUI->getThingType();
		if(itemType->hasFlag(ThingAttribute_WritableOnce) || itemType->hasFlag(ThingAttribute_Writable))
		{
			//Should we leave maxlen that we got from server?
			maxLength = itemType->m_writableSize;
			writeAble = (itemType->hasFlag(ThingAttribute_WritableOnce) ? text.empty() : true);
		}
	}
	if(!text.empty())
	{
		if(!writer.empty())
		{
			if(!date.empty())
				SDL_snprintf(g_buffer, sizeof(g_buffer), "You read the following, written by \n%s\non %s.", writer.c_str(), date.c_str());
			else
				SDL_snprintf(g_buffer, sizeof(g_buffer), "\nYou read the following, written by \n%s", writer.c_str());
		}
		else if(!date.empty())
			SDL_snprintf(g_buffer, sizeof(g_buffer), "\nYou read the following, written on \n%s.", date.c_str());
		else
			SDL_snprintf(g_buffer, sizeof(g_buffer), "\n\nYou read the following:");
	}
	else
	{
		if(writeAble)
			SDL_snprintf(g_buffer, sizeof(g_buffer), "\n\nIt is currently empty.");
		else
			SDL_snprintf(g_buffer, sizeof(g_buffer), "\n\nIt is empty.");
	}

	std::string description(g_buffer);
	if(writeAble)
		description.append("\nYou can enter new text.");
	else
		description.insert(0, "\n");

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, TEXT_WINDOW_WIDTH, TEXT_WINDOW_HEIGHT), (writeAble ? TEXT_WINDOW_TITLE2 : TEXT_WINDOW_TITLE1), GUI_WINDOW_READWRITE);
	GUI_Label* newLabel = new GUI_Label(iRect(TEXT_WINDOW_DESCRIPTION_X, TEXT_WINDOW_DESCRIPTION_Y, 0, 0), std::move(description));
	newWindow->addChild(newLabel);
	GUI_ReadWriteItem* newImage = new GUI_ReadWriteItem(iRect(TEXT_WINDOW_ITEM_X, TEXT_WINDOW_ITEM_Y, TEXT_WINDOW_ITEM_W, TEXT_WINDOW_ITEM_H), itemUI);
	newWindow->addChild(newImage);
	GUI_MultiTextBox* newTextBox = new GUI_MultiTextBox(iRect(TEXT_WINDOW_TEXTBOX_X, TEXT_WINDOW_TEXTBOX_Y, TEXT_WINDOW_TEXTBOX_W, TEXT_WINDOW_TEXTBOX_H), writeAble, text, TEXT_WINDOW_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(SDL_static_cast(Uint32, maxLength));
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	if(writeAble)
	{
		GUI_Button* newButton = new GUI_Button(iRect(TEXT_WINDOW_WIDTH - 56, TEXT_WINDOW_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
		newButton->setButtonEventCallback(&readwrite_Events, TEXT_WINDOW_CANCEL_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
		newButton = new GUI_Button(iRect(TEXT_WINDOW_WIDTH - 109, TEXT_WINDOW_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok");
		newButton->setButtonEventCallback(&readwrite_Events, TEXT_WINDOW_WRITE_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
	}
	else
	{
		GUI_Button* newButton = new GUI_Button(iRect(TEXT_WINDOW_WIDTH - 56, TEXT_WINDOW_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ESCAPE_TRIGGER);
		newButton->setButtonEventCallback(&readwrite_Events, TEXT_WINDOW_CANCEL_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
		newButton = new GUI_Button(iRect(TEXT_WINDOW_WIDTH - 56, TEXT_WINDOW_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok", CLIENT_GUI_ENTER_TRIGGER);
		newButton->setButtonEventCallback(&readwrite_Events, TEXT_WINDOW_CANCEL_EVENTID);
		newButton->startEvents();
		newWindow->addChild(newButton);
		newTextBox->setColor(175, 175, 175);
	}
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, TEXT_WINDOW_HEIGHT - 40, TEXT_WINDOW_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

void UTIL_createReadWriteWindow(Uint8 doorId, Uint32 windowId, const std::string& text)
{
	GUI_Window* pWindow = g_engine.getWindow(GUI_WINDOW_READWRITE);
	if(pWindow)
		g_engine.removeWindow(pWindow);

	g_windowId = windowId;
	g_doorId = doorId;

	GUI_Window* newWindow = new GUI_Window(iRect(0, 0, TEXT_WINDOW_WIDTH, TEXT_WINDOW_HEIGHT), TEXT_WINDOW_TITLE3, GUI_WINDOW_READWRITE);
	GUI_Label* newLabel = new GUI_Label(iRect(TEXT_WINDOW_DESCRIPTION_X, TEXT_WINDOW_DESCRIPTION_Y, 0, 0), "\n\n\nEnter one name per line.");
	newWindow->addChild(newLabel);
	GUI_StaticImage* newImage = new GUI_StaticImage(iRect(TEXT_WINDOW_ITEM_X, TEXT_WINDOW_ITEM_Y, TEXT_WINDOW_ITEM_W, TEXT_WINDOW_ITEM_H), GUI_UI_IMAGE, GUI_UI_ICON_EDITLIST_X, GUI_UI_ICON_EDITLIST_Y);
	newWindow->addChild(newImage);
	GUI_MultiTextBox* newTextBox = new GUI_MultiTextBox(iRect(TEXT_WINDOW_TEXTBOX_X, TEXT_WINDOW_TEXTBOX_Y, TEXT_WINDOW_TEXTBOX_W, TEXT_WINDOW_TEXTBOX_H), true, text, TEXT_WINDOW_TEXTBOX_EVENTID);
	newTextBox->setMaxLength(8192);
	newTextBox->startEvents();
	newWindow->addChild(newTextBox);
	GUI_Button* newButton = new GUI_Button(iRect(TEXT_WINDOW_WIDTH - 56, TEXT_WINDOW_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Cancel", CLIENT_GUI_ESCAPE_TRIGGER);
	newButton->setButtonEventCallback(&readwrite_Events, TEXT_WINDOW_CANCEL_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	newButton = new GUI_Button(iRect(TEXT_WINDOW_WIDTH - 109, TEXT_WINDOW_HEIGHT - 30, GUI_UI_BUTTON_43PX_GRAY_UP_W, GUI_UI_BUTTON_43PX_GRAY_UP_H), "Ok");
	newButton->setButtonEventCallback(&readwrite_Events, TEXT_WINDOW_EDIT_EVENTID);
	newButton->startEvents();
	newWindow->addChild(newButton);
	GUI_Separator* newSeparator = new GUI_Separator(iRect(13, TEXT_WINDOW_HEIGHT - 40, TEXT_WINDOW_WIDTH - 26, 2));
	newWindow->addChild(newSeparator);
	g_engine.addWindow(newWindow);
}

GUI_ReadWriteItem::GUI_ReadWriteItem(iRect boxRect, ItemUI* item, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
	m_item = item;
}

GUI_ReadWriteItem::~GUI_ReadWriteItem()
{
	if(m_item)
		delete m_item;
}

void GUI_ReadWriteItem::render()
{
	if(m_item)
		m_item->render(m_tRect.x1, m_tRect.y1, m_tRect.y2);
}
