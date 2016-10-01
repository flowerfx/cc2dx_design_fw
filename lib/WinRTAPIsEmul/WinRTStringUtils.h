#ifndef __WP8_SOCKET_UTILS_H__
#define __WP8_SOCKET_UTILS_H__
#include <string>

#ifdef _DEBUG
#define SOCK_DEBUG_OUT(...)                     __Debug_out(__FUNCTION__ , __LINE__, __VA_ARGS__)
#else
#define SOCK_DEBUG_OUT(...)
#endif
#define SOCK_API_DEL(a)			{ if(a) { delete (a); a=NULL; } }
#define SOCK_API_DELETE_ARRAY(a)	{ if(a) { delete [] (a); a=NULL; } }
#define SOCK_API_DEL_ARRAY(p, n)	{ if(p) { for (int __i = 0; __i < n;__i++) SOCK_API_DELETE_ARRAY((p)[__i]); delete[] (p); p = NULL; } }
char * SOCK_API_STRNEW( const char * string );
void __Debug_out(const char* pFunction, int lineNumber, const char* pFormat, ...);

std::wstring stows(const std::string& str);
std::string wstos(const std::wstring& wstr);
Platform::String ^stops(std::string s);
std::string pstos(Platform::String^ ps);
Platform::String ^atops(const char *text);
void pstoa(Platform::String^ ps , char* a_szOutPut);

#endif