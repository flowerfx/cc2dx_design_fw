/*
 *  RK.h
 *
 *  Created by Daniel Stephens on 11/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RK_H
#define RK_H
	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
#if defined (GAME_IOS)
#define RKPLATFORM_IOS
#elif defined (GAME_ANDROID)
#define RKPLATFORM_ANDROID
#elif defined (GAME_WIN32)
#define RKPLATFORM_WIN32
#define RKPLATFORM_WINDOWS
#elif defined (GAME_WIN64)
#define RKPLATFORM_WIN64
#define RKPLATFORM_WINDOWS
#endif

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
#if defined (RKPLATFORM_IOS)
#include "CoreFoundation/CoreFoundation.h"
#include <mach/mach_host.h>
#elif defined (RKPLATFORM_WINDOWS)
#ifndef OS_W10
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#elif defined (RKPLATFORM_ANDROID)
#include <unistd.h>
#endif // RKPLATFORM_IOS

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------


	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
#include "RKTypes.h"
namespace RKUtils
{
	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	typedef void (RKCallback)(void* pData);
}
///----------------------------------------------------------------------
#endif // RK_H
