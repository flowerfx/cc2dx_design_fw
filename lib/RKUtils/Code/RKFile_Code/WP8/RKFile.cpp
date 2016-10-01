//
// RKFile.cpp (WinRT)
// 
// Created by Vo Van Khoa on 14/06/12.
// 
// 

//!>> ----------------------------------------------------------------------------
#if defined(OS_WP8) || defined(OS_W8) || defined OS_W10
#include <direct.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../../Common/RK.h"
#include "../RKFile.h"
#include "../../Common/RKMacros.h"
#include "../../RKString_Code/RKString.h"
#include "../RKFileInternal.h"

///-- ? Casual Core dependency.. :(
#include "../Include/MemoryFileSystem.h"
#include "../Include/ArkFileSystem.h"
#include "../Include/NativeFileSystem.h"

#if CC_USE_NETWORK_FS==1
#  include "DebugConnection/DebugHost.h"
#  include "DebugConnection/NetworkFS.h"
#  include "DebugConnection/NetworkFSTrigger.h"
#  include "DebugConnection/LiveUpdateManager.h"
#endif
#include "../../../../WinRTAPIsEmul/WinRTAPIsEmul.h"
#if defined OS_W8 || defined OS_W10
#include "../../RKPlatform/Win8.1/Win8Shared.h"
#endif
namespace RKUtils
{
	//!>> ----------------------------------------------------------------------------
	char              RKFile_InstalledDirectory[RKSTRING_LENGTH_TEMP] = { 0 };
	char			  RKFile_LocalPath[RKSTRING_LENGTH_TEMP] = { 0 };
	static uint32            g_iNumRegisteredSystems = 0;
	RKFileSystem      g_pRegisteredSystems[FILESYSTEM_COUNT];
	static int32            g_iRegisteredSystems = 0;
	bool              g_isRKFileModuleInitialized = false;
	RKFileSystemMode  g_eMode = RKFM_NORMAL;  // This effects RKFile_Open and RKFile_Exists, for use with tools (eg. RKModelViewer)

	RKList<RKString*>*  g_pRegisteredSubdirectories;

	void RKFile_GetSystemPath(char* pStorage, bool installedPath = true)
	{
		static std::string sInstalledFolderPath = "";
		static std::string sSavedFolderPath = "";
		if (installedPath)
		{
			if (sInstalledFolderPath.length() <= 0)
			{
				auto path = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
				std::wstring wsPath(path->Data());
				sInstalledFolderPath = std::string(wsPath.begin(), wsPath.end()) + "\\Data\\";
			}
			strncpy(pStorage, sInstalledFolderPath.c_str(), RKSTRING_LENGTH_TEMP);
		}
		else
		{
			if (sSavedFolderPath.length() <= 0)
			{
				auto path = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
				std::wstring wsPath(path->Data());
				sSavedFolderPath = std::string(wsPath.begin(), wsPath.end()) + "\\";
			}
			strncpy(pStorage, sSavedFolderPath.c_str(), RKSTRING_LENGTH_TEMP);
		}
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateInstalledPath(char* pFullFilePathOut, const char* pFilename)
	{
		strcpy(pFullFilePathOut, RKFile_InstalledDirectory);
		strcat(pFullFilePathOut, pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateInstalledPath(RKString& FullFilePathOut, const char* pFilename)
	{
		FullFilePathOut = RKFile_InstalledDirectory;
		FullFilePathOut += pFilename;
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateLocalPath(char* pFullFilePathOut, const char* pFilename)
	{
		strcpy(pFullFilePathOut, RKFile_LocalPath);
		strcat(pFullFilePathOut, pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateLocalPath(RKString& FullFilePathOut, const char* pFilename)
	{
		FullFilePathOut = RKFile_LocalPath;
		FullFilePathOut += pFilename;
	}

	// [Liam] Testing! Remove me -----------------------------------------------------
	void RKFileWindowsUnitTests()
	{
		const char* pFilename = "TestFile.txt";

		if (RKFile_Exists(pFilename))
			RKASSERT(false, "RKFile_Exists found a file when it shouldn't have");

		RKFile* pFile = RKFile_Open(pFilename, RKFM_WRITE);
		RKASSERT(pFile, "RKFile_Open failed to open file for writing");

		const char* pData = "TestTestTestTestTestTestTestTestTestTest";
		const uint32 iSize = strlen(pData); // 40

		uint32 iWritten = RKFile_Write(pFile, (void*)pData, iSize);
		RKASSERT(iWritten == iSize, "RKFile_Write wrote an invalid number of bytes");

		RKFile_Close(&pFile);

		RKASSERT(RKFile_Exists(pFilename), "RKFile_Exists failed to find a file when it should have");

		pFile = RKFile_Open(pFilename, RKFM_READ);
		RKASSERT(pFile, "RKFile_Open failed to open file for reading");

		char* pBuf = RKNEWARRAY(char, iSize * 2);
		memset(pBuf, 0, iSize * 2);
		uint32 iRead = RKFile_Read(pFile, pBuf, iSize);
		RKFile_Close(&pFile);

		RKASSERT(iRead == iSize, "RKFile_Read read an invalid number of bytes");

		uint32 iSize2 = strlen(pBuf);
		RKASSERT(iSize == iSize2, "Size of original and read data is inequal");
		for (uint32 i = 0; i < iSize; ++i)
			RKASSERT(pData[i] == pBuf[i], "Original and read data does not match");

		RKASSERT(RKFile_CreateDir("TESTDIRECTORY"), "RKFile_CreateDir failed to create directory");
		RKASSERT(RKFile_DirExists("TESTDIRECTORY"), "RKFile_DirExists failed to find existing directory");
		RKASSERT(!RKFile_DirExists("testiesdontexist"), "RKFile_DirExists found directory that shouldn't exist");

		char* pOtherFilename = "RenamedTestFile.txt";

		char pFullFilename[RKSTRING_LENGTH_TEMP] = { 0 };
		RKFile_CreatePath(pFullFilename, pFilename);
		char pFullOtherFilename[RKSTRING_LENGTH_TEMP] = { 0 };
		RKFile_CreatePath(pFullOtherFilename, pOtherFilename);

		RKASSERT(RKFile_RenameAbs(pFullFilename, pFullOtherFilename), "RKFile_RenameAbs failed to rename file");
		RKASSERT(RKFile_RenameAbs(pFullOtherFilename, pFullFilename), "RKFile_RenameAbs failed to rename file");

		bool bRemoved = RKFile_Remove(pFilename);
		RKASSERT(bRemoved, "RKFile_Remove failed to remove file");

		// Testing RKFile_Open flags -----------------
		// These tests require archived data
		const char* pArchivedFileName = "2012_cat_long_tail.anim";
		const char* pTestFilename = "test.txt";
		RKFile* pFlagFile = RKFile_Open(pTestFilename, RKFM_WRITE, RKFileForceNative);
		char* pFlagData = "testtesttesttesttesttesttesttesttesttest";
		uint32 iFlagSize = sizeof(pFlagData);
		if (iFlagSize == RKFile_Write(pFlagFile, (void*)pFlagData, iFlagSize))
			RKFile_Close(&pFlagFile);
		else
			RKASSERT(false, "Wrote invalid amount of test data");

		pFlagFile = RKFile_Open(pArchivedFileName, RKFM_READ, RKFileForceNative);  // A file that exists only in the archive
		if (pFlagFile)
		{
			RKASSERT(false, "Succeed to find file that NativeFileSystem shouldn't be able to find");
			RKFile_Close(&pFlagFile);
		}
		pFlagFile = RKFile_Open(pTestFilename, RKFM_READ, RKFileForceArk);  // A file that exists only external to the archive
		if (pFlagFile)
		{
			RKASSERT(false, "Succeed to find file that ArkFileSystem shouldn't be able to find");
			RKFile_Close(&pFlagFile);
		}

		pFlagFile = RKFile_Open(pTestFilename, RKFM_READ, RKFileForceNative);  // A file that exists only external to the archive
		if (pFlagFile)
			RKFile_Close(&pFlagFile);
		else
			RKASSERT(false, "Failed to find file that NativeFileSystem should be able to find");

		pFlagFile = RKFile_Open(pArchivedFileName, RKFM_READ, RKFileForceArk);  // A file that exists only in the archive
		if (pFlagFile)
			RKFile_Close(&pFlagFile);
		else
			RKASSERT(false, "Failed to find file that ArkFileSystem should be able to find");

		bRemoved = RKFile_Remove(pTestFilename);
		RKASSERT(bRemoved, "RKFile_Remove failed to remove file");
		// Testing RKFile_Open flags -----------------
	}
	// [Liam] Testing! Remove me -----------------------------------------------------

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateInstalledPathFast(char* pFullFilePathOut, const char* pFilename)
	{
		strcpy(pFullFilePathOut, RKFile_InstalledDirectory);
		strcat(pFullFilePathOut, pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateInstalledPathFast(RKString& FullFilePathOut, const char* pFilename)
	{
		FullFilePathOut = RKFile_InstalledDirectory;
		FullFilePathOut += pFilename;
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateLocalPathFast(char* pFullFilePathOut, const char* pFilename)
	{
		strcpy(pFullFilePathOut, RKFile_LocalPath);
		strcat(pFullFilePathOut, pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_CreateLocalPathFast(RKString& FullFilePathOut, const char* pFilename)
	{
		FullFilePathOut = RKFile_LocalPath;
		FullFilePathOut += pFilename;
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_InitModule()
	{
		// Get working data directory
		RKFile_GetSystemPath(RKFile_InstalledDirectory, true);
		// Switch to forward slashes for consistency
		uint32 iLen = strlen(RKFile_InstalledDirectory);
		RKASSERT(iLen + 1 < RKSTRING_LENGTH_TEMP, "Error - RKFile_InitModule got invalid working directory");
		for (uint32 i = 0; i < iLen; ++i)
		{
			if (RKFile_InstalledDirectory[i] == '\\')
				RKFile_InstalledDirectory[i] = '/';
		}
		// End working directory path with a slash
		RKFile_InstalledDirectory[iLen] = '/';
		RKFile_InstalledDirectory[iLen + 1] = 0;

		RKFile_GetSystemPath(RKFile_LocalPath, false);

		g_pRegisteredSubdirectories = RKNEW(RKList<RKString*>);

		// [Liam] Initialise FileSystems in order!
		memset(g_pRegisteredSystems, 0, sizeof(RKFileSystem)* FILESYSTEM_COUNT);
		g_iNumRegisteredSystems = 0;
		g_iRegisteredSystems = 0;

#if CC_USE_NETWORK_FS==1
		if (dbg::CheckNetworkFSShouldBeEnabled())
		{
			RKString tempPath = getenvEmul("TEMP");
			tempPath += "/NetworkFS_Cache/";

			std::string strPath(tempPath.GetString());
			std::string wstrPath(strPath.begin(), strPath.end());
			DWORD tempPathAttribs = GetFileAttributesEx((LPCWSTR)wstrPath.c_str(), GetFileExInfoStandard, NULL);

			if (tempPathAttribs == INVALID_FILE_ATTRIBUTES)
			{
				std::string strTempPath(tempPath.GetString());
				std::wstring wstrTempPath(strTempPath.begin(), strTempPath.end());
				BOOL ok = CreateDirectoryW(wstrTempPath.c_str(), NULL);
				RKASSERT(ok, "Unable to create temp directory for network fs cache");
			}
			else
			{
				RKASSERT(tempPathAttribs & FILE_ATTRIBUTE_DIRECTORY, "Network fs cache folder name already taken by some file, unable to create cache folder");
			}

			RKASSERT(dbg::DebugHost::IsValid(), "Unable to use network filesystem without debug host initialized");
			dbg::NetworkFS::Init(tempPath.GetString(), dbg::DebugHost::GetInstance()); // temporary
			dbg::LiveUpdateManager::Create(dbg::DebugHost::GetInstance());
		}
#endif

		MemoryFileSystem::Initialise();

		ArkFileSystem::Initialise();

		NativeFileSystem::Initialise();

		g_isRKFileModuleInitialized = true;
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_DeinitModule()
	{
		MemoryFileSystem::ShutDown();

		NativeFileSystem::ShutDown();

		ArkFileSystem::ShutDown();      //should ShutDown not deregister ?..

#if CC_USE_NETWORK_FS==1
		if (dbg::NetworkFS::IsValid())
		{
			dbg::LiveUpdateManager::Destroy();
			dbg::NetworkFS::Term();
		}
#endif

		memset(g_pRegisteredSystems, 0, sizeof(RKFileSystem)* FILESYSTEM_COUNT);
		g_iNumRegisteredSystems = 0;
		g_iRegisteredSystems = 0;
		g_isRKFileModuleInitialized = false;

		if (g_pRegisteredSubdirectories)
		{
			for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
				RKDELETE((*g_pRegisteredSubdirectories)[i]);
			RKDELETE(g_pRegisteredSubdirectories);
		}
	}


	//!>> ----------------------------------------------------------------------------
	bool RKFile_IsModuleInitialized()
	{
		return g_isRKFileModuleInitialized;
	}

	//!>> ----------------------------------------------------------------------------
	RKFile* RKFile_ForceFileSystemOpen(const char* pFilename, RKFileAccessMode eMode, EFileSystem eFileSystem)
	{
		RKFileSystem* pFileSystem = GetRegisteredFileSystem(eFileSystem);
		if (pFileSystem && pFileSystem->Open)
		{
			RKFileInternal* pOutFile = pFileSystem->Open(pFilename, eMode);
			if (pOutFile)
			{
				pOutFile->pFileSystem = pFileSystem;
				pOutFile->eMode = eMode;
				return (RKFile*)pOutFile;
			}
		}

		return NULL;
	}

	//!>> ----------------------------------------------------------------------------
	RKFile* RKFile_Open(const char* pFilename, RKFileAccessMode eMode, int32 iFlag)
	{
		RKASSERT(pFilename, "RKFile_Open - Filename is null");

#ifndef RKRETAIL
		iFlag = RKFileNoFlags;
#endif

		// Forced filesystem file access
		if (iFlag & RKFileForceNative)
			return RKFile_ForceFileSystemOpen(pFilename, eMode, FS_NATIVE);
		else if (iFlag & RKFileForceArk)
			return RKFile_ForceFileSystemOpen(pFilename, eMode, FS_ARK);
		else  // Normal file access
		{
			switch (eMode)
			{
			case RKFM_READ:
			{
				// Attempt using the first registered filesystem found.
				RKFile* pOutFile = 0;
				for (uint32 i = 0; i < g_iNumRegisteredSystems; ++i)
				{
					pOutFile = RKFile_ForceFileSystemOpen(pFilename, eMode, g_pRegisteredSystems[i].eType);
					if (pOutFile)
						return pOutFile;
				}
			}
			break;
			case RKFM_WRITE:
			case RKFM_APPEND:
				return RKFile_ForceFileSystemOpen(pFilename, eMode, FS_NATIVE);
				break;
			default:
				RKASSERT(false, "RKFile_Open - Invalid file access mode");
				break;
			}

			RKLOG("RKFile_Open - failed to open file: %s", pFilename);

			return NULL;
		}
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_Close(RKFile** ppFile)
	{
		RKASSERT(ppFile, "RKFile_Close - Pointer to RKFile pointer is null");
		RKASSERT(*ppFile, "RKFile_Close - RKFile pointer is null");

		// Use the filesystem specified by the RKFile* otherwise fail
		RKFileInternal* pFileInt = (RKFileInternal*)*ppFile;
		*ppFile = 0;
		RKFileSystem* pCurrentFileSystem = pFileInt->pFileSystem;
		if (pCurrentFileSystem)
		{
			RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Close - File system not registered");
			RKASSERT(pCurrentFileSystem->Close, "RKFile_Close - File system not completely implemented.");
			pCurrentFileSystem->Close(pFileInt);
		}
	}

	//!>> ----------------------------------------------------------------------------
	uint32 RKFile_Read(RKFile* pFile, void* pData, uint32 iSize)
	{
		RKASSERT(pFile, "RKFile_Read - RKFile pointer is null");
		RKASSERT(pData, "RKFile_Read - Data buffer pointer is null");

		// Use the filesystem specified by the RKFile* otherwise fail
		RKFileInternal* pFileInt = (RKFileInternal*)pFile;
		RKFileSystem* pCurrentFileSystem = pFileInt->pFileSystem;
		if (pCurrentFileSystem)
		{
			//RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Read - File system not registered");
			RKASSERT(pCurrentFileSystem->Read, "RKFile_Read - File system not completely implemented.");
			return pCurrentFileSystem->Read(pFileInt, pData, iSize);
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 RKFile_Write(RKFile* pFile, void* pData, uint32 iSize)
	{
		RKASSERT(pFile, "RKFile_Write - RKFile pointer is null");
		RKASSERT(pData, "RKFile_Write - Data buffer pointer is null");

		// Use the filesystem specified by the RKFile* otherwise fail
		RKFileInternal* pFileInt = (RKFileInternal*)pFile;
		RKFileSystem* pCurrentFileSystem = pFileInt->pFileSystem;
		if (pCurrentFileSystem)
		{
			//RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Write - File system not registered");
			RKASSERT(pCurrentFileSystem->Write, "RKFile_Write - File system not completely implemented.");
			return pCurrentFileSystem->Write(pFileInt, pData, iSize);
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	int32 RKFile_Seek(RKFile* pFile, int iOffset, int iSeekMode)
	{
		RKASSERT(pFile, "RKFile_Seek - RKFile pointer is null");

		// Use the filesystem specified by the RKFile* otherwise fail
		RKFileInternal* pFileInt = (RKFileInternal*)pFile;
		RKFileSystem* pCurrentFileSystem = pFileInt->pFileSystem;
		if (pCurrentFileSystem)
		{
			//RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Seek - File system not registered");
			RKASSERT(pCurrentFileSystem->Seek, "RKFile_Seek - File system not completely implemented.");
			return pCurrentFileSystem->Seek(pFileInt, iOffset, iSeekMode);
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	int32 RKFile_Tell(RKFile* pFile)
	{
		RKASSERT(pFile, "RKFile_Tell - RKFile pointer is null");

		// Use the filesystem specified by the RKFile* otherwise fail
		RKFileInternal* pFileInt = (RKFileInternal*)pFile;
		RKFileSystem* pCurrentFileSystem = pFileInt->pFileSystem;
		if (pCurrentFileSystem)
		{
			//RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Tell - File system not registered");
			RKASSERT(pCurrentFileSystem->Tell, "RKFile_Tell - File system not completely implemented.");
			return pCurrentFileSystem->Tell(pFileInt);
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_Remove(const char* pFilename)
	{
		RKASSERT(pFilename, "RKFile_Remove - Filename is null");

		// Only native filesystem
		// Only support and documents folders

		// Force native filesystem
		RKFileSystem* pNativeFileSystem = GetRegisteredFileSystem(FS_NATIVE);
		if (pNativeFileSystem)
		{
			RKASSERT(pNativeFileSystem->ExistsAbs, "RKFile_Remove - File system not completely implemented.");

			char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };

			RKFile_CreateLocalPath(pFullPath, pFilename);
			if (pNativeFileSystem->ExistsAbs(pFullPath))
#if defined OS_W8 || defined OS_W10
				return (0 == _wremove(Win8Toolkit::ConvertUnicodedFolderPath(pFullPath).c_str()));
#else
				return (0 == remove(pFullPath));
#endif
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	uint32 RKFile_GetSize(RKFile* pFile)
	{
		RKASSERT(pFile, "RKFile_GetSize - RKFile pointer is null");

		// Use the filesystem specified by the RKFile* otherwise fail
		RKFileInternal* pFileInt = (RKFileInternal*)pFile;
		RKFileSystem* pCurrentFileSystem = pFileInt->pFileSystem;
		if (pCurrentFileSystem)
		{
			RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_GetSize - File system not registered");
			RKASSERT(pCurrentFileSystem->GetSize, "RKFile_GetSize - File system not completely implemented.");
			return pCurrentFileSystem->GetSize((RKFileInternal*)pFile);
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_CreatePath(char* pFullFilePathOut, const char* pFilename)
	{
		RKASSERT(pFullFilePathOut, "RKFile_CreatePath - Out filepath buffer is null");
		RKASSERT(pFilename, "RKFile_CreatePath - Filename is null");

		RKFile_CreateInstalledPathFast(pFullFilePathOut, pFilename);
		if (!RKFile_ExistsAbs(pFullFilePathOut))
		{
			RKFile_CreateLocalPathFast(pFullFilePathOut, pFilename);
			return RKFile_ExistsAbs(pFullFilePathOut);
		}
		return true;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_CreatePath(RKString& FullFilePathOut, const char* pFilename)
	{
		RKASSERT(pFilename, "RKFile_CreatePath - Filename is null");

		RKFile_CreateInstalledPathFast(FullFilePathOut, pFilename);
		if (!RKFile_ExistsAbs(FullFilePathOut.GetString()))
		{
			RKFile_CreateLocalPathFast(FullFilePathOut, pFilename);
			return RKFile_ExistsAbs(FullFilePathOut.GetString());
		}
		return true;
	}

	//!>> ----------------------------------------------------------------------------
	const char* RKFile_GetBundlePath()
	{
		return RKFile_LocalPath;
	}

	//!>> ----------------------------------------------------------------------------
	const char* RKFile_GetSupportFilesPath()
	{
		return RKFile_LocalPath;
	}

	//!>> ----------------------------------------------------------------------------
	const char* RKFile_GetDocumentsPath()
	{
		return RKFile_InstalledDirectory;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_Exists(const char* pFilename, bool retailArkOnly/* = false*/)
	{
		RKASSERT(pFilename, "RKFile_Exists - Filename is null");

		switch (g_eMode)
		{
		case RKFM_NORMAL:
		{
			// Attempt using a registered filesystem. Use the first one found.
			RKFileSystem* pCurrentFileSystem = g_pRegisteredSystems;
			for (uint32 i = 0; i < g_iNumRegisteredSystems; ++i)
			{
				//RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Exists - File system not registered");
				RKASSERT(pCurrentFileSystem->Exists, "RKFile_Exists - File system not completely implemented.");
				if (pCurrentFileSystem->Exists(pFilename))
					return true;
				++pCurrentFileSystem;
			}
		}break;
		case RKFM_NATIVEONLY:
		{
			RKFileSystem* pNativeFileSystem = GetRegisteredFileSystem(FS_NATIVE);
			if (pNativeFileSystem && pNativeFileSystem->Exists)
				return pNativeFileSystem->Exists(pFilename);
		}break;
		case RKFM_ARKONLY:
		{
			RKFileSystem* pArkFileSystem = GetRegisteredFileSystem(FS_ARK);
			if (pArkFileSystem && pArkFileSystem->Exists)
				return pArkFileSystem->Exists(pFilename);
		}break;
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_ExistsAbs(const char* pFullFilepath)
	{
		RKASSERT(pFullFilepath, "RKFile_ExistsAbs - File path is null");

		// Force native filesystem
		RKFileSystem* pNativeFileSystem = GetRegisteredFileSystem(FS_NATIVE);
		if (pNativeFileSystem)
		{
			RKASSERT(pNativeFileSystem->ExistsAbs, "RKFile_ExistsAbs - File system not completely implemented.");
			return pNativeFileSystem->ExistsAbs(pFullFilepath);
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_DirExists(const char* pDirName)
	{
		RKASSERT(pDirName, "RKFile_DirExists - Directory path is null");

		char pTemp[RKSTRING_LENGTH_TEMP] = { 0 };
		RKFile_CreateInstalledPathFast(pTemp, pDirName);
		if (!RKFile_DirExistsAbs(pTemp))
		{
			RKFile_CreateLocalPathFast(pTemp, pDirName);
			return RKFile_DirExistsAbs(pTemp);
		}
		return true;
	}

	//!>> ----------------------------------------------------------------------------

	bool RKFile_DirExistsAbs(const char* pFullPath)
	{
		RKASSERT(pFullPath, "RKFile_DirExistsAbs - Directory path is null");
		RKString Subdir = pFullPath;
		if (Subdir.EndsWith("/"))
		{
			Subdir.StripRight(1);
		}
		const char* pStripPath = Subdir.GetString();
		if (0 == _access(pStripPath, 0))
		{
			struct _stat statbuf;
			memset(&statbuf, 0, sizeof(statbuf));
			int rVal = _stat(pStripPath, &statbuf);
			if (rVal != 0)
			{
				return false;
			}
			return (_S_IFDIR & statbuf.st_mode) != 0;
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_CreateDir(const char* pDirName)
	{
		RKASSERT(pDirName, "RKFile_CreateDir - Directory path is null");

		return 0 == _mkdir(pDirName);
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_CreateDirAbs(const char* pFullPath)
	{
		RKASSERT(pFullPath, "RKFile_CreateDirAbs - Directory path is null");

		// This is a bit of a hack. 
		// Iterate through absolute path till we find "data/"
		// the chars after this string can be used as the relative path
		const char* pDataDir = "data/";
		const char* pSubStr = strstr(pFullPath, pDataDir);
		if (pSubStr)
		{
			uint32 iLen = strlen(pDataDir);
			if (strlen(pSubStr) > iLen) // Make sure there IS char's after the "data/" substring
			{
				pSubStr += iLen;
				return 0 == _mkdir(pSubStr);
			}
		}

		return false; // Invalid filename
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_RenameAbs(const char* pOldName, const char* pNewName)
	{
		RKASSERT(pOldName, "RKFile_RenameAbs - pOldName is null");
		RKASSERT(pNewName, "RKFile_RenameAbs - pNewName is null");
#if defined OS_W8 || defined OS_W10
		return 0 == _wrename(Win8Toolkit::ConvertUnicodedFolderPath(pOldName).c_str(), Win8Toolkit::ConvertUnicodedFolderPath(pNewName).c_str());
#else
		return 0 == rename(pOldName, pNewName);
#endif
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_GetAbsSubdirectoryPath(const char* pSubdir, RKString& outAbsSubdirPath)
	{
		RKASSERT(pSubdir, "RKFile_GetAbsSubdirectoryPath - pSubdir is null");

		RKString Subdir = pSubdir;
		if (Subdir.Length())
		{
			// Handle pSubdir beginning with a slash
			while (Subdir.StartsWith("\\") || Subdir.StartsWith("/"))
				Subdir.StripLeft(1);

			// Check for only slashes in pSubdir
			if (Subdir == "")
				return false;

			// pSubdir must end with a forward slash
			while (Subdir.EndsWith("\\"))
			{
				Subdir.StripRight(1);
				Subdir.Append("/");
			}
			if (!Subdir.EndsWith("/"))
				Subdir.Append("/");

			if (Subdir.Contains(RKFile_InstalledDirectory) ||
				Subdir.Contains(RKFile_LocalPath))
			{
				outAbsSubdirPath = Subdir;
				if (RKFile_DirExistsAbs(outAbsSubdirPath.GetString()))
					return true;
			}
			else
			{
				outAbsSubdirPath = RKFile_InstalledDirectory;
				outAbsSubdirPath.Append(Subdir);
				if (RKFile_DirExistsAbs(outAbsSubdirPath.GetString()))
					return true;

				outAbsSubdirPath = RKFile_LocalPath;
				outAbsSubdirPath.Append(Subdir);
				if (RKFile_DirExistsAbs(outAbsSubdirPath.GetString()))
					return true;
			}
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	uint64 RKFile_GetFreeSpace()
	{
		auto path = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
		std::wstring wsPath(path->Data());
		__int64 totalFreeBytes = 0;

		if (GetDiskFreeSpaceEx(wsPath.c_str(), NULL, NULL, (PULARGE_INTEGER)&totalFreeBytes) != 0)
		{
			return static_cast<uint64>(totalFreeBytes);
		}
		return 0;
	}

	void RKFile_CacheClear()
	{
		MemoryFileSystem::Clear();
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_CacheUnloadFile(const char* pFilename)
	{
		RKASSERT(pFilename, "pFilename is null");
		return MemoryFileSystem::UnloadFile(pFilename);
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_CacheFile(const char* pFilename)
	{
		RKASSERT(pFilename, "pFilename is null");

		// Get file from 
		RKFile* pFile = NULL;

		for (uint32 i = 0; i < g_iNumRegisteredSystems; ++i)
		{
			pFile = RKFile_ForceFileSystemOpen(pFilename, RKFM_READ, g_pRegisteredSystems[i].eType);
			if (pFile)
			{
				if (g_pRegisteredSystems[i].eType == FS_MEMORY)
					return true;// Already cached in memory file system
				break;// We have found the file we want
			}
		}

		if (pFile)
		{
			// Read the file into memory
			uint64 iSize = RKFile_GetSize(pFile);
			if (iSize == 0)
			{
				RKLOG_ERROR("File size is zero: %s", pFilename);
				RKFile_Close(&pFile);
				return false;
			}
			char* pData = RKNEWARRAY(char, iSize);
			uint64 iRead = RKFile_Read(pFile, pData, iSize);
			if (iSize != iRead)
			{
				RKLOG_ERROR("Error reading file: %s", pFilename);
				RKDELETEARRAY(pData);
				RKFile_Close(&pFile);
				return false;
			}
			RKFile_Close(&pFile);

			if (IsFileSystemRegistered(FS_MEMORY))
				return MemoryFileSystem::LoadFile(pFilename, pData, iSize);

			RKDELETEARRAY(pData);   //got the data, but no memory file system.. let's not leak ...
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_RegisterSubdirectory(const char* pSubdir)
	{
		RKASSERT(pSubdir, "RKFile_RegisterSubdirectory - pSubdir is null");

		RKString AbsSubdirPath;
		if (RKFile_GetAbsSubdirectoryPath(pSubdir, AbsSubdirPath))
		{
			// Is AbsSubdirPath a working directory
			if (0 == AbsSubdirPath.Compare(RKFile_InstalledDirectory) ||
				0 == AbsSubdirPath.Compare(RKFile_LocalPath))
				return true;

			// Is AbsSubdirPath already registered
			for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
			{
				if (0 == AbsSubdirPath.Compare((*g_pRegisteredSubdirectories)[i]->GetString()))
					return true;
			}

			RKString* pNew = RKNEW(RKString);
			*pNew = AbsSubdirPath;
			g_pRegisteredSubdirectories->Append(pNew);
			return true;
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	bool RKFile_UnregisterSubdirectory(const char* pSubdir)
	{
		RKASSERT(pSubdir, "RKFile_UnregisterSubdirectory - pSubdir is null");

		RKString AbsSubdirPath;
		if (RKFile_GetAbsSubdirectoryPath(pSubdir, AbsSubdirPath))
		{
			// Is AbsSubdirPath a working directory
			if (0 == AbsSubdirPath.Compare(RKFile_InstalledDirectory) ||
				0 == AbsSubdirPath.Compare(RKFile_LocalPath))
				return false;

			// Is AbsSubdirPath already registered
			RKString* pRegisteredSubdir = 0;
			for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
			{
				pRegisteredSubdir = (*g_pRegisteredSubdirectories)[i];
				if (0 == AbsSubdirPath.Compare(pRegisteredSubdir->GetString()))
				{
					RKDELETE(pRegisteredSubdir);
					g_pRegisteredSubdirectories->EraseAt(i);
					return true;
				}
			}
		}

		return false;
	}

	//!>> ----------------------------------------------------------------------------
	void RegisterFileSystem(RKFileSystem& filesystem)
	{
		if (!IsFileSystemRegistered(filesystem.eType))
		{
			// Turn flag on for given FileSystemType
			g_iRegisteredSystems |= filesystem.eType;

			// Copy new filesystem into filesystem array
			g_pRegisteredSystems[g_iNumRegisteredSystems] = filesystem;

			++g_iNumRegisteredSystems;
		}
	}

	//!>> ----------------------------------------------------------------------------
	void DeregisterFileSystem(EFileSystem eType)
	{
		if (IsFileSystemRegistered(eType))
		{
			for (uint32 i = 0; i < g_iNumRegisteredSystems; ++i)
			{
				// Turn flag off for given FileSystemType
				g_iRegisteredSystems ^= eType;

				// Delete unregistered filesystem from filesystem array
				if (g_pRegisteredSystems[i].eType == eType)
					memset(&g_pRegisteredSystems[i], 0, sizeof(RKFileSystem));

				--g_iNumRegisteredSystems;

				return;
			}
		}
	}

	//!>> ----------------------------------------------------------------------------
	bool IsFileSystemRegistered(EFileSystem eType)
	{
		return (g_iRegisteredSystems & eType) == eType;
	}

	//!>> ----------------------------------------------------------------------------
	RKFileSystem* GetRegisteredFileSystem(EFileSystem eType)
	{
		if (IsFileSystemRegistered(eType))
		{
			RKFileSystem* pCurrentFileSystem = g_pRegisteredSystems;
			for (uint32 i = 0; i < g_iNumRegisteredSystems; ++i)
			{
				if (pCurrentFileSystem->eType == eType)
					return pCurrentFileSystem;

				++pCurrentFileSystem;
			}
		}

		return 0;
	}

	//!>> ----------------------------------------------------------------------------
	void RKFile_SetMode(RKFileSystemMode eMode)
	{
		RKASSERT(eMode >= RKFM_NORMAL && eMode <= RKFM_ARKONLY, "Invalid RKFileSystem Mode");
		g_eMode = eMode;
	}

	RKFileSystemMode RKFile_GetMode()
	{
		return g_eMode;
	}
}
#endif//defined(OS_WP8) || defined(OS_W8)
