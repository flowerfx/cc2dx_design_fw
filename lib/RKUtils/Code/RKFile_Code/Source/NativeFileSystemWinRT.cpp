// 
// NativeFileSystem.cpp
// For W8/WP8 platforms.
// Created by Vo Van Khoa on 14/06/12.
//

#if defined(OS_WP8) || defined(OS_W8) || defined OS_W10
#include "../Include/NativeFileSystem.h"
#include <cstdio>
#include <cstring>
#include "../../Common/RKMacros.h"
#include "../../RKString_Code/RKString.h"
#include "../RKFileInternal.h"
#include "../RKFile.h"
#include <io.h>
#include <sys/stat.h>

#if defined OS_W8 || defined OS_W10
#include "../../RKPlatform/Win8.1/Win8Shared.h"
#endif
namespace RKUtils
{
	extern RKList<RKString*>*  g_pRegisteredSubdirectories;

	// Helper function to help enable RKFile_Win32 to open files from outside the data directory
	// Christophe Lebouil
	inline bool IsAbsFilename(const char* pFilename)
	{
		return (pFilename && pFilename[0] != 0 && pFilename[1] == ':');
	}

	//!>> ----------------------------------------------------------------------------
	bool NativeFileSystem::Initialise()
	{
		bool bSuccess = true;

		// Register NativeFileSystem
		RKFileSystem fs;
		fs.eType = FS_NATIVE;
		fs.Exists = &NativeExists;
		fs.ExistsAbs = &NativeExistsAbs;
		fs.Open = &NativeOpen;
		fs.Close = &NativeClose;
		fs.Read = &NativeRead;
		fs.Write = &NativeWrite;
		fs.Seek = &NativeSeek;
		fs.Tell = &NativeTell;
		fs.GetSize = &NativeSize;
		RegisterFileSystem(fs);

		return bSuccess;
	}

	//!>> ----------------------------------------------------------------------------
	void NativeFileSystem::ShutDown()
	{
	}

	//!>> ----------------------------------------------------------------------------
	bool NativeFileSystem::NativeExistsAbs(const char* pFullPath)
	{
#if ! defined OS_W8 && !defined OS_W10
		if (0 == _access(pFullPath, 0))
		{
			struct _stat statbuf;
			_stat(pFullPath, &statbuf);
			return !(_S_IFDIR & statbuf.st_mode);
		}
#else
		if (0 == _waccess(Win8Toolkit::ConvertUnicodedFolderPath(pFullPath).c_str(), 0))
		{
			struct _stat64i32 statbuf;
			_wstat(Win8Toolkit::ConvertUnicodedFolderPath(pFullPath).c_str(), &statbuf);
			return !(_S_IFDIR & statbuf.st_mode);
		}
#endif
		return false;
	}

	//!>> ----------------------------------------------------------------------------
	bool NativeFileSystem::NativeExists(const char* pFilename)
	{
		char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };

		strcpy(pFullPath, RKFile_GetSupportFilesPath());
		strcat(pFullPath, pFilename);
		if (NativeExistsAbs(pFullPath))
			return true;

		strcpy(pFullPath, RKFile_GetDocumentsPath());
		strcat(pFullPath, pFilename);
		if (NativeExistsAbs(pFullPath))
			return true;

		// Check registered subdirectories in order of registration
		for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
		{
			strcpy(pFullPath, (*g_pRegisteredSubdirectories)[i]->GetString());
			strcat(pFullPath, pFilename);
			if (NativeExistsAbs(pFullPath))
				return true;
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	RKFileInternal* NativeFileSystem::NativeOpen(const char* pFilename, RKFileAccessMode eMode)
	{
		RKFileInternal* pFile = 0;
		FILE* pFileHandle = 0;
		char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };
		const char* pMode = 0;
#if defined OS_W8 || defined OS_W10
		const wchar_t* w_pMode = 0;
#endif
		switch (eMode)
		{
		case RKFM_READ:
		{
			pMode = "rb";
#if defined OS_W8 || defined OS_W10
			w_pMode = L"rb";
#endif
			if (!RKFile_CreatePath(pFullPath, pFilename))
			{
				// Check registered subdirectories in order of registration
				for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
				{
					strcpy(pFullPath, (*g_pRegisteredSubdirectories)[i]->GetString());
					strcat(pFullPath, pFilename);
#if !defined OS_W8 && !defined OS_W10
					pFileHandle = fopen(pFullPath, pMode);
#else
					pFileHandle = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(pFullPath).c_str(), w_pMode);
#endif
					if (pFileHandle)
					{
						RKLOGt_INFO("NativeFS", "Opened file: %s, %s", pFullPath, pMode);
						pFile = RKNEW(RKFileInternal);
						pFile->pFileHandle = pFileHandle;
						return pFile;
					}
				}

				// Else just try to open the file ( worst case )
#if !defined OS_W8 && !defined OS_W10
				pFileHandle = fopen(pFullPath, pMode);
#else
				pFileHandle = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(pFullPath).c_str(), w_pMode);
#endif
				if (pFileHandle)
				{
					RKLOGt_INFO("NativeFS", "Opened file: %s, %s", pFilename, pMode);
					pFile = RKNEW(RKFileInternal);
					pFile->pFileHandle = pFileHandle;
					return pFile;
				}

				return NULL;
			}
		} break;
		case RKFM_WRITE:
		{
			strcpy(pFullPath, RKFile_GetSupportFilesPath());
			strcat(pFullPath, pFilename);
			pMode = "wb";
#if defined OS_W8 || defined OS_W10
			w_pMode = L"wb";
#endif
		} break;
		case RKFM_APPEND:
		{
			strcpy(pFullPath, RKFile_GetSupportFilesPath());
			strcat(pFullPath, pFilename);
			pMode = "ab+";
#if defined OS_W8 || defined OS_W10
			w_pMode = L"ab+";
#endif
		} break;
		}

#if !defined OS_W8 && !defined OS_W10
		pFileHandle = fopen(pFullPath, pMode);
#else
		pFileHandle = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(pFullPath).c_str(), w_pMode);
#endif
		if (pFileHandle)
		{
			RKLOGt_INFO("NativeFS", "%s %s", pFullPath, pMode);

			pFile = RKNEW(RKFileInternal);
			pFile->pFileHandle = pFileHandle;
			return pFile;
		}

		RKLOGt_WARN("NativeFS", "Failed to open file: %s in mode: %s", pFullPath, pMode);
		return NULL;
	}

	//!>> ----------------------------------------------------------------------------
	void NativeFileSystem::NativeClose(RKFileInternal* pFile)
	{
		fclose((FILE*)pFile->pFileHandle);
		RKDELETE(pFile);
		pFile = 0;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 NativeFileSystem::NativeRead(RKFileInternal* pFile, void* pData, uint32 iSize)
	{
		return fread(pData, sizeof(char), iSize, (FILE*)pFile->pFileHandle);
	}

	//!>> ----------------------------------------------------------------------------
	uint32 NativeFileSystem::NativeWrite(RKFileInternal* pFile, void* pData, uint32 iSize)
	{
		return fwrite(pData, sizeof(char), iSize, (FILE*)pFile->pFileHandle);
	}

	//!>> ----------------------------------------------------------------------------
	int32 NativeFileSystem::NativeSeek(RKFileInternal* pFile, int32 iOffset, int32 iSeekMode)
	{
		return fseek((FILE*)pFile->pFileHandle, iOffset, iSeekMode);
	}

	//!>> ----------------------------------------------------------------------------
	int32 NativeFileSystem::NativeTell(RKFileInternal* pFile)
	{
		return ftell((FILE*)pFile->pFileHandle);
	}

	//!>> ----------------------------------------------------------------------------
	uint32 NativeFileSystem::NativeSize(RKFileInternal* pFile)
	{
		int iLength = 0;

		FILE* pFileHandle = (FILE*)pFile->pFileHandle;
		if (pFileHandle)
		{
			fseek(pFileHandle, 0, SEEK_END);
			iLength = ftell(pFileHandle);
			fseek(pFileHandle, 0, SEEK_SET);
		}

		return iLength;
	}
}
#endif// defined(OS_WP8) || defined(OS_W8)