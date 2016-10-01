
  #include "Encryption.h"
  #include "../Common/RKAssert.h"

namespace RKUtils
{
    
    bool Codec::DecryptTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key)
    {
        if (!indata || !outData)
            return false;
        
        if (indataSize == 0)
            return false; //no data
        
        if (indataSize & 7)
            return false; //invalid data size - must be a multiple of 8
        
        if (!key)
            return false; //no key
        
        if (indataSize > outdataSize)
            return false; // out data too small
        
        int value[2]; // 64 bit value chunks
        size_t bytes = 0;
        const char* inbuf = static_cast<const char*>(indata);
        char* outbuf = static_cast<char*>(outData);
        
        while (bytes < indataSize) // done in 64 bit chunks so there must be atleast 8 bytes remaining
        {
            // read 8 bits
            memset(value, 0x0, 8);
            memcpy(static_cast<void*>(value), inbuf, 8);
            inbuf += 8;
            bytes += 8;
            
            // TEA encyption
            unsigned int v0 = value[0], v1 = value[1], sum = 0xC6EF3720, i;    /* set up */
            unsigned int delta = 0x9e3779b9;                                /* a key schedule constant */
            unsigned int k0 = key[0], k1 = key[1], k2 = key[2], k3 = key[3];    /* cache key */
            for (i = 0; i < 32; i++)                                        /* basic cycle start */
            {
                v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
                v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
                sum -= delta;
            }                                                            /* end cycle */
            value[0] = v0; value[1] = v1;
            
            memcpy(outbuf, value, 8); // copy 8 bytes into out buffer
            outbuf += 8;
        }
        
        return true;
    }
    
    bool Codec::EncryptXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key)
    {
        if (!indata || !outData)
            return false;
        
        if (indataSize == 0)
            return false; //no data
        
        if (!key)
            return false; //no key
        
        size_t encodedSize = (indataSize & 7) ? (indataSize & (~7)) + 8 : indataSize; //find next multiple of 8
        
        if (encodedSize > outdataSize)
            return false; // out data too small
        
        int value[2]; // 64 bit value chunks
        size_t bytes = 0;
        const char* inbuf = static_cast<const char*>(indata);
        char* outbuf = static_cast<char*>(outData);
        while ((bytes + 8) < indataSize) // done in 64 bit chunks so there must be atleast 8 bytes remaining
        {
            // read 8 bytes
            memset(value, 0x0, 8);
            memcpy(static_cast<void*>(value), inbuf, 8);
            inbuf += 8;
            bytes += 8;
            
            // XTEA encyption
            unsigned int v0 = value[0], v1 = value[1], sum = 0, i;            /* set up */
            unsigned int delta = 0x9e3779b9;                                /* a key schedule constant */
            unsigned int k[4];
            k[0] = key[0], k[1] = key[1], k[2] = key[2], k[3] = key[3];    /* cache key */
            for (i = 0; i < 32; i++) /* basic cycle start */
            {
                v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
                sum += delta;
                v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
            }
            /* end cycle */
            value[0] = v0; value[1] = v1;
            
            memcpy(outbuf, value, 8); // copy 8 bytes into out buffer
            outbuf += 8;
        }
        
        //encode remaining bytes
        if (bytes < encodedSize)
        {
            // read remaining bytes
            memset(value, 0x0, 8);
            memcpy(static_cast<void*>(value), inbuf, indataSize - bytes);
            
            // XTEA encyption
            unsigned int v0 = value[0], v1 = value[1], sum = 0, i;            /* set up */
            unsigned int delta = 0x9e3779b9;                                /* a key schedule constant */
            unsigned int k[4];
            k[0] = key[0], k[1] = key[1], k[2] = key[2], k[3] = key[3];    /* cache key */
            for (i = 0; i < 32; i++) /* basic cycle start */
            {
                v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
                sum += delta;
                v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
            }
            /* end cycle */
            value[0] = v0; value[1] = v1;
            
            memcpy(outbuf, value, 8); // copy 8 bytes into out buffer
        }
        
        return true;
    }
    
    bool Codec::DecryptXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key)
    {
        if (!indata || !outData)
            return false;
        
        if (indataSize == 0)
            return false; //no data
        
        if (indataSize & 7)
            return false; //invalid data size - must be a multiple of 8
        
        if (!key)
            return false; //no key
        
        if (indataSize > outdataSize)
            return false; // out data too small
        
        int value[2]; // 64 bit value chunks
        size_t bytes = 0;
        const char* inbuf = static_cast<const char*>(indata);
        char* outbuf = static_cast<char*>(outData);
        
        while (bytes < indataSize) // done in 64 bit chunks so there must be atleast 8 bytes remaining
        {
            // read 8 bits
            memset(value, 0x0, 8);
            memcpy(static_cast<void*>(value), inbuf, 8);
            inbuf += 8;
            bytes += 8;
            
            // TEA encyption
            unsigned int v0 = value[0], v1 = value[1], i;    /* set up */
            unsigned int delta = 0x9e3779b9, sum = delta << 5;                /* a key schedule constant */
            unsigned int k[4];
            k[0] = key[0], k[1] = key[1], k[2] = key[2], k[3] = key[3];    /* cache key */
            for (i = 0; i < 32; i++)
            {
                v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
                sum -= delta;
                v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
            }                                                            /* end cycle */
            value[0] = v0; value[1] = v1;
            
            memcpy(outbuf, value, 8); // copy 8 bytes into out buffer
            outbuf += 8;
        }
        
        return true;
    }
    
#define XXTEA_MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))
    
    bool Codec::EncryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key)
    {
        if (!indata || !outData)
            return false;
        
        if (indataSize < 8)
            return false; //not enough data
        
        if (!key)
            return false; //no key
        
        size_t encodedSize = (indataSize & 3) ? (indataSize & (~3)) + 4 : indataSize; //find next multiple of 4
        
        if (encodedSize > outdataSize)
            return false; // out data too small
        
        unsigned int y, z, sum;
        unsigned int p, rounds, e;
        const unsigned int DELTA = 0x9e3779b9;
        
        size_t n = encodedSize >> 2; // 32 bit words
        
        if (indata != outData)
            memcpy(outData, indata, indataSize); //algorithm work on it's own data - don't copy if output is same than input
        
        unsigned int* v = static_cast<unsigned int*>(outData);
        
        if (encodedSize != indataSize)//zero the padding
        {
            char* buf = static_cast<char*>(outData);
            memset(buf + indataSize, 0, encodedSize - indataSize);
        }
        
        /* Coding Part */
        rounds = 6 + 52 / static_cast<unsigned int>(n);
        sum = 0;
        z = v[n - 1];
        do
        {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p = 0; p < n - 1; p++)
            {
                y = v[p + 1];
                z = v[p] += XXTEA_MX;
            }
            y = v[0];
            z = v[n - 1] += XXTEA_MX;
        } while (--rounds);
        
        return true;
    }
    
    bool Codec::DecryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key)
    {
        if (!indata || !outData)
            return false;
        
        if (indataSize == 0)
            return false; //no data
        
        if (!key)
            return false; //no key
        
        if (indataSize & 3)
            return false; //invalid size
        
        if (indataSize > outdataSize)
            return false; // out data too small
        
        unsigned int y, z, sum;
        unsigned int p, rounds, e;
        const unsigned int DELTA = 0x9e3779b9;
        
        size_t n = indataSize >> 2; // 32 bit words
        
        if (indata != outData)
            memcpy(outData, indata, indataSize); //algorithm work on it's own data - don't copy if output is same than input
        
        unsigned int* v = static_cast<unsigned int*>(outData);
        
        /* Decoding Part */
        rounds = 6 + 52 / static_cast<unsigned int>(n);
        sum = rounds*DELTA;
        y = v[0];
        do
        {
            e = (sum >> 2) & 3;
            for (p = static_cast<unsigned int>(n)-1; p > 0; p--)
            {
                z = v[p - 1];
                y = v[p] -= XXTEA_MX;
            }
            z = v[n - 1];
            y = v[0] -= XXTEA_MX;
        } while ((sum -= DELTA) != 0);
        
        return true;
    }
    
    size_t Codec::GetEncryptedTEADataSize(size_t inDataSize)
    {
        return (inDataSize & 7) ? (inDataSize & (~7)) + 8 : inDataSize; //find next multiple of 8
    }
    
    size_t Codec::GetEncryptedXTEADataSize(size_t inDataSize)
    {
        return (inDataSize & 7) ? (inDataSize & (~7)) + 8 : inDataSize; //find next multiple of 8
    }
    
    size_t Codec::GetEncryptedXXTEADataSize(size_t inDataSize)
    {
        return (inDataSize & 3) ? (inDataSize & (~3)) + 4 : inDataSize; //find next multiple of 4
    }
    
    
    
  bool Encryption::EncryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key)
  {
    return Codec::EncryptXXTEA(indata, indataSize, outData, outdataSize, key);
  }

  bool Encryption::DecryptXXTEA(const void* indata, size_t indataSize, void* outData, size_t outdataSize, const unsigned int* key)
  {
    return Codec::DecryptXXTEA(indata, indataSize, outData, outdataSize, key);
  }

  size_t Encryption::GetEncryptedXXTEADataSize(size_t inDataSize)
  {
    return Codec::GetEncryptedXXTEADataSize(inDataSize);
  }
#if USE_BLOB
  bool Encryption::EncryptBlob(const unsigned char* src, const unsigned int dataSize, std::string& dest)
  {
	std::string strDest;
    if(glwebtools::Codec::EncodeBlob(src, dataSize, strDest))
    {
      dest.Assign(strDest.c_str());
      return true;
    }
    return false;
  }

  bool Encryption::EncryptBlob(const std::string& src, const unsigned int dataSize, std::string& dest)
  {
    return EncryptBlob(reinterpret_cast<const unsigned char*>(src.GetString()), dataSize, dest);
  }

  unsigned int Encryption::DecryptBlob(const std::string& blobbedData, void* outData)
  {
	  std::string inputData(blobbedData.c_str());
    return Codec::DecodeBlob(inputData, outData);
  }
#endif

#if USE_MD5
  //(const void* data, size_t size, char* dest)
  bool Encryption::EncryptMD5(const void* data, const unsigned int dataSize, std::string& dest)
  {
    /* Initialize the algorithm. */
    glwebtools::md5_state_t md5State;
    md5_init(&md5State);

    /* Append a string to the message. */
    md5_append(&md5State, static_cast<const glwebtools::md5_byte_t*>(data), static_cast<int>(dataSize));

    /* Finish the message and return the digest. */
    glwebtools::md5_byte_t digest[16];
    md5_finish(&md5State, digest);
    dest.AssignFormatted("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
      digest[0],  digest[1],  digest[2],  digest[3], 
      digest[4],  digest[5],  digest[6],  digest[7], 
      digest[8],  digest[9],  digest[10], digest[11], 
      digest[12], digest[13], digest[14], digest[15]);

    return true;
  }

  bool Encryption::EncodeBase64(const unsigned char* src, size_t dataSize, std::string& dest)
  {
	  std::string glDest = dest.c_str();
    return Codec::EncodeBase64(src, dataSize, glDest);
  }

  size_t Encryption::DecodeBase64(const char* base64Data, size_t dataSize, void* outData, bool strict)
  {
    return Codec::DecodeBase64(base64Data, dataSize, outData);
  }

  bool Encryption::MD5Test()
  {
    bool bSuccess = true;

    const uint32 iNumTests = 9;

    const char* pTestData[iNumTests] = {
      "thisisateststring!", 
      "is this a test string?", 
      "one time for your mind", 
      "how many strings could a test string test if a test string could test strings?", 
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", 
      "mad bad wang clan strikes again muahahahahaaa!", 
      "the one that says bad motherfucker", 
      "`1234567890-=[];',./~!@#$%^&*()_+{}|:<>?", 
      "bones turn to dirt, dirt turn to birth, birth turn to death, turn the earth round, making what we all call life, then life give birth to the light, give worth to my world, with the words that i write, write words on my grave, find heaven in life, ready to die like, just another regular guy, and then im gone"
    };

    // Expected results generated using md5 generator found at www.md5.net
    const char* pExpectedResult[iNumTests] = {
      "76c451c9b585af6986d10d692875cf15", 
      "e57439266a0cdb67700be1bcb74555c1", 
      "6715945ed01e707fa5fbb6bf73eb0c64", 
      "eca7c81dbcb09a11acc16c85cd037d0a", 
      "2ad372c377013baa4ee32ab6649d2449", 
      "59ec06acf762630d9096eab096ee751b", 
      "7b327631b2870854c51f04b86755fe97", 
      "c947fd6d238954a8bf1af03776f44f1d", 
      "b776b89df5ab48a55f15f869a2fa44c5"
    };

	std::string sResult;

    for(uint32 i = 0; i < iNumTests; ++i)
    {
      int32 iSize = sizeof(char) * strlen(pTestData[i]);
      if (Encryption::EncryptMD5(pTestData[i], iSize, sResult))
      {
        if (sResult != pExpectedResult[i])
        {
          const char* pTest = sResult.GetString();
          RKASSERT(false, "MD5 test failed - MD5 digest mismatch");
          bSuccess = false;
        }
      }
      else
      {
        RKASSERT(false, "MD5 test failed - EncryptMD5() returned false");
        bSuccess = false;
      }

      sResult.Clear();
    }

    return bSuccess;
  }
  
  EncryptMD5Progressive::EncryptMD5Progressive(std::string a_pDest)
  : m_pDest(a_pDest)
  , m_pMD5State(NULL)
  {
    // Initialize the algorithm.
    if(!m_pMD5State)
    {
      m_pMD5State = new glwebtools::md5_state_s();
    }
    md5_init(m_pMD5State);
  }
  
  EncryptMD5Progressive::~EncryptMD5Progressive()
  {
    if(m_pMD5State)
    {
      delete m_pMD5State;
      m_pMD5State = NULL;
    }
  }
  
  void EncryptMD5Progressive::Update(const void* a_pBuffer, const unsigned int a_uDataSize)
  {
    
    /* Append a string to the message. */
    md5_append(m_pMD5State, static_cast<const glwebtools::md5_byte_t*>(a_pBuffer), static_cast<int>(a_uDataSize));
  }
  
  void EncryptMD5Progressive::Finalise()
  {
    /* Finish the message and return the digest. */
    glwebtools::md5_byte_t digest[16];
    md5_finish(m_pMD5State, digest);
    m_pDest->AssignFormatted("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                         digest[0],  digest[1],  digest[2],  digest[3],
                         digest[4],  digest[5],  digest[6],  digest[7],
                         digest[8],  digest[9],  digest[10], digest[11],
                         digest[12], digest[13], digest[14], digest[15]);
  }
#endif
}

