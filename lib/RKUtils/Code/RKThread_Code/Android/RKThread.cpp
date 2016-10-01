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
#include "RK.h"
#include "RKThread.h"
#include "RKMacros.h"
#include "RKHeap.h"
#include "RKString.h"

#include <pthread.h>
#include <time.h>
#include <stdio.h>
namespace RKUtils {
static uint32 s_RKThread_MainThreadID = 0;

// apparently unnamed semaphores dont work on iOS, so our semaphore implementation is done manuall using two mutexes
//#include <semaphore.h>

///----------------------------------------------------------------------
/// RKThread
///----------------------------------------------------------------------
struct RKThreadInternal
{
  pthread_t handle; // Is the thread id under posix threads

  int priority;
  unsigned long threadStackSize;
  RKThreadCallbackVoid* pThreadAddress;
  void* pThreadParams;
//   RKString name;
};

///----------------------------------------------------------------------
/// RKCriticalSection
///----------------------------------------------------------------------
struct RKCriticalSectionInternal
{
  pthread_mutex_t mutex;
//   RKString name;
};



///----------------------------------------------------------------------
/// RKThreadCondition
///----------------------------------------------------------------------
struct RKThreadCondition
{
  pthread_cond_t condition;  
};

///----------------------------------------------------------------------
/// RKSemaphore
///----------------------------------------------------------------------
struct RKSemaphoreInternal
{
    RKCriticalSection* mutex;
    RKCriticalSection* internalMutex;
    int maxValue;
    int currentValue;
//     RKString name;
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
  RKThreadInternal* pThread = (RKThreadInternal*)RKHeap_AllocAndZero(sizeof(RKThreadInternal));
  if (pThread)
  {
//     pThread->name.Copy(pThreadName);
    pThread->priority = priority;
      pThread->pThreadAddress = (RKThreadCallbackVoid*)pThreadAddress;
      pThread->pThreadParams = pThreadParams;

    unsigned long threadStackSize = 1024*64;
    switch (stackSize)
    {
    case RKThreadStackSize_16K: pThread->threadStackSize = 1024*16; break;
    case RKThreadStackSize_32K: pThread->threadStackSize = 1024*32; break;
    case RKThreadStackSize_64K: pThread->threadStackSize = 1024*64; break;
    case RKThreadStackSize_128K: pThread->threadStackSize = 1024*128; break;
    case RKThreadStackSize_256K: pThread->threadStackSize = 1024*256; break;
    case RKThreadStackSize_512K: pThread->threadStackSize = 1024*512; break;
    case RKThreadStackSize_1024K: pThread->threadStackSize = 1024*1024; break;
    case RKThreadStackSize_2048K: pThread->threadStackSize = 1024*2048; break;
    }

    // Set thread attributes

    // Create the thread (suspended)
    /*int threadStatus = pthread_create(&pThread->handle, NULL, pThreadAddress, pThreadParams);
    if (threadStatus != 0)
    {
      // Error
      RKHeap_Free(pThread);
      pThread = 0;
    }
    else
    {
      // Set thread priority
      // Set thread id
    }*/
  }

  return (RKThread*)pThread;
}

///----------------------------------------------------------------------
/// RKThread_Start
///----------------------------------------------------------------------
bool RKThread_Start(RKThread* pThread)
{
   RKThreadInternal* pThreadInternal = (RKThreadInternal*)pThread;
   /*unsigned long result = ResumeThread(pThreadInternal->handle);
   return result == -1 ? false : true;*/
    
    // set thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&attr, pThreadInternal->threadStackSize);
    
    

    int iResult = pthread_create(&pThreadInternal->handle,
                              &attr,
                              pThreadInternal->pThreadAddress,
                              pThreadInternal->pThreadParams);
    
    pthread_attr_destroy(&attr);
    
    if (iResult != 0)
    {
        return false;    
    }
    else
    {
        // set thread priority
        sched_param param;
        int schedMode;
        pthread_getschedparam(pThreadInternal->handle, &schedMode,&param);
        
        // get priority bounds
        int minprio = sched_get_priority_min(schedMode);
        int maxprio = sched_get_priority_max(schedMode);
        
        // clamp
        int priority = pThreadInternal->priority;
        if (priority < minprio) priority = minprio;
        if (priority > maxprio) priority = maxprio;
        
        // set thread priority
        pthread_setschedparam(pThreadInternal->handle,priority, &param);
        return true;
    }
}

///----------------------------------------------------------------------
/// RKThread_Destroy
///----------------------------------------------------------------------
void RKThread_Destroy(RKThread** ppThread)
{
   RKThreadInternal* pThreadInternal = (RKThreadInternal*)*ppThread;
   *ppThread = NULL;
   /*if (pThreadInternal->handle)
       pthread_cancel(pThreadInternal->handle);*/
   RKHeap_Free(pThreadInternal);
}

///----------------------------------------------------------------------
/// RKThread_WaitForExit
///----------------------------------------------------------------------
void RKThread_WaitForExit(RKThread* pThread)
{
    // <TODO> implement on iOS - Owen
    RKThreadInternal* pThreadInternal = (RKThreadInternal*)pThread;
    pthread_join(pThreadInternal->handle, 0);
    pThreadInternal->handle = 0;
}
///----------------------------------------------------------------------
/// RKThread_Yield
///----------------------------------------------------------------------
void RKThread_Yield()
{
 	//pthread_yield_np();
}

///----------------------------------------------------------------------
/// RKThread_Exit
///----------------------------------------------------------------------
void RKThread_Exit()
{
  pthread_exit(0);
}

///----------------------------------------------------------------------
/// RKThread_Sleep
///----------------------------------------------------------------------
void RKThread_Sleep(uint32 seconds, uint32 milliseconds)
{
  RKASSERT(milliseconds < 1000, "Error milliseconds component must be < 1000");
  timespec sleepTime;
  sleepTime.tv_sec = seconds;
  sleepTime.tv_nsec = milliseconds * 1000000;

  nanosleep(&sleepTime, NULL);
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
uint32 RKThread_GetID()
{
  return (uint32)pthread_self();
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
uint32 RKThread_GetID(RKThread* pThread)
{
  RKThreadInternal* pThreadInternal = (RKThreadInternal*)pThread;
  return (uint32)pThreadInternal->handle;
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
  RKSemaphoreInternal* pSemaphoreInternal = (RKSemaphoreInternal*)RKHeap_AllocAndZero(sizeof(RKSemaphoreInternal));
//   pSemaphoreInternal->name.Copy(pSemaphoreName);
/*
    pSemaphoreInternal->maxValue = maxCount;
    int iResult = sem_init(&pSemaphoreInternal->semaphore, initialCount, maxCount);
*/
    
    // we are manually implementing a semaphore with two mutexes
    pSemaphoreInternal->mutex = RKCriticalSection_Create("semaphoreMutex");
    pSemaphoreInternal->internalMutex = RKCriticalSection_Create("semaphoreInternalMutex");
    pSemaphoreInternal->currentValue = initialCount;
    pSemaphoreInternal->maxValue = maxCount;
    
    if (initialCount == 0)
        RKCriticalSection_Enter(pSemaphoreInternal->mutex);
    
  return (RKSemaphore*)pSemaphoreInternal;
}

///----------------------------------------------------------------------
/// RKSemaphore_Destroy
///----------------------------------------------------------------------
void RKSemaphore_Destroy(RKSemaphore** ppSemaphore)
{
  RKSemaphoreInternal* pSemaphoreInternal = (RKSemaphoreInternal*)*ppSemaphore;
  *ppSemaphore = NULL;
/*
    sem_destroy(&pSemaphoreInternal->semaphore);
  */  

    RKCriticalSection_Destroy(&pSemaphoreInternal->mutex);
    RKCriticalSection_Destroy(&pSemaphoreInternal->internalMutex);
    
  RKHeap_Free(pSemaphoreInternal);
}

///----------------------------------------------------------------------
/// RKSemaphore_Signal
///----------------------------------------------------------------------
void RKSemaphore_Signal(RKSemaphore* pSemaphore)
{
  RKASSERT(pSemaphore, "error invalid semaphore");
  RKSemaphoreInternal* pSemaphoreInternal = (RKSemaphoreInternal*)pSemaphore;
 /*   
    ++pSemaphoreInternal->count;
    if (pSemaphoreInternal->count > pSemaphoreInternal->maxValue)
    {
        --pSemaphoreInternal->count;
        return;
    }

    int result = sem_post(&pSemaphoreInternal->semaphore);
    if (result != 0)
        --pSemaphoreInternal->count;
  */ 
    RKCriticalSection_Enter(pSemaphoreInternal->internalMutex);
    if (pSemaphoreInternal->currentValue == 0)
    {
        ++pSemaphoreInternal->currentValue;
        RKCriticalSection_Leave(pSemaphoreInternal->internalMutex);
        RKCriticalSection_Leave(pSemaphoreInternal->mutex);
        return;
    }
    else if (pSemaphoreInternal->currentValue == pSemaphoreInternal->maxValue)
    {
        RKCriticalSection_Leave(pSemaphoreInternal->internalMutex);
        return;
    }
    
    ++pSemaphoreInternal->currentValue;
    RKCriticalSection_Leave(pSemaphoreInternal->internalMutex);
    
    return;
}

///----------------------------------------------------------------------
/// RKSemaphore_Wait
///----------------------------------------------------------------------
bool RKSemaphore_Wait(RKSemaphore* pSemaphore, bool bBlockThread)
{
  RKASSERT(pSemaphore, "error invalid semaphore");
  RKSemaphoreInternal* pSemaphoreInternal = (RKSemaphoreInternal*)pSemaphore;
/*
    int result = sem_post(&pSemaphoreInternal->semaphore);
    if (result == 0)
    {
        --pSemaphoreInternal->count;
    }

  return result == 0;*/
    
    RKCriticalSection_Enter(pSemaphoreInternal->internalMutex);
    if (pSemaphoreInternal->currentValue > 1)
    {
        --pSemaphoreInternal->currentValue;
        RKCriticalSection_Leave(pSemaphoreInternal->internalMutex);
        return false;
    }
    
    RKCriticalSection_Leave(pSemaphoreInternal->internalMutex);
    
    RKCriticalSection_Enter(pSemaphoreInternal->mutex);
    
    --pSemaphoreInternal->currentValue;
    
    return false;
}

///----------------------------------------------------------------------
/// RKCriticalSection_Create
///----------------------------------------------------------------------
RKCriticalSection* RKCriticalSection_Create(const char* pName)
{
  RKCriticalSectionInternal* pCriticalSectionInternal = (RKCriticalSectionInternal*)RKHeap_AllocAndZero(sizeof(RKCriticalSectionInternal));
//   pCriticalSectionInternal->name.Copy(pName);
    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);

  int status = pthread_mutex_init(&pCriticalSectionInternal->mutex, &attr);
  if (status != 0)
  {
    RKHeap_Free(pCriticalSectionInternal);
    pCriticalSectionInternal = 0;
  }
    pthread_mutexattr_destroy(&attr);

  return (RKCriticalSection*)pCriticalSectionInternal;
}

///----------------------------------------------------------------------
/// RKCriticalSection_Destroy
///----------------------------------------------------------------------
void RKCriticalSection_Destroy(RKCriticalSection** ppCriticalSection)
{
  RKCriticalSectionInternal* pCriticalSectionInternal = (RKCriticalSectionInternal*)*ppCriticalSection;
  *ppCriticalSection = NULL;

  // Make sure we can lock this mutex
  bool lockStatus = pthread_mutex_lock(&pCriticalSectionInternal->mutex);
  if (lockStatus == 0)
  {
    pthread_mutex_t oldMutex = pCriticalSectionInternal->mutex;
      
      // <TODO> - Fix this cleanup code so that it works with the GameSWF Mutex wrapper
    //RKHeap_Free(pCriticalSectionInternal);
    //pthread_mutex_unlock(&oldMutex);
      pthread_mutex_unlock(&pCriticalSectionInternal->mutex);
    
    int destroyStatus = pthread_mutex_destroy(&pCriticalSectionInternal->mutex);
    RKASSERT(destroyStatus == 0, "Error destroying mutex.");
      
      return;
  }

  RKASSERT(false, "Error could not lock critical section for destroy.");
}

///----------------------------------------------------------------------
/// RKCriticalSection_Enter
///----------------------------------------------------------------------
void RKCriticalSection_Enter(RKCriticalSection* pCriticalSection)
{
    RKCriticalSectionInternal* pInternalCS = (RKCriticalSectionInternal*)pCriticalSection;
    pthread_mutex_lock(&pInternalCS->mutex);
}

///----------------------------------------------------------------------
/// RKCriticalSection_TryEnter
///----------------------------------------------------------------------
bool RKCriticalSection_TryEnter(RKCriticalSection* pCriticalSection)
{
    RKCriticalSectionInternal* pInternalCS = (RKCriticalSectionInternal*)pCriticalSection;
    return pthread_mutex_trylock(&pInternalCS->mutex) == 0;
}

///----------------------------------------------------------------------
/// RKCriticalSection_Leave
///----------------------------------------------------------------------
void RKCriticalSection_Leave(RKCriticalSection* pCriticalSection)
{
    RKCriticalSectionInternal* pInternalCS = (RKCriticalSectionInternal*)pCriticalSection;
    pthread_mutex_unlock(&pInternalCS->mutex);
}




///----------------------------------------------------------------------
///----------------------------------------------------------------------
RKThreadCondition* RKThreadCondition_Create(const char* pName)
{
  RKThreadCondition* pCondition = (RKThreadCondition*)RKHeap_AllocAndZero(sizeof(RKThreadCondition));
  pthread_cond_init(&pCondition->condition, NULL);
  return pCondition;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKThreadCondition_Destroy(RKThreadCondition** ppCondition)
{
  RKThreadCondition* pCondition = (RKThreadCondition*)*ppCondition;
  *ppCondition = 0;
  pthread_cond_destroy(&pCondition->condition);	
  RKHeap_Free(pCondition);
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
bool RKThreadCondition_WakeOne(RKThreadCondition* pCondition)
{  
  pthread_cond_signal(&pCondition->condition);
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
bool RKThreadCondition_WakeAll(RKThreadCondition* pCondition)
{  
  return pthread_cond_broadcast(&pCondition->condition);
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
bool RKThreadCondition_Sleep(RKThreadCondition* pCondition, RKCriticalSection* pMutex, RKUtils::DWORD time_sleep)
{  
  RKCriticalSectionInternal* pCriticalSectionInternal = (RKCriticalSectionInternal*)pMutex;
  int iRetVal = pthread_cond_wait(&pCondition->condition, &pCriticalSectionInternal->mutex);
  //RKASSERT(iRetVal != EINVAL,"could not wait on condition");
  return iRetVal==0;
}
}
 