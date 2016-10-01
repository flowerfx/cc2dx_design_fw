///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKThread.h
///
/// Description :	RKThread Module
///----------------------------------------------------------------------
#ifndef RKTHREAD_H
#define RKTHREAD_H

///----------------------------------------------------------------------
///----------------------------------------------------------------------
#include "../Common/RKTypes.h"

namespace RKUtils
{
	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	struct RKThread;
	struct RKCriticalSection;
	struct RKMutex;
	struct RKSemaphore;
	struct RKThreadCondition;

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	typedef uint32(RKThreadCallback)(void* pThreadData);
	typedef void* (RKThreadCallbackVoid)(void* pThreadData);

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	enum RKThreadStackSize
	{
		RKThreadStackSize_16K = 0,
		RKThreadStackSize_32K,
		RKThreadStackSize_64K,
		RKThreadStackSize_128K,
		RKThreadStackSize_256K,
		RKThreadStackSize_512K,
		RKThreadStackSize_1024K,
		RKThreadStackSize_2048K,
	};

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	enum RKThreadPriority
	{
		RKThreadPriority_TimeCritical = 0,
		RKThreadPriority_Highest,
		RKThreadPriority_AboveNormal,
		RKThreadPriority_Normal,
		RKThreadPriority_BelowNormal,
		RKThreadPriority_Lowest,
		RKThreadPriority_Idle,
	};

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	void RKThread_InitModule();
	void RKThread_DeinitModule();

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	RKThread* RKThread_Create(const char* pThreadName, RKThreadCallback* pThreadAddress, void* pThreadParams, RKThreadPriority priority = RKThreadPriority_Normal, RKThreadStackSize stackSize = RKThreadStackSize_32K);
	void RKThread_Destroy(RKThread** ppThread);
	bool RKThread_Start(RKThread* pThread);
	void RKThread_Yield();
	void RKThread_Exit();
	void RKThread_Sleep(uint32 seconds, uint32 milliseconds);
	void RKThread_WaitForExit(RKThread* pThread);
	size_t RKThread_GetID();
	size_t RKThread_GetID(RKThread* pThread);
	bool RKThread_IsMainThread();

	///--------------------------------------------------------------------------------
	/// When creating a thread you should use these macros at the entry and exit points
	///--------------------------------------------------------------------------------
#define RKTHREAD_ENTER
#define RKTHREAD_EXIT RKThread_Exit();

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	RKCriticalSection* RKCriticalSection_Create(const char* pName);
	void RKCriticalSection_Destroy(RKCriticalSection** ppCriticalSection);
	void RKCriticalSection_Enter(RKCriticalSection* pCriticalSection);
	bool RKCriticalSection_TryEnter(RKCriticalSection* pCriticalSection);
	void RKCriticalSection_Leave(RKCriticalSection* pCriticalSection);

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	RKSemaphore* RKSemaphore_Create(const char* pName, int initialCount, int maxCount);
	void RKSemaphore_Destroy(RKSemaphore** ppSemaphore);
	void RKSemaphore_Signal(RKSemaphore* pSemaphore);
	bool RKSemaphore_Wait(RKSemaphore* pSemaphore, bool bBlockThread);

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	RKThreadCondition* RKThreadCondition_Create(const char* pName);
	void RKThreadCondition_Destroy(RKThreadCondition** ppThreadCondition);
	bool RKThreadCondition_Sleep(RKThreadCondition* pThreadCondition, RKCriticalSection* pCriticalSection, RKUtils::DWORD time_sleep = 500);
	bool RKThreadCondition_WakeOne(RKThreadCondition* pThreadCondition);
	bool RKThreadCondition_WakeAll(RKThreadCondition* pThreadCondition);
}
///----------------------------------------------------------------------
#endif // RKTHREAD_H
