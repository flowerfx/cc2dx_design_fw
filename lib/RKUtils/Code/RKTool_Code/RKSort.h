#pragma once

#include "../Common/RK.h"
#include "../Common/RKMacros.h"

#include <functional>
#include <algorithm>
#include <vector>
namespace RKUtils
{
	//
	// This is binary insertion sort, requires O(n log n) compares and 
	// O(n^2) data movements in worst case.
	//
	// This function assumes that the elements in range [0, start)
	// are already sorted.
	//
	template <typename T, typename CMP>
	void BinaryInsertionSort(T* a, int aLen, CMP c, int start = 0) {
		RKASSERT(0 <= start && start <= aLen, "Wrong arguments");
		if (start == 0) {
			start++;
		}
		for (; start < aLen; start++) {
			int left = 0;
			int right = start;
			RKASSERT(left <= right, "Impossible");

			// binary search to find a proper place for a[start]
			while (left < right) {
				int mid = (left + right) >> 1;
				if (c(a[start], a[mid])) {
					right = mid;
				}
				else {
					left = mid + 1;
				}
			}
			RKASSERT(left == right, "Impossible");

			if (start - left > 0) { // shift elements if necessary, and insert a[start] into the right place
				T tmp = a[start];
				for (int i = start; i > left; --i) {
					a[i] = a[i - 1];
				}
				a[left] = tmp;
			}
		}
	};

	//
	// ReverseArray
	// this function reverses given array inplace
	//
	template <typename T>
	void ReverseArray(T* a, int aLen) {
		int lo = 0;
		int hi = aLen - 1;
		while (lo < hi) {
			std::swap(a[hi], a[lo]);
			++lo;
			--hi;
		}
	}

	//
	// RKStableSorter
	//
	// This is an implementation of timsort sorting
	// it's a really fast stable sort, that works exceptionally good on 
	// partially sorted arrays, and works like merge sort on steroids
	// it require temporary storage at least as big as a half of array being sorted
	//
	// see description here: http://svn.python.org/projects/python/trunk/Objects/listsort.txt
	// code used as reference: http://cr.openjdk.java.net/~martin/webrevs/openjdk7/timsort/src/share/classes/java/util/TimSort.java.html
	//
	template <typename T, typename CMP, int MAX_RUNS = 64, bool debugChecks = false>
	class RKStableSorter {
	public:
		static const int MIN_MERGE = 32;
		static const int MIN_GALLOP = 7;

		static void sort(T* a, int aLen, CMP c, T* tmp, int tmpLen) {
			if (aLen <= 1) {
				return;
			}

			if (aLen <= MIN_MERGE) {
				int initRunLen = _CountRunAndMakeAscending(a, aLen, c);
				BinaryInsertionSort(a, aLen, c, initRunLen);
				return;
			}

			int nRemaining = aLen;
			int start = 0;
			RKStableSorter sorter(a, aLen, c, tmp, tmpLen);
			int minRun = _MinRunLength(aLen);
			do {
				int runLen = _CountRunAndMakeAscending(a + start, nRemaining, c);

				// If run is short, extend to min(minRun, nRemaining)
				if (runLen < minRun) {
					int force = nRemaining <= minRun ? nRemaining : minRun;
					BinaryInsertionSort(a + start, force, c, runLen);
					runLen = force;
				}

				sorter._PushRun(start, runLen);
				sorter._MergeCollapse();

				start += runLen;
				nRemaining -= runLen;
			} while (nRemaining != 0);

			// Merge all remaining runs to complete sort
			RKASSERT(start == aLen, "Impossible");
			sorter._MergeForceCollapse();
			RKASSERT(sorter.stackSize == 1, "Impossible");
		}

	private:
		//
		// MinRunLength
		// this function determines minimum run length
		// for rationale see http://svn.python.org/projects/python/trunk/Objects/listsort.txt
		//
		static int _MinRunLength(int n) {
			RKASSERT(n >= 0, "Impossible");
			int r = 0;      // Becomes 1 if any 1 bits are shifted off
			while (n >= MIN_MERGE) {
				r |= (n & 1);
				n >>= 1;
			}
			return n + r;
		}

		// 
		// _CountRunAndMakeAscending
		// this function finds how many elements of a is already sorted
		// ensures that they are sorted in ascending order and returns
		// length of sorted subarray
		//
		static int _CountRunAndMakeAscending(T* a, int aLen, CMP c) {
			RKASSERT(aLen >= 0, "Impossible");
			int idx = 1;
			if (idx == aLen) {
				return 1;
			}

			if (c(a[idx++], a[0])) { // descending
				while (idx < aLen && c(a[idx], a[idx - 1])) {
					idx++;
				}
				ReverseArray(a, idx);
			}
			else { // ascending
				while (idx < aLen && !c(a[idx], a[idx - 1])) {
					idx++;
				}
			}

			return idx;
		}

		//
		// GallopLeft
		// searches for a place to insert element key into sorted array a
		// returns leftmost proper index (in case if there are already elements equal to key)
		// see http://svn.python.org/projects/python/trunk/Objects/listsort.txt for details
		//
		static int _GallopLeft(const T& key, T* a, int aLen, int hint, CMP c) {
			RKASSERT(hint >= 0 && hint < aLen, "Impossible");
			// calculating estimated range in which 'key' belongs using 'hint' as a hint
			int lastOfs = 0;
			int ofs = 1;
			if (c(a[hint], key)) { // galliping right
				int maxOfs = aLen - hint;
				while (ofs < maxOfs && c(a[hint + ofs], key)) {
					lastOfs = ofs;
					ofs = (ofs << 1) + 1;
					if (ofs <= 0) { // int overflow
						ofs = maxOfs;
					}
				}
				if (ofs > maxOfs) {
					ofs = maxOfs;
				}

				lastOfs += hint;
				ofs += hint;
			}
			else { // galliping left
				const int maxOfs = hint + 1;
				while (ofs < maxOfs && !c(a[hint - ofs], key)) {
					lastOfs = ofs;
					ofs = (ofs << 1) + 1;
					if (ofs <= 0) { // int overflow
						ofs = maxOfs;
					}
				}
				if (ofs > maxOfs) {
					ofs = maxOfs;
				}

				int tmp = lastOfs;
				lastOfs = hint - ofs;
				ofs = hint - tmp;
			}
			RKASSERT(-1 <= lastOfs && lastOfs < ofs && ofs <= aLen, "Impossible");

			// range determined, key should be somewhere between lastOfs and ofs
			// using binary search to find the right index
			lastOfs++;
			while (lastOfs < ofs) {
				int m = lastOfs + ((ofs - lastOfs) >> 1);

				if (c(a[m], key)) {
					lastOfs = m + 1;
				}
				else {
					ofs = m;
				}
			}
			RKASSERT(lastOfs == ofs, "Impossible");    // so a[base + ofs - 1] < key <= a[base + ofs]
			return ofs;
		}

		//
		// GallopRight
		// searches for a place to insert element key into sorted array a
		// returns rightmost proper index (in case if there are already elements equal to key)
		// see http://svn.python.org/projects/python/trunk/Objects/listsort.txt for details
		//
		static int _GallopRight(const T& key, T* a, int aLen, int hint, CMP c) {
			// calculating estimated range in which 'key' belongs using 'hint' as a hint
			RKASSERT(aLen > 0 && hint >= 0 && hint < aLen, "Impossible");
			int ofs = 1;
			int lastOfs = 0;
			if (c(key, a[hint])) { // gallop left
				int maxOfs = hint + 1;
				while (ofs < maxOfs && c(key, a[hint - ofs])) {
					lastOfs = ofs;
					ofs = (ofs << 1) + 1;
					if (ofs <= 0) { // int overflow
						ofs = maxOfs;
					}
				}
				if (ofs > maxOfs) {
					ofs = maxOfs;
				}

				int tmp = lastOfs;
				lastOfs = hint - ofs;
				ofs = hint - tmp;
			}
			else { // gallop right
				int maxOfs = aLen - hint;
				while (ofs < maxOfs && !c(key, a[hint + ofs])) {
					lastOfs = ofs;
					ofs = (ofs << 1) + 1;
					if (ofs <= 0) { // int overflow
						ofs = maxOfs;
					}
				}
				if (ofs > maxOfs) {
					ofs = maxOfs;
				}

				lastOfs += hint;
				ofs += hint;
			}
			RKASSERT(-1 <= lastOfs && lastOfs < ofs && ofs <= aLen, "Impossible");

			// range determined, key should be somewhere between lastOfs and ofs
			// using binary search to find the right index
			lastOfs++;
			while (lastOfs < ofs) {
				int m = lastOfs + ((ofs - lastOfs) >> 1);

				if (c(key, a[m])) {
					ofs = m;
				}
				else {
					lastOfs = m + 1;
				}
			}
			RKASSERT(lastOfs == ofs, "Impossible");
			return ofs;
		}

		//
		// _MergeLo
		// merges two adjacent sorted subarrays using temporary buffer
		// should be used if len1 <= len2
		// see http://svn.python.org/projects/python/trunk/Objects/listsort.txt for details
		//
		static void _MergeLo(T* a, int len1, int len2, CMP c, T* tmp, int tmpLen) {
			RKASSERT(len1 > 0 && len2 > 0, "Wrong arguments");

			RKASSERT(len1 <= tmpLen, "Temp buffer is too small");
			std::copy(a, a + len1, tmp); // copying first subarray into the temp buffer

			T* cursor1 = tmp;
			T* cursor2 = a + len1;
			T* dest = a;

			RKASSERT(!c(*cursor1, *cursor2), "Impossible");

			// Move first element of second run and deal with degenerate cases
			*dest++ = *cursor2++;
			if (--len2 == 0) {
				std::copy(cursor1, cursor1 + len1, dest);
				return;
			}
			if (len1 == 1) {
				std::copy(cursor2, cursor2 + len2, dest);
				*(dest + len2) = *cursor1;
				return;
			}

			int minGallop = MIN_GALLOP;
			while (true) {
				int count1 = 0; // Number of times in a row that first run won
				int count2 = 0; // Number of times in a row that second run won

				// straightforward copy
				do {
					RKASSERT(len1 > 1 && len2 > 0, "Impossible");
					if (c(*cursor2, *cursor1)) {
						*dest++ = *cursor2++;
						count2++;
						count1 = 0;
						if (--len2 == 0) {
							goto outer;
						}
					}
					else {
						*dest++ = *cursor1++;
						count1++;
						count2 = 0;
						if (--len1 == 1) {
							goto outer;
						}
					}
				} while ((count1 | count2) < minGallop);

				// galloping copy
				do {
					RKASSERT(len1 > 1 && len2 > 0, "Impossible");
					count1 = _GallopRight(*cursor2, cursor1, len1, 0, c);
					if (count1 != 0) {
						std::copy(cursor1, cursor1 + count1, dest);
						dest += count1;
						cursor1 += count1;
						len1 -= count1;
						if (len1 <= 1) { // len1 == 1 || len1 == 0
							goto outer;
						}
					}
					*dest++ = *cursor2++;
					if (--len2 == 0) {
						goto outer;
					}

					count2 = _GallopLeft(*cursor1, cursor2, len2, 0, c);
					if (count2 != 0) {
						std::copy(cursor2, cursor2 + count2, dest);
						dest += count2;
						cursor2 += count2;
						len2 -= count2;
						if (len2 == 0) {
							goto outer;
						}
					}
					*dest++ = *cursor1++;
					if (--len1 == 1) {
						goto outer;
					}
					minGallop--;
				} while (count1 >= MIN_GALLOP || count2 >= MIN_GALLOP);
				if (minGallop < 0) {
					minGallop = 0;
				}
				minGallop += 2;  // Penalize for leaving gallop mode
			}
		outer:

			if (len1 == 1) {
				RKASSERT(len2 > 0, "Impossible");
				std::copy(cursor2, cursor2 + len2, dest);
				*(dest + len2) = *cursor1;
			}
			else if (len1 == 0) {
				RKASSERT(false, "Comparison method violates its general contract!");
			}
			else {
				RKASSERT(len2 == 0, "Impossible");
				RKASSERT(len1 > 1, "Impossible");
				std::copy(cursor1, cursor1 + len1, dest);
			}
		}

		//
		// _MergeHi
		// merges two adjacent sorted subarrays using temporary buffer
		// should be used if len1 >= len2
		// see http://svn.python.org/projects/python/trunk/Objects/listsort.txt for details
		//
		static void _MergeHi(T* a, int len1, int len2, CMP c, T* tmp, int tmpLen) {
			RKASSERT(len1 > 0 && len2 > 0, "Impossible");

			// Copy second run into temp array
			RKASSERT(len2 <= tmpLen, "Temp buffer is too small");
			std::copy(a + len1, a + len1 + len2, tmp);

			T* cursor1 = a + len1 - 1;
			T* cursor2 = tmp + len2 - 1;
			T* dest = a + len1 + len2 - 1;

			RKASSERT(!c(*cursor1, *cursor2), "Impossible");

			// Move last element of first run and deal with degenerate cases
			*dest-- = *cursor1--;
			if (--len1 == 0) {
				std::copy_backward(tmp, tmp + len2, dest - (len2 - 1) + len2);
				return;
			}
			if (len2 == 1) {
				dest -= len1;
				cursor1 -= len1;
				std::copy_backward(cursor1 + 1, cursor1 + 1 + len1, dest + 1 + len1);
				*dest = *cursor2;
				return;
			}

			int minGallop = MIN_GALLOP;
			while (true) {
				int count1 = 0; // Number of times in a row that first run won
				int count2 = 0; // Number of times in a row that second run won

				// straightforward copy
				do {
					RKASSERT(len1 > 0 && len2 > 1, "Impossible");
					if (c(*cursor2, *cursor1)) {
						*dest-- = *cursor1--;
						count1++;
						count2 = 0;
						if (--len1 == 0) {
							goto outer;
						}
					}
					else {
						*dest-- = *cursor2--;
						count2++;
						count1 = 0;
						if (--len2 == 1) {
							goto outer;
						}
					}
				} while ((count1 | count2) < minGallop);

				// galloping copy
				do {
					RKASSERT(len1 > 0 && len2 > 1, "Impossible");
					count1 = len1 - _GallopRight(*cursor2, a, len1, len1 - 1, c);
					if (count1 != 0) {
						dest -= count1;
						cursor1 -= count1;
						len1 -= count1;
						std::copy_backward(cursor1 + 1, cursor1 + count1 + 1, dest + 1 + count1);
						if (len1 == 0) {
							goto outer;
						}
					}
					*dest-- = *cursor2--;
					if (--len2 == 1) {
						goto outer;
					}

					count2 = len2 - _GallopLeft(*cursor1, tmp, len2, len2 - 1, c);
					if (count2 != 0) {
						dest -= count2;
						cursor2 -= count2;
						len2 -= count2;
						std::copy_backward(cursor2 + 1, cursor2 + 1 + count2, dest + 1 + count2);
						if (len2 <= 1) { // len2 == 1 || len2 == 0
							goto outer;
						}
					}
					*dest-- = *cursor1--;
					if (--len1 == 0) {
						goto outer;
					}
					minGallop--;
				} while (count1 >= MIN_GALLOP || count2 >= MIN_GALLOP);
				if (minGallop < 0) {
					minGallop = 0;
				}
				minGallop += 2;  // Penalize for leaving gallop mode
			}
		outer:

			if (len2 == 1) {
				RKASSERT(len1 > 0, "Impossible");
				dest -= len1;
				cursor1 -= len1;
				std::copy_backward(cursor1 + 1, cursor1 + 1 + len1, dest + 1 + len1);
				*dest = *cursor2;
			}
			else if (len2 == 0) {
				RKASSERT(false, "Comparison method violates its general contract!");
			}
			else {
				RKASSERT(len1 == 0, "Impossible");
				RKASSERT(len2 > 0, "Impossible");
				std::copy_backward(tmp, tmp + len2, dest - (len2 - 1) + len2);
			}
		}

		//
		// _Merge
		// merges two adjacent sorted subarrays using temporary buffer
		//
		// it's the most complex part of the whole sort, if you experience
		// strange behavior and blame sort algorithm, then enable
		// checks and try to reproduce
		//
		static void _Merge(T* a, int len1, int len2, CMP c, T* tmp, int tmpLen) {
			_CheckSorted(a, len1, c);
			_CheckSorted(a + len1, len2, c);

			if (len1 > len2) {
				_MergeHi(a, len1, len2, c, tmp, tmpLen);
			}
			else {
				_MergeLo(a, len1, len2, c, tmp, tmpLen);
			}

			_CheckSorted(a, len1 + len2, c);
		}

		//
		// ctor, just store the array and comparator
		// 
		RKStableSorter(T* array, int arrayLen, CMP comparator, T* tmpArray, int tmpArrayLen)
			: a(array)
			, aLen(arrayLen)
			, c(comparator)
			, tmp(tmpArray)
			, tmpLen(tmpArrayLen)
		{
			RKASSERT(tmpArrayLen * 2 >= arrayLen, "Not enough storage in temporary array");
			stackSize = 0;
		}

		static inline void _CheckSorted(T* a, int aLen, CMP c) {
			if (debugChecks) {
				for (int i = 1; i < aLen; ++i) {
					RKASSERT(!c(a[i], a[i - 1]), "Sanity check failed");
					/*
					if (a[i].v == a[i-1].v) {
					RKASSERT(a[i-1].i < a[i].i, "Sanity check failed");
					}
					*/
				}
			}
		}

		//
		// _MergeAt
		// merges two adjacent runs at indices i and i+1
		// can only merge last or second last pair of runs
		//
		void _MergeAt(int i) {
			RKASSERT(stackSize >= 2, "Impossible");
			RKASSERT(i >= 0, "Impossible");
			RKASSERT(i == stackSize - 2 || i == stackSize - 3, "Impossible");

			int base1 = runBase[i];
			int len1 = runLen[i];
			int base2 = runBase[i + 1];
			int len2 = runLen[i + 1];
			RKASSERT(len1 > 0 && len2 > 0, "Impossible");
			RKASSERT(base1 + len1 == base2, "Impossible");

			// fixing runs stack
			runLen[i] = len1 + len2;
			if (i == stackSize - 3) {
				runBase[i + 1] = runBase[i + 2];
				runLen[i + 1] = runLen[i + 2];
			}
			stackSize--;

			// cutting run1 prefix if possible
			int k = _GallopRight(a[base2], a + base1, len1, 0, c);
			RKASSERT(k >= 0, "Impossible");
			base1 += k;
			len1 -= k;
			if (len1 == 0) {
				return;
			}

			// cutting run2 suffix if possible
			len2 = _GallopLeft(a[base1 + len1 - 1], a + base2, len2, len2 - 1, c);
			RKASSERT(len2 >= 0, "Impossible");
			if (len2 == 0) {
				return;
			}

			// Merge remaining runs, using tmp array with min(len1, len2) elements
			_Merge(a + base1, len1, len2, c, tmp, tmpLen);
		}

		//
		// _PushRun
		// adds run to runs stack
		//
		void _PushRun(int runBase, int runLen) {
			this->runBase[stackSize] = runBase;
			this->runLen[stackSize] = runLen;
			++stackSize;
		}

		//
		// _MergeCollapse
		// tries to merge runs preserving invariants:
		// 1. runLen[i - 3] > runLen[i - 2] + runLen[i - 1]
		// 2. runLen[i - 2] > runLen[i - 1]
		//
		void _MergeCollapse() {
			while (stackSize > 1) {
				int n = stackSize - 2;
				if (n > 0 && runLen[n - 1] <= runLen[n] + runLen[n + 1]) {
					if (runLen[n - 1] < runLen[n + 1]) {
						n--;
					}
					_MergeAt(n);
				}
				else if (runLen[n] <= runLen[n + 1]) {
					_MergeAt(n);
				}
				else {
					break;
				}
			}
		}

		//
		// MergeForceCollapse
		// merges all runs, called once to finalize sort
		//
		void _MergeForceCollapse() {
			while (stackSize > 1) {
				int n = stackSize - 2;
				if (n > 0 && runLen[n - 1] < runLen[n + 1]) {
					n--;
				}
				_MergeAt(n);
			}
		}

		T* a;
		int aLen;
		CMP c;
		T* tmp;
		int tmpLen;

		int stackSize;
		int runBase[MAX_RUNS];
		int runLen[MAX_RUNS];
	};

	//
	// StableSort, implementation of TimSort
	// see RKStableSorter for details
	//
	template <typename T, typename CMP>
	void RKStableSort(T* a, int aLen, CMP c, T* tmp, int tmpLen) {
		RKStableSorter<T, CMP>::sort(a, aLen, c, tmp, tmpLen);
	}

	//
	// StableSort, implementation of TimSort
	// see RKStableSorter for details
	//
	template <typename T>
	void RKStableSort(T* a, int aLen, T* tmp, int tmpLen) {
		StableSort(a, aLen, std::less<T>(), tmp, tmpLen);
	}

	//
	// StableSort, implementation of TimSort
	// see RKStableSorter for details
	//
	// this version allocates temporary buffer
	// which might be undesirable
	// Usually it's better to allocate temporary
	// buffer manually
	//
	template <typename T, typename CMP>
	void RKStableSort(T* a, int aLen, CMP c) {
		if (aLen > RKStableSorter<T, CMP>::MIN_MERGE) {
			std::vector<T> tempStorage;
			tempStorage.resize(aLen / 2 + aLen % 2); // = ceil(aLen / 2)
			RKStableSort(a, aLen, c, &tempStorage[0], tempStorage.size());
		}
		else {
			RKStableSort(a, aLen, c, (T*)NULL, 0);
		}
	}

	//
	// StableSort, implementation of TimSort
	// see RKStableSorter for details
	//
	// this version allocates temporary buffer
	// which might be undesirable
	// Usually it's better to allocate temporary
	// buffer manually
	//
	template <typename T>
	void RKStableSort(T* a, int aLen) {
		RKStableSort(a, aLen, std::less<T>());
	}
}