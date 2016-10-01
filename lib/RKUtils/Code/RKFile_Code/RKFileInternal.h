// 
// RKFileInternal.h
// ...
// Liam de Koster-Kjaer
// February 2012
// 

#ifndef __RK_FILE_INTERNAL_H__
#define __RK_FILE_INTERNAL_H__
#include "../Common/RK.h"
#include "RKFile.h"
#include "../Common/RKTypes.h"
namespace RKUtils
{
#define FILESYSTEM_COUNT  4

	struct RKFileSystem;
    enum RKFileAccessMode : unsigned int;

	struct RKFileInternal
	{
		RKFileSystem*     pFileSystem;
		void*             pFileHandle;
		RKFileAccessMode  eMode;
	};

	enum EFileSystem
	{
		FS_NATIVE = 1 << 0,
		FS_ARK = 1 << 1,
		FS_NET = 1 << 2,
		FS_MEMORY = 1 << 3,
	};

	struct RKFileSystem
	{
		EFileSystem     eType;
		bool(*Exists)(const char* pFilename);
		bool(*ExistsAbs)(const char* pFullPath);
		RKFileInternal* (*Open)(const char* pFilename, RKFileAccessMode eMode);
		void(*Close)(RKFileInternal* pFile);
		uint32(*Read)(RKFileInternal* pFile, void* pData, uint32 iSize);
		uint32(*Write)(RKFileInternal* pFile, void* pData, uint32 iSize);
		int32(*Seek)(RKFileInternal* pFile, int32 iOffset, int32 iSeekMode);
		int32(*Tell)(RKFileInternal* pFile);
		uint32(*GetSize)(RKFileInternal* pFile);
	};

	//!>> ----------------------------------------------------------------------------
	void              RegisterFileSystem(RKFileSystem& filesystem);
	void              DeregisterFileSystem(EFileSystem eType);
	bool              IsFileSystemRegistered(EFileSystem eType);
	RKFileSystem*     GetRegisteredFileSystem(EFileSystem eType);
}
#endif	// __RK_FILE_INTERNAL_H__