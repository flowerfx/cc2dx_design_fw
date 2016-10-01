///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	quaternion.h
///
/// Description :	Implementation of a Lightweight Quaternion Class
///----------------------------------------------------------------------
#ifndef _RKQUATERNION_H
#define _RKQUATERNION_H
namespace RKUtils
{
	struct RKVector;
	struct RKMatrix;

	///-------------------------------------------------------------------------------
	/// class RKQuaternion()
	///-------------------------------------------------------------------------------
	struct RKQuaternion
	{
		union
		{
			struct { float x, y, z, w; };
			unsigned int u32[4];
			float v[4];
		};

		static const RKQuaternion Identity;
		static const RKQuaternion Invalid;

		RKQuaternion() : x(0), y(0), z(0), w(1)
		{}

		RKQuaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
		{}



		float operator [] (int i) const
		{
			return *(((const float*)&x) + i);
		}
		float& operator [] (int i)
		{
			return *(((float*)&x) + i);
		}

		RKQuaternion operator-() const
		{
			RKQuaternion q;
			q.x = x, q.y = y, q.z = z, q.w = -w;
			return q;
		}

		RKQuaternion operator*(const RKQuaternion& q) const
		{
			RKQuaternion result;

			result.x = (q.w * x) + (q.x * w) + (q.y * z) - (q.z * y);
			result.y = (q.w * y) + (q.y * w) + (q.z * x) - (q.x * z);
			result.z = (q.w * z) + (q.z * w) + (q.x * y) - (q.y * x);
			result.w = (q.w * w) - (q.x * x) - (q.y * y) - (q.z * z);

			return result;
		}

		RKQuaternion& operator=(const RKQuaternion& in)
		{
			x = in.x;
			y = in.y;
			z = in.z;
			w = in.w;

			return *this;
		}

		//  this gives incorrect results if used with Entity!!
		//  because Entity has different contract about angle <-> direction representation
		void	FromLookAt(const RKVector& origin, const RKVector& dest);

		void	Normalize();
		void	FromEuler(float pitch, float yaw, float roll);
		void	FromYaw(float radians);
		void	FromPitch(float radians);
		void	FromRoll(float radians);
		float	Dot(const RKQuaternion& q) const;
		float	LengthSqr() const;
		float	Length() const;
		float	Angle(const RKQuaternion& q);

		void  Rotate(RKVector& vec) const;

		void	Lerp(const RKQuaternion& source, const RKQuaternion& dest, float t);
		void  Lerp(const RKQuaternion& dest, float t)
		{
			Lerp(*this, dest, t);
		}

		void  Slerp(const RKQuaternion& source, const RKQuaternion& dest, float t);
		void  Slerp(const RKQuaternion& dest, float t)
		{
			Slerp(*this, dest, t);
		}

		void	NLerp(const RKQuaternion& source, const RKQuaternion& dest, float t);
		void  NLerp(const RKQuaternion& dest, float t)
		{
			NLerp(*this, dest, t);
		}

		void	FromMatrix(const RKMatrix& m);
		void  FromAxisAngle(const RKVector& axis, const float radians);
	};
}
///-------------------------------------------------------------------------------
#endif

