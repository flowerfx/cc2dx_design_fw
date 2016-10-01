//************************************************************************************************************************************
//*
//* RKMath.cpp
//*
//************************************************************************************************************************************

#include "../Common/RK.h"
#include "../Common/RKTypes.h"
#include "RKMath.h"
#include "RKMatrix.h"
namespace RKUtils
{

	static char *sRKMath_LastInvalidF32Desc = NULL;

	//-----------------------------------------------------------------------------------------------------------
	bool RKMath_IsValidFloat(float value_f32)
		//-----------------------------------------------------------------------------------------------------------
	{
		/*
		 IBM : IEEE   -  http://publib.boulder.ibm.com/infocenter/macxhelp/v6v81/index.jsp?topic=/com.ibm.xlf81m.doc/pgs/ug34.htm
		 -- float (32 bits):
		 Positive infinity is represented by the bit pattern X'7F80 0000'.
		 Negative infinity is represented by the bit pattern X'FF80 0000'.
		 A signaling NaN is represented by any bit pattern between X'7F80 0001' and X'7FBF FFFF' or between X'FF80 0001' and X'FFBF FFFF'.
		 A quiet NaN is represented by any bit pattern between X'7FC0 0000' and X'7FFF FFFF' or between X'FFC0 0000' and X'FFFF FFFF'.
		 -- double (64 bits) ---
		 Positive infinity is represented by the bit pattern X'7FF00000 00000000'.
		 Negative infinity is represented by the bit pattern X'FFF00000 00000000'.
		 A signaling NaN is represented by any bit pattern between X'7FF00000 00000001' and X'7FF7FFFF FFFFFFFF' or between X'FFF00000 00000001' and X'FFF7FFFF FFFFFFFF'.
		 A quiet NaN is represented by any bit pattern between X'7FF80000 00000000' and X'7FFFFFFF FFFFFFFF' or between X'FFF80000 00000000' and X'FFFFFFFF FFFFFFFF'.

		 */
		typedef struct
		{
			union{
				float valf32;
				rku32   valu32;
			};
		}stf32iv;

		stf32iv V;
		V.valf32 = value_f32;
		rku32 vU32 = V.valu32;

		if (vU32 == 0x7F800000)		//Positive infinity
		{
			sRKMath_LastInvalidF32Desc = "+INF";
			return false;
		}
		if (vU32 == 0xFF800000)		//Negative infinity
		{
			sRKMath_LastInvalidF32Desc = "-INF";
			return false;
		}
		if ((vU32 >= 0x7F800001 && vU32 <= 0x7FBFFFFF) || (vU32 >= 0xFF800001 && vU32 <= 0xFFBFFFFF))		//A signaling NaN
		{
			sRKMath_LastInvalidF32Desc = "NAN";
			return false;
		}
		if ((vU32 >= 0x7FC00000 && vU32 <= 0x7FFFFFFF) || (vU32 >= 0xFFC00000 && vU32 <= 0xFFFFFFFF))		//A quiet NaN
		{
			sRKMath_LastInvalidF32Desc = "QNAN";
			return false;
		}

		sRKMath_LastInvalidF32Desc = NULL;
		return true;
	}

	//-----------------------------------------------------------------------------------------------------------
	char *RKMath_GetLastInvalidFloatDesc(void)
		//-----------------------------------------------------------------------------------------------------------
	{
		//after call RKMath_IsValidFloat, return "+INF", "-INF", "NAN", "QNAN" or NULL
		return sRKMath_LastInvalidF32Desc;
	}


	//returns number of real roots
	int QuadraticSolver(float A, float B, float C, float& x1, float& x2)
	{
		float rad = B * B - 4.0f * A * C;
		if (rad > RKEPSILON)
		{
			float sqrad = sqrtf(rad);
			x1 = (-B + sqrad) / (2.0f * A);
			x2 = (-B - sqrad) / (2.0f * A);
			return 2;
		}
		else if (fabsf(rad) < RKEPSILON)
		{
			x1 = -B / (2.0f * A);
			return 1;
		}
		return 0;
	}



	//returns number of real roots
	int CubicSolver(float A, float B, float C, float D, float& x1, float& x2, float& x3)
	{
		if (fabsf(A) < RKEPSILON) //if A == 0, it's a quadratic equation
		{
			return QuadraticSolver(B, C, D, x1, x2);
		}
		float B2 = B * B;
		float A2 = A * A;
		float B3 = B2 * B;
		float A3 = A2 * A;
		float f = (3.0f * C / A - (B2 / A2)) / (3.0f);
		float g = ((2.0f * B3 / A3) - (9.0f * B * C / A2) + (27.0f * D / A)) / (27.0f);
		float G2 = g * g;
		float h = G2 * .25f + f * f * f / 27.0f;

		if (h > 0.0f) //one real root
		{
			float sqtH = sqrtf(h);
			float r = -g * .5f + sqtH;
			float s;
			if (r < 0.0f)
			{
				s = -pow(-r, 1.0f / 3.0f);
			}
			else
			{
				s = pow(r, 1.0f / 3.0f);
			}
			float t = -g * .5f - sqtH;
			float u;
			if (t < 0.0f)
			{
				u = -pow(-t, 1.0f / 3.0f);
			}
			else
			{
				u = pow(t, 1.0f / 3.0f);
			}
			x1 = s + u - (B / (3.0f * A));
			return 1;
		}
		else if (fabsf(h) < RKEPSILON  && fabsf(g) < RKEPSILON && fabsf(f) < RKEPSILON) //h == g == f == 0, all 3 roots the same
		{
			x1 = -pow(D / A, 1.0f / 3.0f);
			return 1;
		}
		else //3 real roots
		{
			float i = sqrtf(G2 * .25f - h);
			float j;
			if (i < 0.0f)
			{
				j = -pow(-i, 1.0f / 3.0f);
			}
			else
			{
				j = pow(i, 1.0f / 3.0f);
			}
			float k = acosf(-(g / (2.0f * i)));
			float l = -j;
			float m = cosf(k / 3.0f);
			float n = RKSQRT_THREE * sinf(k / 3.0f);
			float p = -(B / (3.0f * A));
			x1 = 2 * j * cosf(k / 3.0f) - (B / (3.0f * A));
			x2 = l * (m + n) + p;
			x3 = l * (m - n) + p;
			return 3;
		}
	}
}