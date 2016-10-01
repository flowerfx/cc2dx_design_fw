#if defined USE_SOCIAL_ADS
#import "FB_ISO_Helper.h"

#include "../../../FileManager.h"
#include "../../SocialManager.h"

#include "cocos2d.h"
USING_NS_CC;
using namespace Utility;

@implementation FB_IOS_Helper

@synthesize name_facebook_;
@synthesize id_facebook_;
@synthesize email_facebook_;


static FB_IOS_Helper *_sharedHelper_fb = nil;

+ (FB_IOS_Helper*)defaultHelper {
    // dispatch_once will ensure that the method is only called once (thread-safe)
    static dispatch_once_t pred = 0;
    dispatch_once(&pred, ^{
        _sharedHelper_fb = [[FB_IOS_Helper alloc] init];
    });
    return _sharedHelper_fb;
}

- (id)init {
    if ((self = [super init])) {
    }
    email_facebook_ = "";
    id_facebook_ = "";
    email_facebook_ = "";
    return self;
    
}

-(void)LoginFacebook
{
    //create view controller
    
    if(File::SaveMgr->GetDataSave()->GetcharByName(SAVE_STR_FB_TOKEN) == "" || [FBSDKAccessToken currentAccessToken] == nil)
    {
        
       // add to root view first
        auto view = cocos2d::Director::getInstance()->getOpenGLView();
        CCEGLView *eaglview = (CCEGLView *) view->getEAGLView();
        
        [eaglview addSubview:self.view];
        
        NSLog(@"Fb iOS sdk version: %@", [FBSDKSettings sdkVersion]);
        
        SocialMgr->SetStateLoginFB(Social::FACEBOOK_STATE_LOGIN::FB_ON_LOGGING);
        
        // call a login ui by this view controller
        FBSDKLoginManager *loginManager = [[FBSDKLoginManager alloc] init];
        [loginManager logInWithReadPermissions:@[@"public_profile", @"email"]
                            fromViewController:self
                                       handler:^(FBSDKLoginManagerLoginResult *result, NSError *error) {
                                           
                                           //TODO: process error or result
                                           
                                           if (error)
                                           {
                                               NSLog(@"Process error");
                                               SocialMgr->SetStateLoginFB(Social::FACEBOOK_STATE_LOGIN::FB_FAILED_LOGIN);
                                           }
                                           else if (result.isCancelled)
                                           {
                                               SocialMgr->SetStateLoginFB(Social::FACEBOOK_STATE_LOGIN::FB_NONE);
                                               NSLog(@"Cancelled");
                                           }
                                           else
                                           {
                                               NSString * fbaccessToken = [FBSDKAccessToken currentAccessToken].tokenString;
                                               
                                               NSLog(@"accesstoken : %@" ,fbaccessToken);
                                               //save the access token in file
                                               File::SaveMgr->GetDataSave()->SetCharAtName(SAVE_STR_FB_TOKEN, [fbaccessToken UTF8String]);
                                               File::SaveMgr->SaveTheGameSave();
                                               
                                               [self GetFrofileFBWithAccessToken];
                                               
                                               NSLog(@"Logged in");
                                           }
                                       }];
    }
    else
    {
        NSLog(@"Already Logged in");
        //
        //login with access token
        //
        SocialMgr->SetStateLoginFB(Social::FACEBOOK_STATE_LOGIN::FB_LOGIN_SUCCEED);
    }
}

- (void)GetFrofileFBWithAccessToken
{
        
        RKString access_token = File::SaveMgr->GetDataSave()->GetcharByName(SAVE_STR_FB_TOKEN);
        
        
        if(access_token == "")
        {
            return;
            SocialMgr->SetStateLoginFB(Social::FACEBOOK_STATE_LOGIN::FB_NONE);
        }
        else
        {
            //get name and profile of user
            FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc] initWithGraphPath:@"me" parameters:nil];
            [request startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
                if (!error) {
                    // result is a dictionary with the user's Facebook data
                    NSDictionary *userData = (NSDictionary *)result;
                    
                    self->id_facebook_ = [userData[@"id"] UTF8String];
                    self->name_facebook_ = [userData[@"name"] UTF8String] ;
                    NSString *location = userData[@"location"][@"name"];
                    NSString *gender = userData[@"gender"];
                    NSString *birthday = userData[@"birthday"];
                    NSString *relationship = userData[@"relationship_status"];
                    
                    // NSURL *pictureURL = [NSURL URLWithString:[NSString stringWithFormat:@"https://graph.facebook.com/%@/picture?type=large&return_ssl_resources=1",NSString stringWithUTF8String:id_facebook_.getString()]];
                    
                    // Now add the data to the UI elements
                    // ...
                    SocialMgr->SetStateLoginFB(Social::FACEBOOK_STATE_LOGIN::FB_LOGIN_SUCCEED);
                    
                    SocialMgr->DownloadAvatarFB( [self GetURLAvatarFBFromID:[NSString stringWithUTF8String:self->id_facebook_.GetString()]] , "fb:" + id_facebook_ + ".png");
                }
                else{
                    SocialMgr->SetStateLoginFB(Social::FACEBOOK_STATE_LOGIN::FB_FAILED_LOGIN);
                    NSLog(@"error: %@",error.description);
                }
            }];
        }
}

- (int) PostMessageToFacebook:(NSString*)message withURL:(NSString*) url
{
    if(SocialMgr->CurrentStateLogin() != Social::FACEBOOK_STATE_LOGIN::FB_LOGIN_SUCCEED)
        return 1;
    
    FBSDKShareLinkContent *content = [[FBSDKShareLinkContent alloc] init];
    content.contentURL = [NSURL URLWithString:url];
    content.contentDescription = message;
    
    //add to root view first
    auto view = cocos2d::Director::getInstance()->getOpenGLView();
    CCEGLView *eaglview = (CCEGLView *) view->getEAGLView();
    
    [eaglview addSubview:self.view];
    
    FBSDKShareDialog *dialog = [[FBSDKShareDialog alloc] init];
    
    
    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@"fbauth2://"]]){
        dialog.mode = FBSDKShareDialogModeShareSheet;
    }
    else {
        dialog.mode = FBSDKShareDialogModeBrowser; //or FBSDKShareDialogModeAutomatic
    }
    dialog.shareContent = content;
    dialog.delegate = self;
    dialog.fromViewController = self;
    [dialog show];
    
    return 0;
    
    if(![[FBSDKAccessToken currentAccessToken]hasGranted:@"publish_actions"])
        return 2;
    
    [[[FBSDKGraphRequest alloc] initWithGraphPath:@"me/feed"
                                       parameters:@{ @"message" : @"This is a status update" }
                                       HTTPMethod:@"POST"]
     startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
         if ([error.userInfo[FBSDKGraphRequestErrorGraphErrorCode] isEqual:@200])
         {
             
             NSLog(@"permission error");
         }
     }];
}

- (RKString) GetIDFacebook
{
    return id_facebook_;
}

- (RKString) GetNameFacebook
{
    return name_facebook_;
}

- (RKString) GetURLAvatarFBFromID:(NSString*) user_id
{
    NSString * url_img = [NSString stringWithFormat:@"%@%@%@",@"https://graph.facebook.com/" , user_id , @"/picture?type=square" ];
    return [url_img UTF8String];
}

#pragma mark - FBSDKSharingDelegate
- (void)sharer:(id<FBSDKSharing>)sharer didCompleteWithResults :(NSDictionary *)results {
    NSLog(@"FB: SHARE RESULTS=%@\n",[results debugDescription]);
}

- (void)sharer:(id<FBSDKSharing>)sharer didFailWithError:(NSError *)error {
    NSLog(@"FB: ERROR=%@\n",[error debugDescription]);
}

- (void)sharerDidCancel:(id<FBSDKSharing>)sharer {
    NSLog(@"FB: CANCELED SHARER=%@\n",[sharer debugDescription]);
}
@end
#endif
