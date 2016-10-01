//
// RKFile.mm (iOS)
// 
// Created by Daniel Stephens on 12/10/08.
// Edited by Liam de Koster-Kjaer 2012
// 
#ifdef OS_IOS
//!>> ----------------------------------------------------------------------------
#include "RKFile.h"
#include "RKFileInternal.h"
#include "RKMacros.h"
#include "RKString.h"
//#include "CasualCoreConfig.h"
//#include "RKNSLog.h"
#include <sys/stat.h>
#import "Foundation/Foundation.h"
#import "CoreFoundation/CFString.h"
#import "UIKit/UIDevice.h"

#if CC_USE_NETWORK_FS==1
#  include "DebugConnection/DebugHost.h"
#  include "DebugConnection/LiveUpdateManager.h"
#  include "DebugConnection/NetworkFS.h"
#  include "DebugConnection/NetworkFSTrigger.h"
#endif

#include "../Include/ArkFileSystem.h"

#include "../Include/NativeFileSystem.h"
#include "../Include/MemoryFileSystem.h"
namespace RKUtils
{
//!>> ----------------------------------------------------------------------------
char  RKFile_SupportFilesPath[RKSTRING_LENGTH_TEMP] = { 0 };
char  RKFile_BundlePath[RKSTRING_LENGTH_TEMP] = { 0 };
char  RKFile_DocumentsPath[RKSTRING_LENGTH_TEMP] = { 0 };
bool  RKFile_SupportFilesPathIsValid = false;
bool  RKFile_IgnoreBundleFiles = false;

uint32				g_iNumRegisteredSystems = 0;
RKFileSystem	g_pRegisteredSystems[FILESYSTEM_COUNT];
uint32				g_iRegisteredSystems = 0;

RKList<RKString*>*  g_pRegisteredSubdirectories;

// [Liam] Testing! Remove me -----------------------------------------------------
void RKFileiOSUnitTests()
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
  
  char* pBuf = RKNEWARRAY(char, iSize*2);
  memset(pBuf, 0, iSize*2);
  uint32 iRead = RKFile_Read(pFile, pBuf, iSize);
  RKFile_Close(&pFile);

  RKASSERT(iRead == iSize, "RKFile_Read read an invalid number of bytes");
  
  uint32 iSize2 = strlen(pBuf);
  RKASSERT(iSize == iSize2, "Size of original and read data is inequal");
  for (uint32 i = 0; i < iSize; ++i)
  {
    RKASSERT(pData[i] == pBuf[i], "Original and read data does not match");
  }
  
  RKASSERT(RKFile_CreateDir("TESTDIRECTORY"), "RKFile_CreateDir failed to create directory");
  RKASSERT(RKFile_DirExists("TESTDIRECTORY"), "RKFile_DirExists failed to find existing directory");
  RKASSERT(!RKFile_DirExists("testiesdontexist"), "RKFile_DirExists found directory that shouldn't exist");
  
  const char* pOtherFilename = "RenamedTestFile.txt";

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
  const char* pFlagData = "testtesttesttesttesttesttesttesttesttest";
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
void RKFile_GetSystemPath(char* pStorage, NSSearchPathDirectory type)
{
  NSArray* paths = NSSearchPathForDirectoriesInDomains(type, NSUserDomainMask, YES);
  NSString* basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
  RKASSERT([basePath length] < RKSTRING_LENGTH_TEMP, "Path is too long");
  strncpy(pStorage, [basePath UTF8String], RKSTRING_LENGTH_TEMP);
  strncat(pStorage, "/", RKSTRING_LENGTH_TEMP - strlen(pStorage));  // End system path with a slash
  RKASSERT(RKString_Length(pStorage) > 0, "Error - No path to directory");
}

//!>> ----------------------------------------------------------------------------
void EnsureSupportPathIsValid()
{
  if (RKFile_SupportFilesPathIsValid)
    return;
  
  NSString* supportPath = [[[NSString alloc] initWithUTF8String:RKFile_SupportFilesPath] autorelease];
  
  BOOL isDirectory = NO;
  BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath: supportPath isDirectory: &isDirectory];
  
  if (exists)
  {
    RKASSERT(isDirectory, "Application support directory is not a directory");
    RKFile_SupportFilesPathIsValid = isDirectory;
  }
  else
  {
    BOOL created = [[NSFileManager defaultManager] createDirectoryAtPath:supportPath withIntermediateDirectories:YES attributes:nil error:nil];
    if (!created)
      printf("ERROR: Unable to create application support directory"); // it might happen if we are totally out of disk space, we should handle that situation
      
    RKFile_SupportFilesPathIsValid = created;
  }
}

//!>> ----------------------------------------------------------------------------
void RKFile_CreateSupportPath(char* pFullFilePathOut, const char* pFilename)
{
  EnsureSupportPathIsValid();
  strcpy(pFullFilePathOut, RKFile_SupportFilesPath);
  strcat(pFullFilePathOut, pFilename);
}

//!>> ----------------------------------------------------------------------------
void RKFile_CreateSupportPath(RKString& FullFilePathOut, const char* pFilename)
{
  EnsureSupportPathIsValid();
  FullFilePathOut = RKFile_SupportFilesPath;
  FullFilePathOut += pFilename;
}

//!>> ----------------------------------------------------------------------------
void RKFile_CreateBundlePath(char* pFullFilePathOut, const char* pFilename)
{
  strcpy(pFullFilePathOut, RKFile_BundlePath);
  strcat(pFullFilePathOut, pFilename);
}

//!>> ----------------------------------------------------------------------------
void RKFile_CreateBundlePath(RKString& FullFilePathOut, const char* pFilename)
{
  FullFilePathOut = RKFile_BundlePath;
  FullFilePathOut += pFilename;
}

//!>> ----------------------------------------------------------------------------
void RKFile_CreateDocumentsPath(char* pFullFilePathOut, const char* pFilename)
{
  strcpy(pFullFilePathOut, RKFile_DocumentsPath);
  strcat(pFullFilePathOut, pFilename);
}

//!>> ----------------------------------------------------------------------------
void RKFile_CreateDocumentsPath(RKString& FullFilePathOut, const char* pFilename)
{
  FullFilePathOut = RKFile_DocumentsPath;
  FullFilePathOut += pFilename;
}

//!>> ----------------------------------------------------------------------------
void RKFile_InitModule()
{
  RKFile_IgnoreBundleFiles = false;
  
  // Check iOS version
  
  NSString* iOSVersion = [[UIDevice currentDevice] systemVersion];
  bool version51OrNewer = [iOSVersion compare: @"5.1" options:NSNumericSearch] != NSOrderedAscending;
  
  // Get application support files path
  // http://developer.apple.com/library/ios/#documentation/FileManagement/Conceptual/FileSystemProgrammingGUide/FileSystemOverview/FileSystemOverview.html
  if (version51OrNewer)
    RKFile_GetSystemPath(RKFile_SupportFilesPath, NSApplicationSupportDirectory);
  else
    RKFile_GetSystemPath(RKFile_SupportFilesPath, NSCachesDirectory);
  
  // Validate application support files path, create if not exists
  EnsureSupportPathIsValid();  

	// Get bundle path
	CFBundleRef MainBundle = CFBundleGetMainBundle();
	RKASSERT(MainBundle, "Error - Could not find main bundle");  
  CFURLRef resURL = CFBundleCopyBundleURL(MainBundle);
  CFStringRef pathStr = CFURLCopyPath(resURL);
  const char* pPath = [(NSString*)pathStr UTF8String];    
  if(strlen(pPath) < RKSTRING_LENGTH_TEMP)
    strcpy(RKFile_BundlePath, pPath);
  else
    RKASSERT(false, "Error - Could not get main bundle path");
  
  // Release CF resources
  if(resURL)
    CFRelease(resURL);
  if(pathStr)
    CFRelease(pathStr);
  
	// Get documents path
  RKFile_GetSystemPath(RKFile_DocumentsPath, NSDocumentDirectory);

  g_pRegisteredSubdirectories = RKNEW(RKList<RKString*>);
  
  // Initialise FileSystems in order!  
#if CC_USE_NETWORK_FS==1
  if (dbg::CheckNetworkFSShouldBeEnabled())
  {
    RKFile_IgnoreBundleFiles = true;
    NSString* cacheDir = [NSString stringWithUTF8String:RKFile_GetSupportFilesPath()];
    cacheDir = [cacheDir stringByAppendingString:@"/NetworkFS_Cache/"];
    
    BOOL isDirectory = NO;
    BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath: cacheDir isDirectory: &isDirectory];

    if (!exists)
    {
      BOOL created = [[NSFileManager defaultManager] createDirectoryAtPath:cacheDir withIntermediateDirectories:YES attributes:nil error:nil];
      RKASSERT(created, "Unable to create network fs cache directory");
    }
    else
    {
      RKASSERT(isDirectory, "");
    }
    
    RKASSERT(dbg::DebugHost::IsValid(), "Unable to use network filesystem without debug host initialized");
    dbg::NetworkFS::Init([cacheDir UTF8String], dbg::DebugHost::GetInstance());
    dbg::LiveUpdateManager::Create(dbg::DebugHost::GetInstance());
  }
#endif
  
  MemoryFileSystem::Initialise();
  
  ArkFileSystem::Initialise();
  
  NativeFileSystem::Initialise();
}

//!>> ----------------------------------------------------------------------------
void RKFile_DeinitModule()
{
  MemoryFileSystem::ShutDown();
  
  NativeFileSystem::ShutDown();
  
#if CC_USE_ARK==1
	ArkFileSystem::ShutDown();
#endif
  
#if CC_USE_NETWORK_FS==1
  if (dbg::NetworkFS::IsValid())
  {
    dbg::LiveUpdateManager::Destroy();
    dbg::NetworkFS::Term();
  }
#endif

  memset(g_pRegisteredSystems, 0, sizeof(RKFileSystem) * FILESYSTEM_COUNT);
  g_iNumRegisteredSystems = 0;
  g_iRegisteredSystems = 0;

  if (g_pRegisteredSubdirectories)
  {
    for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
      RKDELETE((*g_pRegisteredSubdirectories)[i]);
    RKDELETE(g_pRegisteredSubdirectories);
  }
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
int RKFile_Seek(RKFile* pFile, int iOffset, int iSeekMode)
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
  
  // Only native filesystem
  // Only support and documents folders
  
  // Force native filesystem
  RKFileSystem* pNativeFileSystem = GetRegisteredFileSystem(FS_NATIVE);
  if (pNativeFileSystem)
  {
    RKASSERT(pNativeFileSystem->ExistsAbs, "RKFile_Remove - File system not completely implemented.");
    
    char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };
      
    RKFile_CreateSupportPath(pFullPath, pFilename);
    if (pNativeFileSystem->ExistsAbs(pFullPath))
      return (0 == remove(pFullPath));
     
    RKFile_CreateDocumentsPath(pFullPath, pFilename);
    if (pNativeFileSystem->ExistsAbs(pFullPath))
      return (0 == remove(pFullPath));
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
  RKFile_CreateSupportPath(pFullFilePathOut, pFilename);
  if (RKFile_ExistsAbs(pFullFilePathOut))
    return true;
  
  if (!RKFile_IgnoreBundleFiles)
  {
    RKFile_CreateBundlePath(pFullFilePathOut, pFilename);
    if (RKFile_ExistsAbs(pFullFilePathOut))
      return true;
  }
  
  RKFile_CreateSupportPath(pFullFilePathOut, pFilename);
  return false;
}

bool RKFile_CreatePath(RKString& FullFilePathOut, const char* pFilename)
{
  char path[RKSTRING_LENGTH_TEMP];
  bool ok = RKFile_CreatePath(path, pFilename);
  FullFilePathOut = path;
  return ok;
}

//!>> ----------------------------------------------------------------------------
const char* RKFile_GetBundlePath()
{
  return RKFile_BundlePath;
}

//!>> ----------------------------------------------------------------------------
const char* RKFile_GetSupportFilesPath()
{
  EnsureSupportPathIsValid();
  return RKFile_SupportFilesPath;
}

//!>> ----------------------------------------------------------------------------
const char* RKFile_GetDocumentsPath()
{
  return RKFile_DocumentsPath;
}

//!>> ----------------------------------------------------------------------------
bool RKFile_Exists(const char* pFilename, bool retailArkOnly /* = false*/)
{
  RKASSERT(pFilename, "RKFile_Exists - Filename is null");
#ifdef LOAD_ARK_ONLY
  if(retailArkOnly)
  {
    RKFileSystem* pFileSystem = GetRegisteredFileSystem(FS_ARK);
    if(pFileSystem)
    {
      if (pFileSystem->Exists(pFilename))
        return true;
    }
    return false;
  }
#endif
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
  RKASSERT(pDirName, "RKFile_DirExists - Directory name is null");
  
  char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };
  RKFile_CreateSupportPath(pFullPath, pDirName);
  return RKFile_DirExistsAbs(pFullPath);
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
  
  // Only native filesystem
  // Only support folder
  
  char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };
  RKFile_CreateSupportPath(pFullPath, pDirName);
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
      
    if (Subdir.Contains(RKFile_SupportFilesPath) ||
        Subdir.Contains(RKFile_BundlePath) ||
        Subdir.Contains(RKFile_DocumentsPath))
    {
      outAbsSubdirPath = Subdir;
      if (RKFile_DirExistsAbs(outAbsSubdirPath.GetString()))
        return true;
    }
    else
    {
      outAbsSubdirPath = RKFile_SupportFilesPath;
      outAbsSubdirPath.Append(Subdir);
      if (RKFile_DirExistsAbs(outAbsSubdirPath.GetString()))
        return true;

      outAbsSubdirPath = RKFile_BundlePath;
      outAbsSubdirPath.Append(Subdir);
      if (RKFile_DirExistsAbs(outAbsSubdirPath.GetString()))
        return true;

      outAbsSubdirPath = RKFile_DocumentsPath;
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
  uint64 totalSpace     = 0;
  uint64 totalFreeSpace = 0;
  
  __autoreleasing NSError* error = nil;
  NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSDictionary* dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject] error: &error];
  
  if (dictionary)
  {
    NSNumber* fileSystemSizeInBytes     = [dictionary objectForKey: NSFileSystemSize];
    NSNumber* freeFileSystemSizeInBytes = [dictionary objectForKey:NSFileSystemFreeSize];
    totalSpace                          = [fileSystemSizeInBytes unsignedLongLongValue];
    totalFreeSpace                      = [freeFileSystemSizeInBytes unsignedLongLongValue];

   // RKNSLOG(@"Memory Capacity of %llu MiB with %llu MiB Free memory available.", ((totalSpace/1024ll)/1024ll), ((totalFreeSpace/1024ll)/1024ll));
  }
  else
  {
  //  RKNSLOG(@"Error Obtaining System Memory Info: Domain = %@, Code = %d", [error domain], [error code]);
  }

  return totalFreeSpace;
}

//!>> ----------------------------------------------------------------------------
void RKFile_CacheClear()
{
  MemoryFileSystem::Clear();
}

//!>> ----------------------------------------------------------------------------
bool        RKFile_CacheUnloadFile(const char* pFilename)
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
      if(g_pRegisteredSystems[i].eType == FS_MEMORY)
        return true;// Already cached in memory file system
      break;// We have found the file we want
    }
  }
  
  if(pFile)
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
    if(iSize != iRead)
    {
      RKLOG_ERROR("Error reading file: %s", pFilename);
      RKDELETEARRAY(pData);
      RKFile_Close(&pFile);
      return false;
    }
    RKFile_Close(&pFile);
    
    if(IsFileSystemRegistered(FS_MEMORY))
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
    if (0==AbsSubdirPath.Compare(RKFile_SupportFilesPath) ||
        0==AbsSubdirPath.Compare(RKFile_BundlePath) ||
        0==AbsSubdirPath.Compare(RKFile_DocumentsPath))
      return true;

    // Is AbsSubdirPath already registered
    for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
    {
      if (0==AbsSubdirPath.Compare((*g_pRegisteredSubdirectories)[i]->GetString()))
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
    if (0==AbsSubdirPath.Compare(RKFile_SupportFilesPath) ||
        0==AbsSubdirPath.Compare(RKFile_BundlePath) ||
        0==AbsSubdirPath.Compare(RKFile_DocumentsPath))
      return false;

    // Is AbsSubdirPath already registered
    RKString* pRegisteredSubdir = 0;
    for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
    {
      pRegisteredSubdir = (*g_pRegisteredSubdirectories)[i];
      if (0==AbsSubdirPath.Compare(pRegisteredSubdir->GetString()))
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
    
}
#endif
