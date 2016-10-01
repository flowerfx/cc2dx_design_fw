#ifndef RKCALLABLE_H
#define RKCALLABLE_H

#include "RKMacros.h"
namespace RKUtils
{
	template <typename RET>
	class RKCallableVirtual;

	template <typename RET>
	class RKCallable
	{
	public:
		inline RKCallable();
		inline RKCallable(const RKCallable<RET>& other); // copies content
		// if moveContent is true, then it moves content without copying if possible, passed callable becomes invalid
		inline RKCallable(RKCallable<RET>& other, bool moveContent);
		inline ~RKCallable();

		// creates callable out of function
		explicit RKCallable(RET(*pFn)());
		// creates callable out of function with captured argument
		template <typename ARG1>
		RKCallable(RET(*pFn)(ARG1), ARG1 arg1);
		// creates callable out of instance and method pointer
		template <typename OBJ>
		RKCallable(OBJ* pInst, RET(OBJ::*pMet)());
		// creates callable out of instance and method pointer with captured argument
		template <typename OBJ, typename ARG1>
		RKCallable(OBJ* pInst, RET(OBJ::*pMet)(ARG1), ARG1 arg1);

		// copy/move operators/functions
		inline void AssignCopy(const RKCallable<RET>& other); // copies content
		inline void AssignMove(RKCallable<RET>& other); // moves content without copying if possible, passed callable becomes invalid
		inline RKCallable& operator=(const RKCallable<RET>& other) { AssignCopy(other); return *this; } // copies content

		inline bool IsValid() const;
		inline void Reset(); // makes this callable invalid

		// creates callable out of function
		void Set(RET(*pFn)());
		// creates callable out of function with captured argument
		template <typename ARG1>
		void Set(RET(*pFn)(ARG1), ARG1 arg1);
		// creates callable out of instance and method pointer
		template <typename OBJ>
		void Set(OBJ* pInst, RET(OBJ::*pMet)());
		// creates callable out of instance and method pointer with captured argument
		template <typename OBJ, typename ARG1>
		void Set(OBJ* pInst, RET(OBJ::*pMet)(ARG1), ARG1 arg1);

		inline RET  Call() const;

	private:
		inline void _DestroyContent();
		inline void _Assign(RKCallableVirtual<RET>*& pCallable, size_t callableSize, bool passMemoryOwnership);
		inline void _Assign(RKCallable<RET>& other, bool moveContent);
		inline RKCallableVirtual<RET>* _GetAsCallableVirtual();

		//size_t m_storage[24 / sizeof(size_t)]; // 24 bytes
		size_t m_storage[6];  //6 elements (pointers) : 24 bytes on 32 bits, 48 bytes on 64 bits
	};
}
// scary and highly templated implementation
#define _RKCALLABLE_INTERNALS
#include "RKCallable.hpp"
#undef _RKCALLABLE_INTERNALS

#endif // RKCALLABLE_H