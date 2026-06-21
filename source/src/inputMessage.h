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

#ifndef __FILE_INPUT_MESSAGE_h_
#define __FILE_INPUT_MESSAGE_h_

#include "position.h"
#define INPUTMESSAGE_SIZE 65536

class Protocol;
class ProtocolLogin;
class ProtocolGame;
class Connection;
class InputMessage
{
	public:
		InputMessage() {reset();}

		// non-copyable
		InputMessage(const InputMessage&) = delete;
		InputMessage& operator=(const InputMessage&) = delete;

		// non-moveable
		InputMessage(InputMessage&&) = delete;
		InputMessage& operator=(InputMessage&&) = delete;

		void reset() {m_readPos = 0; m_messageSize = 0;}
		void setReadPos(Uint16 readPos) {m_readPos = readPos;}
		void setMessageSize(Uint16 size) {m_messageSize = size;}
		
		Uint8 getU8()
		{
			if(!canRead(1))
				return 0;

			return m_networkBuffer[m_readPos++];
		}

		Uint8 peekU8()
		{
			if(!canRead(1))
				return 0;

			return m_networkBuffer[m_readPos];
		}

		#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		Uint16 getU16()
		{
			if(!canRead(2))
				return 0;

			Uint16 v;
			memcpy(&v, m_networkBuffer + m_readPos, 2);
			m_readPos += 2;
			return v;
		}
		
		Uint32 getU32()
		{
			if(!canRead(4))
				return 0;

			Uint32 v;
			memcpy(&v, m_networkBuffer + m_readPos, 4);
			m_readPos += 4;
			return v;
		}
		
		Uint64 getU64()
		{
			if(!canRead(8))
				return 0;

			Uint64 v;
			memcpy(&v, m_networkBuffer + m_readPos, 8);
			m_readPos += 8;
			return v;
		}

		float getFloat()
		{
			if(!canRead(5))
				return 0.f;

			Uint8 precision = m_networkBuffer[m_readPos++];
			Sint32 v;
			memcpy(&v, m_networkBuffer + m_readPos, 4);
			m_readPos += 4;
			return ((v - SDL_MAX_SINT32) / SDL_powf(10.f, SDL_static_cast(float, precision)));
		}

		double getDouble()
		{
			if(!canRead(5))
				return 0.0;

			Uint8 precision = m_networkBuffer[m_readPos++];
			Sint32 v;
			memcpy(&v, m_networkBuffer + m_readPos, 4);
			m_readPos += 4;
			return ((v - SDL_MAX_SINT32) / SDL_pow(10.0, SDL_static_cast(double, precision)));
		}

		Position getPosition()
		{
			Position pos;
			if(!canRead(5))
				return pos;

			memcpy(&pos.x, m_networkBuffer + m_readPos, 2);
			m_readPos += 2;
			memcpy(&pos.y, m_networkBuffer + m_readPos, 2);
			m_readPos += 2;
			pos.z = m_networkBuffer[m_readPos++];
			return pos;
		}
		
		Uint16 peekU16()
		{
			if(!canRead(2))
				return 0;

			Uint16 v;
			memcpy(&v, m_networkBuffer + m_readPos, 2);
			return v;
		}
		
		Uint32 peekU32()
		{
			if(!canRead(4))
				return 0;

			Uint32 v;
			memcpy(&v, m_networkBuffer + m_readPos, 4);
			return v;
		}
		
		Uint64 peekU64()
		{
			if(!canRead(8))
				return 0;

			Uint64 v;
			memcpy(&v, m_networkBuffer + m_readPos, 8);
			return v;
		}
		#else
		Uint16 getU16()
		{
			if(!canRead(2))
				return 0;

			Uint16 v = SDL_static_cast(Uint16, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Uint16, m_networkBuffer[m_readPos + 1]) << 8);
			m_readPos += 2;
			return v;
		}

		Uint32 getU32()
		{
			if(!canRead(4))
				return 0;

			Uint32 v = SDL_static_cast(Uint32, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Uint32, m_networkBuffer[m_readPos + 1]) << 8);
			v |= (SDL_static_cast(Uint32, m_networkBuffer[m_readPos + 2]) << 16);
			v |= (SDL_static_cast(Uint32, m_networkBuffer[m_readPos + 3]) << 24);
			m_readPos += 4;
			return v;
		}

		Uint64 getU64()
		{
			if(!canRead(8))
				return 0;

			Uint64 v = SDL_static_cast(Uint64, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 1]) << 8);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 2]) << 16);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 3]) << 24);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 4]) << 32);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 5]) << 40);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 6]) << 48);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 7]) << 56);
			m_readPos += 8;
			return v;
		}

		float getFloat()
		{
			if(!canRead(5))
				return 0.f;

			Uint8 precision = m_networkBuffer[m_readPos++];
			Sint32 v = SDL_static_cast(Sint32, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Sint32, m_networkBuffer[m_readPos + 1]) << 8);
			v |= (SDL_static_cast(Sint32, m_networkBuffer[m_readPos + 2]) << 16);
			v |= (SDL_static_cast(Sint32, m_networkBuffer[m_readPos + 3]) << 24);
			m_readPos += 4;
			return ((v - SDL_MAX_SINT32) / SDL_powf(10.f, SDL_static_cast(float, precision)));
		}

		double getDouble()
		{
			if(!canRead(5))
				return 0.0;

			Uint8 precision = m_networkBuffer[m_readPos++];
			Sint32 v = SDL_static_cast(Sint32, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Sint32, m_networkBuffer[m_readPos + 1]) << 8);
			v |= (SDL_static_cast(Sint32, m_networkBuffer[m_readPos + 2]) << 16);
			v |= (SDL_static_cast(Sint32, m_networkBuffer[m_readPos + 3]) << 24);
			m_readPos += 4;
			return ((v - SDL_MAX_SINT32) / SDL_pow(10.0, SDL_static_cast(double, precision)));
		}

		Position getPosition()
		{
			Position pos;
			if(!canRead(5))
				return pos;

			pos.x = SDL_static_cast(Uint16, m_networkBuffer[m_readPos]);
			pos.x |= (SDL_static_cast(Uint16, m_networkBuffer[m_readPos + 1]) << 8);
			m_readPos += 2;
			pos.y = SDL_static_cast(Uint16, m_networkBuffer[m_readPos]);
			pos.y |= (SDL_static_cast(Uint16, m_networkBuffer[m_readPos + 1]) << 8);
			m_readPos += 2;
			pos.z = m_networkBuffer[m_readPos++];
			return pos;
		}

		Uint16 peekU16()
		{
			if(!canRead(2))
				return 0;
			
			Uint16 v = SDL_static_cast(Uint16, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Uint16, m_networkBuffer[m_readPos + 1]) << 8);
			return v;
		}

		Uint32 peekU32()
		{
			if(!canRead(4))
				return 0;

			Uint32 v = SDL_static_cast(Uint32, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Uint32, m_networkBuffer[m_readPos + 1]) << 8);
			v |= (SDL_static_cast(Uint32, m_networkBuffer[m_readPos + 2]) << 16);
			v |= (SDL_static_cast(Uint32, m_networkBuffer[m_readPos + 3]) << 24);
			return v;
		}

		Uint64 peekU64()
		{
			if(!canRead(8))
				return 0;
			
			Uint64 v = SDL_static_cast(Uint64, m_networkBuffer[m_readPos]);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 1]) << 8);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 2]) << 16);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 3]) << 24);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 4]) << 32);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 5]) << 40);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 6]) << 48);
			v |= (SDL_static_cast(Uint64, m_networkBuffer[m_readPos + 7]) << 56);
			return v;
		}
		#endif

		Sint8 getS8() {return static_cast<Sint8>(getU8());}
		Sint16 getS16() {return static_cast<Sint16>(getU16());}
		Sint32 getS32() {return static_cast<Sint32>(getU32());}
		Sint64 getS64() {return static_cast<Sint64>(getU64());}

		const char* getRawString()
		{
			static char EmptyString[] = "\0";
			Uint16 stringLength = getU16();
			if(!canRead(stringLength))
				return EmptyString;

			Uint16 readPos = m_readPos;
			m_readPos += stringLength;
			return SDL_reinterpret_cast(char*, m_networkBuffer + readPos);
		}

		std::string getString()
		{
			Uint16 stringLength = getU16();
			if(!canRead(stringLength))
				return std::string();

			Uint16 readPos = m_readPos;
			m_readPos += stringLength;
			return std::string(SDL_reinterpret_cast(char*, m_networkBuffer + readPos), stringLength);
		}

		bool getBool()
		{
			if(!canRead(1))
				return 0;

			return (m_networkBuffer[m_readPos++] != 0);
		}

		Sint8 peekS8() {return static_cast<Sint8>(peekU8());}
		Sint16 peekS16() {return static_cast<Sint16>(peekU16());}
		Sint32 peekS32() {return static_cast<Sint32>(peekU32());}
		Sint64 peekS64() {return static_cast<Sint64>(peekU64());}

		Uint16 getReadPos() {return m_readPos;}
		Uint16 getUnreadSize() {return m_messageSize - m_readPos;}
		Uint16 getMessageSize() {return m_messageSize;}
		bool eof() {return m_readPos >= m_messageSize;}

	protected:
		Uint8* getBuffer() {return m_networkBuffer;}
		Uint8* getReadBuffer() {return m_networkBuffer + m_readPos;}

		bool readAdler32();

		friend class Protocol;
		friend class ProtocolLogin;
		friend class ProtocolGame;
		friend class Connection;

	private:
		bool canRead(Uint32 bytes);

		Uint16 m_readPos;
		Uint16 m_messageSize;
		Uint8 m_networkBuffer[INPUTMESSAGE_SIZE];
};

#endif /* __FILE_INPUT_MESSAGE_h_ */
