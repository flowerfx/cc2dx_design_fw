// 
// MemoryFileSystem.cpp
// Liam de Koster-Kjaer
// May 2014
// 

#include "../Include/MemoryFileSystem.h"
#include "../RKFileInternal.h"
#include "../../RKString_Code/RKString.h"
#include "../../Common/RKHashTable.h"

namespace RKUtils
{
	struct MemFileHandle
	{
		char    pFilename[RKSTRING_LENGTH_TEMP];
		char*   pData;
		uint32  iFileSize;
		uint32  iOffset;
	};

	static RKHashTable<MemFileHandle*> g_FileTable;

	//!>> ----------------------------------------------------------------------------
	bool MemoryFileSystem::Initialise()
	{
		bool bSuccess = true;

		g_FileTable.Init(25);

		// Register NativeFileSystem
		RKFileSystem fs;
		fs.eType = FS_MEMORY;
		fs.Exists = &Exists;
		fs.ExistsAbs = &ExistsAbs;
		fs.Open = &Open;
		fs.Close = &Close;
		fs.Read = &Read;
		fs.Write = 0;
		fs.Seek = &Seek;
		fs.Tell = &Tell;
		fs.GetSize = &Size;
		RegisterFileSystem(fs);

		return bSuccess;
	}

	//!>> ----------------------------------------------------------------------------
	void MemoryFileSystem::ShutDown()
	{
		RKHashTable<MemFileHandle*>::Iterator it = g_FileTable.Begin();
		while (it.IsValid())
		{
			RKDELETEARRAY(it.Value()->pData); //right now RKFile is just losing the handle to this memory, so it's important we delete it here.
			RKDELETE(it.Value());
			++it;
		}
		g_FileTable.Deinit();
	}

	//!>> ----------------------------------------------------------------------------
	bool MemoryFileSystem::ExistsAbs(const char* pFullPath)
	{
		RKASSERT(pFullPath, "pFullPath is null");

		char pFilename[RKSTRING_LENGTH_TEMP];
		RKString_ExtractFileName(pFullPath, pFilename, sizeof(pFilename));

		return g_FileTable.ContainsKey(pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	bool MemoryFileSystem::Exists(const char* pFilename)
	{
		RKASSERT(pFilename, "pFilename is null");
		return g_FileTable.ContainsKey(pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	RKFileInternal* MemoryFileSystem::Open(const char* pFilename, RKFileAccessMode eMode)
	{
		RKASSERT(pFilename, "pFilename is null");
		RKASSERT(eMode != RKFM_WRITE && eMode != RKFM_APPEND, "MemoryFileSystem::Open - Attempting to write with the MemoryFileSystem. This is not supported.");
		if (eMode == RKFM_WRITE || eMode == RKFM_APPEND)
			return NULL;

		RKFileInternal* pFile = NULL;

		MemFileHandle* pHandle = g_FileTable.Query(pFilename);
		if (pHandle)
		{
			pFile = RKNEW(RKFileInternal);
			pFile->pFileHandle = (void*)pHandle;

			RKLOG_INFO("Open: %s", pFilename);
		}

		return pFile;
	}

	//!>> ----------------------------------------------------------------------------
	void MemoryFileSystem::Close(RKFileInternal* pFile)
	{
		RKDELETE(pFile);  // Does not delete the MemFileHandle
		pFile = 0;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 MemoryFileSystem::Read(RKFileInternal* pFile, void* pData, uint32 iSize)
	{
		MemFileHandle* pHandle = static_cast<MemFileHandle*>(pFile->pFileHandle);

		if (pHandle->iFileSize > 0 && pHandle->pData != NULL)// Safety first
		{
			// Be sure not to read over the buffer bounds
			uint32 iMax = pHandle->iFileSize - pHandle->iOffset;
			uint32 iRead = iMax < iSize ? iMax : iSize;

			// Read data into given buffer
			memcpy(pData, pHandle->pData + pHandle->iOffset, iRead);

			// Increment file iterator
			pHandle->iOffset += iRead;
			RKASSERT(pHandle->iOffset <= pHandle->iFileSize, "Handle offset is greater than file size!");

			return iRead;
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	int32 MemoryFileSystem::Seek(RKFileInternal* pFile, int32 iOffset, int32 iSeekMode)
	{
		MemFileHandle* pHandle = static_cast<MemFileHandle*>(pFile->pFileHandle);

		switch (iSeekMode)
		{
		case SEEK_SET:
			pHandle->iOffset = iOffset;
			break;
		case SEEK_CUR:
			pHandle->iOffset += iOffset;
			break;
		case SEEK_END:
			pHandle->iOffset = pHandle->iFileSize + iOffset;
			break;
		}

		if (pHandle->iOffset > pHandle->iFileSize)
		{
			pHandle->iOffset = pHandle->iFileSize;
			return -1;
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	int32 MemoryFileSystem::Tell(RKFileInternal* pFile)
	{
		return static_cast<MemFileHandle*>(pFile->pFileHandle)->iOffset;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 MemoryFileSystem::Size(RKFileInternal* pFile)
	{
		return static_cast<MemFileHandle*>(pFile->pFileHandle)->iFileSize;
	}

	//!>> ----------------------------------------------------------------------------
	bool MemoryFileSystem::LoadFile(const char* pFilename, void* pData, uint64 iSize)
	{
		RKASSERT(pFilename, "pFilename is null");

		if (g_FileTable.ContainsKey(pFilename))
			return false;

		MemFileHandle* pHandle = RKNEW(MemFileHandle);
		strcpy(pHandle->pFilename, pFilename);
		pHandle->pData = (char*)pData;
		pHandle->iFileSize = (uint32)iSize;
		pHandle->iOffset = 0;
		//g_FileTable[pFilename] = pHandle;
		g_FileTable.Insert(pHandle, pFilename);
		return true;
	}

	//!>> ----------------------------------------------------------------------------
	bool MemoryFileSystem::UnloadFile(const char* pFilename)
	{
		RKASSERT(pFilename, "pFilename is null");

		RKHashTable<MemFileHandle*>::Iterator it = g_FileTable.FindKey(pFilename);
		if (it.IsValid())
		{
			MemFileHandle* pHandle = it.Value();
			RKDELETEARRAY(pHandle->pData); //right now RKFile is just losing the handle to this memory, so it's important we delete it here.
			RKDELETE(pHandle);
			g_FileTable.Erase(it);
			return true;
		}

		return false;
	}

	void MemoryFileSystem::Clear()
	{
		RKHashTable<MemFileHandle*>::Iterator it = g_FileTable.Begin();
		while (it.IsValid())
		{
			RKDELETEARRAY(it.Value()->pData); //right now RKFile is just losing the handle to this memory, so it's important we delete it here.
			++it;
		}
		g_FileTable.Clear();
	}
}
