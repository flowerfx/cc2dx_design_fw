// 
// ArkManager.cpp
// Liam de Koster-Kjaer
// January 2012
// 

//#include "CasualCoreConfig.h"

#include <cstdlib>
#if defined(GAME_WIN32)
#include <direct.h>
#endif
#include "../Include/ArkManager.h"
#include "../Include/Ark.h"
#include "../RKFile.h"
#include "../../RKTool_Code/RKList.h"


#if defined(OS_WP8) || defined(OS_W8) || defined OS_W10
#include "../../../../WinRTAPIsEmul/WinRTAPIsEmul.h"
#endif

#if defined(OS_W8)|| defined OS_W10
#include "../../RKPlatform/Win8.1/Win8Shared.h"
#endif
namespace RKUtils
{
	#define EXTRACTEDARKLIST  "ExtractedArkList.txt"
	#define DECOMPRESSIONLOG  "ArkDecompressionInfo.txt"

	static const uint32 g_sTextureExtensionCount = 3;
	static const char* g_sTextureExtensions[g_sTextureExtensionCount] = { "pvr", "png", "tga" };

	ArkManager::ArkManager()
	{
#if USE_EXTRAC_THREAD
	  m_pExtractThread      = 0;
	  m_pExtractMutex       = 0;
#endif
	  m_bExtracting         = false;
	  m_fExtractProgress    = 0.0f;
	  m_TotalBytesExtracted = 0;
	  m_TotalBytesToExtract = 0;
	  m_FileTable.Init(5);  // Incase you pass zero - then it will not actually initialise (will still throw asserts unless you call Init with a positive value)

	#ifdef ARK_LOG_DECOMPRESSION_INFO
	  // Initialise the decompression timer 
	  m_DecompressionTimer.Init();
	  char pFilepath[256]; memset(pFilepath, 0, sizeof(pFilepath));
	  RKFile_CreatePath(pFilepath, DECOMPRESSIONLOG);
	  FILE* pFile = fopen(pFilepath, "w");
	  if (pFile)
	  {
		fputs("ArkFileSystem - Decompression Info Log - Gameloft Auckland 2013\n\n", pFile);
		fclose(pFile);
	  }
	#endif  // ARK_LOG_DECOMPRESSION_INFO
	}

	ArkManager::~ArkManager()
	{
	  FileTable::Iterator it = m_FileTable.Begin();
	  while (it.IsValid())
	  {
		ArkFileInfo* pInfo = it.Value();
		if (pInfo)
		  RKDELETE(pInfo);
		++it;
	  }
	  m_FileTable.Clear();
	  m_FileTable.Deinit();
#if USE_EXTRAC_THREAD
	  if (m_pExtractMutex)
	    RKCriticalSection_Destroy(&m_pExtractMutex);

	  if (m_pExtractThread)
	    RKThread_Destroy(&m_pExtractThread);
#endif
	  for (uint32 i = 0; i < m_Arks.Size(); ++i)
	  {
		RKDELETE(m_Arks[i]);
		m_Arks[i] = 0;
	  }
	  m_Arks.Clear();

	#ifdef ARK_LOG_DECOMPRESSION_INFO
	  // Write total decompression time to decompression log file
	  char pTempBuf[256]; memset(pTempBuf, 0, sizeof(pTempBuf));
	  RKFile_CreatePath(pTempBuf, DECOMPRESSIONLOG);
	  FILE* pFile = fopen(pTempBuf, "a");
	  if (pFile)
	  {
		memset(pTempBuf, 0, sizeof(pTempBuf));
		sprintf(pTempBuf, "\nTotal Decompression Time = %f", m_fTotalDecompressionTime);
		fputs(pTempBuf, pFile);
		fclose(pFile);
	  }
	#endif  // ARK_LOG_DECOMPRESSION_INFO
	}

/**
* Initialise
* Win32:    Loads all arks in data directory
* iOS:      Loads startup arks only, other arks are to be loaded from the DLCManager once we have a manifest
* Android:  Loads all arks in data directory
* Then calls GenerateFileTable to generate a hash table of filename | ARK* pairs for fast lookup.
* Liam de Koster-Kjaer
**/
	bool ArkManager::Initialise()
	{
	  bool bSuccess = true;

	  FilenameList ArkFilenames;

	#if defined (GAME_IOS)
	  if (GenerateArkList_iOS(&ArkFilenames))
	#elif defined (OS_W8) || defined (OS_WP8)|| defined OS_W10
	  if (GenerateArkList_WinRT(&ArkFilenames))
	#elif defined (GAME_WIN32)
	  if (GenerateArkList_Win32(&ArkFilenames))
	#elif defined (GAME_ANDROID)
	  if (GenerateArkList_Android(&ArkFilenames))
	#endif
	  {
		uint32 iNumArks = ArkFilenames.Size();
		if (iNumArks)
		{
		  for (uint32 i = 0; i < iNumArks; ++i)
		  {
			if (!LoadArk(ArkFilenames[i]->GetString()))
			  RKLOG("Failed to load archive %s", ArkFilenames[i]->GetString());

			RKDELETE(ArkFilenames[i]);
		  }
		}
		else
		{
		  RKLOG("No Ark files found");
	#ifndef _DEBUG
		  bSuccess = false; // ArkFileList is empty
	#endif
		}
	  }
	  else
	  {
		RKLOG_WARN("Failed to generate list of Ark files");
		bSuccess = false; // No ArkArchive files in data directory
	  }

	#if defined (GAME_IOS) //
	#ifdef _DEBUG
	  return true;
	#endif
	#endif
  
	  return bSuccess;
	}

/**
* InitialiseForSolar
* Win32:    Loads all arks in data directory
* iOS:      Loads startup arks only, other arks are to be loaded from the DLCManager once we have a manifest
* Android:  Loads all arks in data directory
* Then calls GenerateFileTable to generate a hash table of filename | ARK* pairs for fast lookup.
* This function will still return true if no archives are found. 
* Liam de Koster-Kjaer
**/
	bool ArkManager::InitialiseForSolar()
	{
	  bool bSuccess = true;

	  FilenameList ArkFilenames;

	#if defined (GAME_IOS)
	  if (GenerateArkList_iOS(&ArkFilenames))
	#elif defined (GAME_ANDROID)
	  if (GenerateArkList_Android(&ArkFilenames))
	#elif defined (WINAPI_FAMILY)
	  if (GenerateArkList_WinRT(&ArkFilenames))
	#elif defined (GAME_WIN32)
	  if (GenerateArkList_Win32(&ArkFilenames))
	#endif
	  {
		uint32 iNumArks = ArkFilenames.Size();
		if (iNumArks)
		{
		  for (uint32 i = 0; i < iNumArks; ++i)
		  {
			if (!LoadArk(ArkFilenames[i]->GetString()))
			  RKLOG("Failed to load archive %s", ArkFilenames[i]->GetString());

			RKDELETE(ArkFilenames[i]);
		  }
		}
		else
		{
		  RKLOG("No Ark files found");
		}
	  }
	  else
	  {
		RKLOG_WARN("Failed to generate list of Ark files");
	  }

	  return bSuccess;
	}

/**
* FileExists
* Returns true if the file exists in the filetable, and is not flagged for extraction.
* Liam de Koster-Kjaer
**/
	bool ArkManager::FileExists(const char* pFilepath)
	{
	  // Remove subdir
	  char pFilename[MAXSTRING];
	  RKString_ExtractFileName(pFilepath, pFilename, sizeof(pFilename));

	  ArkFileInfo* pFileInfo = m_FileTable.Query(pFilename);
	  return (pFileInfo && pFileInfo->pArk && !pFileInfo->MetaData.IsFlagSet(AFF_EXTRACT));
	}

/**
* GetFile
* Populates given ArkFileHandle with file data.
* Liam de Koster-Kjaer
**/
	bool ArkManager::GetFile(const char* pFilepath, ArkFileHandle& outHandle)
	{
	  RKASSERT(pFilepath, "Filename is null");
	//  RKLOG("ArkManager::GetFile(pFilepath=%X = <%s>)", pFilepath, pFilepath);    //[chris: pFilepath (and stack!) seems overwritten by another thread sometimes ..]

	  bool bSuccess = false;

	  char pFilename[MAXSTRING];
	  RKString_ExtractFileName(pFilepath, pFilename, sizeof(pFilename));

	  ArkFileInfo* pFileInfo = m_FileTable.Query(pFilename);
	  if (pFileInfo)
	  {
		if (pFileInfo->pArk)
		{
		  if (!pFileInfo->MetaData.IsFlagSet(AFF_EXTRACT))
		  {
			bool bRetrievedFile = pFileInfo->pArk->GetFileHandle(pFileInfo->MetaData, outHandle);

	#ifdef ARK_LOG_DECOMPRESSION_INFO
			// Write decompression time to decompression log
			char pTempBuf[256]; memset(pTempBuf, 0, sizeof(pTempBuf));
			RKFile_CreatePath(pTempBuf, DECOMPRESSIONLOG);
			FILE* pFile = fopen(pTempBuf, "a");
			if (pFile)
			{
			  memset(pTempBuf, 0, sizeof(pTempBuf));
			  sprintf(pTempBuf, "%s%10f\n", pFilename, m_fDecompressionTime);
			  fputs(pTempBuf, pFile);
			  fclose(pFile);
			}
			// Update total decompression time
			m_fTotalDecompressionTime += m_fDecompressionTime;
	#endif  // ARK_LOG_DECOMPRESSION_INFO

			return bRetrievedFile;
		  }
		  else
			bSuccess = false; // File is flagged for extraction, not reading from archive
		}
		else
		  bSuccess = false; // FileInfo has invalid Ark pointer
	  }
	  else
		bSuccess = false; // File does not exist in hash table

	  return bSuccess;
	}

/**
* GetFileMetadata
* Liam de Koster-Kjaer
**/
	bool ArkManager::GetFileMetadata(const char* pFilename, ArkFileMetadata& outMeta)
	{
	  RKASSERT(pFilename, "Filename is null");

	  ArkFileInfo* pInfo = m_FileTable.Query(pFilename);
	  if (pInfo)
	  {
		outMeta = pInfo->MetaData;
		return true;
	  }

	  return false;
	}

/**
* ArkExists
* Liam de Koster-Kjaer
**/
	bool ArkManager::ArkExists(const char* pArkFilename)
	{
	  RKASSERT(pArkFilename, "ArkFilename is null");
	  char sFilename [256];
	  uint32 iNumArks = m_Arks.Size();
	  for (unsigned int i = 0; i < iNumArks; ++i)
	  {
		memset(sFilename,0,sizeof(sFilename));
		RKString_ExtractFileName(m_Arks[i]->GetArkFilename(), sFilename, sizeof(sFilename));
		if (0 == strcmp(sFilename, pArkFilename))
		  return true;
	  }

	  return false;
	}

/**
* StartExtractFiles
**/
	bool ArkManager::StartExtractFiles()
	{
	  LoadArkExtractList();

	  if (!CheckExtractFiles())
		return false; // If this fails, we have no files to extract - or a problem o.O

	  m_TotalBytesExtracted = 0;
#if USE_EXTRAC_THREAD
	  if (!m_pExtractMutex)
		m_pExtractMutex = RKCriticalSection_Create("ArkExtractFilesCS");

	  if (m_pExtractThread)
		RKThread_Destroy(&m_pExtractThread);

	  m_pExtractThread = RKThread_Create("ArkExtractFilesThread", ExtractThread, this);

	  if (m_pExtractThread)
	  {
		RKCriticalSection_Enter(m_pExtractMutex);
		m_bExtracting = true;
		RKCriticalSection_Leave(m_pExtractMutex);

		RKThread_Start(m_pExtractThread);
	  }
#else
	  m_bExtracting = true;
#endif
	  return true;
	}

/**
* IsExtractingFiles
**/
bool ArkManager::IsExtractingFiles(float& fProgress)
{
  bool bExtracting = false;
#if USE_EXTRAC_THREAD
  if (m_pExtractMutex)
    RKCriticalSection_Enter(m_pExtractMutex);
#endif
  bExtracting = m_bExtracting;
  fProgress = m_fExtractProgress;
#if USE_EXTRAC_THREAD
  if (m_pExtractMutex)
    RKCriticalSection_Leave(m_pExtractMutex);
#endif
  return bExtracting;
}

/**
* LoadArk
* Liam de Koster-Kjaer
**/
	bool ArkManager::LoadArk(const char* pAbsFilename)
	{
	  bool bSuccess = true;

	  UnloadArk(pAbsFilename);  // Will check if the ark exists, if so this function will unload it.

	  Ark* pArk = RKNEW(Ark);
	  if (pArk->Initialise(pAbsFilename))
	  {
		ArkFileMetadata* pIndex = pArk->GetArkIndex();
		if (pIndex)
		{
		  for(uint32 i = 0, iNumFiles = pArk->GetNumFiles(); i < iNumFiles; ++i)
			AddFileToFileTable(&pIndex[i], pArk);

		  pArk->UnloadIndex();
		  m_Arks.Append(pArk);
		  m_Arks.Sort(ArkSort);
		  RemoveArkFromFailedList(pAbsFilename);
		}
		else
		  bSuccess = false; // Failed to retrieve Ark index
	  }
	  else
		bSuccess = false; // Failed to intialise Ark

	  if (!bSuccess)
	  {
		RKDELETE(pArk);
		AddArkToFailedList(pAbsFilename); // Maintain a list of failed arks
	  }

	  return bSuccess;
	}

/**
* UnloadArk
* Returns true if the Ark existed and was removed.
* Liam de Koster-Kjaer
**/
	bool ArkManager::UnloadArk(const char* pAbsFilepath)
	{
	  RKASSERT(pAbsFilepath, "pAbsFilepath is null");

	  bool bSuccess = true;

	  // Find ark with given filename
	  uint32 iNumArks = m_Arks.Size();
	  Ark* pArkToRemove = NULL;
	  unsigned int iNumOlderArks = 0;
	  for(; iNumOlderArks < iNumArks; ++iNumOlderArks)
	  {
		if(0 == strcmp(pAbsFilepath, m_Arks[iNumOlderArks]->GetArkFilename()))
		{
		  pArkToRemove = m_Arks[iNumOlderArks];
		  break;
		}
	  }
	  // If Ark with given filename is loaded...
	  if(pArkToRemove)
	  {
		// Files from the Ark to be unloaded need to be replaced with files of the same name from other Arks or removed from the FileTable
		// We want to load the Ark indexes only once, its an expensive operation.

		// We can't erase elements from an RKHashTable as we are iterating through it...
		RKList<RKString> ToRemoveFromFileTable;
		RKList<ArkFileInfo*> ToAddToFileTable;

		// Iterate through the FileTable to find files from the Ark we want to remove
		ArkFileInfo* pInfo = NULL;
		char pExt[RKPATH_LENGTH];
		char pPath[RKPATH_LENGTH];
		char pFileNameBase[RKPATH_LENGTH];
		RKString sFilename;
		FileTable::ConstIterator it = m_FileTable.Begin();
		while(it.IsValid())
		{
		  pInfo = it.Value();

		  if(pInfo->pArk == pArkToRemove)// If the file in the FileTable is from the Ark to remove
		  {
			if(pInfo->MetaData.IsFlagSet(AFF_MULTIPLE))
			{
			  bool bArkFileInfoReplaced = false;
			  RKString_ExtractFileExtension(pInfo->MetaData.pFilename, pExt);
			  if((strcmp(pExt, g_sTextureExtensions[0]) == 0) ||
				 (strcmp(pExt, g_sTextureExtensions[1]) == 0) ||
				 (strcmp(pExt, g_sTextureExtensions[2]) == 0))
			  {// File is a texture, we need to replace with any texture files from the next older ark that share the same filename base, else remove
				RKString_ExtractFilePath(pInfo->MetaData.pFilename, pPath, RKPATH_LENGTH);
				RKString_ExtractFileBase(pInfo->MetaData.pFilename, pFileNameBase, RKPATH_LENGTH);
				for(int32 iOlderArk = iNumOlderArks - 1; iOlderArk >= 0; --iOlderArk)// Using an int - if there are no older Arks than the Ark to remove, iOlderArk will be -1 and this for loop will be skipped
				{
				  bArkFileInfoReplaced = false;
				  ArkFileMetadata* pIndex = m_Arks[iOlderArk]->GetArkIndex();
				  uint32 iNumFiles = m_Arks[iOlderArk]->GetNumFiles();
				  // For each file in the Ark index of the next oldest Ark
				  for(uint32 iOlderArkFile = 0; iOlderArkFile < iNumFiles; ++iOlderArkFile)
				  {// If this file has the same file name base as the file from the Ark to unload
					for(uint32 i = 0; i < g_sTextureExtensionCount; ++i)
					{
					  sFilename.AssignFormatted("%s%s.%s", pPath, pFileNameBase, g_sTextureExtensions[i]);
					  if(sFilename == pIndex[iOlderArkFile].pFilename)
					  {
						// Remove existing entry (Can have a different extension to the file it needs to be replaced with)
						ToRemoveFromFileTable.Append(pInfo->MetaData.pFilename);// Because we cannot erase entries from an RKHashTable while we are iterating through it, we keep the key used for this entry and do so later

						// Create new FileInfo to add to FileTable
						ArkFileInfo* pNewArkFileInfo = RKNEW(ArkFileInfo);
						pNewArkFileInfo->pArk = m_Arks[iOlderArk];
						pNewArkFileInfo->MetaData = pIndex[iOlderArkFile];
						pNewArkFileInfo->MetaData.SetFlagOn(AFF_MULTIPLE);// We set this flag, though it may not be true. Worst case scenario we search and fail to find a multiple if this file is removed from the FileTable
						// NOTE: No need to add to the dir table as the entries being added come from replacing textures
						// Replacing textures when unloading arks have complications due to the texture extension handling
						ToAddToFileTable.Append(pNewArkFileInfo);
						bArkFileInfoReplaced = true;// Replaced isn't the right word here, as we haven't actually replaced it yet
						break;
					  }
					}

					if(bArkFileInfoReplaced)
					  break;
				  }

				  if(bArkFileInfoReplaced)
					break;
				}// for(uint32 iOlderArk = iNumOlderArks-1; iOlderArk >= 0; --iOlderArk)
			  }
			  else// File is not a texture, handle normally
			  {
				for(int32 iOlderArk = iNumOlderArks - 1; iOlderArk >= 0; --iOlderArk)// Using an int - if there are no older Arks than the Ark to remove, iOlderArk will be -1 and this for loop will be skipped
				{
				  bArkFileInfoReplaced = false;
				  ArkFileMetadata* pIndex = m_Arks[iOlderArk]->GetArkIndex();
				  uint32 iNumFiles = m_Arks[iOlderArk]->GetNumFiles();

				  // For each file in the Ark index of the next oldest Ark
				  for(uint32 iOlderArkFile = 0; iOlderArkFile < iNumFiles; ++iOlderArkFile)
				  {// If this file has the same filename as the file from the Ark to unload
					if(0 == strcmp(pInfo->MetaData.pFilename, pIndex[iOlderArkFile].pFilename))
					{// Replace ArkFileInfo data with data for this file from this older Ark
					  pInfo->pArk = m_Arks[iOlderArk];
					  pInfo->MetaData = pIndex[iOlderArkFile];
					  pInfo->MetaData.SetFlagOn(AFF_MULTIPLE);// We set this flag, though it may not be true. Worst case scenario we search and fail to find a multiple if this file is removed from the FileTable
					  bArkFileInfoReplaced = true;
					  break;
					}
				  }

				  if(bArkFileInfoReplaced)
					break;
				}// for(uint32 iOlderArk = iNumOlderArks-1; iOlderArk >= 0; --iOlderArk)
			  }

			  // If the ArkFileInfo was not replaced, then there were no duplicates of that file in any older Arks. We can straight remove it from the FileTable
			  if(!bArkFileInfoReplaced)
				ToRemoveFromFileTable.Append(pInfo->MetaData.pFilename);// Because we cannot erase entries from an RKHashTable while we are iterating through it, we keep the key used for this entry and do so later
			}// if(pInfo->MetaData.IsFlagSet(AFF_MULTIPLE))
			else
			{// FileInfo is from ArkToRemove, but there are no other arks that contain this fileinfo
			  ToRemoveFromFileTable.Append(pInfo->MetaData.pFilename);// Because we cannot erase entries from an RKHashTable while we are iterating through it, we keep the key used for this entry and do so later
			}
		  }// if(pInfo->pArk == pArkToRemove)
		  ++it;
		}// while(it.IsValid())

		// We erase any necessary elements from the FileTable here, as we cannot do this while iterating over the RKHashTable
		for(uint32 i = 0, size = ToRemoveFromFileTable.Size(); i < size; ++i)
		{
		  FileTable::Iterator it = m_FileTable.FindKey(ToRemoveFromFileTable[i].GetString());
		  if(it.IsValid())
		  {
			RKDELETE(it.Value());
			it.Value() = NULL;
			m_FileTable.Erase(it);
		  }
		  else
		  {
			RKLOG_ERROR("Failed to remove key from FileTable");
			bSuccess = false;
		  }
		}

		// Add any new elements to the FileTable here, as we cannot do this while iterating over the RKHashTable
		// NOTE: No need to add to the dir table as the entries being added come from replacing textures
		// Replacing textures when unloading arks have complications due to the texture extension handling
		for(uint32 i = 0, size = ToAddToFileTable.Size(); i < size; ++i)
		  m_FileTable.Insert(ToAddToFileTable[i], ToAddToFileTable[i]->MetaData.pFilename);

		// Ark indexes should not be kept in memory
		for(uint32 i = 0; i < iNumOlderArks; ++i)
		  m_Arks[i]->UnloadIndex();

		// And after managing the FileTable, remove the Ark itself
		RKDELETE(pArkToRemove);
		pArkToRemove = NULL;
		m_Arks.EraseAt(iNumOlderArks);// iNumOlderArks is the index of the ark to remove
	  }

	  return bSuccess;
	}

	Ark* ArkManager::GetArk(const char* pAbsFilename)
	{
	  RKASSERT(pAbsFilename, "pAbsFilename is null");

	  uint32 iNumArks = m_Arks.Size();
	  for (uint32 i = 0; i < iNumArks; ++i)
	  {
		if (0 == strcmp(m_Arks[i]->GetArkFilename(), pAbsFilename))
		  return m_Arks[i];
	  }

	  return 0;
	}

	void ArkManager::StartDecompressionTimer()
	{
	#ifdef ARK_LOG_DECOMPRESSION_INFO
	  m_DecompressionTimer.GetElapsedTime();
	#endif  // ARK_LOG_DECOMPRESSION_INFO
	}

	void ArkManager::StopDecompressionTimer()
	{
	#ifdef ARK_LOG_DECOMPRESSION_INFO
	  m_fDecompressionTime = m_DecompressionTimer.GetElapsedTime();
	#endif  // ARK_LOG_DECOMPRESSION_INFO
	}

	bool ArkManager::GenerateArkList_WinRT(FilenameList* ArkFilenames)
	{
		bool bSuccess = true;

	#if defined(OS_WP8) || defined(OS_W8)|| defined OS_W10
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		char pWorkingDir[MAXPATH] = { 0 };
		if (_getcwdEmul(pWorkingDir, MAXPATH))
		{
			std::string strWorkingDir = std::string(pWorkingDir) + "\\Data\\*.ark";
			std::wstring wsWorkingDir = std::wstring(strWorkingDir.begin(), strWorkingDir.end());
			hFind = FindFirstFileEx(wsWorkingDir.c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);
		}
		if (hFind != INVALID_HANDLE_VALUE)
		{
			char pWorkingDir[MAXPATH] = { 0 };
			if (_getcwdEmul(pWorkingDir, MAXPATH))
			{
				pWorkingDir[strlen(pWorkingDir)] = '\\';
				do
				{
					RKString* pAbsArkFilename = RKNEW(RKString(pWorkingDir));
					pAbsArkFilename->Append("\\Data\\");
					pAbsArkFilename->Append(FindFileData.cFileName);
					pAbsArkFilename->ReplaceAll("\\", "/");
					ArkFilenames->Append(pAbsArkFilename);
				} while (FindNextFile(hFind, &FindFileData));
			}
			else
			{
				RKLOG_ERROR("Failed to get current working directory");
			}

			ArkFilenames->Sort(ArkFilenameSort);

			FindClose(hFind);
		}
		///find the ark in store path
		else
		{
			const char* pSaveFilePath = RKFile_GetSupportFilesPath();
			memcpy(pWorkingDir, pSaveFilePath, MAXPATH);
			{
				std::string strWorkingDir = std::string(pWorkingDir) + "\\*.ark";
				std::wstring wsWorkingDir = std::wstring(strWorkingDir.begin(), strWorkingDir.end());
				hFind = FindFirstFileEx(wsWorkingDir.c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);
			}
			if (hFind != INVALID_HANDLE_VALUE)
			{
				char pWorkingDir[MAXPATH] = { 0 };
				memcpy(pWorkingDir, pSaveFilePath, MAXPATH);
				{
					//pWorkingDir[strlen(pWorkingDir)] = '\\';
					do
					{
						RKString* pAbsArkFilename = RKNEW(RKString(pWorkingDir));
						pAbsArkFilename->Append(FindFileData.cFileName);
						pAbsArkFilename->ReplaceAll("\\", "/");
						ArkFilenames->Append(pAbsArkFilename);
					} while (FindNextFile(hFind, &FindFileData));
				}

				ArkFilenames->Sort(ArkFilenameSort);

				FindClose(hFind);
			}
			else
			{
				bSuccess = false;	// No ArkFiles in data directory
			}
		}
	#endif	// OS_WP8/OS_W8/OS_W10

		return bSuccess;
	}

	bool ArkManager::GenerateArkList_Win32(FilenameList* ArkFilenames)
	{
	  bool bSuccess = true;

#if defined(GAME_WIN32) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
	  WIN32_FIND_DATA FindFileData;
	  HANDLE hFind;
	  hFind = FindFirstFile(L"*.ark", &FindFileData);
	  if (hFind != INVALID_HANDLE_VALUE)
	  {
		char pWorkingDir[MAXPATH] = {0};
		if (_getcwd(pWorkingDir, MAXPATH))
		{
		  pWorkingDir[strlen(pWorkingDir)] = '\\';
		  do
		  {
			RKString* pAbsArkFilename = RKNEW(RKString(pWorkingDir));
			pAbsArkFilename->Append(FindFileData.cFileName);
			pAbsArkFilename->ReplaceAll("\\", "/");
			ArkFilenames->Append(pAbsArkFilename);
		  }
		  while (FindNextFile(hFind, &FindFileData));
		}
		else
		{
		  RKLOG_ERROR("Failed to get current working directory");
		}

		ArkFilenames->Sort(ArkFilenameSort);

		FindClose(hFind);
	  }
	  else
		bSuccess= false;	// No ArkFiles in data directory
#endif	// GAME_WIN32

	  return bSuccess;
	}

	bool ArkManager::GenerateArkList_Android(FilenameList* ArkFilenames)
	{
	  bool bSuccess = true;
	// #ifdef GAME_ANDROID
	//   int iNumArks = nativeGetNumArkFiles();

	//   char pFilename[RKSTRING_LENGTH_TEMP];
	//   memset(pFilename, 0, RKSTRING_LENGTH_TEMP);
	//   for (uint32 i = 0; i < iNumArks; ++i)
	//   {
	// 	if (nativeGetArkFilename(i, pFilename, RKSTRING_LENGTH_TEMP) == 0)
	// 	{
	// 	  RKString* pArkFilename = RKNEW(RKString(pFilename)); // above now passes absolute paths - no need to call RKFile_CreatePath, the archives wont be on the sd card anyway (See GEF)
	// 	  RKLOG(pArkFilename->GetString());
	// 	  ArkFilenames->Append(pArkFilename);
	// 	}
	//   }

	//   ArkFilenames->Sort(ArkFilenameSort);
	// #endif
	  return bSuccess;
	}

/**
* ArkSort
* Parameters must be pointers to Ark class objects
* The first four characters of the ArkFilename is the ArkNumber, which we compare against.
* Liam de Koster-Kjaer
**/
	bool ArkManager::ArkSort(void* a, void* b)
	{
	  return static_cast<Ark*>(a)->GetArkNumber() < static_cast<Ark*>(b)->GetArkNumber();
	}

/**
* ArkFilenameSort
* Parameter's must be RKString pointers that contain an Ark filename
* The first four characters of the ArkFilename is the ArkNumber, which we compare against.
* Liam de Koster-Kjaer
**/
	bool ArkManager::ArkFilenameSort(void* a, void* b)
	{
	  char tmpA[256] = {0};
	  char tmpB[256] = {0};

	  RKString_ExtractFileName(static_cast<RKString*>(a)->GetString(), tmpA, sizeof(tmpA));
	  RKString_ExtractFileName(static_cast<RKString*>(b)->GetString(), tmpB, sizeof(tmpB));

	  char pA[5] = { tmpA[0], tmpA[1], tmpA[2], tmpA[3], '\0' };
	  char pB[5] = { tmpB[0], tmpB[1], tmpB[2], tmpB[3], '\0' };

	  return atoi(pA) < atoi(pB);
	}

//!>> ----------------------------------------------------------------------------
// When loading an Ark, the Ark reads the index into memory, then we copy the index data into our filetable.
// This function copies a file from that ark index's data into our filetable.
	bool ArkManager::AddFileToFileTable(ArkFileMetadata* pData, Ark* pArk)
	{
	  RKASSERT(pData, "pData is null");
	  RKASSERT(pArk, "pArk is null");

	  // Note: Files flagged for extraction will not be added to the FileTable
	  bool bFound = false;
	  char pExt[RKPATH_LENGTH] = {0};
	  RKString_ExtractFileExtension(pData->pFilename, pExt);

	  if((strcmp(pExt, g_sTextureExtensions[0]) == 0) ||
		 (strcmp(pExt, g_sTextureExtensions[1]) == 0) ||
		 (strcmp(pExt, g_sTextureExtensions[2]) == 0))// If the file is a texture, we need to check for textures with the same filename base
	  {
		char pPath[RKPATH_LENGTH];
		RKString_ExtractFilePath(pData->pFilename, pPath, RKPATH_LENGTH);
		char pFileNameBase[RKPATH_LENGTH];
		RKString_ExtractFileBase(pData->pFilename, pFileNameBase, RKPATH_LENGTH);
		RKString sFilename = RKString(" ");
		for(uint32 i = 0; i < g_sTextureExtensionCount; ++i)
		{
           sFilename = RKString(pPath) + RKString( pFileNameBase) +  RKString(".")+ RKString(g_sTextureExtensions[i]);
		  //sFilename.AssignFormatted("%s%s.%s", pPath, pFileNameBase, g_sTextureExtensions[i]);
		  FileTable::Iterator it = m_FileTable.FindKey(sFilename.GetString());
		  if(it.IsValid())
		  {
			ArkFileInfo* pFileInfo = it.Value();
			if(pFileInfo)
			{
			  // If this Ark is newer than the existing file's Ark...
			  if(ArkSort(pFileInfo->pArk, pArk)) // Is arknumber from first ark less than arknumber from second ark
			  {
				// Replace ArkFileInfo data
				pFileInfo->pArk = pArk;
				pFileInfo->MetaData = *pData;

				// Remove the old filetable entry and add a new one with the new extension
				m_FileTable.Erase(it);
				m_FileTable.Insert(pFileInfo, pFileInfo->MetaData.pFilename);// Use old FileInfo - no need to reallocate just replace member data
			  }

			  pFileInfo->MetaData.SetFlagOn(AFF_MULTIPLE);  // We flag files that exist multiple times in the ArkFileSystem so as to optimise the maintaining of the FileTable during the UnloadArk process

			  bFound = true;
			  break;
			}
			else
			{
			  RKLOG_ERROR("FileTable contains entry with invalid value: %s", pData->pFilename);
			  return false;
			}
		  }
		}
	  }
	  else// File is not a texture
	  {
		FileTable::ConstIterator it = m_FileTable.FindKey(pData->pFilename);
		if(it.IsValid())// If the file already exists in the FileTable...
		{
		  ArkFileInfo* pFileInfo = it.Value();
		  if(pFileInfo)
		  {
			// If this Ark is newer than the existing file's Ark...
			if(ArkSort(pFileInfo->pArk, pArk)) // Is arknumber from first ark less than arknumber from second ark
			{
			  // Replace ArkFileInfo data
			  pFileInfo->pArk = pArk;
			  pFileInfo->MetaData = *pData;
			}

			pFileInfo->MetaData.SetFlagOn(AFF_MULTIPLE);  // We flag files that exist multiple times in the ArkFileSystem so as to optimise the maintaining of the FileTable during the UnloadArk process

			bFound = true;
		  }
		  else
		  {
			RKLOG_ERROR("FileTable contains entry with invalid value: %s", pData->pFilename);
			return false;
		  }
		}
	  }

	  if(!bFound)
	  {
		// Add new ArkFileInfo to the FileTable
		if(!pData->IsFlagSet(AFF_EXTRACT))  // If the file is not flagged for extraction
		{
		  ArkFileInfo* pNewArkFileInfo = RKNEW(ArkFileInfo);
		  pNewArkFileInfo->pArk = pArk;
		  pNewArkFileInfo->MetaData = *pData;
		  m_FileTable.Insert(pNewArkFileInfo, pNewArkFileInfo->MetaData.pFilename);
		}
	  }

	  return true;
	}

/**
* LoadArkExtractList
* We keep a list of Ark's that are NOT on the extracted ark list in memory for the file extraction process.
* This function is not executed on a seperate thread.
* Liam de Koster-Kjaer
**/
	void ArkManager::LoadArkExtractList()
	{
	  // Load list of extracted arks
	  FilenameList ExtractedArks;

	  RKFile* pFile = RKFile_Open(EXTRACTEDARKLIST, RKFM_READ);
	  if (pFile)
	  {
		// Read file into memory
		uint32 iSize = RKFile_GetSize(pFile);
		if (iSize)
		{
		  char* pFileBuf = RKNEWARRAY(char, iSize);
		  memset(pFileBuf, 0, iSize);
		  int32 iRead = RKFile_Read(pFile, pFileBuf, iSize);
		  if (iRead == iSize)
		  {
			// Parse each line of file, each line is the filename of an ark that has been extracted
			RKString sTemp;
			for (uint32 i = 0; i < iSize; ++i)
			{
			  if (pFileBuf[i] == '\n' || pFileBuf[i] == '\r')
			  {
				if (sTemp.Length() > 0)
				{
				  RKString* pLine = RKNEW(RKString);
				  pLine->Append(sTemp);
				  ExtractedArks.Append(pLine);
				  sTemp.Clear();
				}
			  }
			  else
				sTemp.AppendChar(pFileBuf[i]);
			}
		  }

		  RKDELETEARRAY(pFileBuf);
		}

		RKFile_Close(&pFile);
	  }

	  // Generate list of arks and arkindexes that need extracting
	  uint32 iNumArks = m_Arks.Size();
	  uint32 iNumExtractedArks = ExtractedArks.Size();
	  bool bExtracted;
	  for (uint32 iArk = 0; iArk < iNumArks; ++iArk)
	  {
		bExtracted = false;
		for (uint32 i = 0; i < iNumExtractedArks; ++i)
		{
		  if (0 == strcmp(m_Arks[iArk]->GetArkFilename(), ExtractedArks[i]->GetString()))
		  {
			bExtracted = true;
			break;
		  }
		}

		if (!bExtracted)
		  m_ArkFilesToExtract.Append(m_Arks[iArk]);
	  }

	  // Clean up list of extracted arks
	  for (uint32 i = 0; i < iNumExtractedArks; ++i)
	  {
		RKDELETE(ExtractedArks[i]);
		ExtractedArks[i] = 0;
	  }
	}

// Creating directories cannot be threaded on android 

/**
* CheckExtractFiles
* This function will calculate the total number of bytes to extract and 
* create necessary subdirectories for any files that are to be extracted.
*
* This function is not run on a seperate thread, and was designed as such 
* because the task of creating directories could not be threaded on Android.
*
* This function will load the indexes for all Ark archives that need extraction
* but it will not unload them. 
* Liam de Koster-Kjaer
**/
	bool ArkManager::CheckExtractFiles()
	{
	  m_TotalBytesToExtract = 0;

	  // For each ark that has not already been extracted
	  uint32 iNumArksToExtract = m_ArkFilesToExtract.Size();
	  ArkFileMetadata* pIndex = 0;
	  uint32 iNumFiles = 0;
	  for (uint32 iArk = 0; iArk < iNumArksToExtract; ++iArk)
	  {
		// Get the ark index
		pIndex = m_ArkFilesToExtract[iArk]->GetArkIndex();
		if (pIndex) // If we can't get the index we won't be able to extract from the Ark
		{
		  iNumFiles = m_ArkFilesToExtract[iArk]->GetNumFiles();

		  // For each file in the ark index
		  for (uint32 i = 0; i < iNumFiles; ++i)
		  {
			if (pIndex[i].IsFlagSet(AFF_EXTRACT))  // If that file is flagged for extraction
			{
			  // Calculate total bytes to extract
			  m_TotalBytesToExtract += pIndex[i].iSrcFileSize;

			  // if it has a subdir
			  if (strlen(pIndex[i].pSubdir) > 0)
			  {
				// if that subdir does not exist
				if (!RKFile_DirExists(pIndex[i].pSubdir))
				{
				  // create that subdir
				  if (!RKFile_CreateDir(pIndex[i].pSubdir))
				  {
					RKLOG_ERROR("Failed to create subdir for runtime extraction: %s", pIndex[i].pSubdir);
					return false; // Critical Error
				  }
				}
			  }
			}
		  }
		}
		else
		{
		  RKLOG_ERROR("Failed to get Ark index for Ark: %s", m_ArkFilesToExtract[iArk]->GetArkFilename());
		  return false; // Critical Error
		}
	  }

	  return m_TotalBytesToExtract > 0;
	}

	uint32 ArkManager::ExtractThread(void* pArkManager)
	{
	  ArkManager* This = static_cast<ArkManager*>(pArkManager);
	  return This->ExtractFiles() ? 0 : 1;
	}

/**
* ExtractFiles
* Called from ExtractThread, this function is executed on a seperate thread. 
* This function will unload the indexes from Ark objects that it extracts file from. 
* Liam de Koster-Kjaer
**/
	bool ArkManager::ExtractFiles()
	{
	  bool bSuccess = true;

	  RKLog_EnableCallback(false);  //TEMP, multi thead to fix!!

	  uint32 iNumExtractedFiles = 0;

	  uint32 iNumArksToExtract = m_ArkFilesToExtract.Size();
	  if (iNumArksToExtract)
	  {
#if USE_EXTRAC_THREAD
		RKCriticalSection_Enter(m_pExtractMutex);
		m_bExtracting = true;
		RKCriticalSection_Leave(m_pExtractMutex);
#else
		m_bExtracting = true;
#endif
		// Load list of extracted ark archives
		char pFullFilename[RKSTRING_LENGTH_TEMP];
		memset(pFullFilename, 0, RKSTRING_LENGTH_TEMP);
		strncpy(pFullFilename, RKFile_GetSupportFilesPath(), RKSTRING_LENGTH_TEMP);
		strncat(pFullFilename, EXTRACTEDARKLIST, RKSTRING_LENGTH_TEMP - strlen(pFullFilename));
	#if ! defined OS_W8 && !defined OS_W10
		FILE* pExtractedArkListFile = fopen(pFullFilename, "a+");
	#else
		FILE* pExtractedArkListFile = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(pFullFilename).c_str(), L"a+");
	#endif

		// For each ark that has not already been extracted
		uint32 iNumFiles = 0;
		ArkFileMetadata* pIndex = 0;
		char pFullOutPath[RKSTRING_LENGTH_TEMP];
		ArkFileHandle Handle;
		for (uint32 iArk = 0; iArk < iNumArksToExtract; ++iArk)
		{
		  // Get the ark index
		  pIndex = m_ArkFilesToExtract[iArk]->GetArkIndex();
		  if (pIndex)
		  {
			// For each file in the ark index
			iNumFiles = m_ArkFilesToExtract[iArk]->GetNumFiles();
			for (uint32 i = 0; i < iNumFiles; ++i)
			{
			  if (pIndex[i].IsFlagSet(AFF_EXTRACT))  // If that file is flagged for extraction
			  {
				// Get the file from the archive
				if (m_ArkFilesToExtract[iArk]->GetFileHandle(pIndex[i], Handle))
				{
				  // Obtain full path
				  strncpy(pFullOutPath, RKFile_GetSupportFilesPath(), RKSTRING_LENGTH_TEMP);
				  strncat(pFullOutPath, pIndex[i].pSubdir, RKSTRING_LENGTH_TEMP - strlen(pFullOutPath)); // First to subdir
				  strncat(pFullOutPath, pIndex[i].pFilename, RKSTRING_LENGTH_TEMP - strlen(pFullOutPath)); // Then to file

				  RKLOG("Extracting file: %s", pFullOutPath);

				  // Write file to disk
	#if !defined OS_W8 && !defined OS_W10
				  FILE* pStream = fopen(pFullOutPath, "wb");
	#else
				  FILE* pStream = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(pFullOutPath).c_str(), L"wb");
	#endif
				  if (pStream)
				  {
					if (1 == fwrite(Handle.pData, Handle.iFileSize, 1, pStream))
					{
					  m_TotalBytesExtracted += Handle.iFileSize;
#if USE_EXTRAC_THREAD
					  RKCriticalSection_Enter(m_pExtractMutex);
					  m_fExtractProgress = (float)((double)m_TotalBytesExtracted / (double)m_TotalBytesToExtract);
					  RKCriticalSection_Leave(m_pExtractMutex);
#else
					  m_fExtractProgress = (float)((double)m_TotalBytesExtracted / (double)m_TotalBytesToExtract);
#endif
					  ++iNumExtractedFiles;
					}
					else
					{
					  RKLOG("Error writing file to disk: %s", pFullOutPath);
					  bSuccess = false;
					}

					fclose(pStream);
				  }
				  else
				  {
					RKLOG("Failed to open file: %s", pFullOutPath);
					bSuccess = false;
				  }

				  RKDELETEARRAY(Handle.pData);
				  Handle.pData = 0;
				  memset(&Handle, 0, sizeof(ArkFileHandle));
				}
				else
				{
				  RKLOG("Failed to retrieve file handle: %s%s", pIndex[i].pSubdir, pIndex[i].pFilename);
				  bSuccess = false;
				}
			  }
			} // for (uint32 i = 0; i < iNumFiles; ++i)

			// Write ark filename to extracted list now that we have extracted its files
			if (pExtractedArkListFile)
			{
			  if (bSuccess && iNumExtractedFiles)
			  {
				fputs(m_ArkFilesToExtract[iArk]->GetArkFilename(), pExtractedArkListFile);
				fputc('\n', pExtractedArkListFile);
			  }
			}

			m_ArkFilesToExtract[iArk]->UnloadIndex();
		  } // if (pIndex)
		} // for (uint32 iArk = 0; iArk < iNumArksToExtract; ++iArk)

		if (pExtractedArkListFile)
		  fclose(pExtractedArkListFile);

		m_ArkFilesToExtract.Clear();
	  } // if (iNumArksToExtract)
#if USE_EXTRAC_THREAD
	  RKCriticalSection_Enter(m_pExtractMutex);
	  m_bExtracting = false;
	  RKCriticalSection_Leave(m_pExtractMutex);
#else
	  m_bExtracting = true;
#endif
	  RKLOG("Extracted %i files", iNumExtractedFiles);

	  RKLog_EnableCallback(true);  //TEMP, multi thead to fix!!

	  return bSuccess;
	}

/**
* AddArkToFailedList
* Liam de Koster-Kjaer
**/
	void ArkManager::AddArkToFailedList(const char* pAbsFilename)
	{
	  RKASSERT(pAbsFilename, "pAbsFilename is null");

	  // Check first if the filename exists already in the failed list
	  bool bExists = false;
	  uint32 iNumFailedArks = m_FailedArks.Size();
	  for (uint32 i = 0; i < iNumFailedArks; ++i)
	  {
		if (0 == strcmp(pAbsFilename, m_FailedArks[i]->GetString()))
		{
		  bExists = true;
		  break;
		}
	  }

	  if (!bExists)
		m_FailedArks.Append(RKNEW(RKString(pAbsFilename)));
	}

/**
* RemoveArkFromFailedList
* Returns true if filename was removed from failed list.
* Liam de Koster-Kjaer
**/
	bool ArkManager::RemoveArkFromFailedList(const char* pAbsFilename)
	{
	  RKASSERT(pAbsFilename, "pAbsFilename is null");

	  uint32 iNumFailedArks = m_FailedArks.Size();
	  for (uint32 i = 0; i < iNumFailedArks; ++i)
	  {
		if (0 == strcmp(pAbsFilename, m_FailedArks[i]->GetString()))
		{
		  m_FailedArks.EraseAt(i);
		  return true;
		}
	  }

	  return false;
	}

}
