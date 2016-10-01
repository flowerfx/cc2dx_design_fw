///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKThread.h
///
/// Description :	RKThread Module
///----------------------------------------------------------------------
#ifndef RKTHREAD_WRAPPER_H
#define RKTHREAD_WRAPPER_H

///----------------------------------------------------------------------
///----------------------------------------------------------------------
#include "RKThread.h"
#include "RKInterlock.h"
#include "../RKString_Code/RKString.h"
namespace RKUtils
{
	class ThreadWrapper
	{
	private:
		RKCriticalSection*   m_criticalSection;
		RKThread *			 m_Thread;
		RKInterlock*         m_exit;
		RKThreadCondition*   m_condition;

		volatile bool		 m_IsThreadOnRunning;

		RKUtils::DWORD				m_time_sleep;
	public:
		ThreadWrapper();

		ThreadWrapper(RKUtils::DWORD time_sleep);

		~ThreadWrapper();

		void CreateThreadWrapper(RKString name_thread, RKThreadCallback* pThreadAddress, void* pThreadParams, RKThreadPriority priority = RKThreadPriority_Normal, RKThreadStackSize stackSize = RKThreadStackSize_32K);
		void ReleaseThreadWrapper();

		void OnCheckUpdateThreadWrapper(const std::function<void()>& func);

		void OnWakeUpAndRunThread();

		bool IsThreadRunning() { return m_IsThreadOnRunning; }
	};
}
///----------------------------------------------------------------------
#endif // RK_THREAD_WRAPPER_H
