///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	quaternion.cpp
///
/// Description :	Implementation of a Lightweight Quaternion Class
///----------------------------------------------------------------------
#include "../Common/RK.h"
#include "RKQuaternion.h"
#include "RKMatrix.h"
namespace RKUtils
{
const RKQuaternion RKQuaternion::Identity( 0.0f, 0.0f, 0.0f, 1.0f );
const RKQuaternion RKQuaternion::Invalid ( 0.0f, 0.0f, 0.0f, 0.0f );

///----------------------------------------------------------------------
/// 
///----------------------------------------------------------------------

void RKQuaternion::FromLookAt(const RKVector& origon, const RKVector& dest)
{
  RKMatrix matOrientation;
  matOrientation.MakeLookAtMatrix(origon, dest, RKVector::Up);
  FromMatrix(matOrientation);
}

float norm( const RKQuaternion* quat )
{
  float result;
  result = ( quat->x * quat->x );
  result = ( result + ( quat->y * quat->y ) );
  result = ( result + ( quat->z * quat->z ) );
  result = ( result + ( quat->w * quat->w ) );
  return result;
}

///----------------------------------------------------------------------
/// 
///----------------------------------------------------------------------
#if defined(__ARM_NEON__) && !defined(__arm64__)
void RKQuaternion::Normalize(void)
{
  float* pThis = (float*) this;
	asm volatile (
                "vld1.32 		{d4, d5}, [%0]			\n\t"	//d2={x0,y0}, d3={z0, w0}
                "vmul.f32 		d0, d4, d4				\n\t"	//d0= d4*d4
                "vmla.f32 		d0, d5, d5				\n\t"	//d0 = d0 + d5*d5
                "vpadd.f32 		d0, d0					\n\t"	//d0 = d[0] + d[1]
                
                "vmov.f32 		d1, d0					\n\t"	//d1 = d0
                "vrsqrte.f32 	d0, d0					\n\t"	//d0 = ~ 1.0 / sqrt(d0)
                "vmul.f32 		d2, d0, d1				\n\t"	//d2 = d0 * d1
                "vrsqrts.f32 	d3, d2, d0				\n\t"	//d3 = (3 - d0 * d2) / 2
                "vmul.f32 		d0, d0, d3				\n\t"	//d0 = d0 * d3
                "vmul.f32 		d2, d0, d1				\n\t"	//d2 = d0 * d1
                "vrsqrts.f32 	d3, d2, d0				\n\t"	//d4 = (3 - d0 * d3) / 2
                "vmul.f32 		d0, d0, d3				\n\t"	//d0 = d0 * d4
                
                "vmul.f32 		q2, q2, d0[0]			\n\t"	//d0= d2*d4
                "vst1.32 		{d4, d5}, [%0]			\n\t"	//d2={x0,y0}, d3={z0, w0}
                
                :: "r"(pThis)
                : "d0", "d1", "d2", "d3", "d4", "d5", "memory"
                );
}
#else
void RKQuaternion::Normalize(void)
{
  float lenSqr, lenInv;
  lenSqr = norm( this );
  RKWarning(lenSqr!=0, "Normalize(): bad RKQuaternion!, lenSqr is 0! .. will create INF/ NAN floats..");
  lenInv = ( 1.0f / sqrtf( lenSqr ) );
  x *= lenInv;
  y *= lenInv;
  z *= lenInv;
  w *= lenInv;
}
#endif

///----------------------------------------------------------------------
/// 
///----------------------------------------------------------------------
void RKQuaternion::FromEuler(float pitch, float yaw, float roll)
{
	float cr, cp, cy, sr, sp, sy, cpcy, spsy;

	sr = (float)sinf(pitch*0.5f);
	cr = (float)cosf(pitch*0.5f);

	sp = (float)sinf(yaw*0.5f);
	cp = (float)cosf(yaw*0.5f);

	sy = (float)sinf(roll*0.5f);
	cy = (float)cosf(roll*0.5f);

	cpcy = cp*cy;
	spsy = sp*sy;

	w = cr*cpcy + sr*spsy;
	x = sr*cpcy - cr*spsy;
	y = cr*sp*cy + sr*cp*sy;
	z = cr*cp*sy - sr*sp*cy;
}

///----------------------------------------------------------------------
/// 
///----------------------------------------------------------------------
void RKQuaternion::FromYaw(float radians)
{
	w = (float)cosf(radians*0.5f);
	x = 0.f;
	y = (float)sinf(radians*0.5f);
	z = 0.f;
}

///----------------------------------------------------------------------
/// 
///----------------------------------------------------------------------
void RKQuaternion::FromPitch(float radians)
{
	w = (float)cosf(radians*0.5f);
	x = (float)sinf(radians*0.5f);
	y = 0.f;
	z = 0.f;
}

///----------------------------------------------------------------------
/// 
///----------------------------------------------------------------------
void RKQuaternion::FromRoll(float radians)
{
	w = (float)cosf(radians*0.5f);
	x = 0.f;
	y = 0.f;
	z = (float)sinf(radians*0.5f);
}

///----------------------------------------------------------------------
/// 
///----------------------------------------------------------------------
void RKQuaternion::FromMatrix(const RKMatrix& m)
{
  float trace, radicand, scale, xx, yx, zx, xy, yy, zy, xz, yz, zz, tmpx, tmpy, tmpz, tmpw, qx, qy, qz, qw;
  int negTrace, ZgtX, ZgtY, YgtX;
  int largestXorY, largestYorZ, largestZorX;

  xx = m.Column(0).x; // m.getCol0().getX();
  yx = m.Column(0).y; // m.getCol0().getY();
  zx = m.Column(0).z; // m.getCol0().getZ();
  xy = m.Column(1).x; // m.getCol1().getX();
  yy = m.Column(1).y; // m.getCol1().getY();
  zy = m.Column(1).z; // m.getCol1().getZ();
  xz = m.Column(2).x; // m.getCol2().getX();
  yz = m.Column(2).y; // m.getCol2().getY();
  zz = m.Column(2).z; // m.getCol2().getZ();

  trace = ( ( xx + yy ) + zz );

  negTrace = ( trace < 0.0f );
  ZgtX = zz > xx;
  ZgtY = zz > yy;
  YgtX = yy > xx;
  largestXorY = ( !ZgtX || !ZgtY ) && negTrace;
  largestYorZ = ( YgtX || ZgtX ) && negTrace;
  largestZorX = ( ZgtY || !YgtX ) && negTrace;

  if ( largestXorY )
  {
    zz = -zz;
    xy = -xy;
  }
  if ( largestYorZ )
  {
    xx = -xx;
    yz = -yz;
  }
  if ( largestZorX )
  {
    yy = -yy;
    zx = -zx;
  }

  radicand = ( ( ( xx + yy ) + zz ) + 1.0f );
  scale = ( 0.5f * ( 1.0f / sqrtf( radicand ) ) );

  tmpx = ( ( zy - yz ) * scale );
  tmpy = ( ( xz - zx ) * scale );
  tmpz = ( ( yx - xy ) * scale );
  tmpw = ( radicand * scale );
  qx = tmpx;
  qy = tmpy;
  qz = tmpz;
  qw = tmpw;

  if ( largestXorY )
  {
    qx = tmpw;
    qy = tmpz;
    qz = tmpy;
    qw = tmpx;
  }
  if ( largestYorZ )
  {
    tmpx = qx;
    tmpz = qz;
    qx = qy;
    qy = tmpx;
    qz = qw;
    qw = tmpz;
  }

  x = qx;
  y = qy;
  z = qz;
  w = qw;
}

///----------------------------------------------------------------------
/// Dot
///----------------------------------------------------------------------
#if defined(__ARM_NEON__) && !defined(__arm64__)
float RKQuaternion_Dot(float* v0, float* v1)
{
	asm volatile (
                "vld1.32 		{d2, d3}, [%0]			\n\t"	//d2={x0,y0}, d3={z0, w0}
                "vld1.32 		{d4, d5}, [%1]			\n\t"	//d4={x1,y1}, d5={z1, w1}
                "vmul.f32 		d0, d2, d4				\n\t"	//d0= d2*d4
                "vmla.f32 		d0, d3, d5				\n\t"	//d0 = d0 + d3*d5
                "vpadd.f32 		d0, d0					\n\t"	//d0 = d[0] + d[1]
                :: "r"(v0), "r"(v1) : 
                );	
}
#endif

#if defined(__ARM_NEON__) && !defined(__arm64__)
__attribute__ ((noinline)) // inlining breaks assembly computation
#endif
float RKQuaternion::Dot(const RKQuaternion& q) const
{
#if defined(__ARM_NEON__) && !defined(__arm64__)
  RKQuaternion_Dot((float*) this, (float*) &q);
  asm volatile ("vmov.f32 r0, s0 		\n\t");
#else
	return x * q.x + y * q.y + z * q.z + w * q.w;
#endif
}

///----------------------------------------------------------------------
/// LengthSqr
///----------------------------------------------------------------------
float RKQuaternion::LengthSqr() const
{
	return Dot(*this);
}

///----------------------------------------------------------------------
/// Length
///----------------------------------------------------------------------
float RKQuaternion::Length() const
{
	return sqrtf(LengthSqr());
}

///----------------------------------------------------------------------
/// Angle
///----------------------------------------------------------------------
float RKQuaternion::Angle(const RKQuaternion& q)
{
	float s = sqrtf(LengthSqr() * q.LengthSqr());
	return acos(Dot(q) / s);
}

///----------------------------------------------------------------------
/// Lerp
///----------------------------------------------------------------------
void RKQuaternion::Lerp(const RKQuaternion& source, const RKQuaternion& dest, float t)
{
  x = source.x + ((dest.x - source.x) * t);
  y = source.y + ((dest.y - source.y) * t);
  z = source.z + ((dest.z - source.z) * t);
  w = source.w + ((dest.w - source.w) * t);
}

///----------------------------------------------------------------------
/// Slerp
///----------------------------------------------------------------------
void RKQuaternion::Slerp(const RKQuaternion& source, const RKQuaternion& dest, float t)
{
  float recipSinAngle, scale0, scale1, cosAngle, angle;
  cosAngle = source.Dot(dest);
  if (cosAngle < 0.0f)
  {
    cosAngle = -cosAngle;

    x = -source.x;
    y = -source.y;
    z = -source.z;
    w = -source.w;
  }
  else
  {
    *this = source;
  }

  if (cosAngle < 0.999f) // _VECTORMATH_SLERP_TOL )
  {
    angle = acosf(cosAngle);
    recipSinAngle = (1.0f / sinf(angle));
    scale0 = (sinf(((1.0f - t) * angle)) * recipSinAngle);
    scale1 = (sinf((t * angle)) * recipSinAngle);
  }
  else
  {
    scale0 = (1.0f - t);
    scale1 = t;
  }
  
  x = (x * scale0) + (dest.x * scale1);
  y = (y * scale0) + (dest.y * scale1);
  z = (z * scale0) + (dest.z * scale1);
  w = (w * scale0) + (dest.w * scale1);
}

///----------------------------------------------------------------------
/// NLerp
///----------------------------------------------------------------------
#if defined(__ARM_NEON__) && !defined(__arm64__)
void NeonQLerp(const RKQuaternion* const source, const RKQuaternion* const dest, RKQuaternion* pOut, float t)
{
  float tVal = t;
  float one = 1.0f;
	asm volatile (
                "vdup.32   q3, %4            \n\t" //put 1.0 in q3
                "vdup.32    q9, %3            \n\t" //t into q4
                "vld1.32 		{d2, d3}, [%0]			\n\t"	//d2={x0,y0}, d3={z0, w0}
                "vld1.32 		{d4, d5}, [%1]			\n\t"	//d4={x1,y1}, d5={z1, w1}
                "vsub.f32   q3, q3, q9          \n\t" //b = 1.0 - t
                "vmul.f32 		d0, d2, d4				\n\t"	//d0= d2*d4
                "vmla.f32 		d0, d3, d5				\n\t"	//d0 = d0 + d3*d5
                "vpadd.f32 		d0, d0					\n\t"	//d0 = d[0] + d[1]
                                                    //s0 now tells us which way to interpolate
                "vmul.f32   q1, q1, q3        \n\t" //b*source
                "vmov.f32   q4, q1            \n\t" //copy b*source
                "vmla.f32   q1, q2, q9        \n\t" //b*source + t*dest
                "vmls.f32   q4, q2, q9        \n\t" //b*source - t*dest
                                              //now mask out by multiplying each by 1 or 0, then add the two
                "vdup.32    q0, d0[0]         \n\t" //put our cosAngle into q0
                "vcge.f32   q2, q0, #0        \n\t" //if the dot product is >= 0, q2 is all 1s
                "vdup.32    q3, d4[0]         \n\t" //copy into q3
                "vmvn       q3, q3            \n\t" //now, either q3 is all 0s and q2 all 1s or vice versa
                "vand.32    q1, q1, q2        \n\t" //if cosAngle >= 0, use q1
                "vand.32    q4, q4, q3        \n\t" //otherwise q4 is correct
                "vadd.f32   q0, q1, q4        \n\t" //either q1 or q4 is correct, and the other should now be 0
                                                    //q0 is now our lerp'd quaternion.  Now we need to normalize it

                "vst1.32 		{d0, d1}, [%2]	 	\n\t"
                : 
                : "r"(source), "r"(dest), "r"(pOut), "r"(tVal), "r"(one)
                : "d0", "d1", "d2", "d3", "d4", "d18","d5", "d6", "d7", "d8", "d9", "d19", "memory"
                );
}
#endif
void RKQuaternion::NLerp(const RKQuaternion& source, const RKQuaternion& dest, float t)
{
#if defined(__ARM_NEON__) && !defined(__arm64__)

  NeonQLerp(&source, &dest, this, t);
  
#else
  float cosAngle = source.x * dest.x + source.y * dest.y + source.z * dest.z + source.w * dest.w;
  float b = 1.f - t;

  if (cosAngle < 0.f)
  {
    x = b*source.x + t*-dest.x;
    y = b*source.y + t*-dest.y;
    z = b*source.z + t*-dest.z;
    w = b*source.w + t*-dest.w;
  }
  else
  {
    x = b*source.x + t*dest.x;
    y = b*source.y + t*dest.y;
    z = b*source.z + t*dest.z;
    w = b*source.w + t*dest.w;
  }
#endif
  Normalize();

}
///-------------------------------------------------------------------------
/// FromAxisAngle
///-------------------------------------------------------------------------
void  RKQuaternion::FromAxisAngle(const RKVector& axis, const float radians)
{
	float result = sinf( radians * 0.5f );

	// Calculate the x, y and z of the quaternion
	x = axis.x * result;
	y = axis.y * result;
	z = axis.z * result;

	// Calcualte the w value by cos( theta / 2 )
	w = cosf( radians * 0.5f );

	Normalize();
}

void RKQuaternion::Rotate( RKVector& vec ) const
{
  RKMatrix m;
  m.FromQuaternion( *this, RKVector::Zero );

  vec = m.TransformDir(vec);
}
}