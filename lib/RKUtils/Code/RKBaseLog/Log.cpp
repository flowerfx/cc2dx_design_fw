#include "Log.h"

namespace RKUtils{

	void InitLogging()
	{
		s_log.fileBuffer = 0;
		s_log.fileTarget = 0;
		s_log.enableLogFile = false;
	}

	void SetLoggingEnabled(bool yes)
	{
		if (yes)
		{
			if (!s_log.cout)
			{
#if defined(GAME_WIN32)
				if (IsDebuggerPresent())
				{
					s_log.cout = new logog::OutputDebug;
				}
#if ! defined OS_W8 && ! defined OS_WP8 && ! defined OS_W10
				else
				{
					//s_log.cout = new ColorCout;
				}
#endif
#elif defined(OS_LINUX)
				openlog(s_applicationTitle.c_str(), LOG_PID, LOG_LOCAL0);
				setlogmask(LOG_UPTO(LOG_DEBUG));
				s_log.cout = new SyslogCout;
#elif defined(OS_IOS)
                s_log.cout = new logog::Cout;
#else
				s_log.cout = new logog::Cout;
#endif
			}
		}
		else
		{
			if (s_log.cout != 0)
			{
				delete ((logog::Target*)s_log.cout);
				s_log.cout = 0;
			}
		}
	}

	void FreeLoggin()
	{
		if (s_log.cout != 0)
		{
			delete ((logog::Target*)s_log.cout);
			s_log.cout = 0;
		}
	}
}