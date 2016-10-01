//
// RKFile.cpp (Android)
// 
// Created by Daniel Stephens on 12/10/08.
// Edited by Liam de Koster-Kjaer 2012
// 
#if defined(ANDROID_OS)
//!>> ----------------------------------------------------------------------------
#include "RK.h"
#include "RKFile.h"
#include "RKMacros.h"
#include "RKString.h"
#include "RKFileInternal.h"
#include "FileSystem/NativeFileSystem.h"
#include "FileSystem/ArkFileSystem.h"
#include "config_Android.h"
#include <android/log.h>
#include <sys/types.h>
#include <sys/stat.h>
extern char* m_gAppPath;

//!>> ----------------------------------------------------------------------------
char          RKFile_WorkingDirectory[RKSTRING_LENGTH_TEMP] = { 0 };
uint32        g_iNumRegisteredSystems = 0;
RKFileSystem  g_pRegisteredSystems[FILESYSTEM_COUNT];
uint32        g_iRegisteredSystems = 0;

// [Liam] Testing! Remove me -----------------------------------------------------
void RKFileAndroidUnitTests()
{
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");

  const char* pFilename = "TestFile.txt";

  if (RKFile_Exists(pFilename))
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_Exists found a file when it shouldn't have");

  RKFile* pFile = RKFile_Open(pFilename, RKFM_WRITE);
  if (!pFile)
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_Open failed to open file for writing");

  const char* pData = "TestTestTestTestTestTestTestTestTestTest";
  const uint32 iSize = strlen(pData); // 40

  uint32 iWritten = RKFile_Write(pFile, (void*)pData, iSize);
  if (iWritten != iSize)
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_Write wrote an invalid number of bytes");

  RKFile_Close(&pFile);

  if (!RKFile_Exists(pFilename))
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_Exists failed to find a file when it should have");

  pFile = RKFile_Open(pFilename, RKFM_READ);
  if (!pFile)
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_Open failed to open file for reading");

  char* pBuf = RKNEWARRAY(char, iSize*2);
  memset(pBuf, 0, iSize*2);
  uint32 iRead = RKFile_Read(pFile, pBuf, iSize);
  RKFile_Close(&pFile);

  if (iRead != iSize)
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_Read read an invalid number of bytes");

  uint32 iSize2 = strlen(pBuf);
  if (iSize != iSize2)
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "Size of original and read data is inequal");
  for (uint32 i = 0; i < iSize; ++i)
  {
    if (pData[i] != pBuf[i])
       __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "Original and read data does not match");
  }

  if (!RKFile_CreateDir("TESTDIRECTORY"))
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_CreateDir failed to create directory");
  if (!RKFile_DirExists("TESTDIRECTORY"))
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_DirExists failed to find existing directory");
  if (RKFile_DirExists("testiesdontexist"))
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_DirExists found directory that shouldn't exist");

  char* pOtherFilename = "RenamedTestFile.txt";

  char pFullFilename[RKSTRING_LENGTH_TEMP] = { 0 };
  RKFile_CreatePath(pFullFilename, pFilename);
  char pFullOtherFilename[RKSTRING_LENGTH_TEMP] = { 0 };
  RKFile_CreatePath(pFullOtherFilename, pOtherFilename);

  if (!RKFile_RenameAbs(pFullFilename, pFullOtherFilename))
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_RenameAbs failed to rename file");
  if (!RKFile_RenameAbs(pFullOtherFilename, pFullFilename))
     __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_RenameAbs failed to rename file");

  bool bRemoved = RKFile_Remove(pFilename);

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
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "Wrote invalid amount of test data");

  pFlagFile = RKFile_Open(pArchivedFileName, RKFM_READ, RKFileForceNative);  // A file that exists only in the archive
  if (pFlagFile)
  {
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "Succeed to find file that NativeFileSystem shouldn't be able to find");
    RKFile_Close(&pFlagFile);
  }
  pFlagFile = RKFile_Open(pTestFilename, RKFM_READ, RKFileForceArk);  // A file that exists only external to the archive
  if (pFlagFile)
  {
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "Succeed to find file that ArkFileSystem shouldn't be able to find");
    RKFile_Close(&pFlagFile);
  }

  pFlagFile = RKFile_Open(pTestFilename, RKFM_READ, RKFileForceNative);  // A file that exists only external to the archive
  if (pFlagFile)
    RKFile_Close(&pFlagFile);
  else
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "Failed to find file that NativeFileSystem should be able to find");

  pFlagFile = RKFile_Open(pArchivedFileName, RKFM_READ, RKFileForceArk);  // A file that exists only in the archive
  if (pFlagFile)
    RKFile_Close(&pFlagFile);
  else
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "Failed to find file that ArkFileSystem should be able to find");

  if (!RKFile_Remove(pTestFilename))
    __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*", "RKFile_Remove failed to remove file");

  // Testing RKFile_Open flags -----------------
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
  __android_log_print(ANDROID_LOG_INFO, "!@#$%^&*!@#$%^&*!@#$%^&*!@#$%^&*", "OMG HERE WE GO ITS TESTIES TIME WHOA!!!");
}
// [Liam] Testing! Remove me -----------------------------------------------------

//!>> ----------------------------------------------------------------------------
void RKFile_CreatePathFast(char* pFullFilePathOut, const char* pFilename)
{
  strcpy(pFullFilePathOut, RKFile_WorkingDirectory);
  strcat(pFullFilePathOut, pFilename);
}

//!>> ----------------------------------------------------------------------------
void RKFile_CreatePathFast(RKString& FullFilePathOut, const char* pFilename)
{
  FullFilePathOut = RKFile_WorkingDirectory;
  FullFilePathOut += pFilename;
}

//!>> ----------------------------------------------------------------------------
void RKFile_InitModule()
{
  // Get working data directory
  strncpy(RKFile_WorkingDirectory, m_gAppPath, RKSTRING_LENGTH_TEMP);
  RKASSERT(strlen(RKFile_WorkingDirectory), "Error - RKFile_InitModule failed to get the working directory");

  // [Liam] Initialise FileSystems in order!
  memset(g_pRegisteredSystems, 0, sizeof(RKFileSystem) * FILESYSTEM_COUNT);
  ArkFileSystem::Initialise();
  NativeFileSystem::Initialise();
}

//!>> ----------------------------------------------------------------------------
void RKFile_DeinitModule()
{
  ArkFileSystem::ShutDown();
  NativeFileSystem::ShutDown();
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

#ifndef ENABLE_DLC_SYSTEM
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
    RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Read - File system not registered");
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
    RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Write - File system not registered");
    RKASSERT(pCurrentFileSystem->Write, "RKFile_Write - File system not completely implemented.");
    return pCurrentFileSystem->Write(pFileInt, pData, iSize);
  }

  return 0;
}

//!>> ----------------------------------------------------------------------------
int32 RKFile_Seek(RKFile* pFile, int32 iOffset, int32 iSeekMode)
{
  RKASSERT(pFile, "RKFile_Seek - RKFile pointer is null");

  // Use the filesystem specified by the RKFile* otherwise fail
  RKFileInternal* pFileInt = (RKFileInternal*)pFile;
  RKFileSystem* pCurrentFileSystem = pFileInt->pFileSystem;
  if (pCurrentFileSystem)
  {
    RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Seek - File system not registered");
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
    RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Tell - File system not registered");
    RKASSERT(pCurrentFileSystem->Tell, "RKFile_Tell - File system not completely implemented.");
    return pCurrentFileSystem->Tell(pFileInt);
  }

  return 0;
}

//!>> ----------------------------------------------------------------------------
bool RKFile_Remove(const char* pFilename)
{
  RKASSERT(pFilename, "RKFile_Remove - Filename is null");

  char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };
  RKFile_CreatePathFast(pFullPath, pFilename);
  return (0 == remove(pFullPath));
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

  RKFile_CreatePathFast(pFullFilePathOut, pFilename);
  return RKFile_ExistsAbs(pFullFilePathOut);
}

//!>> ----------------------------------------------------------------------------
bool RKFile_CreatePath(RKString& FullFilePathOut, const char* pFilename)
{
  RKASSERT(pFilename, "RKFile_CreatePath - Filename is null");

  RKFile_CreatePathFast(FullFilePathOut, pFilename);
  return RKFile_ExistsAbs(FullFilePathOut.GetString());
}

//!>> ----------------------------------------------------------------------------
const char* RKFile_GetBundlePath()
{
  return RKFile_WorkingDirectory;
}

//!>> ----------------------------------------------------------------------------
const char* RKFile_GetSupportFilesPath()
{
  return RKFile_WorkingDirectory;
}

//!>> ----------------------------------------------------------------------------
const char* RKFile_GetDocumentsPath()
{
  return RKFile_WorkingDirectory;
}

//!>> ----------------------------------------------------------------------------
bool RKFile_Exists(const char* pFilename)
{
  RKASSERT(pFilename, "RKFile_Exists - Filename is null");

  // Attempt using a registered filesystem. Use the first one found.
  RKFileSystem* pCurrentFileSystem = g_pRegisteredSystems;
  for (uint32 i = 0; i < g_iNumRegisteredSystems; ++i)
  {
    RKASSERT(IsFileSystemRegistered(pCurrentFileSystem->eType), "RKFile_Exists - File system not registered");
    RKASSERT(pCurrentFileSystem->Exists, "RKFile_Exists - File system not completely implemented.");
    if (pCurrentFileSystem->Exists(pFilename))
      return true;

    ++pCurrentFileSystem;
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
  RKFile_CreatePathFast(pTemp, pDirName);
  return RKFile_DirExistsAbs(pTemp);
}

//!>> ----------------------------------------------------------------------------
bool RKFile_DirExistsAbs(const char* pFullPath)
{
  RKASSERT(pFullPath, "RKFile_DirExistsAbs - Directory path is null");

  if (0 == access(pFullPath, F_OK))
  {
    struct stat statbuf;
    stat(pFullPath, &statbuf);
    return (bool)S_ISDIR(statbuf.st_mode);
  }
  return false;
}

//!>> ----------------------------------------------------------------------------
bool RKFile_CreateDir(const char* pDirName)
{
  RKASSERT(pDirName, "RKFile_CreateDir - Directory path is null");

  char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };
  RKFile_CreatePathFast(pFullPath, pDirName);
  return RKFile_CreateDirAbs(pFullPath);
}

//!>> ----------------------------------------------------------------------------
bool RKFile_CreateDirAbs(const char* pFullPath)
{
  RKASSERT(pFullPath, "RKFile_CreateDirAbs - Directory path is null");

  return 0 == mkdir(pFullPath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
}

//!>> ----------------------------------------------------------------------------
bool RKFile_RenameAbs(const char* pOldName, const char* pNewName)
{
  RKASSERT(pOldName, "RKFile_RenameAbs - pOldName is null");
  RKASSERT(pNewName, "RKFile_RenameAbs - pNewName is null");

  return 0 == rename(pOldName, pNewName);
}

//!>> ----------------------------------------------------------------------------
void RegisterFileSystem(RKFileSystem& filesystem)
{
  assert(filesystem.eType > INVALID_FILESYSTEM);

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
#endif