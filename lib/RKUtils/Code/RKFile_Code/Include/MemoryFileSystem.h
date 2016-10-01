// 
// MemoryFileSystem.h
// Liam de Koster-Kjaer
// May 2014
// 

#pragma once

#include "../../Common/RKTypes.h"
#include "../RKFile.h"

//!>> ----------------------------------------------------------------------------
namespace RKUtils
{
	struct RKFileInternal;
	namespace MemoryFileSystem
	{
		bool            Initialise();
		void            ShutDown();

		bool            Exists(const char* pFilename);
		bool            ExistsAbs(const char* pFullPath);  // Absolute path
		RKFileInternal* Open(const char* pFilename, RKFileAccessMode eMode);
		void            Close(RKFileInternal* pFile);
		uint32          Read(RKFileInternal* pFile, void* pData, uint32 iSize);
		int32           Seek(RKFileInternal* pFile, int32 iOffset, int32 iSeekMode);
		int32           Tell(RKFileInternal* pFile);
		uint32          Length(const char* pFilename);
		uint32          Size(RKFileInternal* pFile);

		bool            LoadFile(const char* pFilename, void* pData, uint64 iSize);
		bool            UnloadFile(const char* pFilename);
		void            Clear();
	}
}
