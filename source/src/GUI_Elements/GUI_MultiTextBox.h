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

#ifndef __FILE_GUI_MULTITEXTBOX_h_
#define __FILE_GUI_MULTITEXTBOX_h_

#include "GUI_ScrollBar.h"

struct MultiLine
{
	size_t lineStart;
	size_t lineLength;
	Sint32 selectionStart;
	Sint32 selectionWidth;
};

class GUI_MultiTextBox : public GUI_Element
{
	public:
		GUI_MultiTextBox(iRect boxRect, bool allowEdit, const std::string text, Uint32 internalID = 0, Uint8 red = 223, Uint8 green = 223, Uint8 blue = 223);

		// non-copyable
		GUI_MultiTextBox(const GUI_MultiTextBox&) = delete;
		GUI_MultiTextBox& operator=(const GUI_MultiTextBox&) = delete;

		// non-moveable
		GUI_MultiTextBox(GUI_MultiTextBox&&) = delete;
		GUI_MultiTextBox& operator=(GUI_MultiTextBox&&) = delete;

		void setRect(iRect& NewRect);
		void setTextEventCallback(void (*eventHandlerFunction)(Uint32,Sint32), Uint32 mEvent);
		void setColor(Uint8 red, Uint8 green, Uint8 blue) {m_red = red; m_green = green; m_blue = blue;}
		void setFont(Uint8 font) {m_font = font;}
		
		Sint32 addTextToTextBox(std::string text, Uint32 position);
		void removeTextToTextBox(Sint32 nbr, Uint32 position);
		void setText(const std::string text);
		void setCursor(Uint32 position);
		void moveCursor(Sint32 n);
		void moveCursorVertically(Sint32 n);
		Uint32 getCursorPosition(Sint32 x, Sint32 y);

		void setMaxLength(Uint32 length) {m_maxLength = length;}
		std::string& getText() {return m_sText;}

		void selectAll();
		void setSelection(Uint32 start, Uint32 end);
		void clearSelection();
		void deleteSelection();
		bool hasSelection() {return m_selectionEnd != m_selectionStart;}

		void onTextInput(const char* textInput);
		void onKeyDown(SDL_Event& event);
		void onLMouseDown(Sint32 x, Sint32 y);
		void onLMouseUp(Sint32 x, Sint32 y);
		void onMouseMove(Sint32 x, Sint32 y, bool isInsideParent);
		void onWheel(Sint32 x, Sint32 y, bool wheelUP);
		void activate();
		void deActivate();
		void update();
		void updateSelection();
		void render();

	protected:
		std::vector<MultiLine> m_lines;
		void (*m_eventHandlerFunction)(Uint32,Sint32) = NULL;
		std::string m_sText;
		GUI_VScrollBar m_scrollBar;
		Uint32 m_evtParam = 0;
		Uint32 m_maxLength = SDL_MAX_UINT32;
		Uint32 m_cursorTimer;
		Uint32 m_cursorPosition = 0;
		Uint32 m_textStartPosition = 4;
		Uint32 m_selectionReference = 0;
		Uint32 m_selectionStart = 0;
		Uint32 m_selectionEnd = 0;
		Sint32 m_maxDisplay = 0;
		Uint8 m_red;
		Uint8 m_green;
		Uint8 m_blue;
		Uint8 m_font = CLIENT_FONT_NONOUTLINED;
		bool m_allowEdit;
		bool m_bShowCursor = false;
		bool m_selecting = false;
		bool m_needUpdate = true;
		bool m_needUpdateSelection = false;
};

#endif /* __FILE_GUI_MULTITEXTBOX_h_ */
