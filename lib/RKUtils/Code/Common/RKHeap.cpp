/*
 *  RKHeap.cpp
 *
 *  Created by Daniel Stephens on 12/10/08.
 *  Copyright 2008 Daniel Stephens. All rights reserved.
 *
 */

///----------------------------------------------------------------------
///----------------------------------------------------------------------
#include "RKHeap.h"
#include "RKMacros.h"

#ifdef GAME_WIN32
#include <crtdbg.h>
#endif
namespace RKUtils
{
#ifdef RKHEAP_UNMANAGED
	void RKHeap_InitModule()
	{
	}

	void RKHeap_DeinitModule()
	{
	}

	bool RKHeap_IsInited()
	{
		return true;
	}

	void* RKHeap_Alloc(uint32 size, const char* tag)
	{
		return malloc(size);
	}

	void* RKHeap_AllocAndZero(uint32 size, const char* tag)
	{
		void* p = malloc(size);
		if (p)
		{
			memset(p, 0, size);
		}
		return p;
	}

	void* RKHeap_Realloc(void* p, uint32 newSize)
	{
		return realloc(p, newSize);
	}

	void  RKHeap_Free(void* p, const char* tag)
	{
		free(p);
	}

	void* RKHeap_AllocAligned(uint32 size, uint32 alignment, const char *tag)
	{
#if defined (RKPLATFORM_WIN32)
		return _aligned_malloc(size, alignment);
#elif defined (RKPLATFORM_IOS)
		void* pMemory = 0;
		posix_memalign(&pMemory, alignment, size); // malloc(size);
		return pMemory;
#else
		return malloc(size);
#endif
	}

	void  RKHeap_FreeAligned(void* p, const char *tag)
	{
#if defined (RKPLATFORM_WIN32)
		_aligned_free(p);
#else
		free(p);
#endif // RKPLATFORM_WIN32
	}

	uint32 RKHeap_GetUsage()
	{
		return 0;
	}

	void RKHeap_CheckIntegrity()
	{
#ifdef GAME_WIN32
		if (_CrtCheckMemory() != TRUE)
		{
			RKASSERT(0, "Some memory has been corrupted!");
			char *p = 0; *p = 0;  //temp for disabled asserts..
		}
#else
		//iOS : todo: use a system memory check ?
#ifndef RKRETAIL
		//use system debug check at alloc/free time, and hope to catch problem if any
		void *checkAlloc1 = RKHeap_Alloc(50);
		RKHeap_Free(checkAlloc1);
#endif
#endif
	}

	void RKHeap_SetDumpFilename(const char *pDumpFilename)
	{
	}

	void RKHeapError_EnableReport(bool enab)
	{
	}

	void RKHeap_Report()
	{
	}

	void RKHeap_EnableSeriousMemoryChecking(bool enabled)
	{
	}
#else //RKHEAP_UNMANAGED
#include "RKFile.h"
#include <JRSMemory.h>
#include <JRSMemory_Pools.h>
#ifndef RKRETAIL
#define DEBUG_HEAP_OVERHEAD 170 * 1024 * 1024
#else
#define DEBUG_HEAP_OVERHEAD 0
#endif

#define RKHEAP_ENABLE_LOG
#if defined(GAME_WIN32)
#define RKHEAP_DEFAULTHEAPSIZE 60*1024*1024 + DEBUG_HEAP_OVERHEAD
#define RKHEAP_GRAPHICSHEAPSIZE 10*1024*1024 + DEBUG_HEAP_OVERHEAD
#define RKHEAP_ELEPHANTSIZE 65*1024*1024 + DEBUG_HEAP_OVERHEAD
#else
#define RKHEAP_DEFAULTHEAPSIZE 70*1024*1024 + DEBUG_HEAP_OVERHEAD
#define RKHEAP_GRAPHICSHEAPSIZE 0
#define RKHEAP_ELEPHANTSIZE 75*1024*1024 + DEBUG_HEAP_OVERHEAD
#endif //GAME_WIN32

#define RKHEAP_ELEPHANT_STACK_SKIP_DEPTH 5
#define RKHEAP_DEFAULTALIGNMENT 16
	cHeap* RKDefaultHeap = 0;
	cHeap* RKGraphicsHeap = 0;

#define RK_SMALL_HEAP_SIZE 16
#if RK_SMALL_HEAP_SIZE > 0
#define RK_MID_HEAP_SIZE 32

	cPool* RK16BytePool = 0;
	cPool* RK32BytePool = 0;
#endif //RK_SMALL_HEAP_SIZE > 0

#ifdef RKASSERT
#undef RKASSERT
#endif
#define RKASSERT(a, b) { assert(a && b); } // no special sauce because regular RKASSERT uses heap

	static const char *gRKHeapDumpFilename = 0;
	static bool gRKHeapError_EnableReport = true;
	static int gRKHeapError_Count = 0;

	///----------------------------------------------------------------------
	/// RKHeap_Log()
	///----------------------------------------------------------------------
	void RKHeap_Log(const jrs_i8 *pText)
	{
#if defined(RKHEAP_ENABLE_LOG)
		RKLOG("%s\n", pText);
#endif
	}

	///----------------------------------------------------------------------
	/// RKHeap_SetDumpFilename()
	///----------------------------------------------------------------------
	void RKHeap_SetDumpFilename(const char *pDumpFilename)
	{
		gRKHeapDumpFilename = pDumpFilename;
	}

	///----------------------------------------------------------------------
	/// RKHeapError_EnableReport()
	///----------------------------------------------------------------------
	void RKHeapError_EnableReport(bool enab)
	{
		gRKHeapError_EnableReport = enab;
	}

	///----------------------------------------------------------------------
	/// RKHeap_Error()
	///----------------------------------------------------------------------
	void RKHeap_Error(const jrs_i8 *pError, jrs_u32 uErrorID)
	{
		gRKHeapError_Count++;

		if (uErrorID == 0x2001d)  //eg: Elephant message: "Error 0x2001d: Out of memory, cannot allocate 121 bytes from heap named Default.  Not enough free space"
		{
			//RKLOG and RKASSERT allocates ! .. will recurse and crash ...
			char *p = (char*)0x3e3bad; //'mem bad'
			*p = 0;   //Let's crash Here,on bad write access ..-> "Unhandled exception at 0x... in Corona_d.exe: 0xC0000005: Access violation writing location 0x003e3bad."
		}


		if (gRKHeapError_EnableReport)
		{
			gRKHeapError_EnableReport = false;  //avoid recurse from error in ReportAll() !!  eg: pError	0x0018d5cc "Error 0x4000a: Allocated counts do not match."

			cMemoryManager::Get().ReportAll(gRKHeapDumpFilename);
			RKASSERT(false, pError);
			gRKHeapError_EnableReport = true; //restore
		}
		else
		{
			RKLOG("NOT ASSERTING:  RKHeap_Error(%s)", pError);
		}

	}

	///----------------------------------------------------------------------
	/// RKHeap_Dump()
	///----------------------------------------------------------------------
	void RKHeap_Dump(void *pData, int size, const jrs_i8 *pFilePathAndName, jrs_bool bAppend)
	{
		if (RKFile* file = RKFile_Open(pFilePathAndName, bAppend ? RKFM_APPEND : RKFM_WRITE))
		{
			RKFile_Write(file, pData, size);
			RKFile_Close(&file); 
		}
#if defined(GAME_WIN32)
		else if (FILE* file = fopen(pFilePathAndName, bAppend? "ab" : "wb"))
		{
			fwrite(pData, size, 1, file);
			fclose(file);
		}
#endif // GAME_WIN32
	}

	///----------------------------------------------------------------------
	/// RKHeap_InitModule()
	///----------------------------------------------------------------------
	void RKHeap_InitModule()
	{
		RKASSERT(!cMemoryManager::Get().IsInitialized(), "Memory Manager already initialized");



		cHeap::sHeapDetails details;
#if !defined(RKRETAIL)
		details.bEnableSentinelChecking = true;
		cMemoryManager::InitializeEnhancedDebugging();
		cMemoryManager::InitializeLiveView();
#endif
		details.uDefaultAlignment = RKHEAP_DEFAULTALIGNMENT;
		details.bAllowDestructionWithAllocations = false;
		details.bAllowNullFree = true;  // it happens

		cMemoryManager::InitializeCallbacks(RKHeap_Log, RKHeap_Error, RKHeap_Dump);
		bool success = (cMemoryManager::Get().Initialize(RKHEAP_ELEPHANTSIZE, 0, false) != 0);
		//static volatile bool * pSucc = NULL;
		//success = *pSucc;
		RKASSERT(success, "Memory Manager failed to initialize");

		RKDefaultHeap = cMemoryManager::Get().CreateHeap(RKHEAP_DEFAULTHEAPSIZE, "Default", &details);
		RKDefaultHeap->SetCallstackDepth(RKHEAP_ELEPHANT_STACK_SKIP_DEPTH);



#if defined(GAME_WIN32)
		details.uDefaultAlignment = RKHEAP_DEFAULTALIGNMENT;
		details.bEnableSentinelChecking = false;
		details.bAllowNullFree = false;
		details.bEnableSentinelChecking = false;
		details.bEnableEnhancedDebug = false;
		//RKGraphicsHeap = cMemoryManager::Get().CreateHeap(RKHEAP_GRAPHICSHEAPSIZE, "Graphics", &details);
#endif
#if RK_SMALL_HEAP_SIZE > 0
		Elephant::sPoolDetails DetPoolDetails;
		DetPoolDetails.bThreadSafe = true;
		DetPoolDetails.pOverrunHeap = RKDefaultHeap;

		RK16BytePool = cMemoryManager::Get().CreatePool(RK_SMALL_HEAP_SIZE, 85 * 1024, "SmallPool16", &DetPoolDetails, RKDefaultHeap);

		RK32BytePool = cMemoryManager::Get().CreatePool(RK_MID_HEAP_SIZE, 55 * 1024, "SmallPool32", &DetPoolDetails, RKDefaultHeap);
#endif
	}

	///----------------------------------------------------------------------
	/// RKHeap_DeinitModule()
	///----------------------------------------------------------------------
	void RKHeap_DeinitModule()
	{
		if (cMemoryManager::Get().IsInitialized())
		{
			gRKHeapError_Count = 0;
			cMemoryManager::Get().Destroy();
			if (gRKHeapError_Count == 0)
			{
				RKLOG("------- Elephant Memory Manager succefully destroyed, with NO LEAK or Error :) ---------");
			}
		}
		RKDefaultHeap = 0;
		RKGraphicsHeap = 0;
	}

	bool RKHeap_IsInited()
	{
		return RKDefaultHeap != 0;
	}

	///----------------------------------------------------------------------
	/// RKHeap Standard Allocators
	///----------------------------------------------------------------------

	void* RKHeap_Alloc(uint32 size, const char* tag)
	{
#if RK_SMALL_HEAP_SIZE > 0
		if(size <= RK_MID_HEAP_SIZE)
		{
			if(size <= RK_SMALL_HEAP_SIZE)
			{
				return RK16BytePool->AllocateMemory(tag);
			}else
			{
				return RK32BytePool->AllocateMemory(tag);
			}
		}
#endif
		RKASSERT(RKDefaultHeap, "Default heap not initialized");
		return RKDefaultHeap->AllocateMemory(size, RKHEAP_DEFAULTALIGNMENT, 0, tag);
	}

	void* RKHeap_AllocAndZero(uint32 size, const char* tag)
	{
#if RK_SMALL_HEAP_SIZE > 0
		if(size <= RK_MID_HEAP_SIZE)
		{
			void* pOut;
			if(size <= RK_SMALL_HEAP_SIZE)
			{
				pOut = RK16BytePool->AllocateMemory(tag);
				memset(pOut, 0, RK_SMALL_HEAP_SIZE);
			}else
			{
				pOut = RK32BytePool->AllocateMemory(tag);
				memset(pOut, 0, RK_MID_HEAP_SIZE);
			}
		}
#endif
		RKASSERT(RKDefaultHeap, "Default heap not initialized");
		void* p = RKDefaultHeap->AllocateMemory(size, RKHEAP_DEFAULTALIGNMENT, 0, tag);
		memset(p, 0, size);
		return p;
	}

	void* RKHeap_Realloc(void* p, uint32 newSize)
	{
#if RK_SMALL_HEAP_SIZE > 0
		if(RK16BytePool->IsAllocatedFromThisPool(p))
		{
			if(newSize <= RK_SMALL_HEAP_SIZE)
			{
				return p;
			}
			void* pNewData = RKDefaultHeap->AllocateMemory(newSize, RKHEAP_DEFAULTALIGNMENT);
			memcpy(pNewData, p, RK_SMALL_HEAP_SIZE);
			RK16BytePool->FreeMemory(p);
			return pNewData;
		}else
			if(RK32BytePool->IsAllocatedFromThisPool(p))
			{
				if(newSize <= RK_MID_HEAP_SIZE)
				{
					return p;
				}
				void* pNewData = RKDefaultHeap->AllocateMemory(newSize, RKHEAP_DEFAULTALIGNMENT);
				memcpy(pNewData, p, RK_MID_HEAP_SIZE);
				RK16BytePool->FreeMemory(p);
				return pNewData;
			}
#endif
		RKASSERT(RKDefaultHeap, "Default heap not initialized");
		return RKDefaultHeap->ReAllocateMemory(p, newSize, RKHEAP_DEFAULTALIGNMENT);
	}

	void RKHeap_Free(void* p, const char* tag)
	{
#if RK_SMALL_HEAP_SIZE > 0
		if (cPool* pool = (cPool*)RKDefaultHeap->GetPoolFromAllocatedMemory(p))
		{
			pool->FreeMemory(p, tag);
			return;
		}
#endif
		RKASSERT(RKDefaultHeap, "Default heap not initialized");
		if (p)
		{
			uint32 flag = cMemoryManager::Get().GetAllocationFlag(p);
			RKDefaultHeap->FreeMemory(p, flag, tag);
		}
	}

	void* RKHeap_AllocAligned(uint32 size, uint32 alignment, const char *tag)
	{
		RKASSERT(RKDefaultHeap, "Default heap not initialized");
		return RKDefaultHeap->AllocateMemory(size, alignment, 0, tag);
	}

	void  RKHeap_FreeAligned(void* p, const char *tag)
	{
		RKASSERT(RKDefaultHeap, "Default heap not initialized");
		RKDefaultHeap->FreeMemory(p, 0, tag);
	}




	///----------------------------------------------------------------------
	/// Graphics Heap Allocators
	///
	/// The graphics heap is only used on Windows build to track graphics
	/// memory consumption in Goldfish
	///----------------------------------------------------------------------

	void* RKHeap_AllocGraphics(uint32 size, const char* tag)
	{
#if defined(GAME_WIN32)
		return RKDefaultHeap->AllocateMemory(size, RKHEAP_DEFAULTALIGNMENT, 0, tag);
#else
		return 0;
#endif
	}

	void RKHeap_FreeGraphics(void* p, const char* tag)
	{
#if defined(GAME_WIN32)
		RKDefaultHeap->FreeMemory(p, 0, tag);
#endif
	}

	///----------------------------------------------------------------------
	/// Debug Utilities
	///----------------------------------------------------------------------
	uint32 RKHeap_GetUsage()
	{
		return RKDefaultHeap->GetMemoryUsed();//RKDefaultHeap->GetSize();
	}

	void RKHeap_CheckIntegrity()
	{
		cMemoryManager::Get().CheckForErrors();
	}

	void RKHeap_Report()
	{
		cMemoryManager::Get().ReportAll();
	}

	unsigned int RKHeap_GetAllocedSize(void* memaddress)
	{
		return cMemoryManager::Get().SizeofAllocation(memaddress);
	}

	void RKHeap_CheckValidPointer(void* memaddress)
	{
		if (RKDefaultHeap && RKDefaultHeap->IsAllocatedFromThisHeap(memaddress) == false)
		{
			//not from the main heap ..

#if 1 //RK_SMALL_HEAP_SIZE > 0
			if (RK16BytePool && RK16BytePool->IsAllocatedFromThisPool(memaddress))
				return; //ok

			if (RK32BytePool && RK32BytePool->IsAllocatedFromThisPool(memaddress))
				return; //ok
#endif

			RKAssert("BAD! RKHeap_Destroy(%X), ptr not allocated from Elephant heap or pool! [already released !? / alloc/release missmatch?]", memaddress);
		}
	}


	void RKHeap_EnableSeriousMemoryChecking(bool enabled)
	{
		RKDefaultHeap->EnableExhaustiveSentinelChecking(enabled);
	}
#endif //RKHEAP_UNMANAGED
}