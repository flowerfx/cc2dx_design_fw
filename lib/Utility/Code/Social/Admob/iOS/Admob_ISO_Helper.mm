#if defined USE_SOCIAL_ADS
#import "Admob_ISO_Helper.h"
#include "Admob_IOS_Wrapper.h"
#include "cocos2d.h"
USING_NS_CC;

@implementation Admob_IOS_Helper


- (instancetype)initWithContentViewController:(UIViewController *)contentController andAppId:(NSString *)id_app
{
    self = [super init];
    if (self != nil) {
        _bannerView = [[GADBannerView alloc] initWithAdSize: kGADAdSizeSmartBannerLandscape];
        _bannerView.adUnitID = id_app;
        _bannerView.delegate = self;
        _contentController = contentController;
        _bannerLoaded = NO;

    }
    return self;
}

- (void)loadView
{
    UIView *contentView = [[UIView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    [self addChildViewController:_contentController];
    [contentView addSubview:_contentController.view];
    [_contentController didMoveToParentViewController:self];
    self.view = contentView;
}

#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_6_0
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return [_contentController shouldAutorotateToInterfaceOrientation:interfaceOrientation];
}
#endif

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
    return [_contentController preferredInterfaceOrientationForPresentation];
}

- (NSUInteger)supportedInterfaceOrientations
{
    return [_contentController supportedInterfaceOrientations];
}

// For animation
- (void)viewDidLayoutSubviews
{
    if([[Admob_IOS_Wrapper defaultHelper] is_video_current_play ])
    {
        NSLog(@"have video play !");
        return;
    }
    
    CGRect contentFrame = self.view.bounds;
    CGRect bannerFrame = CGRectZero;
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_6_0
    bannerFrame = _bannerView.frame;
#else
    bannerFrame.size = [_bannerView sizeThatFits:contentFrame.size];
#endif
    
    float width_screen = [[UIScreen mainScreen]bounds].size.width;
    float height_screen = [[UIScreen mainScreen]bounds].size.height;
    
    bannerFrame.size.width = width_screen * 2 / 3;
    bannerFrame.size.height = height_screen / 2;
    
    bannerFrame.origin.x = width_screen / 6;
    
    if (_bannerLoaded)
    {
        bannerFrame.origin.y = bannerFrame.size.height / 4;
    }
    else
    {
        bannerFrame.origin.y = contentFrame.size.height;
    }
    
    NSLog(@"banner frame: x: %f , y:%f, w: %f , h: %f",bannerFrame.origin.x,bannerFrame.origin.y,bannerFrame.size.width,bannerFrame.size.height);
    
    _contentController.view.frame = contentFrame;
    _bannerView.frame = bannerFrame;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _bannerView.rootViewController = self;
}

- (void)adViewDidReceiveAd:(GADBannerView *)bannerView
{
    if([[Admob_IOS_Wrapper defaultHelper] is_video_current_play ])
    {
        [bannerView autorelease];
        NSLog(@"have video play !");
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
    
        //[NSThread sleepForTimeInterval:1.0]; //sleep for 1 sec
        NSLog(@"adViewDidReceiveAd");
        _bannerLoaded = YES;
    
        [_bannerView removeFromSuperview];
        //[_bannerView autorelease];
        _bannerView = nil;
        _bannerView = bannerView;
        
    
        [self.view addSubview:_bannerView];
    
        [UIView animateWithDuration:0.25 animations:^{
            [self.view setNeedsLayout];
            [self.view layoutIfNeeded];
        }];
    
     Utility::Social::Ads::SetVisibleButonAds(TRUE);
    });
    
}

- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error
{
    NSLog(@"adView didFailToReceiveAdWithError");
    _bannerLoaded = NO;
    [UIView animateWithDuration:0.25 animations:^{
        [self.view setNeedsLayout];
        [self.view layoutIfNeeded];
    }];
    
    Utility::Social::Ads::SetVisibleButonAds(false);
}

- (void) hideBanner{
    
    [_bannerView removeFromSuperview];
    _bannerLoaded = NO ;
}

- (void) showBanner{
    if(_bannerLoaded == YES)
        return;
    
    GADRequest *request = [GADRequest request];
    //request.testDevices =  @[ @"da31e17444b4100139085786205988c9" ];
    [_bannerView loadRequest:request];
}

- (void)dealloc {
    _bannerView.delegate = nil;
    [_bannerView release];
    [super dealloc];
}

@end

#endif


