#include "DisplayView.h"
#include "ObjectTexture.h"
#include <random>
namespace Utility
{
	bool g_IsUseArk;
	cocos2d::Size WinSize = cocos2d::Size(0, 0);
	cocos2d::Size DesignSize = cocos2d::Size(480, 800);
	Vec2 ScaleFactor(1, 1);
	float offset_x = 0;

	bool IsPressBackkey = false;
	size_t p_cpuCount;

	bool IsInitGameSize = false;

	std::string data_dir = "Data/";

	using namespace cocos2d;

	std::random_device rd;

	void ChangeToNewSize(int width, int height)
	{
		if (!IsInitGameSize)
			return;

		WinSize.width = width;
		WinSize.height = height;

		if (WinSize.width / WinSize.height > 0.68)
		{
			DesignSize = Vec2(768, 1024);
		}
		//
		bool IsPortrailDesign = (DesignSize.width / DesignSize.height) < 1.0f;
		//
		//the maxium portrail size support is 3:4 = 0.75;
		//
#if defined OS_W8 || defined OS_W10
		if (!IsPortrailDesign && (WinSize.width / WinSize.height) < 0.68f)
		{
			ScaleFactor = Vec2(WinSize.width / DesignSize.width, WinSize.height / DesignSize.height);
		}
		else
		{
			ScaleFactor = Vec2(WinSize.height / DesignSize.height, WinSize.height / DesignSize.height);
			Vec2 ActualSize = cocos2d::Director::getInstance()->getWinSize();
			WinSize.width = (WinSize.height / 1136.f) * 640.f;

			auto glview = Director::getInstance()->getOpenGLView();
			offset_x = ActualSize.x / 2 - WinSize.width / 2;

			glview->setFrameSize(WinSize.width, WinSize.height);
			glview->setDesignResolutionSize(WinSize.width, WinSize.height, ResolutionPolicy::NO_BORDER);

			glview->setViewPortInPoints(offset_x, 0, WinSize.width, WinSize.height);
		}
#elif defined GAME_IOS
		ScaleFactor = Vec2(WinSize.width / DesignSize.width, WinSize.height / DesignSize.height);
#endif
	}

	void InitSizeGame()
	{

		WinSize = cocos2d::Director::getInstance()->getWinSize();
		//
		if (WinSize.width / WinSize.height > 0.68 && WinSize.width / WinSize.height < 1)
		{
			DesignSize = Vec2(768, 1024);
		}
		else if (WinSize.width / WinSize.height >= 1)
		{
			DesignSize = WinSize;
		}

		bool IsPortrailDesign = (DesignSize.width / DesignSize.height) < 1.0f;
		//
		//the maxium portrail size support is 3:4 = 0.75;
		//
#if defined OS_W8 || defined OS_W10
		if (!(IsPortrailDesign)/* && (WinSize.width / WinSize.height) < 0.68f*/)
		{
			ScaleFactor = Vec2(WinSize.width / DesignSize.width, WinSize.height / DesignSize.height);
		}
		else if (IsPortrailDesign)
		{
			ScaleFactor = Vec2(WinSize.height / DesignSize.height, WinSize.height / DesignSize.height);
			Vec2 ActualSize = cocos2d::Director::getInstance()->getWinSize();
			WinSize.width = (WinSize.height / 1024.f) * 768.f;

			auto glview = Director::getInstance()->getOpenGLView();
			offset_x = ActualSize.x / 2 - WinSize.width / 2;

			glview->setFrameSize(WinSize.width,WinSize.height);
			glview->setDesignResolutionSize(WinSize.width, WinSize.height, ResolutionPolicy::NO_BORDER);

			glview->setViewPortInPoints(offset_x, 0, WinSize.width, WinSize.height);
		}
#elif defined GAME_IOS
        ScaleFactor = Vec2(WinSize.width / DesignSize.width, WinSize.height / DesignSize.height);
#endif
		IsInitGameSize = true;
        //
        File::Initialize();
	}

	void InitValue()
	{
        File::InitInstallPath();
		//
#if defined WP8 || defined OS_W8 || defined OS_W10
		SYSTEM_INFO sysinfo;
		GetNativeSystemInfo(&sysinfo);
		p_cpuCount = sysinfo.dwNumberOfProcessors;
#elif defined(OS_IOS) || defined(OS_ANDROID_)
		p_cpuCount = 2;
#else
		p_cpuCount = 4;
#endif

#if defined _DEBUG || defined DEBUG
		LOGOG_INITIALIZE();
		//////////////////////////////
		RKUtils::InitLogging();
		RKUtils::SetLoggingEnabled(true);
#endif
	}
	
	void LoadData()
	{
		//
		//XMLMgr->OnLoadXMLData();
		//
		TextureMgr->Load();
		//
	}
	
	float GetScaleFactor(bool Y)
	{
		if (!Y)
			return ScaleFactor.x;
		return ScaleFactor.y;
	}

	float GetOffsetSCale()
	{
		float ratioDesgin = DesignSize.width / DesignSize.height;
		float curRatio = GetGameSize().width / GetGameSize().height;
		float scaleoffset = 1.f;
		if (curRatio < ratioDesgin)
		{
			scaleoffset = (ratioDesgin - curRatio) / 2;
		}
		return scaleoffset;
	}


	cocos2d::Point GetScreenCenter()
	{
		return (cocos2d::Point(WinSize / 2));
	}
	cocos2d::Point GetTextureCenterPos(MenuItemImage * m)
	{
		return cocos2d::Point(m->getContentSize() / 2);
	}
	cocos2d::Size GetGameSize()
	{
		return WinSize;
	}
	cocos2d::Vec2 GetGameDesignSize()
	{
		return DesignSize;
	}
	cocos2d::Point ConvertPTSCorToCCCor(cocos2d::Point P)
	{
		return cocos2d::Point(P.x, WinSize.height - P.y);
	}
	cocos2d::Point GetViewPoint(cocos2d::Point TopLeft, cocos2d::Point CenterPos)
	{
		cocos2d::Point p = ConvertPTSCorToCCCor(TopLeft);
		return cocos2d::Point(p.x + CenterPos.x, p.y - CenterPos.y);
	}
	float getTimeTick()
	{
		timeval time;
		gettimeofday(&time, NULL);
		unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
		return (float)millisecs;
	}
	cocos2d::Point ConvertPos(cocos2d::Point ps)
	{
		cocos2d::Point p = ps;
#ifdef USE_DX_COR
		p.y = 1 - ps.y;
		p.x = ps.x;
#endif
		return p;
	}

	Vec2 ConvertPos1(Vec2 ps)
	{
		cocos2d::Vec2 p = ps;
#ifdef USE_DX_COR
		p.y = GetGameSize().height - ps.y;
		p.x = ps.x;
#endif
		return p;
	}

	cocos2d::Point GravityVector()
	{
		return cocos2d::Point(0, 1);
	}

	int GetRandom(int num1, int num2, int exceptnumber)
	{
		int max = MAX(num1, num2);
		int min = MIN(num1, num2);
		int returnvalue = min;
		if (max == min)
		{
			min = 0;
			return max;
		}

		std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased

		returnvalue = (int)uni(rng);//((rand() % (max - min)) + min);

		if (exceptnumber != 0xfffff && returnvalue == exceptnumber)
		{
			do
			{
				returnvalue = (int)uni(rng);//((rand() % (max - min)) + min);
			}while(returnvalue == exceptnumber);
		}

		return returnvalue;
	}

#if defined WP8 || defined OS_W8 || defined OS_W10
	std::string CCUnicodeToUtf8(const wchar_t* pwszStr)
	{
		std::string ret = "";
		do
		{
			if (!pwszStr) break;
			size_t len = wcslen(pwszStr);
			if (len <= 0) break;
			size_t convertedChars = 0;
			char * pszUtf8Str = new char[len * 3 + 1];
			WideCharToMultiByte(CP_UTF8, 0, pwszStr, len + 1, pszUtf8Str, len * 3 + 1, 0, 0);
			ret = pszUtf8Str;
			CC_SAFE_DELETE_ARRAY(pszUtf8Str);
		} while (0);

		return ret;
	}

	std::wstring CCUtf8ToUnicode(const char * pszUtf8Str, unsigned len/* = -1*/)
	{
		std::wstring ret;
		do
		{
			if (!pszUtf8Str) break;
			// get UTF8 string length
			if (-1 == len)
			{
				len = (int)strlen(pszUtf8Str);
			}
			if (len <= 0) break;
			// get UTF16 string length
			int wLen = MultiByteToWideChar(CP_UTF8, 0, pszUtf8Str, len, 0, 0);
			if (0 == wLen || 0xFFFD == wLen) break;
			// convert string
			wchar_t * pwszStr = new wchar_t[wLen + 1];
			if (!pwszStr) break;
			pwszStr[wLen] = 0;
			MultiByteToWideChar(CP_UTF8, 0, pszUtf8Str, len, pwszStr, wLen + 1);
			ret = pwszStr;
			CC_SAFE_DELETE_ARRAY(pwszStr);
		} while (0);

		return ret;
	}
#endif

	bool IsBackKeyPress()
	{
		return IsPressBackkey;
	}
	void SetBackkey(bool b)
	{
		IsPressBackkey = b;
	}

	std::string GetPlatForm()
	{
		std::string type = "";
#if defined W8 || defined WP8
		type = "WP8";
#elif defined WIN32
		type = "WIN32";
#elif defined IOS
		type = "IOS";
#elif defined ANDROID
		type = "ANDROID";
#endif
		return type;
	}
	bool IsDeviceLowMem()
	{
#ifdef WP8
		/*long mem = Windows::Phone::System::Memory::MemoryManager::ProcessCommittedLimit / 1000000;
		if (mem < 190)
		{
			return true;
		}*/
		return false;
#endif
		return false;
	}

	uint32 GetCpuCoreCount()
	{
		return (uint32)p_cpuCount;
	}
#ifdef WP8
	extern "C"
	{
		__declspec(dllimport) float g_framerate;
	}
#elif defined(OS_IOS) || defined(OS_ANDROID_)
	extern "C" float g_framerate;
#endif

	float SyncWithFPS(float d)
	{
		return d / 2.f;
	}

	std::string convertToDisplayTime(int timePlay)
	{
		std::string minute = "";
		std::string sec = "";
		int numberMinute = timePlay / 60;
		if (numberMinute < 10)
		{
			minute = "0" + std::to_string(numberMinute);
		}
		else
		{
			minute = std::to_string(numberMinute);
		}

		int numberSecond = timePlay % 60;
		if (numberSecond < 10)
		{
			sec = "0" + std::to_string(numberSecond);
		}
		else
		{
			sec = std::to_string(numberSecond);
		}

		return (minute + std::string(":") + sec);
	}

	float GetMulMul(float mNum, int num)
	{
		while (num > 0)
		{
			mNum *= 2;
			num--;
		}
		return mNum;
	}

	float GetMulDiv(float mNum, int num)
	{
		while (num > 0)
		{
			mNum /= 2;
			num--;
		}
		return mNum;
	}

	void DataDir(std::string str)
	{
		data_dir = str;
	}
		
	std::string DataDir()
	{
		return data_dir;
	}
    
    cocos2d::Rect GetSizeKeyboard()
    {
        auto pGlView = Director::getInstance()->getOpenGLView();
        if (pGlView)
        {
            //return pGlView->GetKeyboardRect();
        }
		return cocos2d::Rect(0, 0, 0, 0);
    }
}