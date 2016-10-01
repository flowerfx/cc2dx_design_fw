//*****************************************************************************************************************************************************
//*
//* Win32DebugWindow.h
//*
//* Win32 debug window for Casual Core application
//*
//* Christophe Le Bouil
//*****************************************************************************************************************************************************
#ifndef _WIN32_DEBUG_WINDOW_H_
#define _WIN32_DEBUG_WINDOW_H_

#ifdef _NO_CC_DBGWIN                //set in project settings for tools, etc
#define CC_USE_DEBUG_WINDOW      0   //0 or 1 - to move in some configuration header ...
#else
#define CC_USE_DEBUG_WINDOW      1   //0 or 1 
#endif

#if CC_USE_DEBUG_WINDOW==1

typedef void FuncItemEventCallback(int itemID, int eventID);

#define PATH_STRING_MAX 2048

//------------------------------------------------------------------------------------------------
class AppDebugWindow
{
public:
  static void dbwRedirectStdio(int argc, char** argv);

  static void dbwCreate(HWND hGameWindow, int argc, char** argv);
  static void dbwDestroy(void);
  static AppDebugWindow* dbwGetInstance(void) { return m_instance_AppDebugWindow; }
  static bool dbwIsCreated();
  static HWND dbwGetMainHWND(void);
  static HMENU dbwGetMainMenu(void);
  static void dbwUpdate(void);

  static void dbwFlushLog(void);

  void dbwAddApplicationButtonAuto(bool *pPressed, char *butTxt);       //a simple way for use application to add app specific buttons ..

  HWND dbwGetHwnd_MainDebugWin() { return m_hMainDebugWin; }
  HWND dbwGetHwnd_LogWin() { return m_hwndLogWin; }
  HWND dbwGetHwnd_CasualCoreToolPane() { return m_hwndCasualCoreToolPane; }
  HWND dbwGetHwnd_ApplicationToolPane() { return m_hwndApplicationToolPane; }

  void WaitForLaunchSizeSelection();
  inline void SetGameWindowHandle(HWND hwnd)        { m_hwndGameWindow = hwnd; }
  inline bool GameWindowLaunchSizeSelected() const  { return m_bGameWindowLaunchSizeSelected; }
  inline void GetSelectedLaunchSize(int& iOutWidth, int& iOutHeight) const  { iOutWidth = m_iGameWindowLaunchWidth; iOutHeight = m_iGameWindowLaunchHeight; }
  //--- internal ---
  static HWND dbwAddPaneWindow(HWND hParent, char *txt, int x, int y, int w, int h);

  static HWND dbwAddStaticText(HWND hParent, const char *txt, int x, int y, int w, int h);
  static HWND dbwAddButton(HWND hParent, char *txt, int x, int y, int w, int h);
  static HWND dbwAddCheckbox(HWND hParent, char *txt, int x, int y, int w, int h);

  void dbwLogMessage(int ilogLevel, const char *logType, const char* file, long line, const char* function, const char *message);
  LRESULT CALLBACK dbwProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  void DestroyLaunchPane();

private:
  AppDebugWindow() { dbwZero(); }
  void dbwZero(void);
  void dbwInit(HWND hParent, int argc, char** argv);
  void CreateLaunchPane();
  void ApplyFilterString();
  void dbwReleaseAll(void);

  bool dbwReadConfigFile(void);
  void dbwWriteConfigFile(void);

  HMENU dbwCreateMenu(void);
  void dbwCreateRichEdit();
  void dbwAddFilterString(HWND hParent, int x, int y);
  void dbwAddCoreToolPaneItems(void);

  void dbwFrameUpdate(void);

  void dbwProcess_WM_COMMAND(WPARAM wParam, LPARAM lParam);
  void dbwProcess_BN_CLICKED(HWND hBut);
  void dbwTextEdit(char *fullTextFilename);
  int dbwProcessStart(char *exeFullName, char *cmdLine, char *currentDir, bool addExeInCommandLine);

  void dbwGameScreenSize_Set(int w, int h);
  void dbwMemory_DumpAll(void);
  void dbwTexture_DumpAll(void);
  void dbwScene_DumpAll(void);
  void wdbLogging_SetFilters(void);
  void wdbLogging_SetViewType(int logViewType);

  void dbwAddMessageLogLine(const char *message, COLORREF msgColor);  

  void dbwAssert_SetParent(HWND hParent, int x, int y);
  void dbwAssert_AddCheckBoxes(void);
  void dbwAssert_RemoveCheckBoxes(void);
  void dbwAssert_EnableCheckBoxes(bool enab);

  static AppDebugWindow *m_instance_AppDebugWindow;

  HWND m_hMainDebugWin;
  HWND m_hwndLogWin;
  HWND m_hwndEditField;
  HWND m_hwndCasualCoreToolPane;
  HWND m_hwndApplicationToolPane;

  HWND m_hwndGameWindow;

  //---
  int m_y0Items;
  HWND m_hwndFrameInfo;
  HWND m_hwndMemoryInfo;
  HWND m_hwndButtonDumpScene;
  HWND m_hwndButtonLogMemory;
  HWND m_hwndButtonEditLogfile;
  HWND m_hwndButtonEditInifile;
  HWND m_hwndButtonTestAssert;
  HWND m_hwndButtonKeyMenu;
  HWND m_hwndButtonKeyHomeShort;
  HWND m_hwndButtonKeyHomeLong;
  HWND m_hwndButtonKeyBack;
  HWND m_hwndButtonFilterApply;
  HWND m_hwndCheckbox_disableALLDial;
  HWND m_hwndCheckbox_disableWarningDial;

  HWND m_assertCheckBoxes_hParent;
  int m_assertCheckBoxes_x;
  int m_assertCheckBoxes_y;

  HMENU m_hMainMenu;
  HMENU m_hSubMenuScreenSize;
  HMENU m_hSubMenuMemory;
  HMENU m_hSubMenuTexture;
  HMENU m_hSubMenuScene;
  HMENU m_hSubMenuLogging;
  HMENU m_hSubMenuAssert;

  char m_applicationPath[PATH_STRING_MAX];
  char m_applicationShortName[128];

  char m_debugConfigINIFullname[PATH_STRING_MAX];

  char m_logfileFullname[PATH_STRING_MAX];
  bool m_textEditorIsNotepad;

  //--- for INI file ---
  bool m_enableDebugWindow;
  int m_mainDebugWinX;
  int m_mainDebugWinY;
  int m_mainDebugWinW;
  int m_mainDebugWinH;

  int m_LogWinH;  //m_richEditH;
  int m_CorePaneH;
  int m_AppPaneH;

  bool m_enableLogToFile;
  bool m_enableLogWithTime;
  char m_textEditorFullFilename[PATH_STRING_MAX];
  bool m_enableAssertCheckBoxes;
  bool m_startupDisableAssertsAll;
  bool m_startupDisableAssertsWarn;

  // [Liam] GameWindow Launch Size -------------------------------------
  HWND m_hLaunchSizePane;
  int m_iLaunchPaneHeight;
  HWND m_hLauchSizeTitle;
  HWND m_hLaunchMode_IPHONE3GS;
  HWND m_hLaunchMode_IPHONE4;
  HWND m_hLaunchMode_IPHONE4S;
  HWND m_hLaunchMode_IPHONE5;
  HWND m_hLaunchMode_IPHONE5S;
  HWND m_hLaunchMode_IPAD1;
  HWND m_hLaunchMode_IPAD2;
  HWND m_hLaunchMode_IPAD3;
  HWND m_hLaunchMode_720p;
  HWND m_hLaunchMode_1080p;

  int  m_iGameWindowLaunchWidth;
  int  m_iGameWindowLaunchHeight;
  bool m_bGameWindowLaunchSizeSelected;

  struct LogLine
  {
    COLORREF color;
    const char* pText;
  };
  DWORD m_owningThreadId;
  RKCriticalSection* m_pFileLogCS;
  RKCriticalSection* m_pPendingLogLinesCS;
  RKList<LogLine> m_pendingLogLines;
  RKString  m_linesFilter;
};

#endif //CC_USE_DEBUG_WINDOW

#endif //_WIN32_DEBUG_WINDOW_H_
