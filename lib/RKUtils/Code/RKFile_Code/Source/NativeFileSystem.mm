// 
// NativeFileSystem.mm
// For iOS platform. For Win32 and Android, see NativeFileSystem.cpp
// Liam de Koster-Kjaer
// February 2012
// 

#include "NativeFileSystem.h"
#include "RKFileInternal.h"
#include "RKFile.h"
#include "RKMacros.h"
#include "RKString.h"

#include <cstdio>
#include <cstring>
#include <sys/stat.h>

#import <Foundation/NSString.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSPredicate.h>
#import <Foundation/NSBundle.h>
#import <UIKit/UIDevice.h>

namespace RKUtils {

extern NSString* const    NSURLIsExcludedFromBackupKey WEAK_IMPORT_ATTRIBUTE;
extern bool               RKFile_IgnoreBundleFiles;
extern RKList<RKString*>* g_pRegisteredSubdirectories;

//!>> ----------------------------------------------------------------------------
void NativeDisableBackupsForPath(const char* fpath)
{
  NSString* iOSVersion = [[UIDevice currentDevice] systemVersion];
  bool version51OrNewer = [iOSVersion compare: @"5.1" options:NSNumericSearch] != NSOrderedAscending;

  if (version51OrNewer)
  {
    NSString* fpathString = [[[NSString alloc] initWithUTF8String:fpath] autorelease];
    BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:fpathString];
    RKASSERT(exists, "Can't exclude non-existing file from backup");
    if(exists)
    {
      NSURL* url = [NSURL fileURLWithPath: fpathString];
      BOOL success = [url setResourceValue: [NSNumber numberWithBool:YES]
                                    forKey: NSURLIsExcludedFromBackupKey
                                     error: nil];
      RKASSERT(success, "Unable to exclude file from backup");
    }
  }
}

//!>> ----------------------------------------------------------------------------
bool NativeFileSystem::Initialise()
{
	bool bSuccess = true;

	// Register NativeFileSystem
	RKFileSystem fs;
	fs.eType      = FS_NATIVE;
  fs.Exists     = &NativeExists;
  fs.ExistsAbs  = &NativeExistsAbs;
	fs.Open       = &NativeOpen;
	fs.Close      = &NativeClose;
	fs.Read       = &NativeRead;
	fs.Write      = &NativeWrite;
	fs.Seek       = &NativeSeek;
  fs.Tell       = &NativeTell;
	fs.GetSize    = &NativeSize;
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
  if (0 == access(pFullPath, F_OK))
  {
    struct stat statbuf;
    stat(pFullPath, &statbuf);
    return !(S_ISDIR(statbuf.st_mode));
  }

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

  if (!RKFile_IgnoreBundleFiles)
  {
    strcpy(pFullPath, RKFile_GetBundlePath());
    strcat(pFullPath, pFilename);
    if (NativeExistsAbs(pFullPath))
      return true;
  }

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
  char pFullPath[RKSTRING_LENGTH_TEMP] = {0};
  const char* pMode = 0;
  switch (eMode)
  {
    case RKFM_READ:
    {
      pMode = "rb";
      if (!RKFile_CreatePath(pFullPath, pFilename))
      {
        // Check registered subdirectories in order of registration
        for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
        {
          strcpy(pFullPath, (*g_pRegisteredSubdirectories)[i]->GetString());
          strcat(pFullPath, pFilename);
          pFileHandle = fopen(pFullPath, pMode);
          if (pFileHandle)
          {
            RKLOGt_INFO("NativeFS", "Opened file: %s, %s", pFullPath, pMode);
            pFile = RKNEW(RKFileInternal);
            pFile->pFileHandle = pFileHandle;
            return pFile;
          }
        }
          
        // Else just try to open the file ( worst case )
        pFileHandle = fopen(pFilename, pMode);
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
    } break;
    case RKFM_APPEND:
    {
      strcpy(pFullPath, RKFile_GetSupportFilesPath());
      strcat(pFullPath, pFilename);
      pMode = "ab+";
    } break;
  }

  pFileHandle = fopen(pFullPath, pMode);
  if (pFileHandle)
  {
    RKLOGt_INFO("NativeFS", "%s %s", pFullPath, pMode);
    
    if (eMode == RKFM_WRITE || eMode == RKFM_APPEND)
      NativeDisableBackupsForPath(pFullPath);
    
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
int NativeFileSystem::NativeSeek(RKFileInternal* pFile, int iOffset, int iSeekMode)
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
	int length = 0;

	FILE* pFileHandle = (FILE*)pFile->pFileHandle;
	if (pFileHandle)
	{
		fseek(pFileHandle, 0, SEEK_END);
		length = ftell(pFileHandle);
		fseek(pFileHandle, 0, SEEK_SET);
	}

	return length;
}
}
