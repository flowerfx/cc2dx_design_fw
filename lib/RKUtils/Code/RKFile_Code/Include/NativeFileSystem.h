// 
// NativeFileSystem.h
// ...
// Liam de Koster-Kjaer
// February 2012
// 

#ifndef __NATIVE_FILE_SYSTEM_H__
#define __NATIVE_FILE_SYSTEM_H__

#include "../../Common/RKTypes.h"
#include "../RKFile.h"

//!>> ----------------------------------------------------------------------------
namespace RKUtils
{
	struct RKFileInternal;
	namespace NativeFileSystem
	{
		bool            Initialise();
		void            ShutDown();

		bool            NativeExists(const char* pFilename);
		bool            NativeExistsAbs(const char* pFullPath);  // Absolute path
		RKFileInternal* NativeOpen(const char* pFilename, RKFileAccessMode eMode);
		void            NativeClose(RKFileInternal* pFile);
		uint32          NativeRead(RKFileInternal* pFile, void* pData, uint32 iSize);
		uint32          NativeWrite(RKFileInternal* pFile, void* pData, uint32 iSize);
		int32           NativeSeek(RKFileInternal* pFile, int32 iOffset, int32 iSeekMode);
		int32           NativeTell(RKFileInternal* pFile);
		uint32          NativeLength(const char* pFilename);
		uint32          NativeSize(RKFileInternal* pFile);
	}
}

#endif	// __NATIVE_FILE_SYSTEM_H__
