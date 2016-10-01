///----------------------------------------------------------------------
/// RKEngine	(c) 2006 + Daniel Stephens
///
/// File		:	RKHashTable.h
///
/// Description :	Templated hashtable class
///----------------------------------------------------------------------
#ifndef _RK_HASHTABLE_H_
#define _RK_HASHTABLE_H_

#include "RK.h"
#include "../RKString_Code/RKString.h"
#include "../RKTool_Code/RKList.h"
#include "../Common/RKTypes.h"
//
// TODO: reduce level of indirection in Iterator (not important for now)
// NOTE: for historical reasons currently there is no way to modify table value
// NOTE: for historical reasons Insert has unusual arguments order
// MAYBE: replace Iterator::IsValid with Iterator::operator bool?
// MAYBE: remove inefficient *Value functions, keep only efficient *Key functions and Find?
// MAYBE: automatic/semi-automatic rebucketing?
//
namespace RKUtils
{
	template <class T>
	class RKHashTable
	{
	private:
		struct Entry
		{
			const char* key;
			uint32 keyHash;
			T value;

			Entry() : key(NULL) {}
			Entry(const Entry& other) : key(NULL) { *this = other; }
			~Entry() { RKDELETEARRAY(key); }

			void Init(const char* initKey, uint32 initKeyHash, const T& initValue);
			Entry& operator=(const Entry& other);
		};
		typedef RKList<Entry> Bucket;
		typedef RKList<Bucket> BucketList;

	public:
		class Iterator;
		class ConstIterator;

		RKHashTable() : _itemCount(0) {}
		RKHashTable(uint32 size) : _itemCount(0) { Init(size); }
		~RKHashTable() { RKASSERT(_SanityCheck(), "Sanity check failed"); }

		// LEGACY initialization (use constructor/destructor instead)
		void Init(uint32 size);
		void Deinit(); // Deinit call is not necessary anymore, everything will be cleaned up automatically

		// general queries
		uint32 BucketCount() const { return _buckets.Size(); }
		uint32 Size() const        { return _itemCount; }
		bool   IsEmpty() const     { return _itemCount == 0; }
		void   Clear();

		// element queries
		bool     ContainsKey(const char* pKey) const { return FindKey(pKey).IsValid(); }
		bool     ContainsValue(const T& value) const { return FindValue(value).IsValid(); }
		uint32   CountOfValues(const T& value) const;
		const T& Query(const char* pKey, const T& defaultValue = T(0)) const; // returns 'defaultValue' if key is not in the map

		//  Accessor, operator[]
		const T& operator[](const char* pKey) const; // asserts if key is not in the map
		T& operator[](const char* pKey)           { return const_cast<T&>(((const RKHashTable<T>*)this)->operator [](pKey)); }
		const T& operator[](const RKString& key) const  { return operator[](key.GetString()); }
		T& operator[](const RKString& key)        { return operator[](key.GetString()); }

		// modifications	
		Iterator Insert(const T& item, const char* pKey); // NOTE: for historical reasons Insert has unusual arguments order
		Iterator Erase(const Iterator& iterator); // returns next valid iterator, argument iterator becomes invalid

		// iteration/search
		ConstIterator Begin() const { return ConstIterator(&_buckets, 0, 0); }
		Iterator Begin()   { return Iterator(&_buckets, 0, 0); }

		// instead of comparison with End() iterator use Iterator::IsValid()
		ConstIterator FindKey(const char* pKey) const   { return (const_cast<RKHashTable<T>*>(this))->FindKey(pKey); }
		Iterator FindKey(const char* pKey);

		ConstIterator FindValue(const T& value) const   { return (const_cast<RKHashTable<T>*>(this))->FindValue(value); }
		Iterator FindValue(const T& value);

		const char* KeyByValue(const T& value) const; // KeyByValue returns NULL if value is not in the map

		bool   RemoveKey(const char* pKey);
		uint32 RemoveAllWithValue(const T& value); // returns count of removed values

		class ConstIterator
		{
			friend class RKHashTable;

		public:
			bool IsValid() const { return _bucketIndex < _buckets->Size(); }

			const T& Value() const  { RKASSERT(IsValid(), "Unable to dereference invalid iterator"); return (*_buckets)[_bucketIndex][_entryIndex].value; }
			const char* Key() const { RKASSERT(IsValid(), "Unable to dereference invalid iterator"); return (*_buckets)[_bucketIndex][_entryIndex].key; }

			ConstIterator& operator++();

		private:
			ConstIterator(const BucketList* buckets, int bucketIndex, int entryIndex);
			void _SkipToFirstExisting();

			const BucketList* _buckets;
			uint32 _bucketIndex;
			uint32 _entryIndex;
		};

		class Iterator : public ConstIterator
		{
		public:
			T& Value()  { return const_cast<T&> (ConstIterator::Value()); }
			Iterator& operator++() { return static_cast<Iterator&>(ConstIterator::operator ++()); }

		private:
			Iterator(const BucketList* buckets, int bucketIndex, int entryIndex) : ConstIterator(buckets, bucketIndex, entryIndex)
			{}

			friend class RKHashTable;
		};


	private:
		RKHashTable(const RKHashTable& other);// : _itemCount(0) { RKASSERT(false, "Forbidden, see comment in RKHashTable<T>::Entry::operator="); }
		RKHashTable& operator=(const RKHashTable& other);//      { RKASSERT(false, "Forbidden, see comment in RKHashTable<T>::Entry::operator="); return *this; }
		bool _SanityCheck() const;

		BucketList _buckets;
		uint32		 _itemCount;
	};

	template <typename T>
	void RKHashTable<T>::Entry::Init(const char* initKey, uint32 initKeyHash, const T& initValue)
	{
		RKASSERT(key == NULL, "Entry is already inited");
		RKASSERT(initKey != NULL, "Unable to init entry with empty key")

			uint32 keyLength = RKString_Length(initKey);
		char* keyStorage = RKNEWARRAY(char, keyLength + 1);
		memcpy(keyStorage, initKey, keyLength + 1);
		key = keyStorage;

		keyHash = initKeyHash;
		value = initValue;
	}

	template <typename T>
	bool RKHashTable<T>::_SanityCheck() const
	{
		uint32 itemCount = 0;
		for (uint32 bucketIdx = 0; bucketIdx < _buckets.Size(); ++bucketIdx)
		{
			itemCount += _buckets[bucketIdx].Size();
			for (uint32 entryIdx = 0; entryIdx < _buckets[bucketIdx].Size(); ++entryIdx)
			{
				RKASSERT(_buckets[bucketIdx][entryIdx].key, "Entry key is invalid, see comment in RKHashTable<T>::Entry::operator=");
			}
		}
		return itemCount == _itemCount;
	}

	template <typename T>
	typename RKHashTable<T>::Entry& RKHashTable<T>::Entry::operator=(const typename RKHashTable<T>::Entry& other)
	{
		// this is a dirty hack that helps to avoid copying keys on reallocation of the array
		// Entry behavior here is similar to auto_ptr behavior
		// Here we just reassign string pointer instead of copying the string
		// and hack source entry to remove string from it (to avoid string destruction on entry dtor)
		// It's bad, but it works _until_ we decide to copy/assign RKHashTable
		//
		// in case of problems (or if you really need copy ctor/assignment operator) 
		// just use old, nice and safe implementation:
		//
		//   delete[] key;
		//   key = NULL;
		//   Init(other.key, other.keyHash, other.value);
		//   return *this;

		RKDELETEARRAY(key);

		if (other.key)
		{
			key = other.key;
			keyHash = other.keyHash;
			value = other.value;
			const_cast<Entry&>(other).key = NULL;
		}
		else
		{
			key = NULL;
		}

		return *this;
	}

	template <typename T>
	void RKHashTable<T>::Init(uint32 size)
	{
		RKASSERT(_buckets.IsEmpty(), "HashTable is already initialized");
		Clear();
		_buckets.Clear();
		_buckets.Resize(size);
	}

	template <typename T>
	void RKHashTable<T>::Deinit()
	{
		Clear();
		_buckets.Clear();
	}

	template <typename T>
	void RKHashTable<T>::Clear()
	{
		for (uint32 i = 0; i < _buckets.Size(); ++i)
		{
			_buckets[i].Clear();
		}
		_itemCount = 0;
	}

	template <typename T>
	uint32 RKHashTable<T>::CountOfValues(const T& value) const
	{
		uint32 count = 0;
		for (uint32 bucketIdx = 0; bucketIdx < _buckets.Size(); ++bucketIdx)
		{
			for (uint32 entryIdx = 0; entryIdx < _buckets[bucketIdx].Size(); ++entryIdx)
			{
				if (_buckets[bucketIdx][entryIdx].value == value)
				{
					++count;
				}
			}
		}
		return count;
	}

	template <typename T>
	const T& RKHashTable<T>::Query(const char* pKey, const T& defaultValue) const
	{
		RKASSERT(!_buckets.IsEmpty(), "Hash table is not initialized");
		RKASSERT(pKey, "Unable to query NULL key");

		// Hash this key
		uint32 hash = RKString_CreateHash(pKey);
		uint32 bucketIndex = hash % _buckets.Size();

		const Bucket& bucket = _buckets[bucketIndex];
		for (uint32 i = 0; i < bucket.Size(); ++i)
		{
			const Entry& entry = bucket[i];
			if (entry.keyHash == hash && RKString_Compare(entry.key, pKey) == 0)
			{
				return entry.value;
			}
		}

		return defaultValue;
	}

	template <typename T>
	const T& RKHashTable<T>::operator[](const char* pKey) const
	{
		RKASSERT(!_buckets.IsEmpty(), "Hash table is not inited");
		RKASSERT(pKey, "Unable to query NULL key");

		// Hash this key
		uint32 hash = RKString_CreateHash(pKey);
		uint32 bucketIndex = hash % _buckets.Size();

		const Bucket& bucket = _buckets[bucketIndex];
		for (uint32 i = 0; i < bucket.Size(); ++i)
		{
			const Entry& entry = bucket[i];
			if (entry.keyHash == hash && RKString_Compare(entry.key, pKey) == 0)
			{
				return entry.value;
			}
		}

		RKASSERT(false, "Unable to find required element in a HashTable");
		return *((T*)0);
	}


	template <typename T>
	typename RKHashTable<T>::Iterator RKHashTable<T>::Insert(const T& item, const char* pKey)
	{
		RKASSERT(_SanityCheck(), "Sanity check failed");
		RKASSERT(!_buckets.IsEmpty(), "Hash table is not initialized");
		RKASSERT(pKey, "Unable to query NULL key");

		// Hash this key
		uint32 hash = RKString_CreateHash(pKey);
		uint32 bucketIndex = hash % _buckets.Size();
		Bucket& bucket = _buckets[bucketIndex];

		for (uint32 i = 0; i < bucket.Size(); ++i)
		{
			if (bucket[i].keyHash == hash && RKString_Compare(bucket[i].key, pKey) == 0)
			{
				RKASSERT(false, "Error key already used in the hash table.");
				return Iterator(&_buckets, bucketIndex, i);
			}
		}

		bucket.Resize(bucket.Size() + 1);
		Entry& entry = bucket[bucket.Size() - 1];
		entry.Init(pKey, hash, item);

		++_itemCount;
		RKASSERT(_SanityCheck(), "Sanity check failed");

		return Iterator(&_buckets, bucketIndex, bucket.Size() - 1);
	}

	template <typename T>
	typename RKHashTable<T>::Iterator RKHashTable<T>::Erase(const typename RKHashTable<T>::Iterator& iterator)
	{
		RKASSERT(_SanityCheck(), "Sanity check failed");
		RKASSERT(!_buckets.IsEmpty(), "Hash table is not initialized");
		RKASSERT(iterator.IsValid(), "Unable to erase invalid iterator");
		RKASSERT(iterator._buckets == &_buckets, "Iterator from other collection detected");

		_buckets[iterator._bucketIndex].EraseFastAt(iterator._entryIndex);
		--_itemCount;

		RKASSERT(_SanityCheck(), "Sanity check failed");
		Iterator nextIterator = iterator;
		nextIterator._SkipToFirstExisting();
		return nextIterator;
	}

	template <typename T>
	typename RKHashTable<T>::Iterator RKHashTable<T>::FindKey(const char* pKey)
	{
		RKASSERT(!_buckets.IsEmpty(), "Hash table is not initialized");
		RKASSERT(pKey, "Unable to query NULL key");

		// Hash this key
		uint32 hash = RKString_CreateHash(pKey);
		uint32 bucketIndex = hash % _buckets.Size();

		const Bucket& bucket = _buckets[bucketIndex];
		for (uint32 i = 0; i < bucket.Size(); ++i)
		{
			const Entry& entry = bucket[i];
			if (entry.keyHash == hash && RKString_Compare(entry.key, pKey) == 0)
			{
				return Iterator(&_buckets, bucketIndex, i);
			}
		}

		return Iterator(&_buckets, _buckets.Size(), 0);
	}

	template <typename T>
	typename RKHashTable<T>::Iterator RKHashTable<T>::FindValue(const T& value)
	{
		RKASSERT(!_buckets.IsEmpty(), "Hash table is not initialized");

		for (uint32 bucketIdx = 0; bucketIdx < _buckets.Size(); ++bucketIdx)
		{
			const Bucket& bucket = _buckets[bucketIdx];
			const uint32 bucketSize = bucket.Size();
			for (uint32 entryIdx = 0; entryIdx < bucketSize; ++entryIdx)
			{
				if (bucket[entryIdx].value == value)
				{
					return Iterator(&_buckets, bucketIdx, entryIdx);
				}
			}
		}
		return Iterator(&_buckets, _buckets.Size(), 0);
	}

	template <typename T>
	const char* RKHashTable<T>::KeyByValue(const T& value) const
	{
		ConstIterator it = FindValue(value);
		return it.IsValid() ? it.Key() : NULL;
	}

	template <typename T>
	bool RKHashTable<T>::RemoveKey(const char* pKey)
	{
		Iterator it = FindKey(pKey);
		if (it.IsValid())
		{
			Erase(it);
			return true;
		}
		return false;
	}

	template <typename T>
	uint32 RKHashTable<T>::RemoveAllWithValue(const T& value)
	{
		uint32 erasedCount = 0;
		Iterator it = Begin();
		while (it.IsValid())
		{
			if (it.Value() == value)
			{
				it = Erase(it);
				++erasedCount;
			}
			else
			{
				++it;
			}
		}
		return erasedCount;
	}

	template <typename T>
	RKHashTable<T>::ConstIterator::ConstIterator(const typename RKHashTable<T>::BucketList* buckets, int bucketIndex, int entryIndex)
		: _buckets(buckets)
		, _bucketIndex(bucketIndex)
		, _entryIndex(entryIndex)
	{
		_SkipToFirstExisting();
	}

	template <typename T>
	void RKHashTable<T>::ConstIterator::_SkipToFirstExisting()
	{
		while (_bucketIndex < _buckets->Size() && _entryIndex == ((*_buckets)[_bucketIndex]).Size())
		{
			++_bucketIndex;
			_entryIndex = 0;
		}
	}

	template <typename T>
	typename RKHashTable<T>::ConstIterator& RKHashTable<T>::ConstIterator::operator ++ ()
	{
		RKASSERT(IsValid(), "Unable to increment invalid iterator");
		++_entryIndex;
		_SkipToFirstExisting();
		return *this;
	}
}

///----------------------------------------------------------------------
#endif // _RK_HASHTABLE_H_
