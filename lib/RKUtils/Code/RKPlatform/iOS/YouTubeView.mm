//
//  YouTubeView.m
//  blogScratchApp
//
//  Created by John on 5/18/10.
//  Copyright 2010 iPhoneDeveloperTips.com. All rights reserved.
//

#import "YouTubeView.h"
#import "CasualAppDelegate.h"
#include "Game.h"
#include "StateManager/State.h"

@implementation YouTubeView

#pragma mark -
#pragma mark Initialization

- (YouTubeView *)initWithStringAsURL:(NSString *)urlString frame:(CGRect)frame isChinese:(bool)inChina :(bool)disableCloseWhilePlaying;
{
  if (self = [super initWithFrame:frame]) 
  {
    _videoId = [urlString copy];
    _frame = frame;
    _watchedToEnd = false;
    self.delegate = self;
    self.mediaPlaybackRequiresUserAction = FALSE;
    self.allowsInlineMediaPlayback = TRUE;
    [self showPlayButton];
    
    if (!disableCloseWhilePlaying)
    {
      [self showCloseButton];
    }
	}
  
  return self;  
}

#pragma mark -
#pragma mark Cleanup

/*---------------------------------------------------------------------------
*  
*--------------------------------------------------------------------------*/
- (void)dealloc
{
  [_videoId release];
	[super dealloc];
}

- (void) showPlayButton
{
  NSString *html;
  
  //if (!inChina)
  //{
    // HTML to embed YouTube video
    NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"youtube" ofType:@"html"];
    NSString *youTubeVideoHTML = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
    // Populate HTML with the URL and requested frame size
    html = [NSString stringWithFormat:youTubeVideoHTML, _frame.size.width, _frame.size.height, _videoId];
  /*}
  else
  {
    // HTML to embed YouTube video
    NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"youku" ofType:@"html"];
    NSString *youKuVideoHTML = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
    // Populate HTML with the URL and requested frame size
    html = [NSString stringWithFormat:youKuVideoHTML, _frame.size.width, _frame.size.height, _videoId, _videoId, _frame.size.width, _frame.size.height ];
  }*/
  // Load the html into the webview
  [self loadHTMLString:html baseURL:nil];
  
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(youTubeVidFinished:) name:@"AVPlayerItemDidPlayToEndTimeNotification" object:nil];
}

- (void) showCloseButton
{
  // Add a button to close the view
  UIButton* button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
  button.frame = CGRectMake( _frame.size.width - 50, 25, 25, 25 );
  [button setTitle:@"X" forState:UIControlStateNormal];
  [button addTarget:self action:@selector(close) forControlEvents:UIControlEventTouchUpInside];
  
  [self addSubview:button];
}

- (void) youTubeVidFinished:(NSNotification*)aNotification
{
  _watchedToEnd = true;
  [self showPlayButton];
  //[self close];
}

- (void)close
{
  CasualCore::State* pMovieTheatre = CGAME->FindState("MovieTheatreState");
  if (pMovieTheatre)
  {
    pMovieTheatre->OnYouTubeVideoFinished(_watchedToEnd);
  }
  
  CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
  [appdel closeYouTube];
}

- (NSString *)getVideoId
{
  return _videoId;
}

@end
