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

#ifndef __FILE_GUI_CONTAINERWINDOW_h_
#define __FILE_GUI_CONTAINERWINDOW_h_

#include "../GUI_Elements/GUI_Container.h"

class GUI_ContainerImage : public GUI_Element
{
	public:
		GUI_ContainerImage(iRect boxRect, Uint8 index, Uint32 internalID = 0);

		// non-copyable
		GUI_ContainerImage(const GUI_ContainerImage&) = delete;
		GUI_ContainerImage& operator=(const GUI_ContainerImage&) = delete;

		// non-moveable
		GUI_ContainerImage(GUI_ContainerImage&&) = delete;
		GUI_ContainerImage& operator=(GUI_ContainerImage&&) = delete;

		void render();

	protected:
		Uint8 m_index;
};

class GUI_ContainerContainer : public GUI_Container
{
	public:
		GUI_ContainerContainer(iRect boxRect, Uint8 index, GUI_PanelWindow* parent, Uint32 internalID = 0) : m_index(index), GUI_Container(boxRect, parent, internalID) {}

		// non-copyable
		GUI_ContainerContainer(const GUI_ContainerContainer&) = delete;
		GUI_ContainerContainer& operator=(const GUI_ContainerContainer&) = delete;

		// non-moveable
		GUI_ContainerContainer(GUI_ContainerContainer&&) = delete;
		GUI_ContainerContainer& operator=(GUI_ContainerContainer&&) = delete;

		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseUp(Sint32 x, Sint32 y);

		void render();

	protected:
		Uint8 m_index;
		bool m_selected = false;
};

class GUI_ContainerItem : public GUI_Element
{
	public:
		GUI_ContainerItem(iRect boxRect, Uint8 cid, size_t index, Uint32 internalID = 0);

		// non-copyable
		GUI_ContainerItem(const GUI_ContainerItem&) = delete;
		GUI_ContainerItem& operator=(const GUI_ContainerItem&) = delete;

		// non-moveable
		GUI_ContainerItem(GUI_ContainerItem&&) = delete;
		GUI_ContainerItem& operator=(GUI_ContainerItem&&) = delete;

		void* onAction(Sint32 x, Sint32 y);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onRMouseDown(Sint32 x, Sint32 y);

		void render();

	protected:
		size_t m_index;
		Uint8 m_cid;
		bool m_selected = false;
};

#endif /* __FILE_GUI_CONTAINERWINDOW_h_ */
