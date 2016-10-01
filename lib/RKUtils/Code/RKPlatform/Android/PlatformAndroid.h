#ifndef __CASUALCORE_PLATFORMANDROID_H_
#define __CASUALCORE_PLATFORMANDROID_H_

#include <jni.h>

class PlatformAndroid
{
public:
	static JavaVM* ptrVM;
	static void GetEnv(JNIEnv*& env);
};

#endif
