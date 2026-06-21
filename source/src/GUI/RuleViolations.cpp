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
#include "../game.h"

extern Engine g_engine;
extern Game g_game;

const char* rvs_get_reason(Uint8 id)
{
	if(g_clientVersion >= 726 && g_clientVersion <= 730)
	{
		switch(id)
		{
			case 0: return "1a) Insulting name";
			case 1: return "1b) Name containing parts of sentences";
			case 2: return "1b) Name with nonsensical letter combination";
			case 3: return "1b) Badly formatted name";
			case 4: return "1c) Name not describing person";
			case 5: return "1c) Name of celebrity";
			case 6: return "1c) Name referring to country";
			case 7: return "1c) Name to fake identity";
			case 8: return "1c) Name to fake official position";
			case 9: return "2a) Insulting statements";
			case 10: return "2b) Spamming";
			case 11: return "2c) Off-topic advertisment";
			case 12: return "2c) Real money advertisment";
			case 13: return "2d) Off-topic channel use";
			case 14: return "2e) Inciting rule violation";
			case 15: return "3a) Bug abuse";
			case 16: return "3b) Game weakness abuse";
			case 17: return "3c) Macro use";
			case 18: return "3d) Using modified client";
			case 19: return "3e) Hacking attempt";
			case 20: return "3f) Multi-clienting";
			case 21: return "3g) Account trading";
			case 22: return "3g) Account sharing";
			case 23: return "4a) Threatening gamemaster";
			case 24: return "4b) Pretending official position";
			case 25: return "4b) Pretending to have influence on staff";
			case 26: return "4c) False reports";
			case 27: return "Excessive unjustified player killing";
			case 28: return "Destructive behaviour";
			case 29: return "Invalid payment";
			default: return "Unknown reason";
		}
	}
	else if(g_clientVersion >= 820)
	{
		//post 8.20 reasons are compatible with post 8.50(3 last reasons are removed) so let's keep them together
		switch(id)
		{
			case 0: return "1a) Offensive Name";
			case 1: return "1b) Invalid Name Format";
			case 2: return "1c) Unsuitable Name";
			case 3: return "1d) Name Inciting Rule Violation";
			case 4: return "2a) Offensive Statement";
			case 5: return "2b) Spamming";
			case 6: return "2c) Illegal Advertising";
			case 7: return "2d) Off-Topic Public Statement";
			case 8: return "2e) Non-English Public Statement";
			case 9: return "2f) Inciting Rule Violation";
			case 10: return "3a) Bug Abuse";
			case 11: return "3b) Game Weakness Abuse";
			case 12: return "3c) Using Unofficial Software to Play";
			case 13: return "3d) Hacking";
			case 14: return "3e) Multi-Clienting";
			case 15: return "3f) Account Trading or Sharing";
			case 16: return "4a) Threatening Gamemaster";
			case 17: return "4b) Pretending to Have Influence on Rule Enforcement";
			case 18: return "4c) False Report to Gamemaster";
			case 19: return "Destructive Behaviour";
			case 20: return "Excessive Unjustified Player Killing";
			case 21: return "Invalid Payment";
			case 22: return "Spoiling Auction";
			default: return "Unknown reason";
		}
	}
	else
	{
		switch(id)
		{
			case 0: return "1a) Offensive name";
			case 1: return "1b) Name containing part of sentence";
			case 2: return "1b) Name with nonsensical letter combination";
			case 3: return "1b) Invalid name format";
			case 4: return "1c) Name not describing person";
			case 5: return "1c) Name of celebrity";
			case 6: return "1c) Name referring to country";
			case 7: return "1c) Name to fake player identity";
			case 8: return "1c) Name to fake official position";
			case 9: return "2a) Offensive statement";
			case 10: return "2b) Spamming";
			case 11: return "2c) Advertisement not related to game";
			case 12: return "2c) Real money advertisement";
			case 13: return "2d) Non-English public statement";
			case 14: return "2d) Off-topic public statement";
			case 15: return "2e) Inciting rule violation";
			case 16: return "3a) Bug abuse";
			case 17: return "3b) Game weakness abuse";
			case 18: return "3c) Macro use";
			case 19: return "3d) Using unofficial software to play";
			case 20: return "3e) Hacking";
			case 21: return "3f) Multi-clienting";
			case 22: return "3g) Account trading";
			case 23: return "3g) Account sharing";
			case 24: return "4a) Threatening gamemaster";
			case 25: return "4b) Pretending to have official position";
			case 26: return "4b) Pretending to have influence on gamemaster";
			case 27: return "4c) False report to gamemaster";
			case 28: return "Excessive unjustified player killing";
			case 29: return "Destructive behaviour";
			case 30: return "Spoiling auction";
			case 31: return "Invalid payment";
			default: return "Unknown reason";
		}
	}
}

const char* rvs_get_action(Uint8 id)
{
	if(g_clientVersion >= 772)
	{
		switch(id)
		{
			case 0: return "Notation";
			case 1: return "Name Report";
			case 2: return "Banishment";
			case 3: return "Name Report + Banishment";
			case 4: return "Banishment + Final Warning";
			case 5: return "Name Report + Banishment + Final Warning";
			case 6: return "Statement Report";
			case 7: return "IP Banishment";
			default: return "Unknown action";
		}
	}
	else
	{
		switch(id)
		{
			case 0: return "Notation";
			case 1: return "Namelock";
			case 2: return "AccountBan";
			case 3: return "Namelock/AccountBan";
			case 4: return "AccountBan + FinalWarning";
			case 5: return "Namelock/AccountBan + FinalWarning";
			case 6: return "IP Banishment";
			default: return "Unknown action";
		}
	}
}

void rvs_Events(Uint32, Sint32)
{
}

void UTIL_createRuleViolations()
{
}
