///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	matrix.h
///
/// Description :	Implementation of a Lightweight RKMatrix Class
///----------------------------------------------------------------------
#ifndef _MATRIX_H
#define _MATRIX_H

///----------------------------------------------------------------------
/// Includes
///----------------------------------------------------------------------
#include "RKVector.h"
#include <algorithm>
using namespace std;
namespace RKUtils
{
#define RKAXIS_X 0
#define RKAXIS_Y 1
#define RKAXIS_Z 2

	struct RKQuaternion;

	///----------------------------------------------------------------------
	/// Class RKMatrix()
	///
	/// RK use column-major matrices (both storage and notation) and column vectors
	/// to match notation used in OpenGL and GLSL
	///
	/// Vector transformation:
	///                    c0  c1  c2  c3
	///                  | m00 m10 m20 m30 |   |x|
	/// V_transformed =  | m01 m11 m21 m31 | * |y| == x*c0 + y*c1 + z*c2 + w*c3
	///                  | m02 m12 m22 m32 |   |z|
	///                  | m03 m13 m23 m33 |   |w|
	///
	/// Matrix multiplication: from right to left:
	/// |M_global| * |M_local|
	///
	/// E.g:
	/// V_projected = |M_proj| * |M_view| * |M_model| * V_local
	///
	/// Info:
	/// http://seanmiddleditch.com/journal/2012/08/matrices-handedness-pre-and-post-multiplication-row-vs-column-major-and-notations/
	/// http://www.mindcontrol.org/~hplus/graphics/matrix-layout.html
	/// http://en.wikibooks.org/wiki/GLSL_Programming/Applying_Matrix_Transformations
	///
	///----------------------------------------------------------------------
	struct RKMatrix
	{
		static const RKMatrix uvnormalizer;
		static const RKMatrix identity;
		static const RKMatrix zero;

		union
		{
			struct
			{
				// WARNING! Matrix has column-major storage,
				// but we use row-major notation!
				// it looks transposed here:
				//
				//    row0 row1 row2 row3
				float m00, m10, m20, m30, // column 0
					m01, m11, m21, m31, // column 1
					m02, m12, m22, m32, // column 2
					m03, m13, m23, m33; // column 3
				//
				// actually looks like:
				// | m00 m01 m02 m03 |
				// | m10 m11 m12 m13 |
				// | m20 m21 m22 m23 |
				// | m30 m31 m32 m33 |
				//
			};

			float f[16];

			float col[4][4];
		};

		RKVector& Column(unsigned int idx)       { return ((RKVector*)(this))[idx]; }
		const RKVector& Column(unsigned int idx) const { return ((RKVector*)(this))[idx]; }

		void SetZero() { memset(this, 0, sizeof(RKMatrix)); }
		void SetIdentity() { SetZero(); m00 = m11 = m22 = m33 = 1.f; }
		void Set(const float* pMatrix) { memcpy(this, pMatrix, sizeof(float) * 16); }
		void FromQuaternion(const RKQuaternion& quaternion, const RKVector& translation);
		void SetAsMultiplication(const RKMatrix& m1, const RKMatrix& m2);
		static void MultiplyAndTransformToPacked4x3(const RKMatrix& m1, const RKMatrix& m2, float* pOut); //pOut must support 12 floats
		bool SetAsInverse(const RKMatrix& src);

		void MakeRotationXMatrix(float angRadians);
		void MakeRotationYMatrix(float angRadians);
		void MakeRotationZMatrix(float angRadians);
		void MakeRotationMatrix(int axisIdx, float angRadians);
		void MakeRotationMatrix(const RKVector& axis, float angRadians);

		void MakeOrientationMatrix(const RKVector& dir, const RKVector& up);

		//  this gives incorrect results if used with Entity!!
		//  because Entity has different contract about angle <-> direction representation
		void MakeLookAtMatrix(const RKVector& origon, const RKVector& dest, const RKVector& up);

		void MakeOrthoMatrix(float l, float r, float b, float t, float n, float f);
		void MakePerspectiveMatrix(float fov, float nearplane, float farplane, float aspect);
		void MakePerspectiveMatrix(float l, float r, float t, float b, float n, float f);

		RKMatrix    Inversed() const { RKMatrix out; out.SetAsInverse(*this); return out; }
		inline void TransposeInplace();

		inline void ConvertToLeftHand();
		inline void ConvertToRightHand();

		const RKVector& GetXAxis() const   { return Column(0); }
		const RKVector& GetYAxis() const   { return Column(1); }
		const RKVector& GetZAxis() const       { return Column(2); }
		const RKVector& GetTranslation() const { return Column(3); }

		void SetXAxis(const RKVector& axis) { Column(0) = axis; }
		void SetYAxis(const RKVector& axis) { Column(1) = axis; }
		void SetZAxis(const RKVector& axis) { Column(2) = axis; }

		inline void SetTranslation(float x, float y, float z);
		inline void SetTranslation(const RKVector& t) { SetTranslation(t.x, t.y, t.z); }

		inline void Translate(float x, float y, float z);
		inline void Translate(const RKVector& t) { Translate(t.x, t.y, t.z); }

		inline void Scale(float x, float y, float z);
		inline void Scale(float scale)            { Scale(scale, scale, scale); }
		inline void Scale(const RKVector& scale)  { Scale(scale.x, scale.y, scale.z); }
		//inline static void Scale(const RKMatrix* m0, const RKVector* v0, RKMatrix* m1);
		inline static void Scale(const RKMatrix* m0, const RKVector* v0, RKMatrix* m1)
		{
#if defined(__ARM_NEON__) && !defined(__arm64__)
			asm volatile (
				"vld1.32 		{d0, d1},   [%2]    \n\t"	//q0 = v0
				"vld1.32 		{d16, d17}, [%0]!	  \n\t"	//q8 = m0.col0
				"vld1.32 		{d18, d19}, [%0]!	  \n\t"	//q9 = m0.col1
				"vld1.32 		{d20, d21}, [%0]!	  \n\t"	//q10 = m0.col2
				"vld1.32 		{d22, d23}, [%0]    \n\t"	//q11 = m0.col3
				"vmul.f32   q8, q8, d0[0]       \n\t" //q8 = q8 * v0.x
				"vmul.f32   q9, q9, d0[1]       \n\t" // q9 = q9 * v0.y
				"vmul.f32   q10, q10, d1[0]     \n\t" // q10 = q10 * v0.z
				"vst1.32 		{d16, d17}, [%1]!   \n\t"	//q8
				"vst1.32 		{d18, d19}, [%1]!	  \n\t"	//q9
				"vst1.32 		{d20, d21}, [%1]!  	\n\t"	//q10
				"vst1.32 		{d22, d23}, [%1]   	\n\t"	//q10
				//: "+r"(m0), "+r"(v0), "+r"(m1)
				//: "r"(m0), "r"(v0), "r"(m1)
				: "+r"(m0), "+r"(m1)
				: "r"(v0)
				: "q0","q8", "q9", "q10", "q11", "memory", "cc"
				);
#else
			m1->Set((const float*)m0);
			m1->Scale(v0->x, v0->y, v0->z);
#endif
		}

		float DetGeneric() const;

		RKMatrix operator*(const RKMatrix& matrixIn) const
		{
			RKMatrix res;
			res.SetAsMultiplication(*this, matrixIn);
			return res;
		}

		RKMatrix& operator*=(const RKMatrix& matrixIn)
		{
			RKMatrix copy = *this;
			SetAsMultiplication(copy, matrixIn);
			return *this;
		}

		inline RKVector TransformPos(const RKVector& pos) const;
		RKVector TransformDir(const RKVector& dir) const;

		// added by frank.kyoshi.liu<frank.liu@gameloft.com>
		// changed the initialization method using dx convention: if you like
		static RKMatrix GetMatrixUsingRowMajorConvention(const float& m11, const float& m12, const float& m13, const float& m14,
			const float& m21, const float& m22, const float& m23, const float& m24,
			const float& m31, const float& m32, const float& m33, const float& m34,
			const float& m41, const float& m42, const float& m43, const float& m44
			);

		// return the determinant of the given matrix:stored in column major 
		static float GetDeterminant(const RKMatrix& mat);

		void SetValuesUsingRowMajorConvention(const float& _m11, const float& _m12, const float& _m13, const float& _m14,
			const float& _m21, const float& _m22, const float& _m23, const float& _m24,
			const float& _m31, const float& _m32, const float& _m33, const float& _m34,
			const float& _m41, const float& _m42, const float& _m43, const float& _m44
			);

		// vector * vector^t with vector^t is the transposed vector of vector and vector is a column vector
		// use the correct convention for point : <x, y, z, 1> and vector : <x, y, z, 0> before perspective transform
		static inline RKMatrix GetTensorMatrix(const RKVector& vector)
		{
			return GetTensorMatrix(vector, vector);
		}

		// vec0 * vec1^t
		static RKMatrix GetTensorMatrix(const RKVector& vec0, const RKVector& vec1);

		RKMatrix operator*(const float scale_factor);

		RKMatrix& operator*=(const float scale_factor);

		inline RKMatrix operator/(const float scale_factor)
		{
			return operator*(1.0f / scale_factor);
		}

		inline RKMatrix& operator /= (const float scale_factor)
		{
			return operator*=(1.0f / scale_factor);
		}
	};

	void RKMatrix::TransposeInplace()
	{
		swap(m01, m10);
		swap(m02, m20);
		swap(m03, m30);

		swap(m12, m21);
		swap(m13, m31);

		swap(m23, m32);
	}

	void RKMatrix::ConvertToLeftHand()
	{
		m02 = -m02;
		m12 = -m12;
		m22 = -m22;
		m32 = -m32;
	}

	void RKMatrix::ConvertToRightHand()
	{
		m02 = -m02;
		m12 = -m12;
		m22 = -m22;
		m32 = -m32;
	}

	void RKMatrix::SetTranslation(float x, float y, float z)
	{
		m03 = x;
		m13 = y;
		m23 = z;
	}

	void RKMatrix::Translate(float x, float y, float z)
	{
		m03 += x;
		m13 += y;
		m23 += z;
	}



	void RKMatrix::Scale(float x, float y, float z)
	{
		m00 *= x;
		m10 *= x;
		m20 *= x;
		m30 *= x;
		m01 *= y;
		m11 *= y;
		m21 *= y;
		m31 *= y;
		m02 *= z;
		m12 *= z;
		m22 *= z;
		m32 *= z;
	}

	// added by frank.kyoshi.liu<frank.liu@gameloft.com>
	// to support diverse kinds of CD calculations
	// the 2x2 mat
	// internally the data will be stored row-major
	class RKMatrix2
	{
	public:
		static RKMatrix2 MakeAMatrix(const RKMatrix& mat4x4);

		static float GetItsDeterminant(const RKMatrix2& mat2x2);

		float GetTheDeterminant()const;

		RKMatrix2(const float& m11, const float& m12,
			const float& m21, const float& m22
			);

		union
		{
			struct
			{
				float _m11, _m12;
				float _m21, _m22;
			};

			float val[2 * 2];
		};
	};

	// 3x3 matrix: internally values are stored row-major
	class RKMatrix3
	{
	public:
		static RKMatrix3 MakeAMatrix(const RKMatrix& mat4x4);

		static float GetItsDeterminant(const RKMatrix3& mat3x3);

		float GetTheDeterminant()const;

		RKMatrix3(const float& m11, const float& m12, const float& m13,
			const float& m21, const float& m22, const float& m23,
			const float& m31, const float& m32, const float& m33
			);
		union
		{
			struct
			{
				float _m11, _m12, _m13;
				float _m21, _m22, _m23;
				float _m31, _m32, _m33;
			};

			float val[3 * 3];
		};
	};


#if defined(__ARM_NEON__) && !defined(__arm64__)
	inline void RKMatrixTransformPos_Neon(const RKMatrix* m0, const RKVector* v0, RKVector* pOut)
	{
		asm volatile (
			"vld1.32 		{d0, d1},   [%2]		\n\t"	//q0 = v0
			"vld1.32 		{d16, d17}, [%0]!		\n\t"	//q8 = m0.col0
			"vld1.32 		{d18, d19}, [%0]!		\n\t"	//q9 = m0.col1
			"vld1.32 		{d20, d21}, [%0]!		\n\t"	//q10 = m0.col2
			"vld1.32 		{d22, d23}, [%0]		\n\t"	//q11 = m0.col3
			"vmul.f32   q1, q8, d0[0]       \n\t" //q1 = m0.col0 * v0.x
			"vmla.f32   q1, q9, d0[1]       \n\t" // q1+= m0.col1 * v0.y
			"vmla.f32   q1, q10, d1[0]       \n\t" // q1+= m0.col2 * v0.z
			"vmla.f32   q1, q11, d1[1]       \n\t" // q1+= m0.col3 * v0.w
			"vst1.32 		{d2,d3}, [%1]					\n\t"	//q1 -> pOut
			: "+r"(m0), "+r"(pOut)
			: "r"(v0)
			: "q0","q1","q8","q9","q10","q11", "memory"
			);
	}
#endif

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	RKVector RKMatrix::TransformPos(const RKVector& v) const
	{
		RKVector res;
#if defined(__ARM_NEON__) && !defined(__arm64__)
		RKMatrixTransformPos_Neon(this, &v, &res);
		return res;
#else
		res.x = (m00*v.x) + (m01*v.y) + (m02*v.z) + (m03*v.w);
		res.y = (m10*v.x) + (m11*v.y) + (m12*v.z) + (m13*v.w);
		res.z = (m20*v.x) + (m21*v.y) + (m22*v.z) + (m23*v.w);
		res.w = (m30*v.x) + (m31*v.y) + (m32*v.z) + (m33*v.w);

		return res;
#endif
	}
}

///----------------------------------------------------------------------
#endif // _MATRIX_H

