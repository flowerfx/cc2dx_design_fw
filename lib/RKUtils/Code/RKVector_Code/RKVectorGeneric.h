///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKVector.h
///
/// Description :	Implementation of a RKVector Class
///----------------------------------------------------------------------
#ifndef RKVECTOR_GENERIC_H
#define RKVECTOR_GENERIC_H
namespace RKUtils
{
	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	template<class T>
	class RKVec2
	{
	public:
		union
		{
			struct { T x, y; };
			struct { T s, t; };
			struct { T r, g; };
			T i[2];
		};

		void Set(T x, T y)
		{
			this->x = x;
			this->y = y;
		}
	};

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	template<class T>
	class RKVec3
	{
	public:
		union
		{
			struct { T x, y, z; };
			struct { T s, t, u; };
			struct { T r, g, b; };
			T i[3];
		};

		void Set(T x, T y, T z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
	};

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	template<class T>
	class RKVec4
	{
	public:
		union
		{
			struct { T x, y, z, w; };
			struct { T s, t, u, v; };
			struct { T r, g, b, a; };
			T i[4];
		};

		void Set(T x, T y, T z, T w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
	};
}
#endif // RKVECTOR_GENERIC_H
