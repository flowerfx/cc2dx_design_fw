#pragma once

#include "Assert.h"

#if defined (OS_ANDROID_)
#	define LOGOG_LEVEL LOGOG_LEVEL_NONE
#elif defined(OS_W8) || defined OS_WP8 || defined OS_W10
#	if defined(_DEBUG)
#		define LOGOG_LEVEL LOGOG_LEVEL_INFO
#	else
#		define LOGOG_LEVEL LOGOG_LEVEL_NONE
#	endif
#elif defined(_DEBUG)
#	define LOGOG_LEVEL LOGOG_LEVEL_ALL
#else
#	define LOGOG_LEVEL LOGOG_LEVEL_INFO
#endif

#include "../Logon/include/logog.hpp"
using namespace RKUtils::logog;

#ifndef JET_H_INCLUDED_CORRECTLY
#error "Include Jet.h instead of this header file directly."
#endif


//////////////////////////////////////////////////////////////////////////
// System stuff

// **************************** IMPORTANT *************************************
// If you add a new macro that contains a code block ( {} ), use the this syntax:
//	   do { - your code here - } while (false)
// This is to prevent bugs with missing ; at the end of your macro.
// ****************************************************************************

////////////////////////////////////////////////////////////////////////
// Debug options

#if defined(_DEBUG) && defined(_WIN32) && !defined(_M_ARM) && !defined(_M_X64)
#	define PBREAKPOINT do { RK_BREAK(); } while (false)
#else
#	define PBREAKPOINT do {} while (false)
#endif

//PASSERT triggers a breakpoint on debug. Use it to check internal inconsistencies (like unexpected NULL pointers)
// For any error that is recoverable from user code (like resource loading), use PTHROW.
// For errors that are recoverable from the caller, check the condition manually and return an error code as well, since asserts are disabled on release:
// Example:
// PASSERT(model != NULL); //this asserts on debug
// if (model == NULL) //this informs the caller about the failure, if the caller can recover.
//		return -1;

//#if defined(_DEBUG)
//#	define PASSERT(cond)	do { if(!(cond)) { if(AssertMessage(#cond, __FILE__, __LINE__, "Error!!!!")) PBREAKPOINT; } } while (false)
//#else
//#	define PASSERT(cond) do {} while (false)
//#endif
//
#define  PASSERT(cond) RK_ASSERT(cond)

//#if defined(_DEBUG)
//#	define PASSERT2(cond, ...)	do { if(!(cond)) { if(AssertMessage(#cond, __FILE__, __LINE__, ##__VA_ARGS__)) PBREAKPOINT; } } while (false)
//#else
//#	define PASSERT2(cond, ...) do {} while (false)
//#endif

#define PASSERT2(cond, msg, ...)	RK_ASSERT_MSG(cond, msg, ##__VA_ARGS__)

#define PASSERT_ONCE(cond)	RK_ASSERT_ONCE(cond)

#define PASSERT2_ONCE(cond, msg, ...)	RK_ASSERT_MSG_ONCE(cond, msg, ##__VA_ARGS__)

#if defined(_DEBUG)
#	define PINTERNALASSERT(cond) do { if (!(cond)) PBREAKPOINT; } while (false)
#else
#	define PINTERNALASSERT(cond) do {} while (false)
#endif


//PTHROW has the following behaviour:
//	if exceptions are enabled, it always throws an exception
//	otherwise it asserts on debug, or does nothing on release
// It's meant to be used for any recoverable error - like loading resources.
// Do NOT use it for unrecoverable errors, like internal inconsistencies - PTHROW(pointer != NULL). For these use PASSERT
#if defined(RK_USE_EXCEPTIONS)
#	define PTHROW(cond)	do { if (!(cond)) { RK_ASSERT(cond); throw PException(); } } while (false)
#else
#	define PTHROW(cond)	RK_ASSERT(cond)
#endif

#if defined(RK_USE_EXCEPTIONS)

#pragma warning(disable:4996)

inline const char* ProcessException(const char* msg, ...)
{
	if (msg != NULL)
	{
		static char messageBuffer[8192];
		{
			va_list args;
			va_start(args, msg);
			_vsnprintf(messageBuffer, 8192, msg, args);
			va_end(args);
		}

		return messageBuffer;
	}

	return 0;
}

#endif //RK_USE_EXCEPTIONS

#if defined(RK_USE_EXCEPTIONS)
#	define PTHROW2(cond, msg,...) do { if (!(cond)) { RK_ASSERT_MSG(cond, msg, ##__VA_ARGS__); throw std::exception(ProcessException(msg, ##__VA_ARGS__)); } } while (false)
#else
#	if defined(_DEBUG)
#		define PTHROW2(cond, msg, ...) RK_ASSERT_MSG(cond, msg, ##__VA_ARGS__)
#	else
#		define PTHROW2(cond, ...) PBREAKPOINT
#	endif
#endif

//#define PTHROW2(cond, msg, ...)	RK_ASSERT_MSG(cond, msg, ##__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////////////
// Debug Macros

//PLOG is written in a log file or sent to stdout.

#if defined OS_ANDROID_
#	define PDBG(msg, ...)
#	define PINFO(msg, ...)
#	define PWARN1(msg, ...)
#	define PWARN2(msg, ...)
#	define PWARN3(msg, ...)
#	define PERR(msg, ...)
//#elif defined OS_IOS
//#	define PDBG(msg, ...)
//#	define PINFO(msg, ...)
//#	define PWARN1(msg, ...)
//#	define PWARN2(msg, ...)
//#	define PWARN3(msg, ...)
//#	define PERR(msg, ...)
#elif defined DEBUG || defined _DEBUG
#	define PDBG(msg, ...) LOGOG_DEBUG(msg, ##__VA_ARGS__)
#	define PINFO(msg, ...) LOGOG_INFO(msg, ##__VA_ARGS__)
#	define PWARN1(msg, ...) LOGOG_WARN1(msg, ##__VA_ARGS__)
#	define PWARN2(msg, ...) LOGOG_WARN2(msg, ##__VA_ARGS__)
#	define PWARN3(msg, ...) LOGOG_WARN3(msg, ##__VA_ARGS__)
#	define PERR(msg, ...) do { PASSERT2(0, msg, ##__VA_ARGS__); LOGOG_ERROR(msg, ##__VA_ARGS__); } while(false)
#else
#	define PDBG(msg, ...)
#	define PINFO(msg, ...)
#	define PWARN1(msg, ...)
#	define PWARN2(msg, ...)
#	define PWARN3(msg, ...)
#	define PERR(msg, ...)
#endif

#if defined(_DEBUG) || !defined(OS_IOS)
#	define RK_DEBUGGER_CHECK do {} while (false)
#	define DO_I_NEED_TO_CRASH false
#else

//use this whenever you want to check for the attached debugger
//Don't forget to use the JET_DEBUGGER_CHECK_INCLUDES define at the top of your file
#	define JET_DEBUGGER_CHECK										\
	do 																\
	{																\
		if (!jet::System::RemoveGlobalTexture_())					\
		{															\
			size_t size = sizeof(struct kinfo_proc);				\
			struct kinfo_proc info;									\
			int ret, name[4];										\
			memset(&info, 0, sizeof(struct kinfo_proc));			\
			name[0] = CTL_KERN;										\
			name[1] = KERN_PROC;									\
			name[2] = KERN_PROC_PID;								\
			name[3] = getpid();										\
			if ((ret = (sysctl(name, 4, &info, &size, NULL, 0)))) 	\
			{														\
				jet::System::GetGlobalDriverUniform_();					\
			}														\
			if (info.kp_proc.p_flag & P_TRACED)						\
			{														\
				jet::System::GetGlobalDriverUniform_();					\
			}														\
		}															\
	}																\
	while (false)

#	define DO_I_NEED_TO_CRASH jet::System::CheckMemoryLimits_()

#endif


