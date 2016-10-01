//***********************************************************************************************************************************
//*
//* RKStdSAFE.h
//*
//* a multi platform alternative to standard unsafe functions, like sprintf, strcpy, etc ..
//*
//* [Christophe Le Bouil]
//*
//***********************************************************************************************************************************

#ifndef _RK_STD_SAFE_H_
#define _RK_STD_SAFE_H_

#include <string.h>		//to avoid to re-include string.h AFTER RKStdSAFE.h !
#include <stdarg.h>     // necessary for defining " va_list "
namespace RKUtils
{
#if 1   //[TEMP ?], do not forbid std unsafe functions ..  
#define ALLOW_STD_PRINTF
#endif


#ifndef ALLOW_STD_PRINTF

#define _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_IS_HERE_THIS_DEFINE_IS_JUST_FOR_NAVIGATION_WITH_F12
	//see #define _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_   _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_IS_HERE

#ifdef _STRSAFE_H_INCLUDED_  //in MSDK_DirectShow\Include\strsafe.h
#ifndef DEPRECATE_SUPPORTED
#define OLD_STR_FUNC_REDEFINED
#endif
#endif

#ifndef OLD_STR_FUNC_REDEFINED
	//forbid usage of std unsafe functions ... :
#ifdef sprintf
#include "yop+++09"
#undef sprintf
#endif

#define sprintf _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_1

#define strcpy _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_2

#define strncpy _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_3

#define strcat _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_4

#define strncat _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_5

#ifdef _tcscat
#undef _tcscat
#define _tcscat		_NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_6
#endif

#if MLTARGET!=MLTARGET_PSP
#define vsprintf _NO_Please_Use_xxxx_SAFE_eg_vsnprintf_SAFE_or_better_MLvsprintf_TODO_7
#endif

#endif //OLD_STR_FUNC_REDEFINED


#ifdef _UNICODE
#define _tcscat_SAFE     wcscat_SAFE		//wcscat
#define _tcscpy_SAFE     wcscpy_SAFE		//wcscpy
#else	//_UNICODE
#define _tcscat_SAFE     strcat_SAFE
#define _tcscpy_SAFE     strcpy_SAFE
#endif	//_UNICODE

#endif //ALLOW_STD_PRINTF

	//----- some "standard" style SAFE text functions ----  see _NO_Please_Use_xxxx_SAFE_eg_sprintf_SAFE_TODO_IS_HERE
	//TODO ++ wcsncpy  etc ..

	int sprintf_SAFE(char *destBuffer, int sizeofDestBuffer, const char *format, ...);
	int sprintfcat_SAFE(char *destBuffer, int sizeofDestBuffer, const char *format, ...);

	char *strncpy_SAFE(char *destBuffer, int sizeofDestBuffer, const char *source, size_t numBytesToCopy);

	char *strcpy_SAFE(char *destBuffer, int sizeofDestBuffer, const char *source);

	char *strcat_SAFE(char *destBuffer, int sizeofDestBuffer, const char *source);

	int vsnprintf_SAFE(char *buf, int nbytes, const char *fmt, va_list args);

	//*** helper _AUTO macros, when dest is an array ...  warning ...
	//#define sprintf_AUTO sprintf_SAFE
#define strcpy_AUTO(dest, src) strcpy_SAFE(dest, sizeof(dest), src)
#define strcat_AUTO(dest, src) strcat_SAFE(dest, sizeof(dest), src)
#define strncpy_AUTO(dest, src, n) strncpy_SAFE(dest, sizeof(dest), src, n)

	//#if _MSC_VER >= 1500	//VC 2008"
	//mmm should return the buffer !!#define itoa_SAFE(val, dest, sizeofDestBuffer, radix) _itoa_s(val, dest, sizeofDestBuffer, radix)
	//char *itoa_SAFE(int val, char *dest, int sizeOfDest, int radix);
	//#define itoa_SAFE(val, dest, sizeofDestBuffer, radix) _itoa_s(val, dest, sizeofDestBuffer, radix)
	//#else
	//	#define itoa_SAFE(val, dest, sizeofDestBuffer, radix) itoa(val, dest, radix)		//TODO !
	//#endif

#ifdef _UNICODE
	_CRTIMP wchar_t * __cdecl wcscat_SAFE(wchar_t *destBuffer, int sizeofDestBufferInBytes, const wchar_t *source);		//sizeofDestBufferInBytes : ex u16 myBuffer[10]  --> sizeof(myBuffer) = 20 bytes
	_CRTIMP wchar_t * __cdecl wcscpy_SAFE(wchar_t *destBuffer, int sizeofDestBufferInBytes, const wchar_t *);
#endif //#ifdef _UNICODE


#ifdef  __cplusplus
	extern "C" {
#endif
		//---- if you really need the real functions, use this ! : if you REALLY REALLY know what you do !!  BAD BAD
		int sprintf_REALLY(char *destBuffer, const char *format, ...);

		char *strncpy_REALLY(char *destBuffer, const char *source, size_t numBytesToCopy);

		char *strcpy_REALLY(char *destBuffer, const char *source);

		char *strcat_REALLY(char *destBuffer, const char *source);

#ifdef  __cplusplus
	}
#endif

}
#endif //_RK_STD_SAFE_H_
