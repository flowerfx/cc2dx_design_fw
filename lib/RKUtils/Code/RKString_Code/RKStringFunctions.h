#ifndef __RKSTRING_FUNCTIONS_H_
#define __RKSTRING_FUNCTIONS_H_
namespace RKUtils
{
	class RKString;

	// BUGBUG: [mark.gere] Remove this once string pooling is implemented.
#define RKSTRING_LENGTH_TEMP 260

	unsigned int RKString_Length(const char* pStr);
	char RKString_ToLowerCase(char c);
	char RKString_ToUpperCase(char c);
	bool RKString_IsAlphaNumeric(char c);
	void RKString_Copy(char* pDest, int sizeOfDest, const char* pSrc);
	void RKString_CopyN(char* pDest, int sizeOfDest, const char* pSrc, const unsigned int lengthToCopy);
	int RKString_Compare(const char* pString1, const char* pString2);
	int RKString_ICompare(const char* pString1, const char* pString2);

	void RKString_ExtractFilePath(const char* pPath, char* pDest, int sizeOfDest);
	void RKString_ExtractFileName(const char* pPath, char* pDest, int sizeOfDest);
	void RKString_ExtractFileBase(const char* pPath, char* pDest, int sizeOfDest);
	void RKString_ExtractFileExtension(const char *pPath, char *pDest, int sizeOfDest);
	void RKString_ExtractLastSubdirectory(const char* pPath, char* pDest, int sizeOfDest);
	void RKString_ConvertDOSToUnixName(char* pDst, int sizeOfDest, const char* pSrc);
	int RKString_CreateHash(const char* pSrc);

	// pass extension without dot, you can pass NULL to remove extension completely
	void RKString_SetExtension(RKString& str, const char* extension);

	//>>> UNSAFE versions .. WILL BE REMOVED ..
#if 1 //temp, while commiting ..
	void RKString_ExtractFilePath(const char* pPath, char* pDest);
	void RKString_ExtractFileName(const char* pPath, char* pDest);
	void RKString_ExtractFileBase(const char* pPath, char* pDest);
	void RKString_ExtractFileExtension(const char *pPath, char *pDest);
	void RKString_ExtractLastSubdirectory(const char* pPath, char* pDest);
	void RKString_ConvertDOSToUnixName(char* pDst, const char* pSrc);

	void RKString_Copy(char* pDest, const char* pSrc);
	void RKString_CopyN(char* pDest, const char* pSrc, const unsigned int lengthToCopy);
#endif //temp
}
#endif // __RKSTRING_FUNCTIONS_H_
