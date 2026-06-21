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

#ifndef __FILE_OUTPUT_MESSAGE_h_
#define __FILE_OUTPUT_MESSAGE_h_

#include "position.h"
#define OUTPUTMESSAGE_SIZE 65536

class Protocol;
class ProtocolLogin;
class ProtocolGame;
class Connection;
class OutputMessage
{
	public:
		OutputMessage(Uint16 headerPos) {reset(headerPos);}

		// non-copyable
		OutputMessage(const OutputMessage&) = delete;
		OutputMessage& operator=(const OutputMessage&) = delete;

		// non-moveable
		OutputMessage(OutputMessage&&) = delete;
		OutputMessage& operator=(OutputMessage&&) = delete;

		void reset(Uint16 headerPos);

		void addU8(Uint8 v)
		{
			if(canWrite(1))
			{
				m_networkBuffer[m_writePos++] = v;
				++m_messageSize;
			}
		}

		#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		void addU16(Uint16 v)
		{
			if(canWrite(2))
			{
				memcpy(m_networkBuffer + m_writePos, &v, 2);
				m_writePos += 2;
				m_messageSize += 2;
			}
		}

		void addU32(Uint32 v)
		{
			if(canWrite(4))
			{
				memcpy(m_networkBuffer + m_writePos, &v, 4);
				m_writePos += 4;
				m_messageSize += 4;
			}
		}

		void addU64(Uint64 v)
		{
			if(canWrite(8))
			{
				memcpy(m_networkBuffer + m_writePos, &v, 8);
				m_writePos += 8;
				m_messageSize += 8;
			}
		}

		void addFloat(float value)
		{
			if(canWrite(5))
			{
				uint32_t floatValue = SDL_static_cast(uint32_t, value * 100.f) + SDL_MAX_SINT32;
				m_networkBuffer[m_writePos++] = 2;
				memcpy(m_networkBuffer + m_writePos, &floatValue, 4);
				m_writePos += 4;
				m_messageSize += 5;
			}
		}

		void addDouble(double value)
		{
			if(canWrite(5))
			{
				uint32_t doubleValue = SDL_static_cast(uint32_t, value * 1000.0) + SDL_MAX_SINT32;
				m_networkBuffer[m_writePos++] = 3;
				memcpy(m_networkBuffer + m_writePos, &doubleValue, 4);
				m_writePos += 4;
				m_messageSize += 5;
			}
		}

		void addPosition(const Position& pos)
		{
			if(canWrite(5))
			{
				memcpy(m_networkBuffer + m_writePos, &pos.x, 2);
				m_writePos += 2;
				memcpy(m_networkBuffer + m_writePos, &pos.y, 2);
				m_writePos += 2;
				m_networkBuffer[m_writePos++] = pos.z;
				m_messageSize += 5;
			}
		}
		#else
		void addU16(Uint16 v)
		{
			if(canWrite(2))
			{
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 8);
				m_messageSize += 2;
			}
		}

		void addU32(Uint32 v)
		{
			if(canWrite(4))
			{
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 8);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 16);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 24);
				m_messageSize += 4;
			}
		}

		void addU64(Uint64 v)
		{
			if(canWrite(8))
			{
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 8);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 16);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 24);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 32);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 40);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 48);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, v >> 56);
				m_messageSize += 8;
			}
		}

		void addFloat(float value)
		{
			if(canWrite(5))
			{
				uint32_t floatValue = (value * 100.0) + SDL_MAX_SINT32;
				m_networkBuffer[m_writePos++] = 2;
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, floatValue);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, floatValue >> 8);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, floatValue >> 16);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, floatValue >> 24);
				m_messageSize += 5;
			}
		}

		void addDouble(double value)
		{
			if(canWrite(5))
			{
				uint32_t doubleValue = (value * 1000.0) + SDL_MAX_SINT32;
				m_networkBuffer[m_writePos++] = 3;
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, doubleValue);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, doubleValue >> 8);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, doubleValue >> 16);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, doubleValue >> 24);
				m_messageSize += 5;
			}
		}

		void addPosition(const Position& pos)
		{
			if(canWrite(5))
			{
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, pos.x);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, pos.x >> 8);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, pos.y);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, pos.y >> 8);
				m_networkBuffer[m_writePos++] = pos.z;
				m_messageSize += 5;
			}
		}
		#endif

		void addS8(Sint8 v) {addU8(SDL_static_cast(Uint8, v));}
		void addS16(Sint16 v) {addU16(SDL_static_cast(Uint16, v));}
		void addS32(Sint32 v) {addU32(SDL_static_cast(Uint32, v));}
		void addS64(Sint64 v) {addU64(SDL_static_cast(Uint64, v));}

		void addRawString(const char* v)
		{
			Uint16 len = SDL_static_cast(Uint16, SDL_strlen(v));
			if(canWrite(len + 2))
			{
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, len);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, len >> 8);
				SDL_memcpy(m_networkBuffer + m_writePos, v, len);
				m_writePos += len;
				m_messageSize += len + 2;
			}
		}

		void addString(const std::string& v)
		{
			Uint16 len = SDL_static_cast(Uint16, v.length());
			if(canWrite(len + 2))
			{
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, len);
				m_networkBuffer[m_writePos++] = SDL_static_cast(Uint8, len >> 8);
				SDL_memcpy(m_networkBuffer + m_writePos, v.c_str(), len);
				m_writePos += len;
				m_messageSize += len + 2;
			}
		}

		void addPaddingBytes(Uint16 bytes, Uint8 byte = 0)
		{
			if(bytes > 0 && canWrite(bytes))
			{
				SDL_memset(m_networkBuffer + m_writePos, byte, bytes);
				m_writePos += bytes;
				m_messageSize += bytes;
			}
		}

		void addBool(bool value)
		{
			if(canWrite(1))
			{
				m_networkBuffer[m_writePos++] = (value ? 1 : 0);
				++m_messageSize;
			}
		}

		Uint16 getWritePos() {return m_writePos;}
		Uint16 getMessageSize() {return m_messageSize;}

		void setWritePos(Uint16 writePos) {m_writePos = writePos;}
		void setMessageSize(Uint16 messageSize) {m_messageSize = messageSize;}

	protected:
		Uint8* getBuffer() {return m_networkBuffer;}
		Uint8* getWriteBuffer() {return m_networkBuffer + m_writePos;}

		friend class Protocol;
		friend class ProtocolLogin;
		friend class ProtocolGame;
		friend class Connection;

	private:
		bool canWrite(Uint32 bytes);

		Uint16 m_writePos;
		Uint16 m_messageSize;
		Uint8 m_networkBuffer[OUTPUTMESSAGE_SIZE];
};

#endif /* __FILE_OUTPUT_MESSAGE_h_ */
