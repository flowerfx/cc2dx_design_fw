//
//  YouTubeView.h
//  blogScratchApp
//
//  Created by John on 5/18/10.
//  Copyright 2010 iPhoneDeveloperTips.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface YouTubeView : UIWebView <UIWebViewDelegate>
{
  NSString *_videoId;
  CGRect _frame;
  bool _watchedToEnd;
}

- (YouTubeView *)initWithStringAsURL:(NSString *)urlString frame:(CGRect)frame isChinese:(bool)inChina :(bool)disableCloseWhilePlaying;
- (void) showPlayButton;
- (void) showCloseButton;
- (void) close;
- (NSString *)getVideoId;
@end
