#include "WinRTStringUtils.h"
#include "Windows.h"

std::wstring stows(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t *buffer = new wchar_t[size_needed];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, size_needed);
    std::wstring wstrTo(buffer);
	delete []buffer;
    return wstrTo;
}

std::string wstos(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char *buffer = new char[size_needed];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer, size_needed, NULL, NULL);
    std::string strTo(buffer);
	delete []buffer;
    return strTo;
}

Platform::String ^stops(std::string s)
{
	std::wstring wText = stows(s.c_str());
	return ref new Platform::String(wText.c_str());
}

std::wstring pstows(Platform::String^ ps)
{
	std::wstring wst(ps->Data());
	return wst;
}

std::string pstos(Platform::String^ ps)
{
	std::wstring wst(ps->Data());
	std::string stRet(wst.begin(), wst.end());
	return stRet;
}

Platform::String ^atops(const char *text)
{	
	std::wstring wText = stows(text);
	return ref new Platform::String(wText.c_str());
}

void pstoa(Platform::String^ ps , char* a_szOutPut)
{
	std::string stdString = pstos( ps);
	const char* szString = stdString.c_str();
	strcpy( a_szOutPut ,szString );
}

char * SOCK_API_STRNEW(const char * string)
{
	if (string == NULL)
	{
		return NULL;
	}
	char * data = NULL;
	int len = strlen(string);

	data = new char[len + 1];
	memset(data, 0, len + 1);
	memcpy(data, string, len);
	return data;
}