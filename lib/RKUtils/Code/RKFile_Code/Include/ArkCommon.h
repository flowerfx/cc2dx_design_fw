#ifndef __ARK_COMMON_H__
#define __ARK_COMMON_H__

#include <cstdio>
#include "../../Common/RKTypes.h"
#include "../../RKLog_Code/RKLog.h"
#include "../../Common/RKHeap.h"

namespace RKUtils
{
	//#define ARK_LOG_DECOMPRESSION_INFO

	// ArkVersion
	// 1.1 - Adding ArkFileMetadata::bDecrypt
	// 1.2 - MAXSTRING changed to 64 (was 128)
	// 1.3 - Adding ArkFileMetadata::iEncrFileSize, removing ArkFileMetadata::bDecrypt
	// 1.3 - g_kiEncryptKey is now a 128 encryption key
	//        This is for TEA encryption which can alter the amount of data written to the ark per file
	// 1.4 - Fixing bug with ArkVersion
	// 1.5 - Ark number is now four digits (ark number is kept in the first four characters of an ark file's filename)
	// 1.6 - ArkFileMetadata member uint32 bExtract is now int32 iFlags. 
	//        There is a flag for Extract that signals the same as the previous value, but the flags are also 
	//        now used at runtime to determine whether there is an older duplicate of this file in an archive
	static const char*  g_kArkVersion = "1.6";
	static const uint32 g_kiEncryptKey[4] = { 20091983, 00455641, 20091983, 10062005 };
	static const char   g_kcEncryptKey = 25;

#define MAXSTRING         64
#define MAXPATH           512
#define MAXVERSIONSTRING  8
#define MD5DIGESTSIZE     16

	enum ArkFileFlag
	{
		AFF_NONE = 0x00000,
		AFF_EXTRACT = 0x00002,
		AFF_MULTIPLE = 0x00004,
	};

#pragma pack(push, 1)
	struct ArkHeader
	{
		uint32  iNumFiles;
		uint32  iIndexOffset;
		char    pArkVersion[MAXVERSIONSTRING];
	};

	struct ArkFileMetadata  // 168 bytes disregarding padding etc...
	{
		char    pFilename[MAXSTRING];
		char    pSubdir[MAXSTRING]; // No slashes
		uint32  iArkOffset;
		uint32  iSrcFileSize;
		uint32  iComprFileSize;
		uint32  iEncrFileSize;
		uint32  iTimestamp;
		uint8   pMD5Digest[16];
		int32   iFlags;

		ArkFileMetadata()                   { iFlags = 0; }
		bool IsFlagSet(ArkFileFlag eFlag)   { return (iFlags & eFlag) == eFlag; }
		void SetFlagOn(ArkFileFlag eFlag)   { iFlags |= eFlag; }
		void SetFlagOff(ArkFileFlag eFlag)  { iFlags ^= eFlag; }
	};
#pragma pack(pop)

	struct ArkFileHandle
	{
		char    pArkFilename[MAXPATH];
		char    pFilename[MAXSTRING];
		char*   pData;
		uint32  iFileSize;
		uint32  iOffset;
	};
}
#endif	// __ARK_COMMON_H__