#ifndef __CASUALCORE_ENCRYPTION_H__
#define __CASUALCORE_ENCRYPTION_H__

#include <stdlib.h>
#include <string>

#define USE_MD5 0
#define USE_BLOB 0

namespace RKUtils
{
	class Codec
	{
	public:
        static bool DecryptTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key);

        static bool EncryptXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key);

        static bool DecryptXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key);

//#define XXTEA_MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

        static bool EncryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key);

        static bool DecryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key);

        static size_t GetEncryptedTEADataSize(size_t inDataSize);

        static size_t GetEncryptedXTEADataSize(size_t inDataSize);

        static size_t GetEncryptedXXTEADataSize(size_t inDataSize);

	};

	class Encryption
	{
	public:
		/** Encrypt data according to XXTEA
		*
		* The input buffer and output buffer can be the same, but input data will be overwriten.
		* \param indata Data to encrypt
		* \param indataSize Size of the data to encrypt
		* \param outData Buffer to write the encrypted data to <b>(must be of size indataSize round up to a multiple of 4)</b>
		* \param outdataSize Size of the output buffer
		* \param key 128 bit encryption key (4 x unsigned int)
		* \return True if encryption is successful
		*/
		static bool EncryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key);

		/** Decrypt data according to XXTEA
		*
		* The input buffer and output buffer can be the same, but input data will be overwriten.
		* \param indata Data to decrypt
		* \param indataSize Size of the data to decrypt
		* \param outData Buffer to write the decrypted data to <b>(must be of size indataSize or more)</b>
		* \param outdataSize Size of the output buffer
		* \param key 128 bit encryption key (4 x unsigned int)
		* \return True if decryption is successful
		*/
		static bool DecryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key);

		/** Get the encrypted size of the data using XXTEA
		*
		* \param inDataSize Size of the data to encrypt
		* \return Size of the encrypted Data
		*/
		static size_t GetEncryptedXXTEADataSize(size_t inDataSize);
#if USE_BLOB
		/** Encrypt a block of data in a blob string
		*
		* \param src Block of data to encode
		* \param dataSize Size of the block of data to encode
		* \param dest Reference to append the encoded string to
		* \return True if the data was encoded successfully
		*/
		static bool EncryptBlob(const unsigned char* src, const unsigned int dataSize, std::string& dest);
		static bool EncryptBlob(const std::string &src, const unsigned int dataSize, std::string& dest);
		/** Decrypt a blob encoded string into a block of data
		*
		* \param blobbedData Blobbed string to decode
		* \param outData Pointer to the buffer to write to <b>(must be already allocated)</b>
		* \return Size of the decrypt data
		*/
		static unsigned int DecryptBlob(const std::string& blobbedData, void* outData);
#endif

#if USE_MD5
		/** Generate a MD5 checksum of a data buffer
		*
		* \param data Data to generate the checksum from
		* \param size Size of the data
		* \param dest A 32 bytes buffer to write the check sum (hexa string)
		* \return True if checksum was generated successfully
		*/
		static bool EncryptMD5(const void* data, const unsigned int dataSize, std::string& dest);

		/** Encode a block of data in a base64 string
		*
		* \param src Block of data to encode
		* \param dataSize Size of the block of data to encode
		* \param dest Reference to append the encoded string to
		* \return True if the data was encoded successfully
		*/
		static bool EncodeBase64(const unsigned char* src, size_t dataSize, std::string& dest);

		/** Decode a base64 encoded string into a block of data
		*
		* \param base64Data Base64 c-string to decode
		* \param dataSize Size of the string to decode
		* \param outData Pointer to the buffer to write to <b>(must be already allocated)</b>
		* \param strict If true, all character not in the alphabet for Base64, will be ignored.
		* \return Size of the decoded data
		*/
		static size_t DecodeBase64(const char* base64Data, size_t dataSize, void* outData, bool strict=false);

		static bool MD5Test();
#endif
	};

#if USE_MD5
	class EncryptMD5Progressive
	{
	public:
		EncryptMD5Progressive(std::string a_pDest);
		~EncryptMD5Progressive();
		void Update(const void* a_pBuffer, const unsigned int a_uDataSize);
		void Finalise();
	private:
		std::string m_pDest;
		glwebtools::md5_state_s* m_pMD5State;
	};
#endif
	
}
#endif // __CASUALCORE_ENCRYPTION_H__
