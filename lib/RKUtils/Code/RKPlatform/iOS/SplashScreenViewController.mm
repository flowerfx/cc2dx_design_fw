#import "SplashScreenViewController.h"
#include "Game.h" // Gameloft Auckland specific 

#include <sys/types.h>
#include <sys/sysctl.h>

//-----------------------------------------------------------------------------------------------------------------------------
//SPLASH SCREEN SPECIFIC code
#include <sys/socket.h> // Per msqr
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>

#define USE_AUTOROTATE
#define K_SPLASH_SCREEN_LINK_TOPDEALS @"http://ingameads.gameloft.com/redir/ads/splashscreen_view.php?from=FROM&country=COUNTRY&lg=LANG&udid=UDIDPHONE&d=DEVICE&f=FIRMWARE&game_ver=VERSION&ver=1.0.0"

//#define SPLASH_SCREEN_YOUR_IGP_GAMECODE @"TEST"
//#define SPLASH_SCREEN_YOUR_IGP_GAMECODE_IPAD @"FAMI"
//#define SPLASH_SCREEN_YOUR_GAMEVERSION @"1.0.0"

NSString *SPLASH_SCREEN_TXT_LANGUAGE_LIST[] = {
	@"EN",
	@"FR",
	@"DE",
	@"IT",
	@"SP",
	@"JP",
	@"KR",
	@"CN",
	@"BR",
	@"RU"
};

int _splashScreenGameLanguage = 0;
NSString*	urlAddressSplashScreen;

@implementation SplashScreenViewController

@synthesize splash_webview;//, addressBar, activityIndicator;

//@synthesize m_backButton;
//@synthesize igptimer;
//END SPLASH SCREEN SPECIFIC code =====================================
//-----------------------------------------------------------------------------------------------------------------------------


// Gameloft Auckland specific
- (void)setLanguage:(NSString*)language {
  // Done like this because SPLASH_SCREEN_TXT_LANGUAGE_LIST is needed by other functions.
  NSString* uppercaseLanguage = [language uppercaseString];
  for (int i = 0; i < sizeof(SPLASH_SCREEN_TXT_LANGUAGE_LIST) / sizeof(SPLASH_SCREEN_TXT_LANGUAGE_LIST[0]); ++i) {
    
    if ([uppercaseLanguage isEqualToString:SPLASH_SCREEN_TXT_LANGUAGE_LIST[i]]) {
      _splashScreenGameLanguage = i;
      break;
    }
  }
}


// Implement loadView to create a view hierarchy programmatically,
// without using a nib.
- (void)loadView 
{
	//-----------------------------------------------------------------------------------------------------------------------------
	//SPLASH SCREEN SPECIFIC code
	splash_screen_display_mode = SPLASHSCREEN_SET_IN_LANDSCAPE_RIGHT_MODE; // Gameloft Auckland specific
	
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_RIGHT_MODE)
	{
		splash_screen_SHADOW_X = 0;
		splash_screen_SHADOW_Y = 160;
	}
	
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_LEFT_MODE)
	{
		splash_screen_SHADOW_X = 0;
		splash_screen_SHADOW_Y = 0;
	}
	
	NSString *firmware = [[UIDevice currentDevice] systemVersion];

  //	b_is_in_mm = true;
	splash_screen_screenOrientation = UIInterfaceOrientationPortrait;
  
	
	//END SPLASH SCREEN SPECIFIC code
	//-----------------------------------------------------------------------------------------------------------------------------
	
	//allocate the view
	//self.view = [[UIView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
	free(machine);
	if([platform hasPrefix:@"iPad"])
	{
		self.view = [[UIView alloc] initWithFrame:CGRectMake(0.0, 0.0, 768, 1024.0)];
	}
	else
	{
		self.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 320, 480)];
	}
  
  

//  Gameloft Auckland specific

	//set the view's background color
	//self.view.backgroundColor = [UIColor orangeColor];
  
//	for(int i = 0; i < NO_LANGS; i++)
//	{
//		lang_buttons[i] = [UIButton buttonWithType:UIButtonTypeRoundedRect];
//		lang_buttons[i].frame = CGRectMake(10 + 30*i, 0, 30, 30);
//		[lang_buttons[i] setTitle:SPLASH_SCREEN_TXT_LANGUAGE_LIST[i] forState:UIControlStateNormal];
//		[lang_buttons[i] addTarget:self action:@selector(buttonPressedSplash:) forControlEvents:UIControlEventTouchUpInside];
//		[self.view addSubview:lang_buttons[i]];
//	}

	//-----------------------------------------------------------------------------------------------------------------------------
	//SPLASH SCREEN SPECIFIC code =====================================
  //	b_drawing_done = false;
	keyboard_is_shown = false;
	
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_RIGHT_MODE)
	{
		splash_screen_screenOrientation = UIInterfaceOrientationLandscapeRight;
	}
	
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_LEFT_MODE)
	{
		splash_screen_screenOrientation = UIInterfaceOrientationLandscapeLeft;
	}
	
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_PORTRAIT_MODE)
	{
		splash_screen_screenOrientation = UIInterfaceOrientationPortrait;
	}
	
  // Gameloft Auckland specific
  {
    getSplashScreenLink();
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@&check=1", urlAddressSplashScreen]];
    
    // perform request
    NSURLRequest* request = [NSURLRequest 
                             requestWithURL:url//[NSURL URLWithString:url] 
                             cachePolicy:NSURLRequestReloadIgnoringCacheData 
                             timeoutInterval:13.0];
    myConnection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
    if (myConnection) {
      receivedData = [[NSMutableData data] retain];
    } 
  }
	//END SPLASH SCREEN SPECIFIC code =====================================
	//-----------------------------------------------------------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------------------------------------------------------
//SPLASH SCREEN SPECIFIC code =====================================
-(void) drawSPLASHSCREENBrowserWindow:(int)orientation: (int) yoffset
{
	
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
	free(machine);
	if([platform hasPrefix:@"iPad"])
	{
		
		if(orientation == UIInterfaceOrientationLandscapeRight)
		{
			int x = 122;//556;
			int y = LANDSCAPE_Y - yoffset + 82 - 160; //+ 100;
			
			
			//[m_headerView setFrame:CGRectMake(x, y, 456, 430)];
			//[m_backButton setFrame:CGRectMake(x+406, y+5, 45, 34)];		
			[splash_webview setFrame:CGRectMake(x+10, /*y+48*/132, /*437*/760, /*242 + 130*/504)];
      //			[activityIndicator setFrame:CGRectMake(x+208, y+130 + 65, 32, 32)];
			[dimming_image2 setFrame:CGRectMake(splash_screen_SHADOW_X, splash_screen_SHADOW_Y - 160, 1024, 768)];
			
		}
		
		if(orientation == UIInterfaceOrientationLandscapeLeft)
		{
			int x = 122;//556;
			int y = LANDSCAPE_Y - yoffset - 78;
			
			//[m_headerView setFrame:CGRectMake(x, y, 456, 430)];
			//[m_backButton setFrame:CGRectMake(x+406, y+5, 45, 34)];
			[splash_webview setFrame:CGRectMake(x+10, /*y+48*/132, /*437*/760, /*242 + 130*/504)];
			//[activityIndicator setFrame:CGRectMake(x+208, y+130 + 65, 32, 32)];
			
			[dimming_image2 setFrame:CGRectMake(splash_screen_SHADOW_X, splash_screen_SHADOW_Y, 1024, 768)];
		}
		
		if(orientation == UIInterfaceOrientationPortrait)
		{
			//[m_headerView setFrame:CGRectMake(10, 65, 300, 245.0)];
			//[m_backButton setFrame:CGRectMake(255, 73, 45, 35)];
			[splash_webview setFrame:CGRectMake(13.0, 109, 294, 195)];
			//[activityIndicator setFrame:CGRectMake(145, 160, 32, 32)];
			
			[dimming_image2 setFrame:CGRectMake(0, -20, 320, 480)];
		}
	}
	else 
	{
		if(orientation == UIInterfaceOrientationLandscapeRight)
		{
			int x = 12;
			int y = LANDSCAPE_Y - yoffset + 162 - 240;
			
			
			//[m_headerView setFrame:CGRectMake(x, y, 456, 300)];
			//[m_backButton setFrame:CGRectMake(x+406, y+5, 45, 34)];		
			[splash_webview setFrame:CGRectMake(x+10, y/*+48*/ + 12, 437, 242 + 36)];
			//[activityIndicator setFrame:CGRectMake(x+208, y+130, 32, 32)];
			[dimming_image2 setFrame:CGRectMake(splash_screen_SHADOW_X, splash_screen_SHADOW_Y + 80 -240, 480, 320)];
			
		}
		
		if(orientation == UIInterfaceOrientationLandscapeLeft)
		{
			int x = 12 ;//- 80;
			int y = LANDSCAPE_Y - yoffset + 162 - 240;//- 78 + 80;
			
			//[m_headerView setFrame:CGRectMake(x, y, 456, 300)];
			//[m_backButton setFrame:CGRectMake(x+406, y+5, 45, 34)];
			[splash_webview setFrame:CGRectMake(x+10, y/*+48*/ + 12, 437, 242 + 36)];
			//[activityIndicator setFrame:CGRectMake(x+208, y+130, 32, 32)];
			
			[dimming_image2 setFrame:CGRectMake(splash_screen_SHADOW_X , splash_screen_SHADOW_Y , 480, 320)];
		}
		
		if(orientation == UIInterfaceOrientationPortrait)
		{
			//[m_headerView setFrame:CGRectMake(10, 65, 300, 245.0)];
			//[m_backButton setFrame:CGRectMake(255, 73, 45, 35)];
			[splash_webview setFrame:CGRectMake(13.0, 109, 294, 195)];
			//[activityIndicator setFrame:CGRectMake(145, 160, 32, 32)];
			
			[dimming_image2 setFrame:CGRectMake(0, -20, 320, 480)];
		}
	}
}

double _degreesToRadianSS(int degrees)
{
	double pi = 3.14159265;
	return degrees*pi/180;
}

-(IBAction)buttonPressedSplash:(id)sender
{
  // Gameloft Auckland specific
//	for(int i = 0; i < NO_LANGS; i++)
//	{
//		if (sender == lang_buttons[i])
//		{
//			_splashScreenGameLanguage = i;
//		}
//	}
	
	getSplashScreenLink();
	NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@&check=1", urlAddressSplashScreen]];
	
	// perform request
	NSURLRequest* request = [NSURLRequest 
                           requestWithURL:url//[NSURL URLWithString:url] 
                           cachePolicy:NSURLRequestReloadIgnoringCacheData 
                           timeoutInterval:13.0];
	myConnection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
	if (myConnection) {
		receivedData = [[NSMutableData data] retain];
	} 
	
  //	[self startSplash];
}

- (void)startSplash {
	{
		//	connection_was_ok = false;
		
		//	if(!b_is_in_mm)
		//	{
		//		return;
		//	}
		
		//timeout after 13 seconds
		//	igptimer = [NSTimer scheduledTimerWithTimeInterval: 13 target:self 
		//											  selector:@selector(targetMethod:) userInfo:nil repeats:YES];
		if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_RIGHT_MODE)
		{
			splash_screen_SHADOW_X = 0;
			splash_screen_SHADOW_Y = 160;
		}
		
		if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_LEFT_MODE)
		{
			splash_screen_SHADOW_X = 0;
			splash_screen_SHADOW_Y = 0;
		}	
		if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_RIGHT_MODE)
		{
			//[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
		}
		
		if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_LEFT_MODE)
		{
			//[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
		}
		
		if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_PORTRAIT_MODE)
		{
			//[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortrait;
		}
		
		
		splash_screen_subVC = [[UIViewController alloc] init];
		
		
		
		//shadow rect
		size_t size;
		sysctlbyname("hw.machine", NULL, &size, NULL, 0);
		char *machine = (char*)malloc(size);
		sysctlbyname("hw.machine", machine, &size, NULL, 0);
		NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
		free(machine);
		CGRect rect;
		if([platform hasPrefix:@"iPad"])
		{
			rect = CGRectMake(0, 0, 1024, 1024);
		}
		else
		{
			rect = CGRectMake(0, 0, 480, 480);
		}
		
		
		UIGraphicsBeginImageContext(rect.size);
		CGContextRef ctx = UIGraphicsGetCurrentContext();
		CGContextSetRGBFillColor(ctx, 0.0f, 0.0f, 0.0f, 0.5f);
		CGContextFillRect(ctx, rect);
		
		dimming_image = UIGraphicsGetImageFromCurrentImageContext();
		UIGraphicsEndImageContext();
		
		dimming_image2 = [[UIImageView alloc] initWithImage:dimming_image];
		if([platform hasPrefix:@"iPad"])
		{
			splash_screen_subVC.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 768, 1024)];
		}
		else
		{
//			splash_screen_subVC.view = [[UIView alloc] init];
//			[splash_screen_subVC.view setCenter:CGPointMake(0, 0)];
//			[splash_screen_subVC.view setBounds:CGRectMake(0, 0, 320, 480)];
      splash_screen_subVC.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 480, 320)];
		}
		
		
		[splash_screen_subVC.view addSubview:dimming_image2];
		
    /*		
     //window background
     char * buff;
     int len;
     
     FILE* ptr;
     if([platform hasPrefix:@"iPad"])
     {
     ptr= openFileSS((char*)[TXT_RES_IPAD_LIST[_gamelanguage] UTF8String]);
     }
     else
     {
     ptr= openFileSS((char*)[TXT_RES_LIST[_gamelanguage] UTF8String]);
     }
     
     
     len = ftell(ptr);
     fseek( ptr, 0L, SEEK_END);
     len = ftell(ptr) - len + 1;
     fseek( ptr, 0L, SEEK_SET);
     
     buff = (char *)malloc(len * sizeof(char));
     fread(buff, sizeof(char), len, ptr);
     
     NSData* data = [[NSData alloc]initWithBytes:buff length:len];
     UIImage* img = [UIImage imageWithData:data];
     
     free(buff);
     fclose(ptr);
     */		
		//m_headerView = [[UIImageView alloc] initWithImage:img];
		//[m_headerView setFrame:CGRectMake(20.0, 130.0-25.0, 280.0, 220.0)];
		//[splash_screen_subVC.view addSubview:m_headerView];
		
		
    /*		
     //close button image
     NSLog(@"fwr:%@",[[UIDevice currentDevice] systemVersion]);
     if([[[UIDevice currentDevice] systemVersion] hasPrefix:@"3."])
     {
     ptr = openFileSS("close_but_low.png");
     }
     else
     {
     ptr = openFileSS("close_but.png");
     }
     
     len = ftell(ptr);
     fseek( ptr, 0L, SEEK_END);
     len = ftell(ptr) - len + 1;
     fseek( ptr, 0L, SEEK_SET);
     
     buff = (char *)malloc(len * sizeof(char));
     fread(buff, sizeof(char), len, ptr);
     
     NSData* data_close_button = [[NSData alloc]initWithBytes:buff length:len];
     UIImage* img_close_button = [UIImage imageWithData:data_close_button];
     
     free(buff);
     fclose(ptr);
     */		
		//close button object
		//	m_backButton = [[UIButton alloc] initWithFrame:CGRectMake(250.0, 152.0-25.0, 35.0, 39.0)];
		//[m_backButton setImage: img_close_button forState:UIControlStateNormal];
		//[m_backButton addTarget:self action:@selector(quitIGPTD:) forControlEvents:UIControlEventTouchUpInside];
		//[splash_screen_subVC.view addSubview:m_backButton];
		
		//web browser
		CGRect webFrame;
		
		webFrame = CGRectMake(23.0, 130.0 + 59.0-25.0, 274.0, 155.0);
		
		splash_webview = [[UIWebView alloc] initWithFrame:webFrame];
		[splash_screen_subVC.view addSubview:splash_webview];
		
		[splash_webview setBackgroundColor:[UIColor clearColor]];
		getSplashScreenLink();
		NSURL *url = [NSURL URLWithString:urlAddressSplashScreen];
		NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
		[splash_webview loadRequest:requestObj];
		[splash_webview setDelegate:self];
    splash_webview.opaque = NO;
		
		//activity indicator - loading animation
		//activityIndicator = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
		//[splash_screen_subVC.view addSubview:activityIndicator];
		//[activityIndicator setFrame:CGRectMake(145.0, 240.0-25.0, 32, 32)];
		//[activityIndicator setHidden:NO];
		
		//[self.view addSubview:splash_screen_subVC.view];
		
		
		//b_is_in_mm = false;	
		
		
		if(splash_screen_screenOrientation == UIInterfaceOrientationLandscapeRight)
		{
			[self drawSPLASHSCREENBrowserWindow:UIInterfaceOrientationLandscapeRight:0];
		}
		
		if(splash_screen_screenOrientation == UIInterfaceOrientationLandscapeLeft)
		{
			[self drawSPLASHSCREENBrowserWindow:UIInterfaceOrientationLandscapeLeft:0];
		}
		
		if(splash_screen_screenOrientation == UIInterfaceOrientationPortrait)
		{
			[self drawSPLASHSCREENBrowserWindow:UIInterfaceOrientationPortrait:0];
		}
		
#ifndef USE_AUTOROTATE
		if (splash_screen_screenOrientation == UIInterfaceOrientationLandscapeRight)
		{
			NSLog(@"\nROTATE LANDSCAPE RIGHT\n");
			splash_screen_subVC.view.transform = CGAffineTransformMakeRotation(M_PI/2);
			if([platform hasPrefix:@"iPad"])
			{
				[splash_screen_subVC.view setCenter:CGPointMake(384, 512)];
				[splash_screen_subVC.view setBounds:CGRectMake(0, 0, 1024, 768)];
			}
			else
			{
				[splash_screen_subVC.view setCenter:CGPointMake(160, 240 )];
				[splash_screen_subVC.view setBounds:CGRectMake(0, 0, 480, 320)];
			}
			return;
		}
		
		if (splash_screen_screenOrientation == UIInterfaceOrientationLandscapeLeft)
		{
			NSLog(@"\nROTATE LANDSCAPE LEFT\n");
			splash_screen_subVC.view.transform = CGAffineTransformMakeRotation(-M_PI/2);
			
			if([platform hasPrefix:@"iPad"])
			{
				[splash_screen_subVC.view setBounds:CGRectMake(0, 0, 1024, 768)];
				
			}
			else
			{
				[splash_screen_subVC.view setCenter:CGPointMake(160, 240)];
				[splash_screen_subVC.view setBounds:CGRectMake(0, 0, 480, 320)];
			}
			return;
		}
		
		if (splash_screen_screenOrientation == UIInterfaceOrientationPortrait)
		{
			NSLog(@"\nPORTRAIT\n");
			splash_screen_subVC.view.transform = CGAffineTransformMakeRotation(_degreesToRadianSS(0));
			return;
		}
		
		if (splash_screen_screenOrientation == UIInterfaceOrientationPortraitUpsideDown)
		{
			NSLog(@"\nUPSIDE DOWN\n");
			splash_screen_subVC.view.transform = CGAffineTransformMakeRotation(_degreesToRadianSS(180));
			return;
		}
#endif
		
		/*if(splash_screen_screenOrientation == UIInterfaceOrientationLandscapeRight)
		 {
		 [self drawSPLASHSCREENBrowserWindow:UIInterfaceOrientationLandscapeRight:0];
		 }
		 
		 if(splash_screen_screenOrientation == UIInterfaceOrientationLandscapeLeft)
		 {
		 [self drawSPLASHSCREENBrowserWindow:UIInterfaceOrientationLandscapeLeft:0];
		 }
		 
		 if(splash_screen_screenOrientation == UIInterfaceOrientationPortrait)
		 {
		 [self drawSPLASHSCREENBrowserWindow:UIInterfaceOrientationPortrait:0];
		 }*/
	}
  
}


- (void)viewDidAppear {
  [super viewDidLoad];
	
	NSLog(@"%@\n", urlAddressSplashScreen);
	
	NSURL *url = [NSURL URLWithString:urlAddressSplashScreen];
	NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
	
	[splash_webview loadRequest:requestObj];
  //	[addressBar setText:urlAddressSplashScreen];
}


- (BOOL)webView:(UIWebView*)webView shouldStartLoadWithRequest:(NSURLRequest*)request navigationType:(UIWebViewNavigationType)navigationType 
{
  
	NSURL *url = request.URL;
	NSString *urlString = url.absoluteString;
	NSLog(@"%@", urlString);
	
	if(navigationType == UIWebViewNavigationTypeLinkClicked) {
		
		NSURL *url = [NSURL URLWithString:urlString];
		NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
		[webView loadRequest:requestObj];
    //		[addressBar setText:urlAddressSplashScreen];
		return NO;
	}
	
	if([urlString hasPrefix:@"http://phobos.apple.com"]||[urlString hasPrefix:@"http://itunes.apple.com"])
	{
		//[activityIndicator stopAnimating];
		//connection_was_ok = true;
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
		[self quitSplash:nil];
	}
	if([urlString hasPrefix:@"link:"])
	{
		//[activityIndicator stopAnimating];
		//connection_was_ok = true;
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:[urlString substringFromIndex:5]]];
		[self quitSplash:nil];
	}
	if([urlString hasPrefix:@"play:"])
	{
		//[activityIndicator stopAnimating];
		//connection_was_ok = true;
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:[urlString substringFromIndex:5]]];
	}
	if([urlString hasPrefix:@"exit:"])
	{
		[self quitSplash:nil];
	}
	
	if([urlString hasPrefix:@"goto:"])
	{
		[self quitSplash:nil];
		const char *name = [[urlString substringFromIndex:5] UTF8String];

    // Gameloft Auckland specific
		//extern void splashScreenFunc(const char* name = "");
		//splashScreenFunc(name);
    
    CasualCore::State* currentState = CasualCore::Game::GetInstance()->GetCurrentState();
    assert(currentState);
    currentState->OnSplashScreen(name);
	}
  
	return YES;
}

- (void)webViewDidStartLoad:(UIWebView *)webView {
	//[activityIndicator startAnimating];
}

- (void)webViewDidFinishLoad:(UIWebView *)webView {
	[self.view addSubview:splash_screen_subVC.view];
	//[activityIndicator stopAnimating];
	//connection_was_ok = true;
}


- (void)webView:(UIWebView *)webView didFailWithError:(NSError *)error
{
	NSLog(@"Error:%@", error);
	[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible: NO];
}


/* Get path to a file in resources or documents folder*/
void GetFileNameSplashScreen (const char* filename, bool bWrite, char*p)
{
  
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	sprintf(p, "%s", filename);
	NSString *path = [NSString stringWithUTF8String:p];
	
	if(bWrite)
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex:0];
		path = [documentsDirectory stringByAppendingPathComponent:path];
	}
	else
	{
		if(![path isAbsolutePath])
		{
			//	path = [[NSBundle mainBundle] pathForResource:path ofType:nil];
			
			path = [NSString stringWithFormat:@"%@%@%@",[[NSBundle mainBundle] resourcePath],@"/SPLASHSCREEN_RES/",path];
      
			
      
			
		}
	}
	if (path != nil) sprintf(p, "%s", [path UTF8String]);
	
	[pool release];
}


FILE* openFileSS(char* name)
{
	FILE* f;
	char path[256];
	GetFileNameSplashScreen(name, false, path);
	f = fopen(path, "rb");
	if (f) return f;
	GetFileNameSplashScreen(name, true, path);
	f = fopen(path, "rb");
	return f;	
}

- (void) quitSplash:(id) sender
{	
	if(keyboard_is_shown)
	{
		return;
	}
	
	NSLog(@"Quitting IGP...n");
	
	
	//[m_headerView removeFromSuperview];
	//[m_headerView release];
	
	//[m_backButton removeFromSuperview];
	//[m_backButton release];
	
	[dimming_image2 removeFromSuperview];
	[dimming_image2 release];
	
  //	free(dimming_image);
	
	[splash_webview removeFromSuperview];
	[splash_webview release];
	
	//[activityIndicator removeFromSuperview];
	//[activityIndicator release];
	
#ifdef USE_AUTOROTATE
  splash_screen_screenOrientation = UIInterfaceOrientationPortrait;
#endif
	
	//b_is_in_mm = true;
	
  //	[igptimer invalidate];
  
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_RIGHT_MODE)
	{
		//[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
	}
  
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_LANDSCAPE_LEFT_MODE)
	{
		//[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
	}
  
	if (splash_screen_display_mode == SPLASHSCREEN_SET_IN_PORTRAIT_MODE)
	{
		//[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortrait;
	}
	
	if (splash_screen_subVC)
	{
    [splash_screen_subVC.view removeFromSuperview];
    [splash_screen_subVC release];
    splash_screen_subVC = nil;
	}
  
  // Credit to Rong who discovered this
  if(self.view)    
  {
    [self.view removeFromSuperview];
    [self.view release];
    self.view = nil;
  }
}

void getSplashScreenLink()
{
	urlAddressSplashScreen = K_SPLASH_SCREEN_LINK_TOPDEALS;
  
  // Gameloft Auckland specific
	urlAddressSplashScreen = [urlAddressSplashScreen
                            stringByReplacingOccurrencesOfString:@"VERSION"
                            withString:[NSString stringWithUTF8String:CasualCore::Game::GetInstance()->GetGameVersion()]];
	urlAddressSplashScreen = [urlAddressSplashScreen stringByReplacingOccurrencesOfString:@"LANG" withString:SPLASH_SCREEN_TXT_LANGUAGE_LIST[_splashScreenGameLanguage]];
	urlAddressSplashScreen = [urlAddressSplashScreen stringByReplacingOccurrencesOfString:@"COUNTRY" withString:[[NSLocale currentLocale] objectForKey:NSLocaleCountryCode]];
	
	size_t size;
  sysctlbyname("hw.machine", NULL, &size, NULL, 0);
  char *machine = (char*)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
	free(machine);
	
  // Gameloft Auckland specific
//	if([platform hasPrefix:@"iPad"])
//	{
//		urlAddressSplashScreen = [urlAddressSplashScreen stringByReplacingOccurrencesOfString:@"FROM"  withString:SPLASH_SCREEN_YOUR_IGP_GAMECODE_IPAD];
//	}
//	else {
//		urlAddressSplashScreen = [urlAddressSplashScreen stringByReplacingOccurrencesOfString:@"FROM"  withString:SPLASH_SCREEN_YOUR_IGP_GAMECODE];
//	}
  
  urlAddressSplashScreen = [urlAddressSplashScreen
                            stringByReplacingOccurrencesOfString:@"FROM"
                            withString:[NSString stringWithUTF8String:CasualCore::Game::GetInstance()->GetGameCode()]];
	
	urlAddressSplashScreen = [urlAddressSplashScreen stringByReplacingOccurrencesOfString:@"DEVICE" withString: platform];
	urlAddressSplashScreen = [urlAddressSplashScreen stringByReplacingOccurrencesOfString:@"FIRMWARE" withString:[[UIDevice currentDevice] systemVersion]];
	urlAddressSplashScreen = [urlAddressSplashScreen stringByReplacingOccurrencesOfString:@"UDIDPHONE" withString:macaddressSPLASH()];

//  urlAddressSplashScreen = @"http://ingameads.gameloft.com/redir/ads/splashscreen_view.php?from=FAMI&country=NZ&lg=EN&udid=1C:AB:A7:95:CE:BE&d=iPad3,1&f=5.1&game_ver=1.0.0&ver=1.0.0";

  NSLog(@"URL: %@", urlAddressSplashScreen);	
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
  // this method is called when the server has determined that it
  // has enough information to create the NSURLResponse
  // it can be called multiple times, for example in the case of a
  // redirect, so each time we reset the data.
  // receivedData is declared as a method instance elsewhere
  [receivedData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
  // append the new data to the receivedData
  // receivedData is declared as a method instance elsewhere
  [receivedData appendData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
  // release the connection, and the data object
	// receivedData is declared as a method instance elsewhere
	[myConnection release]; myConnection = nil;
	[receivedData release]; receivedData = nil;
	
  // inform the user
  NSLog(@"Connection failed! Error - %@ %@",
        [error localizedDescription],
        [[error userInfo] objectForKey:NSURLErrorFailingURLStringErrorKey]);
  
  [self quitSplash:nil];
}                                

- (void)connectionDidFinishLoading:(NSURLConnection *)connection      
{                                
  // do something with the data                    
  // receivedData is declared as a method instance elsewhere        
  NSLog(@"Succeeded! Received %d bytes of data", [receivedData length]);
	
	NSString* scriptData = [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding];
  NSLog(@"Script data: %@", scriptData);
	
	if([scriptData hasPrefix:@"1"])
	{
		[self startSplash];
	}
  else
  {
    [self quitSplash:nil];
  }
	
	// release the connection, and the data object 
	[myConnection release];	myConnection = nil;
	[receivedData release];	receivedData = nil;
}

NSString* macaddressSPLASH(){
  
  int                 mib[6];
  size_t              len;
  char                *buf;
  unsigned char       *ptr;
  struct if_msghdr    *ifm;
  struct sockaddr_dl  *sdl;
  
  mib[0] = CTL_NET;
  mib[1] = AF_ROUTE;
  mib[2] = 0;
  mib[3] = AF_LINK;
  mib[4] = NET_RT_IFLIST;
  
  if ((mib[5] = if_nametoindex("en0")) == 0) {
    printf("Error: if_nametoindex error\n");
    return NULL;
  }
  
  if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
    printf("Error: sysctl, take 1\n");
    return NULL;
  }
  
  if ((buf = (char*)malloc(len)) == NULL) {
    printf("Could not allocate memory. error!\n");
    return NULL;
  }
  
  if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
    printf("Error: sysctl, take 2");
    return NULL;
  }
  
  ifm = (struct if_msghdr *)buf;
  sdl = (struct sockaddr_dl *)(ifm + 1);
  ptr = (unsigned char *)LLADDR(sdl);
  NSString *outstring = [NSString stringWithFormat:@"%02X:%02X:%02X:%02X:%02X:%02X", 
                         *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
  free(buf);
  
  return outstring;
}

//END SPLASH SCREEN SPECIFIC code =====================================
//-----------------------------------------------------------------------------------------------------------------------------

@end
