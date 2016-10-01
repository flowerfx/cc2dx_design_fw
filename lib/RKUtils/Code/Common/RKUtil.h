/*
 *  RKUtil.h
 *  miPet
 *
 *  Created by Daniel Stephens on 23/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef RKUTIL_H
#define RKUTIL_H

#include "../RKVector_Code/RKVector.h"
#include "RKTypes.h"
namespace RKUtils
{

	float RKUtil_ComputeBoxVolume(const RKVector& extents);
	float RKUtil_ComputeSphereVolume(float radius);
	float RKUtil_ComputeCylinderVolume(float radius);
	float RKUtil_ComputeCapsuleVolume(float radius, float height);
	float RKUtil_ComputeConeVolume(float radius, float height);

	void RKUtil_SetRandomSeed(uint32 seed);
	int  RKUtil_RandomInt(int min, int max);

	RKVector RKUtil_RGBtoHSB(const RKVector& rgb);
	RKVector RKUtil_HSBtoRGB(const RKVector& hsb);

	uint32 RKUtil_wcstombs(char* s, const wchar_t* pwcs, size_t n);

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	int8  PackUnitFloat_Int8(float f);
	float UnpackUnitFloat_Int8(int8 s);
	void UnpackUnitFloat_Int8x4(const int8* s, float fOut[4]);
	uint8 PackUnitFloat_UInt8(float f);
	float UnpackUnitFloat_UInt8(uint8 s);
	void UnpackUnitFloat_UInt8x4(const uint8* s, float fOut[4]);

	int16 PackUnitFloat_Int16(float f);
	float UnpackUnitFloat_Int16(int16 s);
	void UnpackUnitFloat_Int16x4(const int16* s, float fOut[4]);
	uint16 PackUnitFloat_UInt16(float f);
	float UnpackUnitFloat_UInt16(uint16 s);
	void UnpackUnitFloat_UInt16x4(const uint16* s, float fOut[4]);


	int16 PackFloat_Int16(float f);
	float UnpackFloat_Int16(int16 i);
	uint16 PackFloat_UInt16(float f);
	float  UnpackFloat_UInt16(uint16 i);

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	uint32 RKCompress(void* pDest, uint32 destLen, void* pSrc, uint32 srcLen);
	uint32 RKDecompress(void* pDest, uint32 destLen, void* pSrc, uint32 srcLen);


	// --- extra tools; may be used by some Win32 tools --- [christophe]
	typedef bool resolveFilenameCB(const char** pNewNamePtr, char *destBuffer, int sizeOfDest, const char *orgFilename, const char *hintExt);
	void RKUtil_SetFilenameResolverCallback(resolveFilenameCB *pCallback);
	bool RKUtil_ResolveFilename(const char** pNewNamePtr, char *destBuffer, int sizeOfDest, const char *orgFilename, const char *hintExt);
}
#endif // RKUTIL_H