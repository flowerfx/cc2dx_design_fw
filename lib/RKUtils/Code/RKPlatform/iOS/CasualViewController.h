#ifndef __CASUALCORE_CASUALVIEWCONTROLLER_H_
#define __CASUALCORE_CASUALVIEWCONTROLLER_H_

#import <UIKit/UIKit.h>

#define MAX_TOUCHES 5

@class CADisplayLink;
@class EAGLContext;
@class CasualTouchesDelegate;

@interface CasualViewController : UIViewController
{
  CADisplayLink* displaylink_;
  CasualTouchesDelegate* touchesdelegate_;

  bool casualcoreinited_;
  int pausecounter_;
  
  UIInterfaceOrientation preferredScreenOrientation_;
  UIInterfaceOrientation currentOrientation_;
  BOOL orientationLocked_;
  BOOL renderBufferLocked_;
  
  bool texturesUnloaded_;
  bool runninglean_;

  unsigned int renderbuffer_;
  unsigned int depthbuffer_;
  unsigned int framebuffer_;
  unsigned int msaarenderbuffer_;
  unsigned int msaaframebuffer_;
  unsigned int msaadepthbuffer_;
  bool enablemsaa_;
}

- (void)resetAllTouches;
- (void)pauseDisplay:(BOOL)pause;
- (void)handleSwipeFrom:(UISwipeGestureRecognizer *) recognizer;
- (void)handleTapFrom:(UITapGestureRecognizer *) recognizer;
- (void)handleLongPressFrom:(UILongPressGestureRecognizer *) recognizer;
- (void)subscribeGesture: (int) gestureid;
- (void)unsubscribeGesture: (int) gestureid;
- (void)unsubscribeAllGestures;
- (BOOL)prefersStatusBarHidden;
- (void)runLean: (BOOL)lean;
- (void)suspendGL;
- (void)tryCorrectOrientation;
- (void)enableMSAA: (bool) enabled;

@property (retain, nonatomic) CasualTouchesDelegate* touchesdelegate;
@property (retain, nonatomic) EAGLContext* context;
@property (retain, nonatomic) CADisplayLink* displayLink;
@property (nonatomic) BOOL orientationLocked;
@property (nonatomic) BOOL renderBufferLocked;

@end

#endif
