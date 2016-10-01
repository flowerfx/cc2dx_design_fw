//*****************************************************************************************************************************************************
//*
//* Win32DebugWindow.cpp
//*
//* Win32 debug window for Casual Core application
//*
//* Christophe Le Bouil
//*****************************************************************************************************************************************************
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <richedit.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <sys/stat.h>

#include "RKLog.h"
#include "RKAssert.h"
#include "RKStdSAFE.h"
#include "RKFile.h"
#include "RKIniFile.h"
#include "RKDevice.h"
#include "RKThread.h"
//#include "RKTimer.h"
#include <time.h>

#include "CasualCore.h"
#include "Platform/Hardware.h"
#include <RKAssertPrivate.h>

#include "Win32DebugWindow.h"
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")
#define WM_PROCESS_PENDING_LOG_LINES (WM_USER + 0x0001)

#if CC_USE_DEBUG_WINDOW==1

AppDebugWindow *AppDebugWindow::m_instance_AppDebugWindow = NULL;

void AppDebugWindow_LogCallback(RKLogLevel ilogLevel, const char *logType, const char* file, long line, const char* function, const char *message);

static const char *s_MsgSelectResolution = "Please select starting resolution...";

//---------------------------------------------------------------- for dbwAddApplicationButtonAuto(), etc
class DBWS_ItemData
{
public:
  DBWS_ItemData()
  {
    m_itemID = -1;
    m_hwndItem = NULL;
    m_pPressed = NULL;
    m_eventCallback = NULL;
  }
  int m_itemID;
  HWND m_hwndItem;
  bool *m_pPressed;
  FuncItemEventCallback *m_eventCallback;
};

#define DBW_ITEM_MAX 100

DBWS_ItemData gDBWS_ItemDataTable[DBW_ITEM_MAX];
int gDBWS_ItemDataCount = 0;
int gDBWS_currentX = 0;
int gDBWS_currentY = 0;



float _GetGlobalUpTimeSafe()  //RKTimer_GetGlobalUpTimeSafe()
{
  //valid anytime in application lifetime
  clock_t clk = clock();
  float upTimeSec = (float) ( (double)clk / (double)CLOCKS_PER_SEC);
  return upTimeSec;
}



//*******************************************************************************************************************************
//* (unsuccessful attempt to redirect stdio to RKLOG .. TOFIX)
//*  http://stackoverflow.com/questions/6532064/redirect-stdout-to-an-edit-control-win32-mark-ii
//*
HANDLE hStdRead, hStdWrite;
HANDLE hStdWriteError;
HANDLE m_hRedirectStdoutThread;

HWND g_hWindow = NULL;

#define MYPRINT      1
#define WM_MYMESSAGE (WM_USER+1)

//---------------------------------------------------------------------------------------------------------------------
DWORD WINAPI myRedirectStdoutThreadRun(void*)
  //---------------------------------------------------------------------------------------------------------------------
{
  char buf_stdio[2048];
  DWORD wasRead = 0;
  bool wantEndThread = false;

  //HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);   //same as hStdWrite ?

  while (wantEndThread == false)
  {
    BOOL readOK = ReadFile(hStdWrite, buf_stdio, sizeof(buf_stdio), &wasRead, NULL);
    if (readOK)
    {
      RKWarning(0, "myRedirectStdoutThreadRun(), TODO");
    }
  }

#if 0
  // Redirect stdout to pipe
  int fds[2];
  _pipe(fds, 1024, O_TEXT);
  _dup2(fds[1], 1); // 1 is stdout

  char buffer[1024];
  for (;;)
  {
    // Need to flush the pipe
    _flushall();
    // Read stdout from pipe
    DWORD dwNumberOfBytesRead = 0;
    //peek
    dwNumberOfBytesRead = _read(fds[0], buffer, 1024 - 1);
    buffer[dwNumberOfBytesRead] = 0;

    // Send data as a message
    COPYDATASTRUCT myCDS;
    myCDS.dwData = MYPRINT;
    myCDS.cbData = dwNumberOfBytesRead + 1;
    myCDS.lpData = buffer;
    PostMessage(g_hWindow,
      WM_MYMESSAGE,
      0,
      (LPARAM)(LPVOID) &myCDS);
  }
#endif

  return 0;
}

//--- static ----------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwRedirectStdio(int argc, char** argv)
  //---------------------------------------------------------------------------------------------------------------------
{
  if (AppDebugWindow::dbwIsCreated()==false)
    AppDebugWindow::dbwCreate(NULL, argc, argv);

  return; /// TOFIX ...

  /*
  //--- hide default console ..
  HWND consHwnd = NULL;
  consHwnd = GetConsoleWindow();
  if (consHwnd)
  {
    ShowWindow(consHwnd, SW_HIDE);
  }

  //---
  CreatePipe(&hStdRead, &hStdWrite, NULL, 0);

  SetStdHandle(STD_OUTPUT_HANDLE, hStdWrite);
  SetStdHandle(STD_ERROR_HANDLE, hStdWriteError);

  //---
  g_hWindow = AppDebugWindow::dbwGetMainHWND();

  // Create worker thread
  DWORD dwThreadId = 0;
  m_hRedirectStdoutThread = CreateThread(
    NULL,// default security
    0,// default stack size
    &myRedirectStdoutThreadRun,  // routine to execute
    NULL, // thread parameter
    0,// immediately run the thread
    &dwThreadId);// thread Id
  if (NULL == m_hRedirectStdoutThread)
  {
    printf("Error creating stdin thread\n");
    return;
  }
  */
}

//*******************************************************************************************************************************


//--- static ----------------------------------------------------------------------------------------------------------
bool AppDebugWindow::dbwIsCreated()
  //---------------------------------------------------------------------------------------------------------------------
{
  return (m_instance_AppDebugWindow != NULL);
}

//--- static ----------------------------------------------------------------------------------------------------------
HWND AppDebugWindow::dbwGetMainHWND(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  if (m_instance_AppDebugWindow)
  {
    return m_instance_AppDebugWindow->m_hMainDebugWin;
  }
  return NULL;
}

//--- static ----------------------------------------------------------------------------------------------------------
HMENU AppDebugWindow::dbwGetMainMenu(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  if (m_instance_AppDebugWindow)
  {
    return m_instance_AppDebugWindow->m_hMainMenu;
  }
  return NULL;
}

//--- static ----------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwUpdate(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  if (m_instance_AppDebugWindow)
  {
    m_instance_AppDebugWindow->dbwFrameUpdate();
  }
}

//--- static ----------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwFlushLog(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  if (m_instance_AppDebugWindow)
  {
    //--- force processing all messages of windows/items ..
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

//--- static ----------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwCreate(HWND hParent, int argc, char** argv)
  //---------------------------------------------------------------------------------------------------------------------
{
  if (m_instance_AppDebugWindow)  //already created ..
  {
    RKLOG("AppDebugWindow::dbwCreate(), already created!..");
    return;
  }

  m_instance_AppDebugWindow = RKNEW(AppDebugWindow);
  m_instance_AppDebugWindow->dbwInit(hParent, argc, argv);

  //--- force processing all messages of windows/items creation..
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

//--- static ----------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwDestroy(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  if (m_instance_AppDebugWindow)
  {
    m_instance_AppDebugWindow->dbwReleaseAll();
    RKDELETE(m_instance_AppDebugWindow);
    m_instance_AppDebugWindow = NULL;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwZero(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  m_hMainDebugWin = NULL;
  m_hwndLogWin = NULL;
  m_hwndGameWindow = NULL;
  m_y0Items = 0;
  m_hwndFrameInfo = NULL;
  m_hwndMemoryInfo = NULL;
  m_hwndButtonEditLogfile = NULL;
  m_hwndButtonEditInifile = NULL;
  m_hwndButtonDumpScene = NULL;
  m_hwndButtonLogMemory = NULL;
  m_hwndButtonTestAssert = NULL;
  m_hwndButtonKeyMenu = NULL;
  m_hwndButtonKeyHomeShort = NULL;
  m_hwndButtonKeyHomeLong = NULL;
  m_hwndButtonKeyBack = NULL;
  m_hwndCheckbox_disableALLDial = NULL;
  m_hwndCheckbox_disableWarningDial = NULL;

  m_assertCheckBoxes_hParent = NULL;
  m_assertCheckBoxes_x = 0;
  m_assertCheckBoxes_y = 0;

  m_applicationPath[0] = 0;
  m_applicationShortName[0] = 0;
  m_textEditorFullFilename[0] = 0;
  m_textEditorIsNotepad = true;

  m_mainDebugWinX = 1028;
  m_mainDebugWinY = 0;
  m_mainDebugWinW = 650;
  m_mainDebugWinH = 700;

  m_LogWinH = 300;
  m_CorePaneH = 100;
  m_AppPaneH = 175;

  m_enableDebugWindow = true;
  m_enableLogToFile = true;
  m_enableLogWithTime = true;
  m_enableAssertCheckBoxes = false;
  m_startupDisableAssertsAll = false;
  m_startupDisableAssertsWarn = false;

  // [Liam] GameWindow Launch Size -------------------------------------
  m_hLaunchSizePane               = NULL;
  m_iLaunchPaneHeight             = 100;
  m_hLaunchMode_IPHONE3GS         = NULL;
  m_hLaunchMode_IPHONE4           = NULL;
  m_hLaunchMode_IPHONE4S          = NULL;
  m_hLaunchMode_IPHONE5           = NULL;
  m_hLaunchMode_IPHONE5S          = NULL;
  m_hLaunchMode_IPAD1             = NULL;
  m_hLaunchMode_IPAD2             = NULL;
  m_hLaunchMode_IPAD3             = NULL;
  m_iGameWindowLaunchWidth        = 0;
  m_iGameWindowLaunchHeight       = 0;
  m_bGameWindowLaunchSizeSelected = false;

  m_owningThreadId = 0;
  m_pPendingLogLinesCS = NULL;
  m_pFileLogCS = NULL;
}

//---------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
  //---------------------------------------------------------------------------------------------------------------------
{
  AppDebugWindow *pAppDebugWindow = AppDebugWindow::dbwGetInstance();
  if (pAppDebugWindow)
    return pAppDebugWindow->dbwProc(hwnd, msg, wParam, lParam);

  return DefWindowProc(hwnd, msg, wParam, lParam);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK AppDebugWindow::dbwProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
  //---------------------------------------------------------------------------------------------------------------------
{
  switch (msg)
  {
  case WM_CLOSE:
    PostQuitMessage(0);
    return 0;
    break;
  case WM_COMMAND:
    {
      dbwProcess_WM_COMMAND(wParam, lParam);
    }
    break;
  case WM_PROCESS_PENDING_LOG_LINES:
    {
      RKList<LogLine> pendingLogLinesCopy;

      // copying pending lines, that's the best way to ensure
      // that CS is locked for the least possible amount of time
      RKCriticalSection_Enter(m_pPendingLogLinesCS);
      pendingLogLinesCopy = m_pendingLogLines;
      m_pendingLogLines.Clear();
      RKCriticalSection_Leave(m_pPendingLogLinesCS);

      for (uint32 i = 0; i < pendingLogLinesCopy.Size(); ++i)
      {
        dbwAddMessageLogLine(pendingLogLinesCopy[i].pText, pendingLogLinesCopy[i].color);
        RKHeap_Free(const_cast<char*>(pendingLogLinesCopy[i].pText));
      }
      pendingLogLinesCopy.Clear();
    }
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

//---------------------------------------------------------------------------------------------------------------------
char *_GetCurrentTimeAndDate(char *pDest, int destSize)
  //---------------------------------------------------------------------------------------------------------------------
{
  SYSTEMTIME st;
  GetLocalTime(&st);
  sprintf_SAFE(pDest, destSize, "[%02d-%02d-%02d;%02d:%02d:%02d.%03d] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  return pDest;
}
//---------------------------------------------------------------------------------------------------------------------
char *_GetCurrentLogTime(char *pDest, int destSize)
  //---------------------------------------------------------------------------------------------------------------------
{
  SYSTEMTIME st;
  GetLocalTime(&st);
  sprintf_SAFE(pDest, destSize, "[%02d:%02d:%02d.%03d]", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  return pDest;
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwInit(HWND hGameWindow, int argc, char** argv)
//---------------------------------------------------------------------------------------------------------------------
{
  m_hwndGameWindow = hGameWindow;

  m_pPendingLogLinesCS = RKCriticalSection_Create("CCDebugWinLogCS");
  m_pFileLogCS = RKCriticalSection_Create("CCDebugWinFileLogCS");

  char pFilenameNoExt[MAX_PATH] = {0};
  RKString_ExtractFilePath(argv[0], m_applicationPath, sizeof(m_applicationPath));
  RKString_ExtractFileBase(argv[0], pFilenameNoExt, sizeof(pFilenameNoExt));
  sprintf_s(m_logfileFullname, PATH_STRING_MAX, "%s%s_LOG.csv", m_applicationPath, pFilenameNoExt);
  sprintf_s(m_debugConfigINIFullname, PATH_STRING_MAX, "%s%s_w32DebugConfig.ini", m_applicationPath, pFilenameNoExt);

  if (!dbwReadConfigFile())
  {
    dbwWriteConfigFile();
    dbwReadConfigFile();
  }

  if (m_enableLogToFile)
  {
    FILE *pf = fopen(m_logfileFullname, "wb");
    if (pf==NULL)
    {
      RKWarning(pf, "BAD, can't write logfile %s! \n(May be locked by some editor / OpenOffice Calc .. ==> Close it and restart..)", m_logfileFullname);    //may be locked by some editor / OpenOffice Calc .. ==> Close it and restart..
      m_enableLogToFile = false;
    }
    if (m_enableLogToFile)
    {
      char bufferTimeDate[128];
      fprintf(pf, "--- Logfile created on %s ---\r\n", _GetCurrentTimeAndDate(bufferTimeDate, sizeof(bufferTimeDate)) );
      if (pf)
        fclose(pf);
    }
  }

  if (m_enableDebugWindow)
  {
    HWND hParent = NULL;

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0; 
    wc.lpfnWndProc = (WNDPROC) DebugWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon   = (HICON)LoadImage(NULL, "logicon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wc.hIconSm = (HICON)LoadImage(NULL, "logicon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "CCDebugWin";

    RegisterClassEx(&wc);

    unsigned int dwWindowStyle = (WS_SIZEBOX | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

    int x = m_mainDebugWinX;
    int y = m_mainDebugWinY;
    int w = m_mainDebugWinW;
    int h = m_mainDebugWinH;

    HMENU hMenu = dbwCreateMenu();  //NULL;
    HINSTANCE hInst = GetModuleHandle(NULL);
    LPVOID lParam = NULL;

    hParent = NULL;
    m_hMainDebugWin = CreateWindow("CCDebugWin", "CasualCoreDebugWin", dwWindowStyle, x, y, w, h, hParent, hMenu, hInst, lParam);
    m_owningThreadId = GetCurrentThreadId();  

    SetClassLong(m_hMainDebugWin, GCL_HBRBACKGROUND,(LONG) GetStockObject (LTGRAY_BRUSH));  //set the background brush using SetClassLong on your dialog's window class.

    dbwCreateRichEdit();
    m_y0Items = m_LogWinH;

    CreateLaunchPane();

    ShowWindow(m_hMainDebugWin, SW_SHOW);

    dbwFrameUpdate();
  }

  if (m_enableDebugWindow || m_enableLogToFile)
  {
    RKLog_SetExtraLogCallback(AppDebugWindow_LogCallback);
    // buildmachine tests need output to stdout!
    //RKLog_Enable_printf(false);
  }
}

////---------------------------------------------------------------------------------------------------------------------
//void AppDebugWindow::dbwInit(HWND hGameWindow, int argc, char** argv)
////---------------------------------------------------------------------------------------------------------------------
//{
//  m_hwndGameWindow = hGameWindow;
//
//  m_pPendingLogLinesCS = RKCriticalSection_Create("CCDebugWinLogCS");
//  m_pFileLogCS = RKCriticalSection_Create("CCDebugWinFileLogCS");
//
//  char *fullCmdLine =  GetCommandLine();
//  char *pStartCmdLine = fullCmdLine;
//  if (pStartCmdLine[0] == '"') pStartCmdLine++; 
//
//  int iLastSlash = strlen(pStartCmdLine)-1;
//  while (iLastSlash>0 && pStartCmdLine[iLastSlash] != '\\')
//    iLastSlash--;
//
//  strncpy_SAFE(m_applicationPath, sizeof(m_applicationPath), pStartCmdLine, iLastSlash);
//  m_applicationPath[iLastSlash] = 0;
//  strcpy_SAFE(m_applicationShortName, sizeof(m_applicationShortName), pStartCmdLine+iLastSlash+1);    //with extra space ! and '"' ..
//
//  int iPeriod = strlen(pStartCmdLine)-1;
//  while (iPeriod>0 && pStartCmdLine[iPeriod] != '.')
//    iPeriod--;
//  strncpy_SAFE(m_logfileFullname, sizeof(m_logfileFullname), pStartCmdLine, iPeriod);
//  RKAssert(iPeriod < sizeof(m_logfileFullname), "BAD");
//  m_logfileFullname[iPeriod] = 0;
//  strcat_SAFE(m_logfileFullname, sizeof(m_logfileFullname), "_LOG.csv");  //.log");
//
//  strncpy_SAFE(m_debugConfigINIFullname, sizeof(m_debugConfigINIFullname), pStartCmdLine, iPeriod);
//  RKAssert(iPeriod < sizeof(m_debugConfigINIFullname), "BAD");
//  m_debugConfigINIFullname[iPeriod] = 0;
//  strcat_SAFE(m_debugConfigINIFullname, sizeof(m_debugConfigINIFullname), "_w32DebugConfig.ini");
//
//
//  dbwReadConfigFile();
//
//  if (m_enableLogToFile)
//  {
//    FILE *pf = fopen(m_logfileFullname, "wb");
//    if (pf==NULL)
//    {
//      RKWarning(pf, "BAD, can't write logfile %s! \n(May be locked by some editor / OpenOffice Calc .. ==> Close it and restart..)", m_logfileFullname);    //may be locked by some editor / OpenOffice Calc .. ==> Close it and restart..
//      m_enableLogToFile = false;
//    }
//    if (m_enableLogToFile)
//    {
//      char bufferTimeDate[128];
//      fprintf(pf, "--- Logfile created on %s ---\r\n", _GetCurrentTimeAndDate(bufferTimeDate, sizeof(bufferTimeDate)) );
//      if (pf)
//        fclose(pf);
//    }
//  }
//
//  if (m_enableDebugWindow)
//  {
//    HWND hParent = NULL;
//
//    WNDCLASSEX wc;
//    wc.cbSize = sizeof(WNDCLASSEX);
//    wc.style = 0; 
//    wc.lpfnWndProc = (WNDPROC) DebugWindowProc;
//    wc.cbClsExtra = 0;
//    wc.cbWndExtra = 0;
//    wc.hInstance = GetModuleHandle(NULL);
//    wc.hIcon   = (HICON)LoadImage(NULL, "logicon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
//    wc.hIconSm = (HICON)LoadImage(NULL, "logicon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
//    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//    wc.lpszMenuName = NULL;
//    wc.lpszClassName = "CCDebugWin";
//
//    RegisterClassEx(&wc);
//
//    unsigned int dwWindowStyle = (WS_SIZEBOX | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
//
//    int x = m_mainDebugWinX;
//    int y = m_mainDebugWinY;
//    int w = m_mainDebugWinW;
//    int h = m_mainDebugWinH;
//
//    HMENU hMenu = dbwCreateMenu();  //NULL;
//    HINSTANCE hInst = GetModuleHandle(NULL);
//    LPVOID lParam = NULL;
//
//    hParent = NULL;
//    m_hMainDebugWin = CreateWindow("CCDebugWin", "CasualCoreDebugWin", dwWindowStyle, x, y, w, h, hParent, hMenu, hInst, lParam);
//    m_owningThreadId = GetCurrentThreadId();  
//
//    SetClassLong(m_hMainDebugWin, GCL_HBRBACKGROUND,(LONG) GetStockObject (LTGRAY_BRUSH));  //set the background brush using SetClassLong on your dialog's window class.
//
//    dbwCreateRichEdit();
//    m_y0Items = m_LogWinH;
//
//    CreateLaunchPane();
//
//    ShowWindow(m_hMainDebugWin, SW_SHOW);
//
//    dbwFrameUpdate();
//  }
//
//  if (m_enableDebugWindow || m_enableLogToFile)
//  {
//    RKLog_SetExtraLogCallback(AppDebugWindow_LogCallback);
//    // buildmachine tests need output to stdout!
//    //RKLog_Enable_printf(false);
//  }
//}


void AppDebugWindow::CreateLaunchPane()
{
  // Create launch size pane
  m_hLaunchSizePane = CreateWindow("CCDebugWin", 
                                   "m_hLaunchSizePane", 
                                   WS_VISIBLE | WS_CHILD | WS_BORDER | WS_GROUP | WS_THICKFRAME, 
                                   0, m_y0Items, m_mainDebugWinW - 8, m_mainDebugWinH, 
                                   m_hMainDebugWin, m_hMainMenu, GetModuleHandle(NULL), NULL);

  // Cerate launch size pane child buttons
  int x = 0;
  int y = 0;
  int h = 20;
  int w = 120;
  //dbwAddStaticText(m_hLaunchSizePane, "Please select starting resolution...", x, y, 600, h); y += h;
  m_hLauchSizeTitle = dbwAddStaticText(m_hLaunchSizePane, s_MsgSelectResolution, x, y, 600, h); y += h;

  m_hLaunchMode_IPHONE3GS = dbwAddButton(m_hLaunchSizePane, "iPhone 3GS - 480x320", x, y, w, h); x += w;
  m_hLaunchMode_IPHONE4   = dbwAddButton(m_hLaunchSizePane, "iPhone 4 - 960x640",   x, y, w, h); x += w;
  m_hLaunchMode_IPHONE4S  = dbwAddButton(m_hLaunchSizePane, "iPhone 4S - 960x640",  x, y, w, h); x += w;
  m_hLaunchMode_IPHONE5   = dbwAddButton(m_hLaunchSizePane, "iPhone 5 - 1136x640",  x, y, w, h); x += w;
  m_hLaunchMode_IPHONE5S   = dbwAddButton(m_hLaunchSizePane, "iPhone 5S - 1136x640",  x, y, w, h); x = 0; y += h;
  m_hLaunchMode_IPAD1     = dbwAddButton(m_hLaunchSizePane, "iPad 1 - 1024x768",    x, y, w, h); x += w;
  m_hLaunchMode_IPAD2     = dbwAddButton(m_hLaunchSizePane, "iPad 2 - 1024x768",    x, y, w, h); x += w;
  m_hLaunchMode_IPAD3     = dbwAddButton(m_hLaunchSizePane, "iPad 3 - 2048x1536",   x, y, w, h); x = 0; y += h;
  m_hLaunchMode_720p      = dbwAddButton(m_hLaunchSizePane, "720p - 1280x720",      x, y, w, h); x += w;
  m_hLaunchMode_1080p     = dbwAddButton(m_hLaunchSizePane, "1080p - 1920x1080",    x, y, w, h); x += w;
  x = 0; y += h; w = 200;
  dbwAssert_SetParent(m_hLaunchSizePane, x,y);
  if (m_enableAssertCheckBoxes)
    dbwAssert_AddCheckBoxes();
}

void AppDebugWindow::DestroyLaunchPane()
{
  DestroyWindow(m_hLaunchSizePane);
  m_hLaunchSizePane = NULL;

  unsigned int dwWindowStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | WS_GROUP | WS_THICKFRAME;
  int x = 0;
  int y = m_y0Items;
  int w = m_mainDebugWinW - 8;
  int h = m_CorePaneH;
  HINSTANCE hInst = GetModuleHandle(NULL);

  m_hwndCasualCoreToolPane = CreateWindow("CCDebugWin", "m_hwndCasualCoreToolPane", dwWindowStyle, x, y, w, h, m_hMainDebugWin, m_hMainMenu, hInst, NULL);

  y += h + 4;
  h = m_AppPaneH;
  m_hwndApplicationToolPane = CreateWindow("CCDebugWin", "m_hwndApplicationToolPane", dwWindowStyle, x, y, w, h, m_hMainDebugWin, m_hMainMenu, hInst, NULL);

  dbwAddCoreToolPaneItems();

}

enum
{
  DBWMENU_FIRST_ = 16000,

  DBWMENU_SCRSIZE_FIRST_,
  DBWMENU_SCRSIZE_480x320 ,
  DBWMENU_SCRSIZE_640x480 ,
  DBWMENU_SCRSIZE_800x480 ,
  DBWMENU_SCRSIZE_800x600 ,
  DBWMENU_SCRSIZE_854x480 ,
  DBWMENU_SCRSIZE_960x540 ,
  DBWMENU_SCRSIZE_1024x580,
  DBWMENU_SCRSIZE_1024x600,
  DBWMENU_SCRSIZE_1024x768,
  DBWMENU_SCRSIZE_1196x720,
  DBWMENU_SCRSIZE_1280x720,
  DBWMENU_SCRSIZE_1280x752,
  DBWMENU_SCRSIZE_1280x800,
  DBWMENU_SCRSIZE_LAST_,

  DBWMENU_MEMORY_DUMPALL,
  DBWMENU_TEXTURE_DUMPALL,
  DBWMENU_SCENE_DUMPALL,
  DBWMENU_LOG_FILTERS,
  DBWMENU_LOG_VIEWTYPE_TEXT,
  DBWMENU_LOG_VIEWTYPE_LIST,

  DBWMENU_ASSERT_CHECKBOXES_SHOW,
  DBWMENU_ASSERT_CHECKBOXES_HIDE,
  DBWMENU_ASSERT_RESET_IGNORE,

  DBWMENU_LAUNCH_SCRSIZE_FIRST_,
  DBWMENU_LAUNCH_SCRSIZE_480x320 ,
  DBWMENU_LAUNCH_SCRSIZE_640x480 ,
  DBWMENU_LAUNCH_SCRSIZE_800x480 ,
  DBWMENU_LAUNCH_SCRSIZE_800x600 ,
  DBWMENU_LAUNCH_SCRSIZE_854x480 ,
  DBWMENU_LAUNCH_SCRSIZE_960x540 ,
  DBWMENU_LAUNCH_SCRSIZE_1024x580,
  DBWMENU_LAUNCH_SCRSIZE_1024x600,
  DBWMENU_LAUNCH_SCRSIZE_1024x768,
  DBWMENU_LAUNCH_SCRSIZE_1196x720,
  DBWMENU_LAUNCH_SCRSIZE_1280x720,
  DBWMENU_LAUNCH_SCRSIZE_1280x752,
  DBWMENU_LAUNCH_SCRSIZE_1280x800,
  DBWMENU_LAUNCH_SCRSIZE_LAST_,
};

//---------------------------------------------------------------------------------------------------------------------
HMENU AppDebugWindow::dbwCreateMenu(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  long flag_noGray = 0;
  long flag_normal = MF_STRING | flag_noGray;

  //m_hSubMenuScreenSize = CreateMenu();
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_480x320 , "480x320 ");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_640x480 , "640x480 ");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_800x480 , "800x480 ");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_800x600 , "800x600 ");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_854x480 , "854x480 ");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_960x540 , "960x540 ");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_1024x580, "1024x580");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_1024x600, "1024x600");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_1024x768, "1024x768");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_1196x720, "1196x720");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_1280x720, "1280x720");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_1280x752, "1280x752");
  //AppendMenu(m_hSubMenuScreenSize, flag_normal,	DBWMENU_SCRSIZE_1280x800, "1280x800");

  m_hSubMenuMemory = CreateMenu();
  AppendMenu(m_hSubMenuMemory, flag_normal,	DBWMENU_MEMORY_DUMPALL, "Dump all");

  m_hSubMenuTexture = CreateMenu();
  AppendMenu(m_hSubMenuTexture, flag_normal,	DBWMENU_TEXTURE_DUMPALL, "Dump all");

  m_hSubMenuScene = CreateMenu();
  AppendMenu(m_hSubMenuScene, flag_normal,	DBWMENU_SCENE_DUMPALL, "Dump all objects");

  m_hSubMenuLogging = CreateMenu();
  AppendMenu(m_hSubMenuLogging, flag_normal,	DBWMENU_LOG_FILTERS, "Set Filters");
  AppendMenu(m_hSubMenuLogging, MF_SEPARATOR, 0, 0);
  AppendMenu(m_hSubMenuLogging, flag_normal,	DBWMENU_LOG_VIEWTYPE_TEXT, "View Type:Text");
  AppendMenu(m_hSubMenuLogging, flag_normal,	DBWMENU_LOG_VIEWTYPE_LIST, "View Type:List");

  m_hSubMenuAssert = CreateMenu();
  AppendMenu(m_hSubMenuAssert, flag_normal,	DBWMENU_ASSERT_CHECKBOXES_SHOW, "Show 'Disable asserts CheckBoxes'");
  AppendMenu(m_hSubMenuAssert, flag_normal,	DBWMENU_ASSERT_CHECKBOXES_HIDE, "Hide 'Disable asserts CheckBoxes'");
  AppendMenu(m_hSubMenuAssert, MF_SEPARATOR, 0, 0);
  AppendMenu(m_hSubMenuAssert, flag_normal,	DBWMENU_ASSERT_RESET_IGNORE, "Reset 'Ignore Always' flags");

  //---
  m_hMainMenu = CreateMenu();
  //AppendMenu(m_hMainMenu, MF_POPUP | MF_STRING | flag_noGray,	(UINT)m_hSubMenuScreenSize, "Screen Size");
  AppendMenu(m_hMainMenu, MF_POPUP | MF_STRING | flag_noGray,	(UINT)m_hSubMenuMemory, "Memory");
  AppendMenu(m_hMainMenu, MF_POPUP | MF_STRING | flag_noGray,	(UINT)m_hSubMenuTexture, "Textures");
  AppendMenu(m_hMainMenu, MF_POPUP | MF_STRING | flag_noGray,	(UINT)m_hSubMenuScene, "Scene");
  AppendMenu(m_hMainMenu, MF_POPUP | MF_STRING | flag_noGray,	(UINT)m_hSubMenuLogging, "Logging");
  AppendMenu(m_hMainMenu, MF_POPUP | MF_STRING | flag_noGray,	(UINT)m_hSubMenuAssert, "Asserts");

  return m_hMainMenu;
}

//-------------------------------------------------------------------------------
void AppDebugWindow_LogCallback(RKLogLevel ilogLevel, const char *logType, const char* file, long line, const char* function, const char *message)
  //-------------------------------------------------------------------------------
{
  AppDebugWindow *pAppDebugWindow = AppDebugWindow::dbwGetInstance();
  if (pAppDebugWindow)
  {
    pAppDebugWindow->dbwLogMessage(ilogLevel, logType, file, line, function, message);
  }
}

//--------------------------------------------------------------------
bool _hasDoubleQuote(const char *p)
  //--------------------------------------------------------------------
{
  while (*p)
  {
    if (*p++ == '"')
      return true;
  }
  return false;
}

//--------------------------------------------------------------------
const char *_reQuote(const char *src, char *dest, int sizeOfDest)
  //--------------------------------------------------------------------
{
  //add a double quote for CSV compliance .. [rem: using OpenOffice Calc, set 'separated by' to Comma and Space, + Merge delimiters ]
  int dquoteCnt=0;
  const char *p=src;
  char *dst = dest;
  int writeCnt = 0;
  while (*p && writeCnt+2<sizeOfDest)
  {
    char c = *p++;
    if (c == '\"')
    {
      dquoteCnt++;
      *dst++ = c;
      *dst++ = c;
      writeCnt++;
    }
    else
      *dst++ = c;
    writeCnt++;
  }

  *dst++ = 0;

  return dest;
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwAddMessageLogLine(const char* message, COLORREF msgColor)
//---------------------------------------------------------------------------------------------------------------------
{
  CHARRANGE cRange;
  cRange.cpMin = -1;
  cRange.cpMax = -1;

  //  Filter in action
  if( !m_linesFilter.IsEmpty() && StrStrI(message, m_linesFilter.GetString()) == NULL )
  {
    return;
  }

  //The SendMessage function calls the window procedure for the specified window and does not return until the window procedure has processed the message.
  //The PostMessage function places (posts) a message in the message queue associated with the thread that created the specified window and returns without waiting for the thread to process the message.
  SendMessage( m_hwndLogWin, EM_EXSETSEL, 0,  (LPARAM)&cRange);

  CHARFORMAT txtFormat;
  txtFormat.cbSize = sizeof(CHARFORMAT);
  txtFormat.crTextColor = msgColor;
  txtFormat.dwEffects = 0;
  txtFormat.dwMask = CFM_COLOR;
  SendMessage(m_hwndLogWin, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&txtFormat);

  char messageCR[2048];
  sprintf_SAFE(messageCR, sizeof(messageCR), "%s\n", message);
  BOOL enableUndo = FALSE;
  SendMessage( m_hwndLogWin, EM_REPLACESEL, enableUndo , (LPARAM)messageCR);

  SendMessage(m_hwndLogWin, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&txtFormat);

  SendMessage( m_hwndLogWin, EM_EXSETSEL, 0,  (LPARAM)&cRange);
  SendMessage(m_hwndLogWin, EM_SCROLLCARET, 0, 0);
  SendMessage(m_hwndLogWin, EM_SCROLL, SB_BOTTOM, 0);
}


//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwLogMessage(int ilogLevel, const char *logType, const char* file, long line, const char* function_0, const char *message_0)
  //---------------------------------------------------------------------------------------------------------------------
{
  const char *function = function_0;
  const char *message = message_0;
  char function_requoted[1024];
  char message_requoted[1024];

  static COLORREF col_RKLL_UNKNOWN = RGB(0,0,0);
  static COLORREF col_RKLL_FATAL = RGB(200,0,0);
  static COLORREF col_RKLL_ERROR = RGB(255,100,64);
  static COLORREF col_RKLL_WARN = RGB(63,156,255);
  static COLORREF col_RKLL_INFO = RGB(0,120,0);
  static COLORREF col_RKLL_TRACE = RGB(90,90,90);
  COLORREF msgColor = col_RKLL_UNKNOWN;
  switch(ilogLevel)
  {
  case RKLL_FATAL: msgColor = col_RKLL_FATAL; break;
  case RKLL_ERROR: msgColor = col_RKLL_ERROR; break;
  case RKLL_WARN: msgColor = col_RKLL_WARN; break;
  case RKLL_INFO: msgColor = col_RKLL_INFO; break;
  case RKLL_TRACE: msgColor = col_RKLL_TRACE; break;
  }

  char logTime[64]="";
  if (m_enableLogWithTime)
    _GetCurrentLogTime(logTime, sizeof(logTime));

  if (m_enableLogToFile)
  {
    RKCriticalSection_Enter(m_pFileLogCS);

    FILE *pf = fopen(m_logfileFullname, "ab");
    if (pf)
    {
      if (_hasDoubleQuote(function)) 
        function = _reQuote(function_0, function_requoted, sizeof(function_requoted));
      if (_hasDoubleQuote(message_0)) 
        message = _reQuote(message_0, message_requoted, sizeof(message_requoted));

      if (m_enableLogWithTime)
        fprintf(pf, "%s, ", logTime);

      fprintf(pf, "%d, %s, %s, %d, \"%s\", \"%s\"\r\n", ilogLevel, logType, file, line, function, message);
      fclose(pf);
    }

    RKCriticalSection_Leave(m_pFileLogCS);
  }

  if (m_enableDebugWindow && m_hwndLogWin)
  {
    if (GetCurrentThreadId() == m_owningThreadId)
    {
      dbwAddMessageLogLine(message, msgColor);
    }
    else
    {
      uint32 messageLength = strlen(message);
      char* messageCopy = (char*)RKHeap_Alloc(messageLength + 1);
      memcpy(messageCopy, message, messageLength + 1);

      LogLine logLine;
      logLine.color = msgColor;
      logLine.pText = messageCopy;

      RKCriticalSection_Enter(m_pPendingLogLinesCS);
      m_pendingLogLines.Append(logLine);
      // if that's the first line, otherwise message had already been sent
      if (m_pendingLogLines.Size() == 1)
      {
        PostMessage(m_hMainDebugWin, WM_PROCESS_PENDING_LOG_LINES, NULL, NULL);
      }
      RKCriticalSection_Leave(m_pPendingLogLinesCS);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwReleaseAll(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  RKCriticalSection_Enter(m_pPendingLogLinesCS);
  RKCriticalSection_Enter(m_pFileLogCS);
  RKLog_SetExtraLogCallback(NULL);
  RKCriticalSection_Leave(m_pFileLogCS);
  RKCriticalSection_Leave(m_pPendingLogLinesCS);

  RKCriticalSection_Destroy(&m_pPendingLogLinesCS);
  RKCriticalSection_Destroy(&m_pFileLogCS);
  for (uint32 i = 0; i < m_pendingLogLines.Size(); ++i)
  {
    RKHeap_Free(const_cast<char*>(m_pendingLogLines[i].pText));
  }
  m_pendingLogLines.Clear();

  DestroyWindow(m_hMainDebugWin);
  m_hMainDebugWin = NULL;
}

static HINSTANCE m_hInstRichEdit20;
//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwCreateRichEdit(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  //
  extern HINSTANCE m_hInstRichEdit20;
  if (m_hInstRichEdit20 == NULL)
    m_hInstRichEdit20 = LoadLibrary("riched20.dll") ;

  HWND hwndParentWin = m_hMainDebugWin;
  HINSTANCE appHInstance = GetModuleHandle(NULL);

  int x=0, y=0, w=m_mainDebugWinW - 8, h = m_LogWinH;

  m_hwndLogWin = CreateWindowEx(0, RICHEDIT_CLASS, TEXT("dbwCreateRichEdit() OK\n"),
    ES_LEFT |ES_MULTILINE |	WS_VSCROLL |WS_HSCROLL | 	WS_CHILD|ES_AUTOVSCROLL|ES_AUTOHSCROLL| WS_VISIBLE | WS_TABSTOP | WS_BORDER
    , x,y,w,h, hwndParentWin, NULL, appHInstance, NULL);

  if (m_hwndLogWin)		// rem Rich Edit 2.0 has both ANSI and Unicode window classes RichEdit20A and RichEdit20W, ...  need /see LoadLibrary
  {
    SendMessage(m_hwndLogWin, EM_EXLIMITTEXT, 0, 500000);
    SendMessage(m_hwndLogWin, EM_SETOPTIONS, ECOOP_OR, ECO_AUTOWORDSELECTION);
    SendMessage(m_hwndLogWin, EM_SCROLLCARET, 0, 0);
    HFONT hFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
    SendMessage( m_hwndLogWin, WM_SETFONT, (WPARAM)hFont, TRUE );				// Tell the control to use the new font.
    ShowWindow(m_hwndLogWin, SW_SHOW);
    SendMessage(m_hwndLogWin, EM_SCROLLCARET, 0, 0);
  }
  else
  {
    m_LogWinH = 0;
  }
}


void AppDebugWindow::dbwAddFilterString(HWND hParent, int x, int y)
  //---------------------------------------------------------------------------------------------------------------------
{
  int w = m_mainDebugWinW - 8, h = 24;

  HWND hwndParentWin = m_hMainDebugWin;
  HINSTANCE appHInstance = GetModuleHandle(NULL);

  m_hwndEditField = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "< filter pattern >",
    WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
    x, y, w, h,
    hParent, (HMENU)(101),
   appHInstance, NULL);


  if (m_hwndEditField)		// rem Rich Edit 2.0 has both ANSI and Unicode window classes RichEdit20A and RichEdit20W, ...  need /see LoadLibrary
  {
  }

}


void AppDebugWindow::ApplyFilterString()
{
  const int c_bufSize = 2048;

  char szBuffer[c_bufSize];

  // Obtains input from the textbox and puts it into the char array
  GetWindowText(m_hwndEditField, szBuffer, c_bufSize);

  m_linesFilter = szBuffer;

  //  Filter all log
  if( !m_linesFilter.IsEmpty() )
  {
    RKString  wholeLog;
    int linesCount = SendMessage( m_hwndLogWin, EM_GETLINECOUNT, 0, 0 );
    for(int i = 0; i < linesCount; ++i)
    {
      ((WORD*) szBuffer)[0] = c_bufSize;
      int lastCharacterIdx = SendMessage( m_hwndLogWin, EM_GETLINE, i, (LPARAM)szBuffer );
      szBuffer[lastCharacterIdx] = 0;

      if( StrStrI(szBuffer, m_linesFilter.GetString()) == NULL )
      {
        continue;
      }

      wholeLog += szBuffer;
//      wholeLog += "\n";
    }

    //  Replace whole text
    SendMessage( m_hwndLogWin, WM_SETTEXT, 0, (LPARAM)wholeLog.GetString() );
    SendMessage(m_hwndLogWin, EM_SCROLLCARET, 0, 0);
    SendMessage(m_hwndLogWin, EM_SCROLL, SB_BOTTOM, 0);
  }
}


//--- static ----------------------------------------------------------------------------------------------------------
HWND AppDebugWindow::dbwAddPaneWindow(HWND hParent, char *txt, int x, int y, int w, int h)
  //---------------------------------------------------------------------------------------------------------------------
{
  HINSTANCE instance = GetModuleHandle(NULL);
  HWND hNewText = CreateWindowA("STATIC" , txt,		SS_LEFT|WS_CHILD|WS_VISIBLE   , x,y,w,h, hParent, NULL, instance, NULL);

  HFONT hFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
  SendMessage( hNewText, WM_SETFONT, (WPARAM)hFont, TRUE );				// Tell the control to use the new font.

  return hNewText;
}


//--- static ----------------------------------------------------------------------------------------------------------
HWND AppDebugWindow::dbwAddStaticText(HWND hParent, const char *txt, int x, int y, int w, int h)
  //---------------------------------------------------------------------------------------------------------------------
{
  HINSTANCE instance = GetModuleHandle(NULL);
  HWND hNewText = CreateWindowA("STATIC", txt, SS_LEFT|WS_CHILD|WS_VISIBLE, x, y, w, h, hParent, NULL, instance, NULL);

  HFONT hFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
  SendMessage( hNewText, WM_SETFONT, (WPARAM)hFont, TRUE ); // Tell the control to use the new font.

  return hNewText;
}

//--- static ----------------------------------------------------------------------------------------------------------
HWND AppDebugWindow::dbwAddButton(HWND hParent, char *txt, int x, int y, int w, int h)
  //---------------------------------------------------------------------------------------------------------------------
{
  HINSTANCE instance = GetModuleHandle(NULL);
  HWND hNewBut = CreateWindowA("BUTTON", txt, BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP, x, y, w, h, hParent, NULL, instance, NULL);

  HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
  SendMessage(hNewBut, WM_SETFONT, (WPARAM)hFont, TRUE);  // Tell the control to use the new font.

  return hNewBut;
}

//---------------------------------------------------------------------------------------------------------------------------------------
HWND AppDebugWindow::dbwAddCheckbox(HWND hParent, char *txt, int x, int y, int w, int h)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HINSTANCE instance = GetModuleHandle(NULL);

	HWND hNewBut = CreateWindow("BUTTON" , txt,		BS_AUTOCHECKBOX |WS_CHILD|WS_VISIBLE   , x,y,w,h, hParent, NULL, instance, NULL);

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hNewBut, WM_SETFONT, (WPARAM)hFont, TRUE);  // Tell the control to use the new font.
	return hNewBut;
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwAddCoreToolPaneItems(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  int x = 0;
  int y = 0;
  int h = 18;
  int w = 200;
  HWND hParent = m_hwndCasualCoreToolPane;

  m_hwndFrameInfo = dbwAddStaticText(hParent, "frame info..", x, y, w, h);
  m_hwndMemoryInfo = dbwAddStaticText(hParent, "memory info..", x+w, y, w, h);
  y += h;

  w = 120;
  h = 20;
  x = 0;
  m_hwndButtonEditLogfile = dbwAddButton(hParent, "Edit Logfile", x, y, w,h);
  x += w;
  m_hwndButtonEditInifile = dbwAddButton(hParent, "Edit INI file", x, y, w,h);
  x += w;
  m_hwndButtonDumpScene = dbwAddButton(hParent, "Dump scene", x, y, w,h);
  x += w;
  //m_hwndButtonLogMemory = dbwAddButton(hParent, "Log memory...", x, y, w,h);
  //x += w;
  m_hwndButtonTestAssert = dbwAddButton(hParent, "Test RKAssert", x, y, w,h);

  y += h;
  x = 0;

  m_hwndButtonKeyMenu = dbwAddButton(hParent, "Key:Menu", x, y, w,h);
  x += w;
  m_hwndButtonKeyHomeShort = dbwAddButton(hParent, "Key:Home short", x, y, w,h);
  x += w;
  m_hwndButtonKeyHomeLong = dbwAddButton(hParent, "Key:Home long", x, y, w,h);
  x += w;
  m_hwndButtonKeyBack = dbwAddButton(hParent, "Key:Back", x, y, w,h);
  x += w;
  m_hwndButtonFilterApply = dbwAddButton(hParent, "Filter", x, y, w, h);
  x += w;

  y += h;
  x = 0;
  dbwAssert_SetParent(hParent, x,y);
  if (m_enableAssertCheckBoxes)
    dbwAssert_AddCheckBoxes();

  dbwAddFilterString(hParent, 0, y);

  dbwFlushLog();  //force update win items
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwAssert_SetParent(HWND hParent, int x, int y)
//---------------------------------------------------------------------------------------------------------------------
{
  m_assertCheckBoxes_hParent = hParent;
  m_assertCheckBoxes_x = x;
  m_assertCheckBoxes_y = y;
}

extern bool gErrDisplayW32_forceDisableAllDialogs;       //m_forceDisableAllDialogs
extern bool gErrDisplayW32_forceDisableWarningDialogs;   //m_forceDisableWarningDialogs

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwAssert_AddCheckBoxes(void)
//---------------------------------------------------------------------------------------------------------------------
{
  HWND hParent = m_assertCheckBoxes_hParent;
  if (hParent==NULL)
    return; //ooppss

  int x = m_assertCheckBoxes_x;
  int y = m_assertCheckBoxes_y;
  int h = 20;
  int w = 200;

  dbwAssert_RemoveCheckBoxes();

  m_hwndCheckbox_disableALLDial = dbwAddCheckbox(hParent, "Disable ALL assert dialogs", x, y, w,h); x+=w;
  int isDisabledALL_checked = gErrDisplayW32_forceDisableAllDialogs ? BST_CHECKED : BST_UNCHECKED;
	SendMessage(m_hwndCheckbox_disableALLDial,  BM_SETCHECK, isDisabledALL_checked, 0);
  m_hwndCheckbox_disableWarningDial = dbwAddCheckbox(hParent, "Disable RKWarning assert dialogs", x, y, w,h); x+=w;
  int isDisabledWarn_checked = gErrDisplayW32_forceDisableWarningDialogs ? BST_CHECKED : BST_UNCHECKED;
	SendMessage(m_hwndCheckbox_disableWarningDial,  BM_SETCHECK, isDisabledWarn_checked, 0);
  EnableWindow(m_hwndCheckbox_disableWarningDial, isDisabledALL_checked ? FALSE : TRUE);
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwAssert_RemoveCheckBoxes(void)
//---------------------------------------------------------------------------------------------------------------------
{
  if (m_hwndCheckbox_disableALLDial)
  {
    DestroyWindow(m_hwndCheckbox_disableALLDial);
    m_hwndCheckbox_disableALLDial = NULL;
  }
  if (m_hwndCheckbox_disableWarningDial)
  {
    DestroyWindow(m_hwndCheckbox_disableWarningDial);
    m_hwndCheckbox_disableWarningDial = NULL;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwAssert_EnableCheckBoxes(bool enab)
//---------------------------------------------------------------------------------------------------------------------
{
  m_enableAssertCheckBoxes = enab;
  dbwWriteConfigFile();

  if (enab)
  {
    if (m_hwndCheckbox_disableALLDial == NULL)
      dbwAssert_AddCheckBoxes();
  }
  else
  {
    if (m_hwndCheckbox_disableALLDial)
      dbwAssert_RemoveCheckBoxes();
  }
}

//---------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwFrameUpdate(void)
  //---------------------------------------------------------------------------------------------------------------------
{
  static unsigned int m_frameCount=0;
  char buffer[64];
  sprintf_SAFE(buffer, sizeof(buffer), "m_frameCount=%u", m_frameCount);
  SetWindowText(m_hwndFrameInfo, buffer);

  m_frameCount++;
}

//--------------------------------------------------------------------------------------
void AppDebugWindow::dbwProcess_BN_CLICKED(HWND hBut)
  //--------------------------------------------------------------------------------------
{
  if(hBut == m_hwndButtonFilterApply)
  {
    ApplyFilterString();
  }

  //--- internal buttons ---
  if (hBut == m_hwndButtonLogMemory)
  {
    RKLOG_WARN("ButtonLogMemory.. RKLOG_WARN [TODO]...");
    dbwMemory_DumpAll();
  }
  if (hBut == m_hwndButtonDumpScene)
  {
    dbwScene_DumpAll();
  }

  if (hBut == m_hwndButtonEditLogfile)
  {
    RKLOG_WARN("editing %s ...", m_logfileFullname);
    dbwTextEdit(m_logfileFullname);
  }
  if (hBut == m_hwndButtonEditInifile)
  {
    RKLOG_WARN("editing %s ...", m_debugConfigINIFullname);
    dbwTextEdit(m_debugConfigINIFullname);
  }

  if (hBut == m_hwndButtonTestAssert)
  {
    int i=234;
    RKWarning(i<100, "[RKWarning/RKAssert test] oops, i=%d", i);
  }

  if (hBut == m_hwndButtonKeyMenu)
  {
    RKLOG("ButtonKey..TODO");
    printf("test printf m_hwndButtonKeyMenu..\n");  //redirected to log ?
  }
  if (hBut == m_hwndButtonKeyHomeShort)
  {
    RKLOG("ButtonKey..TODO");
  }
  if (hBut == m_hwndButtonKeyHomeLong)
  {
    RKLOG("ButtonKey..TODO");
  }
  if (hBut == m_hwndButtonKeyBack)
  {
    RKLOG("ButtonKey..TODO");
  }
  if (hBut == m_hwndCheckbox_disableALLDial)
  {
    bool isChecked = ( Button_GetCheck(m_hwndCheckbox_disableALLDial) == BST_CHECKED );
    gRKErrConf.ecClearIgnoreList();
    if (isChecked==false) { RKLOG("All 'Ignore always' RKAsserts have been cleared.."); }
    EnableWindow(m_hwndCheckbox_disableWarningDial, isChecked ? FALSE : TRUE);
    gErrDisplayW32_forceDisableAllDialogs = isChecked;
    m_startupDisableAssertsAll = gErrDisplayW32_forceDisableAllDialogs;
    dbwWriteConfigFile();
  }
  if (hBut == m_hwndCheckbox_disableWarningDial)
  {
    bool isChecked = ( Button_GetCheck(m_hwndCheckbox_disableWarningDial) == BST_CHECKED );
    gRKErrConf.ecClearIgnoreList();
    if (isChecked==false) { RKLOG("All 'Ignore always' RKAsserts have been cleared.."); }
    gErrDisplayW32_forceDisableWarningDialogs = isChecked;
    m_startupDisableAssertsWarn = gErrDisplayW32_forceDisableWarningDialogs;
    dbwWriteConfigFile();
  }

  // [Liam] GameWindow Launch Size -------------------------------------
  bool resolutionButtonPressed = false;
  if (hBut == m_hLaunchMode_IPHONE3GS)
  {
    m_iGameWindowLaunchWidth = 480;
    m_iGameWindowLaunchHeight = 320;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_LOW, CasualCore::Hardware::GPU_LOW, CasualCore::Hardware::MEM_LOW, 1);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_IPHONE4)
  {
    m_iGameWindowLaunchWidth = 960;
    m_iGameWindowLaunchHeight = 640;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_LOW, CasualCore::Hardware::GPU_LOW, CasualCore::Hardware::MEM_MEDIUM, 1);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_IPHONE4S)
  {
    m_iGameWindowLaunchWidth = 960;
    m_iGameWindowLaunchHeight = 640;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_MEDIUM, CasualCore::Hardware::GPU_MEDIUM, CasualCore::Hardware::MEM_MEDIUM, 2);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_IPHONE5)
  {
    m_iGameWindowLaunchWidth = 1136;
    m_iGameWindowLaunchHeight = 640;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_HIGH, CasualCore::Hardware::GPU_HIGH, CasualCore::Hardware::MEM_HIGH, 2);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_IPHONE5S)
  {
    m_iGameWindowLaunchWidth = 1136;
    m_iGameWindowLaunchHeight = 640;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_HIGH, CasualCore::Hardware::GPU_HIGH, CasualCore::Hardware::MEM_HIGH, 2, 3);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_IPAD1)
  {
    m_iGameWindowLaunchWidth = 1024;
    m_iGameWindowLaunchHeight = 768;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_LOW, CasualCore::Hardware::GPU_LOW, CasualCore::Hardware::MEM_LOW, 1);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_IPAD2)
  {
    m_iGameWindowLaunchWidth = 1024;
    m_iGameWindowLaunchHeight = 768;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_MEDIUM, CasualCore::Hardware::GPU_MEDIUM, CasualCore::Hardware::MEM_MEDIUM, 2);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_IPAD3)
  {
    m_iGameWindowLaunchWidth = 2048;
    m_iGameWindowLaunchHeight = 1536;
    m_bGameWindowLaunchSizeSelected = true;
    CasualCore::Hardware::GetInstance()->SetSimulatedDevice(CasualCore::Hardware::CPU_MEDIUM, CasualCore::Hardware::GPU_MEDIUM, CasualCore::Hardware::MEM_HIGH, 2);
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_720p)
  {
    m_iGameWindowLaunchWidth = 1280;
    m_iGameWindowLaunchHeight = 720;
    m_bGameWindowLaunchSizeSelected = true;
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  if (hBut == m_hLaunchMode_1080p)
  {
    m_iGameWindowLaunchWidth = 1920;
    m_iGameWindowLaunchHeight = 1080;
    m_bGameWindowLaunchSizeSelected = true;
    DestroyLaunchPane();
    resolutionButtonPressed = true;
  }
  
  //--- application specific buttons ----
  int iItem;
  for (iItem=0; iItem<DBW_ITEM_MAX; iItem++)
  {
    DBWS_ItemData *pItem = &gDBWS_ItemDataTable[iItem];
    if (hBut == pItem->m_hwndItem)
    {
      if (pItem->m_pPressed)
        (*pItem->m_pPressed) = true;    //the app must reset it to false, when processed ..
      if (pItem->m_eventCallback)
        pItem->m_eventCallback(pItem->m_itemID, 1); //to test..
    }
  }
}


//-----------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwProcess_WM_COMMAND(WPARAM wParam, LPARAM lParam)
  //-----------------------------------------------------------------------------------------------------------
{
  int wmId;
  UINT codeNotify;

  wmId    = LOWORD(wParam); 
  codeNotify = HIWORD(wParam); 
  HWND hwndCtl = (HWND)lParam;

  if ( codeNotify==BN_CLICKED)
  {
    if (hwndCtl)
    {
      HWND hBut = hwndCtl;
      dbwProcess_BN_CLICKED(hBut);
      return;
    }
  }


  //if (wmId > DBWMENU_SCRSIZE_FIRST_ && wmId< DBWMENU_SCRSIZE_LAST_)
  //{
  //  int w=1024, h=768;
  //  switch(wmId)
  //  {
  //  case DBWMENU_SCRSIZE_480x320 : w=480; h=320; break;
  //  case DBWMENU_SCRSIZE_640x480 : w=640; h=480; break;
  //  case DBWMENU_SCRSIZE_800x480 : w=800; h=480; break;
  //  case DBWMENU_SCRSIZE_800x600 : w=800; h=600; break;
  //  case DBWMENU_SCRSIZE_854x480 : w=854; h=480; break;
  //  case DBWMENU_SCRSIZE_960x540 : w=960; h=540; break;
  //  case DBWMENU_SCRSIZE_1024x580: w=1024; h=580; break;
  //  case DBWMENU_SCRSIZE_1024x600: w=1024; h=600; break;
  //  case DBWMENU_SCRSIZE_1024x768: w=1024; h=768; break;
  //  case DBWMENU_SCRSIZE_1196x720: w=1196; h=720; break;
  //  case DBWMENU_SCRSIZE_1280x720: w=1280; h=720; break;
  //  case DBWMENU_SCRSIZE_1280x752: w=1280; h=752; break;
  //  case DBWMENU_SCRSIZE_1280x800: w=1280; h=800; break;
  //  }
  //  dbwGameScreenSize_Set(w,h);
  //}

  if (wmId == DBWMENU_MEMORY_DUMPALL)
    dbwMemory_DumpAll();

  if (wmId == DBWMENU_TEXTURE_DUMPALL)
    dbwTexture_DumpAll();

  if (wmId == DBWMENU_SCENE_DUMPALL)
    dbwScene_DumpAll();

  if (wmId == DBWMENU_LOG_FILTERS)
    wdbLogging_SetFilters();
  if (wmId == DBWMENU_LOG_VIEWTYPE_TEXT)
    wdbLogging_SetViewType(0);
  if (wmId == DBWMENU_LOG_VIEWTYPE_LIST)
    wdbLogging_SetViewType(1);

  if (wmId == DBWMENU_ASSERT_CHECKBOXES_SHOW)
    dbwAssert_EnableCheckBoxes(true);
  if (wmId == DBWMENU_ASSERT_CHECKBOXES_HIDE)
    dbwAssert_EnableCheckBoxes(false);
  if (wmId == DBWMENU_ASSERT_RESET_IGNORE)
  {
    gRKErrConf.ecClearIgnoreList();
    RKLOG("All 'Ignore always' RKAsserts have been cleared..");
  }
    
}


//--------------------------------------------------------------------------------------
void AppDebugWindow::dbwGameScreenSize_Set(int w, int h)
  //--------------------------------------------------------------------------------------
{
  WINDOWINFO wInfo;
  GetWindowInfo(m_hwndGameWindow, &wInfo);
  MoveWindow(m_hwndGameWindow, wInfo.rcWindow.left, wInfo.rcWindow.top, w, h, true);

  RKDevice_SetWidth(w);
  RKDevice_SetHeight(h);

  // [Liam] TODO - resize GameSWF
  /*
  RKLOG_WARN("dbwGameScreenSize_Set(%d,%d)... TODO..", w, h);
  //see CreateGameWindow(width, height) in PlatformWin32.cpp, Platform::Start() .. 
  //-> add m_GameWinX,..Y,W,H
  */
}

//--------------------------------------------------------------------------------------
void AppDebugWindow::dbwMemory_DumpAll(void)
  //--------------------------------------------------------------------------------------
{
  RKLOG("--- dbwMemory_DumpAll()--- TODO ...");
  RKLOG_TRACE("test RKLOG_TRACE.. ");
  RKLOG_INFO("test RKLOG_INFO.. ...");
  RKLOG_WARN("test RKLOG_WARN.. ");
  RKLOG_ERROR("test RKLOG_ERROR.. ");
  RKLOG_FATAL("test RKLOG_FATAL.. ...");
  RKLOG("testing file not found:");
  RKFile_Open("dummyFile.bin", RKFM_READ);      //test file not found log..
  RKLOG("test log NORMAL.. end of dbwMemory_DumpAll()");
}

//--------------------------------------------------------------------------------------
void AppDebugWindow::dbwTexture_DumpAll(void)
  //--------------------------------------------------------------------------------------
{
  RKLOG("dbwTexture_DumpAll().. TODO ...");
  //TODO: add get functions in RKTexture/ RKMaterial
}

//--------------------------------------------------------------------------------------
void AppDebugWindow::dbwScene_DumpAll(void)
  //--------------------------------------------------------------------------------------
{
  //RKLOG("dbwScene_DumpAll().. TODO ...");
  CasualCore::Scene* scene = CasualCore::Game::GetInstance()->GetScene();

  scene->Report();

  //uint32 objCount = scene->GetNumObjects();
  CasualCore::Scene::ObjectList* pObjList = scene->GetObjectList();
  int objCount = pObjList->Size();
  RKLOG_INFO("--- dbwScene_DumpAll(): objCount=%d ---", objCount);
  int i;
  for (i=0; i<objCount; i++)
  {
    CasualCore::Object* pObj = (*pObjList)[i];
    const char *name = pObj->GetName();
    const char *className = pObj->ClassName();
    const char *filename = pObj->GetFileName();

    RKLOG_INFO("#%d : name=%s, className=%s,  filename=<%s>", i, name, className, filename);
  }
}

//--------------------------------------------------------------------------------------
void AppDebugWindow::wdbLogging_SetFilters(void)
  //--------------------------------------------------------------------------------------
{
  RKLOG("wdbLogging_SetFilters().. TODO ...");
  //TODO : re-load CSV log file (m_logfileFullname) , and re-populate Text window (m_LogWinH, RichEdit) or List window [TODO]
}

//--------------------------------------------------------------------------------------
void AppDebugWindow::wdbLogging_SetViewType(int logViewType)
  //--------------------------------------------------------------------------------------
{
  RKLOG("wdbLogging_SetViewType().. TODO ...");
  //0:(m_LogWinH, RichEdit) or 1:List window [TODO]
}

//--------------------------------------------------------------------------------------
void _changeToBackslash(char *p)
  //--------------------------------------------------------------------------------------
{
  while (*p)
  {
    if (*p=='/')
      *p = '\\';
    p++;
  }
}
//--------------------------------------------------------------------------------------
void AppDebugWindow::dbwTextEdit(char *fullTextFilename)
  //--------------------------------------------------------------------------------------
{
  char processCmdLine[2048] = "";

  sprintf_SAFE(processCmdLine, sizeof(processCmdLine), "\"%s\" %s", m_textEditorFullFilename, fullTextFilename);
  _changeToBackslash(processCmdLine);

  dbwProcessStart(NULL, processCmdLine, NULL, false);
}

bool AppDebugWindow::dbwReadConfigFile(void)
{
  RKASSERT(strlen(m_debugConfigINIFullname), "m_debugConfigINIFullname is an empty string");

  bool bSuccess = true;

  FILE* pFile = fopen(m_debugConfigINIFullname, "rb");
  if (pFile)
  {
    fseek(pFile, 0, SEEK_END);
    uint32 iSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    char* pIniFileBuffer = new char[iSize+1]; // +1 for an extra null termination character, treating the data as a c string
    memset(pIniFileBuffer, 0, iSize+1);
    if (fread(pIniFileBuffer, iSize, 1, pFile) == 1)
    {
      //!>> Read in ini values -------------------------------------------------------
      enum EIniDataType { DT_BOOL, DT_INT, DT_STRING, };

      struct IniIntData
      {
        const char* pKey;
        void* pValue;
        EIniDataType eType;
      };

      IniIntData pIni[] = 
      {
        {"m_enableDebugWindow = ",  &m_enableDebugWindow,       DT_BOOL}, 
        {"m_mainDebugWinX = ",      &m_mainDebugWinX,           DT_INT}, 
        {"m_mainDebugWinY = ",      &m_mainDebugWinY,           DT_INT}, 
        {"m_mainDebugWinW = ",      &m_mainDebugWinW,           DT_INT}, 
        {"m_mainDebugWinH = ",      &m_mainDebugWinH,           DT_INT}, 
        {"m_LogWinH = ",            &m_LogWinH,                 DT_INT}, 
        {"m_CorePaneH = ",          &m_CorePaneH,               DT_INT}, 
        {"m_AppPaneH = ",           &m_AppPaneH,                DT_INT}, 
        {"m_enableLogToFile = ",    &m_enableLogToFile,         DT_BOOL}, 
        {"m_enableLogWithTime = ",  &m_enableLogWithTime,       DT_BOOL}, 
        {"TextEditor = ",           &m_textEditorFullFilename,  DT_STRING}, 
        {"m_enableAssertCheckBoxes = ",    &m_enableAssertCheckBoxes,    DT_BOOL}, 
        {"m_startupDisableAssertsAll = ",  &m_startupDisableAssertsAll,  DT_BOOL}, 
        {"m_startupDisableAssertsWarn = ", &m_startupDisableAssertsWarn, DT_BOOL}, 
      };

      char* pIt = 0;
      const uint32 iMaxValueSize = 256;
      char pValue[iMaxValueSize] = {0};
      for (uint32 i = 0, size = ARRAYSIZE(pIni); i < size; ++i)
      {
        pIt = strstr(pIniFileBuffer, pIni[i].pKey);
        if (pIt)
        {
          pIt += strlen(pIni[i].pKey);  // Move iterator to position of value
          for (uint32 j = 0; j < iMaxValueSize; ++j)
          {
            if (pIt[j] == '\n' || pIt[j] == '\r' || pIt[j] == '\0')
            {
              pValue[j] = '\0';
              break;
            }

            pValue[j] = pIt[j];
          }

          switch(pIni[i].eType)
          {
          case DT_BOOL:
              *((bool*)pIni[i].pValue) = atoi(pValue) > 0 ? true : false;
              break;
          case DT_INT:
            *((int*)pIni[i].pValue) = atoi(pValue);
            break;
          case DT_STRING:
            strcpy_s((char*)pIni[i].pValue, PATH_STRING_MAX, pValue);
            break;
          default:
            RKLOG_ERROR("Invalid ini data type");
            bSuccess = false;
            break;
          }
        }
        else
        {
          RKLOG_ERROR("Ini key: %s could not be found for debug window config file: %s", pIni[i].pKey, m_debugConfigINIFullname);
        }
      }

      // Check if text editor executable exists
      bool bTextEditorExeExists = false;
      if (0 == _access(m_textEditorFullFilename, 0))
      {
        struct _stat statbuf;
        _stat(m_textEditorFullFilename, &statbuf);
        bTextEditorExeExists =  !(_S_IFDIR & statbuf.st_mode);
      }

      if (bTextEditorExeExists)
      {
        m_textEditorIsNotepad = false;
      }
      else
      {
        char buffWinDir[260];
        GetWindowsDirectory(buffWinDir, sizeof(buffWinDir));
        sprintf_SAFE(m_textEditorFullFilename, sizeof(m_textEditorFullFilename), "%s\\notepad.exe", buffWinDir);
        m_textEditorIsNotepad = true;
      }

      gErrDisplayW32_forceDisableAllDialogs = m_startupDisableAssertsAll;
      gErrDisplayW32_forceDisableWarningDialogs = m_startupDisableAssertsWarn;
      if (m_startupDisableAssertsAll || m_startupDisableAssertsWarn)
        m_enableAssertCheckBoxes = true;
    }
    else
    {
      RKLOG_ERROR("Failed to read debug window config ini file");
      bSuccess = false;
    }

    delete[] pIniFileBuffer;
    pIniFileBuffer = 0;
    fclose(pFile);
  }
  else
  {
    RKLOG_ERROR("Failed to open debug window config ini file");
    bSuccess = false;
  }

  return bSuccess;
}

////--------------------------------------------------------------------------------------
//void AppDebugWindow::dbwReadConfigFile(void)
//  //--------------------------------------------------------------------------------------
//{
//  bool wasInited = RKFile_IsModuleInitialized();
//  if (wasInited==false)
//  {
//    RKFile_InitModule();  //mm not inited yet... need system files ...
//  }
//
//  RKIniFile* pINI = RKIniFile_Open(m_debugConfigINIFullname);
//  if (pINI==NULL)
//  {
//    dbwWriteConfigFile();
//    pINI = RKIniFile_Open(m_debugConfigINIFullname);
//    if (pINI == NULL)
//    {
//      RKWarning(0, "ooppss FAILED to reload INI file <%s>", m_debugConfigINIFullname);
//      return;
//    }
//  }
//
//  int valInt=0;
//  RKIniFile_GetInt(pINI, "m_enableDebugWindow", valInt, 1);
//  m_enableDebugWindow = (valInt!=0);
//
//  RKIniFile_GetInt(pINI, "m_mainDebugWinX", m_mainDebugWinX, 0);
//  RKIniFile_GetInt(pINI, "m_mainDebugWinY", m_mainDebugWinY, 0);
//  RKIniFile_GetInt(pINI, "m_mainDebugWinW", m_mainDebugWinW, 600);
//  RKIniFile_GetInt(pINI, "m_mainDebugWinH", m_mainDebugWinH, 500);
//
//  RKIniFile_GetInt(pINI, "m_LogWinH", m_LogWinH, 300);
//  RKIniFile_GetInt(pINI, "m_CorePaneH", m_CorePaneH, 100);
//  RKIniFile_GetInt(pINI, "m_AppPaneH", m_AppPaneH, 170);
//
//  valInt=0;
//  RKIniFile_GetInt(pINI, "m_enableLogToFile", valInt, 1);
//  m_enableLogToFile = (valInt!=0);
//
//  valInt=0;
//  RKIniFile_GetInt(pINI, "m_enableLogWithTime", valInt, 1);
//  m_enableLogWithTime = (valInt!=0);
//
//  RKString valString;
//  RKIniFile_GetString(pINI, "TextEditor", valString);
//  strcpy_SAFE(m_textEditorFullFilename, sizeof(m_textEditorFullFilename), valString.GetString());
//  if (RKFile_ExistsAbs(m_textEditorFullFilename))
//    m_textEditorIsNotepad = false;
//  else
//  {
//    char buffWinDir[260];
//    GetWindowsDirectory(buffWinDir, sizeof(buffWinDir));
//    sprintf_SAFE(m_textEditorFullFilename, sizeof(m_textEditorFullFilename), "%s\\notepad.exe", buffWinDir);
//    m_textEditorIsNotepad = true;
//  }
//
//  RKIniFile_Close(&pINI);
//
//  if (wasInited==false)
//  {
//    RKFile_DeinitModule();
//    //+ see Platform::Platform() : 
//
//#ifdef RKDEVICE_DX11
//#  define WIN32_WORKPATH "../dxdata"
//    _chdir(WIN32_WORKPATH);
//#else
////#  define WIN32_WORKPATH "../data"  // [Liam] is already set to trunk/data, this does nothing :P
//#endif
//
//    //_chdir(WIN32_WORKPATH);
//  }
//}

//--------------------------------------------------------------------------------------
void AppDebugWindow::dbwWriteConfigFile(void)
  //--------------------------------------------------------------------------------------
{
  FILE *pf = fopen(m_debugConfigINIFullname, "wb");
  if (pf==NULL)
  {
    RKWarning(0, "ooppss FAILED to save INI file <%s>", m_debugConfigINIFullname);
    return;
  }

  fprintf(pf, "//------ Config file <%s>, for <%s> -----\r\n", m_debugConfigINIFullname, __FILE__);
  fprintf(pf, "\r\n");

  fprintf(pf, "m_enableDebugWindow = %d\r\n", m_enableDebugWindow);
  fprintf(pf, "\r\n");

  fprintf(pf, "m_mainDebugWinX = %d\r\n", m_mainDebugWinX);
  fprintf(pf, "m_mainDebugWinY = %d\r\n", m_mainDebugWinY);
  fprintf(pf, "m_mainDebugWinW = %d\r\n", m_mainDebugWinW);
  fprintf(pf, "m_mainDebugWinH = %d\r\n", m_mainDebugWinH);
  fprintf(pf, "\r\n");

  fprintf(pf, "m_LogWinH = %d\r\n", m_LogWinH);
  fprintf(pf, "m_CorePaneH = %d\r\n", m_CorePaneH);
  fprintf(pf, "m_AppPaneH = %d\r\n", m_AppPaneH);
  fprintf(pf, "\r\n");

  ///++ todo:  fprintf(pf, "m_GameWinX = %d\r\n", m_GameWinX);  .. see dbwGameScreenSize_Set()

  fprintf(pf, "m_enableLogToFile = %d\r\n", m_enableLogToFile);
  fprintf(pf, "m_enableLogWithTime = %d\r\n", m_enableLogWithTime);
  fprintf(pf, "m_enableAssertCheckBoxes = %d\r\n", m_enableAssertCheckBoxes);
  fprintf(pf, "m_startupDisableAssertsAll = %d\r\n", m_startupDisableAssertsAll);
  fprintf(pf, "m_startupDisableAssertsWarn = %d\r\n", m_startupDisableAssertsWarn);
  fprintf(pf, "\r\n");

  fprintf(pf, "TextEditor = %s\r\n", "C:\\Program Files (x86)\\Notepad++\\notepad++.exe");   //m_textEditorFullFilename

  fclose(pf);
}


LPVOID m_lastErrorMessageBuffer = NULL;
//---static-----------------------------------------------------------------------------------------------------
char *MLW32_wGetLastErrorText(DWORD *pErrCode)
  //--------------------------------------------------------------------------------------------------------------
{
  if (m_lastErrorMessageBuffer)
  {
    LocalFree( m_lastErrorMessageBuffer );
    m_lastErrorMessageBuffer = NULL;
  }

  DWORD lastError = GetLastError();

  if (pErrCode)
    *pErrCode = lastError;

  if (!FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    lastError,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US), //SUBLANG_DEFAULT), // Default language
    (LPTSTR) &m_lastErrorMessageBuffer,
    0,
    NULL ))
  {
    // Handle the error.
    if (m_lastErrorMessageBuffer==NULL)
    {
      m_lastErrorMessageBuffer = LocalAlloc(0, 1024);
      sprintf_SAFE( (char*)m_lastErrorMessageBuffer, 1024, "could not get FormatMessage() for err=%X ...\n", lastError);
    }
  }

  return (char*)m_lastErrorMessageBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
int AppDebugWindow::dbwProcessStart(char *exeFullName, char *cmdLine, char *currentDir, bool addExeInCommandLine)
  //----------------------------------------------------------------------------------------------------------------------
{
  int ret = 0;

  PROCESS_INFORMATION procInfo;
  STARTUPINFO startInfo;
  memset(&startInfo, 0, sizeof(STARTUPINFO));
  startInfo.cb = sizeof(STARTUPINFO);

  memset(&procInfo, 0, sizeof(PROCESS_INFORMATION));

  SECURITY_ATTRIBUTES *p_secuAttr = NULL;
  SECURITY_ATTRIBUTES *p_threadAttrb = NULL;
  BOOL doIneritHandles = FALSE;
  DWORD dwCreationFlags = 0;
  void *p_environ = NULL;
  LPCTSTR lpCurrentDirectory = NULL;

  if (currentDir == NULL)
    lpCurrentDirectory = m_applicationPath; 
  else
    lpCurrentDirectory = currentDir;

  char fullCommandLine[2048];
  if (addExeInCommandLine == true)
  {
    sprintf_SAFE(fullCommandLine, sizeof(fullCommandLine), "%s %s", exeFullName, cmdLine);
  }
  else
    strcpy_AUTO(fullCommandLine, cmdLine);

  BOOL createOK = CreateProcess(exeFullName, fullCommandLine, p_secuAttr, p_threadAttrb, doIneritHandles, dwCreationFlags, p_environ, lpCurrentDirectory, &startInfo, &procInfo);	//&lpStartupInfo, lpProcessInformation);

  if (createOK)
  {
    ret = procInfo.dwProcessId;
  }
  else
  {
    DWORD errCode;
    char *errText = MLW32_wGetLastErrorText(&errCode);
    RKLOG_WARN("CreateProcess(<%s>, <%s>) FAILED!, err=%X\n<%s>", exeFullName, fullCommandLine, errCode, errText);
    LocalFree( m_lastErrorMessageBuffer );  //fff
    m_lastErrorMessageBuffer = NULL;
  }
  return ret;
}


//----------------------------------------------------------------------------------------------------------------------
void AppDebugWindow::dbwAddApplicationButtonAuto(bool *pPressed, char *butTxt)
  //----------------------------------------------------------------------------------------------------------------------
{
  //a simple way for user application to add app specific buttons...

  if (gDBWS_ItemDataCount >= DBW_ITEM_MAX)
  {
    RKWarning(0, "dbwAddApplicationButtonAuto() : gDBWS_ItemDataCount=%d!!", gDBWS_ItemDataCount);
    return;
  }

  HWND hParent = m_hwndApplicationToolPane;
  int x = gDBWS_currentX;
  int y = gDBWS_currentY;
  int w = 100;
  int h = 18;

  DBWS_ItemData *pItem = &gDBWS_ItemDataTable[gDBWS_ItemDataCount];
  pItem->m_pPressed = pPressed;

  pItem->m_hwndItem = dbwAddButton(hParent, butTxt, x, y, w,h);

  gDBWS_ItemDataCount++;
  gDBWS_currentX += w;
  if (gDBWS_currentX + w> m_mainDebugWinW)
  {
    gDBWS_currentX = 0;
    gDBWS_currentY += h;
  }
}

void AppDebugWindow::WaitForLaunchSizeSelection()
{
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  unsigned int count = 0;

  float updateInterval = 0.2f;
  float tStart = _GetGlobalUpTimeSafe();
  float tNextUpdate = tStart + updateInterval;
  float tElapsedFromStart = 0;
  float tSelectionTimeout = 10.0f;

  while (!m_bGameWindowLaunchSizeSelected)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    if (true) //m_enableSizeSelectionTimeout)
    {
      float tNow = _GetGlobalUpTimeSafe();
      if (tNow > tNextUpdate)
      {
        tElapsedFromStart = tNow - tStart;
        tNextUpdate = tNow + updateInterval;
        char newTitleTxt[256];
        float tRemain = tSelectionTimeout - tElapsedFromStart;
        if (tRemain < 0)
        {
          RKLOG("Size selection Cancelled (time out) -> Using default size..");
          SetWindowText(m_hLauchSizeTitle, "Default resolution Selected..");
          DestroyLaunchPane();
          dbwFlushLog();  //force update win items
          break;
        }
        sprintf_SAFE(newTitleTxt, sizeof(newTitleTxt), "%s (default in %0.1f sec)", s_MsgSelectResolution, tRemain);
        //char please m_hLauchSizeTitle  s_MsgSelectResolution
        SetWindowText(m_hLauchSizeTitle, newTitleTxt);
      }
      /*
      ++count;
      if (count >= 2147483646)
      {
        RKLOG("Size selection Cancelled (time out) -> Using default size..");
        break;
      }
      */
    }
  }
}

#endif //CC_USE_DEBUG_WINDOW


