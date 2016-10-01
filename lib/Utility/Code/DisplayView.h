#ifndef _DISPLAY_VIEW_H_
#define _DISPLAY_VIEW_H_


#include "RKBaseLog/debug.h"
#include "RKBaseLog/log.h"
#include "RKString_Code/RKString.h"

#include "cocos2d.h"
#define USE_ANTIALIAS 0

#define USE_BLOOM_EFFECT 1
//simulate dx cor on opengl, no-need on directX, fix for design from photoshop
#define USE_DX_COR 1

#ifdef USE_ARK
#define USE_ARK_ARCHIVE 1
#endif
extern "C"
{
#if defined(OS_IOS)
    __attribute__((dllimport)) bool g_IsUseArk ;
#elif defined(OS_ANDROID_)
	extern bool g_IsUseArk;
#else
	__declspec(dllimport) bool g_IsUseArk ;
#endif

}

//enable sound in game
#define USE_CC_AUDIO 1

#ifdef USE_CC_AUDIO
//use new engine sound
#define USE_NEW_SOUND

//if use new engine sound , we dont need sound thread anymore
#ifndef USE_NEW_SOUND
#define USE_AUDIO_ON_THREAD
#endif
#endif



#define USE_DYNAMIC_BG 1

#define USE_PARTICLE 1

//#define USE_IAP 0

//#define USE_TEXT

#define USE_THREAD_BG


#ifdef USE_TEXT
#define LABEL Text
#else
#define LABEL Label
#endif

#define USE_SPRITE_FRAME 1

#if defined(OS_IOS) || defined(OS_ANDROID_)
//#include "Init/../../iOS/IOSCode/DisplayView_IOS.h"
typedef unsigned int uint32;
#endif
using namespace cocos2d;

namespace Utility
{

	int GetRandom(int num1, int num2, int except_num = 0xfffff);
	Vec2 ConvertPos(Vec2 ps);
	Vec2 ConvertPos1(Vec2 ps);
	Vec2 GravityVector();
	Vec2 ConvertPTSCorToCCCor(Vec2 P);
	Vec2 GetViewPoint(Vec2 TopLeft, Vec2 CenterPos);

	void InitSizeGame();
	void InitValue();
	void LoadData();

	void ChangeToNewSize(int width, int height);

	Vec2 GetScreenCenter();
	Vec2 GetTextureCenterPos(MenuItemImage * m);

    cocos2d::Size  GetGameSize();
	Vec2  GetGameDesignSize();
	//
	float GetScaleFactor(bool Y = false);
	float GetOffsetSCale();
	//in milisecond
	float getTimeTick();
	//backkey
	bool IsBackKeyPress();
	void SetBackkey(bool b);
	//use in the editor
	//convert string
#if defined WP8 || defined OS_W8 || defined OS_W10
	std::string CCUnicodeToUtf8(const wchar_t* pwszStr);
	std::wstring CCUtf8ToUnicode(const char * pszUtf8Str, unsigned len/* = -1*/);
#endif
	//get platform
	std::string GetPlatForm();
	//get mempeak
	bool IsDeviceLowMem();
	unsigned int GetCpuCoreCount();
	//sync with fps
	float SyncWithFPS(float d);
	//convert to display time
	std::string convertToDisplayTime(int timePlay);

	//math
	float GetMulMul(float mNum, int num);
	float GetMulDiv(float mNum, int num);

	//data dir
	void DataDir(std::string str);
	std::string DataDir();
    
    cocos2d::Rect GetSizeKeyboard() ;
}
//---------------------------------------------------------------------
//define path
//---------------------------------------------------------------------
#define XML_DIR			"xml/"


#define LOADER_XML        "xml/LoaderXML.xml"

#endif //_DISPLAY_VIEW_H_