///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKThread.cpp
///
/// Description :	RKThread Module
///----------------------------------------------------------------------

///----------------------------------------------------------------------
/// Includes
///----------------------------------------------------------------------
#include <process.h>

#include "RK.h"
#include "RKThread.h"
#include "RKMacros.h"
#include "RKHeap.h"
#include "RKString.h"
#include "RKApplication.h"
#include "../CasualCore/Platform/Hardware.h"

#if !defined(RK_USE_STD_THREAD)
// ugly official solution, well done Microsoft!
// see http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
// or  http://stackoverflow.com/questions/905876/how-to-set-name-to-a-win32-thread
const DWORD MS_VC_EXCEPTION=0x406D1388;

#if defined(OS_W8) || defined(OS_WP8)
#include "WinRTAPIsEmul.h"
#endif

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( DWORD dwThreadID, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR),       (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{}
}

static uint32 s_RKThread_MainThreadID = 0;

///----------------------------------------------------------------------
// A lookup array - MUST MATCH RKThread_Priority enum
///----------------------------------------------------------------------
static const int threadPriorityLookup[RKThreadPriority_Idle+1] = 
{
  THREAD_PRIORITY_TIME_CRITICAL,  // RKThreadPriority_TimeCritical
  THREAD_PRIORITY_HIGHEST,        // RKThreadPriority_Highest
  THREAD_PRIORITY_ABOVE_NORMAL,   // RKThreadPriority_AboveNormal
  THREAD_PRIORITY_NORMAL,         // RKThreadPriority_Normal
  THREAD_PRIORITY_BELOW_NORMAL,   // RKThreadPriority_BelowNormal
  THREAD_PRIORITY_LOWEST,         // RKThreadPriority_Lowest
  THREAD_PRIORITY_IDLE            // RKThreadPriority_Idle
};

///----------------------------------------------------------------------
/// RKThread
///----------------------------------------------------------------------
struct RKThreadInternal
{
  HANDLE handle;
  DWORD id;
  int priority;
//   RKString name;
};

///----------------------------------------------------------------------
/// RKCriticalSection
///----------------------------------------------------------------------
struct RKCriticalSectionInternal
{
  CRITICAL_SECTION  cs;
//   RKString name;
};


///----------------------------------------------------------------------
/// RKSemaphore
///----------------------------------------------------------------------
struct RKSemaphoreInternal
{
  RKSemaphoreInternal()
  {
    handle = 0;
  }

  HANDLE  handle;
//   RKString name;
};

///----------------------------------------------------------------------
///----------------------------------------------------------------------
struct RKThreadCondition
{
  CONDITION_VARIABLE cv;
};

///----------------------------------------------------------------------
/// RKThread_InitModule
///----------------------------------------------------------------------
void RKThread_InitModule()
{
  s_RKThread_MainThreadID = RKThread_GetID();
}

///----------------------------------------------------------------------
/// RKThread_DeinitModule
///----------------------------------------------------------------------
void RKThread_DeinitModule()
{
}

///----------------------------------------------------------------------
/// RKThread_Create
///----------------------------------------------------------------------
RKThread* RKThread_Create(const char* pThreadName, RKThreadCallback* pThreadAddress, void* pThreadParams, RKThreadPriority priority, RKThreadStackSize stackSize)
{
#if defined(OS_W8) || defined(OS_WP8)
  switch (CasualCore::Hardware::GetInstance()->GetCpuRating())
  {
	  case CasualCore::Hardware::CPU_LOW:
	  {
		priority == RKThreadPriority_BelowNormal;
		break;
	  }
	  case CasualCore::Hardware::CPU_MEDIUM:
	  {
		priority == RKThreadPriority_Normal;
		break;
	  }
  }
#endif
  RKThreadInternal* pThread = RKNEW(RKThreadInternal);
  if (pThread)
  {
//     pThread->name.Copy(pThreadName);
    pThread->priority = priority;

    unsigned long threadStackSize = 1024*64;
    switch (stackSize)
    {
    case RKThreadStackSize_16K: threadStackSize = 1024*16; break;
    case RKThreadStackSize_32K: threadStackSize = 1024*32; break;
    case RKThreadStackSize_64K: threadStackSize = 1024*64; break;
    case RKThreadStackSize_128K: threadStackSize = 1024*128; break;
    case RKThreadStackSize_256K: threadStackSize = 1024*256; break;
    case RKThreadStackSize_512K: threadStackSize = 1024*512; break;
    case RKThreadStackSize_1024K: threadStackSize = 1024*1024; break;
    case RKThreadStackSize_2048K: threadStackSize = 1024*2048; break;
    }

#if defined(OS_W8) || defined(OS_WP8)
	pThread->handle = CreateThreadEmul(NULL, threadStackSize, (LPTHREAD_START_ADDRRESS)pThreadAddress, pThreadParams, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION, &pThread->id);
	SetThreadPriorityEmul(pThread->handle, threadPriorityLookup[pThread->priority]);
#else
    pThread->handle = CreateThread(NULL, threadStackSize, (LPTHREAD_START_ROUTINE)pThreadAddress, pThreadParams, CREATE_SUSPENDED|STACK_SIZE_PARAM_IS_A_RESERVATION, &pThread->id);
		SetThreadName(pThread->id, pThreadName);

    SetThreadPriority(pThread->handle, threadPriorityLookup[pThread->priority]);
#endif
  }

  return (RKThread*)pThread;
}

///----------------------------------------------------------------------
/// RKThread_Start
///----------------------------------------------------------------------
bool RKThread_Start(RKThread* pThread)
{
  RKThreadInternal* pThreadInternal = (RKThreadInternal*)pThread;
#if defined(OS_W8) || defined(OS_WP8)
  unsigned long result = ResumeThreadEmul(pThreadInternal->handle);
#else
  unsigned long result = ResumeThread(pThreadInternal->handle);
#endif
  RKAssert(pThreadInternal->id!=0, "thread ID should have been set by CreateThread()!");  // pThreadInternal->id = GetCurrentThreadId();
  return result == -1 ? false : true;
}

///----------------------------------------------------------------------
/// RKThread_Destroy
///----------------------------------------------------------------------
void RKThread_Destroy(RKThread** ppThread)
{
  RKThreadInternal* pThreadInternal = (RKThreadInternal*)*ppThread;
  *ppThread = NULL;
  CloseHandle(pThreadInternal->handle);
  RKDELETE(pThreadInternal);
}

///----------------------------------------------------------------------
/// RKThread_Yield
///----------------------------------------------------------------------
void RKThread_Yield()
{
#if defined(OS_W8) || defined(OS_WP8)
  YieldEmul();
#else
  Sleep(0);
#endif
}

///----------------------------------------------------------------------
/// RKThread_Exit
///----------------------------------------------------------------------
void RKThread_Exit()
{
}

///----------------------------------------------------------------------
/// RKThread_Sleep
///----------------------------------------------------------------------
void RKThread_Sleep(uint32 seconds, uint32 milliseconds)
{
  RKASSERT(RKThread_IsMainThread() == false, "Err");

#if defined(OS_W8) || defined(OS_WP8)
  SleepEmul((seconds * 1000) + milliseconds);
#else
  Sleep((seconds * 1000) + milliseconds);
#endif
}

///----------------------------------------------------------------------
/// RKThread_WaitForExit
///----------------------------------------------------------------------
void RKThread_WaitForExit(RKThread* pThread)
{
	RKThreadInternal* pThreadInternal = (RKThreadInternal*)pThread;
#if defined(OS_W8) || defined(OS_WP8)
	::WaitForSingleObjectEx(pThreadInternal->handle, INFINITE, false);
#else
	::WaitForSingleObject(pThreadInternal->handle, INFINITE);
#endif
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
uint32 RKThread_GetID()
{
  DWORD currentThreadID = GetCurrentThreadId();
  return (uint32)currentThreadID;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
uint32 RKThread_GetID(RKThread* pThread)
{
  RKThreadInternal* pThreadInternal = (RKThreadInternal*)pThread;
  return (uint32)pThreadInternal->id;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
bool RKThread_IsMainThread()
{
  return s_RKThread_MainThreadID == RKThread_GetID();
}

///----------------------------------------------------------------------
/// RKSemaphore_Create
///----------------------------------------------------------------------
RKSemaphore* RKSemaphore_Create(const char* pSemaphoreName, int initialCount, int maxCount)
{
  RKSemaphoreInternal* pSemaphoreInternal = RKNEW(RKSemaphoreInternal);
//   pSemaphoreInternal->name.Copy(pSemaphoreName);

#if defined(OS_W8) || defined(OS_WP8)
  pSemaphoreInternal->handle = CreateSemaphoreEx(NULL, initialCount, maxCount, NULL, 0, SEMAPHORE_ALL_ACCESS);
#else
  pSemaphoreInternal->handle = CreateSemaphore(NULL, initialCount, maxCount, NULL);
#endif

  return (RKSemaphore*)pSemaphoreInternal;
}

///----------------------------------------------------------------------
/// RKSemaphore_Destroy
///----------------------------------------------------------------------
void RKSemaphore_Destroy(RKSemaphore** ppSemaphore)
{
  RKSemaphoreInternal* pSemaphoreInternal = (RKSemaphoreInternal*)*ppSemaphore;
  *ppSemaphore = NULL;

  CloseHandle(pSemaphoreInternal->handle);
  RKDELETE(pSemaphoreInternal);
}

///----------------------------------------------------------------------
/// RKSemaphore_Signal
///----------------------------------------------------------------------
void RKSemaphore_Signal(RKSemaphore* pSemaphore)
{
  RKASSERT(pSemaphore, "error invalid semaphore");
  RKSemaphoreInternal* pSemaphoreInternal = (RKSemaphoreInternal*)pSemaphore;
  ReleaseSemaphore(pSemaphoreInternal->handle, 1, NULL);
}

///----------------------------------------------------------------------
/// RKSemaphore_Wait
///----------------------------------------------------------------------
bool RKSemaphore_Wait(RKSemaphore* pSemaphore, bool bBlockThread)
{
  RKASSERT(pSemaphore, "error invalid semaphore");
  RKSemaphoreInternal* pSemaphoreInternal = (RKSemaphoreInternal*)pSemaphore;
#if defined(OS_W8) || defined(OS_WP8)
  return (WaitForSingleObjectEx(pSemaphoreInternal->handle, bBlockThread ? INFINITE : 0, false) == WAIT_OBJECT_0);
#else
  return (WaitForSingleObject(pSemaphoreInternal->handle, bBlockThread ? INFINITE : 0) == WAIT_OBJECT_0);
#endif
}

///----------------------------------------------------------------------
/// RKCriticalSection_Create
///----------------------------------------------------------------------
RKCriticalSection* RKCriticalSection_Create(const char* pName)
{
  RKCriticalSectionInternal* pCriticalSectionInternal = RKNEW(RKCriticalSectionInternal);
//   pCriticalSectionInternal->name.Copy(pName);
#if defined(OS_W8) || defined(OS_WP8)
  InitializeCriticalSectionEx(&pCriticalSectionInternal->cs, 0, 0);
#else
  InitializeCriticalSection(&pCriticalSectionInternal->cs);
#endif
  return (RKCriticalSection*)pCriticalSectionInternal;
}

///----------------------------------------------------------------------
/// RKCriticalSection_Destroy
///----------------------------------------------------------------------
void RKCriticalSection_Destroy(RKCriticalSection** ppCriticalSection)
{
  RKCriticalSectionInternal* pCriticalSectionInternal = (RKCriticalSectionInternal*)*ppCriticalSection;
  *ppCriticalSection = NULL;

  DeleteCriticalSection(&pCriticalSectionInternal->cs);
  RKDELETE(pCriticalSectionInternal);
}

///----------------------------------------------------------------------
/// RKCriticalSection_Enter
///----------------------------------------------------------------------
void RKCriticalSection_Enter(RKCriticalSection* pCriticalSection)
{
  EnterCriticalSection(&(((RKCriticalSectionInternal*)pCriticalSection)->cs));
}

///----------------------------------------------------------------------
/// RKCriticalSection_TryEnter
///----------------------------------------------------------------------
bool RKCriticalSection_TryEnter(RKCriticalSection* pCriticalSection)
{
  return TryEnterCriticalSection(&(((RKCriticalSectionInternal*)pCriticalSection)->cs))==TRUE;
}

///----------------------------------------------------------------------
/// RKCriticalSection_Leave
///----------------------------------------------------------------------
void RKCriticalSection_Leave(RKCriticalSection* pCriticalSection)
{
  LeaveCriticalSection(&(((RKCriticalSectionInternal*)pCriticalSection)->cs));
}


///----------------------------------------------------------------------
///----------------------------------------------------------------------
RKThreadCondition* RKThreadCondition_Create(const char* pName)
{
  // A condition variable cannot be moved or copied. The process must not modify the object, and must instead treat it as logically opaque. Only use the condition variable functions to manage condition variables.
  RKThreadCondition* pCV = RKNEW(RKThreadCondition);
  InitializeConditionVariable(&pCV->cv);

  return pCV;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKThreadCondition_Destroy(RKThreadCondition** ppThreadCondition)
{
  RKThreadCondition* pThreadCondition = *ppThreadCondition;
  *ppThreadCondition = 0;
  RKDELETE(pThreadCondition);
};

///----------------------------------------------------------------------
///----------------------------------------------------------------------
bool RKThreadCondition_Sleep(RKThreadCondition* pThreadCondition, RKCriticalSection* pCriticalSection)
{
  RKCriticalSectionInternal* pCriticalSectionInternal = (RKCriticalSectionInternal*)pCriticalSection;

  //return SleepConditionVariableCS(&pThreadCondition->cv, &pCriticalSectionInternal->cs, INFINITE)==TRUE;
  // bang.nguyenhuu: set time out to 500 instead of INFINITE, to prevent game freeze
  // !IMPORTANT: this fix can probably ruin the game. but at the moment, it's only way to fix freeze. revert this change if anything wrong.
  DWORD timeOut_ms = 500;// INFINITE;
  if (RKApplication_IsExiting())
    timeOut_ms = 10;  //1000 for 1 sec
  
  return SleepConditionVariableCS(&pThreadCondition->cv, &pCriticalSectionInternal->cs, timeOut_ms)==TRUE;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
bool RKThreadCondition_WakeOne(RKThreadCondition* pCondition)
{  
  WakeConditionVariable(&pCondition->cv);
  return true;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
bool RKThreadCondition_WakeAll(RKThreadCondition* pConditionVariable)
{
  WakeAllConditionVariable(&pConditionVariable->cv);
  return true;
}
#endif//!defined(RK_USE_STD_THREAD)