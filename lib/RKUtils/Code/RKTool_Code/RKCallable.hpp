#ifndef _RKCALLABLE_INTERNALS
#  error This file should be included from RKCallable.h only
#endif
namespace RKUtils
{
	// encapsulates an entity that can be called with a result of type RET
	template <typename RET>
	class RKCallableVirtual
	{
		friend class RKCallable<RET>; // needs access to _RawSize and _Copy
	public:
		virtual ~RKCallableVirtual() {}

		virtual RET Call() const = 0;

	private: // these low-level private function used by RKCallable to allow copying of callables
		virtual size_t _RawSize() const = 0;              // returns sizeof of implementation
		virtual void _CreateCopyAtMemory(void* pDestination) const = 0; // create a full copy placed at pDestination, much like placement copy ctor
	};

#pragma region RKCallable

	// different virtual callables have different sizeof, and because of that
	// can't be allocated on stack, which leads to reduced performance
	// because of dynamic allocations
	//
	// this handle provides a way of allocating a storage of fixed size
	// on stack and placing callable in it in a safe way
	//
	// RKCallable can be either vaid or invalid
	// invalid callable roughly corresponds to (RKCallableVirtual*) == NULL;

	// RKCallable can be in four states:
	// m_storage[0] == 0 and m_storage[1] == 0                       --> invalid
	// m_storage[0] != 0                                             --> m_storage contains memory of statically allocated RKCallableVirtual
	// m_storage[0] == 0 and m_storage[1] != 0 and m_storage[2] == 0 --> m_storage[1] contains poitner to dynamically allocated RKCallableVirtual, which was allocated by external code and should be deallocated with RKDELETE
	// m_storage[0] == 0 and m_storage[1] != 0 and m_storage[2] != 0 --> m_storage[1] contains poitner to dynamically allocated RKCallableVirtual, which was allocated inside of RKCallable and should be deallocated with RKHeap_Free, m_storage[2] contains raw size of RKCallableVirtual

	template <typename RET>
	bool RKCallable<RET>::IsValid() const
	{
		RKSTATICASSERT(sizeof(m_storage) == sizeof(*this), "Padding occured, memory wasted");
		RKSTATICASSERT(ARRAYSIZE(m_storage) >= 3, "Storage buffer is not big enough");
		RKSTATICASSERT(sizeof(m_storage[0]) == sizeof(void*), "First element must be the same size as pointer, otherwise IsValid function won't work");
		// if callable is valid, then first 4 bytes would contain pointer to virtual table
		return m_storage[0] != 0 || m_storage[1] != 0;
	}

	template <typename RET>
	RKCallableVirtual<RET>* RKCallable<RET>::_GetAsCallableVirtual()
	{
		if (m_storage[0] != 0)
		{
			return reinterpret_cast<RKCallableVirtual<RET>*>(&m_storage);
		}
		else
		{
			RKASSERT(m_storage[1] != 0, "Trying to access an invalid callable");
			return reinterpret_cast<RKCallableVirtual<RET>*>(m_storage[1]);
		}
	}

	// warning! it does NOT mark object as invalid, it only destroys content
	template <typename RET>
	void RKCallable<RET>::_DestroyContent()
	{
		if (IsValid())
		{
			RKCallableVirtual<RET>* pCallable = _GetAsCallableVirtual();
			if (m_storage[0]) // if it's a statically allocated callable
			{
				pCallable->~RKCallableVirtual(); // calling destructor for statically allocated RKCallable
			}
			else
			{
				if (m_storage[1] == 0)
				{
					RKDELETE(pCallable); // deleting dynamically allocated RKCallable
				}
				else
				{
					RKHeap_Free(pCallable);
				}
			}
		}
	}

	// warning! it does NOT destroy current content, calling code should ensure that content is destroyed
	// if callableSize == 0, it means that size is unknown and has to be queried
	template <typename RET>
	void RKCallable<RET>::_Assign(RKCallableVirtual<RET>*& pCallable, size_t callableSize, bool passMemoryOwnership)
	{
		if (pCallable)
		{
			if (passMemoryOwnership)
			{
				m_storage[0] = 0;
				m_storage[1] = reinterpret_cast<size_t>(pCallable);
				m_storage[2] = 0;
				pCallable = NULL;
				RKASSERT(callableSize == 0, "If callable size is known, then most likely it's an internally allocated callable, that shouldn't be happening (or you have to properly initialize m_storage[2])");
			}
			else
			{
				if (callableSize == 0)
				{
					callableSize = pCallable->_RawSize();
				}
				if (callableSize <= sizeof(m_storage))
				{
					pCallable->_CreateCopyAtMemory(&m_storage);
					RKASSERT(m_storage[0] != 0, "Impossible happened, looks like your callable is not virtual, IsValid won't work");
				}
				else
				{
					m_storage[0] = 0;
					void* data = RKHeap_Alloc((uint32)callableSize, "RKCallable copy");
					pCallable->_CreateCopyAtMemory(data);
					m_storage[1] = reinterpret_cast<size_t>(data);
					m_storage[2] = callableSize;
				}
			}
		}
		else
		{
			m_storage[0] = 0;
			m_storage[1] = 0;
		}
	}

	// warning! it does NOT destroy current content, calling code should ensure that content is destroyed
	template <typename RET>
	void RKCallable<RET>::_Assign(RKCallable<RET>& other, bool moveContent)
	{
		if (other.IsValid())
		{
			if (moveContent && other.m_storage[0] == 0)
			{
				m_storage[0] = 0;
				m_storage[1] = other.m_storage[1];
				m_storage[2] = other.m_storage[2];
				other.m_storage[1] = 0; // reset other
			}
			else
			{
				RKCallableVirtual<RET>* pCallable = other._GetAsCallableVirtual();
				if (other.m_storage[0] != 0)
				{
					_Assign(pCallable, sizeof(m_storage), false);
				}
				else
				{
					_Assign(pCallable, other.m_storage[2], false);
				}
				RKASSERT(pCallable == other._GetAsCallableVirtual(), "Internal inconsistency, const variable was changed");
				if (moveContent)
				{
					other.Reset();
				}
			}
		}
		else
		{
			m_storage[0] = 0;
			m_storage[1] = 0;
		}
	}

	template <typename RET>
	RKCallable<RET>::RKCallable()
	{
		m_storage[0] = 0;
		m_storage[1] = 0;
	}

	template <typename RET>
	RKCallable<RET>::RKCallable(RET(*pFn)())
	{
		m_storage[0] = 0;
		m_storage[1] = 0;

		Set(pFn);
	}

	template <typename RET>
	template <typename ARG1>
	RKCallable<RET>::RKCallable(RET(*pFn)(ARG1), ARG1 arg1)
	{
		m_storage[0] = 0;
		m_storage[1] = 0;

		Set(pFn, arg1);
	}

	template <typename RET>
	template <typename OBJ>
	RKCallable<RET>::RKCallable(OBJ* pInst, RET(OBJ::*pMet)())
	{
		m_storage[0] = 0;
		m_storage[1] = 0;

		Set(pInst, pMet);
	}

	template <typename RET>
	template <typename OBJ, typename ARG1>
	RKCallable<RET>::RKCallable(OBJ* pInst, RET(OBJ::*pMet)(ARG1), ARG1 arg1)
	{
		m_storage[0] = 0;
		m_storage[1] = 0;

		Set(pInst, pMet, arg1);
	}

	template <typename RET>
	RKCallable<RET>::~RKCallable()
	{
		_DestroyContent();
	}

	template <typename RET>
	RKCallable<RET>::RKCallable(const RKCallable<RET>& other)
	{
		_Assign(const_cast<RKCallable<RET>&>(other), false);
	}

	template <typename RET>
	RKCallable<RET>::RKCallable(RKCallable<RET>& other, bool moveContent)
	{
		_Assign(other, moveContent);
	}

	template <typename RET>
	void RKCallable<RET>::AssignCopy(const RKCallable<RET>& other)
	{
		_DestroyContent();
		_Assign(const_cast<RKCallable<RET>&>(other), false);
	}

	template <typename RET>
	void RKCallable<RET>::AssignMove(RKCallable<RET>& other)
	{
		_DestroyContent();
		_Assign(other, true);
	}

	template <typename RET>
	inline void RKCallable<RET>::Reset()
	{
		_DestroyContent();
		m_storage[0] = 0;
		m_storage[1] = 0;
	}

	template <typename RET>
	inline RET RKCallable<RET>::Call() const
	{
		return const_cast<RKCallable<RET>*>(this)->_GetAsCallableVirtual()->Call();
	}

#pragma endregion 

#pragma region RKCallableImplementations

	template <typename RET, typename IMPL>
	class RKCallableCopyHelper : public RKCallableVirtual<RET>
	{
		virtual size_t _RawSize() const
		{
			return sizeof(IMPL);
		}

		virtual void _CreateCopyAtMemory(void* pDestination) const
		{
			new (pDestination)IMPL(*guarded_pointer_cast<const IMPL>(this));
		}
	};

	template <typename RET>
	class RKFunctionCallable : public RKCallableCopyHelper<RET, RKFunctionCallable<RET> >
	{
	public:
		typedef RET(*FunctionPtr)();

		explicit RKFunctionCallable(FunctionPtr pFn) : m_pFn(pFn)                         { RKASSERT(m_pFn, "NULL function"); }
		virtual RET Call() const { return m_pFn(); }

	private:
		FunctionPtr m_pFn;
	};

	template <typename RET, typename ARG1>
	class RKFunctionArg1Callable : public RKCallableCopyHelper<RET, RKFunctionArg1Callable<RET, ARG1> >
	{
	public:
		typedef RET(*FunctionPtr)(ARG1);

		RKFunctionArg1Callable(FunctionPtr pFn, ARG1 arg1) : m_pFn(pFn), m_arg1(arg1)                          { RKASSERT(m_pFn, "NULL function"); }
		virtual RET Call() const { return m_pFn(m_arg1); }

	private:
		FunctionPtr m_pFn;
		ARG1 m_arg1;
	};

	template <typename RET, typename OBJ>
	class RKMethodCallable : public RKCallableCopyHelper<RET, RKMethodCallable<RET, OBJ> >
	{
	public:
		typedef RET(OBJ::*MethodPtr)();

		RKMethodCallable(OBJ* pInst, MethodPtr pMet) : m_pInst(pInst), m_pMet(pMet)                    { RKASSERT(m_pInst, "NULL instance"); RKASSERT(m_pMet, "NULL method"); }
		RKMethodCallable(const RKMethodCallable& other) : m_pInst(other.m_pInst), m_pMet(other.m_pMet) { RKASSERT(m_pInst, "NULL instance"); RKASSERT(m_pMet, "NULL method"); }
		virtual RET Call() const { return (m_pInst->*m_pMet)(); }

	private:
		OBJ* m_pInst;
		MethodPtr m_pMet;
	};

	template <typename RET, typename OBJ, typename ARG1>
	class RKMethodArg1Callable : public RKCallableCopyHelper<RET, RKMethodArg1Callable<RET, OBJ, ARG1> >
	{
	public:
		typedef RET(OBJ::*MethodPtr)(ARG1);

		RKMethodArg1Callable(OBJ* pInst, MethodPtr pMet, ARG1 arg1) : m_pInst(pInst), m_pMet(pMet), m_arg1(arg1)                     { RKASSERT(m_pInst, "NULL instance"); RKASSERT(m_pMet, "NULL method"); }
		virtual RET Call() const { return (m_pInst->*m_pMet)(m_arg1); }

	private:
		OBJ* m_pInst;
		MethodPtr m_pMet;
		ARG1 m_arg1;
	};

#pragma endregion

#pragma region RKCallableSetters

	template <typename RET>
	void RKCallable<RET>::Set(RET(*pFn)())
	{
		typedef RKFunctionCallable<RET> CallableType;

		Reset();
		if (sizeof(m_storage) >= sizeof(CallableType))
		{
			new (this) CallableType(pFn);
		}
		else
		{
			RKCallableVirtual<RET>* pCallable = RKNEW(CallableType)(pFn);
			_Assign(pCallable, 0, true);
		}
	}

	template <typename RET> template <typename ARG1>
	void RKCallable<RET>::Set(RET(*pFn)(ARG1), ARG1 arg1)
	{
		typedef RKFunctionArg1Callable<RET, ARG1> CallableType;

		Reset();
		if (sizeof(m_storage) >= sizeof(CallableType))
		{
			new (this) CallableType(pFn, arg1);
		}
		else
		{
			RKCallableVirtual<RET>* pCallable = RKNEW(CallableType)(pFn, arg1);
			_Assign(pCallable, 0, true);
		}
	}

	template <typename RET> template <typename OBJ>
	void RKCallable<RET>::Set(OBJ* pInst, RET(OBJ::*pMet)())
	{
		typedef RKMethodCallable<RET, OBJ> CallableType;

		Reset();
		if (sizeof(m_storage) >= sizeof(CallableType))
		{
			new (this) CallableType(pInst, pMet);
		}
		else
		{
			RKCallableVirtual<RET>* pCallable = RKNEW(CallableType)(pInst, pMet);
			_Assign(pCallable, 0, true);
		}
	}

	template <typename RET> template <typename OBJ, typename ARG1>
	void RKCallable<RET>::Set(OBJ* pInst, RET(OBJ::*pMet)(ARG1), ARG1 arg1)
	{
		typedef RKMethodArg1Callable<RET, OBJ, ARG1> CallableType;

		Reset();
		if (sizeof(RKCallable<RET>) >= sizeof(CallableType))
		{
			new (this) CallableType(pInst, pMet, arg1);
		}
		else
		{
			RKCallableVirtual<RET>* pCallable = RKNEW(CallableType)(pInst, pMet, arg1);
			_Assign(pCallable, 0, true);
		}
	}
}
#pragma endregion