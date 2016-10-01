#import "CasualTouchesDelegate.h"

#include "CasualCore.h"
#include "RKNSLog.h"

#define TOUCH_THRESHOLD 20
static bool debugFlagPrintState = false;
static bool debugFlagPrintPassedCasualCoreEvents = false;

@implementation CasualTouchesDelegate

-(id) initWithScaleFactor: (float) scalefactor
{
  self = [super init];
  if (self)
  {
    scalefactor_ = scalefactor;
    
    for (int i = 0; i < MAX_TOUCHES; i++)
      touchpoints_[i] = nil;
  }
  return self;
}

- (void)dealloc
{
  for (int i = 0; i < MAX_TOUCHES; i++)
       [touchpoints_[i] release];
  [super dealloc];
}

-(void) processTouchEnd: (UITouch*) touch cancel:(bool)cancel
{
  CGPoint currentPoint = [touch locationInView:[touch view]];
  CGPoint previousPoint = [touch previousLocationInView:[touch view]];
  float diffx = currentPoint.x - lastsingletouch_.x;
  float diffy = currentPoint.y - lastsingletouch_.y;
  if (fabs(diffx) + fabs(diffy) <= TOUCH_THRESHOLD) //Manhatten distance for speed
  {
    currentPoint = lastsingletouch_;
  }
  CGPoint scenePoint;
  scenePoint.x = currentPoint.x * scalefactor_;
  scenePoint.y = currentPoint.y * scalefactor_;
  
  if (!CGPointEqualToPoint(currentPoint, previousPoint))
  {
    if (debugFlagPrintPassedCasualCoreEvents) RKNSLOG(@"InjectTouchDrag()");
    CasualCore::Game::GetInstance()->GetScene()->InjectTouchDrag(scenePoint.x, scenePoint.y);
  }
  
  if (!cancel)
  {
    if (debugFlagPrintPassedCasualCoreEvents) RKNSLOG(@"InjectTouchUp()");
    CasualCore::Game::GetInstance()->GetScene()->InjectTouchUp(scenePoint.x, scenePoint.y);
  }
  else
  {
    if (debugFlagPrintPassedCasualCoreEvents) RKNSLOG(@"InjectTouchCancel()");
    CasualCore::Game::GetInstance()->GetScene()->InjectTouchCancel(scenePoint.x, scenePoint.y);
  }
}


-(void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
  // ANLA: Close the keyboard if we touch outside of it
  if (PLATFORM->IsKeyboardOpen())
  {
    UITouch * touch = [touches anyObject];
    if(touch.phase == UITouchPhaseBegan) {
      PLATFORM->CloseKeyboard();
    }
  }

  [self debugPrintState:@"+touchesBegan" args:touches];
  [self wipeLostTouches];
  
  /*
  NSEnumerator* rawItr = [touches objectEnumerator];
  while (UITouch* touch = [rawItr nextObject])
  {
    CGPoint point = [touch locationInView: [touch view]];
    CGSize size = [[touch view] bounds].size;
    CasualCore::Game::GetInstance()->GetScene()->InjectRawTouchDown((point.x - size.width/2)* self->scalefactor_, (point.y - size.height/2) * self->scalefactor_);
  }
  */
  
  if ((touchcount_ + [touches count]) > 1)
  {
    canceltouch_ = true;
  }
  
  if (touchcount_ == 1 && [touches count] > 0)
  {
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
      if (touchpoints_[i])
      {
        [self processTouchEnd: touchpoints_[i] cancel:canceltouch_];
      }
    }
  }
  
  NSEnumerator* itr = [touches objectEnumerator];
  while (UITouch* touch = [itr nextObject])
  {
    bool foundFreeSlot = false;
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
      if (!touchpoints_[i])
      {
        touchpoints_[i] = [touch retain]; // have to retain it, see wipeLostTouches comment
        ++touchcount_;
        foundFreeSlot = true;
        break;
      }
    }
    if (!foundFreeSlot)
    {
      RKNSLOG(@"touchesBegan() couldn't find free slot to register new touch event");
    }
  }
  
  if (touchcount_ == 1 && [touches count] == 1)
  {
    UITouch* touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
    lastsingletouch_ = point;
    if (debugFlagPrintPassedCasualCoreEvents) RKNSLOG(@"InjectTouchDown()");
    CasualCore::Game::GetInstance()->GetScene()->InjectTouchDown(point.x * scalefactor_, point.y* scalefactor_);
  }
  
  [self debugPrintState:@"-touchesBegan" args:nil];
}

-(void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
  [self debugPrintState:@"+touchesMoved" args:touches];
  [self wipeLostTouches];
  
  if (true) { // sanity check, can be safely removed for retail build
    int foundTouches = 0;
    NSEnumerator* itr = [touches objectEnumerator];
    while (UITouch* touch = [itr nextObject])
    {
      for (int i = 0; i < MAX_TOUCHES; ++i)
      {
        if (touchpoints_[i] == touch)
        {
          ++foundTouches;
          break;
        }
      }
    }
    if (foundTouches != [touches count])
    {
      [self resetAllTouches : @"Sanity check failed"];
      return;
    }
  }
  
  if (touchcount_ == 1)
  {
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
      UITouch* touch = touchpoints_[i];
      if (touchpoints_[i])
      {
        CGPoint point = [touch locationInView: [touch view]];
        if (debugFlagPrintPassedCasualCoreEvents) RKNSLOG(@"InjectTouchDrag()");
        CasualCore::Game::GetInstance()->GetScene()->InjectTouchDrag(point.x * scalefactor_, point.y * scalefactor_);
        break;
      }
    }
  }
  else if (touchcount_ == 2)
  {
    //---- use the 2 valid touch info
    UITouch* touch1 = NULL;
    UITouch* touch2 = NULL;
    
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
      if (touchpoints_[i])
      {
        if (!touch1)
        {
          touch1 = touchpoints_[i];
        }
        else if (!touch2)
        {
          touch2 = touchpoints_[i];
          break;
        }
      }
    }
    
    //----
    if (!touch1 || !touch2)
    {
      [self resetAllTouches : @"Unable to find two valid touches for swipe"];
      return;
    }
    
    CGPoint point1 = [touch1 locationInView: [touch1 view]];
    CGPoint prevPoint1 = [touch1 previousLocationInView: [touch1 view]];
    CGPoint point2 = [touch2 locationInView: [touch2 view]];
    CGPoint prevPoint2 = [touch2 previousLocationInView: [touch2 view]];
    
    float diffx = point1.x - point2.x;
    float diffy = point1.y - point2.y;
    float dist = sqrt(diffx*diffx + diffy*diffy) * self->scalefactor_;
    
    float olddiffx = prevPoint1.x - prevPoint2.x;
    float olddiffy = prevPoint1.y - prevPoint2.y;
    float olddist = sqrt(olddiffx*olddiffx + olddiffy*olddiffy) * self->scalefactor_;
    
    CGPoint midpoint;
    midpoint.x = (point1.x + point2.x) / 2;
    midpoint.y = (point1.y + point2.y) / 2;
    
    if (debugFlagPrintPassedCasualCoreEvents) RKNSLOG(@"InjectPinch()");
    CasualCore::Game::GetInstance()->GetScene()->InjectPinch(midpoint.x * scalefactor_, midpoint.y * scalefactor_, dist, olddist);
    
  } //endif touchcount_ == 2
  
  [self debugPrintState:@"-touchesMoved" args:nil];
}

-(void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
  [self debugPrintState:@"+touchesEnded" args:touches];
  [self wipeLostTouches];
  
  /*
  NSEnumerator* rawItr = [touches objectEnumerator];
  while (UITouch* touch = [rawItr nextObject])
  {
    CGPoint point = [touch locationInView: [touch view]];
    CGSize size = [[touch view] bounds].size;
    CasualCore::Game::GetInstance()->GetScene()->InjectRawTouchUp((point.x - size.width/2)* self->scalefactor_, (point.y - size.height/2) * self->scalefactor_);
  }
   */
  
  int foundCount = 0;
  NSEnumerator* itr = [touches objectEnumerator];
  while (UITouch* touch = [itr nextObject])
  {
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
      if (touchpoints_[i] == touch)
      {
        [touchpoints_[i] release];
        touchpoints_[i] = nil;
        --touchcount_;
        ++foundCount;
      }
    }
  }
  
  if (foundCount != [touches count])
  {
    [self resetAllTouches : @"Couldn't find some touches to deregister"];
    return;
  }
  
  if (touchcount_ < 0)
  {
    [self resetAllTouches : @"touchcount less than zero! hey, that's impossible!"];
    return;
  }
  
  if (touchcount_ == 0)
  {
    if ([touches count] == 1 && canceltouch_ == false)
    {
      UITouch* touch = [touches anyObject];
      [self processTouchEnd:touch cancel:canceltouch_];
    }
    else
    {
      CasualCore::Game::GetInstance()->GetScene()->InjectPinchCancel();
    }
    canceltouch_ = false;
  }
  else if (touchcount_ == 1)  //still one touch ?
  {
    for (int i = 0; i < MAX_TOUCHES; ++i)
    {
      UITouch* touch = touchpoints_[i];
      if (touch)
      {
        CGPoint point = [touch locationInView: [touch view]];
        if (debugFlagPrintPassedCasualCoreEvents) RKNSLOG(@"InjectTouchDown()");
        CasualCore::Game::GetInstance()->GetScene()->InjectTouchDown(point.x * scalefactor_, point.y * scalefactor_);
        break;
      }
    }
  }
  
  [self debugPrintState:@"-touchesEnded" args:nil];
}

-(void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
{
  [self debugPrintState:@"+touchesCancelled" args:touches];
  
  [self touchesEnded: touches withEvent: event];
  
  [self debugPrintState:@"-touchesCancelled" args:nil];
}

-(void) resetAllTouches: (NSString*) reason
{
  RKNSLOG(@"Resetting touches state because of: %@", reason);
  [self debugPrintState:@"+resetAllTouches" args:nil];
  
  for (int i = 0; i < MAX_TOUCHES; ++i)
  {
    if (touchpoints_[i])
    {
      [touchpoints_[i] release];
      touchpoints_[i] = nil;
    }
  }
  touchcount_ = 0;
  canceltouch_ = false;
  
  [self debugPrintState:@"-resetAllTouches" args:nil];
}

//
// here goes workaround for nasty bug of unknown origin
//
// sometimes (quite rare, under special conditions and heavy load)
// CasualViewController does not recieve touchesEnded event
//
// iOS releases UITouches that has been ended, and
// as a result next time we recieve an event,
// stored pointer to UITouch points to invalid memory
//
// previous approach was to copy all information from UITouch
// into our own structure and use this saved data instead of
// accessing UITouch object
//
// but even though we can fix crash by doing that, that won't help
// us to detect and correct the error
//
// if touchesEnded event is lost, we would never know that event
// has actually ended and will have it registered in our touch array forever
// as a result any touch performed will be detected as a second touch
// and user won't be able to interact with application
//
// the workaround here is to retain UITouch objects and detects broken
// touches by looking at retainCount of stored UITouches
//
// if retainCount==1, then it means that iOS has already released
// this object (the only remaining owner is our CasualViewController)
// but we didn't recieve event for some reason
// in this case we wipe such objects from our array
//
- (void)wipeLostTouches;
{
  for (int i = 0; i < MAX_TOUCHES; ++i)
  {
    if (touchpoints_[i])
    {
      if ([touchpoints_[i] retainCount] == 1)
      {
        RKNSLOG(@"wipeLostTouches(): Invalid/broken/lost touch cleaned up: %@", touchpoints_[i]);
        [touchpoints_[i] release];
        touchpoints_[i] = nil;
        --touchcount_;
        [self debugPrintState:@"post-wipeLostTouches()" args:nil];
      }
    }
  }
}

- (void)debugPrintState:(NSString*)stateName  args:(NSSet*)pTouchArgs;
{
  if (!debugFlagPrintState)
  {
    return;
  }
  
  RKNSLOG(@"Touches state %@, touchcount_=%d, canceltouch_=%d", stateName, touchcount_, int(canceltouch_));
  for (int i = 0; i < MAX_TOUCHES; ++i)
  {
    RKNSLOG(@"  Touch %d : %@", i, touchpoints_[i]);
  }
  if (pTouchArgs)
  {
    RKNSLOG(@"  Arguments set [%d]:", [pTouchArgs count]);
    NSEnumerator* itr = [pTouchArgs objectEnumerator];
    int idx = 0;
    while (UITouch* touch = [itr nextObject])
    {
      RKNSLOG(@"    Touch %d : %@", idx, touch);
      ++idx;
    }
  }
}

@end