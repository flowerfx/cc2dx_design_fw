/*
 *  RKHeap.h
 *
 *  Created by Daniel Stephens on 12/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RKHEAP_H
#define RKHEAP_H

#include "RKTypes.h"
#include <new>
namespace RKUtils
{
#  define RKHEAP_UNMANAGED 1



#if defined(RKHEAP_UNMANAGED)
#define RKNEW(T) new T
#define RKDELETE(P) delete P
#define RKNEWARRAY(T, X) new T[X]
#define RKDELETEARRAY(P) delete [] P
#elif defined(RKRETAIL)
#define RKNEW(T) new(RKHeap_Alloc(sizeof(T))) T
#define RKDELETE(P) RKHeap_Destroy(P)
#define RKNEWARRAY(T, X) RKHeap_CreateArray<T>(X)
#define RKDELETEARRAY(P) RKHeap_DestroyArray(P)
#else
#define RKTMPSUBSTR2(X)     #X
#define RKTMPSUBSTR(X)      RKTMPSUBSTR2(X)
#define RKFILEPOS    __FILE__ ":" RKTMPSUBSTR(__LINE__) 
#define RKNEW(T) new(RKHeap_Alloc(sizeof(T), RKFILEPOS)) T
#define RKDELETE(P) RKHeap_Destroy(P, RKFILEPOS)
#define RKNEWARRAY(T, X) RKHeap_CreateArray<T>(X, RKFILEPOS)
#define RKDELETEARRAY(P) RKHeap_DestroyArray(P, RKFILEPOS)
#endif //RKRETAIL

	void RKHeap_InitModule();
	void RKHeap_DeinitModule();
	bool RKHeap_IsInited();

	void* RKHeap_Alloc(uint32 size, const char* tag = 0);
	void* RKHeap_AllocAndZero(uint32 size, const char* tag = 0);
	void* RKHeap_Realloc(void* p, uint32 newSize);
	void  RKHeap_Free(void* p, const char* tag = 0);
	void* RKHeap_AllocAligned(uint32 size, uint32 alignment, const char *tag = 0);
	void  RKHeap_FreeAligned(void* p, const char *tag = 0);

	uint32 RKHeap_GetUsage();
	void RKHeap_CheckIntegrity();
	unsigned int RKHeap_GetAllocedSize(void* memaddress);
	void RKHeap_CheckValidPointer(void* memaddress);
	void RKHeap_SetDumpFilename(const char *pDumpFilename);
	void RKHeap_Report();
	void RKHeap_EnableSeriousMemoryChecking(bool enabled);
	void RKHeapError_EnableReport(bool enab);


	template <class T>
	void RKHeap_Destroy(const T* p, const char* tag = 0)
	{
		if (p)
		{
#if 0 //#ifdef _DEBUG
			//to have an assert here, instead of deeper in elephant error report handler /comment out if you feel some slowdown - [christophe.lebouil - 2014-04-01], leak hunt fixes
			RKHeap_CheckValidPointer((T*)p);
#endif

			p->~T();
			RKHeap_Free((T*)p, tag);
		}
	}

	/**
	 * You may be wondering why on earth this function exists and you would be right to do so.
	 *
	 * Basically, according to the C++ standard, when an array of N objects is allocated for
	 * a class T, the space it takes up is N*sizeof(T)+Y. What is Y? Y is used to keep track of how
	 * big the array is so that it can be cleaned up later. However, the sizeof(Y) is completely
	 * implementation specific, meaning that placement new for arrays is effectively useless, and we
	 * will have no idea how much space to allocate for an array without, well, allocating the array.
	 *
	 * The implementation we have here mimics what Visual C++ does by inserting a count field at the
	 * head of the array. Note that this means any special alignment you planned that is > 4 bytes is
	 * effectively screwed. Anyway, if it was that important to you, you would be using RKHeap_AllocAligned()!
	 */
	template <class T>
	T* RKHeap_CreateArray(uint32 numelements, const char* tag = 0)
	{
		uint32* storage = (uint32*)RKHeap_Alloc(numelements * sizeof(T) + sizeof(uint32), tag);
		*storage = numelements;
		T* data = (T*)(++storage);
		for (uint32 i = 0; i < numelements; ++i)
		{
			new (data + i) T;
		}
		return data;
	}

	template <class T>
	void RKHeap_DestroyArray(const T* p, const char* tag = 0)
	{
		if (p)
		{
			uint32* numelements = (uint32*)(p)-1;
			for (int i = *numelements - 1; i >= 0; --i) // destruction in reverse order of construction
			{
				p[i].~T();
			}
			RKHeap_Free(numelements, tag);
		}
	}
}
#endif // RKHEAP_H