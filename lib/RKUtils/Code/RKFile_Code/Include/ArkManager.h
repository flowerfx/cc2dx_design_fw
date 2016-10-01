//
// ArkManager.h
// Liam de Koster-Kjaer
// January 2012
//

#ifndef __ARK_MANAGER_H__
#define __ARK_MANAGER_H__
#define USE_EXTRAC_THREAD 0

#include "ArkCommon.h"
#include "../../RKTool_Code/RKList.h"
#include "../../RKString_Code/RKString.h"
#include "../../Common/RKHashTable.h"
#if USE_EXTRAC_THREAD
#include "RKThread.h"
#include "RKTimer.h"
#endif
namespace RKUtils
{
	class Ark;

	struct ArkFileInfo
	{
		Ark*            pArk;
		ArkFileMetadata MetaData;
	};

	typedef RKList<RKString*,0>         FilenameList;
	typedef RKHashTable<ArkFileInfo*> FileTable;
	typedef RKList<Ark*>              ArkList;

	typedef void(*ExtractProgressCallback) (float fProgress);

	class ArkManager
	{
		// Member functions
	public:
		ArkManager();
		~ArkManager();

		bool                Initialise();
		bool                InitialiseForSolar();

		bool                FileExists(const char* pFilepath);

		bool                GetFile(const char* pFilepath, ArkFileHandle& outHandle);
		bool                GetFileMetadata(const char* pFilename, ArkFileMetadata& outMeta);
		bool                ArkExists(const char* pArkFilename);

		bool                StartExtractFiles();
		bool                IsExtractingFiles(float& fProgress);

		bool                LoadArk(const char* pAbsFilename);
		bool                UnloadArk(const char* pAbsFilename);

		const ArkList&      GetArkList()  { return m_Arks; }
		Ark*                GetArk(const char* pAbsFilename);

		FilenameList&       GetFailedArkList() { return m_FailedArks; }

		void                StartDecompressionTimer();
		void                StopDecompressionTimer();

	private:
		ArkManager(const ArkManager& _kr);
		ArkManager& operator=(const ArkManager& _kr);

		bool                GenerateArkList_iOS(FilenameList* ArkFilenames);
		bool                GenerateArkList_WinRT(FilenameList* ArkFilenames);
		bool                GenerateArkList_Win32(FilenameList* ArkFilenames);
		bool                GenerateArkList_Android(FilenameList* ArkFilenames);

		static bool         ArkSort(void* a, void* b);          // For RKList<Ark*>
		static bool         ArkFilenameSort(void* a, void* b);  // For RKList<RKString*>

		bool                AddFileToFileTable(ArkFileMetadata* pData, Ark* pArk);

		void                LoadArkExtractList();
		bool                CheckExtractFiles();
		static uint32       ExtractThread(void* pArkManager);
		bool                ExtractFiles();

		void                AddArkToFailedList(const char* pAbsFilename);
		bool                RemoveArkFromFailedList(const char* pAbsFilename);

		// Member variables
	private:
		ArkList             m_Arks; // ArkList is sorted by ArkNumber (lowest to highest) when an Ark is loaded
		FileTable           m_FileTable;

		FilenameList        m_FailedArks;

		// For Extracting files from Archives on a Background Thread
#if USE_EXTRAC_THREAD
		RKThread*           m_pExtractThread;
		RKCriticalSection*  m_pExtractMutex;
#endif
		bool                m_bExtracting;
		float               m_fExtractProgress;
		ArkList             m_ArkFilesToExtract;
		uint64              m_TotalBytesToExtract;
		uint64              m_TotalBytesExtracted;

#ifdef ARK_LOG_DECOMPRESSION_INFO
		RKTimer             m_DecompressionTimer;
		float               m_fDecompressionTime;
		float               m_fTotalDecompressionTime;
#endif  // ARK_LOG_DECOMPRESSION_INFO
	};
}
#endif	// __ARK_MANAGER_H__