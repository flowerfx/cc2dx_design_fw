#include "RKStringFunctions.h"
#include "RKString.h"
#include "../Common/RKStdSAFE.h"
#include "../Common/RKMacros.h"
namespace RKUtils
{
	///---------------------------------------------------------------------
	/// SetExtension
	///---------------------------------------------------------------------

	void RKString_SetExtension(RKString& str, const char* extension)
	{
		int dotIdx = str.FindLast(".");
		if ((dotIdx < 0) || (str.FindLast("/") > dotIdx) || (str.FindLast("\\") > dotIdx))
		{
			dotIdx = str.Length();
		}

		if (extension != NULL)
		{
			if (dotIdx == str.Length())
			{
				str.Append(".");
			}
			str.ReplaceRegion(dotIdx + 1, str.Length() - dotIdx - 1, extension);
		}
		else
		{
			str.ReplaceRegion(dotIdx, str.Length() - dotIdx, "");
		}
	}

	///----------------------------------------------------------------------
	/// RKString_Length
	///----------------------------------------------------------------------
	unsigned int RKString_Length(const char* pStr)
	{
		unsigned int length = 0;
		while (*pStr++)
		{
			++length;
		}
		return length;
	}

	///-------------------------------------------------------------------------------
	/// ToLowerCase
	///-------------------------------------------------------------------------------
	char RKString_ToLowerCase(char c)
	{
		if (c >= (char)'A' && c <= (char)'Z')
		{
			c += (char)'a' - (char)'A';
		}
		return c;
	}

	///-------------------------------------------------------------------------------
	/// ToUpperCase
	///-------------------------------------------------------------------------------
	char RKString_ToUpperCase(char c)
	{
		if (c >= (char)'a' && c <= (char)'z')
		{
			c -= (char)'a' - (char)'A';
		}
		return c;
	}

	///-------------------------------------------------------------------------------
	/// return true if it is an alpha numeric character
	///-------------------------------------------------------------------------------
	bool RKString_IsAlphaNumeric(char c)
	{
		if ((c >= (char)'A' && c <= (char)'Z')
			|| (c >= (char)'a' && c <= (char)'z')
			|| (c >= (char)'0' && c <= (char)'9')
			)
		{
			return true;
		}
		return false;
	}


	///-------------------------------------------------------------------------------
	/// RKString_Copy
	///-------------------------------------------------------------------------------
	void RKString_Copy(char* pDest, int sizeOfDest, const char* pSrc)
	{
		RKASSERT(pDest, "RKString_Copy - pDest is null");
		RKASSERT(pSrc, "RKString_Copy - pSrc is null");

		if (pSrc)
		{
			strcpy_SAFE(pDest, sizeOfDest, pSrc);
		}
		else
		{
			if (pDest)
				pDest = (char *)'\0';
		}
	}

	///-------------------------------------------------------------------------------
	/// RKString_CopyN
	///-------------------------------------------------------------------------------
	void RKString_CopyN(char* pDest, int sizeOfDest, const char* pSrc, const unsigned int lengthToCopy)
	{
		unsigned int length = RKMIN(RKString_Length(pSrc), lengthToCopy);
		if ((int)length >= sizeOfDest)
			length = sizeOfDest - 1;
		for (unsigned int i = 0; i < length; ++i)
		{
			pDest[i] = pSrc[i];
		}
		pDest[length] = 0;
	}

	///-------------------------------------------------------------------------------
	/// RKString_Compare
	///-------------------------------------------------------------------------------
	int RKString_Compare(const char* pString1, const char* pString2)
	{
		unsigned int length = RKString_Length(pString1);
		for (unsigned int i = 0; i <= length; ++i)
		{
			if (pString1[i] != pString2[i])
			{
				return pString1[i] < pString2[i] ? -1 : 1;
			}
		}
		return 0;
	}

	///-------------------------------------------------------------------------------
	/// RKString_ReverseInPlace
	///-------------------------------------------------------------------------------
	int RKString_ReverseInPlace(char* pStr)
	{
		int len = RKString_Length(pStr);
		if (len)
		{
			char temp;

			for (int i = 0; i < len / 2; ++i)
			{
				temp = pStr[len - i - 1];
				pStr[len - i - 1] = pStr[i];
				pStr[i] = temp;
			}
		}

		return len;
	}

	///-------------------------------------------------------------------------------
	/// RKString_Compare
	///-------------------------------------------------------------------------------
	int RKString_ICompare(const char* pString1, const char* pString2)
	{
		char c1, c2;
		uint32 length = RKString_Length(pString1);
		for (uint32 i = 0; i <= length; ++i)
		{
			c1 = RKString_ToLowerCase(pString1[i]);
			c2 = RKString_ToLowerCase(pString2[i]);
			if (c1 != c2)
			{
				return c1 < c2 ? -1 : 1;
			}
		}
		return 0;
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractFilePath
	///----------------------------------------------------------------------
	void RKString_ExtractFilePath(const char* pPath, char* pDest, int sizeOfDest)
	{
		if (sizeOfDest <= 0)
		{
			RKAssert(0, "RKString_ExtractFilePath(), sizeOfDest=%d!", sizeOfDest);
			return;
		}
		*pDest = 0;

		int srcLen = strlen(pPath);
		if (srcLen <= 0)
		{
			RKWarning(0, "RKString_ExtractFilePath(), srcLen=%d!", srcLen);
			return;
		}
		const char* pSrc = pPath + srcLen - 1;

		// back up until a \ or the start
		while (pSrc > pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}

		int lenToCopy = pSrc - pPath;
		if (lenToCopy >= sizeOfDest)
		{
			RKWarning(0, "RKString_ExtractFilePath(): dest TOO SMALL (%d) --> truncated!", sizeOfDest);
			lenToCopy = sizeOfDest - 1;
		}
		memcpy(pDest, pPath, lenToCopy);
		pDest[lenToCopy] = 0;
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractLastSubdirectory
	///----------------------------------------------------------------------
	void RKString_ExtractLastSubdirectory(const char* pPath, char* pDest, int sizeOfDest)
	{
		if (sizeOfDest <= 0)
		{
			RKAssert(0, "RKString_ExtractFileName(), sizeOfDest=%d!", sizeOfDest);
			return;
		}
		*pDest = 0;

		int srcLen = strlen(pPath);
		if (srcLen <= 0)
		{
			RKWarning(0, "RKString_ExtractLastSubdirectory(), srcLen=%d!", srcLen);
			return;
		}
		const char* pSrc = pPath + srcLen - 1;

		// back up until a \ or the start
		while (pSrc > pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}


		// if we didnt hit the beginning of the string
		if (pSrc != pPath)
		{
			pSrc--; // remove last \

			const char* pSrc2 = pSrc;

			while (pSrc2 != pPath && *(pSrc2 - 1) != '/' && *(pSrc2 - 1) != '\\')
			{
				pSrc2--;
			}

			if (pSrc - pSrc2 < sizeOfDest)
			{
				memcpy(pDest, pSrc2, pSrc - pSrc2);
				pDest[pSrc - pSrc2] = 0;
			}
			else
			{
				RKWarning(0, "RKString_ExtractLastSubdirectory(), dest TOO SMALL! (%d) -> truncated!", sizeOfDest);
				memcpy(pDest, pSrc2, sizeOfDest - 1);
				pDest[sizeOfDest - 1] = 0;
			}
		}
		else
		{
			// return a blank path
			memset(pDest, 0, sizeOfDest);
		}
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractFileName
	///----------------------------------------------------------------------
	void RKString_ExtractFileName(const char* pPath, char* pDest0, int sizeOfDest)
	{
		if (sizeOfDest <= 0)
		{
			RKAssert(0, "RKString_ExtractFileName(), sizeOfDest=%d!", sizeOfDest);
			return;
		}
		*pDest0 = 0;

		int srcLen = strlen(pPath);
		if (srcLen <= 0)
		{
			RKWarning(0, "RKString_ExtractFileName(), srcLen=%d!", srcLen);
			return;
		}
		const char* pSrc = pPath + srcLen - 1;

		// back up until a \ or the start
		while (pSrc > pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}

		char* pDest = pDest0;
		while (*pSrc)
		{
			if ((pDest - pDest0) >= sizeOfDest - 1)
			{
				RKWarning(0, "RKString_ExtractFileName() dest buffer TOO SMALL (%d)!  srcLen=%d !!", sizeOfDest, srcLen);
				*(pDest0 + sizeOfDest - 1) = 0;
				return;
			}
			*pDest++ = *pSrc++;
		}

		*pDest = 0;
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractFileBase
	///----------------------------------------------------------------------
	void RKString_ExtractFileBase(const char* pPath, char* pDest0, int sizeOfDest)
	{
		if (sizeOfDest <= 0)
		{
			RKAssert(0, "RKString_ExtractFileBase(), sizeOfDest=%d!", sizeOfDest);
			return;
		}
		*pDest0 = 0;

		int srcLen = strlen(pPath);
		if (srcLen <= 0)
		{
			RKWarning(0, "RKString_ExtractFileBase(), srcLen=%d!", srcLen);
			return;
		}
		const char* pSrc = pPath + srcLen - 1;

		// back up until a \ or the start
		while (pSrc > pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}

		char* pDest = pDest0;
		while (*pSrc && *pSrc != '.')
		{
			if ((pDest - pDest0) >= sizeOfDest - 1)
			{
				RKWarning(0, "RKString_ExtractFileBase() dest buffer TOO SMALL (%d)!  srcLen=%d !!", sizeOfDest, srcLen);
				*(pDest0 + sizeOfDest - 1) = 0;
				return;
			}
			*pDest++ = *pSrc++;
		}

		*pDest = 0;
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractFileExtension
	///----------------------------------------------------------------------
	void RKString_ExtractFileExtension(const char *pPath, char *pDest, int sizeOfDest)
	{
		if (sizeOfDest <= 0)
		{
			RKAssert(0, "RKString_ExtractFileBase(), sizeOfDest=%d!", sizeOfDest);
			return;
		}
		*pDest = 0;

		int srcLen = strlen(pPath);
		if (srcLen <= 0)
		{
			RKWarning(0, "RKString_ExtractFileBase(), srcLen=%d!", srcLen);
			return;
		}
		const char* pSrc = pPath + srcLen - 1;

		// back up until a . or the start
		while (pSrc > pPath && *(pSrc - 1) != '.')
		{
			pSrc--;
		}

		if (pSrc == pPath)
		{
			*pDest = 0;	// no extension
			return;
		}

		strcpy_SAFE(pDest, sizeOfDest, pSrc);
	}


	///----------------------------------------------------------------------
	/// RKString_ConvertDOSToUnixName
	///----------------------------------------------------------------------
	void RKString_ConvertDOSToUnixName(char* pDst0, int sizeOfDest, const char* pSrc)
	{
		char* pDst = pDst0;

		while (*pSrc)
		{
			if (*pSrc == '\\')
			{
				*pDst = '/';
			}
			else
			{
				*pDst = *pSrc;
			}

			if (pDst - pDst0 >= sizeOfDest - 1)
			{
				RKWarning(0, "BAD, RKString_ConvertDOSToUnixName(), dest too small (%d) -> truncated!", sizeOfDest);
				*pDst = 0;
				return;
			}

			pDst++;
			pSrc++;
		}

		*pDst = 0;
	}


	///----------------------------------------------------------------------
	/// RKString_CreateHash
	///----------------------------------------------------------------------
	int RKString_CreateHash(const char* pSrc)
	{
		unsigned hash = 2166136261U;

		while (*pSrc)
			hash = 16777619U * (hash ^ (unsigned)*pSrc++);

		return int(hash % INT_MAX);
	}











	//******************** TEMP copy of unsafe versions ******************
#if 1 //temp 1, while commiting ..
	///----------------------------------------------------------------------
	/// RKString_ExtractFileName  - UNSAFE
	///----------------------------------------------------------------------
	void RKString_ExtractFileName(const char* pPath, char* pDest)
	{
		const char* pSrc = pPath + strlen(pPath) - 1;

		// back up until a \ or the start
		while (pSrc != pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}

		while (*pSrc)
		{
			*pDest++ = *pSrc++;
		}

		*pDest = 0;
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractFileBase  - UNSAFE
	///----------------------------------------------------------------------
	void RKString_ExtractFileBase(const char* pPath, char* pDest)
	{
		const char* pSrc = pPath + strlen(pPath) - 1;

		// back up until a \ or the start
		while (pSrc != pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}

		while (*pSrc && *pSrc != '.')
		{
			*pDest++ = *pSrc++;
		}

		*pDest = 0;
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractFilePath  - UNSAFE
	///----------------------------------------------------------------------
	void RKString_ExtractFilePath(const char* pPath, char* pDest)
	{
		const char* pSrc = pPath + strlen(pPath) - 1;

		// back up until a \ or the start
		while (pSrc != pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}

		memcpy(pDest, pPath, pSrc - pPath);
		pDest[pSrc - pPath] = 0;
	}

	///----------------------------------------------------------------------
	/// RKString_ExtractLastSubdirectory  - UNSAFE
	///----------------------------------------------------------------------
	void RKString_ExtractLastSubdirectory(const char* pPath, char* pDest)
	{
		const char* pSrc = pPath + strlen(pPath) - 1;

		// back up until a \ or the start
		while (pSrc != pPath && *(pSrc - 1) != '/' && *(pSrc - 1) != '\\')
		{
			pSrc--;
		}


		// if we didnt hit the beginning of the string
		if (pSrc != pPath)
		{
			pSrc--; // remove last \

			const char* pSrc2 = pSrc;

			while (pSrc2 != pPath && *(pSrc2 - 1) != '/' && *(pSrc2 - 1) != '\\')
			{
				pSrc2--;
			}

			memcpy(pDest, pSrc2, pSrc - pSrc2);
			pDest[pSrc - pSrc2] = 0;
		}
		else
		{
			// return a blank path
			memset(pDest, 0, pPath - pSrc);
		}
	}


	///----------------------------------------------------------------------
	/// RKString_ExtractFileExtension  - UNSAFE
	///----------------------------------------------------------------------
	void RKString_ExtractFileExtension(const char *pPath, char *pDest)
	{
		const char *pSrc = pPath + strlen(pPath) - 1;

		// back up until a . or the start
		while (pSrc != pPath && *(pSrc - 1) != '.')
		{
			pSrc--;
		}

		if (pSrc == pPath)
		{
			*pDest = 0;	// no extension
			return;
		}

		strcpy(pDest, pSrc);
	}

	///----------------------------------------------------------------------
	/// RKString_ConvertDOSToUnixName  - UNSAFE
	///----------------------------------------------------------------------
	void RKString_ConvertDOSToUnixName(char* pDst, const char* pSrc)
	{
		while (*pSrc)
		{
			if (*pSrc == '\\')
			{
				*pDst = '/';
			}
			else
			{
				*pDst = *pSrc;
			}

			pDst++;
			pSrc++;
		}

		*pDst = 0;
	}

	///-------------------------------------------------------------------------------
	/// RKString_Copy   - UNSAFE
	///-------------------------------------------------------------------------------
	void RKString_Copy(char* pDest, const char* pSrc)
	{
		RKASSERT(pDest, "RKString_Copy - pDest is null");
		RKASSERT(pSrc, "RKString_Copy - pSrc is null");

		if (pSrc)
		{
			strcpy(pDest, pSrc);
		}
		else
		{
			if (pDest)
				pDest = (char *)'\0';
		}
	}

	///-------------------------------------------------------------------------------
	/// RKString_CopyN  - UNSAFE
	///-------------------------------------------------------------------------------
	void RKString_CopyN(char* pDest, const char* pSrc, const unsigned int lengthToCopy)
	{
		unsigned int length = RKMIN(RKString_Length(pSrc), lengthToCopy);
		for (unsigned int i = 0; i < length; ++i)
		{
			pDest[i] = pSrc[i];
		}
		pDest[length] = 0;
	}
}

#endif //#if 1/0  - temp unsafe

