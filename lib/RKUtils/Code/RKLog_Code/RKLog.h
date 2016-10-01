#ifndef _RK_LOG_H
#define _RK_LOG_H
namespace RKUtils
{
	// determine if logging should be disabled
#ifdef RKRETAIL
	//#  ifdef GAME_GOLD
#    ifndef GAME_DEBUG
#define RKLOG_DISABLE_LOGGING
#    endif
	//#  endif
#endif

	//#ifndef GAME_DEBUG
	//  #define RKLOG_DISABLE_LOGGING //Logging should be off on release by default.
	//#endif

	enum RKLogLevel
	{
		RKLL_UNKNOWN = 0, // backward compatibility enum, not recommended to use
		RKLL_FATAL,       // fatal error, it's impossible to proceed, application will be shut down
		RKLL_ERROR,       // error means that something is seriously wrong, but maybe it's still fine to continue
		RKLL_WARN,        // warning means that something is wrong, but it's safe to continue
		RKLL_INFO,        // info used to present useful information
		RKLL_TRACE,       // trace used for debugging purposes only, you should not commit any code that contains traces
	};

	// heart and soul of RKLog, logging function call
#ifndef RKLOG_DISABLE_LOGGING
	void _RKLogOutImpl(RKLogLevel logLevel, const char* logType, const char* file, long line, const char* function, const char* format, ...);
#  ifndef GAME_WIN32
#    define _RKLOG(logLevel, logType, format, ...) _RKLogOutImpl(logLevel, logType, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, ##__VA_ARGS__)
#  else
#    define _RKLOG(logLevel, logType, format, ...) _RKLogOutImpl(logLevel, logType, __FILE__, __LINE__, __FUNCSIG__, format, ##__VA_ARGS__)
#  endif
	inline void _RKLog_NOP_StubFunction() {}
#else
	inline void _RKLog_NOP_StubFunction() {}
	inline void _RKLogOutImpl(RKLogLevel logLevel, const char* logType, const char* file, long line, const char* function, const char* format, ...) {}
#  define _RKLOG(logLevel, logType, format, ...) _RKLog_NOP_StubFunction()
#endif

	//
	// RKLog provides a cross-platform and convenient way to log events
	// It consists of multiple sets of macros
	//
	// Each macro represent one log level (error/warn/etc please see RKLogLevel comments)
	// and accepts sprintf format string followed by any amount of sprintf arguments
	//
	// Example:
	// RKLOG_INFO("Math check: 2*2 = %d", 2*2);
	//
	// RKLog will automatically:
	// 1) print log level and log type (if any)
	// 2) print file/line information
	// 3) print name of the function it is called from
	// 4) add a newline at the end (!)
	//
	// As a result:
	// 1) You don't have to specify function name in your log message
	// 2) You should neither start log message with a newline nor end it with a newline
	//
	// You can also find a macro called RKNSLOG (in file RKNSLog.h)
	// that behaves exactly as objective-c NSLog
	// but can be compiled out in master builds
	//
#define RKLOG(format, ...)        _RKLOG(RKUtils::RKLogLevel::RKLL_UNKNOWN, "", format, ##__VA_ARGS__)
#define RKLOG_FATAL(format, ...)  _RKLOG(RKUtils::RKLogLevel::RKLL_FATAL  , "", format, ##__VA_ARGS__)
#define RKLOG_ERROR(format, ...)  _RKLOG(RKUtils::RKLogLevel::RKLL_ERROR  , "", format, ##__VA_ARGS__)
#define RKLOG_WARN(format, ...)   _RKLOG(RKUtils::RKLogLevel::RKLL_WARN   , "", format, ##__VA_ARGS__)
#define RKLOG_INFO(format, ...)   _RKLOG(RKUtils::RKLogLevel::RKLL_INFO   , "", format, ##__VA_ARGS__)
#define RKLOG_TRACE(format, ...)  _RKLOG(RKUtils::RKLogLevel::RKLL_TRACE  , "", format, ##__VA_ARGS__)

	//
	// Using this set of macros you can specify 
	// additional logType, e.g. "FILE", "MEMORY", "SOUND", "VIDEO"
	// as a first argument
	//
	// Example:
	// RKLOGt_INFO("Math", "Math check: 2*2 = %d", 2*2)
	//
#define RKLOGt(logType, format, ...)        _RKLOG(RKUtils::RKLogLevel::RKLL_UNKNOWN, logType, format, ##__VA_ARGS__)
#define RKLOGt_FATAL(logType, format, ...)  _RKLOG(RKUtils::RKLogLevel::RKLL_FATAL  , logType, format, ##__VA_ARGS__)
#define RKLOGt_ERROR(logType, format, ...)  _RKLOG(RKUtils::RKLogLevel::RKLL_ERROR  , logType, format, ##__VA_ARGS__)
#define RKLOGt_WARN(logType, format, ...)   _RKLOG(RKUtils::RKLogLevel::RKLL_WARN   , logType, format, ##__VA_ARGS__)
#define RKLOGt_INFO(logType, format, ...)   _RKLOG(RKUtils::RKLogLevel::RKLL_INFO   , logType, format, ##__VA_ARGS__)
#define RKLOGt_TRACE(logType, format, ...)  _RKLOG(RKUtils::RKLogLevel::RKLL_TRACE  , logType, format, ##__VA_ARGS__)

	//
	// if you don't want to specify logType each time, then you can use 
	// auxiliary include file RKLogLocalDefines.h
	// it will automatically define local set of log macros called RKLOGl(), RKLOGl_WARN() and so all
	//
	// to do so first define RKLOG_TYPE as a string, then include RKLogLocalDefines.h
	// after that you can use newly defined RKLOGl* macros without specifying log type
	//
	// #define RKLOG_TYPE "Math"
	// #include "RKLogLocalDefines.h"
	// RKLOGl_INFO("Math check: 2*2 = %d", 2*2);
	// // that's completely equal to RKLOGt("Math", "Math check: 2*2 = %d", 2*2);
	//
	//
	// This is a bit hacky operation so you must take care about following:
	//
	// 1) You can only include RKLogLocalDefines.h in C++ file
	//    If you generate macros in header file, they will affect all other files
	//    that include your header file
	//
	// 2) Inclusion of RKLogLocalDefines.h should be the last inclusion in your C++ file.
	//    Otherwise your local macros will affect all files included after RKLogLocalDefines.h
	// 
	// But don't worry about that, there is a compile-time guard to ensure RKLogLocalDefines 
	// correct usage and it won't let you to use in a wrong way
	//

	void RKLog_Enable_printf(bool want_printf);

	typedef void RKLogCallbackType(RKLogLevel logLevel, const char *logType, const char* file, long line, const char* function, const char *message);
	void RKLog_SetExtraLogCallback(RKLogCallbackType* pCallback);
	void RKLog_EnableCallback(bool enab);


#define RKLogStringize( L )			#L
#define RKLogMakeString( M, L )		M(L)
#define $RKLogCompileLine	RKLogMakeString( RKLogStringize, __LINE__ )

#ifdef GAME_WIN32
#define RKCompileMessage  __FILE__ "(" $RKLogCompileLine ") : "
#define RKCompileReminder __FILE__ "(" $RKLogCompileLine ") : Reminder: "
#define RKCompileTODO     __FILE__ "(" $RKLogCompileLine ") : TODO: "
#else
#define RKCompileMessage  ""
#define RKCompileReminder "Reminder: "
#define RKCompileTODO     "TODO: "
#endif
}
#endif // _RK_LOG_H
