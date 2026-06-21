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

#include "container.h"
#include "engine.h"
#include "game.h"

#include "GUI/itemUI.h"

extern Engine g_engine;
extern Game g_game;
extern Uint32 g_frameTime;

Container::~Container()
{
	if(m_containerItem)
		delete m_containerItem;

	for(std::vector<ItemUI*>::iterator it = m_items.begin(), end = m_items.end(); it != end; ++it)
		delete (*it);
}

ItemUI* Container::getItem(size_t index)
{
	if(index < m_items.size())
		return m_items[index];

	return NULL;
}

void Container::addItem(Uint16 slot, ItemUI* item)
{
	if(slot < m_firstIndex)
	{
		delete item;
		return;
	}

	slot -= m_firstIndex;
	++m_size;
	if(m_hasPages)
	{
		m_dirty = true;
		if(slot >= SDL_static_cast(Uint16, m_capacity))
		{
			delete item;
			return;
		}
	}

	m_items.insert(m_items.begin() + SDL_static_cast(size_t, slot), item);
	updatePositions();
}

void Container::setItems(std::vector<ItemUI*>& items)
{
	m_items = std::move(items);
	updatePositions();
}

void Container::transformItem(Uint16 slot, ItemUI* item)
{
	if(slot < m_firstIndex)
	{
		delete item;
		return;
	}

	slot -= m_firstIndex;
	if(SDL_static_cast(size_t, slot) >= m_items.size())
	{
		delete item;
		return;
	}

	ItemUI* oldItem = m_items[slot];
	m_items[slot] = item;

	Position position = Position(0xFFFF, (SDL_static_cast(Uint16, m_id) | 0x40), SDL_static_cast(Uint8, slot));
	item->setCurrentPosition(position);
	delete oldItem;
}

void Container::removeItem(Uint16 slot, ItemUI* lastItem)
{
	if(slot < m_firstIndex)
	{
		if(m_hasPages)
		{
			--m_size;
			m_dirty = true;
		}

		if(lastItem)
			delete lastItem;
		return;
	}

	slot -= m_firstIndex;
	if(SDL_static_cast(size_t, slot) >= m_items.size())
	{
		if(m_hasPages)
		{
			--m_size;
			m_dirty = true;
		}

		if(lastItem)
			delete lastItem;
		return;
	}
	else if(m_hasPages)
		m_dirty = true;

	bool needUpdate;
	if(!g_game.hasGameFeature(GAME_FEATURE_CONTAINER_PAGINATION))
		needUpdate = (m_items.size() >= SDL_static_cast(size_t, m_capacity));
	else
		needUpdate = false;

	ItemUI* oldItem = m_items[slot];
	m_items.erase(m_items.begin() + slot);
	if(lastItem)
	{
		addItem(m_firstIndex + SDL_static_cast(Uint16, m_capacity) - 1, lastItem);
		--m_size;
	}
	--m_size;

	updatePositions();
	if(needUpdate)
		g_game.sendUpdateContainer(m_id);

	delete oldItem;
}

void Container::updatePositions()
{
	Position containerPosition = Position(0xFFFF, (SDL_static_cast(Uint16, m_id) | 0x40), 0);
	for(size_t i = 0, end = m_items.size(); i < end; ++i)
	{
		containerPosition.z = SDL_static_cast(Uint8, i);
		m_items[i]->setCurrentPosition(containerPosition);
	}
}
