// 
// ArkFileSystem.h
// ...
// Liam de Koster-Kjaer
// January 2012
// 

#ifndef __ARK_FILE_SYSTEM_H__
#define __ARK_FILE_SYSTEM_H__

#include "ArkCommon.h"
#include "../RKFile.h"

//!>> ----------------------------------------------------------------------------
namespace RKUtils
{
	class Ark;
	class ArkManager;
	struct RKFileInternal;
	namespace ArkFileSystem
	{
		 bool            Initialise();
		 bool            StartExtractFiles();
		 bool            IsExtractingFiles(float& fProgress);
		 void            ShutDown();

		 bool            ArkExists(const char* pFilename);
		 bool            ArkExistsAbs(const char* pFullPath);  // Absolute path (will make a relative path out of it, not optimal but convenient)
		 RKFileInternal* ArkOpen(const char* pFilename, RKFileAccessMode eMode);
		 void            ArkClose(RKFileInternal* pFile);
		 uint32          ArkRead(RKFileInternal* pFile, void* pData, uint32 iSize);
		 uint32          ArkWrite(RKFileInternal* pFile, void* pData, uint32 iSize);
		 int32           ArkSeek(RKFileInternal* pFile, int32 iOffset, int32 iSeekMode);
		 int32           ArkTell(RKFileInternal* pFile);
		 uint32          ArkLength(const char* pFilename);
		 uint32          ArkSize(RKFileInternal* pFile);
	}
}

#endif	// __ARK_FILE_SYSTEM_H__