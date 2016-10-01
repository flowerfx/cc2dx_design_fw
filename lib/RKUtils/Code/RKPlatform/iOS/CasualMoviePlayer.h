#import "MediaPlayer/MPMoviePlayerController.h"

@interface UILabelOutlined : UILabel
{
  //UIColor* outlineColor;
}

- (void)drawTextInRect:(CGRect)rect;

@end

@class CasualViewController;

@interface CasualMoviePlayer : NSObject
{
  CasualViewController*    viewController;
  MPMoviePlayerController* moviePlayer;
  UIButton*                movieSkipButton;
  UILabel*                 movieOverlayText;
  UILabelOutlined*         movieSubtitles;
  bool                     moviePaused;
  int                      subTitlesCharacterWidth;
  int                      subTitlesLineWidth;
}

- (id) initWithViewController: (CasualViewController*) viewController;

- (void) playMovie:(const char*)movieFile;
- (void) stopMovie;

- (void) pause;
- (void) resume;

- (void) setSkipButton:(const wchar_t*)sLocalisedText
        buttonUpImage:(const char*)sBtnUpImage
      buttonDownImage:(const char*)sBtnDownImage
                 xPos:(int)x  yPos:(int)y  width:(int)w  height:(int)h;

- (void) setSubtitlesText:(NSString*)text;
- (void) setMoviePlaybackTime;
- (void) setMovieDownloadOverlayText:(const wchar_t*)sLocalisedText withPercentage:(int)percentage;
- (int) getSubTitlesCharacterWidth;
- (int) getSubTitlesLineWidth;

@end
