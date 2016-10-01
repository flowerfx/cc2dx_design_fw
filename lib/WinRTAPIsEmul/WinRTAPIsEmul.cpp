#include "pch.h"
#include <sys/stat.h>
#include "WinRTAPIsEmul.h"
#include "WinRTStringUtils.h"
#include "ThreadEmulation.h"
#include <thread>

int g_AppReturnValue = 0;//fix glf compiler error
// XuyenNT add * Bug Fixed ['6916448']: '[UD2][W8][Tracking][51806]: Some parameters always return value as 0
#if defined(OS_W8) || defined(OS_WP8) || defined OS_W10
//extern std::string strDocumentPath = pstos(Windows::Storage::ApplicationData::Current->LocalFolder->Path);
#endif

// end
void SleepEmul(DWORD dwMilliseconds)
{
	ThreadEmulation::Sleep(dwMilliseconds);
}

void YieldEmul()
{
	ThreadEmulation::Sleep(1);
}

static std::string sLocalTempPath = "";

void GetTempPathEmul(int path_size, char* path)
{	
	memset(path, 0, path_size);
	if (sLocalTempPath.empty())
	{
		sLocalTempPath = pstos(Windows::Storage::ApplicationData::Current->LocalFolder->Path) +"\\tmp";
	}
	strncpy(path, sLocalTempPath.c_str(), path_size);
}

static std::string sInstalledFolderPath = "";
char* _getcwdEmul(char* path, int path_size)
{
	memset(path, 0, path_size);
	if (sInstalledFolderPath.empty())
	{
		sInstalledFolderPath = pstos(Windows::ApplicationModel::Package::Current->InstalledLocation->Path);
	}
	strncpy(path, sInstalledFolderPath.c_str(), path_size);
	return path;
}

bool IsExistingDirectory(const char* path)
{
	WIN32_FILE_ATTRIBUTE_DATA attrData;
	std::string strPath(path);
	std::wstring wstrPath(strPath.begin(), strPath.end());
	if (GetFileAttributesExW(wstrPath.c_str(), GetFileExInfoStandard, &attrData))
		return true;
	return false;
}

int _chdirEmul(const char* path)
{
	std::string temp;
	if (path != NULL && strlen(path) > 1)
	{
		if (path[1] != ':')
			temp = sInstalledFolderPath + "\\" + path;
		else
			temp = path;

		int ch_pos = std::string::npos;
		do{
			ch_pos = temp.find("..");
			if (ch_pos != std::string::npos)
			{
				std::string part2 = temp.substr(ch_pos + 2, temp.length() - ch_pos - 2);
				std::string part1 = temp.substr(0, ch_pos - 1);
				int pos2 = part1.find_last_of("\\/");
				if (pos2 != std::string::npos)
				{
					ch_pos = pos2 + 1;
					part1 = temp.substr(0, ch_pos);
				}
				temp = part1 + part2;
			}
		} while (ch_pos != std::string::npos);

		if (IsExistingDirectory(temp.c_str()))
			sInstalledFolderPath = temp;
		else
			return -1;
	}
	else
		return -1;
	return 0;
}

char* getenvEmul(char* _VarName)
{
	return NULL;
}

///////////////Thread APIs////////////////////
_Use_decl_annotations_ HANDLE CreateThreadEmul(LPSECURITY_ATTRIBUTES unusedThreadAttributes, SIZE_T unusedStackSize, LPTHREAD_START_ADDRRESS lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD unusedThreadId)
{
	return ThreadEmulation::CreateThread(unusedThreadAttributes, unusedStackSize, lpStartAddress, lpParameter, dwCreationFlags, unusedThreadId);
}
_Use_decl_annotations_ BOOL SetThreadPriorityEmul(HANDLE hThread, int nPriority)
{
	return ThreadEmulation::SetThreadPriority(hThread, nPriority);
}
_Use_decl_annotations_ DWORD ResumeThreadEmul(HANDLE hThread)
{
	return ThreadEmulation::ResumeThread(hThread);
}
///////////////Thread APIs////////////////////