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

#include "animator.h"

extern Uint32 g_frameTime;

Animator::Animator()
{
	m_startPhase = 0;
	m_animationPhases = 0;
	m_loopCount = 0;
	m_async = false;
}

void Animator::loadAnimator(Sint32 animationPhases, SDL_RWops* rwops)
{
	m_animationPhases = animationPhases;
	m_async = (SDL_ReadU8(rwops) == 0);
	m_loopCount = SDL_static_cast(Sint32, SDL_ReadLE32(rwops));
	m_startPhase = SDL_static_cast(Sint32, SDL_static_cast(Sint8, SDL_ReadU8(rwops)));
	for(Sint32 i = 0; i < m_animationPhases; ++i)
	{
		AnimationDurations data;
		data.m_min = SDL_static_cast(Sint32, SDL_ReadLE32(rwops));
		data.m_max = SDL_static_cast(Sint32, SDL_ReadLE32(rwops));
		m_phaseDurations.push_back(data);
    }
}

void Animator::setPhase(Animation& cAnim, Sint32 phase)
{
	if(cAnim.m_phase == phase)
		return;

	if(m_async)
	{
		if(phase == AnimationPhase_Async)
			cAnim.m_phase = 0;
		else if(phase == AnimationPhase_Random)
			cAnim.m_phase = UTIL_random(0, m_animationPhases - 1);
		else if(phase >= 0 && phase < m_animationPhases)
			cAnim.m_phase = phase;
		else
			cAnim.m_phase = getStartPhase();

		cAnim.m_isComplete = false;
		cAnim.m_lastPhaseTicks = g_frameTime;
		cAnim.m_currentDuration = getPhaseDuration(cAnim.m_phase);
		cAnim.m_currentLoop = 0;
	}
	else
		calculateSynchronous(cAnim);
}

Sint32 Animator::getPhase(Animation& cAnim, Uint32 movementSpeed)
{
	if(g_frameTime != cAnim.m_lastPhaseTicks && !cAnim.m_isComplete)
	{
		Sint32 elapsedTicks = (g_frameTime - cAnim.m_lastPhaseTicks);
		if(elapsedTicks >= cAnim.m_currentDuration)
		{
			Sint32 phase = 0;
			if(m_loopCount < 0)
				phase = getPingPongPhase(cAnim);
			else
				phase = getLoopPhase(cAnim);
			
			if(cAnim.m_phase != phase)
			{
				Sint32 duration = (movementSpeed != 0 ? getMovementDuration(movementSpeed) : (getPhaseDuration(phase) - (elapsedTicks - cAnim.m_currentDuration)));
				if(duration < 0 && !m_async)
					calculateSynchronous(cAnim);
				else
				{
					cAnim.m_phase = phase;
					cAnim.m_currentDuration = UTIL_max<Sint32>(0, duration);
				}
			}
			else
				cAnim.m_isComplete = true;
		}
		else
			cAnim.m_currentDuration -= elapsedTicks;

		cAnim.m_lastPhaseTicks = g_frameTime;
	}
	return cAnim.m_phase;
}

Sint32 Animator::getStartPhase()
{
	if(m_startPhase >= 0 && m_startPhase < m_animationPhases)
		return m_startPhase;
	return UTIL_random(0, m_animationPhases - 1);
}

void Animator::addPhaseDuration(Sint32 min, Sint32 max)
{
	AnimationDurations data;
	data.m_min = min;
	data.m_max = max;
	m_phaseDurations.push_back(data);
	++m_animationPhases;
}

void Animator::resetAnimation(Animation& cAnim, Sint32 phase)
{
	cAnim.m_isComplete = false;
	cAnim.m_currentDirection = AnimationDirection_Forward;
	cAnim.m_currentLoop = 0;
	cAnim.m_phase = 0;
	cAnim.m_lastPhaseTicks = g_frameTime;
	cAnim.m_currentDuration = 20;
	if(phase != AnimationPhase_Invalid)
		setPhase(cAnim, phase);
}

Sint32 Animator::getMovementDuration(Uint32 movementSpeed)
{
	Uint32 maximum_speed_delay = 100;
	Uint32 minimum_speed_delay = 550;
	Uint32 interval = UTIL_min<Uint32>(UTIL_max<Uint32>(maximum_speed_delay, movementSpeed), minimum_speed_delay);
	float delta_interval = SDL_static_cast(float, interval - maximum_speed_delay) / SDL_static_cast(float, minimum_speed_delay - maximum_speed_delay);
	float minimum_speed_duration = (720.f / m_animationPhases);
	float maximum_speed_duration = (280.f / m_animationPhases);
	return SDL_static_cast(Sint32, SDL_ceilf((minimum_speed_duration - maximum_speed_duration) * delta_interval + maximum_speed_duration));
}

Sint32 Animator::getPhaseDuration(Sint32 phase)
{
	AnimationDurations& data = m_phaseDurations[phase];
	if(data.m_min == data.m_max)
		return data.m_min;

	return UTIL_random(data.m_min, data.m_max);
}

Sint32 Animator::getPingPongPhase(Animation& cAnim)
{
	Sint32 count = (cAnim.m_currentDirection == AnimationDirection_Forward ? 1 : -1);
	Sint32 nextPhase = cAnim.m_phase + count;
	if(nextPhase < 0 || nextPhase >= m_animationPhases)
	{
		cAnim.m_currentDirection = (cAnim.m_currentDirection == AnimationDirection_Forward ? AnimationDirection_Backward : AnimationDirection_Forward);
		count *= -1;
	}
	return cAnim.m_phase + count;
}

Sint32 Animator::getLoopPhase(Animation& cAnim)
{
	Sint32 nextPhase = cAnim.m_phase + 1;
	if(nextPhase < m_animationPhases)
		return nextPhase;

	if(m_loopCount == 0)
		return 0;

	if(cAnim.m_currentLoop < (m_loopCount - 1))
	{
		++cAnim.m_currentLoop;
		return 0;
	}
	return cAnim.m_phase;
}

void Animator::calculateSynchronous(Animation& cAnim)
{
	Sint32 totalDuration = 0;
	for(Sint32 i = 0; i < m_animationPhases; ++i)
		totalDuration += getPhaseDuration(i);

	Sint32 elapsedTicks = (g_frameTime % totalDuration);
	Sint32 totalTime = 0;
	for(Sint32 i = 0; i < m_animationPhases; ++i)
	{
		Sint32 duration = getPhaseDuration(i);
		if(elapsedTicks >= totalTime && elapsedTicks < (totalTime + duration))
		{
			cAnim.m_phase = i;
			cAnim.m_currentDuration = duration - (elapsedTicks - totalTime);
			break;
		}
		totalTime += duration;
	}
	cAnim.m_lastPhaseTicks = g_frameTime;
}
