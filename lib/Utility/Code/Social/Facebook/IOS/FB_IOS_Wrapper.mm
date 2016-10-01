//
//  IAP_IOS_.m
//  CrystalNumberful
//
//  Created by qchien on 12/3/15.
//
//
#import "FB_ISO_Helper.h"
#import "FB_IOS_Wrapper.h"

namespace Utility
{
    namespace Social
    {
        namespace Facebook
        {
            
            void LoginFacebook()
            {
#if defined USE_SOCIAL_ADS
                [[FB_IOS_Helper defaultHelper] LoginFacebook];
#endif
               
            }
            
            int PostMessageToFacebook(RKString message_str , RKString url)
            {
 #if defined USE_SOCIAL_ADS
                NSString * message_fb = [[NSString alloc]initWithBytes:(const void *)message_str.ToUnicode()
                                                                length:message_str.LengthUTF()
                                                                encoding:NSUTF32StringEncoding];
                
                NSString * url_fb = [NSString stringWithUTF8String:url.GetString()];
                
                return [[FB_IOS_Helper defaultHelper] PostMessageToFacebook:message_fb withURL:url_fb];
#endif
                return 0;
            }
            
            void GetFrofileFBWithAccessToken()
            {
#if defined USE_SOCIAL_ADS
                [[FB_IOS_Helper defaultHelper]GetFrofileFBWithAccessToken ];
#endif
                
            }
            
            RKString GetIDFacebook()
            {
#if defined USE_SOCIAL_ADS
                return [[FB_IOS_Helper defaultHelper]GetIDFacebook];
#endif
                return "";
                
            }
            
            RKString GetNameFacebook()
            {
#if defined USE_SOCIAL_ADS
                return [[FB_IOS_Helper defaultHelper]GetNameFacebook];
#endif
                return "";
            }
            
            RKString GetURLAvatarFBFromID(RKString user_id)
            {
#if defined USE_SOCIAL_ADS
                NSString * id_str = [NSString stringWithUTF8String:user_id.GetString()];
                return [[FB_IOS_Helper defaultHelper]GetURLAvatarFBFromID:id_str];
#endif
            }
            
        }
    }
}