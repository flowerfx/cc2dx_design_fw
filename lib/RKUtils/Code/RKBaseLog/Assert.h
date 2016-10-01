#pragma once

#include "Enum.h"

///////////////////////////////////////////////////////////////////////////////
//

#ifdef _DEBUG
#define ASSERTS_ENABLED
#define RK_USE_EXCEPTIONS
#endif 

namespace assertLib
{
	struct AssertTypeDef
	{
		enum type
		{
			BREAK,
			CONTINUE,
		};
	};

	typedef safe_enum<AssertTypeDef> AssertType;

	typedef	AssertType (*AssertHandler)(const char* condition, const char* msg, 
									const char* file, int line);
	
	AssertType TestCondition(const char* condition, const char* file, 
									int line, const char* msg, ...);	
	
	AssertType TestConditionOnce(const char* condition, const char* file, 
									int line, const char* msg, ...);	

	void SetAssertHandler(AssertHandler);
}

//#if TARGET_CPU_ARM
//    #define DebugBreak() __asm__ __volatile__ ("mov r0, %0\nmov r1, %1\nmov r12, #37\nswi 128\n" \
//                                            : : "r" (getpid ()), "r" (signal) : "r12", "r0", "r1", "cc")
//#elif TARGET_CPU_X86
//    #define DebugBreak() __asm__("int $3\n" : : )
//#endif

#ifdef _WIN32
	#define RK_BREAK() __debugbreak()
    #define RK_BREAKIF(x) if (x) { RK_BREAK(); }
#else 
//	#define JBREAK() kill(getpid(), SIGINT)
	#define JBREAK() break
#endif 

#define RK_UNUSED(x) do { (void)sizeof(x); } while(0)

#ifdef ASSERTS_ENABLED

	#define RK_ASSERT(cond) \
		do { \
			if (!(cond)) { \
				if ( assertLib::TestCondition(#cond, __FILE__, __LINE__, 0) == assertLib::AssertType::BREAK)  \
				{\
					RK_BREAK();  \
				}\
			} \
		} while(0)

	#define RK_ASSERT_MSG(cond, msg, ...) \
		do { \
			if (!(cond)) { \
				if (assertLib::TestCondition(#cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__) == assertLib::AssertType::BREAK) \
					{ \
						RK_BREAK();\
					} \
				} \
			} while(0)


	#define RK_ASSERT_ONCE(cond) \
		do { \
			if (!(cond)) { \
				if (assertLib::TestConditionOnce(#cond, __FILE__, __LINE__, 0) == assertLib::AssertType::BREAK) \
					{ \
						RK_BREAK();\
					} \
				} \
			} while(0)

	#define RK_ASSERT_MSG_ONCE(cond, msg, ...) \
		do { \
			if (!(cond)) { \
				if (assertLib::TestConditionOnce(#cond, __FILE__, __LINE__, (msg), ##__VA_ARGS__) == assertLib::AssertType::BREAK) \
					{ \
						RK_BREAK();\
					} \
				} \
			} while(0)

#else
	#define RK_ASSERT(condition) do { RK_UNUSED(condition); } while(0)
	#define RK_ASSERT_MSG(condition, msg, ...) do { RK_UNUSED(condition); RK_UNUSED(msg); } while(0)	
	#define RK_ASSERT_ONCE(condition) do { RK_UNUSED(condition); } while(0)
	#define RK_ASSERT_MSG_ONCE(condition, msg, ...) do { RK_UNUSED(condition); RK_UNUSED(msg); } while(0)	
#endif


#if _MSC_VER >= 1600
	#define RK_COMPILE_ASSERT(exp) static_assert(exp, #exp)
#endif


#ifndef RK_COMPILE_ASSERT
#define RK_COMPILE_ASSERT(exp) typedef char __RK_COMPILE_ASSERT__[(exp)?1:-1]
#endif
 
//
///////////////////////////////////////////////////////////////////////////////
