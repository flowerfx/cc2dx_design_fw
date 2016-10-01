#if 0
#import "CasualMoviePlayer.h"
#import "CasualViewController.h"

#include "Game.h"

@implementation UILabelOutlined

- (void)drawTextInRect:(CGRect)rect
{
  CGSize shadowOffset = self.shadowOffset;
  UIColor *textColor = self.textColor;
  
  CGContextRef c = UIGraphicsGetCurrentContext();
  CGContextSetLineWidth(c, 1);
  CGContextSetLineJoin(c, kCGLineJoinRound);
  
  CGContextSetTextDrawingMode(c, kCGTextStroke);
  self.textColor = [UIColor blackColor];
  [super drawTextInRect:rect];
  
  CGContextSetTextDrawingMode(c, kCGTextFill);
  self.textColor = textColor;
  self.shadowOffset = CGSizeMake(0, 0);
  [super drawTextInRect:rect];
  
  self.shadowOffset = shadowOffset;
}

/*- (void)dealloc
{
  [outlineColor release];
  [super dealloc];
}*/

@end

@implementation CasualMoviePlayer

- (id) initWithViewController:(CasualViewController *)casualViewController
{
  self = [super init];
  if (self)
  {
    viewController = casualViewController;
    moviePaused = false;
  }
  return self;
}

- (void)dealloc
{
  [moviePlayer release];
  [movieSkipButton release];
  [movieOverlayText release];
  [movieSubtitles release];
  [super dealloc];
}

- (void) playMovie:(const char*)movieFile
{
  NSString* path = [[NSString alloc] initWithUTF8String:movieFile];
  [moviePlayer release];
  moviePlayer = [[MPMoviePlayerController alloc] initWithContentURL:[NSURL fileURLWithPath:path]];
  
  moviePlayer.controlStyle = MPMovieControlStyleNone;
  moviePlayer.fullscreen = true;
  //moviePlayer.useApplicationAudioSession = true; // Deprecated in iOS 6.0. + The default value of this property is YES + There is not replacement for this property and its use is discouraged.
  
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(movieFinishedCallback:) name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(forceMoviePlayback:) name:MPMoviePlayerPlaybackStateDidChangeNotification object:nil];

  UIViewController * root = [UIApplication sharedApplication].keyWindow.rootViewController;
  CGRect rectViewFrame = [root.view bounds];

  CGRect rectFrame = rectViewFrame;//[[UIScreen mainScreen] bounds];
  moviePlayer.view.frame = rectFrame;//CGRectMake(0, 0, rectFrame.size.height, rectFrame.size.width);
  [viewController.view addSubview:moviePlayer.view];
  //[self.viewController.view setUserInteractionEnabled:false];
  
  
  // create the label if need be
  if (movieOverlayText == nil)
  {
    CGRect screenRect = rectViewFrame;//[[UIScreen mainScreen] bounds];
    
    movieOverlayText = [[UILabel alloc] initWithFrame:CGRectMake(0,screenRect.size.height * 0.01f,screenRect.size.width,screenRect.size.height * 0.2f) ];
    // bottom left align
    movieOverlayText.textColor = [UIColor whiteColor];
    movieOverlayText.backgroundColor = [UIColor clearColor];
    movieOverlayText.textAlignment = NSTextAlignmentRight;	//deprecated in iOS 6.0 : UITextAlignmentRight;
    //movieOverlayText.baselineAdjustment = UIBaselineAdjustmentAlignBaselines;
    [moviePlayer.view addSubview:movieOverlayText];
  }
  
  if (movieSubtitles == nil)
  {
    CGRect screenRect = rectViewFrame;//[[UIScreen mainScreen] bounds];
    subTitlesLineWidth = screenRect.size.width;
    
    bool bIsIpad = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
    
    float fSubsVerticalPosition;
    
    if (bIsIpad)
    {
      fSubsVerticalPosition = screenRect.size.height  * 0.82f;
    }
    else
    {
      fSubsVerticalPosition = screenRect.size.height  * 0.75f;
      CasualCore::Game::GetInstance()->GetPlatform()->LockOrientation(true);
    }
    
    movieSubtitles = [[UILabelOutlined alloc] initWithFrame:CGRectMake(0,
                                                               fSubsVerticalPosition,
                                                               screenRect.size.width,
                                                            screenRect.size.height - fSubsVerticalPosition)];
        
    NSString* strFontName = [NSString stringWithUTF8String:PLATFORM->GetSubTitlesFontName()];
    [movieSubtitles setFont:[UIFont fontWithName:strFontName size:20.0f]];
    movieSubtitles.lineBreakMode = NSLineBreakByWordWrapping;
    movieSubtitles.numberOfLines = 3;
    movieSubtitles.textColor = [UIColor whiteColor];
    movieSubtitles.backgroundColor = [UIColor clearColor];
    movieSubtitles.textAlignment = NSTextAlignmentCenter;  //deprecated in iOS 6.0 : UITextAlignmentCenter;
    //movieSubtitles.shadowColor = [UIColor blackColor];
    //movieSubtitles.shadowOffset = CGSizeMake(0,1);
    
    [moviePlayer.view addSubview:movieSubtitles];
  }
  
  
  UILabel *pLabel = [[UILabel alloc] initWithFrame:CGRectMake(50, 100, 200, 100)];
  NSString* strFontName = [NSString stringWithUTF8String:PLATFORM->GetSubTitlesFontName()];
  [pLabel setFont:[UIFont fontWithName:strFontName size:20.0]];
  NSString* character = @"W";
  subTitlesCharacterWidth = [character sizeWithFont:pLabel.font].width;
  [pLabel release];
  
  [moviePlayer prepareToPlay];
  
  [moviePlayer play];
  moviePaused = false;
  
  [path release];
}

- (void) stopMovie
{
  if ((UI_USER_INTERFACE_IDIOM() != UIUserInterfaceIdiomPad))
  {
    CasualCore::Game::GetInstance()->GetPlatform()->LockOrientation(false);
    CasualCore::Game::GetInstance()->GetPlatform()->TryCorrectOrientation();
  }
  
  if (moviePlayer)
  {
    // remove skip button
    if (movieSkipButton)
    {
      [movieSkipButton removeFromSuperview];
      [movieSkipButton release];
      movieSkipButton = nil;
    }
    
    if (movieOverlayText)
    {
      [movieOverlayText removeFromSuperview];
      [movieOverlayText release];
      movieOverlayText = nil;
    }
    
    // remove observers
    [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackStateDidChangeNotification object:nil];
    
    // ensure the movie is stopped
    [moviePlayer pause];
    [moviePlayer stop];
    
    // re-enable touch and remove movie view
    [viewController resetAllTouches];
    [viewController.view setUserInteractionEnabled:true];
    [moviePlayer.view removeFromSuperview];
    
    // clean up the movie player
    [moviePlayer release];
    moviePlayer = nil;
    
    CasualCore::Game::GetInstance()->GetPlatform()->OnVideoStateChanged(CasualCore::VIDEO_COMPLETE);
  }
}

- (void) pause
{
  moviePaused = true;
  [moviePlayer pause];
}

- (void) resume
{
  if (moviePaused)
  {
    moviePaused = false;
    [moviePlayer play];
  }
}

-(void) setMoviePlaybackTime
{
  float playBackTime = 0.0f;
  
  if (moviePlayer != nil)
  {
    if (moviePlayer.playbackState == MPMoviePlaybackStatePlaying)
    {
      playBackTime = (float)moviePlayer.currentPlaybackTime;
    }
  }
  
  CasualCore::Game::GetInstance()->GetPlatform()->SetVideoPlaybackTime(playBackTime);
}

-(void) setSubtitlesText:(NSString*) text
{
  [movieSubtitles setText:text];
  [movieSubtitles setNeedsDisplay];
}

// a text field at the bottom left of the screen, for displaying the download percentage
-(void) setMovieDownloadOverlayText:(const wchar_t*)sLocalisedText withPercentage:(int)percentage
{
  // convert wchar_t text to NSString
  NSString* localizedText = [[NSString alloc] initWithBytes:sLocalisedText
                                                     length:wcslen(sLocalisedText)*sizeof(wchar_t)
                                                   encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
  
  NSString* downloadString = [[NSString alloc]  initWithFormat:@"%@ %0d%%", localizedText, percentage];
  
  if (movieOverlayText != nil)
  {
    //set the text
    [movieOverlayText setText:downloadString];
  }
  
  [downloadString release];
  [localizedText release];
}

-(void) setSkipButton:(const wchar_t*)sLocalisedText  buttonUpImage:(const char*)sBtnUpImage  buttonDownImage:(const char*)sBtnDownImage  xPos:(int)x  yPos:(int)y  width:(int)w  height:(int)h
{
  // clear any existing skip button
  if (movieSkipButton)
  {
    [movieSkipButton removeFromSuperview];
    [movieSkipButton release];
    movieSkipButton = nil;
  }
  
  if (moviePlayer)
  {
    // create a new skip button
    movieSkipButton = [[UIButton buttonWithType:UIButtonTypeCustom] retain];
    NSString* strFontName = [NSString stringWithUTF8String:PLATFORM->GetSubTitlesFontName()];
    movieSkipButton.titleLabel.font = [UIFont fontWithName:strFontName size:20.0f];
    // convert wchar_t text to NSString
    NSString* localizedText = [[NSString alloc] initWithBytes:sLocalisedText
                                                       length:wcslen(sLocalisedText)*sizeof(wchar_t)
                                                     encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
    
    const wchar_t* wspacingText = L"QQ";
    // convert wchar_t text to NSString
    NSString* spacingText = [[NSString alloc] initWithBytes:wspacingText
                                                     length:wcslen(wspacingText)*sizeof(wchar_t)
                                                   encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
    
    int buttonWidth = [localizedText sizeWithFont:movieSkipButton.titleLabel.font].width + [spacingText sizeWithFont:movieSkipButton.titleLabel.font].width;
    x = moviePlayer.view.bounds.size.width - buttonWidth;
    //y = h =[spacingText sizeWithFont:movieSkipButton.titleLabel.font].height * 0.5f;
    
    movieSkipButton.titleLabel.shadowOffset = CGSizeMake(0.0f,1.0f);
    [movieSkipButton setTitleShadowColor: [UIColor blackColor] forState:UIControlStateNormal];
    [movieSkipButton setTitle:localizedText forState:UIControlStateNormal];
    NSString* buttonDown = [[NSString alloc] initWithCString:sBtnDownImage encoding:NSUTF8StringEncoding];
    NSString* buttonUp = [[NSString alloc] initWithCString:sBtnUpImage encoding:NSUTF8StringEncoding];
    [movieSkipButton setBackgroundImage:[UIImage imageNamed:buttonUp] forState:UIControlStateNormal];
    [movieSkipButton setBackgroundImage:[UIImage imageNamed:buttonDown] forState:UIControlStateNormal];
    [movieSkipButton addTarget:self action:@selector(stopMovie) forControlEvents:UIControlEventTouchUpInside];
    
      
    //CGRect rectFrame = [[UIScreen mainScreen] bounds];
    // OJW - 20120719 - Fix bug 5190255 - Skip button not showing
    // Adjust coordinates by screen scale factor. This assumes you passed in co-ordinates scaled by PLATFORM->GetScreenDimensions
    // which for iPhone 4 would return 960 by 640 instead of 480 by 320 which the UI expects
    CGFloat scaleFactor = 1.0f / [moviePlayer.view contentScaleFactor];
    x *= scaleFactor;
    y *= scaleFactor;
    w *= scaleFactor;
    h /= [viewController.view contentScaleFactor];
    movieSkipButton.frame = CGRectMake(x,y,buttonWidth,h);
    [moviePlayer.view addSubview:movieSkipButton];
    
    [localizedText release];
    [spacingText release];
    [buttonUp release];
    [buttonDown release];
  }
}

- (void) forceMoviePlayback:(NSNotification *)aNotification
{
  
  if (!moviePlayer)
    return;
  
  if (moviePlayer.playbackState == MPMoviePlaybackStatePlaying)
  {
    CasualCore::Game::GetInstance()->GetPlatform()->OnVideoStateChanged(CasualCore::VIDEO_PLAYING);
  }
  else if (moviePlayer.playbackState == MPMoviePlaybackStatePaused)
  {
    // Fix bug 5194481
    if (!moviePaused)
    {
      [moviePlayer play];
    }
    else
    {
      CasualCore::Game::GetInstance()->GetPlatform()->OnVideoStateChanged(CasualCore::VIDEO_PAUSED);
    }
  }
  else if (moviePlayer.playbackState == MPMoviePlaybackStateInterrupted)
  {
    [moviePlayer play];
  }
  else if (moviePlayer.playbackState == MPMoviePlaybackStateStopped)
  {
    CasualCore::Game::GetInstance()->GetPlatform()->OnVideoStateChanged(CasualCore::VIDEO_STOPPED);
    [[NSNotificationCenter defaultCenter] postNotificationName:MPMoviePlayerPlaybackDidFinishNotification object:nil];
  }
  
}

- (void) movieFinishedCallback:(NSNotification*)aNotification
{
  movieSkipButton.hidden = YES;
  [self stopMovie];
}

- (int) getSubTitlesCharacterWidth
{
  return subTitlesCharacterWidth;
}

- (int) getSubTitlesLineWidth
{
  return subTitlesLineWidth;
}

@end

#endif