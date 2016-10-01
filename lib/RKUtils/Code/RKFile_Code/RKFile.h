//
// RKFile.h
// 
// Created by Daniel Stephens on 12/10/08.
// Edited by Liam de Koster-Kjaer 2012
// 

#ifndef RKFILE_H
#define RKFILE_H

#include "../Common/RKTypes.h"
#include "RKFileInternal.h"
namespace RKUtils
{
	enum RKFileAccessMode : unsigned int
	{
		RKFM_READ,
		RKFM_WRITE, // iOS - writes to the support dir and flags the file to stop it from being backed up to cloud
		RKFM_APPEND
	};

	enum RKFileSystemMode
	{
		RKFM_NORMAL,
		RKFM_NATIVEONLY,
		RKFM_ARKONLY
	};

#define RKFileNoFlags     0x00000000
#define RKFileForceNative 0x00000001
#define RKFileForceArk    0x00000002
	//#define RKFileDecrypt     0x00000004  // TODO: Implement other flags

	typedef struct RKFileInternal RKFile ;
	struct RKFileSystem ;
	class RKString;

	//!>> ----------------------------------------------------------------------------
	RKFile*     RKFile_Open(const char* pFilename, RKFileAccessMode eMode, int32 iFlag = RKFileNoFlags);
	RKFile*     RKFile_OpenAbs(const char* pFilename, RKFileAccessMode eMode);
	void        RKFile_Close(RKFile** ppFile);
	uint32      RKFile_Read(RKFile* pFile, void* pData, uint32 iSize);
	uint32      RKFile_Write(RKFile* pFile, void* pData, uint32 iSize);
	int32       RKFile_Seek(RKFile* pFile, int32 iOffset, int32 iSeekMode);
	int32       RKFile_Tell(RKFile* pFile);
	bool        RKFile_Remove(const char* pFilename);
	uint32      RKFile_GetSize(RKFile* pFile);
	bool        RKFile_CreatePath(char* pFullFilePathOut, const char* pFilename);     // For use with non-archived files - Returns true if file exists
	bool        RKFile_CreatePath(RKString& FullFilePathOut, const char* pFilename);  // For use with non-archived files - Returns true if file exists
	void        RKFile_InitModule();
	void        RKFile_DeinitModule();
	bool        RKFile_IsModuleInitialized();

	const char* RKFile_GetBundlePath();       // Cannot write to the bundle. 
	const char* RKFile_GetDocumentsPath();    // Only for user data - can get backed up to cloud.
	const char* RKFile_GetSupportFilesPath(); // Doesnt get backed up to cloud, application data.

	bool        RKFile_Exists(const char* pFilename, bool retailArkOnly = false);         // Ark / Native -> only Ark in retail build if retailArkOnly set
	bool        RKFile_ExistsAbs(const char* pFullFilePath);  // Native only
	bool        RKFile_DirExists(const char* pDirName);       // Native only (iOS - Support files path only)
	bool        RKFile_DirExistsAbs(const char* pFullPath);   // Native only
	bool        RKFile_CreateDir(const char* pDirName);       // Native only (iOS - Support files path only)
	bool        RKFile_CreateDirAbs(const char* pFullPath);   // Native only
	bool        RKFile_RenameAbs(const char* pOldName, const char* pNewName);

	uint64      RKFile_GetFreeSpace();
	bool        RKFile_CacheFile(const char* pFilename);
	bool        RKFile_CacheUnloadFile(const char* pFilename);
	void        RKFile_CacheClear();

	bool        RKFile_RegisterSubdirectory(const char* pSubdir);
	bool        RKFile_UnregisterSubdirectory(const char* pSubdir);

#if _RK_USE_RKVIEWER==1
	void              RKFile_SetWorkingDirectory(const char* pNewPath);
	void              RKFile_SetMode(RKFileSystemMode eMode);
	RKFileSystemMode  RKFile_GetMode();
#endif  // _RK_USE_RKVIEWER
}
#endif // RKFILE_H
