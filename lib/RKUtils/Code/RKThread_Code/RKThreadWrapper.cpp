
///----------------------------------------------------------------------
///----------------------------------------------------------------------
#include "RKThreadWrapper.h"

namespace RKUtils
{
	ThreadWrapper::ThreadWrapper()
		: m_criticalSection(0)
		, m_Thread(0)
		, m_exit(0)
		, m_condition(0)
	{
		m_IsThreadOnRunning = false;
		m_time_sleep = 0xFFFFFFF;//sleep infinity
	}

	ThreadWrapper::ThreadWrapper(RKUtils::DWORD time_sleep)
		: m_criticalSection(0)
		, m_Thread(0)
		, m_exit(0)
		, m_condition(0)
	{
		m_IsThreadOnRunning = false;
		m_time_sleep = time_sleep;
	}

	ThreadWrapper::~ThreadWrapper()
	{
		bool bExit = false;
		if (m_exit)
		{
			bExit = RKInterlock_Query(m_exit) != 0;
		}

		if (!bExit)
		{
			// tell the work thread to exit
			RKInterlock_Lock(m_exit);

			// wake work thread
			if (m_condition)
			{
				RKThreadCondition_WakeAll(m_condition);
			}

			if (m_Thread)
			{
				RKUtils::RKThread_WaitForExit(m_Thread);
				RKUtils::RKThread_Destroy(&m_Thread);
			}

			if (m_condition)
			{
				RKThreadCondition_Destroy(&m_condition);
			}

			if (!m_exit)
			{
				RKInterlock_Destroy(&m_exit);
			}
		}

		m_IsThreadOnRunning = false;
		m_time_sleep = 0xFFFFFFF;//sleep infinity

	}

	void ThreadWrapper::CreateThreadWrapper(RKString name_thread, RKThreadCallback* pThreadAddress, void* pThreadParams, RKThreadPriority priority, RKThreadStackSize stackSize)
	{
        m_IsThreadOnRunning = true;
        
		m_criticalSection = RKCriticalSection_Create((name_thread + "_mutex").GetString());

		m_condition = RKThreadCondition_Create((name_thread + "_condition").GetString());

		m_exit = RKInterlock_Create((name_thread + "_exit").GetString());

		m_Thread = RKThread_Create(name_thread.GetString(), pThreadAddress, pThreadParams, priority, stackSize);

		RKThread_Start(m_Thread);

	}

	void ThreadWrapper::ReleaseThreadWrapper()
	{
		if (this)
			delete this;
	}

	void ThreadWrapper::OnCheckUpdateThreadWrapper(const std::function<void()>& func)
	{
		// keep the thread running until we want to stop it
		bool bExit = RKInterlock_Query(m_exit) != 0;
		while (!bExit)
		{
			// wait for work to be added
			RKCriticalSection_Enter(m_criticalSection);
			bExit = RKInterlock_Query(m_exit) != 0;

			while (!m_IsThreadOnRunning && !bExit)
			{
				RKThreadCondition_Sleep(m_condition, m_criticalSection, m_time_sleep); //sleep infinity and wait wake up
				bExit = RKInterlock_Query(m_exit) != 0;
			}

			if (bExit)
			{
				RKCriticalSection_Leave(m_criticalSection);
				break;
			}

			func();
			//run the function here !

			m_IsThreadOnRunning = false;

			RKCriticalSection_Leave(m_criticalSection);
		}
		RKInterlock_Reset(m_exit);
	}

	void ThreadWrapper::OnWakeUpAndRunThread()
	{
		m_IsThreadOnRunning = true;
		RKThreadCondition_WakeAll(m_condition);
	}
}
