///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	matrix.cpp
///
/// Description :	Implementation of a Lightweight RKMatrix Class
///----------------------------------------------------------------------

///----------------------------------------------------------------------
/// Includes
///----------------------------------------------------------------------

///----------------------------------------------------------------------
///----------------------------------------------------------------------
#include "RKMatrix.h"

#include "../Common/RK.h"
#include "RKQuaternion.h"
namespace RKUtils
{
///----------------------------------------------------------------------
///----------------------------------------------------------------------

///----------------------------------------------------------------------
///----------------------------------------------------------------------
const RKMatrix RKMatrix::identity =
{ 1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1 };

const RKMatrix RKMatrix::zero =
{ 0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0 };

const RKMatrix RKMatrix::uvnormalizer =
{
  0.5f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.5f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.5f, 0.0f,
  0.5f, 0.5f, 0.5f, 1.0f,
};


#if defined(__ARM_NEON__) && !defined(__arm64__)
void RKMatrixMul_Neon(const RKMatrix* m0, const RKMatrix* m1, RKMatrix* d)
{
	asm volatile (
                "vld1.32 		{d0, d1}, [%1]!			\n\t"	//q0 = m1.col0
                "vld1.32 		{d2, d3}, [%1]!			\n\t"	//q1 = m1.col1
                "vld1.32 		{d4, d5}, [%1]!			\n\t"	//q2 = m1.col2
                "vld1.32 		{d6, d7}, [%1]			\n\t"	//q3 = m1.col3
                "vld1.32 		{d16, d17}, [%0]!		\n\t"	//q8 = m0.col0
                "vld1.32 		{d18, d19}, [%0]!		\n\t"	//q9 = m0.col1
                "vld1.32 		{d20, d21}, [%0]!		\n\t"	//q10 = m0.col2
                "vld1.32 		{d22, d23}, [%0]		\n\t"	//q11 = m0.col3
                
                "vmul.f32 		q12, q8, d0[0] 			\n\t"	//q12 = m0.col0 * m1.col0,0
                "vmul.f32 		q13, q8, d2[0] 			\n\t"	//q13 = m0.col0 * m1.col1,0
                "vmul.f32 		q14, q8, d4[0] 			\n\t"	//q14 = m0.col0 * m1.col2,0
                "vmul.f32 		q15, q8, d6[0]	 		\n\t"	//q15 = m0.col0 * m1.col3,0
                "vmla.f32 		q12, q9, d0[1] 			\n\t"	//q12 += m0.col1 * m1.col0,1
                "vmla.f32 		q13, q9, d2[1] 			\n\t"	//q13 += m0.col1 * m1.col0,1
                "vmla.f32 		q14, q9, d4[1] 			\n\t"	//q14 += m0.col1 * m1.col0,1
                "vmla.f32 		q15, q9, d6[1] 			\n\t"	//q15 += m0.col1 * m1.col0,1
                "vmla.f32 		q12, q10, d1[0] 		\n\t"	//q12 += m0.col2 * m1.col0,2
                "vmla.f32 		q13, q10, d3[0] 		\n\t"	//q13 += m0.col2 * m1.col1,2
                "vmla.f32 		q14, q10, d5[0] 		\n\t"	//q14 += m0.col2 * m1.col2,2
                "vmla.f32 		q15, q10, d7[0] 		\n\t"	//q15 += m0.col2 * m1.col3,2
                "vmla.f32 		q12, q11, d1[1] 		\n\t"	//q12 += m0.col3 * m1.col0,3
                "vmla.f32 		q13, q11, d3[1] 		\n\t"	//q13 += m0.col3 * m1.col1,3
                "vmla.f32 		q14, q11, d5[1] 		\n\t"	//q14 += m0.col3 * m1.col2,3
                "vmla.f32 		q15, q11, d7[1]	 		\n\t"	//q15 += m0.col3 * m1.col3,3
                
                "vst1.32 		{d24, d25}, [%2]! 	\n\t"	//q12
                "vst1.32 		{d26, d27}, [%2]!		\n\t"	//q13
                "vst1.32 		{d28, d29}, [%2]! 	\n\t"	//q14
                "vst1.32 		{d30, d31}, [%2]	 	\n\t"	//q15
                
                : "+r"(m0), "+r"(m1), "+r"(d) :
                : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15",
                "memory"
                );
}

//multiplies matrix, and returns a packed-transposed matrix
void RKMatrixMulTranspose4x3_Neon(const RKMatrix* m0, const RKMatrix* m1, float* d)
{
	asm volatile (
                "vld1.32 		{d0, d1}, [%1]!			\n\t"	//q0 = m1.col0
                "vld1.32 		{d2, d3}, [%1]!			\n\t"	//q1 = m1.col1
                "vld1.32 		{d4, d5}, [%1]!			\n\t"	//q2 = m1.col2
                "vld1.32 		{d6, d7}, [%1]			\n\t"	//q3 = m1.col3
                "vld1.32 		{d16, d17}, [%0]!		\n\t"	//q8 = m0.col0
                "vld1.32 		{d18, d19}, [%0]!		\n\t"	//q9 = m0.col1
                "vld1.32 		{d20, d21}, [%0]!		\n\t"	//q10 = m0.col2
                "vld1.32 		{d22, d23}, [%0]		\n\t"	//q11 = m0.col3
                
                "vmul.f32 		q12, q8, d0[0] 			\n\t"	//q12 = m0.col0 * m1.col0,0
                "vmul.f32 		q13, q8, d2[0] 			\n\t"	//q13 = m0.col0 * m1.col1,0
                "vmul.f32 		q14, q8, d4[0] 			\n\t"	//q14 = m0.col0 * m1.col2,0
                "vmul.f32 		q15, q8, d6[0]	 		\n\t"	//q15 = m0.col0 * m1.col3,0
                "vmla.f32 		q12, q9, d0[1] 			\n\t"	//q12 += m0.col1 * m1.col0,1
                "vmla.f32 		q13, q9, d2[1] 			\n\t"	//q13 += m0.col1 * m1.col0,1
                "vmla.f32 		q14, q9, d4[1] 			\n\t"	//q14 += m0.col1 * m1.col0,1
                "vmla.f32 		q15, q9, d6[1] 			\n\t"	//q15 += m0.col1 * m1.col0,1
                "vmla.f32 		q12, q10, d1[0] 		\n\t"	//q12 += m0.col2 * m1.col0,2
                "vmla.f32 		q13, q10, d3[0] 		\n\t"	//q13 += m0.col2 * m1.col1,2
                "vmla.f32 		q14, q10, d5[0] 		\n\t"	//q14 += m0.col2 * m1.col2,2
                "vmla.f32 		q15, q10, d7[0] 		\n\t"	//q15 += m0.col2 * m1.col3,2
                "vmla.f32 		q12, q11, d1[1] 		\n\t"	//q12 += m0.col3 * m1.col0,3
                "vmla.f32 		q13, q11, d3[1] 		\n\t"	//q13 += m0.col3 * m1.col1,3
                "vmla.f32 		q14, q11, d5[1] 		\n\t"	//q14 += m0.col3 * m1.col2,3
                "vmla.f32 		q15, q11, d7[1]	 		\n\t"	//q15 += m0.col3 * m1.col3,3
                
                "vtrn.32      q12,q13             \n\t" //transpose our final matrix
                "vtrn.32      q14,q15             \n\t"
                "vswp         d25,d28             \n\t"
                "vswp         d27,d30             \n\t"
                
                "vst1.32 		{d24, d25}, [%2]! 	\n\t"	//q12
                "vst1.32 		{d26, d27}, [%2]!		\n\t"	//q13
                "vst1.32 		{d28, d29}, [%2] 		\n\t"	//q14
                
                : "+r"(m0), "+r"(m1), "+r"(d) :
                : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15",
                "memory"
                );
}



#endif

///----------------------------------------------------------------------
/// MatMul44
///----------------------------------------------------------------------
void RKMatrix::SetAsMultiplication(const RKMatrix& m1, const RKMatrix& m2)
{
	RKASSERT(this != &m1, "Self modification detected");
	RKASSERT(this != &m2, "Self modification detected");
#if defined(__ARM_NEON__) && !defined(__arm64__)
  RKMatrixMul_Neon(&m1, &m2, this);
#else
	m00 = m1.m00*m2.m00 + m1.m01*m2.m10 + m1.m02*m2.m20 + m1.m03*m2.m30;
	m01 = m1.m00*m2.m01 + m1.m01*m2.m11 + m1.m02*m2.m21 + m1.m03*m2.m31;
	m02 = m1.m00*m2.m02 + m1.m01*m2.m12 + m1.m02*m2.m22 + m1.m03*m2.m32;
	m03 = m1.m00*m2.m03 + m1.m01*m2.m13 + m1.m02*m2.m23 + m1.m03*m2.m33;

	m10 = m1.m10*m2.m00 + m1.m11*m2.m10 + m1.m12*m2.m20 + m1.m13*m2.m30;
	m11 = m1.m10*m2.m01 + m1.m11*m2.m11 + m1.m12*m2.m21 + m1.m13*m2.m31;
	m12 = m1.m10*m2.m02 + m1.m11*m2.m12 + m1.m12*m2.m22 + m1.m13*m2.m32;
	m13 = m1.m10*m2.m03 + m1.m11*m2.m13 + m1.m12*m2.m23 + m1.m13*m2.m33;

	m20 = m1.m20*m2.m00 + m1.m21*m2.m10 + m1.m22*m2.m20 + m1.m23*m2.m30;
	m21 = m1.m20*m2.m01 + m1.m21*m2.m11 + m1.m22*m2.m21 + m1.m23*m2.m31;
	m22 = m1.m20*m2.m02 + m1.m21*m2.m12 + m1.m22*m2.m22 + m1.m23*m2.m32;
	m23 = m1.m20*m2.m03 + m1.m21*m2.m13 + m1.m22*m2.m23 + m1.m23*m2.m33;

	m30 = m1.m30*m2.m00 + m1.m31*m2.m10 + m1.m32*m2.m20 + m1.m33*m2.m30;
	m31 = m1.m30*m2.m01 + m1.m31*m2.m11 + m1.m32*m2.m21 + m1.m33*m2.m31;
	m32 = m1.m30*m2.m02 + m1.m31*m2.m12 + m1.m32*m2.m22 + m1.m33*m2.m32;
	m33 = m1.m30*m2.m03 + m1.m31*m2.m13 + m1.m32*m2.m23 + m1.m33*m2.m33;
#endif
}


///----------------------------------------------------------------------
/// MultiplyAndTransformToPacked4x3
///----------------------------------------------------------------------
void RKMatrix::MultiplyAndTransformToPacked4x3(const RKMatrix& m1, const RKMatrix& m2, float* pOut)
{
#if defined(__ARM_NEON__) && !defined(__arm64__)
  RKMatrixMulTranspose4x3_Neon(&m1, &m2, pOut);
#else
  pOut[0] = m1.m00*m2.m00 + m1.m01*m2.m10 + m1.m02*m2.m20 + m1.m03*m2.m30;
  pOut[1] = m1.m00*m2.m01 + m1.m01*m2.m11 + m1.m02*m2.m21 + m1.m03*m2.m31;
  pOut[2] = m1.m00*m2.m02 + m1.m01*m2.m12 + m1.m02*m2.m22 + m1.m03*m2.m32;
  pOut[3] = m1.m00*m2.m03 + m1.m01*m2.m13 + m1.m02*m2.m23 + m1.m03*m2.m33;
  
  pOut[4] = m1.m10*m2.m00 + m1.m11*m2.m10 + m1.m12*m2.m20 + m1.m13*m2.m30;
  pOut[5] = m1.m10*m2.m01 + m1.m11*m2.m11 + m1.m12*m2.m21 + m1.m13*m2.m31;
  pOut[6] = m1.m10*m2.m02 + m1.m11*m2.m12 + m1.m12*m2.m22 + m1.m13*m2.m32;
  pOut[7] = m1.m10*m2.m03 + m1.m11*m2.m13 + m1.m12*m2.m23 + m1.m13*m2.m33;
  
  pOut[8] = m1.m20*m2.m00 + m1.m21*m2.m10 + m1.m22*m2.m20 + m1.m23*m2.m30;
  pOut[9] = m1.m20*m2.m01 + m1.m21*m2.m11 + m1.m22*m2.m21 + m1.m23*m2.m31;
  pOut[10] = m1.m20*m2.m02 + m1.m21*m2.m12 + m1.m22*m2.m22 + m1.m23*m2.m32;
  pOut[11] = m1.m20*m2.m03 + m1.m21*m2.m13 + m1.m22*m2.m23 + m1.m23*m2.m33;
#endif
}


///----------------------------------------------------------------------
/// DetGeneric()
///----------------------------------------------------------------------
float RKMatrix::DetGeneric() const
{
	return
		(m00*m11 - m10*m01)*(m22*m33 - m32*m23)
		-(m00*m21 - m20*m01)*(m12*m33 - m32*m13)
		+(m00*m31 - m30*m01)*(m12*m23 - m22*m13)
		+(m10*m21 - m20*m11)*(m02*m33 - m32*m03)
		-(m10*m31 - m30*m11)*(m02*m23 - m22*m03)
		+(m20*m31 - m30*m21)*(m02*m13 - m12*m03);
}

///----------------------------------------------------------------------
/// FromQuaternion()
///----------------------------------------------------------------------
void RKMatrix::FromQuaternion(const RKQuaternion& quaternion, const RKVector& translation)
{
  float qx, qy, qz, qw, qx2, qy2, qz2, qxqx2, qyqy2, qzqz2, qxqy2, qyqz2, qzqw2, qxqz2, qyqw2, qxqw2;
  qx = quaternion.x;
  qy = quaternion.y;
  qz = quaternion.z;
  qw = quaternion.w;
  qx2 = ( qx + qx );
  qy2 = ( qy + qy );
  qz2 = ( qz + qz );
  qxqx2 = ( qx * qx2 );
  qxqy2 = ( qx * qy2 );
  qxqz2 = ( qx * qz2 );
  qxqw2 = ( qw * qx2 );
  qyqy2 = ( qy * qy2 );
  qyqz2 = ( qy * qz2 );
  qyqw2 = ( qw * qy2 );
  qzqz2 = ( qz * qz2 );
  qzqw2 = ( qw * qz2 );

  Column(0).Set( ( ( 1.0f - qyqy2 ) - qzqz2 ), ( qxqy2 + qzqw2 ), ( qxqz2 - qyqw2 ) , 0);
  Column(1).Set( ( qxqy2 - qzqw2 ), ( ( 1.0f - qxqx2 ) - qzqz2 ), ( qyqz2 + qxqw2 ) , 0);
  Column(2).Set( ( qxqz2 + qyqw2 ), ( qyqz2 - qxqw2 ), ( ( 1.0f - qxqx2 ) - qyqy2 ) , 0);
  Column(3) = translation;
	Column(3).w = 1.0; // sometime people forget to initialize w component =(, easier to fix it here than to fix it everywhere else
}


///----------------------------------------------------------------------
/// Inversed()
///----------------------------------------------------------------------
bool RKMatrix::SetAsInverse(const RKMatrix& src)
{
	RKASSERT(this != &src, "Self modification detected");
	
	float d = src.DetGeneric();

	if (fabsf(d) < RKSMALLEREPSILON)
	{
		// non-invertible
		return false;
	}

	d = 1.0f / d;

	f[0] =  d * (src.m11*(src.m22*src.m33 - src.m32*src.m23) + src.m21*(src.m32*src.m13 - src.m12*src.m33) + src.m31*(src.m12*src.m23 - src.m22*src.m13));
	f[1] = 	d * (src.m12*(src.m20*src.m33 - src.m30*src.m23) + src.m22*(src.m30*src.m13 - src.m10*src.m33) + src.m32*(src.m10*src.m23 - src.m20*src.m13));
	f[2] = 	d * (src.m13*(src.m20*src.m31 - src.m30*src.m21) + src.m23*(src.m30*src.m11 - src.m10*src.m31) + src.m33*(src.m10*src.m21 - src.m20*src.m11));
	f[3] = 	d * (src.m10*(src.m31*src.m22 - src.m21*src.m32) + src.m20*(src.m11*src.m32 - src.m31*src.m12) + src.m30*(src.m21*src.m12 - src.m11*src.m22));

	f[4] = 	d * (src.m21*(src.m02*src.m33 - src.m32*src.m03) + src.m31*(src.m22*src.m03 - src.m02*src.m23) + src.m01*(src.m32*src.m23 - src.m22*src.m33));
	f[5] = 	d * (src.m22*(src.m00*src.m33 - src.m30*src.m03) + src.m32*(src.m20*src.m03 - src.m00*src.m23) + src.m02*(src.m30*src.m23 - src.m20*src.m33));
	f[6] = 	d * (src.m23*(src.m00*src.m31 - src.m30*src.m01) + src.m33*(src.m20*src.m01 - src.m00*src.m21) + src.m03*(src.m30*src.m21 - src.m20*src.m31));
	f[7] = 	d * (src.m20*(src.m31*src.m02 - src.m01*src.m32) + src.m30*(src.m01*src.m22 - src.m21*src.m02) + src.m00*(src.m21*src.m32 - src.m31*src.m22));

	f[8] = 	d * (src.m31*(src.m02*src.m13 - src.m12*src.m03) + src.m01*(src.m12*src.m33 - src.m32*src.m13) + src.m11*(src.m32*src.m03 - src.m02*src.m33));
	f[9] = 	d * (src.m32*(src.m00*src.m13 - src.m10*src.m03) + src.m02*(src.m10*src.m33 - src.m30*src.m13) + src.m12*(src.m30*src.m03 - src.m00*src.m33));
	f[10] = d * (src.m33*(src.m00*src.m11 - src.m10*src.m01) + src.m03*(src.m10*src.m31 - src.m30*src.m11) + src.m13*(src.m30*src.m01 - src.m00*src.m31));
	f[11] = d * (src.m30*(src.m11*src.m02 - src.m01*src.m12) + src.m00*(src.m31*src.m12 - src.m11*src.m32) + src.m10*(src.m01*src.m32 - src.m31*src.m02));

	f[12] = d * (src.m01*(src.m22*src.m13 - src.m12*src.m23) + src.m11*(src.m02*src.m23 - src.m22*src.m03) + src.m21*(src.m12*src.m03 - src.m02*src.m13));
	f[13] = d * (src.m02*(src.m20*src.m13 - src.m10*src.m23) + src.m12*(src.m00*src.m23 - src.m20*src.m03) + src.m22*(src.m10*src.m03 - src.m00*src.m13));
	f[14] = d * (src.m03*(src.m20*src.m11 - src.m10*src.m21) + src.m13*(src.m00*src.m21 - src.m20*src.m01) + src.m23*(src.m10*src.m01 - src.m00*src.m11));
	f[15] = d * (src.m00*(src.m11*src.m22 - src.m21*src.m12) + src.m10*(src.m21*src.m02 - src.m01*src.m22) + src.m20*(src.m01*src.m12 - src.m11*src.m02));
	
	return true;
};

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKMatrix::MakeRotationXMatrix(float angRadians)
{
	float	cosa, sina;
	SetIdentity();

	cosa = cosf(angRadians);
	sina = sinf(angRadians);

	m11 = cosa;
	m22 = cosa;
	m12 = sina;
	m21 = -sina;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKMatrix::MakeRotationYMatrix(float angRadians)
{
	float	cosa, sina;
	SetIdentity();

	cosa = cosf(angRadians);
	sina = sinf(angRadians);

	m00 = cosa;
	m22 = cosa;
	m02 = -sina;
	m20 = sina;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKMatrix::MakeRotationZMatrix(float angRadians)
{
	float	cosa, sina;
	SetIdentity();

	cosa = cosf(angRadians);
	sina = sinf(angRadians);

	m00 = cosa;
	m11 = cosa;
	m01 = sina;
	m10 = -sina;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKMatrix::MakeRotationMatrix(int axis, float angRadians)
{
	switch (axis)
	{
	case 0:
		MakeRotationXMatrix(angRadians);
		break;
	case 1:
		MakeRotationYMatrix(angRadians);
		break;
	case 3:
		MakeRotationZMatrix(angRadians);
		break;
	default:
		RKASSERT(false, "Invalid argument");
		break;
	}
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKMatrix::MakeRotationMatrix(const RKVector& axis, float angRadians)
{
	float c = cos(angRadians);
	float s = sin(angRadians);
	float t = 1.0f - c;

	float txx = t*axis.x*axis.x;
	float tyy = t*axis.y*axis.y;
	float tzz = t*axis.z*axis.z;

	float txy = t*axis.x*axis.y;
	float txz = t*axis.x*axis.z;
	float tyz = t*axis.y*axis.z;

	float sx = s*axis.x;
	float sy = s*axis.y;
	float sz = s*axis.z;

	m00 = txx + c;  m01 = txy + sz; m02 = txz - sy; m03 = 0.f;
	m10 = txy - sz; m11 = tyy + c;  m12 = tyz + sx; m13 = 0.f;
	m20 = txz + sy; m21 = tyz - sx; m22 = tzz + c;  m23 = 0.f;
	m30 = 0.f;      m31 = 0.f;      m32 = 0.f;      m33 = 1.f;
}

///----------------------------------------------------------------------
/// MakeOrientationFromDir()
///----------------------------------------------------------------------
void RKMatrix::MakeOrientationMatrix(const RKVector& dir, const RKVector& up)
{
	// Now get orientation
	RKVector z = dir;
	RKVector x = up.Cross(z);
	x.Normalize();
	RKVector y = z.Cross(x);

	// build matrix
	SetIdentity();
	SetXAxis(x);
	SetYAxis(y);
	SetZAxis(z);
}

///----------------------------------------------------------------------
/// FromLookAt
///----------------------------------------------------------------------
void RKMatrix::MakeLookAtMatrix(const RKVector& origon, const RKVector& dest, const RKVector& up)
{
	RKVector zaxis = dest - origon;
	zaxis.Normalize();

	// xaxis = up x zaxis
	RKVector xaxis = zaxis.Cross(up);
	xaxis.Normalize();

	// yaxis = zaxis x xaxis;
	RKVector yaxis = xaxis.Cross(zaxis);
	yaxis.Normalize();

	SetIdentity();

	xaxis *= -1;
	xaxis.w = 0;
	zaxis.w = 0;
	yaxis.w = 0;

	SetXAxis(xaxis);
	SetZAxis(zaxis);
	SetYAxis(yaxis);

	SetTranslation(origon);
}

///----------------------------------------------------------------------
/// SetOrtho
///----------------------------------------------------------------------
void RKMatrix::MakeOrthoMatrix(float l, float r, float b, float t, float n, float f)
{
  //openGL -1 to 1 z clip space
  //this->f[0] = 2.f / (r - l); this->f[1] = 0.f;         this->f[2] = 0.f;           this->f[3] = 0.f;
  //this->f[4] = 0.f;           this->f[5] = 2.f / (t-b); this->f[6] = 0.f;           this->f[7] = 0.f;
  //this->f[8] = 0.f;           this->f[9] = 0.f;         this->f[10]= 2.f / (f-n);  this->f[11] = 0.f;
  //this->f[12] = (l+r)/(l-r);  this->f[13] = (t+b)/(b-t);this->f[14] = -(f + n)/(f-n);          this->f[15] = 1.f;

  //d3d 0 to 1 z clip space
    this->f[0] = 2.f / (r - l); this->f[1] = 0.f;         this->f[2] = 0.f;           this->f[3] = 0.f;
    this->f[4] = 0.f;           this->f[5] = 2.f / (t-b); this->f[6] = 0.f;           this->f[7] = 0.f;
    this->f[8] = 0.f;           this->f[9] = 0.f;         this->f[10]= 1.f / (f-n);  this->f[11] = 0.f;
    this->f[12] = (l+r)/(l-r);  this->f[13] = (t+b)/(b-t);this->f[14] = -n/(f-n);          this->f[15] = 1.f;
}

void RKMatrix::MakePerspectiveMatrix(float fov, float nearplane, float farplane, float aspect)
{
  /*SetIdentity();
  float scale = 1.f / tan(fov * RKDEGREES_TO_RADIANS * 0.5f);
  m00 = m11 = scale;
  m22 = farplane / (farplane - nearplane);
  m23 = -farplane * nearplane / (farplane - nearplane);
  m32 = 1;
  m33 = 0;*/
  float r = nearplane  * tan(fov * 0.5f);
  float l = -r;
  float t = r * aspect;
  float b = -t;
  MakePerspectiveMatrix(l, r, t, b, nearplane, farplane);
}

void RKMatrix::MakePerspectiveMatrix(float l, float r, float t, float b, float n, float f)
{
  SetIdentity();
  m00 = 2 * n / (r - l);
  m11 = 2 * n / (t - b);
  m02 = -(r + l) / (r - l);
  m12 = -(t + b) / (t - b);
  m22 = (f + n) / (f - n);
  m32 = 1;
  m23 = -(2 * f * n) / (f - n);
  m33 = 0;
}



///----------------------------------------------------------------------
///----------------------------------------------------------------------
RKVector RKMatrix::TransformDir(const RKVector& v) const
{
	RKVector res;

	res.x = (m00*v.x)+(m01*v.y)+(m02*v.z);
	res.y = (m10*v.x)+(m11*v.y)+(m12*v.z);
	res.z = (m20*v.x)+(m21*v.y)+(m22*v.z);
	res.w = 1.f;

	return res;
}

RKMatrix RKMatrix::GetMatrixUsingRowMajorConvention( const float& m11, const float& m12, const float& m13, const float& m14, 
                                                     const float& m21, const float& m22, const float& m23, const float& m24, 
                                                     const float& m31, const float& m32, const float& m33, const float& m34, 
                                                     const float& m41, const float& m42, const float& m43, const float& m44
                                                   )
{
  RKMatrix new_mat = {
                       m11, m21, m31, m41,
                       m12, m22, m32, m42,
                       m13, m23, m33, m43,
                       m14, m24, m34, m44
                     };

  return new_mat;
}

float RKMatrix::GetDeterminant( const RKMatrix& mat )
{
  // change the storing ordering to row major
  RKMatrix mat_stored_in_row_major = {
                                       mat.m00, mat.m01, mat.m02, mat.m03,
                                       mat.m10, mat.m11, mat.m12, mat.m13,
                                       mat.m20, mat.m21, mat.m22, mat.m23,
                                       mat.m30, mat.m31, mat.m32, mat.m33
                                     };
  float a0 = mat_stored_in_row_major.f[ 0]*mat_stored_in_row_major.f[ 5] - mat_stored_in_row_major.f[ 1]*mat_stored_in_row_major.f[ 4];
  float a1 = mat_stored_in_row_major.f[ 0]*mat_stored_in_row_major.f[ 6] - mat_stored_in_row_major.f[ 2]*mat_stored_in_row_major.f[ 4];
  float a2 = mat_stored_in_row_major.f[ 0]*mat_stored_in_row_major.f[ 7] - mat_stored_in_row_major.f[ 3]*mat_stored_in_row_major.f[ 4];
  float a3 = mat_stored_in_row_major.f[ 1]*mat_stored_in_row_major.f[ 6] - mat_stored_in_row_major.f[ 2]*mat_stored_in_row_major.f[ 5];
  float a4 = mat_stored_in_row_major.f[ 1]*mat_stored_in_row_major.f[ 7] - mat_stored_in_row_major.f[ 3]*mat_stored_in_row_major.f[ 5];
  float a5 = mat_stored_in_row_major.f[ 2]*mat_stored_in_row_major.f[ 7] - mat_stored_in_row_major.f[ 3]*mat_stored_in_row_major.f[ 6];
  float b0 = mat_stored_in_row_major.f[ 8]*mat_stored_in_row_major.f[13] - mat_stored_in_row_major.f[ 9]*mat_stored_in_row_major.f[12];
  float b1 = mat_stored_in_row_major.f[ 8]*mat_stored_in_row_major.f[14] - mat_stored_in_row_major.f[10]*mat_stored_in_row_major.f[12];
  float b2 = mat_stored_in_row_major.f[ 8]*mat_stored_in_row_major.f[15] - mat_stored_in_row_major.f[11]*mat_stored_in_row_major.f[12];
  float b3 = mat_stored_in_row_major.f[ 9]*mat_stored_in_row_major.f[14] - mat_stored_in_row_major.f[10]*mat_stored_in_row_major.f[13];
  float b4 = mat_stored_in_row_major.f[ 9]*mat_stored_in_row_major.f[15] - mat_stored_in_row_major.f[11]*mat_stored_in_row_major.f[13];
  float b5 = mat_stored_in_row_major.f[10]*mat_stored_in_row_major.f[15] - mat_stored_in_row_major.f[11]*mat_stored_in_row_major.f[14];

  float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;

  return det;
}

void RKMatrix::SetValuesUsingRowMajorConvention( const float& _m11, const float& _m12, const float& _m13, const float& _m14, 
                                                 const float& _m21, const float& _m22, const float& _m23, const float& _m24, 
                                                 const float& _m31, const float& _m32, const float& _m33, const float& _m34, 
                                                 const float& _m41, const float& _m42, const float& _m43, const float& _m44 )
{
  m00 = _m11; m01 = _m12; m02 = _m13; m03 = _m14; 
  m10 = _m21; m11 = _m22; m12 = _m23; m13 = _m24; 
  m20 = _m31; m21 = _m32; m22 = _m33; m23 = _m34; 
  m30 = _m41; m31 = _m42; m32 = _m43; m33 = _m44; 
}

RKMatrix RKMatrix::GetTensorMatrix( const RKVector& vec0, const RKVector& vec1 )
{
  return RKMatrix::GetMatrixUsingRowMajorConvention( vec0.x * vec1.x, vec0.x * vec1.y, vec0.x * vec1.z, vec0.x * vec1.w,
                                                      vec0.y * vec1.x, vec0.y * vec1.y, vec0.y * vec1.z, vec0.y * vec1.w,
                                                      vec0.z * vec1.x, vec0.z * vec1.y, vec0.z * vec1.z, vec0.z * vec1.w,
                                                      vec0.w * vec1.x, vec0.w * vec1.y, vec0.w * vec1.z, vec0.w * vec1.w
                                                    );
}

RKMatrix RKMatrix::operator*( const float scale_factor )
{
  return RKMatrix::GetMatrixUsingRowMajorConvention( m00 * scale_factor, m01 * scale_factor, m02 * scale_factor, m03 * scale_factor,
                                                      m10 * scale_factor, m11 * scale_factor, m12 * scale_factor, m13 * scale_factor,
                                                      m20 * scale_factor, m21 * scale_factor, m22 * scale_factor, m23 * scale_factor,
                                                      m30 * scale_factor, m31 * scale_factor, m32 * scale_factor, m33 * scale_factor
                                                   );
}

RKMatrix& RKMatrix::operator*=( const float scale_factor )
{
  SetValuesUsingRowMajorConvention( m00 * scale_factor, m01 * scale_factor, m02 * scale_factor, m03 * scale_factor,
                                    m10 * scale_factor, m11 * scale_factor, m12 * scale_factor, m13 * scale_factor,
                                    m20 * scale_factor, m21 * scale_factor, m22 * scale_factor, m23 * scale_factor,
                                    m30 * scale_factor, m31 * scale_factor, m32 * scale_factor, m33 * scale_factor
                                  );

  return *this;
}

//
RKMatrix2 RKMatrix2::MakeAMatrix( const RKMatrix& mat4x4 )
{
  return RKMatrix2(mat4x4.m00, mat4x4.m01,
                    mat4x4.m10, mat4x4.m11
                   );
}

float RKMatrix2::GetTheDeterminant() const
{
  return _m11 * _m22 - _m12 * _m21;
}

RKMatrix2::RKMatrix2( const float& m11, const float& m12, 
                      const float& m21, const float& m22 
                    )
:
  _m11(m11), _m12(m12),
  _m21(m21), _m22(m22)
{

}

  float RKMatrix2::GetItsDeterminant( const RKMatrix2& mat2x2 )
  {
    return mat2x2.GetTheDeterminant();
  }

RKMatrix3 RKMatrix3::MakeAMatrix( const RKMatrix& mat4x4 )
{
  return RKMatrix3(mat4x4.m00, mat4x4.m01, mat4x4.m02,
                    mat4x4.m10, mat4x4.m11, mat4x4.m12,
                    mat4x4.m20, mat4x4.m21, mat4x4.m22
                   );
}

float RKMatrix3::GetTheDeterminant() const
{
  float a = _m22 * _m33 - _m23 * _m32;
  float b = _m23 * _m31 - _m21 * _m33;
  float c = _m21 * _m32 - _m22 * _m31;

  return _m11 * a + _m12 * b + _m13 * c;
}

RKMatrix3::RKMatrix3( const float& m11, const float& m12, const float& m13, 
                      const float& m21, const float& m22, const float& m23, 
                      const float& m31, const float& m32, const float& m33 
                    )
:
  _m11(m11), _m12(m12), _m13(m13),
  _m21(m21), _m22(m22), _m23(m23),
  _m31(m31), _m32(m32), _m33(m33)
{

}

  float RKMatrix3::GetItsDeterminant( const RKMatrix3& mat3x3 )
  {
    return mat3x3.GetTheDeterminant();
  }

}