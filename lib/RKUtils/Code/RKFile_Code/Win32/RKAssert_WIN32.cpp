//**************************************************************************************************************************************
//*
//* RKAssert_WIN32.cpp
//*
//**************************************************************************************************************************************
#define _WIN32_WINNT 0x0500		//for IsDebuggerPresent
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <RKString.h>
#include <RKAssert.h>
#include <RKLog.h>
#include <RKStdSAFE.h>

#define MLQA_ENABLE_OUTPUTIDE 1
/*
static char *tblErrorTypeName[RKERRT_LAST_+1] =		//make sure to match the enum ... see RKERRT_LAST_
{
	"None",
	"WARNING",
	"VERIFY",
	"ASSERT",
	"FATAL",

	"UNKNOWN"	//RKERRT_LAST_
};
*/


//extern char MLError_Buffer[];
#define ERR_BUF_MAX 4100
char RKError_Buffer[ERR_BUF_MAX]="";  //char MLError_Buffer[ERR_BUF_MAX]="";




extern bool gRKAssertWantBreakpoint;
static int errDisplay_RecurseLevel = 0;

bool gErrDisplayW32_forceDisableAllDialogs = false;       //to be set by Win32DebugWindow.cpp ...
bool gErrDisplayW32_forceDisableWarningDialogs = false;


//---- misc tools
char* RKAssert_fullNameToFilename(char *fullname, char *destFilename, int sizeofDest);		//remove path



		    //
//-------------------------------------------------------------------------------------------------------------
HINSTANCE RKAssertGetAppHInstance(void)
//-------------------------------------------------------------------------------------------------------------
{
  //instead of MLApplication_PC::getInstance();	
  return GetModuleHandle(NULL); //returns the instance handle.;
}

HWND RKAssert_hwndMain = NULL;

BOOL CALLBACK RKAssert_EnumWindowProc(HWND hwnd, LPARAM lParam)
{
    HINSTANCE hinst=(HINSTANCE)GetModuleHandle(NULL);

    if((HINSTANCE)GetWindowLongPtr(hwnd, GWL_HINSTANCE)==hinst &&
        IsWindowVisible(hwnd))
    {
        RKAssert_hwndMain=hwnd;
        return FALSE;
    }
    else
        return TRUE;
}

HWND RKAssert_GetAppHWND(void)
{
  //MLApplication_PC::getHwnd()
  if (RKAssert_hwndMain)
    return RKAssert_hwndMain;

  EnumWindows(RKAssert_EnumWindowProc, 0);

  return RKAssert_hwndMain;
}
//-------------------------------------------------------------------------------------------------------------
bool RKAssert_IssRunningFromDebugger(void)
//-------------------------------------------------------------------------------------------------------------
{
//MLCore::RunFromDebugger()
	BOOL grrr = IsDebuggerPresent();
  return (grrr==TRUE);
}

//*************************************************************************************************************
//misc
//-static------------------------------------------------------------------------------------------------------
//char* MLFile::fullNameToFilename(char *fullname, char *destFilename, int sizeofDest)		//remove path
const char* RKAssert_fullNameToFilename(const char *fullname, char *destFilename, int sizeofDest)		//remove path
//-------------------------------------------------------------------------------------------------------------
{
	if (fullname==NULL)
	{
		if (destFilename)
			destFilename[0] = 0;
		return destFilename;
	}
	const char *p = fullname + strlen(fullname)-1;
	while (p>fullname && *p != '\\' && *p != '/' )
		p--;

	//MLWarning(0)("temp strcpy_REALLY");

	if (*p == '\\' || *p == '/')
		p++;
	
	if (destFilename)
	{
		strcpy_SAFE(destFilename, sizeofDest, p);
		return destFilename;
	}
	return p;
}




//**************************************************************************************************************************************************
//* from MLError_PC.cpp

//***************************************************************************************
//***************************************************************************************

static HINSTANCE m_hInstRichEdit20 = NULL;


#define MLERRORDIAL_W 500
#define MLERRORDIAL_H 100
#define MLERRORDIAL_H2 200


static bool gEnableAssertLogging = true;
static bool gEnableAssertVC6Output = true;
//bool enableAlertDisplay = true;

static int dialBoxWantedPosX = -999;
static int dialBoxWantedPosY = -999;

static int gMoreSettingFlag = 0;

static bool gErr_SoftQuitRequested = false;

enum { DIALBOX_ERRORCLOSE_NONE,  DIALBOX_ERRORCLOSE_QUIT, DIALBOX_ERRORCLOSE_IGNORE, DIALBOX_ERRORCLOSE_BREAKPOINT};

static HWND hButIgnoreOnce, hButIgnoreAlways, hButIgnoreType, hButIgnoreSource, hButIgnoreAll;
static HWND hButIgnoreAlwaysAnyVar	= NULL;
static HWND hButQuit								= NULL;
static HWND hButMoreSettings				= NULL;
static HWND hButBreak							= NULL;
static HWND hButLogMemory					= NULL;
static HWND hButClearIgnoreList		= NULL;
static HWND hButAddUserMessage			= NULL;

static HWND hTextTop								= NULL;
static HWND hTextMessage						= NULL;		//stay NULL if no rich edit
static HWND hwndRichedit						= NULL;

static HWND hCheckLogging					= NULL;
static HWND hCheckShowAlert				= NULL;
static HWND hCheckVC6Output				= NULL;

static HWND hCheckTypeOutFlag[RKERRT_LAST_][3];

static char *gTempAssertText				= NULL;

#if MLQA_ADD_EXTRA_SOFT_QUIT_BUTTON==1
static HWND hButQuit_Soft					= NULL;
#endif

RKASError gCurrenteError;
static RKASError *pErr = &gCurrenteError; //NULL;

extern const char *gRKTblErrorTypeName[RKERRT_LAST_+1];// = 
/*{
	"None",
	"ASSERT",
	"VERIFY",
	"WARNING",
	"FATAL",

	"UNKNOWN"	//RKERRT_LAST_
};
*/
static HWND RKErrorDial_hWnd = NULL;

static int MLError_DialBoxActivated = 0;


//--- some externs, durty, but for error processing we prefer direct access ...
//extern int MLDxApplication_ForcePresentOnPaint;	//tempForcePresentOnPaint;
//extern HINSTANCE m_hInstRichEdit20;		//dll : loaded by / freed by diMainQuit
extern char RKError_Buffer[]; //MLError_Buffer[];




//***************************************************************************************************************************************
//--- Win32 helpers ---
//---------------------------------------------------------------------------------------------------------------------------------------
void RKErrorDial_SetFont(HWND hItem)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HFONT hFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
	SendMessage( hItem, WM_SETFONT, (WPARAM)hFont, TRUE );				// Tell the control to use the new font.
}




//---------------------------------------------------------------------------------------------------------------------------------------
HWND RKErrorDial_AddButton(char *text, int x, int y, int w, int h)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HWND hWnd = RKErrorDial_hWnd;
	HINSTANCE instance = RKAssertGetAppHInstance();			    //GetModuleHandle(NULL); returns the instance handle.

	HWND hNewBut = CreateWindow("BUTTON" , text,		BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP   , x, y, w, h, hWnd, NULL, instance, NULL);
	RKErrorDial_SetFont(hNewBut);

	return hNewBut;
}

//---------------------------------------------------------------------------------------------------------------------------------------
HWND RKErrorDial_AddCheck(char *txt, int x, int y, int w, int h)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HWND hWnd = RKErrorDial_hWnd;
	HINSTANCE instance = RKAssertGetAppHInstance();			

	HWND hNewCheck = CreateWindow("BUTTON" , txt,		BS_AUTOCHECKBOX |WS_CHILD|WS_VISIBLE   , x,y,w,h, hWnd, NULL, instance, NULL);

	RKErrorDial_SetFont(hNewCheck);
	return hNewCheck;
}


//---------------------------------------------------------------------------------------------------------------------------------------
HWND RKErrorDial_AddStaticText(char *txt, int x, int y, int w, int h)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HWND hWnd = RKErrorDial_hWnd;
	HINSTANCE instance = RKAssertGetAppHInstance();			
	HWND newHandle = CreateWindow("STATIC" , txt,		SS_LEFT|WS_CHILD|WS_VISIBLE   , x,y,w,h, hWnd, NULL, instance, NULL);

	RKErrorDial_SetFont(newHandle);
	return newHandle;
}



//SS_BLACKFRAME SS_WHITEFRAME SS_WHITERECT SS_ETCHEDFRAME
//---------------------------------------------------------------------------------------------------------------------------------------
HWND RKErrorDial_AddStaticBox(int x, int y, int w, int h)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HWND hWnd = RKErrorDial_hWnd;
	HINSTANCE instance = RKAssertGetAppHInstance();			
	HWND newHandle = CreateWindow("STATIC" , NULL,		SS_ETCHEDFRAME   |WS_CHILD|WS_VISIBLE   , x,y,w,h, hWnd, NULL, instance, NULL);

//	RKErrorDial_SetFont(newHandle);
	return newHandle;
}


//***************************************************************************************************************************************



//---------------------------------------------------------------------------------------------------------------------------------------
void RKErrorDial_addItems(void)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HWND hWnd = RKErrorDial_hWnd;
	HINSTANCE instance = RKAssertGetAppHInstance();			
	int x,y,w,h;

	//--- should set all handles to NULL ... TODO
	hButIgnoreOnce				= NULL;
	hButIgnoreType				= NULL;
	hButIgnoreSource			= NULL;
	hButIgnoreAll					= NULL;
	hButQuit							= NULL;
#if MLQA_ADD_EXTRA_SOFT_QUIT_BUTTON==1
	hButQuit_Soft					= NULL;
#endif
	hButMoreSettings			= NULL;
	hButBreak							= NULL;
	hButLogMemory					= NULL;
	hButClearIgnoreList		= NULL;

	hTextTop							= NULL;
	hTextMessage					= NULL;
	hwndRichedit					= NULL;

	hCheckLogging					= NULL;
	hCheckShowAlert				= NULL;
	hCheckVC6Output				= NULL;


	if (RKAssert_IssRunningFromDebugger())
	{
		if (pErr->m_type==RKERRT_LAST_)
		{
			char *p=0; *p=0;	//TEMP!!!
		}
	}


	//--- "top" text : error type 
	hTextTop = CreateWindow("STATIC" , gRKTblErrorTypeName[pErr->m_type],		SS_LEFT|WS_CHILD|WS_VISIBLE   , 10, 0, 400, 50, hWnd, NULL, instance, NULL);


	//--- add the buttons -------
	x=10; y=130; w=90; h=20;

	w=50;
	bool addQuit = true;
	if (pErr->m_type == RKERRT_WARNING && gRKErrConf.ecAllowQuitBut(RKERRT_WARNING)==false)
		addQuit = false;

	if (addQuit)
	{
		hButQuit = RKErrorDial_AddButton("Quit", x,y,w,h);
		#if MLQA_ADD_EXTRA_SOFT_QUIT_BUTTON==1
		hButQuit_Soft = hButQuit;
		hButQuit = RKErrorDial_AddButton("EXIT", x,y+25,w,h-4);
		#endif
		x+=60;
	}
	
	w=90;
	if (pErr->m_type != RKERRT_FATAL)	// || MLCore::RunFromDebugger())		//no ignore for Fatals ... except for "us"
	{
		hButIgnoreOnce = RKErrorDial_AddButton("Ignore Once", x,y,w,h);
		x+=100;

		//x=200; y=y-15;
		hButIgnoreAlways	= RKErrorDial_AddButton("Ignore Always"	, x,     y    , w,h);
		x+=100;

/*
		hButIgnoreType		= RKErrorDial_AddButton("Ignore Type"		, x,     y+30 , w,h);
		hButIgnoreSource	= RKErrorDial_AddButton("Ignore Source"	, x+100, y    , w,h);
		hButIgnoreAll			= RKErrorDial_AddButton("Ignore All !"	, x+100, y+30 , w,h);
*/
	}

	x=550; y=130;
	if (RKAssert_IssRunningFromDebugger())
		hButBreak = RKErrorDial_AddButton("BreakPoint", x,y,w,h);
	else
		hButBreak = RKErrorDial_AddButton("Debug"			, x,y,w,h);

	x+=100;
	hButMoreSettings = RKErrorDial_AddButton("More...", x,y,w,h);



	//------- add the error message -----------
	
	x=10; y=30; w=730; h=80;
	hwndRichedit = CreateWindowEx(WS_EX_CLIENTEDGE, 	"RichEdit20A" , "",		ES_LEFT |ES_MULTILINE |	ES_READONLY | WS_VSCROLL | 	WS_CHILD|ES_AUTOVSCROLL|WS_VISIBLE   , x,y,w,h, hWnd, NULL, instance, NULL);

	if (hwndRichedit)		// rem Rich Edit 2.0 has both ANSI and Unicode window classes RichEdit20A and RichEdit20W, ...  need /see LoadLibrary
	{
		SetWindowText(hwndRichedit, gTempAssertText);
		SendMessage(hwndRichedit, EM_SCROLLCARET, 0, 0);
		//SetFocus(hwndRichedit);
		RKErrorDial_SetFont(hwndRichedit);
	}
	else //if (hwndRichedit == NULL)	//because dll cound not be loaded ?   
	{
		hTextMessage = CreateWindow("STATIC" , gTempAssertText,		SS_LEFT|WS_THICKFRAME|WS_CHILD|WS_VISIBLE   , x,y,w,h, hWnd, NULL, instance, NULL);
		RKErrorDial_SetFont(hTextMessage);
	}

	if (hButIgnoreOnce)
	{
		SetWindowLong(hButIgnoreOnce, GWL_STYLE,  BS_DEFPUSHBUTTON	| WS_CHILD|WS_VISIBLE|WS_TABSTOP ); //default
		SetFocus(hButIgnoreOnce);
	}
	else if (hButQuit)
	{
		HWND butToFocus = hButQuit;
		#if MLQA_ADD_EXTRA_SOFT_QUIT_BUTTON==1
		if (hButQuit_Soft)
			butToFocus = hButQuit_Soft;
		#endif
		SetWindowLong(butToFocus, GWL_STYLE,  BS_DEFPUSHBUTTON	| WS_CHILD|WS_VISIBLE|WS_TABSTOP ); //default
		SetFocus(butToFocus);
	}


}

#define HEIGHT_MOREITEMS 190
static int MoreItems_alreadyAdded = 0;
//---------------------------------------------------------------------------------------------------------------------------------------
void RKErrorDial_addMoreItems(bool bAdd)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	HWND hWnd = RKErrorDial_hWnd;
	int x,y,w,h;

	RECT nowPos;
	GetWindowRect(hWnd, &nowPos);
	int dx,dy;

	if (bAdd == false)
	{
		gMoreSettingFlag = 0;
		dy = -HEIGHT_MOREITEMS;
		SetWindowPos(hWnd, 0, 0,0, nowPos.right - nowPos.left, nowPos.bottom-nowPos.top + dy , SWP_NOMOVE|SWP_NOZORDER);
		return;
	}
	else
	{
		gMoreSettingFlag = 1;
		dy = HEIGHT_MOREITEMS;
		SetWindowPos(hWnd, 0, 0,0, nowPos.right - nowPos.left, nowPos.bottom-nowPos.top + dy , SWP_NOMOVE|SWP_NOZORDER);

		if (MoreItems_alreadyAdded==1)
			return;
	}

	MoreItems_alreadyAdded = 1;

	//--- add the CheckBoxs ---------
	// "Ignoring will assign thoses flags to this error/source/type/all"
	x=600; y=210; w=80; h=25;

	x=500;
	y=190;
	RKErrorDial_AddStaticText("Ignoring will assign thoses flags", x,y, 300, 20 );
	y+=18;
	RKErrorDial_AddStaticText("to this error/source/type/all", x,y, 300, 20 );
	y+=18;

	x=660;
	y=180;
	//w=60;
	//if (pErr->m_type != RKERRT_FATAL)		//can't ignore FATALs
	{
		hCheckLogging = RKErrorDial_AddCheck("Log", x,y,w,h);
		y+=18;
		if (RKAssert_IssRunningFromDebugger())
		{
			hCheckVC6Output = RKErrorDial_AddCheck("Ide",x,y,w,h);
			y+=18;
		}

		hCheckShowAlert = RKErrorDial_AddCheck("Show Alert", x,y,w,h);
	}

	//--- set the CheckBox State ----------
	if (pErr->m_oFlags & rkoLOG)
		SendMessage( hCheckLogging,  BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage( hCheckLogging,  BM_SETCHECK, BST_UNCHECKED, 0);


	if (pErr->m_oFlags & rkoIDE)
		SendMessage( hCheckVC6Output,  BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage( hCheckVC6Output,  BM_SETCHECK, BST_UNCHECKED, 0);

	if (0)	//ignoring : at least no Dialog !   pErr->m_oFlags & rkoDISP)
		SendMessage( hCheckShowAlert,  BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage( hCheckShowAlert,  BM_SETCHECK, BST_UNCHECKED, 0);

	//--- add other buttons --------
	int y0 = 186;
	dy = 25;
	x=10; y=y0; w=90; h=20;
	hButIgnoreAlwaysAnyVar = RKErrorDial_AddButton("Ig Always(any var)", x,y,w,h);

	y=y0+dy+dy;
	hButLogMemory = RKErrorDial_AddButton("Log Memory", x,y,w,h);

	x+=100;

	y=y0;
	hButIgnoreSource	= RKErrorDial_AddButton("Ignore Source"	, x,y , w,h);
	y+=dy;

	hButIgnoreType		= RKErrorDial_AddButton("Ignore Type"		, x,y , w,h);
	y+=dy;

	hButIgnoreAll			= RKErrorDial_AddButton("Ignore All !"	, x,y , w,h);
	y+=dy;

	x+=100;
	y=y0+dy;
	if (hButIgnoreOnce==NULL && gRKErrConf.ecAllowIgnoreBut(pErr->m_type)==true)
	{
		hButIgnoreOnce = RKErrorDial_AddButton("Ignore Once !", x,y,w,h);
		x+=100;
	}

	hButClearIgnoreList = RKErrorDial_AddButton("Clear Ignore list", x,y,w,h);
	y+=dy;

	x+=130; w=140;
	hButAddUserMessage = RKErrorDial_AddButton("Add Message to Logfile...", x,y,w,h);

	//--- add a separation box ...
	RKErrorDial_AddStaticBox(4, 176, 740, 86);


	//****** Type info view / setting ******
	y0 = 268;
	dy = 18;
	dx = 80;
	x=10; y=y0+dy;
	RKErrorDial_AddStaticText("TOTAL", x,y, w, h);
	y+=dy;
	RKErrorDial_AddStaticText("Log", x,y, w, h);
	y+=dy;
	RKErrorDial_AddStaticText("Ide", x,y, w, h);
	y+=dy;
	RKErrorDial_AddStaticText("Disp", x,y, w, h);
	y+=dy;


	x=100; y=y0;
	RKErrorDial_AddStaticText("Warning", x,y, w, h);
	x+=dx;
	RKErrorDial_AddStaticText("Verify", x,y, w, h);
	x+=dx;
	RKErrorDial_AddStaticText("Assert", x,y, w, h);
	x+=dx;
	RKErrorDial_AddStaticText("Fatal", x,y, w, h);
	x+=dx;

	x=100; y=y0+dy;
	char bufferNum[32];
	RKErrTypeConf *type_conf;
	int n;
	char *yesNo;
	
	for (n=RKERRT_WARNING; n<=RKERRT_FATAL; n++)
	{
		type_conf = gRKErrConf.ecGetTypeConfiguration(n);
		sprintf_SAFE(bufferNum, sizeof(bufferNum), "%d", type_conf->m_ErrorCount);
		RKErrorDial_AddStaticText(bufferNum, x,y, w, h);

		if ((type_conf->m_QASettingsOutputFlags & rkoLOG))		yesNo = "y"; else yesNo = "-";
		y+=dy;
		RKErrorDial_AddStaticText(yesNo, x,y, w, h);
		hCheckTypeOutFlag[n][0] = RKErrorDial_AddCheck("", x+17, y, 20,20);

		if ((type_conf->m_QASettingsOutputFlags & rkoIDE))		yesNo = "y"; else yesNo = "-";
		y+=dy;
		RKErrorDial_AddStaticText(yesNo, x,y, w, h);
		hCheckTypeOutFlag[n][1] = RKErrorDial_AddCheck("", x+17, y, 20,20);

		if ((type_conf->m_QASettingsOutputFlags & rkoDISP))		yesNo = "y"; else yesNo = "-";
		y+=dy;
		RKErrorDial_AddStaticText(yesNo, x,y, w, h);
		hCheckTypeOutFlag[n][2] = RKErrorDial_AddCheck("", x+17, y, 20,20);
		
		y-=3*dy;	
		x+=dx;
	}



	//--- misc
	x=600; y=y0; w=90; h=20; dy=25;
HWND hhh = RKErrorDial_AddButton("Lib Infos...", x,y,w,h);		//TODO show all QA flags, version, compile time, etc  ---> + TODO : log the AQ flags in MainInit !!
y+=dy;
HWND hhh2 = RKErrorDial_AddButton("edit Log file...", x,y,w,h);		//TODO 
y+=dy;
HWND hhh3 = RKErrorDial_AddButton("Take ScreenShots...", x,y,w,h);		//TODO 
y+=dy;
HWND hhh4 = RKErrorDial_AddButton("send logfile...", x,y,w,h);		//TODO 
y+=dy;
HWND hhh5 = RKErrorDial_AddButton("email to emory@uniconpro.co.jp...", x,y,w,h);		//TODO  :) 




	InvalidateRect(RKErrorDial_hWnd, NULL, false);	//mmm last texts not well displayed the first time ...

}


//---------------------------------------------------------------------------------------------------------------------------------------
static int readIgnoreCheckBox(void)
//---------------------------------------------------------------------------------------------------------------------------------------
{
	int oFlag = rkoNONE;		//0

	if (hCheckVC6Output==NULL)	//no check box if  "More" not clicked...  gMoreSettingFlag
		return (rkoLOG | rkoIDE);		//default "ignore" output

	//---- get the Check Box states -------------
	if (SendMessage(hCheckVC6Output, BM_GETCHECK, 0,0))			//the IsDlgButtonChecked function sends a BM_GETCHECK 
		oFlag |= rkoIDE;

	if (SendMessage(hCheckLogging, BM_GETCHECK, 0,0))
		oFlag |= rkoLOG;

	if (SendMessage(hCheckShowAlert, BM_GETCHECK, 0,0))
		oFlag |= rkoDISP;

	return oFlag;
}


//*****************************************************
//*
//*      the error dialog W32 message handler 
//*
//*****************************************************
//---------------------------------------------------------------------------------------------------------------------------------------
#if 1	//_MSC_VER >= 800	//not DXSDK2005
 int __stdcall  RKError_DialogBoxMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
#else
 INT_PTR CALLBACK  RKError_DialogBoxMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
//---------------------------------------------------------------------------------------------------------------------------------------
{
	int closedCode = DIALBOX_ERRORCLOSE_NONE;

	RKErrorDial_hWnd = hWnd;

	switch(msg)
	{
		case WM_INITDIALOG:
		{
			//gMoreSettingFlag = 0;

			RKErrorDial_addItems();		//<--- add butons, text, etc ...
			if (gMoreSettingFlag==1)
				RKErrorDial_addMoreItems(true);

			//---- set dialog position :  same as previously ---
			if (dialBoxWantedPosX!= -999)
			{
				SetWindowPos(hWnd, NULL, dialBoxWantedPosX, dialBoxWantedPosY, 0,0, SWP_NOSIZE | SWP_NOZORDER);
			}
			
			char *fullCmdLine =  GetCommandLine();    //get application name
			char newAlertName[1024];			
			sprintf_SAFE(newAlertName, sizeof(newAlertName), "%s - An error has occurred...", fullCmdLine);
			SetWindowText(hWnd, newAlertName); //"An error has occured...");	//newAlertName);

		}		//end of WM_INITDIALOG
		break;

		case WM_COMMAND:
			//--- buttons ---
			if ( HIWORD(wParam)==BN_CLICKED && (HWND)lParam)
			{
				HWND hBut = (HWND)lParam;

				int oNewFlag = readIgnoreCheckBox();

				if (hBut == hButQuit)								
				{
					closedCode = DIALBOX_ERRORCLOSE_QUIT;
					gRKErrConf.ecIgnoreError(pErr, oNewFlag);						//quit -> add error to the list for final count / report..
				}

				#if MLQA_ADD_EXTRA_SOFT_QUIT_BUTTON==1
				if (hBut == hButQuit_Soft)								
				{
					closedCode = DIALBOX_ERRORCLOSE_QUIT;
					gRKErrConf.ecIgnoreError(pErr, oNewFlag);						//quit -> add error to the list for final count / report..
					gErr_SoftQuitRequested = true;
				}
				#endif

				//---- ignores -------
				else if (hBut == hButIgnoreOnce)		
				{
					oNewFlag |= rkoDISP;																//ignore once : we still want the dialog box next time
					gRKErrConf.ecIgnoreError(pErr, oNewFlag);
					closedCode = DIALBOX_ERRORCLOSE_IGNORE;
				}
				else if (hBut == hButIgnoreType)
				{
					gRKErrConf.ecIgnoreType(pErr->m_type, oNewFlag);
					closedCode = DIALBOX_ERRORCLOSE_IGNORE;
				}
				else if (hBut == hButIgnoreSource)
				{
					gRKErrConf.ecIgnoreSource(pErr->m_filename, oNewFlag);
					closedCode = DIALBOX_ERRORCLOSE_IGNORE;
				}
				else if (hBut == hButIgnoreAll)			
				{
					gRKErrConf.ecIgnoreAll(oNewFlag);
					closedCode = DIALBOX_ERRORCLOSE_IGNORE;
				}
				else if (hBut == hButIgnoreAlways)
				{/*
					int oFlags = pErr->m_oFlags;
					oFlags &= 0xFFFFFFFF - rkoDISP;			//remove the Alert Display flag
					//RKASError::errThisErrorSetOutput(pErr->m_filename, pErr->m_line, pErr->m_type, oFlags);
					*/
					gRKErrConf.ecIgnoreError(pErr, oNewFlag);	//oFlags);		//add to ignore list
					closedCode = DIALBOX_ERRORCLOSE_IGNORE;
				}

				//----
				else if (hBut == hButBreak)					closedCode = DIALBOX_ERRORCLOSE_BREAKPOINT;

				else if (hBut == hButMoreSettings)
				{
					if (gMoreSettingFlag==0)
						RKErrorDial_addMoreItems(true);
					else
						RKErrorDial_addMoreItems(false);
				} //endif hButMoreSettings

				else if (hBut == hButLogMemory)
				{
					#if ML_USE_MEMORY==1
					MLMemory::mDumpInfo();
					#endif
				}

				else if (hBut==hButClearIgnoreList)
				{
					gRKErrConf.ecClearIgnoreList();
				}

			} //endif BN_CLICKED

			else	//other WM_COMMAND
			{
				switch( LOWORD(wParam))
				{
					case IDCANCEL:		//the close button
						closedCode = DIALBOX_ERRORCLOSE_QUIT;
					break;
				}
			}
			
		break;	//end case WM_COMMAND

	} //end switch(msg)


	if (closedCode != DIALBOX_ERRORCLOSE_NONE)
	{
		//---- closing : get the Check Box states -------------
/*		if (SendMessage(hCheckVC6Output, BM_GETCHECK, 0,0))			//the IsDlgButtonChecked function sends a BM_GETCHECK 
			gEnableAssertVC6Output = true;
		else
			gEnableAssertVC6Output = false;

		if (SendMessage(hCheckLogging, BM_GETCHECK, 0,0))
			gEnableAssertLogging = true;
		else
			gEnableAssertLogging = false;
*/
		RECT nowPos;
		GetWindowRect(hWnd, &nowPos);
		dialBoxWantedPosX = nowPos.left;		// -- save dialog pos for next time
		dialBoxWantedPosY = nowPos.top;

		EndDialog(hWnd, closedCode);

		return TRUE;
	}


	return FALSE; 
}






//---------------------------------------------------------------------------------------------------------------------------------------
int RKError_DisplayDialogBox(void)	//char *txt)
//---------------------------------------------------------------------------------------------------------------------------------------
{
/*
 DialogBox() implements it's own message loop and does not return untill the dialog is closed, CreateDialog() acts more like a window created with CreateWindowEx() in that it returns immediately
*/
	if (MLError_DialBoxActivated == 1)
	{
		RKLOG("***** RKError_DisplayDialogBox() : BAD : recursive errors : now Quitting");
		//MLLog("***** next error:%s\n", txt);		//already logged ...  mmm except if rkoLog==0 --> better to repeat
		exit(9999-0); //MLCore_FATALQUIT();		//exit()
	}

/*	#if MLPCVIDEO == MLPCVIDEO_DIRECTX
	MLDXApplication::apEndScene();		//MLVideo::endFrame(); recuse if ...
	#endif*/

	HINSTANCE hInstance = RKAssertGetAppHInstance();
	HWND hwnd = RKAssert_GetAppHWND();  //MLApplication_PC::getHwnd();
	int ret;


/*	#if MLPCVIDEO == MLPCVIDEO_DIRECTX
	if (MLDXApplication::apIsWindowed()==false)
		MLDXApplication::apToggleFullscreen();
	#endif*/

	if (m_hInstRichEdit20 == NULL)
		m_hInstRichEdit20 = LoadLibrary("riched20.dll") ;

	//gTempAssertText = txt;

	typedef struct 
	{
		DLGTEMPLATE dlt;		//  In a standard template for a dialog box, the DLGTEMPLATE structure is always immediately followed by three variable-length arrays that specify the menu, class, and title for the dialog box. 
												//When the DS_SETFONT style is specified, these arrays are also followed by a 16-bit value specifying point size and another variable-length array specifying a typeface name. 
												//Each array consists of one or more 16-bit elements. The menu, class, title, and font arrays must be aligned on WORD boundaries.
		WORD m;							//menu
		WORD c;							//class
		WCHAR title[8]; 
		// The font data begins on the WORD boundary that follows the title array.
		// The font data specifies a 16-bit point size value and a Unicode font name string. If possible, the system creates a font according to the specified values. Then the system sends a WM_SETFONT message to the dialog box and to each control to provide a handle to the font. If DS_SETFONT is not specified, the dialog box template does not include the font data.
		WORD pt;						//16-bit value specifying point size
		WCHAR font[6]; 
	}TDlgData; 


	TDlgData dtp=
	{
		{	//DLGTEMPLATE
			DS_3DLOOK|DS_SETFONT|DS_CENTER|DS_MODALFRAME|WS_POPUP|WS_CAPTION|WS_VISIBLE|WS_SYSMENU,			//WS_SYSMENU  DS_SETFONT		//style;
			0,						//dwExtendedStyle;
			0,//1,//3,		//cdit;	Specifies the number of items in the dialog box.
			0,0,MLERRORDIAL_W, MLERRORDIAL_H		//dialog pos and size ...			0,0,278,196		//dialog pos and size ...
		},			
		0,	//menu
		0,	//class
		L"NoName ",		//temporary title : lenght MUST match WCHAR title[8];   : to be renamed by SetWindowText
		8,										//16-bit value specifying point size
		L"Arial",			//temporary font name: lenght MUST	match WCHAR font[6];     L"MS Sans Serif",		mmm doesn't work ...

	}; 

	/*#if MLPCVIDEO == MLPCVIDEO_DIRECTX
	MLDxApplication_ForcePresentOnPaint = 1;		//to allow the DX ->Present when WM_PAINT is received (dialog box is modal, and application is "frozzen" until a button is pressed)
	#endif*/
	MLError_DialBoxActivated = 1;
	
	BOOL isValidHwnd = IsWindow(hwnd);
	if (isValidHwnd == FALSE)   //when app is closing ... hwnd may be already destroyed ! ..
	{
		RKLOG("OOPPSS, RKError_DisplayDialogBox() WILL FAIL with invalid hwnd 0x%X", hwnd);
		hwnd = NULL;
	}

	ShowCursor(TRUE);		//if was hidden ...
	ret = DialogBoxIndirect(hInstance,(DLGTEMPLATE*)&dtp, hwnd, RKError_DialogBoxMsgProc);			// <------ the call : modal dial box
	ShowCursor(FALSE);

	MLError_DialBoxActivated = 0;
	/*#if MLPCVIDEO == MLPCVIDEO_DIRECTX
	MLDxApplication_ForcePresentOnPaint = 0;
	#endif*/
	MoreItems_alreadyAdded = 0;

	 return ret;
}

//extern bool gMLErrorWantBreakpoint;

//static int errDisplay_RecurseLevel = 0;
//called by MLAssert etc macro













/*
//---static------------------------------------------------------------------------------------------------------------------------
void MLError::errMainInit_PC(void)	
//---------------------------------------------------------------------------------------------------------------------------------
{
	if (m_hInstRichEdit20 == NULL)
		m_hInstRichEdit20 = LoadLibrary("riched20.dll") ;
}
*/
/*
//---static------------------------------------------------------------------------------------------------------------------------
void MLError::errMainQuit_PC(void)	
//---------------------------------------------------------------------------------------------------------------------------------
{
	MLApplication_PC::apEmergencyQuit();	// m_emergencyQuitCallback!= NULL)

	if (m_hInstRichEdit20)
		FreeLibrary(m_hInstRichEdit20) ;

	m_hInstRichEdit20 = NULL;
}
*/





//***** MISC ****
//#if MLQA_ENABLE_OUTPUTIDE==1
/*
//---static------------------------------------------------------------------------------------------------------------------------
void MLError::outputIDE(char* txt)
//---------------------------------------------------------------------------------------------------------------------------------
{ 
	//use OutputDebugString on PC / VisualC
	//mmm better to use MLLogFile::lfConsoleLog(char *txt, ...)
	OutputDebugString(txt); 
}
*/
//#endif



















//**************************************************************************************************************
void RKErrorDisplay_Platform(int errType, const char *cond, const char *file, const char *fn, int line, const char *bufferMessage)
{
  int m_type = errType;
  const char *m_filename = file;
  const char *m_functionName = fn;
  int m_line = line;
  const char *m_expression = cond;
  const char *m_msg = bufferMessage;

	gRKAssertWantBreakpoint = false;

	errDisplay_RecurseLevel++;
	if (errDisplay_RecurseLevel!=1)
	{
		//---mmm bad : may be a problem in log, string_SAFE, etc !: ignore ???
		//if (errDisplay_RecurseLevel==2)	//1st level of recurs: try to log ?
		//	MLLog(
		OutputDebugString("ERR_RECURS--");
		errDisplay_RecurseLevel--;
		return;
	}


	//MLError err;
  //MLError *pErr
  //-- rem pErr = &gCurrenteError
	gCurrenteError.errType(errType);
	gCurrenteError.set_expr(cond);
	gCurrenteError.add_info_context(file, line);

	#if MLDEBUG==1
	if (MLCore::RunFromDebugger())
	{
		if (errType==RKERRT_LAST_ || errType==8)
		{
			char *p=0; *p=0;	//TMP!!!
		}
		if (err.m_type==RKERRT_LAST_ || err.m_type==8)
		{
			char *p=0; *p=0;	//TMP!!!
		}
	}
	#endif

	//char buffer[1024];
	//MLA_snprintf(buffer, sizeof(buffer), args);
	gCurrenteError.m_msg = bufferMessage; //buffer;
  gCurrenteError.m_oFlags = rkoDEFAULT;



//  pErr->errDisplay

	bool enableLogging				= true;		//oDEFAULT -> all true
	bool enableIDE						= true;
	bool enableAlertDisplay		= true;




  int occurence=0;
	int oFlags = gRKErrConf.ecGetOutputFlags(pErr, &occurence);

	int maxOcc = gRKErrConf.ecGetMaxLogOccurence();
	if (gRKErrConf.ecGetMaxLogOccurence()!= 0)
	{
		if (oFlags != rkoNONE && occurence==gRKErrConf.ecGetMaxLogOccurence())
			RKLOG("STOP LOGGING this error : Max Occurrence (%d) reached...", occurence);
		if (occurence>gRKErrConf.ecGetMaxLogOccurence())
			oFlags = rkoNONE;
	}


	if (oFlags == rkoDEFAULT)
	{
			//TODO : use the TYPE /global setting depending of type of error  
			oFlags = rkoALL;
	}
	else
	{
		enableLogging				= ( (oFlags & rkoLOG) != 0);
		enableIDE						= ( (oFlags & rkoIDE) != 0);
		enableAlertDisplay	= ( (oFlags & rkoDISP) != 0);
	}

  if (gErrDisplayW32_forceDisableAllDialogs)
    enableAlertDisplay = false;
  if (gErrDisplayW32_forceDisableWarningDialogs && errType==RKERRT_WARNING)
    enableAlertDisplay = false;





	if (enableLogging == true)		//---- log to file first -------
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		//bool saveGeneralEnableLog = MLLogFile::lfEnable(true);

		RKString message = "";

		char buffer[256];
		_snprintf(buffer, 256, "ERROR_type %s! [%02d-%02d-%02d,%02d:%02d:%02d]", gRKTblErrorTypeName[m_type],  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		message.Append(buffer);
		if (m_filename && m_functionName)
		{
			_snprintf(buffer, 256, " %s : File: \"%s\", Function: %s, Line: %d :",m_expression, m_filename, m_functionName, m_line);
			message.Append(buffer);
		}
		else if (m_filename)
		{
			_snprintf(buffer, 256, " %s : File: \"%s\", Line: %d :",m_expression, m_filename, m_line);
			message.Append(buffer);
		}

		if (RKError_Buffer && RKError_Buffer[0])
		{
			message.Append("<");
			message.Append(RKError_Buffer);
			message.Append(">");
		}
		if (m_msg && m_msg[0])
		{
			message.Append(" msg=");
			message.Append(m_msg);
		}
		RKLOG(message.GetString());
		//MLLogFile::lfEnable(saveGeneralEnableLog);
	}

	if (m_type <RKERRT_NONE || m_type>RKERRT_LAST_)
		m_type = RKERRT_LAST_;	//unknown


	char bufferFullErrMessage[2048];		//mmm < sizeof MLError_Buffer ! BAD : todo : safe

  //errDisplay

	#if MLQA_ENABLE_OUTPUTIDE==1
	if (enableIDE==true)		//--- display on VC6 IDE debug window ---
	{
		sprintf_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), "%s(%d): ERROR %s : (%s)<%s>",m_filename, m_line, gRKTblErrorTypeName[m_type], m_expression, RKError_Buffer);

		if (m_functionName && m_functionName[0])
		{
			strcat_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), " fn=");
			strcat_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), m_functionName);
		}

		if (m_msg && m_msg[0])
		{
			strcat_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), " msg=");
			strcat_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), m_msg);
		}
		strcat_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), "\n");
		OutputDebugString(bufferFullErrMessage);
	}
	#endif //MLQA_ENABLE_OUTPUTIDE

	if (gRKErrConf.ecIsQuitRequestedForError()==true) //MLCore::IsQuitRequested())	//already requested ! ... we are in a long process with many assert, let's quit badly !
	{
		//MLLogFile::lfEnable(true);
		RKLOG("trying to quit clean, but multiple other errors before clean quit -> Quitting badly ... ( exit(); )");
		exit(9999-0);  //MLCore_FATALQUIT();
	}


	//----- breakpoint if debbuger / or dialog box -----------
	if (enableAlertDisplay)	//gEnableAssertBreakPoint)		// <---- possible to manually set to 0 during debuging...
	{
		//--- init some var for dial box ---
		char shortFilename[64];
		RKAssert_fullNameToFilename(m_filename, shortFilename, sizeof(shortFilename));		//unsafe ? : TODO : safe !
		if (m_functionName)
			sprintf_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), "%s Line %d Function=%s : ERROR : (%s) FAILED\n%s\n%s", shortFilename, m_line, m_functionName, m_expression, m_msg, RKError_Buffer);
		else
			sprintf_SAFE(bufferFullErrMessage, sizeof(bufferFullErrMessage), "%s Line %d : ERROR : (%s) FAILED\n%s\n%s", shortFilename, m_line, m_expression, m_msg, RKError_Buffer);

		///now gCurrenteError  pErr = this;
		gTempAssertText = bufferFullErrMessage;

		HINSTANCE hInstance = RKAssertGetAppHInstance();			
		if (hInstance == NULL)	//quitting, or not init ??
		{
			RKLOG("errDisplay Can't display error dialog : hInstance == NULL !!");
			if (enableLogging==false)	//didn't log error ?
				RKLOG("%s", bufferFullErrMessage);
			gRKErrConf.ecIgnoreError(pErr, oFlags); ///this, oFlags);	//add to error counts
			errDisplay_RecurseLevel--;
			#if ML_USE_MEMORY==1
				//#if MLMEMORY_GLOBAL_CONSTRUCTOR_ENABLED
				if (MLMemoryHeap::mainHeap() == NULL)  //not yet initialized ... 
					;	//continue : ok to display dialog...
				else //// to check .. no message ok ???
					return;
			#else			
			return;
			#endif
		}
		int ret = RKError_DisplayDialogBox();		// <----- run Modal dialog box

		if (ret == DIALBOX_ERRORCLOSE_QUIT)
		{
			gRKErrConf.ecQuittRequestedForError();
			#if MLQA_ADD_EXTRA_SOFT_QUIT_BUTTON==1
			if (gErr_SoftQuitRequested==true)
			{
				MLLogFile::lfEnable(true);
				MLCore::SetQuitRequest();			//may crash ... but may give some more informations to track bugs ...
			}
			else
			#endif
			{
				///MLLogFile::lfEnable(true);
				///MLLogFile::lfSetWinTextOut(NULL);
				///MLLogFile::lfSetWinTextOut2(NULL);
				RKLOG("\nQuitting by 'Quit' button after an error...");
				///MLError::errMainQuit();	//log error list...
				exit(9999); //MLCore_FATALQUIT();
			}

/*
			if (m_type == RKERRT_FATAL)		//probably can't quit the clean way ...
			{
				MLLog("\nQuitting by 'Quit' button after a FATAL error...\n");
				MLError::errMainQuit();	//log error list...
				MLCore_FATALQUIT();
			}
			else
			{
				#if MLQA_ADD_EXTRA_SOFT_QUIT_BUTTON==1
				MLCore::SetQuitRequest();			//may crash ... but may give some more informations to track bugs ...
				#else
				MLCore_FATALQUIT();
				#endif
			}*/
		}
		if (ret == DIALBOX_ERRORCLOSE_IGNORE)
		{
			//MLLog("This error is now ignored...\n");	//TODO : log the new oFlag !!!
		}
		
		if (ret == DIALBOX_ERRORCLOSE_BREAKPOINT)
			//{ __asm { int 3 };		}		//real breakpoint : can continue by F5		when not run from debugger, will show an alert box with OK (quit) Retry (debug application)   see DebugBreak(); __debugbreak();  ASSERT  _CrtDbgBreak
			gRKAssertWantBreakpoint = true;	 //see BREAKPT_
		

	}
	else	//enableAlertDisplay
	{
				gRKErrConf.ecIgnoreError(pErr, oFlags);	//add to error counts
	}

	errDisplay_RecurseLevel--;
}


//---------------------------------------------------------------------------------------------------------------------------------
bool RKErrorDial_IsOpen(void)
//---------------------------------------------------------------------------------------------------------------------------------
{
  //true when an error dialog is open, until user close it
  return (errDisplay_RecurseLevel > 0);
}

