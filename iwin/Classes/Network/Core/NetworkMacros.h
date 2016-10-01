#ifndef __NETWORK_MACROS_H__
#define __NETWORK_MACROS_H__
#include "cocos2d.h"
#include "Common/Common.h"

#define NS_NW_BEGIN		namespace network {
#define NS_NW_END		}
#define USING_NS_NW		using namespace network;

#undef SAFE_DELETE  
#define SAFE_DELETE(p)         if(p) { delete (p); (p) = nullptr; }

#undef SAFE_DELETE_ARRAY    
#define SAFE_DELETE_ARRAY(p)   if(p) { delete[] (p); (p) = nullptr; }

#undef SAFE_DELETE_VECTOR
#define SAFE_DELETE_VECTOR(v ) \
for(auto p : v) { \
	delete p ; \
} v.clear(); 

#define SAFE_DELETE_MAP(v ) \
for(auto p : v) { \
	delete p.second ; \
} v.clear(); 



#endif