/*
 *  RKUtil.cpp
 *  miPet
 *
 *  Created by Daniel Stephens on 23/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "RKUtil.h"
#include "../RKVector_Code/RKVector.h"
#include "RKMacros.h"
#include <stdlib.h>
#if defined OS_W8
#include "../../../../../cocos2dx-lib/external/winrt_8.1-specific/zlib/include/zlib.h"
#elif defined OS_WP8
#include "../../../../../cocos2dx-lib/external/wp_8.1-specific/zlib/include/zlib.h"
#elif defined OS_W10
#include "../../../../../cocos2dx-lib/external/win10-specific/zlib/include/zlib.h"
#elif defined WIN32
#include "../../../../cocos2dx-lib/external/win32-specific/zlib/include/zlib.h"
#else
#include "../../../../cocos2dx-lib/external/zlib/include/zlib.h"
#endif
namespace RKUtils
{
	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	float RKUtil_ComputeBoxVolume(const RKVector& extents)
	{
		return extents.x * extents.y * extents.z;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	float RKUtil_ComputeSphereVolume(float radius)
	{
		return (4.f / 3.f) * RKPI * radius * radius * radius;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	float RKUtil_ComputeCylinderVolume(float radius, float height)
	{
		return RKPI * radius * radius * height;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	float RKUtil_ComputeCapsuleVolume(float radius, float height)
	{
		return RKUtil_ComputeCylinderVolume(radius, height) + RKUtil_ComputeSphereVolume(radius);
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	float RKUtil_ComputeConeVolume(float radius, float height)
	{
		return (1.f / 3.f) * RKPI * radius * radius * height;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	void RKUtil_SetRandomSeed(uint32 seed)
	{
		srand(seed);
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	// Returns a random int in the rannge of min,max (inclusive)
	int RKUtil_RandomInt(int min, int max)
	{
		return rand() % max + min;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	// The HSL (Hue, Saturation, Luminance) model
	RKVector RKUtil_RGBtoHSL(const RKVector& rgb)
	{
		float r = rgb.r * 255.f;
		float g = rgb.g * 255.f;
		float b = rgb.b * 255.f;

		float minRGB = RKMIN(RKMIN(r, g), b); // minRGB := Min(Min(rgb.Red, rgb.Green), rgb.Blue) ;
		float maxRGB = RKMAX(RKMAX(r, g), b); // maxRGB := Max(Max(rgb.Red, rgb.Green), rgb.Blue) ;

		float delta = maxRGB - minRGB; // delta := ( maxRGB - minRGB ) ;
		float brightness = (minRGB + maxRGB) / 2; // maxRGB; // b := maxRGB ;

		float saturation = 0.f;
		if (maxRGB != 0)
		{
			saturation = 255.f * delta / maxRGB;
		}

		float hue = -1.f;
		if (saturation != 0)
		{
			if (r == maxRGB)
			{
				hue = (g - b) / delta;
			}
			else if (g == maxRGB)
			{
				hue = 2.f + (b - r) / delta;
			}
			else if (rgb.b == maxRGB)
			{
				hue = 4.f + (r - g) / delta;
			}
		}

		hue = hue * 60.f;
		if (hue < 0)
		{
			hue = hue + 360.f;
		}

		RKVector HSB; HSB.Set(hue, saturation * 100.f / 255.f, brightness * 100.f / 255.f);
		return HSB;
	}

	/*
	 HSL2RGB Converts hue, saturation, luminance values to the equivalent red, green and blue values.
	 For details on this conversion, see Fundamentals of Interactive Computer Graphics by Foley and van Dam (1982, Addison and Wesley)
	 You can also find HSL to RGB conversion algorithms by searching the Internet.
	 See also http://en.wikipedia.org/wiki/HSV_color_space for a theoretical explanation
	 */
	RKVector RKUtil_HSL2RGB(const RKVector& hsl)
	{
		float h = hsl.x;
		float s = hsl.y;
		float l = hsl.z;

		RKVector out; out.Set(0.f, 0.f, 0.f);

		float			temp1,
			temp2;
		float			temp[3];
		int				i;

		// Check for saturation. If there isn't any just return the luminance value for each, which results in gray.
		if (s == 0.0)
		{
			RKVector lum; lum.Set(l, l, l);
			return lum;
		}

		// Test for luminance and compute temporary values based on luminance and saturation 
		if (l < 0.5f)
		{
			temp2 = l * (1.0f + s);
		}
		else
		{
			temp2 = l + s - l * s;
		}

		temp1 = 2.0f * l - temp2;

		// Compute intermediate values based on hue
		temp[0] = h + 1.0f / 3.0f;
		temp[1] = h;
		temp[2] = h - 1.0f / 3.0f;

		for (i = 0; i < 3; ++i)
		{
			// Adjust the range
			if (temp[i] < 0.0)
			{
				temp[i] += 1.0;
			}

			if (temp[i] > 1.0)
			{
				temp[i] -= 1.0;
			}

			if (6.0f * temp[i] < 1.0f)
			{
				temp[i] = temp1 + (temp2 - temp1) * 6.0f * temp[i];
			}
			else
			{
				if (2.0 * temp[i] < 1.0)
				{
					temp[i] = temp2;
				}
				else
				{
					if (3.0f * temp[i] < 2.0f)
					{
						temp[i] = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - temp[i]) * 6.0f;
					}
					else
					{
						temp[i] = temp1;
					}
				}
			}
		}

		// Assign temporary values to R, G, B
		RKVector RGB; RGB.Set(temp[0], temp[1], temp[2]);
		return RGB;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	uint32 RKUtil_wcstombs(char* s, const wchar_t* pwcs, size_t n)
	{
		int count = 0;
		if (n != 0)
		{
			do
			{
				if ((*s++ = (char)*pwcs++) == 0)
				{
					break;
				}
				count++;
			} while (--n != 0);
		}

		return count;
	}

#define Three2767Div1024 31.9990234375f
#define One024Div32767 0.0312509537034211f

#define Six5535Div1024 63.9990234375f
#define One024Div65535 0.0156252384222171f

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	int8 PackUnitFloat_Int8(float f) { return (int8)(f*127.f); }
	float UnpackUnitFloat_Int8(int8 s) { return (float)(s / 127.f); }

#if defined(__ARM_NEON__) && !defined(__arm64__)
	void UnpackUnitFloat_Int8x4(const int8* s, float fOut[4])
	{
		asm volatile (
			"veor.i64    d4,d4               \n\t"
			"veor.i64    d6,d6               \n\t"
			"veor.i64    d7,d7               \n\t"

			"vld1.32 		{d2}, [%0]			\n\t"	//d2={x0,y0,z0,w0}
			"vzip.8     d4,d2               \n\t" //d4 should be 16 bit values of our 8 bit numbers (8 empty bits onto the begining of each)
			"vzip.16    d6,d4               \n\t" //now {d6,d4} should be our 32 bit values for qx,qy,qz,garbage
			"vswp       d7,d4               \n\t" //q3 is now qz,qy,qz,qw
			"vshr.s32   q3,q3,#16           \n\t" //bit shift right to make 32 bit values
			"vcvt.f32.s32 q1,q3,15          \n\t" //15 bits of decimal for signed
			"vst1.32 {d2,d3},[%1]           \n\t" //store out our quaternion
			:
		: "r"(s), "r"(fOut)  :
			"q0","q1","q2","q3","q4","memory"
			);
	}
#else
	void UnpackUnitFloat_Int8x4(const int8* s, float fOut[4])
	{
		fOut[0] = (float)s[0] / 127.f;
		fOut[1] = (float)s[1] / 127.f;
		fOut[2] = (float)s[2] / 127.f;
		fOut[3] = (float)s[3] / 127.f;
	}
#endif //__ARM_NEON__

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	uint8 PackUnitFloat_UInt8(float f) { return (uint8)(f*255.f); }
	float UnpackUnitFloat_UInt8(uint8 s) { return (float)(s / 255.f); }

#if defined(__ARM_NEON__) && !defined(__arm64__)
	void UnpackUnitFloat_UInt8x4(const uint8* s, float fOut[4])
	{
		asm volatile (
			"veor.i64    d4,d4               \n\t"
			"veor.i64    d6,d6               \n\t"
			"veor.i64    d7,d7               \n\t"

			"vld1.32 		{d2}, [%0]			\n\t"	//d2={x0,y0,z0,w0}
			"vzip.8     d4,d2               \n\t" //d4 should be 16 bit values of our 8 bit numbers (8 empty bits onto the begining of each)
			"vzip.16    d6,d4               \n\t" //now {d6,d4} should be our 32 bit values for qx,qy,qz,garbage
			"vswp       d7,d4               \n\t" //q3 is now qz,qy,qz,qw
			"vshr.u32   q3,q3,#16           \n\t" //bit shift right to make 32 bit values
			"vcvt.f32.u32 q1,q3,16          \n\t" //16 bits of decimal for unsigned
			"vst1.32 {d2,d3},[%1]           \n\t" //store out our quaternion
			:
		: "r"(s), "r"(fOut)  :
			"q0","q1","q2","q3","q4","memory"
			);
	}
#else
	void UnpackUnitFloat_UInt8x4(const uint8* s, float fOut[4])
	{
		fOut[0] = (float)s[0] / 255.f;
		fOut[1] = (float)s[1] / 255.f;
		fOut[2] = (float)s[2] / 255.f;
		fOut[3] = (float)s[3] / 255.f;
	}
#endif //__ARM_NEON__

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	int16 PackUnitFloat_Int16(float f) { return (int16)(f*32767.f); }
	float UnpackUnitFloat_Int16(int16 s) { return (float)(s / 32767.f); }

#if defined(__ARM_NEON__) && !defined(__arm64__)
	void UnpackUnitFloat_Int16x4(const int16* s, float fOut[4])
	{
		asm volatile (
			"veor.i64   d0,d0               \n\t"
			"veor.i64   d1,d1               \n\t"
			"vld1.32 		{d2, d3}, [%0]			\n\t"	//d2={x0,y0}, d3={z0, w0}
			"vzip.16    d0,d2               \n\t" //{d0,d2} should be 32 bit versions of x,y,z,w
			"vswp       d1,d2               \n\t" //q0 is now x,y,z,qw
			"vshr.s32   q0,q0,#16           \n\t" //sign extend shift them right 16 bits to fill 32 bit register
			"vcvt.f32.s32 q2,q0,15           \n\t" //15 bits of decimal
			"vst1.32 {d4,d5},[%1]           \n\t" //store our vector (fixup w later)
			:
		: "r"(s), "r"(fOut)  :
			"q0","q1","q2","q3","q4","memory"
			);
	}
#else
	void UnpackUnitFloat_Int16x4(const int16* s, float fOut[4])
	{
		fOut[0] = (float)s[0] / 32767.0f;
		fOut[1] = (float)s[1] / 32767.0f;
		fOut[2] = (float)s[2] / 32767.0f;
		fOut[3] = (float)s[3] / 32767.0f;
	}
#endif //__ARM_NEON__

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	uint16 PackUnitFloat_UInt16(float f) { return (uint16)(f*65535.f); }
	float UnpackUnitFloat_UInt16(uint16 s) { return (float)(s / 65535.f); }

#if defined(__ARM_NEON__) && !defined(__arm64__)
	void UnpackUnitFloat_UInt16x4(const uint16* s, float fOut[4])
	{
		asm volatile (
			"veor.i64   d0,d0               \n\t"
			"veor.i64   d1,d1               \n\t"
			"vld1.32 		{d2, d3}, [%0]			\n\t"	//d2={x0,y0}, d3={z0, w0}
			"vzip.16    d0,d2               \n\t" //{d0,d2} should be 32 bit versions of x,y,z,w
			"vswp       d1,d2               \n\t" //q0 is now x,y,z,qw
			"vshr.u32   q0,q0,#16           \n\t" //sign extend shift them right 16 bits to fill 32 bit register
			"vcvt.f32.u32 q2,q0,16           \n\t" //16 bits of decimal for uint
			"vst1.32 {d4,d5},[%1]           \n\t" //store our vector (fixup w later)
			:
		: "r"(s), "r"(fOut)  :
			"q0","q1","q2","q3","q4","memory"
			);
	}
#else
	void UnpackUnitFloat_UInt16x4(const uint16* s, float fOut[4])
	{
		fOut[0] = (float)s[0] / 65535.f;
		fOut[1] = (float)s[1] / 65535.f;
		fOut[2] = (float)s[2] / 65535.f;
		fOut[3] = (float)s[3] / 65535.f;
	}
#endif //__ARM_NEON__

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	int16 PackFloat_Int16(float f) { return (int16)ceil(f*Three2767Div1024/*32767/1024*/); }
	float UnpackFloat_Int16(int16 i) { return ((float)i)*One024Div32767/*1024/32767*/; }

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	uint16 PackFloat_UInt16(float f) { return (uint16)ceil(f*Six5535Div1024/*65535/1024*/); }
	float  UnpackFloat_UInt16(uint16 i) { return ((float)i)*One024Div65535/*1024/65535*/; }


	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	uint32 RKCompress(void* pDest, uint32 destLen, void* pSrc, uint32 srcLen)
	{
		uLongf sourceLength = srcLen;
		uLongf destLength = destLen;

		int err = compress2((Bytef*)pDest, &destLength, (Bytef*)pSrc, sourceLength, Z_BEST_COMPRESSION);
		if (err != Z_OK)
		{
			destLength = 0;
		}

		return (uint32)destLength;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	uint32 RKDecompress(void* pDest, uint32 destLen, void* pSrc, uint32 srcLen)
	{
		uLongf sourceLength = srcLen;
		uLongf destLength = destLen;

		int err = uncompress((Bytef*)pDest, &destLength, (Bytef*)pSrc, sourceLength);
		if (err != Z_OK)
		{
			destLength = 0;
		}

		return (uint32)destLength;
	}




	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	static resolveFilenameCB *gRKUtil_resolveFilenameCB = NULL;

	//--------------------------------------------------------------------------------------------------------------------------------------
	void RKUtil_SetFilenameResolverCallback(resolveFilenameCB *pCallback)
		//--------------------------------------------------------------------------------------------------------------------------------------
	{
		gRKUtil_resolveFilenameCB = pCallback;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------
	bool RKUtil_ResolveFilename(const char** pNewNamePtr, char *destBuffer, int sizeOfDest, const char *orgFilename, const char *hintExt)
		//--------------------------------------------------------------------------------------------------------------------------------------
	{
		if (gRKUtil_resolveFilenameCB)
		{
			bool needNewName = gRKUtil_resolveFilenameCB(pNewNamePtr, destBuffer, sizeOfDest, orgFilename, hintExt);
			return needNewName;
		}
		return false;
	}

	//**********
}