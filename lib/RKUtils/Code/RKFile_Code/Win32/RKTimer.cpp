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
#include "rk.h"
#include "rktimer.h"

#include "RKHeap.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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

///----------------------------------------------------------------------
/// RKTimer_GetElapsedTime
///----------------------------------------------------------------------
float RKTimer_GetElapsedTime()
{
	return RKTimer_pClock->GetElapsedTime();
}

float RKTimer_GetBufferedElapsedTime()
{
  return RKTimer_pClock->GetBufferedElapsedTime();
}

float RKTimer_GetBufferedUpTime()
{
  return RKTimer_pClock->GetBufferedUpTime();
}

///----------------------------------------------------------------------
/// RKTimer:: Constructor
///----------------------------------------------------------------------
RKTimer::RKTimer() :
  m_FrameTime(0.f)
  ,m_UpTime(0.f)
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
	QueryPerformanceFrequency(&m_PerformanceFrequency);
  QueryPerformanceCounter(&m_InitTime);
  m_LastTime = m_InitTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetUpTime()
///----------------------------------------------------------------------
float RKTimer::GetUpTime()
{
  LARGE_INTEGER currentTime; QueryPerformanceCounter(&currentTime);
  m_UpTime = (float)(currentTime.QuadPart - m_InitTime.QuadPart) / m_PerformanceFrequency.QuadPart;
  return m_UpTime;
}

///----------------------------------------------------------------------
/// RKTimer::GetElapsedTime()
///----------------------------------------------------------------------
float RKTimer::GetElapsedTime()
{
  LARGE_INTEGER currentTime; QueryPerformanceCounter(&currentTime);
	m_FrameTime = (float)(currentTime.QuadPart - m_LastTime.QuadPart) / m_PerformanceFrequency.QuadPart;
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