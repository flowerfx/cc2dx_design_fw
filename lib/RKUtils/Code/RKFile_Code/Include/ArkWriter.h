// 
// ArkWriter.h
// Liam de Koster-Kjaer
// January 2012
// 

#ifndef __ARK_WRITER_H__
#define __ARK_WRITER_H__
#include <vector>
#include "ArkCommon.h"

namespace RKUtils
{
	typedef std::vector<ArkFileMetadata*> MetadataList;

	class ArkWriter
	{
		// Member functions
	public:
		ArkWriter();
		~ArkWriter();

		// Create an ArkArchive adding a file at a time
		bool              InitialiseArk(const char* pArkFilename, bool bMessageBoxErrors = false);
		bool              AddFileToArk(const char* pFilename, const char* pSubdir = 0, bool bExtract = false, bool bCompress = true, bool bEncrypt = false); // pFilename must be absolute path to the file
		uint32            GetCurrentArkSize() const;
		bool              FinaliseArk(bool bValidateMD5 = false);

		void              SetCompressionThreshold(float fThreshold);

		bool              MD5ValidateFiles(const char* pArkFilename);

		void              LogArkIndex();
		void              LogArkFileMetadata();

	protected:
	private:
		ArkWriter(const ArkWriter& _kr);
		ArkWriter& operator=(const ArkWriter& _kr);

		bool              LoadSourceFile(const char* pFilename, ArkFileMetadata* pInfo);
		bool              CompressFile(ArkFileMetadata* pInfo, char* pUncompr, uint32 iUncomprLen, char*& pCompr, uint32& iComprLen);
		bool              EncryptFileBuffer(ArkFileMetadata* pInfo, char* pInData, uint32 iInSize, char*& pOutData, uint32& iOutSize);
		bool              WriteFileToArk(ArkFileMetadata* pInfo, char* pData, uint32 iSize);
		bool              WriteHeaderToArk();
		bool              WriteIndexToArk();

		uint32            GetFileSize(FILE* pFile);
		void              ResizeBufferToFit(char*& pBuf, uint32& iCurrentSize, uint32 iSizeToFit, bool bCopyOldBuffer = false);

		void              ErrorMessageAlert(const char* pMsg);

		bool              DecompressFile(char* pCompr, uint32 iComprLen, char* pUncompr, uint32 iUncomprLen);

		// Member variables
	public:
	protected:
	private:
		// Manifest metadata
		char              m_ArkFilename[MAXPATH];
		MetadataList      m_ArkIndex;

		// Temp source file data 
		char*             m_pSrcFile;
		uint32            m_iSrcBufferSize;

		// Temp compressed file data
		char*             m_pComprFile;
		uint32            m_iComprBufferSize;

		// Temp encrypted file data
		char*             m_pEncrFile;
		uint32            m_iEncrBufferSize;

		float             m_fCompressionThreshold;
		uint32            m_iUncompressedFileCount;

		// Ark archive
		uint32            m_iCurrentArkSize;
		FILE*             m_pArkStream;

		// Error reporting
		bool              m_bMessageBox;
	};
}
#endif  // __ARK_WRITER_H__