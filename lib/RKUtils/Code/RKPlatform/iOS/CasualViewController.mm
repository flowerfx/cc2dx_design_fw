#import "CasualViewController.h"
#import "CasualView.h"

#import <OpenGLES/EAGL.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <QuartzCore/CADisplayLink.h>
#import "CasualTouchesDelegate.h"
#import "CasualAppDelegate.h"

#include "RK.h"
#include "RKCore.h"
#include "RKDevice.h"
#include "RKPRofileManager.h"
#include "Game.h"
#include "Platform.h"
#include "Scene.h"
#include "SWF/SWFManager.h"
#include "Platform/Hardware.h"

#include <sys/types.h>
#include <sys/sysctl.h>
#include "Platform/Hardware.h"

extern "C" int CasualMain(int argc, char** argv);

@implementation CasualViewController

@synthesize context = _context;
@synthesize displayLink = displaylink_;
@synthesize touchesdelegate = touchesdelegate_;
@synthesize orientationLocked = orientationLocked_;
@synthesize renderBufferLocked = renderBufferLocked_;

#define SWIPE_UP_THRESHOLD -200.0f
#define SWIPE_DOWN_THRESHOLD 200.0f
#define SWIPE_LEFT_THRESHOLD -200.0f
#define SWIPE_RIGHT_THRESHOLD 200.0f

static bool s_usingSwipeGesture = false;

static bool s_swipeDetected = false;

-(id) init
{
  self = [super init];
  if (self)
  {
    casualcoreinited_ = false;
    pausecounter_ = 1; // created paused, will be unpaused upon initialization
    
    preferredScreenOrientation_ = UIInterfaceOrientationLandscapeLeft;
    currentOrientation_ = UIInterfaceOrientationLandscapeLeft;
    orientationLocked_ = NO;
    renderBufferLocked_ = NO;
    
    texturesUnloaded_ = false;
    
    enablemsaa_ = false;
    runninglean_ = false;
  }
  return self;
}

- (void)dealloc
{
  [_context release];
  [displaylink_ release];
  [touchesdelegate_ release];
  
  [super dealloc];
}

-(void) loadView
{
  self.view = [[CasualView alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
  [self.view release];
  [self.view layoutSubviews];
}

-(void) enableMSAA: (bool) enabled
{
  if (enabled)
  {
    size_t size = 0;
    sysctlbyname("hw.machine", NULL, &size, 0, 0);
    char* machine = (char*) RKHeap_Alloc(sizeof(char*) * size);
    sysctlbyname("hw.machine", machine, &size, 0, 0);
    if (!strcmp(machine, "iPad3,1") || !strcmp(machine, "iPad3,2") || !strcmp(machine, "iPad3,3"))
    {
      glBindFramebuffer(GL_FRAMEBUFFER, msaaframebuffer_);
      enablemsaa_ = true;
    }
    RKHeap_Free(machine);
  }
  else if (enablemsaa_)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    enablemsaa_ = false;
  }
}

-(void) viewDidLoad
{
  [super viewDidLoad];
  [NSThread.mainThread setThreadPriority:1.0];
  [self setCurrentOrientionToLandscape];
  //disable that smarmy voiceover accessibility feature
  [self.view setAccessibilityTraits:UIAccessibilityTraitAllowsDirectInteraction];
  self.view.isAccessibilityElement = YES;
  // content scale setup
  size_t size = 0;
  sysctlbyname("hw.machine", NULL, &size, 0, 0);
  char* machine = (char*) RKHeap_Alloc(sizeof(char*) * size);
  sysctlbyname("hw.machine", machine, &size, 0, 0);
  if (!strcmp(machine, "iPad3,1") || !strcmp(machine, "iPad3,2") || !strcmp(machine, "iPad3,3"))
  {
    [self.view setContentScaleFactor: 1];
    enablemsaa_ = true;
  }
  else
  {
    [self.view setContentScaleFactor: [UIScreen mainScreen].scale];
  }
  CasualCore::Game::GetInstance()->GetPlatform()->SetViewportScale(1);

  // touches setup
  self.view.multipleTouchEnabled = YES;
  self.touchesdelegate = [[[CasualTouchesDelegate alloc] initWithScaleFactor: self.view.contentScaleFactor] autorelease];
  
  //filthy temp hack to fix some devices not profiling correctly (iPad2)
  //we won't be allowed to use the profiling anyway, since GameOptions is being rammed down our throats
  if(!strcmp(machine, "iPhone3,3") || !strcmp(machine, "iPhone3,2") || !strcmp(machine, "iPhone3,1") || !strcmp(machine, "iPod4,1"))
  {
    CasualCore::Hardware::GetInstance()->SetCpuRating(CasualCore::Hardware::CPU_LOW);
    CasualCore::Hardware::GetInstance()->SetGpuRating(CasualCore::Hardware::GPU_LOW);

  }
  RKHeap_Free(machine);
  RKTexture_SetDeviceNeedHalfSize(CasualCore::Hardware::GetInstance()->GetMemoryRating() <= CasualCore::Hardware::MEM_LOW);   //to set to true by CasualCore if PLATFORM->GetDeviceCalibre() <= CasualCore::LOW_CALIBRE
  RKTexture_SetDeviceNeedSimplifiedShaders(CasualCore::Hardware::GetInstance()->GetGpuRating() < CasualCore::Hardware::GPU_MEDIUM);
  // opengl context creation
  if(floor(NSFoundationVersionNumber) <= NSFoundationVersionNumber_iOS_6_1)
  {
    //we can't even try to make an opengles3 context in os versions below 7
    RKDevice_SetGLContextLevel(2);
    self.context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
    [self.context release];
    RKASSERT(self.context, "Failed to create GLES Context")
    
  }else //IOS 7 or higher
  {
    self.context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES3];
    [self.context release];
    if(self.context != nil)
    {
      RKDevice_SetGLContextLevel(3);
    }else
    {
      RKDevice_SetGLContextLevel(2);
      self.context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
      [self.context release];
    }
    RKASSERT(self.context, "Failed to create GLES Context")
  }

  
  [self setupGL];
}

-(void) viewDidUnload
{
  [super viewDidUnload];
  [self tearDownGL];
  
  if ([EAGLContext currentContext] == self.context)
  {
    [EAGLContext setCurrentContext: nil];
  }

  self.view = nil;
  self.context = nil;
  self.touchesdelegate = nil;
}


- (void)handlePanSwipe:(UIPanGestureRecognizer*)recognizer
{
    RKLOG("state = %d", recognizer.state);
    switch (recognizer.state)
    {
        case UIGestureRecognizerStateBegan:
        {
            s_swipeDetected = false;
        }
            // fallthrough
        case UIGestureRecognizerStateChanged://UIGestureRecognizerStateEnded)
        {
            if(s_swipeDetected)
                break;
            
            CGPoint vel = [recognizer velocityInView:recognizer.view];
            int newDirection = -1;
            
            if (fabsf(vel.x) > fabsf(vel.y))
            {
                if (vel.x < SWIPE_LEFT_THRESHOLD)
                {
                    newDirection = CasualCore::GESTURE_SWIPE_LEFT;
                }
                else if (vel.x > SWIPE_RIGHT_THRESHOLD)
                {
                    newDirection = CasualCore::GESTURE_SWIPE_RIGHT;
                }
            }
            else
            {
                if (vel.y < SWIPE_UP_THRESHOLD)
                {
                    newDirection = CasualCore::GESTURE_SWIPE_UP;
                }
                else if (vel.y > SWIPE_DOWN_THRESHOLD)
                {
                    newDirection = CasualCore::GESTURE_SWIPE_DOWN;
                }
            }
            
            //float timeNow = RKTimer_GetUpTime();
            
            if (newDirection != -1)
            {
                CasualCore::Game::GetInstance()->GetScene()->InjectGesture(newDirection, vel.x, vel.y);
                
                s_swipeDetected = true;
            }
        }
        break;
            
        case UIGestureRecognizerStateEnded:
        {
            s_swipeDetected = false;
        }
            break;
    }
}

- (void)handleSwipeFrom:(UISwipeGestureRecognizer *) recognizer
{
  CGPoint point = [recognizer locationInView: [self view]];
  point.x *= self.view.contentScaleFactor;
  point.y *= self.view.contentScaleFactor;
  switch([recognizer direction])
  {
    case UISwipeGestureRecognizerDirectionRight:
      CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_SWIPE_RIGHT, point.x, point.y);
      break;

    case UISwipeGestureRecognizerDirectionLeft:
      CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_SWIPE_LEFT, point.x, point.y);
      break;

    case UISwipeGestureRecognizerDirectionUp:
      CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_SWIPE_UP, point.x, point.y);
      break;

    case UISwipeGestureRecognizerDirectionDown:
      CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_SWIPE_DOWN, point.x, point.y);
      break;
  }
}

- (void)handleTapFrom:(UITapGestureRecognizer *) recognizer
{
  CGPoint point = [recognizer locationInView: [self view]];
  point.x *= self.view.contentScaleFactor;
  point.y *= self.view.contentScaleFactor;
  if ([recognizer numberOfTapsRequired] == 2)
  {
    CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_DOUBLE_TAP, point.x, point.y);
  }
  else if ([recognizer numberOfTouchesRequired] == 2)
  {
    CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_TWO_FINGER_TAP, point.x, point.y);
  }
  else
  {
    CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_TAP, point.x, point.y);
  }
}

- (void)handleLongPressFrom:(UILongPressGestureRecognizer *) recognizer
{
  CGPoint point = [recognizer locationInView: [self view]];
  point.x *= self.view.contentScaleFactor;
  point.y *= self.view.contentScaleFactor;
  CasualCore::Game::GetInstance()->GetScene()->InjectGesture(CasualCore::GESTURE_LONG_PRESS, point.x, point.y);
}

- (void)subscribeGesture: (int) gestureid
{
  switch (gestureid)
  {
    case CasualCore::GESTURE_SWIPE_LEFT:
    case CasualCore::GESTURE_SWIPE_RIGHT:
    case CasualCore::GESTURE_SWIPE_UP:
    case CasualCore::GESTURE_SWIPE_DOWN:
    {
        if (s_usingSwipeGesture)
	    {
            UISwipeGestureRecognizer* gesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeFrom:)];
            gesture.direction = 1 << gestureid - CasualCore::GESTURE_SWIPE_RIGHT;
            [[self view] addGestureRecognizer: gesture]; // retains
            [gesture release];
        }
        else
        {
            UIPanGestureRecognizer* panSwipeRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanSwipe:)];
            [[self view] addGestureRecognizer: panSwipeRecognizer];
            [panSwipeRecognizer release];
        }
        break;
    }
    case CasualCore::GESTURE_TAP:
    case CasualCore::GESTURE_DOUBLE_TAP:
    case CasualCore::GESTURE_TWO_FINGER_TAP:
    {
      UITapGestureRecognizer* gesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTapFrom:)];
      gesture.numberOfTapsRequired = gestureid == CasualCore::GESTURE_DOUBLE_TAP ? 2 : 1;
      gesture.numberOfTouchesRequired = gestureid == CasualCore::GESTURE_TWO_FINGER_TAP ? 2 : 1;
      [[self view] addGestureRecognizer: gesture]; // retains
      [gesture release];
      break;
    }
    case CasualCore::GESTURE_LONG_PRESS:
    {
      UILongPressGestureRecognizer* gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressFrom:)];
      [[self view] addGestureRecognizer: gesture]; // retains
      [gesture release];
      break;
    }
  }
}

- (void)unsubscribeGesture: (int) gestureid
{
  NSEnumerator* enumerator = [[[self view] gestureRecognizers] objectEnumerator];
  switch (gestureid)
  {
    case CasualCore::GESTURE_SWIPE_LEFT:
    case CasualCore::GESTURE_SWIPE_RIGHT:
    case CasualCore::GESTURE_SWIPE_UP:
    case CasualCore::GESTURE_SWIPE_DOWN:
       while (UIGestureRecognizer* gesture = [enumerator nextObject])
         {
            if ([gesture isKindOfClass: [UISwipeGestureRecognizer class]] &&
                [(UISwipeGestureRecognizer*)gesture direction] == 1 << gestureid - CasualCore::GESTURE_SWIPE_RIGHT)
            {
                [[self view] removeGestureRecognizer: gesture];
                break;
            }
            else if ([gesture isKindOfClass: [UIPanGestureRecognizer class]])
            {
                [[self view] removeGestureRecognizer: gesture];
                break;
            }
        }
        break;
      
    case CasualCore::GESTURE_TAP:
    case CasualCore::GESTURE_DOUBLE_TAP:
    case CasualCore::GESTURE_TWO_FINGER_TAP:
      while (UIGestureRecognizer* gesture = [enumerator nextObject])
      {
        if ([gesture isKindOfClass: [UITapGestureRecognizer class]])
        {
          if (([(UITapGestureRecognizer*)gesture numberOfTapsRequired] == 2 && gestureid == CasualCore::GESTURE_DOUBLE_TAP)
            || ([(UITapGestureRecognizer*)gesture numberOfTouchesRequired] == 2 && gestureid == CasualCore::GESTURE_TWO_FINGER_TAP)
            || ([(UITapGestureRecognizer*)gesture numberOfTouchesRequired] == 1 && [(UITapGestureRecognizer*)gesture numberOfTapsRequired] == 1 && gestureid == CasualCore::GESTURE_TAP))

          {
            [[self view] removeGestureRecognizer: gesture];
          }

          break;
        }
      }
      break;
      
    case CasualCore::GESTURE_LONG_PRESS:
      while (UIGestureRecognizer* gesture = [enumerator nextObject])
      {
        if ([gesture isKindOfClass: [UILongPressGestureRecognizer class]])
        {
          [[self view] removeGestureRecognizer: gesture];
          break;
        }
      }
      break;
  }
}

- (void)unsubscribeAllGestures
{
  while ([[[self view] gestureRecognizers] count])
    [[self view] removeGestureRecognizer: [[[self view] gestureRecognizers] objectAtIndex:0]];
}

// autorotation queries
- (BOOL)shouldAutorotate
{
  return !orientationLocked_;
}

// ios6 support
- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
  // prevents incorrect orientation when returning from certain web views (GameCenter, youtube...)
  return currentOrientation_;//preferredScreenOrientation_;
}
- (NSUInteger)supportedInterfaceOrientations
{
  if (orientationLocked_)
  {
    return (1 << currentOrientation_); // return UIInterfaceOrientationMask, not UIInterfaceOrientation
  }
  if (UIInterfaceOrientationIsPortrait(preferredScreenOrientation_))
  {
    return UIInterfaceOrientationMaskPortrait;
  }
  else
  {
    return UIInterfaceOrientationMaskLandscape;
  }
}

// ios5 support
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
  if (orientationLocked_)
  {
    return interfaceOrientation == currentOrientation_;
  }
  if (UIInterfaceOrientationIsPortrait(preferredScreenOrientation_))
  {
    return UIInterfaceOrientationIsPortrait(interfaceOrientation);
  }
  else
  {
    return UIInterfaceOrientationIsLandscape(interfaceOrientation);
  }
}

-(void) updateBuffersSize
{
  CGSize size = self.view.bounds.size;
  size.width *= self.view.contentScaleFactor;
  size.height *= self.view.contentScaleFactor;
  
  RKDevice_SetWidth(size.width);
  RKDevice_SetHeight(size.height);
  
  [EAGLContext setCurrentContext: self.context];
  
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
  [[self context] renderbufferStorage: GL_RENDERBUFFER fromDrawable: (CAEAGLLayer*)[[self view] layer]];
  
  //glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer_);
  //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, size.width, size.height);
  RKASSERT(glGetError() == 0, "Unable to create texture");
  glBindTexture(GL_TEXTURE_2D, depthbuffer_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if(RKDevice_GetGLContextLevel() >= 3)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size.width, size.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
  }else
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.width, size.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
  }
  RKASSERT(glGetError() == 0, "Unable to create texture");
  CasualCore::Game::GetInstance()->GetPlatform()->SetDepthTexture(depthbuffer_);
  if (enablemsaa_)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, msaaframebuffer_);
    
    glBindRenderbuffer(GL_RENDERBUFFER, msaarenderbuffer_);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_RGBA8_OES, size.width, size.height);
    
    glBindRenderbuffer(GL_RENDERBUFFER, msaadepthbuffer_);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT16, size.width, size.height);
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    [self performSelector:@selector(updateBuffersSize) withObject:nil afterDelay:0.1f];
  }
}

// autorotation processing
-(void) customOnOrientation
{
  [self setCurrentOrientionToLandscape];
  CasualAppDelegate* appdel = (CasualAppDelegate*) [[UIApplication sharedApplication] delegate];
  if (![appdel isGamePaused])
  {
    [self updateBuffersSize];
    [self renderView]; // instantly update backbuffer content
  }
}


-(void) viewWillLayoutSubviews
{
  [self customOnOrientation];
}

-(void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation) fromInterfaceOrientation
{
  [self customOnOrientation];
}

-(void) tryCorrectOrientation
{
  if ([UIViewController respondsToSelector:@selector(attemptRotationToDeviceOrientation)])
  {
    [UIViewController attemptRotationToDeviceOrientation];
  }
}

-(void) setupGL
{
  [EAGLContext setCurrentContext: self.context];
  
  CAEAGLLayer* layer = (CAEAGLLayer*)[[self view] layer];
  layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool: false], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
  layer.opaque = YES;
  
  [EAGLContext setCurrentContext: self.context];
  
  float version = [[[UIDevice currentDevice] systemVersion] floatValue];
  //enablemsaa_ = false;//(version >= 4.2f && devicetype == CasualCore::DEVICE_IPAD2);
  
  // generating buffers
  glGenFramebuffers(1, &framebuffer_);
  glGenRenderbuffers(1, &renderbuffer_);
  //glGenRenderbuffers(1, &depthbuffer_);
  glGenTextures(1, &depthbuffer_);
  if (enablemsaa_)
  {
    glGenRenderbuffers(1, &msaarenderbuffer_);
    glGenRenderbuffers(1, &msaadepthbuffer_);
    glGenFramebuffers(1, &msaaframebuffer_);
  }
  
  // this function creates/recreates buffers content
  [self updateBuffersSize];
  
  // attaching buffers
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer_);
  //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbuffer_, 0);
  
  if (enablemsaa_)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, msaaframebuffer_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaarenderbuffer_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, msaadepthbuffer_);
  }
}

-(void) tearDownGL
{
  [EAGLContext setCurrentContext: self.context];
  
  glDeleteRenderbuffers(1, &renderbuffer_);
  //glDeleteRenderbuffers(1, &depthbuffer_);
  glDeleteTextures(1, &depthbuffer_);
  glDeleteFramebuffers(1, &framebuffer_);
  renderbuffer_ = 0;
  depthbuffer_ = 0;
  framebuffer_ = 0;
  
  if (enablemsaa_)
  {
    glDeleteRenderbuffers(1, &msaarenderbuffer_);
    glDeleteRenderbuffers(1, &msaadepthbuffer_);
    glDeleteFramebuffers(1, &msaaframebuffer_);
    msaarenderbuffer_ = 0;
    msaadepthbuffer_ = 0;
    msaaframebuffer_ = 0;
  }
}

-(void) renderView
{
  //
  // CasualCore lazily initialized for a reason
  //
  // iOS creates all views in portrait mode and
  // then if necessary rotates them to landscape mode
  // if the CasualCore was initialized on creation of
  // the view, it would have received the wrong
  // screen dimensions
  //
  // now first renderView called from customOnOrientation fn
  // which is called when iOS rotates the view to landscape
  // after creation
  //
  if (!casualcoreinited_)
  {
    CasualMain(0, 0);
    CasualCore::Game::GetInstance()->GetPlatform()->Initialize();
    [self pauseDisplay:NO];
    casualcoreinited_ = true;
  }
  
  if (!RKDevice_HasEnoughDiscSpace()) // because gldrawarrays seems to write to disc if it's compiling shaders
  {
  	return;
  }
  
  [EAGLContext setCurrentContext: self.context];
  
  if (enablemsaa_)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, msaaframebuffer_);
  }
  else
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  }
  
  if (texturesUnloaded_)
  {
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      return; // not ready to reload textures
    }
    
    if (RKTexture_ReloadUnloadedTextures())
    {
      texturesUnloaded_ = false;
      if (renderBufferLocked_ == NO)
      {
        CasualAppDelegate* appdel = (CasualAppDelegate*) [[UIApplication sharedApplication] delegate];
        [appdel hideActivityIndicator: NO];
      }
    }
    else
    {
      // we are not ready yet
      return;
    }
  }

  { // main game cycle, update call populates renderlists used by RKEngine in RKCore_OnDraw()
    CasualCore::Game::GetInstance()->Update();
    CasualCore::SWFManager* pSWFManager = CasualCore::Game::GetInstance()->GetFlashManager();
    if (pSWFManager)
      pSWFManager->Render();
    RKCore_OnDraw();
  }

  if (renderBufferLocked_ == NO)
  {
    if (enablemsaa_) // process msaa, draw into "framebuffer_"
    {
      GLenum attachments[] = {GL_DEPTH_ATTACHMENT};
      glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, attachments);
      glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, msaaframebuffer_);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, framebuffer_);

      glResolveMultisampleFramebufferAPPLE();
    }

    // present the result
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
    [[self context] presentRenderbuffer: GL_RENDERBUFFER];
  }
  // When iOS detects change of orientation, it asks current
  // _topmost_ view if it should rotate device into that orientation
  // using 'shouldAutorotateToInterfaceOrientation' call and rotates
  // the device if that call returns 'YES'
  //
  // If we have third-party view (on top of our CasualView) that forbids
  // rotation to some orientation, it might cause some problms:
  // 1. Third-party view created on top of CasualView
  // 2. User reorients the device, third-party view being asked whether
  //    the interface should be rotated but rejects rotaion, device's
  //    interface stays in original orientation
  // 3. Third-party view deleted
  // 4. CasualView ends up being in wrong orientation, because
  //    iOS tries to rotate interface only if it detects change
  //    in device orientation
  //
  // This code detects wrong orientation of CasualView and ask iOS
  // to try to reorient interface
  // When CasualView become a topmost view, it's orientation will
  // be automatically fixed
  if (currentOrientation_ != [[UIApplication sharedApplication] statusBarOrientation])
  {
    if ([UIViewController respondsToSelector:@selector(attemptRotationToDeviceOrientation)])
    {
      [UIViewController attemptRotationToDeviceOrientation];
    }
  }
}

-(void) pauseDisplay:(BOOL)pause
{
  pausecounter_ += (pause == YES) ? 1 : -1;
  RKASSERT(pausecounter_ >= 0, "CasualViewController unpaused while not paused");
  
  //we have some craziness in iOS 8 where in certain conditions, where applicationDidEnterBackground is called twice in a row by iOS.
  //these changes try to maintain integrity while allowing for iOS 8 to suck.
  if ((pausecounter_ > 0) == (displaylink_ == 0)) // already in correct state
  {
    pausecounter_ = (pause == YES) ? 1 : 0;
    return;
  }
  
  if ( pause )
  {
    pausecounter_ = 1;
    RKASSERT(displaylink_, "Internal inconsistency");
    [displaylink_ removeFromRunLoop:[NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
    displaylink_ = NULL;
  }
  else
  {
    pausecounter_ = 0;
    RKASSERT(!displaylink_, "Internal inconsistency");
    displaylink_ = [CADisplayLink displayLinkWithTarget: self selector: @selector(renderView)];
    [displaylink_ setFrameInterval: 1];
    [displaylink_ addToRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
  }
}

-(void) runLean: (BOOL) lean
{
  runninglean_ = lean;
}

-(void) suspendGL
{
  if (!texturesUnloaded_)
  {
    // Uncomment the next line to only conditionally remove textures when going to background. You might do this if there are complaints about the resume time. However, it may cause clean restarts when multitasking between apps, which is nono for the checklist
    
    //if (runninglean_ || CasualCore::Hardware::GetInstance()->GetMemoryRating() == CasualCore::Hardware::MEM_LOW)
    {
      CasualAppDelegate* appdel = (CasualAppDelegate*) [[UIApplication sharedApplication] delegate];
      [appdel showActivityIndicator: NO];
      RKTexture_UnloadAllTextures();
      texturesUnloaded_ = true;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// All touch events delegated to CasualTouchesDelegate
//----------------------------------------------------------------------------------------------------------------------
-(void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
  [self.touchesdelegate touchesBegan:touches withEvent:event];
}
-(void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
  [self.touchesdelegate touchesMoved:touches withEvent:event];
}
-(void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
  [self.touchesdelegate touchesEnded:touches withEvent:event];
}
-(void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
{
  [self.touchesdelegate touchesEnded:touches withEvent:event];
}
- (void)resetAllTouches
{
  [self.touchesdelegate resetAllTouches:@"Manually called"];
}

-(BOOL)prefersStatusBarHidden
{
  return YES;
}

-(void)setCurrentOrientionToLandscape
{
  UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
  if(UIInterfaceOrientationIsLandscape(orientation))
  {
    currentOrientation_ = orientation;
  }
}
@end


