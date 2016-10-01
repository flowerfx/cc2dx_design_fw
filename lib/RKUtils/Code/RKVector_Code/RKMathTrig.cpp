#include "RKMathTrig.h"
#include <math.h>
#include "RKVector.h"
#include "../Common/RKTypes.h"
namespace RKUtils
{
	// these functions left as a reference
	// that's basicly how neon version works

	// expects x to be in range [0, pi]
	// see http://devmaster.net/forums/topic/4648-fast-and-accurate-sinecosine/ for details
	// this version is modified to avoid fabsf calls by storing sign bit separatedly
	// and working with positive values only
	static inline float _FastSin(float x)
	{
		const float B = 4 / RKPI;
		const float C = -4 / (RKPI * RKPI);

		float y = B * x + C * x * x;

		// this block adds some precision, can be removed in favor of speed
		{
			//  const float Q = 0.775;
			const float P = 0.225f;

			y = P * (y * y - y) + y;   // Q * y + P * y * abs(y)
		}

		return y;
	}

	static inline float _Sin(float x)
	{
		// storing sign bit and making angle positive
		uint32 signBit = *((uint32*)&x) & 0x80000000;
		x = fabsf(x);

		// converting angle into [0, pi] range
		// if angle more than pi, then toggling stored sign bit
		int periodCount = int(x / RKPI);
		signBit ^= periodCount << 31;
		x -= periodCount * RKPI;

		// calculating fast sine
		float y = _FastSin(x);

		// reapplying sign bit
		*((uint32*)&y) ^= signBit;

		return y;
	}

	static inline float _Cos(float x)
	{
		return _Sin(x + RKPI / 2);
	}

#if !defined (__ARM_NEON__) || defined (__arm64__)
	// returns {sin, cos}
	void RKMath_SinCos(float angle, RKVector2& results)
	{
		results.x = _Sin(angle);
		results.y = _Cos(angle);
	}

	// returns {sin1, cos1, sin2, cos2}
	void RKMath_SinCos2(const RKVector2& angles, RKVector& results)
	{
		results.x = _Sin(angles.x);
		results.y = _Cos(angles.x);
		results.z = _Sin(angles.y);
		results.w = _Cos(angles.y);
	}
#endif // else see RKMathTrig_NEON
}