#pragma once

#include "RKVector.h"
namespace RKUtils
{
	inline float RKMath_Sin(float angle) { return sinf(angle); } // sinf/cosf are still faster on ios, because they use fixed point math inside
	inline float RKMath_Cos(float angle) { return cosf(angle); } // unfortunately I don't have enough time to port algorithm to fixed point

	void RKMath_SinCos(float angle, RKVector2& results);             // returns {sin, cos} in results
	void RKMath_SinCos2(const RKVector2& angles, RKVector& results); // returns {sin1, cos1, sin2, cos2} in results

	// if you need these, let me know
	//inline RKVector2 RKMath_Sin2(const RKVector2& angles);
	//inline RKVector2 RKMath_Cos2(const RKVector2& angles);
	//inline RKVector4 RKMath_Sin4(const RKVector& angles);
	//inline RKVector4 RKMath_Cos4(const RKVector& angles);
}