#include "../Include/Ark.h"
#include <cstring>
#include <cstdlib>
#include "../../Common/RKMacros.h"
#include "../../Common/RKUtil.h"
#include "../../RKTool_Code/Encryption.cpp"
#include "../../RKString_Code/RKStringFunctions.h"
#include "../RKFile.h"

#if defined OS_W8 || defined OS_W10
#include "../../RKPlatform/Win8.1/Win8Shared.h"
#include <string>
#endif
namespace RKUtils
{
	Ark::Ark()
	{
		memset(&m_Header, 0, sizeof(ArkHeader));
		m_pIndex = 0;
	}
	Ark::~Ark()
	{
		UnloadIndex();
	}
	bool Ark::Initialise(const char* pAbsFilename)
	{
		bool bSuccess = true;

		// Open ArkArchive
#if defined OS_W8 || defined OS_W10
		FILE* pArkFile = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(pAbsFilename).data(), L"rb");
#else
		FILE* pArkFile = fopen(pAbsFilename, "rb");
#endif
		if (pArkFile)
		{
			m_sArkFilename = pAbsFilename;

			// Load header
			if (1 == fread(&m_Header, sizeof(ArkHeader), 1, pArkFile))
			{
				// Validate header
				if (m_Header.iNumFiles <= 0)
				{
					RKLOG_ERROR("Invalid file count: %i", m_Header.iNumFiles);
					bSuccess = false;
				}
				if (m_Header.iIndexOffset <= sizeof(ArkHeader))
				{
					RKLOG_ERROR("Invalid offset: %i", m_Header.iIndexOffset);
					bSuccess = false;
				}
				if (0 != strcmp(m_Header.pArkVersion, g_kArkVersion))
				{
					RKLOG_ERROR("ArkVersion mismatch - System: %s Ark: %s", g_kArkVersion, m_Header.pArkVersion);
					bSuccess = false;
				}
			}
			else
			{
				RKLOG_ERROR("Error reading header for Ark file: %s", pAbsFilename);
				bSuccess = false;
			}

			fclose(pArkFile);
		}
		else
		{
			RKLOG_WARN("Failed to open Ark file: %s", pAbsFilename);
			bSuccess = false;
		}

		return bSuccess;
	}

	/**
	* GetFileHandle
	* Populates given ArkFileHandle, allocates memory for file, and reads file into that memory.
	* If file was not compressed, the function will simply read the data into the buffer.
	* Will also decrypt the file if necessary.
	* Liam de Koster-Kjaer
	**/
	bool Ark::GetFileHandle(ArkFileMetadata& Meta, ArkFileHandle& outHandle)
	{
		bool bSuccess = true;

		//!>> ArkFilename --------------------------------------------------------------------------
		strcpy(outHandle.pArkFilename, m_sArkFilename.GetString());
		//!>> Filename -----------------------------------------------------------------------------
		strcpy(outHandle.pFilename, Meta.pFilename);
		//!>> Data ---------------------------------------------------------------------------------
		// Open ArkArchive
#ifdef OS_W8
		FILE* pStream = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(m_sArkFilename.GetString()).data(), L"rb");
#else
		FILE* pStream = fopen(m_sArkFilename.GetString(), "rb");
#endif
		if (pStream)
		{
			// Seek to file data in ArkArchive
			if (0 == fseek(pStream, Meta.iArkOffset, SEEK_SET))
			{
				outHandle.pData = RKNEWARRAY(char, Meta.iSrcFileSize);

				// If the file was encrypted
				if (Meta.iEncrFileSize)
				{
					// Read encrypted data into memory
					char* pTempData = RKNEWARRAY(char, Meta.iEncrFileSize);
					RKASSERT(pTempData, "Failed to create encrypted data buffer");
					uint32 iRead = fread(pTempData, Meta.iEncrFileSize, 1, pStream);
					if (iRead == 1)
					{
						if (Encryption::DecryptXXTEA(pTempData, Meta.iEncrFileSize, pTempData, Meta.iEncrFileSize, g_kiEncryptKey))
						{
							// If the file was compressed
							if (Meta.iComprFileSize != Meta.iSrcFileSize)
							{
								// Decompress decrypted data into outHandle.pData
								if (RKDecompress(outHandle.pData, Meta.iSrcFileSize, pTempData, Meta.iComprFileSize) != Meta.iSrcFileSize)
								{
									RKLOG_ERROR("Failed to decompress encrypted file: %s/%s", m_sArkFilename.GetString(), Meta.pFilename);
									bSuccess = false;
								}
							}
							else  // The file was not compressed
								memcpy(outHandle.pData, pTempData, Meta.iSrcFileSize); // Copy decrypted data into outhandle
						}
						else
						{
							RKLOG_ERROR("Failed to decrypt file: %s/%s", m_sArkFilename.GetString(), Meta.pFilename);
							bSuccess = false;
						}
					}
					else
					{
						RKLOG_ERROR("Error reading encrypted file: %s/%s", m_sArkFilename.GetString(), Meta.pFilename);
						bSuccess = false;
					}

					RKDELETEARRAY(pTempData);
				}
				else  // The file was not encrypted
				{
					// If the file was compressed
					if (Meta.iComprFileSize != Meta.iSrcFileSize)
					{
						// Read compressed data into memory
						char* pComprData = RKNEWARRAY(char, Meta.iComprFileSize);
						RKASSERT(pComprData, "Failed to create compressed data buffer");
						uint32 iRead = fread(pComprData, Meta.iComprFileSize, 1, pStream);
						if (iRead == 1)
						{
							// Decompress pComprData into outHandle.pData
							if (RKDecompress(outHandle.pData, Meta.iSrcFileSize, pComprData, Meta.iComprFileSize) != Meta.iSrcFileSize)
							{
								RKLOG_ERROR("Failed to decompress file: %s/%s", m_sArkFilename.GetString(), Meta.pFilename);
								bSuccess = false;
							}
						}
						else
						{
							RKLOG_ERROR("Error reading compressed file: %s/%s", m_sArkFilename.GetString(), Meta.pFilename);
							bSuccess = false;
						}

						RKDELETEARRAY(pComprData);
					}
					else  // The file was not compressed
					{
						// Read archived data into outHandle.pData
						uint32 iRead = fread(outHandle.pData, Meta.iSrcFileSize, 1, pStream);
						if (iRead != 1)
						{
							RKLOG_ERROR("Error reading file: %s/%s", m_sArkFilename.GetString(), Meta.pFilename);
							bSuccess = false;
						}
					}
				}
			}

			fclose(pStream);
		}
		//!>> File size ----------------------------------------------------------------------------
		outHandle.iFileSize = Meta.iSrcFileSize;
		//!>> Iterator -----------------------------------------------------------------------------
		outHandle.iOffset = 0;

		return bSuccess;
	}


	ArkFileMetadata* Ark::GetArkIndex()
	{
		if (!m_pIndex)
			LoadIndex();

		return (ArkFileMetadata*)m_pIndex;
	}

	void Ark::UnloadIndex()
	{
		if (m_pIndex)
		{
			RKHeap_Free(m_pIndex);
			m_pIndex = 0;
		}
	}

	uint32 Ark::GetArkNumber()
	{
		char pFilename[256] = { 0 };
		RKString_ExtractFileName(m_sArkFilename.GetString(), pFilename, sizeof(pFilename));
		char pArknumber[5] = { pFilename[0], pFilename[1], pFilename[2], pFilename[3], '\0' };
		return atoi(pArknumber);
	}

	bool Ark::ExtractFile(const char* pFilename, const char* pAbsOutDir)
	{
		RKASSERT(pFilename, "pFilename is null");
		RKASSERT(pAbsOutDir, "pAbsOutDir is null");

		ArkFileMetadata* pIndex = GetArkIndex();
		ArkFileMetadata* pMeta = 0;
		if (pIndex)
		{
			for (uint32 i = 0; i < m_Header.iNumFiles; ++i)
			{
				if (0 == strcmp(pFilename, pIndex[i].pFilename))
				{
					pMeta = &pIndex[i];
					break;
				}
			}
		}

		UnloadIndex();

		if (pMeta)
		{
			if (RKFile_DirExistsAbs(pAbsOutDir))  // Does not need RKFile_InitModule
			{
				ArkFileHandle Handle;
				if (GetFileHandle(*pMeta, Handle))
				{
					char pAbsFilepath[1024] = { 0 };
					strcpy(pAbsFilepath, pAbsOutDir);
					strcat(pAbsFilepath, pFilename);
#if defined OS_W8 || defined OS_W10
					FILE* pFile = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(pAbsFilepath).data(), L"wb");
#else
					FILE* pFile = fopen(pAbsFilepath, "wb");
#endif
					if (pFile)
					{
						int iWritten = fwrite(Handle.pData, Handle.iFileSize, 1, pFile);
						fclose(pFile);

						if (iWritten == 1)
							return true;
					}
				}
			}
		}

		return false;
	}

	bool Ark::LoadIndex()
	{
		bool bSuccess = true;

		UnloadIndex();
#if defined OS_W8 || defined OS_W10
		FILE* pArkFile = _wfopen(Win8Toolkit::ConvertUnicodedFolderPath(m_sArkFilename.GetString()).data(), L"rb");
#else
		FILE* pArkFile = fopen(m_sArkFilename.GetString(), "rb");
#endif
		if (pArkFile)
		{
			// Seek to encrypted ArkIndex
			if (0 == fseek(pArkFile, m_Header.iIndexOffset, SEEK_SET))
			{
				// The Ark index is encrypted using XXTEA, this means the encrypted index may be bigger than sizeof(ArkFileMetadata) * m_Header.iNumFiles
				uint32 iIndexSize = sizeof(ArkFileMetadata) * m_Header.iNumFiles;
				uint32 iEncryptedSize = Encryption::GetEncryptedXXTEADataSize(iIndexSize);

				m_pIndex = (char*)RKHeap_Alloc(iEncryptedSize);
				RKASSERT(m_pIndex, "Failed to allocate Ark index data buffer - low memory?");

				uint32 iRead = fread(m_pIndex, iEncryptedSize, 1, pArkFile);
				if (iRead == 1)
				{
					if (!Encryption::DecryptXXTEA(m_pIndex, iEncryptedSize, m_pIndex, iIndexSize, g_kiEncryptKey))
					{
						UnloadIndex();
						RKLOG_ERROR("Failed to decrypt Ark index: %s", m_sArkFilename.GetString());
						bSuccess = false;
					}
				}
				else
				{
					UnloadIndex();
					RKLOG_ERROR("Error reading encrypted index buffer for Ark: %s", m_sArkFilename.GetString());
					bSuccess = false;
				}
			}
			else
			{
				RKLOG_ERROR("Failed to seek to index for Ark: %s", m_sArkFilename.GetString());
				bSuccess = false;
			}
		}
		else
		{
			RKLOG_ERROR("Failed to open Ark archive file: %s", m_sArkFilename.GetString());
			bSuccess = false;
		}

		return bSuccess;
	}

}