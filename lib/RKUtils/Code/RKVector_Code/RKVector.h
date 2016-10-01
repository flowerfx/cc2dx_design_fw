///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKVector.h
///
/// Description :	Implementation of a RKVector Class
///----------------------------------------------------------------------
#ifndef RKVECTOR_H
#define RKVECTOR_H

#include "math.h"

#include "../Common/RKMacros.h"
#include "RKVectorGeneric.h"
namespace RKUtils
{
	struct RKMatrix;
	struct RKVector2;

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	struct RKVector
	{
		union
		{
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };

			unsigned int u32[4];
			float v[4];
		};

		static const RKVector Red;
		static const RKVector Green;
		static const RKVector Blue;
		static const RKVector White;
		static const RKVector Yellow;

		static const RKVector Right;
		static const RKVector Up;
		static const RKVector Forward;

		static const RKVector One;
		static const RKVector Zero;

		static const RKVector InvalidValue;

		enum NoInit_ { NoInit };

		inline RKVector() : x(0), y(0), z(0), w(0) {}
		inline RKVector(NoInit_) {}
		inline explicit RKVector(float ax, float ay, float az = 0.f, float aw = 1.f) : x(ax), y(ay), z(az), w(aw) {}
		inline explicit RKVector(float* xyz, float aw = 1.f) : x(xyz[0]), y(xyz[1]), z(xyz[2]), w(aw) {}
		inline explicit RKVector(const RKVector2& vec2, float az = 0.f, float aw = 1.f);

		void Set(float ax, float ay, float az, float aw = 1.f) { x = ax;     y = ay;     z = az;     w = aw; }
		void Set(float* xyz, float aw = 1.f)                   { x = xyz[0]; y = xyz[1]; z = xyz[2], w = aw; }

		// operators
		float& operator[](unsigned int index)       { RKASSERT(index < 4, "Index out of range"); return v[index]; }
		float  operator[](unsigned int index) const { RKASSERT(index < 4, "Index out of range"); return v[index]; }

		RKVector operator*(const float f) const { return RKVector(x * f, y * f, z * f); }
		RKVector operator/(const float f) const { return (*this) * (1.f / f); }

		RKVector operator+(const RKVector& vecIn) const { return RKVector(x + vecIn.x, y + vecIn.y, z + vecIn.z); }
		RKVector operator-(const RKVector& vecIn) const { return RKVector(x - vecIn.x, y - vecIn.y, z - vecIn.z); }
		RKVector operator-() const                      { return RKVector(-x, -y, -z); }

		RKVector& operator+=(const RKVector& vecIn) { x += vecIn.x; y += vecIn.y; z += vecIn.z; return *this; }
		RKVector& operator-=(const RKVector& vecIn) { x -= vecIn.x; y -= vecIn.y; z -= vecIn.z; return *this; }

		RKVector& operator*=(const float f)         { x *= f; y *= f; z *= f; return *this; }
		RKVector& operator/=(const float f)         { return (*this) *= (1.f / f); }

		bool operator==(const RKVector& vecIn) const { return (this->x == vecIn.x) && (this->y == vecIn.y) && (this->z == vecIn.z) && (this->w == vecIn.w); }
		bool operator!=(const RKVector& vecIn) const { return !(*this == vecIn); }

		// Utility functions
		void SetZero() { *this = Zero; }
		void SetOne() { *this = One; }

		// length/normal
		float LengthSquared() const { return (x * x + y * y + z * z); }
		float Length() const        { return sqrtf(LengthSquared()); }

		float LengthXZ() const { return sqrtf(x*x + z*z); }
		float LengthXY() const { return sqrtf(x*x + y*y); }
		float LengthYZ() const { return sqrtf(y*y + z*z); }

		inline float Normalize(float tolerance = RKEPSILON);
		inline float NormalizeXY(float tolerance = RKEPSILON);

		// special
		inline void Lerp(const RKVector& v1, const RKVector& v2, const float t);
		inline void Lerp(const RKVector& vDest, const float t);

		inline void Reflect(const RKVector& normal);

		// statics
		static RKVector MultiplyPerComponent(const RKVector& v1, const RKVector& v2) { return RKVector(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
		static float    Dot(const RKVector& v1, const RKVector& v2)                  { return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z); }
		static float    Dot4(const RKVector& v1, const RKVector& v2)								 { return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w); }
		static RKVector Cross(const RKVector& v1, const RKVector& v2)                { return RKVector(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }
		static float    AngleBetween(const RKVector& v1, const RKVector& v2)         { return (float)acos(Dot(v1, v2)); }
		static RKVector Scale4(const RKVector& v1, const float f)                  { return RKVector(v1.x * f, v1.y * f, v1.z * f, v1.w * f); }
		static RKVector Add4(const RKVector& v1, const RKVector& v2)               { return RKVector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }
		inline static void MAdd(RKVector* v0, RKVector* v1, float s);//v0 += v1 * s
		// added by frank.kyoshi.liu <frank.liu@gameloft.com>
		// to support matrix manipulation for cross product
		// this func returns the matrix representation of u x in the cross production of : u x some
		static RKMatrix GetCrossProductMatrix(const RKVector& u);
		// to build a orthonormal base using one given vector: the frame is <u, v, w> : right handed system
		// this function assumes the given vector is unit vector
		static void GetOrthonormalBase(RKVector& u_axis, const RKVector& given_vector, RKVector& w_axis);

		// member helpers
		RKVector Cross(const RKVector& vecIn) const { return RKVector::Cross(*this, vecIn); }
		float    Dot(const RKVector& vecIn) const  { return RKVector::Dot(*this, vecIn); }
	};

#if defined(__ARM_NEON__) && !defined(__arm64__)
#include<arm_neon.h>

	inline void RKVector::MAdd(RKVector* v0, RKVector* v1, float s)
	{
		asm volatile (
			"vdup.32    q0, %2               \n\t" //q0 = s,s,s,s
			"vld1.32 		{d2, d3}, [%0]			\n\t"	//q1 = v0
			"vld1.32 		{d4, d5}, [%1]			\n\t"	//q2 = v1
			"vmla.f32 	q1, q0, q2          \n\t"	//q1 += q0 * q2 //v0 += v1 * s
			"vst1.32 		{d2,d3}, [%0]					\n\t"	//d0 = d[0] + d[1]
			:: "r"(v0), "r"(v1), "r"(s) :
			"q0","q1","q2"
			);
	}

#else
	inline void RKVector::MAdd(RKVector* v0, RKVector* v1, float s)
	{
		RKVector v = RKVector::Add4(*v0, RKVector::Scale4(*v1, s));
		*v0 = v;
	}

#endif

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	struct RKVector2
	{
		union
		{
			struct { float x, y; };
			float v[2];
		};

		static const RKVector2 InvalidValue;

		RKVector2(float ax = 0, float ay = 0) : x(ax), y(ay) {}

		void Set(float ax, float ay) { x = ax; y = ay; }
		void Set(float xy[]) { x = xy[0]; y = xy[1]; }

		RKVector2 operator+(const RKVector2& vecIn) const { return RKVector2(x + vecIn.x, y + vecIn.y); }
		RKVector2 operator-(const RKVector2& vecIn) const { return RKVector2(x - vecIn.x, y - vecIn.y); }
		RKVector2 operator-() const                       { return RKVector2(-x, -y); }
		RKVector2 operator*(const float f) const          { return RKVector2(x * f, y * f); }
		RKVector2 operator/(const float f) const          { return *this * (1.f / f); }

		RKVector2& operator+=(const RKVector2& vecIn)     { x += vecIn.x; y += vecIn.y; return *this; }
		RKVector2& operator-=(const RKVector2& vecIn)     { x -= vecIn.x; y -= vecIn.y; return *this; }
		RKVector2& operator*=(const float f)              { x *= f; y *= f; return *this; }
		RKVector2& operator/=(const float f)              { return *this *= (1.f / f); }

		bool operator==(const RKVector2& vecIn) const     { return x == vecIn.x && y == vecIn.y; }
		bool operator!=(const RKVector2& vecIn) const     { return !(*this == vecIn); }

		float LengthSquared() const                       { return (x*x + y*y); }
		float Length() const                              { return sqrtf(LengthSquared()); }

		static float Dot(const RKVector2& v1, const RKVector2& v2)          { return (v1.x*v2.x + v1.y*v2.y); }
		static float Cross(const RKVector2& v1, const RKVector2& v2)        { return (v1.x*v2.y - v1.y*v2.x); }
		static float AngleBetween(const RKVector2& v1, const RKVector2& v2) { return (float)acos(Dot(v1, v2)); }

		float Dot(const RKVector2& vecIn) const           { return RKVector2::Dot(*this, vecIn); }

		void Lerp(const RKVector2& v1, const RKVector2& v2, const float t) { *this = v1 + (v2 - v1) * t; }

		inline float Normalize(float tolerance = RKEPSILON);

		inline void Reflect(const RKVector2& normal);

		// added by frank.kyoshi.liu
		// to get the perpendicular vector of the given 2D vector: for 2D the perpendicular is just random selected
		// if v is <x, y> the perpendicular vector of v is u:<-y, x>. because the dot product is finely defined for 2D and v dot u := <x,y> dot <-y, x> == 0
		static RKVector2 GetPerpendicularVector(const RKVector2& v);
	};

	struct RKVec2i : RKVec2 < int >
	{
		RKVec2i() {}
		RKVec2i(int x, int y) { this->x = x; this->y = y; }

		RKVec2i operator-(const RKVec2i& vecIn) const { return RKVec2i(x - vecIn.x, y - vecIn.y); }
		RKVec2i operator+(const RKVec2i& vecIn) const { return RKVec2i(x + vecIn.x, y + vecIn.y); }

		bool operator!=(const RKVec2i& vecIn) const  { return x != vecIn.x || y != vecIn.y; }
		bool operator==(const RKVec2i& vecIn) const  { return !(*this != vecIn); }

		int   LengthSquared() const             { return (x*x + y*y); }
		float Length() const                    { return sqrtf(static_cast<float>(LengthSquared())); }
	};

	RKVector::RKVector(const RKVector2& vec2, float az /* = 0.f */, float aw /* = 1.f */) : x(vec2.x), y(vec2.y), z(az), w(aw)
	{
	}

	void RKVector::Lerp(const RKVector& v1, const RKVector& v2, const float t)
	{
		if (t <= 0.0f)
		{
			*this = v1;
		}
		else if (t >= 1.0f)
		{
			*this = v2;
		}
		else
		{
			*this = v1 + (v2 - v1) * t;
		}
	}

	void RKVector::Lerp(const RKVector& vDest, const float t)
	{
		if (t <= 0.0f)
		{
			return;
		}
		else if (t >= 1.0f)
		{
			*this = vDest;
		}
		else
		{
			// *this = v1 + (v2 - v1) * t;
			x += ((vDest.x - x) * t);
			y += ((vDest.y - y) * t);
			z += ((vDest.z - z) * t);
			w += ((vDest.w - w) * t);
		}
	}

	float RKVector::Normalize(float tolerance /* = RKEPSILON */)
	{
		float length = Length();

		float inv;
		if (length < tolerance)
			inv = 0.0f;
		else
			inv = 1.0f / length;

		*this *= inv;

		return length;
	}

	float RKVector::NormalizeXY(float tolerance /* = RKEPSILON */)
	{
		float length = LengthXY();
		float invMag;
		if (length < tolerance)
			invMag = 0.f;
		else
			invMag = 1.f / length;

		x *= invMag;
		y *= invMag;
		z = 0.f;
		w = 0.f;

		return length;
	}

	void RKVector::Reflect(const RKVector& normal)
	{
		// *this vector is the incident vector
		const float perpendicular_magnitude = normal.Dot(*this);

		RKVector scaleNormal = normal * (2.f * perpendicular_magnitude);

		*this -= scaleNormal;
	}

	inline RKVector operator * (float sc, const RKVector& vec)
	{
		return vec * sc;
	}

	inline RKVector2 operator * (float sc, const RKVector2& vec)
	{
		return vec * sc;
	}

	float RKVector2::Normalize(float tolerance /* = RKEPSILON */)
	{
		float length = Length();
		float invMag;
		if (length < tolerance)
			invMag = 0.f;
		else
			invMag = 1.f / length;

		x *= invMag;
		y *= invMag;

		return length;
	}

	void RKVector2::Reflect(const RKVector2& normal)
	{
		// this vector is the incident vector
		const float perpendicular_magnitude = normal.Dot(*this);

		RKVector2 scaleNormal = normal * (2.f * perpendicular_magnitude);

		*this -= scaleNormal;
	}
}
#endif // RKVECTOR_H
