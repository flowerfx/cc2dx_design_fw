/*
 *  RKTypes.h
 *
 *  Created by Daniel Stephens on 14/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RKTYPES_H
#define RKTYPES_H

#include <stdint.h>
#include "RK.h"
namespace RKUtils
{
	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	typedef uint64_t          uint64;
	typedef unsigned int      uint32;
	typedef unsigned short    uint16;
	typedef unsigned char     uint8;

	typedef signed int        int32;
	typedef signed short      int16;
	typedef signed char       int8;

	typedef unsigned long     DWORD;
	typedef unsigned short    WORD;
	typedef unsigned char     BYTE;

	//typedef uint32				u32;
	typedef long long		 int64;
	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	enum RKEnabledState
	{
		RKUNKNOWN = -1,
		RKFALSE = 0,
		RKTRUE = 1,
	};

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------


	///----------------------------------------------------------------------
	///----------------------------------------------------------------------

}
///----------------------------------------------------------------------
#endif // RKTYPES_H
