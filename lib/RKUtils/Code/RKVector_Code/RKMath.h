		///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKMath.h
///
/// Description :	Math helper functions
///----------------------------------------------------------------------
#ifndef RKMATH_H
#define RKMATH_H

#include "RKVector.h"
namespace RKUtils
{
	template <class T> int RKMath_Sign(const T& value)
	{
		if (value > 0)
			return 1;

		if (value < 0)
			return -1;

		return 0;
	}

	/*
	inline float deg2rad(float angle)
	{
	return angle * PI / 180.f;
	}

	inline float rad2deg(float angle)
	{
	return angle * 180.f / PI;
	}
	*/

	//Return a random Number between 1 and 0
	inline float RKMath_RandFloat()
	{
		return (float)rand() / (float)(RAND_MAX);
	}

	// Return a random float in the range of [min, max)
	// Works on Win32, Win64 and iOS. On other platforms check value of RAND_MAX.
	inline float RKMath_RandRange(float min, float max)
	{
		static uint32 rkmath_max = (uint32)RAND_MAX + 1;
		float randfloat = (float)rand() / (float)(rkmath_max);
		return (randfloat * (max - min)) + min;
	}

	// Returns a random int in the range of [min,max)
	inline int RKMath_RandRangeInt(int min, int max)
	{
		return (rand() % (max - min)) + min;
	}

	//--- check for NAN, QNAN, +INF, -INF, .. use RKMath_GetLastInvalidFloatDesc() for details
	bool RKMath_IsValidFloat(float value_f32);
	char *RKMath_GetLastInvalidFloatDesc(void);

	//returns number of real roots
	int QuadraticSolver(float A, float B, float C, float& x1, float& x2);

	//returns the number of real roots
	int CubicSolver(float A, float B, float C, float D, float& x1, float& x2, float& x3);
}
#endif // RKMATH_H
