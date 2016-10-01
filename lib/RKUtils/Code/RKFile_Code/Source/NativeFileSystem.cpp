// 
// NativeFileSystem.cpp
// For Windows and Android platforms. For iOS, see NativeFileSystem.mm
// Liam de Koster-Kjaer
// February 2012
// 
#if !defined(OS_WP8) && !defined(OS_W8) && !defined OS_W10
#include "../Include/NativeFileSystem.h"
#include <cstdio>
#include <cstring>
#include "Common/RKMacros.h"
#include "RKString_Code/RKString.h"
#include "RKFile_Code/RKFileInternal.h"
#include "RKFile_Code/RKFile.h"
#if defined (GAME_WIN32)
  #include <io.h>
#elif defined (GAME_ANDROID)
  #include <unistd.h>
#endif
#include <sys/stat.h>
namespace RKUtils {

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
#if defined (GAME_WIN32)
  if (0 == _access(pFullPath, 0))
  {
    struct _stat statbuf;
    _stat(pFullPath, &statbuf);
    return !(_S_IFDIR & statbuf.st_mode);
  }
#elif defined (GAME_ANDROID)
  if (0 == access(pFullPath, F_OK))
  {
    struct stat statbuf;
    stat(pFullPath, &statbuf);
    return !(S_ISDIR(statbuf.st_mode));
  }
#endif
  return false;
}

//!>> ----------------------------------------------------------------------------
bool NativeFileSystem::NativeExists(const char* pFilename)
{
  // Win32 only - we allow absolute filepaths for tools etc...
#ifdef GAME_WIN32
  if (IsAbsFilename(pFilename))
    return NativeExistsAbs(pFilename);
#endif

  char pFullPath[RKSTRING_LENGTH_TEMP] = { 0 };
  strcpy(pFullPath, RKFile_GetSupportFilesPath());  // Returns working data directory
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

  const char* pMode = 0;
  switch (eMode)
  {
  case RKFM_READ:
    pMode = "rb";
    break;
  case RKFM_WRITE:
    pMode = "wb";
    break;
  case RKFM_APPEND:
    pMode = "ab+";
    break;
  }

  char pFullPath[RKSTRING_LENGTH_TEMP] = {0};
#ifdef GAME_WIN32 // Win32 only - we allow absolute filepaths for tools etc...
  if (IsAbsFilename(pFilename))
    strcat(pFullPath, pFilename);
  else
#endif
  {
    strcpy(pFullPath, RKFile_GetSupportFilesPath());  // Returns working data directory
    strcat(pFullPath, pFilename);
  }

  FILE* pFileHandle = fopen(pFullPath, pMode);
  if (pFileHandle)
  {
//    RKLOGt_INFO("NativeFS", "Opened file: %s, %s", pFullPath, pMode);
    pFile = RKNEW(RKFileInternal);
    pFile->pFileHandle = pFileHandle;
    return pFile;
  }
   //else try to open in bundle path
    
    strcpy(pFullPath, (RKFile_GetBundlePath() + RKString("Data/")).GetString());
    strcat(pFullPath, pFilename);
    
    pFileHandle = fopen(pFullPath, pMode);
    if (pFileHandle)
    {
        //    RKLOGt_INFO("NativeFS", "Opened file: %s, %s", pFullPath, pMode);
        pFile = RKNEW(RKFileInternal);
        pFile->pFileHandle = pFileHandle;
        return pFile;
    }
    
    //else open in local path (doc path)
    
    strcpy(pFullPath, RKFile_GetDocumentsPath());
    strcat(pFullPath, pFilename);
    
    pFileHandle = fopen(pFullPath, pMode);
    if (pFileHandle)
    {
        //    RKLOGt_INFO("NativeFS", "Opened file: %s, %s", pFullPath, pMode);
        pFile = RKNEW(RKFileInternal);
        pFile->pFileHandle = pFileHandle;
        return pFile;
    }
    
  if (eMode == RKFM_READ)
  {
    // Check registered subdirectories in order of registration
    for (uint32 i = 0, size = g_pRegisteredSubdirectories->Size(); i < size; ++i)
    {
      strcpy(pFullPath, (*g_pRegisteredSubdirectories)[i]->GetString());
      strcat(pFullPath, pFilename);
      pFileHandle = fopen(pFullPath, pMode);
      if (pFileHandle)
      {
//        RKLOGt_INFO("NativeFS", "Opened file: %s, %s", pFullPath, pMode);
        pFile = RKNEW(RKFileInternal);
        pFile->pFileHandle = pFileHandle;
        return pFile;
      }
    }
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
#endif// !defined(OS_WP8) && !defined(OS_W8) && !defined OS_W10