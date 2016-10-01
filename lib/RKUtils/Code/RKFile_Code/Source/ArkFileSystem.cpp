// 
// ArkFileSystem.cpp
// Liam de Koster-Kjaer
// January 2012
// 

//#include "CasualCoreConfig.h"
#if CC_USE_ARK==1

#include "../Include/ArkFileSystem.h"
#include "../Include/ArkManager.h"
#include "../../RKString_Code/RKString.h"
#include "../RKFileInternal.h"
#include "../RKFile.h"
namespace RKUtils
{
	//!>> ----------------------------------------------------------------------------
	ArkManager* g_pArkManager;
	//!>> ----------------------------------------------------------------------------
	bool ArkFileSystem::Initialise()
	{
		bool bSuccess = true;

		// Create ArkManager
		if (g_pArkManager) RKDELETE(g_pArkManager);
		g_pArkManager = RKNEW(ArkManager);

		// Register ArkFileSystem (Needs to be registered before initialising ArkManager!)
		RKFileSystem fs;
		fs.eType = FS_ARK;
		fs.Exists = &ArkExists;
		fs.ExistsAbs = &ArkExistsAbs;
		fs.Open = &ArkOpen;
		fs.Close = &ArkClose;
		fs.Read = &ArkRead;
		fs.Write = 0;
		fs.Seek = &ArkSeek;
		fs.Tell = &ArkTell;
		fs.GetSize = &ArkSize;
		RegisterFileSystem(fs);

		// Initialise ArkManager
#if _RK_USE_RKVIEWER==1
		if (!g_pArkManager->InitialiseForSolar())
		{
			DeregisterFileSystem(FS_ARK);
			bSuccess = false;	// Failed to initialise ArkManager
		}
#else
		if (!g_pArkManager->Initialise())
		{
			DeregisterFileSystem(FS_ARK);
			bSuccess = false;	// Failed to initialise ArkManager
		}
#endif

		return bSuccess;
	}

	//!>> ----------------------------------------------------------------------------
	bool ArkFileSystem::StartExtractFiles()
	{
		return g_pArkManager->StartExtractFiles();
	}

	//!>> ----------------------------------------------------------------------------
	bool ArkFileSystem::IsExtractingFiles(float& fProgress)
	{
		return g_pArkManager->IsExtractingFiles(fProgress);
	}

	//!>> ----------------------------------------------------------------------------
	void ArkFileSystem::ShutDown()
	{
		RKDELETE(g_pArkManager);
		g_pArkManager = 0;
	}

	//!>> ----------------------------------------------------------------------------
	bool ArkFileSystem::ArkExistsAbs(const char* pFullPath)
	{
		// This is redundant - ArkManager::FileExists removes path information from pFullPath itself

		//int32 iLastChar = strlen(pFullPath)-1;
		//if (iLastChar > 0)
		//{
		//  for(const char* pFilename = pFullPath + iLastChar; pFilename != pFullPath; --pFilename)
		//  {
		//    if (*pFilename == '/' || *pFilename == '\\')
		//      return g_pArkManager->FileExists(++pFilename);
		//  }
		//}

		//return false;

		return g_pArkManager->FileExists(pFullPath);
	}

	//!>> ----------------------------------------------------------------------------
	bool ArkFileSystem::ArkExists(const char* pFilename)
	{
		return g_pArkManager->FileExists(pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	RKFileInternal* ArkFileSystem::ArkOpen(const char* pFilename, RKFileAccessMode eMode)
	{
		RKASSERT(eMode != RKFM_WRITE && eMode != RKFM_APPEND, "ArkFileSystem::ArkOpen - Attempting to write with the ArkFileSystem. This is not supported.");
		if (eMode == RKFM_WRITE || eMode == RKFM_APPEND)
			return 0;

		RKFileInternal* pFile = 0;
		ArkFileHandle* pHandle = RKNEW(ArkFileHandle);

		if (g_pArkManager->GetFile(pFilename, *pHandle))
		{
			//RKLOG(pFilename);
			pFile = RKNEW(RKFileInternal);
			pFile->pFileHandle = (void*)pHandle;
		}
		else
		{
			RKDELETE(pHandle);
			pHandle = 0;
		}

		return pFile;
	}

	//!>> ----------------------------------------------------------------------------
	void ArkFileSystem::ArkClose(RKFileInternal* pFile)
	{
		ArkFileHandle* pHandle = static_cast<ArkFileHandle*>(pFile->pFileHandle);

		RKDELETEARRAY(pHandle->pData);
		pHandle->pData = 0;

		RKDELETE(pHandle);
		pHandle = 0;

		RKDELETE(pFile);
		pFile = 0;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 ArkFileSystem::ArkRead(RKFileInternal* pFile, void* pData, uint32 iSize)
	{
		ArkFileHandle* pHandle = static_cast<ArkFileHandle*>(pFile->pFileHandle);

		if (pHandle->iFileSize > 0 && pHandle->pData != NULL) // Safety first
		{
			// Be sure not to read over the buffer bounds
			uint32 iMax = pHandle->iFileSize - pHandle->iOffset;
			uint32 iRead = iMax < iSize ? iMax : iSize;

			// Read data into given buffer
			memcpy(pData, pHandle->pData + pHandle->iOffset, iRead);

			// Increment file iterator
			pHandle->iOffset += iRead;
			RKASSERT(pHandle->iOffset <= pHandle->iFileSize, "ArkFileSystem::ArkRead - FileHandle offset is greater than file size!");

			return iRead;
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 ArkFileSystem::ArkWrite(RKFileInternal* pFile, void* pData, uint32 iSize)
	{
		RKASSERT(false, "ArkFileSystem::ArkWrite - Attempting to write with the ArkFileSystem. This is not supported.");
		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	int32 ArkFileSystem::ArkSeek(RKFileInternal* pFile, int32 iOffset, int32 iSeekMode)
	{
		ArkFileHandle* pHandle = static_cast<ArkFileHandle*>(pFile->pFileHandle);

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
	int32 ArkFileSystem::ArkTell(RKFileInternal* pFile)
	{
		return static_cast<ArkFileHandle*>(pFile->pFileHandle)->iOffset;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 ArkFileSystem::ArkLength(const char* pFilename)
	{
		ArkFileMetadata Meta;
		if (g_pArkManager->GetFileMetadata(pFilename, Meta))
			return Meta.iSrcFileSize;

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 ArkFileSystem::ArkSize(RKFileInternal* pFile)
	{
		return static_cast<ArkFileHandle*>(pFile->pFileHandle)->iFileSize;
	}
	
}
#endif //#if CC_USE_ARK==1

