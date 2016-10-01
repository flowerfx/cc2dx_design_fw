
#include "platform/CCPlatformConfig.h"
#include "cocos2d.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#include "Platform_ios.h"
#include "renderer/CCRenderer.h"
#include "base/CCDirector.h"
#include "platform/CCGLView.h"
#include "platform/ios/CCEAGLView-ios.h"
#include "platform/CCFileUtils.h"
#include "UIKit/UIImage.h"
#include "UIKit/UIButton.h"

@interface WebViewWrapper : NSObject

@property (nonatomic) UIView* _view;
@property (nonatomic) UIWebView* _webView;


+ (instancetype)webViewWrapper;
- (void)initWebView;
- (void)loadUrl:(const std::string &) url;
- (void)hide;
- (void)cellButtonPressed;
@end


@implementation WebViewWrapper {
    
}


+ (instancetype)webViewWrapper {
    return [[[self alloc] init] autorelease];
}

- (instancetype)init {
    self = [super init];
    if(self){
        
        [self initWebView];
    }
    return self;
}

- (void)initWebView {
    if(self._view == nullptr)
    {
        auto view = cocos2d::Director::getInstance()->getOpenGLView();
        auto eaglview = (CCEAGLView *) view->getEAGLView();
    
        int padding = 17;
    
        cocos2d::Size device_size = cocos2d::Size(eaglview.frame.size.width, eaglview.frame.size.height);//cocos2d::Director::getInstance()->getVisibleSize();
        UIImage* image = [UIImage imageNamed:@"close_button.png"];
        UIButton* btnClose = [UIButton buttonWithType:UIButtonTypeRoundedRect ];
        btnClose.frame = CGRectMake(0, 0, image.size.width, image.size.height);
        [btnClose setImage:image forState:UIControlStateNormal];
        [btnClose addTarget:self action:@selector(cellButtonPressed) forControlEvents:UIControlEventTouchUpInside];
        btnClose.center = CGPointMake(device_size.width - image.size.width / 2, image.size.height / 2);
        self._view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, device_size.width, device_size.height)];
        self._webView = [[UIWebView alloc] initWithFrame:CGRectMake(padding, padding, device_size.width - padding * 2, device_size.height - padding * 2)];
        [self._view addSubview:self._webView];
        [self._view addSubview:btnClose];
    
        [eaglview addSubview:self._view];
  
    }
    
}

- (void)cellButtonPressed
{
    [self hide];
}

-(void)loadUrl:(const std::string &)urlString
{
    self._view.hidden = false;
    NSURL *url = [NSURL URLWithString:@(urlString.c_str())];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    [self._webView loadRequest:request];
    
}

-(void)hide
{
    self._view.hidden = true;
}

@end

#define SYSTEM_VERSION_EQUAL_TO(v)                  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
#define SYSTEM_VERSION_GREATER_THAN(v)              ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN(v)                 ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)



namespace Platform
{
    namespace ios{
        
        WebViewWrapper* s_webview = nullptr;
        
        void nativeShowWebView(std::string url)
        {
            if(s_webview == nullptr)
            {
                s_webview = [WebViewWrapper webViewWrapper];
                [s_webview retain];
                
            }
            [s_webview loadUrl:url];
        }
        
        void nativeHideWebView()
        {
            [s_webview hide];
        }
        
        void nativeMakeCall(std::string phoneNumber)
        {
            NSString* phone_num = [NSString stringWithUTF8String:phoneNumber.c_str()];
            NSString* url = [@"telprompt://" stringByAppendingString:phone_num];
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
        }
        
        void nativeShowSMSComposer(std::string phoneNumber, std::string body)
        {
            if( SYSTEM_VERSION_LESS_THAN(@"6.0.0"))
            {
                
            }
            else
            {
                [[UIPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:body.c_str()]];
                NSString* phone_num = [NSString stringWithUTF8String:phoneNumber.c_str()];
                NSString* msg_body = [NSString stringWithUTF8String:body.c_str()];
                NSString* url = [[NSString stringWithFormat:@"sms:%@&body=%@", phone_num, msg_body] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                NSLog(url);
                if( [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:url]] )
                {
                    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url] ];
                }

            }
        }
        
        std::string nativeGetBundleId()
        {
            return [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
        }
        
        std::string nativeGetAppName()
        {
            return [[[[NSBundle mainBundle] localizedInfoDictionary] objectForKey:@"CFBundleDisplayName"] UTF8String];
        }
    }

}

#endif

