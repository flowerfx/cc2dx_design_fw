// 
// ArkWriter.cpp
// Liam de Koster-Kjaer
// January 2012
// 

#include "CasualCoreConfig.h"
#if CC_USE_ARK==1

#include "ArkWriter.h"
#include <cassert>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <Windows.h>
#include "RKMacros.h"
#include "RKUtil.h"
#include "Tools/Encryption.h"

#define ErrorLogFilename    "ArkWriter_ErrorLog.txt"
#define TempFileBufferSize  5000

/**
* Constructor
* Liam de Koster-Kjaer
**/
ArkWriter::ArkWriter()
  : m_pSrcFile(0)
  , m_iSrcBufferSize(0)
  , m_pComprFile(0)
  , m_iComprBufferSize(0)
  , m_pEncrFile(0)
  , m_iEncrBufferSize(0)
  , m_fCompressionThreshold(0.9f)
  , m_iUncompressedFileCount(0)
  , m_iCurrentArkSize(0)
  , m_pArkStream(0)
  , m_bMessageBox(true)
{
  memset(m_ArkFilename, 0, MAXPATH);
}

/**
* Destructor
* Liam de Koster-Kjaer
**/
ArkWriter::~ArkWriter()
{
  if (m_pArkStream)
    fclose(m_pArkStream);

  if (m_pSrcFile)
    RKDELETEARRAY(m_pSrcFile);

  if (m_pComprFile)
    RKDELETEARRAY(m_pComprFile);

  if (m_pEncrFile)
    RKDELETEARRAY(m_pEncrFile);

  for (uint32 i = 0; i < m_ArkIndex.size(); ++i)
    RKDELETE(m_ArkIndex[i]);
}

/**
* InitialiseArk
* Liam de Koster-Kjaer
**/
bool ArkWriter::InitialiseArk(const char* pArkFilename, bool bMessageBoxErrors)
{
  assert(pArkFilename);

  bool bSuccess = true;

  m_iUncompressedFileCount = 0;

  // Set member data
  strcpy(m_ArkFilename, pArkFilename);
  m_bMessageBox = bMessageBoxErrors;

  // Initialise Ark stream
  m_pArkStream = fopen(m_ArkFilename, "wb");
  if (m_pArkStream)
  {
    // Reserve space at beginning of ArkArchive for header:
    fseek(m_pArkStream, sizeof(ArkHeader), SEEK_SET);
    m_iCurrentArkSize = sizeof(ArkHeader);

    // Create temporary file buffers
    // Source file data buffer
    m_iSrcBufferSize = TempFileBufferSize;
    if (m_pSrcFile) RKDELETEARRAY(m_pSrcFile);
    m_pSrcFile = RKNEWARRAY(char, m_iSrcBufferSize);
    if (m_pSrcFile)
    {
      memset(m_pSrcFile, 0, m_iSrcBufferSize);

      // Compressed file data buffer
      m_iComprBufferSize = TempFileBufferSize;
      if (m_pComprFile) RKDELETEARRAY(m_pComprFile);
      m_pComprFile = RKNEWARRAY(char, m_iComprBufferSize);
      if (m_pComprFile)
      {
        memset(m_pComprFile, 0, m_iComprBufferSize);

        // Encrypted file data buffer
        m_iEncrBufferSize = TempFileBufferSize;
        if (m_pEncrFile) RKDELETEARRAY(m_pEncrFile);
        m_pEncrFile = RKNEWARRAY(char, m_iEncrBufferSize);
        if (m_pEncrFile)
          memset(m_pEncrFile, 0, m_iEncrBufferSize);
        else
        {
          char pErrorMsg[MAXPATH];
          sprintf(pErrorMsg, "ArkWriter::InitialiseArk - Failed to allocate memory for encrypted file buffer: %s\n", pArkFilename);
          ErrorMessageAlert(pErrorMsg);
          bSuccess = false;
        }
      }
      else
      {
        char pErrorMsg[MAXPATH];
        sprintf(pErrorMsg, "ArkWriter::InitialiseArk - Failed to allocate memory for compressed file buffer: %s\n", pArkFilename);
        ErrorMessageAlert(pErrorMsg);
        bSuccess = false;
      }
    }
    else
    {
      char pErrorMsg[MAXPATH];
      sprintf(pErrorMsg, "ArkWriter::InitialiseArk - Failed to allocate memory for source file buffer: %s\n", pArkFilename);
      ErrorMessageAlert(pErrorMsg);
      bSuccess = false;
    }
  }
  else
  {
    char pErrorMsg[MAXPATH];
    sprintf(pErrorMsg, "ArkWriter::InitialiseArk - Failed to Failed to open file stream: %s\n", pArkFilename);
    ErrorMessageAlert(pErrorMsg);
    bSuccess = false;
  }

  return bSuccess;
}

/**
* AddFileToArk
* pFilename must be absolute path to the file.
* Liam de Koster-Kjaer
**/
bool ArkWriter::AddFileToArk(const char* pFilename, const char* pSubdir, bool bExtract, bool bCompress, bool bEncrypt)
{
  assert(pFilename);

  bool bSuccess = true;

  ArkFileMetadata* pInfo = RKNEW(ArkFileMetadata);
  memset(pInfo, 0, sizeof(ArkFileMetadata));

  if (bExtract)
    pInfo->SetFlagOn(AFF_EXTRACT);

  if (pSubdir)
    strncpy(pInfo->pSubdir, pSubdir, MAXSTRING);

  if (LoadSourceFile(pFilename, pInfo))  // Load source file into temporary buffer
  {
    if (bCompress)
    {// Compress source file into temporary buffer
      ResizeBufferToFit(m_pComprFile, m_iComprBufferSize, pInfo->iSrcFileSize);
      pInfo->iComprFileSize = RKCompress(m_pComprFile, m_iComprBufferSize, m_pSrcFile, pInfo->iSrcFileSize);

      // If compression has reached the threshold...
      if (pInfo->iComprFileSize <= m_fCompressionThreshold * pInfo->iSrcFileSize)
      {
        if (bEncrypt)
        {
          if (EncryptFileBuffer(pInfo, m_pComprFile, pInfo->iComprFileSize, m_pEncrFile, m_iEncrBufferSize))  // Will set pInfo->iEncrFileSize
          {
            if (!WriteFileToArk(pInfo, m_pEncrFile, pInfo->iEncrFileSize)) // Write compressed and encrypted file to ArkArchive
            {
              char pErrorMsg[MAXPATH];
              sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to write compressed and encrypted file: %s to archive: %s\n", pFilename, m_ArkFilename);
              ErrorMessageAlert(pErrorMsg);
              bSuccess = false;
            }
          }
          else
          {
            char pErrorMsg[MAXPATH];
            sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to encrypt compressed file: %s to archive: %s\n", pFilename, m_ArkFilename);
            ErrorMessageAlert(pErrorMsg);
            bSuccess = false;
          }
        }
        else  // no encryption
        {
          if (!WriteFileToArk(pInfo, m_pComprFile, pInfo->iComprFileSize)) // Write compressed source file to ArkArchive
          {
            char pErrorMsg[MAXPATH];
            sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to write compressed source file: %s to archive: %s\n", pFilename, m_ArkFilename);
            ErrorMessageAlert(pErrorMsg);
            bSuccess = false;
          }
        }
      }
      else  // Compression did not reach the threshold, using uncompressed data
      {
        if (bEncrypt)
        {
          if (EncryptFileBuffer(pInfo, m_pSrcFile, pInfo->iSrcFileSize, m_pEncrFile, m_iEncrBufferSize))  // Will set pInfo->iEncrFileSize
          {
            pInfo->iComprFileSize = pInfo->iSrcFileSize;  // Set compressed size to src size, this indicates to the ark manager that the file has not been compressed
            if (WriteFileToArk(pInfo, m_pEncrFile, pInfo->iEncrFileSize)) // Write encrypted source file to ArkArchive
              ++m_iUncompressedFileCount;
            else
            {
              char pErrorMsg[MAXPATH];
              sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to write encrypted source file: %s to archive: %s\n", pFilename, m_ArkFilename);
              ErrorMessageAlert(pErrorMsg);
              bSuccess = false;
            }
          }
          else
          {
            char pErrorMsg[MAXPATH];
            sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to encrypt source file: %s to archive: %s\n", pFilename, m_ArkFilename);
            ErrorMessageAlert(pErrorMsg);
            bSuccess = false;
          }
        }
        else  // No encryption
        {
          pInfo->iComprFileSize = pInfo->iSrcFileSize;  // Set compressed size to src size, this indicates to the ark manager that the file has not been compressed
          if (WriteFileToArk(pInfo, m_pSrcFile, pInfo->iSrcFileSize)) // Write source file to ArkArchive
            ++m_iUncompressedFileCount;
          else
          {
            char pErrorMsg[MAXPATH];
            sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to write file: %s to archive: %s\n", pFilename, m_ArkFilename);
            ErrorMessageAlert(pErrorMsg);
            bSuccess = false;
          }
        }
      }
    }
    else  // Forced no compression
    {
      if (bEncrypt)
      {
        if (EncryptFileBuffer(pInfo, m_pSrcFile, pInfo->iSrcFileSize, m_pEncrFile, m_iEncrBufferSize))  // Will set pInfo->iEncrFileSize
        {
          pInfo->iComprFileSize = pInfo->iSrcFileSize;  // Set compressed size to src size, this indicates to the ark manager that the file has not been compressed
          if (WriteFileToArk(pInfo, m_pEncrFile, pInfo->iEncrFileSize)) // Write encrypted source file to ArkArchive
            ++m_iUncompressedFileCount;
          else
          {
            char pErrorMsg[MAXPATH];
            sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to write encrypted source file: %s to archive: %s\n", pFilename, m_ArkFilename);
            ErrorMessageAlert(pErrorMsg);
            bSuccess = false;
          }
        }
        else
        {
          char pErrorMsg[MAXPATH];
          sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to encrypt source file: %s to archive: %s\n", pFilename, m_ArkFilename);
          ErrorMessageAlert(pErrorMsg);
          bSuccess = false;
        }
      }
      else  // No encryption
      {
        pInfo->iComprFileSize = pInfo->iSrcFileSize;  // Set compressed size to src size, this indicates to the ark manager that the file has not been compressed
        if (WriteFileToArk(pInfo, m_pSrcFile, pInfo->iSrcFileSize)) // Write uncompressed file to ArkArchive
          ++m_iUncompressedFileCount;
        else
        {
          char pErrorMsg[MAXPATH];
          sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to write file: %s to archive: %s\n", pFilename, m_ArkFilename);
          ErrorMessageAlert(pErrorMsg);
          bSuccess = false;
        }
      }
    }
  }
  else
  {
    char pErrorMsg[MAXPATH];
    sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to write file: %s to archive: %s\n", pFilename, m_ArkFilename);
    ErrorMessageAlert(pErrorMsg);
    bSuccess = false;
  }

  if (bSuccess)
  {
    // Calculate MD5
    RKString sMD5Digest;
    if (CasualCore::Encryption::EncryptMD5(m_pSrcFile, pInfo->iSrcFileSize, sMD5Digest))
    {
      memcpy(pInfo->pMD5Digest, sMD5Digest.GetString(), MD5DIGESTSIZE);
      m_ArkIndex.push_back(pInfo);
    }
    else
    {
      char pErrorMsg[MAXPATH];
      sprintf(pErrorMsg, "ArkWriter::AddFileToArk - Failed to genereate MD5 digest for file: %s to archive: %s\n", pFilename, m_ArkFilename);
      ErrorMessageAlert(pErrorMsg);
      bSuccess = false;
    }
  }
  else
    RKDELETE(pInfo);

  return bSuccess;
}

/**
* GetCurrentArkSize
* Liam de Koster-Kjaer
**/
uint32 ArkWriter::GetCurrentArkSize() const
{
  return m_iCurrentArkSize + sizeof(ArkFileMetadata) * m_ArkIndex.size();
}

/**
* FinaliseArk
* Liam de Koster-Kjaer
**/
bool ArkWriter::FinaliseArk(bool bValidateMD5)
{
  bool bSuccess = true;

  if (!WriteHeaderToArk())  // Write header to beginning of ArkArchive
    bSuccess = false;

  if (bSuccess) 
    bSuccess = WriteIndexToArk(); // Write index file to end of ArkArchive 

  fclose(m_pArkStream);
  m_pArkStream = 0;

  if (bSuccess && bValidateMD5) 
    bSuccess = MD5ValidateFiles(m_ArkFilename); // Validate files using MD5 digest

  LogArkIndex();
  LogArkFileMetadata();

  return bSuccess;
}

void ArkWriter::SetCompressionThreshold(float fThreshold)
{
  m_fCompressionThreshold = RKMAX(0.0f, RKMIN(1.0f, fThreshold));
}

/**
* LoadSourceFile
* Loads an uncompressed file into memory(into m_pSrcFile), loads filename and source file size into pInfo.
* pFilename must be absolute path to the file.
* Liam de Koster-Kjaer
**/
bool ArkWriter::LoadSourceFile(const char* pFilename, ArkFileMetadata* pInfo)
{
  bool bSuccess = true;

  // Open data file 
  FILE* pSrcFile = fopen(pFilename, "rb");
  if (pSrcFile)
  {
    pInfo->iSrcFileSize = GetFileSize(pSrcFile);
    if (pInfo->iSrcFileSize)
    {
      // Remove path from filename, after using the path to find the file
      const char* pIndex = pFilename;

      do
      {
        pIndex = strchr(pIndex, '\\');
        if (pIndex)
        {
          ++pIndex;
          pFilename = pIndex;
        }
      }
      while (pIndex);

      if (strlen(pFilename) < MAXSTRING)
      {
        // Obtain uncompressed/source file filename
        strcpy(pInfo->pFilename, pFilename);

        // Ensure space in uncompressed/source file buffer
        if (m_iSrcBufferSize < pInfo->iSrcFileSize)
          ResizeBufferToFit(m_pSrcFile, m_iSrcBufferSize, pInfo->iSrcFileSize);

        // Read uncompressed/source file into memory
        uint32 elementsRead = fread(m_pSrcFile, sizeof(char), pInfo->iSrcFileSize, pSrcFile);
        if (elementsRead != pInfo->iSrcFileSize)
        {
          char pErrorMsg[MAXPATH];
          sprintf(pErrorMsg, "ArkWriter::LoadSourceFile - Error reading file: %s\n", pFilename);
          ErrorMessageAlert(pErrorMsg);
          bSuccess = false;
        }
      }
      else
      {
        char pErrorMsg[MAXPATH];
        sprintf(pErrorMsg, "ArkWriter::LoadSourceFile - Filename is too long , ArkWriter will not add file to ArkArchive: %s\n", pFilename);
        ErrorMessageAlert(pErrorMsg);
        bSuccess = false;
      }

    }
    else
    {
      char pErrorMsg[MAXPATH];
      sprintf(pErrorMsg, "ArkWriter::LoadSourceFile - Filesize zero, ArkWriter will not add file to ArkArchive: %s\n", pFilename);
      ErrorMessageAlert(pErrorMsg);
      bSuccess = false;
    }

    fclose(pSrcFile);
  }
  else
  {
    char pErrorMsg[MAXPATH];
    sprintf(pErrorMsg, "ArkWriter::LoadSourceFile - could not open file: %s\n", pFilename);
    ErrorMessageAlert(pErrorMsg);
    bSuccess = false;
  }

  return bSuccess;
}

bool ArkWriter::EncryptFileBuffer(ArkFileMetadata* pInfo, char* pInData, uint32 iInSize, char*& pOutData, uint32& iOutSize)
{
  RKASSERT(pInfo, "ArkWriter::EncryptFile - pInfo is null");
  RKASSERT(pInData, "ArkWriter::EncryptFile - pInData is null");
  RKASSERT(iInSize, "ArkWriter::EncryptFile - iInSize is null");
  RKASSERT(pOutData, "ArkWriter::EncryptFile - pOutData is null");
  RKASSERT(iOutSize, "ArkWriter::EncryptFile - iOutSize is null");

  // Ensure space in encrypted file buffer
  pInfo->iEncrFileSize = CasualCore::Encryption::GetEncryptedXXTEADataSize(iInSize);
  if (iOutSize < pInfo->iEncrFileSize)
      ResizeBufferToFit(pOutData, iOutSize, pInfo->iEncrFileSize);  // Will set pOutData to new buffer, and iOutSize to new size

  return CasualCore::Encryption::EncryptXXTEA(pInData, iInSize, pOutData, iOutSize, g_kiEncryptKey);
}

/**
* WriteFileToArk
* Liam de Koster-Kjaer
**/
bool ArkWriter::WriteFileToArk(ArkFileMetadata* pInfo, char* pData, uint32 iSize)
{
  bool bSuccess = true;

  // Write file to ark
  uint32 iElementsWritten = fwrite(pData, sizeof(char), iSize, m_pArkStream);

  // Write file info to ArkIndex
  if (iElementsWritten == iSize)
  {
    pInfo->iArkOffset = m_iCurrentArkSize;
    pInfo->iTimestamp = time(0);

    m_iCurrentArkSize += iSize;
    assert(ftell(m_pArkStream) == m_iCurrentArkSize); // Sanity check - Ensure ArkArchive is as big as we think it is
  }
  else
  {
    ErrorMessageAlert("ArkWriter::WriteFileToArk - fwrite wrote an amount of data inequal to the size of the compressed file");
    bSuccess = false;
  }

  return bSuccess;
}

/**
* WriteHeaderToArk
* Writes header(Number of files, offest to ArkIndex) to ArkArchive.
* Liam de Koster-Kjaer
**/
bool ArkWriter::WriteHeaderToArk()
{
  bool bSuccess = true;

  ArkHeader header;
  header.iNumFiles    = m_ArkIndex.size();
  header.iIndexOffset = m_iCurrentArkSize;
  strncpy(header.pArkVersion, g_kArkVersion, MAXVERSIONSTRING);

  // Write header at the start of the file...
  fseek(m_pArkStream, 0, SEEK_SET);

  if (1 != fwrite(&header, sizeof(ArkHeader), 1, m_pArkStream))
  {
    ErrorMessageAlert("ArkWriter::WriteIndexToArk - Failed to write header to ArkArchive");
    bSuccess = false;
  }

  return bSuccess;
}

/**
* WriteIndexToArk
* Writes ArkIndex to the end of the ArkArchive.
* The first word (4bytes) of the ArkArchive contains the number of files in the ArkArchive.
* The second word contains the offset from the beginning of the ArkArchive file to the beginning of the ArkIndex (uint32).
* Encrypts the index using XXTEA.
* Liam de Koster-Kjaer
**/
bool ArkWriter::WriteIndexToArk()
{
  bool bSuccess = true;

  // Create a contigious array of ArkFileMetadata structs
  uint32 iNumFiles = m_ArkIndex.size();
  ArkFileMetadata* pArkIndex = RKNEWARRAY(ArkFileMetadata, iNumFiles);
  uint32 iIndexSize = sizeof(ArkFileMetadata) * iNumFiles;
  memset(pArkIndex, 0, iIndexSize);
  for (uint32 i = 0; i < iNumFiles; ++i)
    memcpy(&pArkIndex[i], m_ArkIndex[i], sizeof(ArkFileMetadata));

  // Encrypt Index
  uint32 iEncryptedSize = CasualCore::Encryption::GetEncryptedXXTEADataSize(iIndexSize);
  char* pEncryptedIndex = 0;
  bool bUseTwoBuffers = iEncryptedSize != iIndexSize;
  if (bUseTwoBuffers)
  {
    pEncryptedIndex = RKNEWARRAY(char, iEncryptedSize);
    memset(pEncryptedIndex, 0, iEncryptedSize);
  }
  else
    pEncryptedIndex = (char*)pArkIndex;

  bSuccess = CasualCore::Encryption::EncryptXXTEA(pArkIndex, iIndexSize, pEncryptedIndex, iEncryptedSize, g_kiEncryptKey);

  // Write encrypted index to archive
  fseek(m_pArkStream, 0, SEEK_END); // Seek back to end of data in ArkArchive
  uint32 iWritten = fwrite(pEncryptedIndex, iEncryptedSize, 1, m_pArkStream);
  if (iWritten == 1)
    m_iCurrentArkSize += iEncryptedSize;
  else
  {
    ErrorMessageAlert("ArkWriter::WriteIndexToArk - Failed to write ArkIndex to ArkArchive");
    bSuccess = false;
  }

  RKDELETEARRAY(pArkIndex);
  if (bUseTwoBuffers)
    RKDELETEARRAY(pEncryptedIndex);

  return bSuccess;
}

/**
* GetFileSize
* Returns size of a file in bytes, sets file stream position to start of file.
* Liam de Koster-Kjaer
**/
uint32 ArkWriter::GetFileSize(FILE* pFile)
{
  fseek(pFile, 0, SEEK_END);
  uint32 iSize = ftell(pFile);
  fseek(pFile, 0, SEEK_SET);
  return iSize;
}

/**
* ResizeBufferToFit
* Takes a member data buffer, a member variable containing buffer size and a size that the buffer must exceed.
* Creates a new buffer (larger than iSizeToFit), copies old buffer data into new buffer, deletes old buffer
* and reassigns given buffer pointer and size variable.
* Liam de Koster-Kjaer
**/
void ArkWriter::ResizeBufferToFit(char*& pBuf, uint32& iCurrentSize, uint32 iSizeToFit, bool bCopyOldBuffer)
{
  if (iSizeToFit <= iCurrentSize)
    return;

  // Obtain a buffer size large enough to fit the file
  uint32 iNewSize = iCurrentSize;
  while (iNewSize < iSizeToFit)
    iNewSize += 1024 * 1024 * 1;  // 1MB

  // Allocate new buffer
  char* pNewBuf = RKNEWARRAY(char, iNewSize);
  assert(pNewBuf);
  memset(pNewBuf, 0, iNewSize);
  // Copy old data into new buffer
  if (bCopyOldBuffer)
    memcpy(pNewBuf, (void*)pBuf, iCurrentSize);
  // Delete old buffer
  RKDELETEARRAY(pBuf);
  // Assign new buffer and size 
  pBuf = pNewBuf;
  iCurrentSize = iNewSize;
}

/**
* ErrorMessageAlert
* Liam de Koster-Kjaer
**/
void ArkWriter::ErrorMessageAlert(const char* pMsg)
{
  assert(pMsg && "ArkWriter::ErrorMessageAlert - pMsg is null");

  if (m_bMessageBox)
  {
    wchar_t pWideCharBuf[MAXPATH];
    memset(pWideCharBuf, 0, sizeof(wchar_t) * MAXPATH);

    uint32 iLen = strlen(pMsg);
    iLen = iLen > MAXPATH ? MAXPATH : iLen;
    uint32 i = 0;

    for (i; i < iLen; ++i)
      pWideCharBuf[i] = pMsg[i];

    pWideCharBuf[i] = '\0';

    MessageBoxW(NULL, pWideCharBuf, L"ArkWriter - Error", MB_OK | MB_ICONEXCLAMATION);
  }
  else
    printf("%s\n", pMsg);
}

/**
* MD5ValidateFiles
* Liam de Koster-Kjaer
**/
bool ArkWriter::MD5ValidateFiles(const char* pArkFilename)
{
  bool bSuccess = true;

  FILE* pArkFile = fopen(pArkFilename, "rb");
  if (pArkFile)
  {
    uint32 iSize = GetFileSize(pArkFile);
    char* pArkBuf = RKNEWARRAY(char, iSize);
    memset(pArkBuf, 0, iSize);
    if (fread(pArkBuf, iSize, 1, pArkFile) == 1)
    {
      // ArkIndex is encrypted using XXTEA, this means the index may be bigger than sizeof(ArkFileMetadata) * m_Header.iNumFiles
      // For this reason, I will read the index into a temporary buffer, then decrypt 
      ArkHeader* pHeader      = (ArkHeader*)pArkBuf;
      uint32 iIndexSize       = sizeof(ArkFileMetadata) * pHeader->iNumFiles;
      uint32 iEncryptedSize   = CasualCore::Encryption::GetEncryptedXXTEADataSize(iIndexSize);
      char* pEncryptedIndex   = pArkBuf + pHeader->iIndexOffset;
      ArkFileMetadata* pIndex = (ArkFileMetadata*)RKNEWARRAY(char, iEncryptedSize);
      // Decrypt index
      if (CasualCore::Encryption::DecryptXXTEA(pEncryptedIndex, iEncryptedSize, pIndex, iEncryptedSize, g_kiEncryptKey))
      {
        RKString sMD5Digest;
        for (uint32 i = 0; i < pHeader->iNumFiles; ++i)
        {
          // If the file was encrypted
          if (pIndex[i].iEncrFileSize)
          {
            char* pDecrypted = RKNEWARRAY(char, pIndex[i].iEncrFileSize);
            if (CasualCore::Encryption::DecryptXXTEA(pArkBuf + pIndex[i].iArkOffset, pIndex[i].iEncrFileSize, pDecrypted, pIndex[i].iEncrFileSize, g_kiEncryptKey))
            {
              // If the file was compressed
              if (pIndex[i].iComprFileSize != pIndex[i].iSrcFileSize)
              {
                char* pDecompressed = RKNEWARRAY(char, pIndex[i].iSrcFileSize);
                if (RKDecompress(pDecompressed, pIndex[i].iSrcFileSize, pDecrypted, pIndex[i].iComprFileSize) == pIndex[i].iSrcFileSize)
                {
                  // Generate an MD5 digest for the file
                  sMD5Digest.Clear();
                  if (CasualCore::Encryption::EncryptMD5(pDecompressed, pIndex[i].iSrcFileSize, sMD5Digest))
                  {
                    // Check the MD5 digest
                    if (!RKString_Compare((const char*)pIndex[i].pMD5Digest, sMD5Digest.GetString()))
                    {
                      char pErrorMsg[MAXPATH];
                      sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to validate file: %s\n", pIndex[i].pFilename);
                      ErrorMessageAlert(pErrorMsg);
                      bSuccess = false;
                    }
                  }
                  else
                  {
                    char pErrorMsg[MAXPATH];
                    sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to generate MD5 digest for file: %s\n", pIndex[i].pFilename);
                    ErrorMessageAlert(pErrorMsg);
                    bSuccess = false;
                  }
                }
                else
                {
                  char pErrorMsg[MAXPATH];
                  sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to decompress file: %s\n", pIndex[i].pFilename);
                  ErrorMessageAlert(pErrorMsg);
                  bSuccess = false;
                }

                RKDELETEARRAY(pDecompressed);
              }
              else  // File was not compressed
              {
                // Generate an MD5 digest for the file
                sMD5Digest.Clear();
                if (CasualCore::Encryption::EncryptMD5(pDecrypted, pIndex[i].iSrcFileSize, sMD5Digest))
                {
                  // Check the MD5 digest
                  if (!RKString_Compare((const char*)pIndex[i].pMD5Digest, sMD5Digest.GetString()))
                  {
                    char pErrorMsg[MAXPATH];
                    sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to validate file: %s\n", pIndex[i].pFilename);
                    ErrorMessageAlert(pErrorMsg);
                    bSuccess = false;
                  }
                }
                else
                {
                  char pErrorMsg[MAXPATH];
                  sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to generate MD5 digest for file: %s\n", pIndex[i].pFilename);
                  ErrorMessageAlert(pErrorMsg);
                  bSuccess = false;
                }
              }
            }
            else
            {
              char pErrorMsg[MAXPATH];
              sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to decrypt file: %s\n", pIndex[i].pFilename);
              ErrorMessageAlert(pErrorMsg);
              bSuccess = false;
            }

            RKDELETEARRAY(pDecrypted);
          }
          else  // File was not encrypted
          {
            // If the file was compressed
            if (pIndex[i].iComprFileSize != pIndex[i].iSrcFileSize)
            {
              char* pDecompressed = RKNEWARRAY(char, pIndex[i].iSrcFileSize);
              if (RKDecompress(pDecompressed, pIndex[i].iSrcFileSize, pArkBuf + pIndex[i].iArkOffset, pIndex[i].iComprFileSize) == pIndex[i].iSrcFileSize)
              {
                // Generate an MD5 digest for the file
                sMD5Digest.Clear();
                if (CasualCore::Encryption::EncryptMD5(pDecompressed, pIndex[i].iSrcFileSize, sMD5Digest))
                {
                  // Check the MD5 digest
                  if (!RKString_Compare((const char*)pIndex[i].pMD5Digest, sMD5Digest.GetString()))
                  {
                    char pErrorMsg[MAXPATH];
                    sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to validate file: %s\n", pIndex[i].pFilename);
                    ErrorMessageAlert(pErrorMsg);
                    bSuccess = false;
                  }
                }
                else
                {
                  char pErrorMsg[MAXPATH];
                  sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to generate MD5 digest for file: %s\n", pIndex[i].pFilename);
                  ErrorMessageAlert(pErrorMsg);
                  bSuccess = false;
                }
              }
              else
              {
                char pErrorMsg[MAXPATH];
                sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to decompress file: %s\n", pIndex[i].pFilename);
                ErrorMessageAlert(pErrorMsg);
                bSuccess = false;
              }

              RKDELETEARRAY(pDecompressed);
            }
            else  // File was not compressed
            {
              // Generate an MD5 digest for the file
              sMD5Digest.Clear();
              if (CasualCore::Encryption::EncryptMD5(pArkBuf + pIndex[i].iArkOffset, pIndex[i].iSrcFileSize, sMD5Digest))
              {
                // Check the MD5 digest
                if (!RKString_Compare((const char*)pIndex[i].pMD5Digest, sMD5Digest.GetString()))
                {
                  char pErrorMsg[MAXPATH];
                  sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to validate file: %s\n", pIndex[i].pFilename);
                  ErrorMessageAlert(pErrorMsg);
                  bSuccess = false;
                }
              }
              else
              {
                char pErrorMsg[MAXPATH];
                sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to generate MD5 digest for file: %s\n", pIndex[i].pFilename);
                ErrorMessageAlert(pErrorMsg);
                bSuccess = false;
              }
            }
          }
        }
      }
      else
      {
        char pErrorMsg[MAXPATH];
        sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to decrypt ArkIndex: \n%s", pArkFilename);
        ErrorMessageAlert(pErrorMsg);
        bSuccess = false;
      }

      RKDELETEARRAY(pIndex);
    }
    else
    {
      char pErrorMsg[MAXPATH];
      sprintf(pErrorMsg, "ArkWriter::MD5ValidateFiles - Failed to read Arkfile: %s\n", pArkFilename);
      ErrorMessageAlert(pErrorMsg);
      bSuccess = false;
    }

    RKDELETEARRAY(pArkBuf);
    fclose(pArkFile);
  }

  return bSuccess;
}

/**
* LogArkIndex
* Logs ArkIndex to file for debugging purposes.
* Liam de Koster-Kjaer
**/
void ArkWriter::LogArkIndex()
{
  if (m_ArkIndex.size() > 0)
  {
    // Create meaningful filename for logged ArkIndex. Don't judge my ugly code! xD
    char pFilename[MAXPATH];
    memset(pFilename, 0, sizeof(pFilename));  // So we don't have to null terminate 
    strcpy(pFilename, m_ArkFilename);

    char* pExt = strrchr(pFilename, '.');
    if (!pExt)
    {
      RKASSERT(pExt, "ArkWriter::LogArkIndex - Invalid ArkFilename, failed to log ArkIndex");
      return;
    }

    const char* pFilenameSuffix = "_Index.txt";
    const uint32 iLen = strlen(pFilenameSuffix);
    for (uint32 i = 0; i < iLen; ++i)
      pExt[i] = pFilenameSuffix[i];

    FILE* pArkIndex = fopen(pFilename, "w");
    if (pArkIndex)
    {
      char* pBuf = RKNEWARRAY(char, MAXSTRING);
      assert(pBuf && "ArkWriter::LogArkIndex - Failed to allocate temporary buffer");
      memset(pBuf, 0, MAXSTRING);

      // Write MD5 digest of ArkArchive to log
      FILE* pArkArchive = fopen(m_ArkFilename, "rb");
      if (pArkArchive)
      {
        char* pArkBuf = RKNEWARRAY(char, m_iCurrentArkSize);
        assert(pArkBuf && "ArkWriter::LogArkIndex - Failed to allocate ark buffer");
        memset(pArkBuf, 0, m_iCurrentArkSize);
        uint32 iRead = fread(pArkBuf, m_iCurrentArkSize, 1, pArkArchive);
        if (iRead == 1)
        {
          RKString sMD5Digest;
          if (CasualCore::Encryption::EncryptMD5(pArkBuf, m_iCurrentArkSize, sMD5Digest))
          {
            fputs("MD5 Digest:            ", pArkIndex);
            fputs(sMD5Digest.GetString(), pArkIndex);
            fputs("\n", pArkIndex);
          }
          else
            ErrorMessageAlert("ArkWriter::LogArkIndex - Failed to generate MD5 digest");
        }
        else
          ErrorMessageAlert("ArkWriter::LogArkIndex - Error reading ArkArchive");

        RKDELETEARRAY(pArkBuf);
        fclose(pArkArchive);
      }
      else
        ErrorMessageAlert("ArkWriter::LogArkIndex - Failed to open ArkArchive");

      // Write total number of files
      fputs("NumFiles:              ", pArkIndex);
      sprintf(pBuf, "%d", m_ArkIndex.size());
      fputs(pBuf, pArkIndex);
      fputs("\n", pArkIndex);
      
      // Write number of uncompressed files
      fputs("NumUncompressedFiles:  ", pArkIndex);
      sprintf(pBuf, "%d", m_iUncompressedFileCount);
      fputs(pBuf, pArkIndex);
      fputs("\n", pArkIndex);

      // Write Ark file size
      fputs("Ark file size:         ", pArkIndex);
      sprintf(pBuf, "%d", m_iCurrentArkSize);
      fputs(pBuf, pArkIndex);
      fputs("\n\n", pArkIndex);

      for (uint32 i = 0; i < m_ArkIndex.size(); ++i)
      {
        // Write filename
        fputs("Filename:              ", pArkIndex);
        fputs(m_ArkIndex[i]->pFilename, pArkIndex);
        fputs("\n", pArkIndex);
        // Write subdir
        fputs("Subdir:                ", pArkIndex);
        fputs(m_ArkIndex[i]->pSubdir, pArkIndex);
        fputs("\n", pArkIndex);
        // Write offset in ArkArchive
        fputs("Offset in ArkArchive:  ", pArkIndex);
        sprintf(pBuf, "%d", m_ArkIndex[i]->iArkOffset);
        fputs(pBuf, pArkIndex);
        fputs("\n", pArkIndex);
        // Write source/uncompressed filesize
        fputs("Uncompressed filesize: ", pArkIndex);
        sprintf(pBuf, "%d", m_ArkIndex[i]->iSrcFileSize);
        fputs(pBuf, pArkIndex);
        fputs("\n", pArkIndex);
        // Write compressed filesize
        fputs("Compressed filesize:   ", pArkIndex);
        sprintf(pBuf, "%d", m_ArkIndex[i]->iComprFileSize);
        fputs(pBuf, pArkIndex);
        fputs("\n", pArkIndex);
        // Write timestamp
        fputs("Timestamp:             ", pArkIndex);
        sprintf(pBuf, "%d", m_ArkIndex[i]->iTimestamp);
        fputs(pBuf, pArkIndex);
        fputs("\n", pArkIndex);
        // Write extract flag
        fputs("Extract:               ", pArkIndex);
        if (m_ArkIndex[i]->IsFlagSet(AFF_EXTRACT))
          fputs("true", pArkIndex);
        else
          fputs("false", pArkIndex);
        fputs("\n", pArkIndex);
        // Write decrypt flag
        fputs("Decrypt:               ", pArkIndex);
        if (m_ArkIndex[i]->iEncrFileSize)
          fputs("true", pArkIndex);
        else
          fputs("false", pArkIndex);
        fputs("\n", pArkIndex);

        fputs("\n", pArkIndex);
      }

      RKDELETEARRAY(pBuf);
      pBuf = 0;

      fclose(pArkIndex);
    }
  }
}

/**
* LogArkIndexCSV
* Logs ArkIndex to file in CSV format for debugging purposes
* Liam de Koster-Kjaer
**/
void ArkWriter::LogArkFileMetadata()
{
  if (m_ArkIndex.size() > 0)
  {
    // Create meaningful filename for logged ArkIndex. Don't judge my ugly code! xD
    char pFilename[MAXPATH];
    memset(pFilename, 0, sizeof(pFilename));  // So we don't have to null terminate 
    strcpy(pFilename, m_ArkFilename);

    char* pExt = strrchr(pFilename, '.');
    if (!pExt)
    {
      RKASSERT(pExt, "ArkWriter::LogArkIndex - Invalid ArkFilename, failed to log ArkIndex");
      return;
    }

    const char* pFilenameSuffix = "_Metadata.txt";
    const uint32 iLen = strlen(pFilenameSuffix);
    for (uint32 i = 0; i < iLen; ++i)
      pExt[i] = pFilenameSuffix[i];

    FILE* pArkIndex = fopen(pFilename, "w");
    if (pArkIndex)
    {
      char* pBuf = RKNEWARRAY(char, MAXSTRING);
      memset(pBuf, 0, MAXSTRING);

      uint32 iLen = 0;
      char* pIt = 0;

      for (uint32 i = 0; i < m_ArkIndex.size(); ++i)
      {
        // Write filename
        fputs(m_ArkIndex[i]->pFilename, pArkIndex);
        fputs(", ", pArkIndex);
        // Write extension
        iLen = strlen(m_ArkIndex[i]->pFilename);
        if (iLen)
        {
          pIt = &m_ArkIndex[i]->pFilename[iLen-1];
          while(*pIt != '.' && pIt != m_ArkIndex[i]->pFilename)
            --pIt;
          fputs(pIt, pArkIndex);
          fputs(", ", pArkIndex);
        }
        else
        {
          fputs("ext error", pArkIndex);
          fputs(", ", pArkIndex);
        }
        // Write subdir
        fputs(m_ArkIndex[i]->pSubdir, pArkIndex);
        fputs(", ", pArkIndex);
        // Write source/uncompressed filesize
        sprintf(pBuf, "%d", m_ArkIndex[i]->iSrcFileSize);
        fputs(pBuf, pArkIndex);
        fputs(", ", pArkIndex);
        // Write compressed filesize
        sprintf(pBuf, "%d", m_ArkIndex[i]->iComprFileSize);
        fputs(pBuf, pArkIndex);
        fputs(", ", pArkIndex);
        // Write if compressed or not
        if (m_ArkIndex[i]->iSrcFileSize == m_ArkIndex[i]->iComprFileSize)
          fputs("!compressed, ", pArkIndex);
        else
          fputs("compressed, ", pArkIndex);
        // Write extract flag
        if (m_ArkIndex[i]->IsFlagSet(AFF_EXTRACT))
          fputs("extract", pArkIndex);
        else
          fputs("!extract", pArkIndex);
        fputs(", ", pArkIndex);
        // Write decrypt flag
        if (m_ArkIndex[i]->iEncrFileSize)
          fputs("decrypt", pArkIndex);
        else
          fputs("!decrypt", pArkIndex);
        fputs("\n", pArkIndex);
      }

      RKDELETEARRAY(pBuf);
      pBuf = 0;

      fclose(pArkIndex);
    }
  }
}

#endif //#if CC_USE_ARK==1
