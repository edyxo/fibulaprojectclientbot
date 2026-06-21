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

#include "GUI_Content.h"
#include "../engine.h"

extern Engine g_engine;
extern Uint32 g_frameTime;

GUI_Content::GUI_Content(iRect boxRect, Uint32 internalID)
{
	setRect(boxRect);
	m_internalID = internalID;
}

GUI_Content::~GUI_Content()
{
	clearChilds();
}

void GUI_Content::setRect(iRect& NewRect)
{
	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		GUI_Element* child = (*it);
		iRect childRect = child->getRect();
		iRect newChildRect;

		newChildRect.x1 = NewRect.x1 + (childRect.x1 - m_tRect.x1);
		newChildRect.y1 = NewRect.y1 + (childRect.y1 - m_tRect.y1);
		newChildRect.x2 = childRect.x2;
		newChildRect.y2 = childRect.y2;
		child->setRect(newChildRect);
	}
	m_tRect = NewRect;
}

void GUI_Content::setActiveElement(GUI_Element* actElement)
{
	if(m_actElement == actElement)
		return;

	if(m_actElement)
		m_actElement->deActivate();

	m_actElement = actElement;
	if(m_actElement)
        m_actElement->activate();
}

void GUI_Content::clearChilds()
{
	setActiveElement(NULL);
	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
		delete (*it);
	m_childs.clear();
}

void GUI_Content::addChild(GUI_Element* pChild)
{
	m_childs.push_back(pChild);

	iRect newRect;
	newRect.x1 = m_tRect.x1 + pChild->getRect().x1;
	newRect.y1 = m_tRect.y1 + pChild->getRect().y1;
	newRect.x2 = pChild->getRect().x2;
	newRect.y2 = pChild->getRect().y2;
	pChild->setRect(newRect);
}

void GUI_Content::removeChild(GUI_Element* pChild)
{
	if(pChild == m_actElement)
		setActiveElement(NULL);

	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		if((*it) == pChild)
		{
			m_childs.erase(it);
			break;
		}
	}
	delete pChild;
}

GUI_Element* GUI_Content::getChild(Uint32 internalID)
{
	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		if((*it)->getInternalID() == internalID)
			return (*it);
	}
	return NULL;
}

void* GUI_Content::onAction(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return NULL;

	for(std::vector<GUI_Element*>::reverse_iterator it = m_childs.rbegin(), end = m_childs.rend(); it != end; ++it)
	{
		if((*it)->isEventable() && (*it)->getRect().isPointInside(x, y))
			return (*it)->onAction(x, y);
	}
	return NULL;
}

void GUI_Content::onLMouseDown(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	for(std::vector<GUI_Element*>::reverse_iterator it = m_childs.rbegin(), end = m_childs.rend(); it != end; ++it)
	{
		if((*it)->isEventable() && (*it)->getRect().isPointInside(x, y))
		{
			setActiveElement((*it));
			(*it)->onLMouseDown(x, y);
			return;
		}
	}
}

void GUI_Content::onLMouseUp(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		if((*it)->isEventable())
			(*it)->onLMouseUp(x, y);
	}
}

void GUI_Content::onRMouseDown(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	for(std::vector<GUI_Element*>::reverse_iterator it = m_childs.rbegin(), end = m_childs.rend(); it != end; ++it)
	{
		if((*it)->isEventable() && (*it)->getRect().isPointInside(x, y))
		{
			setActiveElement((*it));
			(*it)->onRMouseDown(x, y);
			return;
		}
	}
}

void GUI_Content::onRMouseUp(Sint32 x, Sint32 y)
{
	if(!m_visible)
		return;

	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
	{
		if((*it)->isEventable())
			(*it)->onRMouseUp(x, y);
	}
}

void GUI_Content::onMouseMove(Sint32 x, Sint32 y, bool isInsideParent)
{
	if(!m_visible)
		return;

	if(isInsideParent)
		isInsideParent = isInsideRect(x, y);
	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
		(*it)->onMouseMove(x, y, isInsideParent);
}

void GUI_Content::onWheel(Sint32 x, Sint32 y, bool wheelUP)
{
	if(!m_visible)
		return;

	for(std::vector<GUI_Element*>::reverse_iterator it = m_childs.rbegin(), end = m_childs.rend(); it != end; ++it)
	{
		if((*it)->isEventable() && (*it)->getRect().isPointInside(x, y))
		{
			(*it)->onWheel(x, y, wheelUP);
			return;
		}
	}
}

GUI_Element* GUI_Content::getElementAt(Sint32 x, Sint32 y)
{
	if(!m_visible || !m_tRect.isPointInside(x, y))
		return NULL;

	for(std::vector<GUI_Element*>::reverse_iterator it = m_childs.rbegin(), end = m_childs.rend(); it != end; ++it)
	{
		if((*it)->getRect().isPointInside(x, y))
		{
			GUI_Element* element = (*it)->getElementAt(x, y);
			return (element ? element : (*it));
		}
	}
	return this;
}

void GUI_Content::render()
{
	if(!m_visible)
		return;

	for(std::vector<GUI_Element*>::iterator it = m_childs.begin(), end = m_childs.end(); it != end; ++it)
		(*it)->render();
}
