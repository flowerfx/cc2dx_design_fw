#ifndef __MAUBINH_SERVICE_H__
#define __MAUBINH_SERVICE_H__
#include "Common/Common.h"
class MauBinhService 
{
public:
	static void autoBinh(ubyte roomID, ubyte boardID);
	static void doFire(ubyte roomID, ubyte boardID, unsigned char* cardIDs);
};

#endif //__MAUBINH_SERVICE_H__

