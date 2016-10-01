#ifndef RKTHREADPOOL_H
#define RKTHREADPOOL_H

#include "RKCallable.h"
#include "RKTypes.h"
#include "RKThread.h"
#include "RKList.h"
#include "RKQueue.h"
#include "RKString.h"
namespace RKUtils
{
	// all user functions of thread pool are thread-safe
	class RKThreadPool
	{
	public:
		// singleton-related
		static void Init(uint32 initialThreadsCount = 0,
			uint32 maxThreadsCount = 32,
			RKThreadPriority priority = RKThreadPriority_Normal,
			RKThreadStackSize stackSize = RKThreadStackSize_32K);
		static void Term();
		static RKThreadPool& GetInstance();

		// cheap-to-copy class containing id of the task, used to communicate with RKThreadPool
		class TaskHandle {
			friend class RKThreadPool;
		public:
			TaskHandle() : taskId(-1) {}

			bool operator==(const TaskHandle& other) const { return other.taskId == taskId; }
			uint32 Debug_GetID() const { return taskId; } //  for logging

		private:
			TaskHandle(uint32 id) : taskId(id) {}
			uint32 taskId;
		};

		// RunOnce/RunUntilFalse/RunWithOutput accept either of following argument sets:
		// 1. task name, function pointer and optionally some count of arguments
		// 2. task name, object instance pointer, object method pointer and optionally some count of arguments
		// 3. callable

		// RunOnce just runs function/method asynchronously once
		TaskHandle RunOnce(const char* taskName, void(*pFn)())                                     { _NewTask(taskName).simpleTaskCallable.Set(pFn);                  return _FinalizeNewTask(); }
		template <typename ARG1>
		TaskHandle RunOnce(const char* taskName, void(*pFn)(ARG1), ARG1 arg1)                      { _NewTask(taskName).simpleTaskCallable.Set(pFn, arg1);            return _FinalizeNewTask(); }
		template <typename OBJ>
		TaskHandle RunOnce(const char* taskName, OBJ* pInst, void (OBJ::*pMethod)())                { _NewTask(taskName).simpleTaskCallable.Set(pInst, pMethod);       return _FinalizeNewTask(); }
		template <typename OBJ, typename ARG1>
		TaskHandle RunOnce(const char* taskName, OBJ* pInst, void (OBJ::*pMethod)(ARG1), ARG1 arg1) { _NewTask(taskName).simpleTaskCallable.Set(pInst, pMethod, arg1); return _FinalizeNewTask(); }
		TaskHandle RunOnce(const char* taskName, const RKCallable<void>& callable)                  { _NewTask(taskName).simpleTaskCallable.AssignCopy(callable);      return _FinalizeNewTask(); }

		// RunUntilFalse accepts callable that returns boolean
		// it will continuously call this callable until
		// either it returns false, or StopRepeating is called
		TaskHandle RunUntilFalse(const char* taskName, bool(*pFn)())                                     { _NewTask(taskName).repeatingTaskCallable.Set(pFn);                  return _FinalizeNewTask(); }
		template <typename ARG1>
		TaskHandle RunUntilFalse(const char* taskName, bool(*pFn)(ARG1), ARG1 arg1)                      { _NewTask(taskName).repeatingTaskCallable.Set(pFn, arg1);            return _FinalizeNewTask(); }
		template <typename OBJ>
		TaskHandle RunUntilFalse(const char* taskName, OBJ* pInst, bool (OBJ::*pMethod)())                { _NewTask(taskName).repeatingTaskCallable.Set(pInst, pMethod);       return _FinalizeNewTask(); }
		template <typename OBJ, typename ARG1>
		TaskHandle RunUntilFalse(const char* taskName, OBJ* pInst, bool (OBJ::*pMethod)(ARG1), ARG1 arg1) { _NewTask(taskName).repeatingTaskCallable.Set(pInst, pMethod, arg1); return _FinalizeNewTask(); }
		TaskHandle RunUntilFalse(const char* taskName, const RKCallable<bool>& callable)                  { _NewTask(taskName).repeatingTaskCallable.AssignCopy(callable);      return _FinalizeNewTask(); }

		// RunWithOutput runs function/method and store it's result in location specified by last argument	
		/*
		template <typename RET>
		TaskHandle RunWithOutput(const char* taskName, RET (*pFn)(), RET* pOut)                                     { return _EnqueueSimpleTask(taskName, RKNEW(CallableOutputWrapper<RET>)(RKCallableFactory::CreateCallable(pFn), pOut)); }
		template <typename RET, typename ARG1>
		TaskHandle RunWithOutput(const char* taskName, RET (*pFn)(ARG1), ARG1 arg1, RET* pOut)                      { return _EnqueueSimpleTask(taskName, RKNEW(CallableOutputWrapper<RET>)(RKCallableFactory::CreateCallable(pFn, arg1), pOut)); }
		template <typename RET, typename OBJ>
		TaskHandle RunWithOutput(const char* taskName, OBJ* pInst, RET (OBJ::*pMethod)(), RET* pOut)                { return _EnqueueSimpleTask(taskName, RKNEW(CallableOutputWrapper<RET>)(RKCallableFactory::CreateCallable(pInst, pMethod), pOut)); }
		template <typename RET, typename OBJ, typename ARG1>
		TaskHandle RunWithOutput(const char* taskName, OBJ* pInst, RET (OBJ::*pMethod)(ARG1), ARG1 arg1, RET* pOut) { return _EnqueueSimpleTask(taskName, RKNEW(CallableOutputWrapper<RET>)(RKCallableFactory::CreateCallable(pInst, pMethod, arg1), pOut)); }
		template <typename RET>
		TaskHandle RunWithOutput(const char* taskName, RKCallableVirtual<RET>* pCallable, RET* pOut)                       { return _EnqueueSimpleTask(taskName, RKNEW(CallableOutputWrapper<RET>)(pCallable, pOut)); }
		*/

		// It's safe to call all these functions even if task is not processed anymore
		// or taskHandle is an invalid handle
		bool IsTaskProcessing(TaskHandle taskHandle) const;
		void Wait(TaskHandle taskHandle);
		void StopRepeating(TaskHandle taskHandle);
		void StopRepeatingAndWait(TaskHandle taskHandle) { StopRepeating(taskHandle); Wait(taskHandle); }

	private:
		RKThreadPool(uint32 initialThreadsCount, uint32 maxThreadsCount, RKThreadPriority priority, RKThreadStackSize stackSize);
		~RKThreadPool();

		// definition of task to run
		struct PendingTask
		{
			uint32           taskId;
			RKString         taskName;
			RKCallable<void> simpleTaskCallable;
			RKCallable<bool> repeatingTaskCallable;
			bool             dontRepeatTask; // true if repeating task was stopped before it was started, we still want to process it at least once
		};

		PendingTask&            _NewTask(const char* taskName);
		RKCallable<void>& _NewSimpleTask(const char* taskName);
		RKCallable<bool>& _NewRepeatingTask(const char* taskName);
		TaskHandle              _FinalizeNewTask(); // _FinalizeNewTask _MUST_ be called after every NewTask* call

		// aux functions, cs has to be already locked
		void   _SpawnNewThread();
		const char* _GetTaskNameAndIndices(uint32 taskId, int& threadIdx, int& queueIdx) const; // returns NULL if task is not found
		uint32 _GenerateNewTaskId();

		// main worker thread function
		static uint32 _ThreadFn(void* pVoidData); // data passed there is ThreadData struct

		// functions called from worker threads, from _ThreadFn
		bool     _ThreadAcquireNextTask(uint32 threadIdx, RKCallable<void>& simpleTaskCallable, RKCallable<bool>& repeatingTaskCallable, uint32& taskId);
		void     _ThreadNotifyTaskDone(uint32 threadIdx);
		bool     _ThreadNotifyRepeatingTaskIterationDone(uint32 threadIdx); // returns true if ThreadPool allows thread to continue

		// structure passed to worker thread
		struct ThreadData
		{
			RKThreadPool* pThreadPool;
			int threadIdx;
		};

		// information about running thread
		struct ThreadInfo
		{
			// constant thread data
			ThreadData         threadData;
			RKThread*          pThread;

			// mutable thread data
			uint32             taskId;
			RKString           taskName;
			bool               stoppingRepeatingTask;
		};

		const uint32            m_maxThreadCount;
		const RKThreadPriority  m_threadPriority;
		const RKThreadStackSize m_threadStackSize;

		RKCriticalSection*  m_pCriticalSection;
		RKList<ThreadInfo*> m_threadInfos;
		bool                m_terminating;
		uint32              m_waitingThreadsCount;
		uint32						  m_nextTaskId;  // don't use directly, use _GenerateNewTaskId() instead	

		RKThreadCondition*  m_pTaskAvailableCondition;
		RKList<PendingTask> m_pendingTasksQueue;

		RKThreadCondition*  m_pTaskDoneCondition;
		RKList<uint32>      m_awaitedTasks;

		static RKThreadPool* m_pInstance;
	};
}
#endif // RKTHREADPOOL_H