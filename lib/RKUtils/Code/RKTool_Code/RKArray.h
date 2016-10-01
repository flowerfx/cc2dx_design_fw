#ifndef _RK_ARRAY_H_
#define _RK_ARRAY_H_

#include <algorithm>

#include "../Common/RK.h"
#include "RKSort.h"
namespace RKUtils
{
	// COLLECTION can be RKList or RKArray, SUBCOMP can be used to specify comparison object for collection elements
	template <typename COLLECTION, typename SUBCOMP = std::less<typename COLLECTION::ElementType> >
	class RKCollectionComparisonObject
	{
	public:
		RKCollectionComparisonObject() {}
		RKCollectionComparisonObject(const SUBCOMP& subcomp) : m_subcomp(subcomp) {}

		bool operator()(const COLLECTION& c1, const COLLECTION& c2) const
		{
			if (c1.Size() != c2.Size())
			{
				return c1.Size() < c2.Size();
			}
			for (uint32 i = 0; i < c1.Size(); ++i)
			{
				if (m_subcomp(c1[i], c2[i]))
				{
					return true;
				}
				if (m_subcomp(c2[i], c1[i]))
				{
					return false;
				}
			}
			return false;
		}
	private:
		SUBCOMP m_subcomp;
	};

	// defines an array with values: 
	// RKARRAY_DEF(int, arrayName, 1, 2, 3, 4, 5);
	// roughly equals to
	// RKArray<int, 5> listName = {1, 2, 3, 4, 5};
#define RKARRAY_DEF(T, name, ...) \
	T __initialArray_##name[] = {__VA_ARGS__};\
	RKArray<T, ARRAYSIZE(__initialArray_##name) > name(__initialArray_##name, ARRAYSIZE(__initialArray_##name));

	// if you need to put RKArray into a map/tree, see RKCollectionComparisonObject
	template <typename T, uint32 SIZE>
	class RKArray
	{
		RKSTATICASSERT(SIZE > 0, "Unable to instantiate zero-sized static array");
	public:
		typedef T ElementType;

		explicit RKArray(const T& initialValue = T())      { FillWith(initialValue); }
		explicit RKArray(const T* array, uint32 arraySize) { AssignArray(array, arraySize); }
		RKArray(const RKArray& other)                      { AssignArray(other.ArrayPointer(), other.Size()); }

		uint32 Size() const { return SIZE; }

		const T& operator[](uint32 index) const;
		T&			 operator[](uint32 index);
		const T& GetAt(uint32 index) const           { return operator[](index); } // more readable when you only have a pointer to RKArray, in other cases please use operator[]
		void     SetAt(uint32 index, const T& value) { operator[](index) = value; }
		const T& FirstElement() const { return operator[](0); }
		const T& LastElement()  const { return operator[](SIZE - 1); }
		const T* ArrayPointer() const { return m_Array; }
		T*       ArrayPointer()       { return m_Array; }

		void   FillWith(const T& value);
		void   AssignArray(const T* array, uint32 arraySize);
		bool   EqualToArray(const T* array, uint32 arraySize) const;

		bool   Contains(const T& entry) const;     // returns true if array contains element
		uint32 Count(const T& entry) const;        // returns count of elements like that in array
		int    FirstIndexOf(const T& entry, uint32 startingFrom = 0) const; // returns -1 if not found

		// these functions work faster than regular versions
		// but can only be called on sorted arrays
		template <typename CMP> bool   IsSorted(CMP comparator) const;
		template <typename CMP> bool   ContainsSorted(const T& entry, CMP comparator) const;
		template <typename CMP> uint32 CountSorted(const T& entry, CMP comparator) const;
		template <typename CMP> int    FirstIndexOfSorted(const T& entry, CMP comparator) const;

		// these functions work faster than regular versions
		// but can only be called on sorted arrays
		bool   IsSorted() const                         { return IsSorted(std::less<T>()); }
		bool   ContainsSorted(const T& entry) const			{ return ContainsSorted(entry, std::less<T>()); }
		uint32 CountSorted(const T& entry) const        { return CountSorted(entry, std::less<T>()); }
		int    FirstIndexOfSorted(const T& entry) const { return FirstIndexOfSorted(entry, std::less<T>()); }

		template <typename CMP>
		void StableSort(CMP comparator) { RKStableSort(m_Array, SIZE, comparator); }
		void StableSort()								{ RKStableSort(m_Array, SIZE); }
		template <typename CMP>
		void StableSort(CMP comparator, T* tmpBuffer, int tmpBufferSIZE) { RKStableSort(m_Array, SIZE, comparator, tmpBuffer, tmpBufferSIZE); }
		void StableSort(T* tmpBuffer, int tmpBufferSIZE)								 { RKStableSort(m_Array, SIZE, tmpBuffer, tmpBufferSIZE); }
		template <typename CMP>
		void Sort(CMP comparator) { std::sort(m_Array, m_Array + SIZE, comparator); }
		void Sort()               { std::sort(m_Array, m_Array + SIZE); }

		bool IsUnique() const;   // returns true if all elements in list are unique
		void Reverse()            { std::reverse(m_Array, m_Array + SIZE); }

		RKArray& operator =(const RKArray& other)       { AssignArray(other.ArrayPointer(), other.Size()); return *this; }
		bool     operator==(const RKArray& other) const { return EqualToArray(other.ArrayPointer(), other.Size()); }
		bool     operator!=(const RKArray& other) const { return !operator==(other); }

	private:
		T      m_Array[SIZE];
	};

	template <typename T, uint32 SIZE>
	const T& RKArray<T, SIZE>::operator[](uint32 index) const
	{
		RKASSERT(index < SIZE, "Attempt to Get element from beyond end of RKArray");
		return m_Array[index];
	}

	template <typename T, uint32 SIZE>
	T&			 RKArray<T, SIZE>::operator[](uint32 index)
	{
		RKASSERT(index < SIZE, "Attempt to Get element from beyond end of RKArray");
		return m_Array[index];
	}

	template <typename T, uint32 SIZE>
	void RKArray<T, SIZE>::FillWith(const T& value)
	{
		for (uint32 i = 0; i < SIZE; ++i)
		{
			m_Array[i] = value;
		}
	}

	template <typename T, uint32 SIZE>
	void RKArray<T, SIZE>::AssignArray(const T* array, uint32 arraySize)
	{
		RKASSERT(arraySize == SIZE, "Unable to assign an array with non-matching size");
		for (uint32 i = 0; i < arraySize; ++i)
		{
			m_Array[i] = array[i];
		}
	}

	template <typename T, uint32 SIZE>
	bool RKArray<T, SIZE>::EqualToArray(const T* array, uint32 arraySize) const
	{
		if (SIZE != arraySize)
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

	template <typename T, uint32 SIZE>
	bool RKArray<T, SIZE>::Contains(const T& entry) const
	{
		return FirstIndexOf(entry) != -1;
	}

	template <typename T, uint32 SIZE>
	uint32 RKArray<T, SIZE>::Count(const T& entry) const
	{
		uint32 count = 0;
		for (uint32 i = 0; i < SIZE; ++i)
		{
			if (m_Array[i] == entry)
			{
				++count;
			}
		}

		return count;
	}

	template <typename T, uint32 SIZE>
	int RKArray<T, SIZE>::FirstIndexOf(const T& entry, uint32 startingFrom) const
	{
		for (uint32 i = startingFrom; i < SIZE; ++i)
		{
			if (m_Array[i] == entry)
			{
				return i;
			}
		}

		return -1;
	}

	template <typename T, uint32 SIZE> template <typename CMP>
	bool RKArray<T, SIZE>::IsSorted(CMP comparator) const
	{
		for (uint32 i = 1; i < SIZE; ++i)
		{
			if (comparator(m_Array[i], m_Array[i - 1]))
			{
				return false;
			}
		}
		return true;
	}


	template <typename T, uint32 SIZE> template <typename CMP>
	bool RKArray<T, SIZE>::ContainsSorted(const T& entry, CMP comparator) const
	{
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		return std::binary_search(m_Array, m_Array + SIZE, entry, comparator);
	}

	template <typename T, uint32 SIZE> template <typename CMP>
	uint32 RKArray<T, SIZE>::CountSorted(const T& entry, CMP comparator) const
	{
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		const T* lbound = std::lower_bound(m_Array, m_Array + SIZE, entry, comparator);
		const T* ubound = std::upper_bound(m_Array, m_Array + SIZE, entry, comparator);
		return ubound - lbound;
	}

	template <typename T, uint32 SIZE> template <typename CMP>
	int RKArray<T, SIZE>::FirstIndexOfSorted(const T& entry, CMP comparator) const
	{
		RKASSERT(IsSorted(comparator), "Trying to call 'sorted' functions on non-sorted array");
		const T* lbound = std::lower_bound(m_Array, m_Array + SIZE, entry, comparator);
		uint32 idx = lbound - m_Array;
		if (idx < SIZE && !comparator(entry, *lbound))
		{
			return idx;
		}
		else
		{
			return -1;
		}
	}

	template <typename T, uint32 SIZE>
	bool RKArray<T, SIZE>::IsUnique() const
	{
		for (uint32 i = 0; i < SIZE; ++i)
		{
			if (FirstIndexOf(m_Array[i], i + 1) != -1)
			{
				return false;
			}
		}
		return true;
	}
}

#endif // _RK_ARRAY_H_
