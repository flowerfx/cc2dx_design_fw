#pragma once
#include "metrosockdef.h"
#include <ThreadEmulation.h>
void GetTempPathEmul(int path_size, char* path);
char* _getcwdEmul(char* path, int path_size);
char* getenvEmul(char* _VarName);
int _chdirEmul(const char* path);

uint64 timeGetTimeEmul(void);
void SleepEmul(DWORD dwMilliseconds);
void YieldEmul();
///////////////Thread APIs////////////////////
HANDLE CreateThreadEmul(_In_opt_ LPSECURITY_ATTRIBUTES unusedThreadAttributes, _In_ SIZE_T unusedStackSize, _In_ LPTHREAD_START_ADDRRESS lpStartAddress, _In_opt_ LPVOID lpParameter, _In_ DWORD dwCreationFlags, _Out_opt_ LPDWORD unusedThreadId);
BOOL SetThreadPriorityEmul(_In_ HANDLE hThread, _In_ int nPriority);
DWORD ResumeThreadEmul(_In_ HANDLE hThread);
///////////////Thread APIs////////////////////
