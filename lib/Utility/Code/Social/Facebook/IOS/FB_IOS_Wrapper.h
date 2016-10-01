//
//  __FACEBOOK_IOS_WRAPPER_H__
//  CrystalNumberful
//
//  Created by qchien on 12/3/15.
//
//

#ifndef __FACEBOOK_IOS_WRAPPER_H__
#define __FACEBOOK_IOS_WRAPPER_H__

#include "DisplayView.h"
#include "cocos2d.h"
#include "RKString_Code/RKString.h"
USING_NS_CC;
using namespace RKUtils;

namespace Utility
{
    namespace Social
    {
        namespace Facebook
        {
            void LoginFacebook();
            
            int PostMessageToFacebook(RKString message_str, RKString url = "https://www.facebook.com");
            
            void GetFrofileFBWithAccessToken();
            
            RKString GetIDFacebook();
            
            RKString GetNameFacebook();
            
            RKString GetURLAvatarFBFromID(RKString user_id);
            
        }

    }
}



#endif //__FACEBOOK_IOS_WRAPPER_H__
