#include "RKLog.h"
#include "../Common/RKMacros.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>
namespace RKUtils
{
#ifdef GAME_WIN32
#  define snprintf _snprintf
#ifndef OS_W10
#  define WIN32_LEAN_AND_MEAN
#endif
#  include <windows.h>
#  define localtime_safe(out, in) localtime_s(out, in)
#else
#  define localtime_safe(out, in) localtime_r(in, out)
#endif

	//HDVD
#ifdef WINAPI_FAMILY
#include <string>
#endif

#ifdef GAME_ANDROID
#  include <android/log.h>

	inline static android_LogPriority RKLogLevelToAndroidLogPriority(RKLogLevel logLevel)
	{
		switch (logLevel)
		{
		case RKLL_UNKNOWN:
			return ANDROID_LOG_INFO; // can't use ANDROID_LOG_UNKNOWN here, most log filters ignore it, better to use INFO
		case RKLL_FATAL:
			return ANDROID_LOG_FATAL;
		case RKLL_ERROR:
			return ANDROID_LOG_ERROR;
		case RKLL_WARN:
			return ANDROID_LOG_WARN;
		case RKLL_INFO:
			return ANDROID_LOG_INFO;
		case RKLL_TRACE:
			return ANDROID_LOG_DEBUG;
		default:
			RKASSERT(false, "Unexpected logLevel");
		}
		return ANDROID_LOG_UNKNOWN;
	}
#endif

	static RKLogCallbackType *gRKLogCallback = NULL;
	static bool gRKLogEnableCallback = true;
	static bool gRKLog_want_printf = true;

	inline static const char* RKLogLevelToString(RKLogLevel logLevel)
	{
		switch (logLevel)
		{
		case RKLL_UNKNOWN:
			return "LOG";
		case RKLL_FATAL:
			return "FATAL";
		case RKLL_ERROR:
			return "ERROR";
		case RKLL_WARN:
			return "WARNING";
		case RKLL_INFO:
			return "INFO";
		case RKLL_TRACE:
			return "TRACE";
		default:
			return "INVALID_LOGLEVEL";
		}
	}

	inline static const char* FilePathToFileName(const char* path)
	{
		const char* name = path;

		const char* ptr = path;
		while (*ptr)
		{
			if ((*ptr) == '\\' || (*ptr) == '/')
			{
				name = ptr + 1;
			}
			++ptr;
		}

		return name;
	}

	inline static void ParseFunctionSignature(const char* fullSignature, const char*& functionNameStart, int& functionNameLength)
	{
		int templateBracketsCount = 0;

		functionNameStart = fullSignature;
		functionNameLength = 0;

		const char* ptr = fullSignature;
		while (*ptr)
		{
			if (*ptr == ' ' && templateBracketsCount == 0)
			{
				functionNameStart = ptr + 1;
				functionNameLength = 0;
			}
			else if (*ptr == '(' && templateBracketsCount == 0)
			{
				return;
			}
			else
			{
				if (*ptr == '<')
				{
					++templateBracketsCount;
				}
				else if (*ptr == '>')
				{
					--templateBracketsCount;
				}
				++functionNameLength;
			}
			++ptr;
		}
		return;
	}


	//------------------------------------------------------------------------------------------------------------------------------------------
	void RKLog_SetExtraLogCallback(RKLogCallbackType *pCallback)
		//------------------------------------------------------------------------------------------------------------------------------------------
	{
		gRKLogCallback = pCallback;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------
	void RKLog_Enable_printf(bool want_printf)
		//------------------------------------------------------------------------------------------------------------------------------------------
	{
		gRKLog_want_printf = want_printf;
	}

	//------------------------------------------------------------------------------------------------------------------------------------------
	void RKLog_EnableCallback(bool enab)
		//------------------------------------------------------------------------------------------------------------------------------------------
	{
		gRKLogEnableCallback = enab;
	}

#ifndef RKLOG_DISABLE_LOGGING
	//------------------------------------------------------------------------------------------------------------------------------------------
	void _RKLogOutImpl(RKLogLevel logLevel, const char* logType, const char* file, long line, const char* function, const char* format, ...)
		//------------------------------------------------------------------------------------------------------------------------------------------
	{
		static const int messageSize = 2048;
		char message[messageSize];

		const char* functionName;
		int functionNameLength;
		ParseFunctionSignature(function, functionName, functionNameLength);

		const char* fileName = FilePathToFileName(file);

		int headerSize = 0;
		{ // formatting header
#ifndef GAME_ANDROID
			time_t currentTime;
			time(&currentTime);
			tm localTime;
			localtime_safe(&localTime, &currentTime);
			int timestampSize = strftime(message, messageSize, "[%d/%m %H:%M:%S]:", &localTime);

			const char* logLevelString = RKLogLevelToString(logLevel);
			int infoSize = snprintf(message + timestampSize, messageSize - timestampSize, "%s:%s:%s:%ld(%.*s): ", logLevelString, logType, fileName, line, functionNameLength, functionName);

			headerSize = (infoSize >= 0) ? (timestampSize + infoSize) : -1; // store -1 in case of error
#else // no need in log level or log type for android
			headerSize = snprintf(message, messageSize, "%s:%ld(%.*s): ", fileName, line, functionNameLength, functionName);
#endif
			if (headerSize < 0) // handle snprintf error
			{
				headerSize = messageSize;
			}
		}

		va_list args;
		va_start(args, format);
		vsnprintf(message + headerSize, messageSize - headerSize, format, args);
		va_end(args);

		message[messageSize - 2] = '~';
		message[messageSize - 1] = 0;

#ifdef GAME_ANDROID
		__android_log_write(RKLogLevelToAndroidLogPriority(logLevel), logType, message);
#else
		if (gRKLog_want_printf)
		{
			printf("%s\n", message);
		}
#endif

#ifdef GAME_WIN32
#ifdef WINAPI_FAMILY
		/*std::string msgStr(message);
		msgStr += "\n";
		std::wstring wMsgStr = std::wstring(msgStr.begin(), msgStr.end());*/
		//HDVD - add log for windows 8
		std::string strMsg(message);
		strMsg += "\n";
		std::wstring wstrMsg(strMsg.begin(), strMsg.end());
		OutputDebugStringW(wstrMsg.c_str());
#else
		// TODO: print it with a single call
		OutputDebugString(message);
		OutputDebugString("\n");
#endif
#endif

		if (gRKLogCallback && gRKLogEnableCallback)
		{
			gRKLogCallback(logLevel, logType, fileName, line, function, message + headerSize); // pass message without a header here
		}
	}
#endif // RKLOG_DISABLE_LOGGING
}