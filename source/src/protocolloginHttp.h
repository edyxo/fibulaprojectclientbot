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

#ifndef __FILE_PROTOCOL_LOGIN11_h_
#define __FILE_PROTOCOL_LOGIN11_h_

#include "protocol.h"

class ProtocolLoginHttp
{
	public:
		ProtocolLoginHttp() = default;

		// non-copyable
		ProtocolLoginHttp(const ProtocolLoginHttp&) = delete;
		ProtocolLoginHttp& operator=(const ProtocolLoginHttp&) = delete;

		// non-moveable
		ProtocolLoginHttp(ProtocolLoginHttp&&) = delete;
		ProtocolLoginHttp& operator=(ProtocolLoginHttp&&) = delete;

		Uint32 getRequestId() {return m_requestId;}

		void parseMessage();
		void initializeConnection();
		void closeConnection();
		
	private:
		Uint32 m_requestId = 0;
};

#endif /* __FILE_PROTOCOL_LOGIN11_h_ */
