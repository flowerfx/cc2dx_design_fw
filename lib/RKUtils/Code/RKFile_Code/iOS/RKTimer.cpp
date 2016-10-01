#ifdef OS_IOS
///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKTimer.cpp
///
/// Description :	X86 High resolution timer class
///----------------------------------------------------------------------

///----------------------------------------------------------------------
/// Includes
///----------------------------------------------------------------------
#include "RK.h"
#include "RKTimer.h"
#include "RKMacros.h"
#include "CoreFoundation/CoreFoundation.h"
#include "OnlineService.h"
namespace RKUtils
{
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

float RKTimer_GetBufferedUpTime()
{
  return RKTimer_pClock->GetBufferedUpTime();
}

float RKTimer_GetBufferedElapsedTime()
{
	return RKTimer_pClock->GetBufferedElapsedTime();
}

///----------------------------------------------------------------------
/// RKTimer:: Constructor
///----------------------------------------------------------------------
RKTimer::RKTimer() :
  m_FrameTime(0.f),
  m_UpTime(0.f)
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
	m_InitTime = CasualCoreOnline::CCOnlineService::GetMonotonicTime();
	m_LastTime = m_InitTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetUpTime()
///----------------------------------------------------------------------
float RKTimer::GetUpTime()
{
	double currentTime = CasualCoreOnline::CCOnlineService::GetMonotonicTime();
	m_UpTime = RKMAX( currentTime - m_InitTime, 0.0 );
	
	return m_UpTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetElapsedTime()
///----------------------------------------------------------------------
float RKTimer::GetElapsedTime()
{
	double currentTime = CasualCoreOnline::CCOnlineService::GetMonotonicTime();
	m_FrameTime = RKMAX( currentTime - m_LastTime, 0.0 );
	m_LastTime = currentTime;
	
	return m_FrameTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetBufferedElapsedTime()
///----------------------------------------------------------------------
float RKTimer::GetBufferedElapsedTime()
{
    return m_FrameTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetBufferedUpTime()
///----------------------------------------------------------------------
float RKTimer::GetBufferedUpTime()
{
    return m_UpTime;
}
}
#endif