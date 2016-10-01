#ifndef __WMAP_H__
#define __WMAP_H__
#include <map>
#include "RKTool_Code/RKList.h"
using namespace RKUtils;
namespace Utility
{
	template<typename T, typename U>
	class wPair
	{
	public:
		T value_first;
		U value_second;
	public:
		wPair<T, U>() {
			value_first = T();
			value_second = U();
		};
		wPair<T, U>(T f, U s) {
			value_first = f;
			value_second = s;
		};
		wPair<T, U>(wPair<T,U> * p) {
			value_first = p->value_first;
			value_second = p->value_second;
		};
		void operator=(const wPair<T, U> & wp)
		{
			value_first = wp.value_first;
			value_second = wp.value_second;
		}
		virtual ~wPair() {};
	};


	template <typename T , typename U>
	class wMap
	{
	public:
		RKList<wPair<T, U>> map_value;
	public:
		wMap() {};
		virtual ~wMap() { map_value.Clear(); }

		void operator=(const RKList<wPair<T, U>*> & D)
		{
			for (size_t i = 0; i < D->map_value.Size(); i++)
			{
				map_value.Insert(D->map_value.GetAt(i));
			}
		}


		wPair<T,U> end()
		{
			return wPair<T, U>();
		}

		bool contain(T value)
		{
			for (size_t i = 0; i < map_value.Size(); i++)
			{
				auto val = map_value.GetAt(i);
				if (val.value_first == value)
				{
					return true;
				}
			}
			return false;
		}

		wPair<T, U> find(T value)
		{
			for (size_t i = 0; i < map_value.Size(); i++)
			{
				auto val = map_value.GetAt(i);
				if (val.value_first == value)
				{
					return val;
				}
			}
			return wPair<T, U>();
		}

		U at(T value)
		{
			for (size_t i = 0; i < map_value.Size(); i++)
			{
				auto val = map_value.GetAt(i);
				if (val.value_first == value)
				{
					return val.value_second;
				}
			}
			return U();
		}

		void insert(T val1, U val2)
		{
			map_value.Insert(wPair<T, U>(val1, val2), map_value.Size());
		}

		void insert(wPair<T, U> val)
		{
			map_value.Insert(val, map_value.Size());
		}

		void erase(T valu)
		{
			for (size_t i = 0; i < map_value.Size(); i++)
			{
				wPair<T, U>  val = map_value.GetAt(i);
				if (val.value_first == valu)
				{
					map_value.EraseAt(i);
					return;
				}
			}
		}

		void clear()
		{
			map_value.Clear();
		}

		size_t size()
		{
			return map_value.Size();
		}

		std::map<T,U> toMap()
		{
			std::map<T,U> value;

			for (size_t i = 0; i < map_value.Size(); i++)
			{
				wPair<T,U>  val = map_value.GetAt(i);
				value.insert({ val.value_first, val.value_second });
			}

			return value;
		}
	};
};
#endif //__WMAP_H__