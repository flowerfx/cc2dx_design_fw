///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKTimer.cpp
///
/// Description :	General timer class. Not as high resolution as Win32/OSX
///----------------------------------------------------------------------

///----------------------------------------------------------------------
/// Includes
///----------------------------------------------------------------------
#if defined(ANDROID_OS)
#include "rk.h"
#include "rktimer.h"
#include "RKHeap.h"
#include <ctime>
#include <time.h>


static RKTimer* RKTimer_pClock = 0;

///----------------------------------------------------------------------
/// RKTimer_InitModule
///----------------------------------------------------------------------
void RKTimer_InitModule()
{
  RKTimer_pClock = RKNEW(RKTimer);
  RKTimer_pClock->Init();
}

///----------------------------------------------------------------------
/// RKTimer_DeinitModule
///----------------------------------------------------------------------
void RKTimer_DeinitModule()
{
  if (RKTimer_pClock)
  {
    RKDELETE(RKTimer_pClock);
    RKTimer_pClock = 0;
  }
}

///----------------------------------------------------------------------
/// RKTimer_GetUpTime
///----------------------------------------------------------------------
float RKTimer_GetUpTime()
{
  return RKTimer_pClock->GetUpTime();
}

float RKTimer_GetElapsedTime()
{
	return RKTimer_pClock->GetElapsedTime();
}

///----------------------------------------------------------------------
/// RKTimer:: Constructor
///----------------------------------------------------------------------
RKTimer::RKTimer()
{
}

///----------------------------------------------------------------------
/// RKTimer:: Destructor
///----------------------------------------------------------------------
RKTimer::~RKTimer()
{
}

///----------------------------------------------------------------------
/// RKTimer::Init()
///----------------------------------------------------------------------
void RKTimer::Init()
{
	m_InitTime =  ((double) clock() / CLOCKS_PER_SEC);
	m_LastTime = m_InitTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetUpTime()
///----------------------------------------------------------------------
float RKTimer::GetUpTime()
{
	double currentTime =  ((double) clock() / CLOCKS_PER_SEC);
	double elapsedTime = currentTime - m_InitTime;
	if (elapsedTime < 0) //funny testers changing system time backwards ...! Christophe, Zoo bug 5086197
		elapsedTime = 0;
	
	return elapsedTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetElapsedTime()
///----------------------------------------------------------------------
float RKTimer::GetElapsedTime()
{
	double currentTime =  ((double) clock() / CLOCKS_PER_SEC);
	double elapsedTime = currentTime - m_LastTime;
	if (elapsedTime < 0) //funny testers changing system time backwards ...! Christophe, Zoo bug 5086197
		elapsedTime = 0;
	m_LastTime = currentTime;
	
	return elapsedTime;
}
#endif