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

#ifndef __FILE_ANIMATOR_h_
#define __FILE_ANIMATOR_h_

#include "defines.h"

enum AnimationPhase : Sint32
{
	AnimationPhase_Automatic = -1,
	AnimationPhase_Random = -2,
	AnimationPhase_Async = -3,
	AnimationPhase_Invalid = -4
};

enum AnimationDirection : Uint8
{
	AnimationDirection_Forward = 0,
	AnimationDirection_Backward = 1
};

struct AnimationDurations
{
	Sint32 m_min;
	Sint32 m_max;
};

struct Animation
{
	Uint32 m_lastPhaseTicks;
	Sint32 m_currentLoop;
	Sint32 m_currentDuration;
	Sint32 m_phase;
	AnimationDirection m_currentDirection;
	bool m_isComplete;
};

class Animator
{
	public:
		Animator();

		void loadAnimator(Sint32 animationPhases, SDL_RWops* rwops);

		void setPhase(Animation& cAnim, Sint32 phase);
		Sint32 getPhase(Animation& cAnim, Uint32 movementSpeed = 0);

		void setStartPhase(Sint32 startPhase) {m_startPhase = startPhase;}
		Sint32 getStartPhase();

		Sint32 getAnimationPhases() {return m_animationPhases;}
		bool isComplete(Animation& cAnim) {return cAnim.m_isComplete;}
		
		void setLoopCount(Sint32 loopCount) {m_loopCount = loopCount;}
		void setAsync(bool async) {m_async = async;}
		bool isAsync() {return m_async;}

		void addPhaseDuration(Sint32 min, Sint32 max);
		void resetAnimation(Animation& cAnim, Sint32 phase = AnimationPhase_Automatic);

	protected:
		Sint32 getMovementDuration(Uint32 movementSpeed);
		Sint32 getPhaseDuration(Sint32 phase);
		Sint32 getPingPongPhase(Animation& cAnim);
		Sint32 getLoopPhase(Animation& cAnim);
		void calculateSynchronous(Animation& cAnim);

		std::vector<AnimationDurations> m_phaseDurations;

		Sint32 m_startPhase;
		Sint32 m_animationPhases;
		Sint32 m_loopCount;
		bool m_async;
};

#endif /* __FILE_ANIMATOR_h_ */
