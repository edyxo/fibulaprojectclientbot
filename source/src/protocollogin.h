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

#ifndef __FILE_PROTOCOL_LOGIN_h_
#define __FILE_PROTOCOL_LOGIN_h_

#include "protocol.h"

enum LoginOpcodes : Uint8
{
	//Send Opcodes
	SendLoginOpcode = 0x01,

	//Recv Opcodes
	RecvErrorOpcode = 0x0A,
	RecvErrorNewOpcode = 0x0B,
	RecvTokenSuccessOpcode = 0x0C,
	RecvTokenFailedOpcode = 0x0D,
	RecvMotdOpcode = 0x14,
	RecvSurveyInvitationOpcode = 0x15,
	RecvUpdate1Opcode = 0x1E,
	RecvUpdate2Opcode = 0x1F,
	RecvUpdateBlockOpcode = 0x20,
	RecvSessionKeyOpcode = 0x28,
	RecvCharacterListOpcode = 0x64,
	RecvCharacterListNewOpcode = 0x65
};

class ProtocolLogin : public Protocol
{
	public:
		ProtocolLogin() = default;

		// non-copyable
		ProtocolLogin(const ProtocolLogin&) = delete;
		ProtocolLogin& operator=(const ProtocolLogin&) = delete;

		// non-moveable
		ProtocolLogin(ProtocolLogin&&) = delete;
		ProtocolLogin& operator=(ProtocolLogin&&) = delete;

		virtual void parseMessage(InputMessage& msg);
		virtual void onConnect();
		virtual void onConnectionError(ConnectionError error);
		virtual void onDisconnect();

	private:
		void parseError(InputMessage& msg);
		void parseTokenSuccess(InputMessage& msg);
		void parseTokenFailed(InputMessage& msg);
		void parseMotd(InputMessage& msg);
		void parseSurveyInvitation(InputMessage& msg);
		void parseUpdate(InputMessage& msg);
		void parseUpdateBlock(InputMessage& msg);
		void parseSessionKey(InputMessage& msg);
		void parseCharacterList(InputMessage& msg);
		void parseCharacterListNew(InputMessage& msg);

		ConnectionError m_lastError = CONNECTION_ERROR_INVALID;
		bool m_skipErrors = false;
};

#endif /* __FILE_PROTOCOL_LOGIN_h_ */
