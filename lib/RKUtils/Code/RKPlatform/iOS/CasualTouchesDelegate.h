#ifndef __CASUALCORE_CASUALTOUCHESDELEGATE_H_
#define __CASUALCORE_CASUALTOUCHESDELEGATE_H_

#import <UIKit/UIKit.h>

#define MAX_TOUCHES 5

@interface CasualTouchesDelegate : NSObject {
  float scalefactor_;
  
  UITouch* touchpoints_[MAX_TOUCHES];
  int touchcount_;
  bool canceltouch_;
  bool texturesUnloaded_;
  CGPoint lastsingletouch_;
}

-(id) initWithScaleFactor: (float) scalefactor;
-(void) resetAllTouches: (NSString*) reason;

-(void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event;
-(void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;
-(void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;
-(void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event;

@end

#endif // __CASUALCORE_CASUALTOUCHESDELEGATE_H_
