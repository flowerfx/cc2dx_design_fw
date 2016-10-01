///----------------------------------------------------------------------
/// RKEngine	(c) 2006 + Daniel Stephens
///
/// File		:	RKLists.h
///
/// Description :	Templated vector class
///----------------------------------------------------------------------
#ifndef _RK_LISTS_H_
#define _RK_LISTS_H_

#include <algorithm>

#include "../Common/RK.h"
#include "RKSort.h"
#include "RKArray.h"
#include "../Common/RKHeap.h"
#include "../Common/RKTypes.h"
namespace RKUtils
{
	// defines a non-resizable list with values
	// RKLIST_DEF(int, listName, 1, 2, 3, 4, 5);
	// roughly equals to
	// RKList<int> listName = {1, 2, 3, 4, 5};
#define RKLIST_DEF(T, name, ...) \
	T __initialArray_##name[] = {__VA_ARGS__};\
	RKList<T> name(__initialArray_##name, ARRAYSIZE(__initialArray_##name));


	class RKListBase
	{
	public:
		RKListBase()
			: m_Size(0)
			, m_Capacity(0)
		{
		}

		uint32 Size() const     { return m_Size; }
		uint32 Capacity() const { return m_Capacity; }

	protected:
		uint32 m_Size;
		uint32 m_Capacity;
	};

	// TODO: if needed add Append/Prepend/Insert Array/List
	// TODO: Squeeze function? Squeeze <==> Reallocate(Size())
	// TODO: EraseAll/EraseAllSorted are quite inefficient
	// MAYBE: it is possible to remove requirement of default ctor existence 
	//        in T by removing implicit object creation in Resize function
	// if you need to put RKList into a map/tree, see RKCollectionComparisonObject
	template <typename T, uint32 LOCALSTORAGESIZE = 0>
	class RKList : private RKListBase
	{
	public:
		typedef T ElementType;

		RKList();
		explicit RKList(uint32 nCapacity, bool bResizable = false);
		RKList(const T* array, uint32 arraySize, bool bResizable = false);
		RKList(const RKList& other);
		template <uint32 SIZE>
		explicit RKList(const RKArray<T, SIZE>& rkarray);
		~RKList()               { m_Mode = MODE_DYNAMIC; Clear(); RKASSERT(m_Array == NULL, "RKList data was not deallocated for some reason"); }

		bool   IsEmpty() const  { return m_Size == 0; }
		bool   IsFull() const   { return m_Mode == MODE_FIXED_STORAGE && m_Size == m_Capacity; }
		void   Clear();
		using  RKListBase::Size;
		using  RKListBase::Capacity;

		const T& operator[](uint32 index) const;
		T&			 operator[](uint32 index);
		const T& GetAt(uint32 index) const           { return operator[](index); } // more readable when you only have a pointer to RKList, in other cases please use operator[]
		const T* GetAddress(uint32 index) const           { return &operator[](index); }
		void     SetAt(uint32 index, const T& value) { operator[](index) = value; }
		const T& FirstElement() const { return operator[](0); }
		const T& LastElement()  const { return operator[](Size() - 1); }
		T& FirstElement()       { return operator[](0); }
		T& LastElement()        { return operator[](Size() - 1); }
		const T* ArrayPointer() const { return m_Array; }
		T*       ArrayPointer()       { return m_Array; }

		void   FillWith(const T& value, int size = -1); // by default uses current size
		void   AssignArray(const T* array, uint32 arraySize);
		bool   EqualToArray(const T* array, uint32 arraySize) const;

		bool   Contains(const T& entry) const;     // returns true if array contains element
		uint32 Count(const T& entry) const;        // returns count of elements like that in array
		int    FirstIndexOf(const T& entry, uint32 startingFrom = 0) const; // returns -1 if not found

		void   MakeNonShrinking()             { RKASSERT(m_Mode == MODE_DYNAMIC, "Can change only dynamic lists"); m_Mode = MODE_NON_SHRINKING; }
		void   Reserve(uint32 nCapacity)      { _Reserve(nCapacity); }
		void   ReserveFixed(uint32 nCapacity) { _Reserve(nCapacity); m_Mode = MODE_FIXED_STORAGE; }
		void   Resize(uint32 nSize, bool bPreserveContent = true, const T& fillValue = T());
		void   Append(const T& entry);
		void   Insert(const T& entry, uint32 atPosition);
		void   Prepend(const T& entry) { Insert(entry, 0); }
		// Append/Insert/Prepend Unique only add element if it's not yet presented in list, return true on success
		bool   AppendUnique(const T& entry)                    { if (Contains(entry)) return false; Append(entry); return true; }
		bool   InsertUnique(const T& entry, uint32 atPosition) { if (Contains(entry)) return false; Insert(entry, atPosition); return true; }
		bool   PrependUnique(const T& entry)                   { return InsertUnique(entry, 0); }

		void   EraseAt(uint32 position);
		bool   EraseFirst(const T& entry, uint32 startingFrom = 0); // returns true if found and erased
		uint32 EraseAll(const T& entry, uint32 startingFrom = 0);   // returns count of elements erased
		// EraseFast functions erase element faster
		// but don't preserve order of remaining elements
		// It moves last element to the position of element to be deleted
		// then decrement array size
		void   EraseFastAt(uint32 position);
		bool   EraseFastFirst(const T& entry, uint32 startingFrom = 0); // returns true if found and erased
		uint32 EraseFastAll(const T& entry, uint32 startingFrom = 0);   // returns count of elements erased
		void   PopFront()     { EraseAt(0); }
		void   PopFrontFast() { EraseFastAt(0); }
		void   PopBack()      { EraseAt(Size() - 1); }

		// these functions work faster than regular versions
		// but can only be called on sorted lists
		template <typename CMP> bool   IsSorted(CMP comparator) const;
		template <typename CMP> bool   ContainsSorted(const T& entry, CMP comparator) const;
		template <typename CMP> uint32 CountSorted(const T& entry, CMP comparator) const;
		template <typename CMP> int    FirstIndexOfSorted(const T& entry, CMP comparator) const;
		template <typename CMP> void   InsertSorted(const T& entry, CMP comparator);
		template <typename CMP> bool   InsertUniqueSorted(const T& entry, CMP comparator);
		template <typename CMP> bool   EraseFirstSorted(const T& entry, CMP comparator);
		template <typename CMP> uint32 EraseAllSorted(const T& entry, CMP comparator);

		// these functions work faster than regular versions
		// but can only be called on sorted lists
		bool   IsSorted() const                         { return IsSorted(std::less<T>()); }
		bool   ContainsSorted(const T& entry) const			{ return ContainsSorted(entry, std::less<T>()); }
		uint32 CountSorted(const T& entry) const        { return CountSorted(entry, std::less<T>()); }
		int    FirstIndexOfSorted(const T& entry) const { return FirstIndexOfSorted(entry, std::less<T>()); }
		void   InsertSorted(const T& entry)             { return InsertSorted(entry, std::less<T>()); }
		bool   InsertUniqueSorted(const T& entry)       { return InsertUniqueSorted(entry, std::less<T>()); }
		bool   EraseFirstSorted(const T& entry)         { return EraseFirstSorted(entry, std::less<T>()); }
		uint32 EraseAllSorted(const T& entry)           { return EraseAllSorted(entry, std::less<T>()); }

		template <typename CMP>
		void StableSort(CMP comparator)																	 { RKStableSort(m_Array, m_Size, comparator); }
		void StableSort()																								 { RKStableSort(m_Array, m_Size); }
		template <typename CMP>
		void StableSort(CMP comparator, T* tmpBuffer, int tmpBufferSize) { RKStableSort(m_Array, m_Size, comparator, tmpBuffer, tmpBufferSize); }
		void StableSort(T* tmpBuffer, int tmpBufferSize)								 { RKStableSort(m_Array, m_Size, tmpBuffer, tmpBufferSize); }
		template <typename CMP>
		void Sort(CMP comparator)																				 { std::sort(m_Array, m_Array + m_Size, comparator); }
		void Sort()																										   { std::sort(m_Array, m_Array + m_Size); }


		bool IsUnique() const;   // returns true if all elements in list are unique
		void MakeUnique();       // removes duplicated elements
		void MakeUniqueFast();   // same as MakeUnique, but uses EraseFast, as a result shuffles the array
		void Reverse()            { std::reverse(m_Array, m_Array + m_Size); }

		RKList& operator =(const RKList& other)       { AssignArray(other.ArrayPointer(), other.Size()); return *this; }
		bool    operator==(const RKList& other) const { return EqualToArray(other.ArrayPointer(), other.Size()); }
		bool    operator!=(const RKList& other) const { return !operator==(other); }

		template <uint32 SIZE>
		RKList& operator =(const RKArray<T, SIZE>& rkarray)       { AssignArray(rkarray.ArrayPointer(), rkarray.Size()); return *this; }
		template <uint32 SIZE>
		bool    operator==(const RKArray<T, SIZE>& rkarray) const { return EqualToArray(rkarray.ArrayPointer(), rkarray.Size()); }
		template <uint32 SIZE>
		bool    operator!=(const RKArray<T, SIZE>& rkarray) const { return !operator==(rkarray); }

	private:
		enum Mode
		{
			MODE_DYNAMIC = 0,
			MODE_FIXED_STORAGE,
			MODE_NON_SHRINKING,
		};

		void _Reserve(uint32 nCapacity);
		// static helper
		static inline bool _IsShrinkRequired(uint32 size, uint32 capacity, Mode mode);

		// no function partial template specialization, so using overloading instead
		// these empty structs serve as a flag for _Reallocate (and related) function
		struct PreserveContentFlag {};         // copies content into new array, destroys elements of old array
		struct DefaultConstructContentFlag {}; // default costructs new array, destroys elements of old array
		struct LeaveRawContentFlag {};         // just destroys elements of old array
		// allcoations-related functions
		inline void _MoveArray(T* dst, uint32 dstCapacity, T* src, uint32 count, PreserveContentFlag);
		inline void _MoveArray(T* dst, uint32 dstCapacity, T* src, uint32 count, DefaultConstructContentFlag);
		inline void _MoveArray(T* dst, uint32 dstCapacity, T* src, uint32 count, LeaveRawContentFlag);
		inline void _DummyMoveArray(T* dst, uint32 dstCapacity, uint32 count, PreserveContentFlag) {} // doing nothing at all
		inline void _DummyMoveArray(T* dst, uint32 dstCapacity, uint32 count, DefaultConstructContentFlag);
		inline void _DummyMoveArray(T* dst, uint32 dstCapacity, uint32 count, LeaveRawContentFlag);
		template <typename CONTENTFLAG> inline void _Reallocate(uint32 newCapacity);
		template <typename CONTENTFLAG> inline void _ExpandListToFit(uint32 elementsCount);
		template <typename CONTENTFLAG> inline void _ShrinkIfRequired();

		inline void _Assign(T& dst, const T& src);
		T*   _GetLocalStorage() { RKASSERT(LOCALSTORAGESIZE > 0, "Unable to get local storage for class without local storage"); return (T*)(m_RawMemory + sizeof(void*)); }

		Mode   m_Mode;
		union
		{
			T*     m_Array;
			char   m_RawMemory[sizeof(T*) + LOCALSTORAGESIZE * sizeof(T)];
		};
	};

	template <typename T, uint32 LSS>
	RKList<T, LSS>::RKList()
		: m_Array(NULL)
		, m_Mode(MODE_DYNAMIC)
	{
	}

	template <typename T, uint32 LSS>
	RKList<T, LSS>::RKList(uint32 nCapacity, bool bResizable)
		: m_Array(NULL)
		, m_Mode(MODE_DYNAMIC)
	{
		_Reserve(nCapacity);
		m_Mode = bResizable ? MODE_DYNAMIC : MODE_FIXED_STORAGE;
	}

	template <typename T, uint32 LSS>
	RKList<T, LSS>::RKList(const T* array, uint32 arraySize, bool bResizable)
		: m_Array(NULL)
		, m_Mode(MODE_DYNAMIC)
	{
		AssignArray(array, arraySize);
		m_Mode = bResizable ? MODE_DYNAMIC : MODE_FIXED_STORAGE;
	}

	template <typename T, uint32 LSS>
	RKList<T, LSS>::RKList(const RKList& other)
		: m_Array(NULL)
		, m_Mode(MODE_DYNAMIC)
	{
		AssignArray(other.ArrayPointer(), other.Size());
		m_Mode = other.m_Mode;
	}

	template <typename T, uint32 LSS> template<uint32 SIZE>
	RKList<T, LSS>::RKList(const RKArray<T, SIZE>& rkarray)
		: m_Array(NULL)
		, m_Mode(MODE_DYNAMIC)
	{
		AssignArray(rkarray.ArrayPointer(), rkarray.Size());
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::_Assign(T& dst, const T& src)
	{
		// it is possible to automatically detect if class can be assigned or not
		// using SFINAE, but resulting code is quite comples so I discarded it
		dst = src;
		// if class is not assignable, we can use dtor + copyctor
		/*
		dst.~T();
		new (&dst) T(src);
		*/
	}

	template <typename T, uint32 LSS>  // preserving content
	void RKList<T, LSS>::_MoveArray(T* dst, uint32 dstCapacity, T* src, uint32 count, PreserveContentFlag)
	{
		RKASSERT(src != dst, "Can't move array into itself");
		RKASSERT(dstCapacity >= count, "Internal error, list truncation detected");
		for (uint32 i = 0; i < count; ++i)
		{
			new (dst + i) T(src[i]); // calling copy constructor for new object
			src[i].~T();                  // calling destructor for old object
		}
	}

	template <typename T, uint32 LSS> // not preserving content
	void RKList<T, LSS>::_MoveArray(T* dst, uint32 dstCapacity, T* src, uint32 count, DefaultConstructContentFlag)
	{
		RKASSERT(src != dst, "Can't move array into itself");
		RKASSERT(dstCapacity >= count, "Internal error, list truncation detected");
		for (uint32 i = 0; i < count; ++i)
		{
			new (dst + i) T; // calling default constructor for new object
			src[i].~T();     // calling destructor for old object
		}
	}

	template <typename T, uint32 LSS> // not preserving content
	void RKList<T, LSS>::_MoveArray(T* dst, uint32 dstCapacity, T* src, uint32 count, LeaveRawContentFlag)
	{
		RKASSERT(src != dst, "Can't move array into itself");
		for (uint32 i = 0; i < count; ++i)
		{
			src[i].~T();     // calling destructor for old object
		}
	}

	template <typename T, uint32 LSS> // not preserving content
	void RKList<T, LSS>::_DummyMoveArray(T* dst, uint32 dstCapacity, uint32 count, DefaultConstructContentFlag)
	{
		RKASSERT(dstCapacity >= count, "Internal error, list truncation detected");
		for (uint32 i = 0; i < count; ++i)
		{
			dst[i].~T();     // destructing and reconstructing object inplace
			new (dst + i) T;
		}
	}

	template <typename T, uint32 LSS> // not preserving content
	void RKList<T, LSS>::_DummyMoveArray(T* dst, uint32 dstCapacity, uint32 count, LeaveRawContentFlag)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			dst[i].~T();     // destructing object inplace
		}
	}

	template <typename T, uint32 LSS> template <typename CONTENTFLAG>
	void RKList<T, LSS>::_Reallocate(uint32 newCapacity)
	{
		RKASSERT(newCapacity != m_Capacity || newCapacity <= LSS, "Internal error, unncesessary reallocation");

		// both old and new array fit into local storage, no reallocation required
		if (LSS > 0 && m_Capacity <= LSS && newCapacity <= LSS)
		{
			_DummyMoveArray(m_Array, newCapacity, m_Size, CONTENTFLAG());
			m_Array = (newCapacity > 0) ? _GetLocalStorage() : NULL;
			m_Capacity = newCapacity;
			return;
		}

		T* newArray;
		if (newCapacity > 0)
		{
			if (LSS > 0 && newCapacity <= LSS)
			{
				// using local storage if possible
				newArray = _GetLocalStorage();
			}
			else
			{
				// alloc here because we don't want constructors called
				// they will be called on actual initialization of element
				newArray = (T*)RKHeap_Alloc(sizeof(T) * newCapacity, "RKList");
				RKASSERT(newArray != NULL, "Could not allocate memory for RKList");
			}
		}
		else
		{
			newArray = NULL;
		}

		_MoveArray(newArray, newCapacity, m_Array, m_Size, CONTENTFLAG());
		if (LSS == 0 || m_Capacity > LSS)
		{
			RKHeap_Free(m_Array, "RKList");
		}

		m_Array = newArray;
		m_Capacity = newCapacity;

		if (LSS > 0)
		{
			RKASSERT(((m_Capacity > 0) && (m_Capacity <= LSS)) == (m_Array == _GetLocalStorage()), "Array can fit into local storage, but still using dynamic memory");
		}
		RKASSERT((m_Capacity == 0) == (m_Array == NULL), "Capacity is zero, but array is not null for some reason");
	}

	template <typename T, uint32 LSS> template <typename CONTENTFLAG>
	void RKList<T, LSS>::_ExpandListToFit(uint32 elementsCount)
	{
		if (elementsCount > m_Capacity)
		{
			RKASSERT(m_Mode != MODE_FIXED_STORAGE, "Attempt to add additional entry to full, non-resizable RKList");

			uint32 newCapacity;
			if (LSS > 0 && elementsCount <= LSS)
			{
				newCapacity = LSS;
			}
			else
			{
				newCapacity = RKMAX(m_Capacity * 2, 1);
				while (elementsCount > newCapacity)
				{
					newCapacity *= 2;
				}
			}
			_Reallocate<CONTENTFLAG>(newCapacity);
		}
	}

	/* static */
	template <typename T, uint32 LSS>
	bool RKList<T, LSS>::_IsShrinkRequired(uint32 size, uint32 capacity, Mode mode)
	{
		return (mode == MODE_DYNAMIC) && (capacity != 0) && (size <= (capacity / 4));
	}

	template <typename T, uint32 LSS> template <typename CONTENTFLAG>
	void RKList<T, LSS>::_ShrinkIfRequired()
	{
		if (_IsShrinkRequired(m_Size, m_Capacity, m_Mode))
		{
			uint32 newCapacity = m_Capacity / 2;
			while (m_Size <= (newCapacity / 4) && newCapacity != 0)
			{
				newCapacity = newCapacity / 2;
			}

			if (LSS > 0 && newCapacity > 0 && newCapacity <= LSS)
			{
				newCapacity = LSS;
			}

			_Reallocate<CONTENTFLAG>(newCapacity);
		}
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::_Reserve(uint32 nCapacity)
	{
		if (nCapacity > m_Capacity)
		{
			//  redundant ASSERT if we call ReserveFixed, ReserveFixed twice in a row
			RKASSERT(m_Mode != MODE_FIXED_STORAGE, "Unable to reallocate non-resizable array");

			_Reallocate<PreserveContentFlag>(nCapacity);
		}
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::Clear()
	{
		for (uint32 i = 0; i < m_Size; ++i)
		{
			m_Array[i].~T();
		}
		m_Size = 0;
		_ShrinkIfRequired<PreserveContentFlag>();
	}

	template <typename T, uint32 LSS>
	inline const T& RKList<T, LSS>::operator[](uint32 index) const
	{
		RKASSERT(index < m_Size, "Attempt to Get element from beyond end of RKList");
		return m_Array[index];
	}

	template <typename T, uint32 LSS>
	inline T& RKList<T, LSS>::operator[](uint32 index)
	{
		RKASSERT(index < m_Size, "Attempt to Get element from beyond end of RKList");
		return m_Array[index];
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::FillWith(const T& value, int size)
	{
		if (size >= 0)
		{
			Resize(size, false);
		}
		for (uint32 i = 0; i < m_Size; ++i)
		{
			_Assign(m_Array[i], value);
		}
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::AssignArray(const T* array, uint32 arraySize)
	{
		RKASSERT(m_Mode != MODE_FIXED_STORAGE || (arraySize <= m_Capacity), "Unable to assing non-resizable array of different size");
		const bool bShouldBeReallocated = (arraySize > m_Capacity) || _IsShrinkRequired(arraySize, m_Capacity, m_Mode);
		if (bShouldBeReallocated)
		{
			_Reallocate<LeaveRawContentFlag>(arraySize);

			// initialize new
			m_Size = arraySize;
			for (uint32 i = 0; i < m_Size; ++i)
			{
				new (m_Array + i) T(array[i]);
			}
		}
		else
		{
			RKASSERT(arraySize <= m_Capacity, "Internal logic error");
			if (arraySize <= m_Size)
			{
				for (uint32 i = 0; i < arraySize; ++i)
				{
					_Assign(m_Array[i], array[i]);
				}
				for (uint32 i = arraySize; i < m_Size; ++i)
				{
					m_Array[i].~T();
				}
			}
			else
			{
				for (uint32 i = 0; i < m_Size; ++i)
				{
					_Assign(m_Array[i], array[i]);
				}
				for (uint32 i = m_Size; i < arraySize; ++i)
				{
					new (m_Array + i) T(array[i]);
				}
			}
			m_Size = arraySize;
		}
	}

	template <typename T, uint32 LSS>
	bool RKList<T, LSS>::EqualToArray(const T* array, uint32 arraySize) const
	{
		if (Size() != arraySize)
		{
			return false;
		}
		for (uint32 i = 0; i < arraySize; ++i)
		{
			if (!(m_Array[i] == array[i]))
			{
				return false;
			}
		}
		return true;
	}

	template <typename T, uint32 LSS>
	inline bool RKList<T, LSS>::Contains(const T& entry) const
	{
		return FirstIndexOf(entry) != -1;
	}

	template <typename T, uint32 LSS>
	uint32 RKList<T, LSS>::Count(const T& entry) const
	{
		uint32 count = 0;
		for (uint32 i = 0; i < m_Size; ++i)
		{
			if (m_Array[i] == entry)
			{
				++count;
			}
		}

		return count;
	}

	template <typename T, uint32 LSS>
	int RKList<T, LSS>::FirstIndexOf(const T& entry, uint32 startingFrom) const
	{
		for (uint32 i = startingFrom; i < m_Size; ++i)
		{
			if (m_Array[i] == entry)
			{
				return i;
			}
		}

		return -1;
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::Resize(uint32 nSize, bool bPreserveContent /* = true */, const T& fillValue)
	{
		if (nSize > m_Size)
		{
			if (bPreserveContent)
			{
				_ExpandListToFit<PreserveContentFlag>(nSize);
			}
			else
			{
				_ExpandListToFit<DefaultConstructContentFlag>(nSize);
			}
			for (uint32 i = m_Size; i < nSize; ++i)
			{
				new (m_Array + i) T(fillValue);
			}
			m_Size = nSize;
		}
		else
		{
			for (uint32 i = nSize; i < m_Size; ++i)
			{
				m_Array[i].~T();
			}
			m_Size = nSize;
			if (bPreserveContent)
			{
				_ShrinkIfRequired<PreserveContentFlag>();
			}
			else
			{
				_ShrinkIfRequired<DefaultConstructContentFlag>();
			}
		}
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::Append(const T& entry)
	{
		_ExpandListToFit<PreserveContentFlag>(m_Size + 1);
		new (m_Array + m_Size) T(entry);
		++m_Size;
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::Insert(const T& entry, uint32 atPosition)
	{
		RKASSERT(atPosition <= m_Size, "Attempt to Insert element into RKList beyond end of list");

		_ExpandListToFit<PreserveContentFlag>(m_Size + 1);

		if (m_Size > 0)
		{
			new (m_Array + m_Size) T(m_Array[m_Size - 1]);
			for (uint32 i = m_Size - 1; i > atPosition; --i)
			{
				_Assign(m_Array[i], m_Array[i - 1]);
			}
			m_Array[atPosition] = entry;
		}
		else
		{
			new (m_Array)T(entry);
		}

		++m_Size;
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::EraseAt(uint32 atPosition)
	{
		RKASSERT(atPosition < m_Size, "Attempt to Erase non-existent element from RKList");

		--m_Size;
		for (uint32 i = atPosition; i < m_Size; ++i)
		{
			_Assign(m_Array[i], m_Array[i + 1]);
		}
		m_Array[m_Size].~T();

		_ShrinkIfRequired<PreserveContentFlag>();
	}

	template <typename T, uint32 LSS>
	bool RKList<T, LSS>::EraseFirst(const T& entry, uint32 startingFrom)
	{
		int idx = FirstIndexOf(entry, startingFrom);
		if (idx != -1)
		{
			EraseAt(idx);
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename T, uint32 LSS>
	uint32 RKList<T, LSS>::EraseAll(const T& entry, uint32 startingFrom)
	{
		// this function is very inefficient, should be optimized if necessary
		uint32 erasedCount = 0;
		int idx = startingFrom;
		while (true)
		{
			idx = FirstIndexOf(entry, idx);
			if (idx != -1)
			{
				EraseAt(idx);
				++erasedCount;
			}
			else
			{
				return erasedCount;
			}
		}
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::EraseFastAt(uint32 atPosition)
	{
		RKASSERT(atPosition < m_Size, "Attempt to Erase non-existent element from RKList");

		--m_Size;
		if (atPosition != m_Size)
		{
			m_Array[atPosition] = m_Array[m_Size];
		}
		m_Array[m_Size].~T();

		_ShrinkIfRequired<PreserveContentFlag>();
	}

	template <typename T, uint32 LSS>
	bool RKList<T, LSS>::EraseFastFirst(const T& entry, uint32 startingFrom)
	{
		int idx = FirstIndexOf(entry, startingFrom);
		if (idx != -1)
		{
			EraseFastAt(idx);
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename T, uint32 LSS>
	uint32 RKList<T, LSS>::EraseFastAll(const T& entry, uint32 startingFrom)
	{
		uint32 erasedCount = 0;
		int idx = startingFrom;
		while (true)
		{
			idx = FirstIndexOf(entry, idx);
			if (idx != -1)
			{
				EraseFastAt(idx);
				++erasedCount;
			}
			else
			{
				return erasedCount;
			}
		}
	}

	template <typename T, uint32 LSS> template <typename CMP>
	bool RKList<T, LSS>::IsSorted(CMP comparator) const
	{
		for (uint32 i = 1; i < m_Size; ++i)
		{
			if (comparator(m_Array[i], m_Array[i - 1]))
			{
				return false;
			}
		}
		return true;
	}

	template <typename T, uint32 LSS> template <typename CMP>
	bool RKList<T, LSS>::ContainsSorted(const T& entry, CMP comparator) const
	{
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		return std::binary_search(m_Array, m_Array + m_Size, entry, comparator);
	}

	template <typename T, uint32 LSS> template <typename CMP>
	uint32 RKList<T, LSS>::CountSorted(const T& entry, CMP comparator) const
	{
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		const T* lbound = std::lower_bound(m_Array, m_Array + m_Size, entry, comparator);
		const T* ubound = std::upper_bound(m_Array, m_Array + m_Size, entry, comparator);
		return ubound - lbound;
	}

	template <typename T, uint32 LSS> template <typename CMP>
	int RKList<T, LSS>::FirstIndexOfSorted(const T& entry, CMP comparator) const
	{
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		const T* lbound = std::lower_bound(m_Array, m_Array + m_Size, entry, comparator);
		uint32 idx = lbound - m_Array;
		if (idx < m_Size && !comparator(entry, *lbound))
		{
			return idx;
		}
		else
		{
			return -1;
		}
	}

	template <typename T, uint32 LSS> template <typename CMP>
	void RKList<T, LSS>::InsertSorted(const T& entry, CMP comparator)
	{
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		T* lbound = std::lower_bound(m_Array, m_Array + m_Size, entry, comparator);
		uint32 idx = lbound - m_Array;
		Insert(entry, idx);
		RKASSERT(IsSorted(comparator), "Internal error or bad comparator, array is not sorted anymore");
	}

	template <typename T, uint32 LSS> template <typename CMP>
	bool RKList<T, LSS>::InsertUniqueSorted(const T& entry, CMP comparator)
	{
		if (!ContainsSorted(entry, comparator))
		{
			InsertSorted(entry, comparator);
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename T, uint32 LSS> template <typename CMP>
	bool RKList<T, LSS>::EraseFirstSorted(const T& entry, CMP comparator)
	{
		int idx = FirstIndexOfSorted(entry, comparator);
		if (idx != -1)
		{
			EraseAt(idx);
			RKASSERT(IsSorted(comparator), "Internal error or bad comparator, array is not sorted anymore");
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename T, uint32 LSS> template <typename CMP>
	uint32 RKList<T, LSS>::EraseAllSorted(const T& entry, CMP comparator)
	{
		// this function is very inefficient, should be optimized if necessary
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		uint32 count = 0;
		const T* lbound = std::lower_bound(m_Array, m_Array + m_Size, entry, comparator);
		uint32 idx = lbound - m_Array;
		while (idx < m_Size && !comparator(entry, m_Array[idx]))
		{
			EraseAt(idx);
			++count;
		}
		RKASSERT(IsSorted(comparator), "Internal error or bad comparator, array is not sorted anymore");
		return count;
	}

	template <typename T, uint32 LSS>
	bool RKList<T, LSS>::IsUnique() const
	{
		for (uint32 i = 0; i < m_Size; ++i)
		{
			if (FirstIndexOf(m_Array[i], i + 1) != -1)
			{
				return false;
			}
		}
		return true;
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::MakeUnique()
	{
		for (uint32 i = 0; i < m_Size; ++i)
		{
			EraseAll(m_Array[i], i + 1);
		}
	}

	template <typename T, uint32 LSS>
	void RKList<T, LSS>::MakeUniqueFast()
	{
		for (uint32 i = 0; i < m_Size; ++i)
		{
			EraseFastAll(m_Array[i], i + 1);
		}
	}

	template <typename T, uint32 SIZE, uint32 LSS>
	inline bool operator==(const RKArray<T, SIZE>& rkarray, const RKList<T, LSS>& rklist) { return rklist == rkarray; }
	template <typename T, uint32 SIZE, uint32 LSS>
	inline bool operator!=(const RKArray<T, SIZE>& rkarray, const RKList<T, LSS>& rklist) { return rklist != rkarray; }
}
///----------------------------------------------------------------------
#endif // _RK_LISTS_H_
