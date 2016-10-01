
#import <UIKit/UIKit.h>

#define SPLASHSCREEN_SET_IN_LANDSCAPE_RIGHT_MODE	1
#define SPLASHSCREEN_SET_IN_LANDSCAPE_LEFT_MODE		2
#define SPLASHSCREEN_SET_IN_PORTRAIT_MODE			    3

#define LANDSCAPE_Y 88

@interface SplashScreenViewController : UIViewController <UIWebViewDelegate> {
  
	//-----------------------------------------------------------------------------------------------------------------------------
	//IGP TOP DEALS SPECIFIC code =====================================
	IBOutlet UIWebView	*splash_webview;
  //	IBOutlet UITextField *addressBar;
  //	IBOutlet UIActivityIndicatorView *activityIndicator;
	
	//UIImageView*	m_headerView;
	//UILabel*		m_titleLabel;
  //	UIButton*		m_backButton;
	UIImage*		dimming_image;
	UIImageView*	dimming_image2;
	
	NSURLConnection*	myConnection;
	NSMutableData*		receivedData;
	
  //	bool b_thread_done;
  //	bool b_open_browser_automatically;
  //	bool b_drawing_done;

//  Gameloft Auckland specific
//  #define NO_LANGS 10
//	UIButton *lang_buttons[NO_LANGS];
//	UIButton *click_me_button;
	
	int splash_screen_screenOrientation;
  //	int keyboardOrientation;
  //	bool b_is_in_mm;
	
	//bool connection_was_ok;
	
	bool keyboard_is_shown;
	
	UIViewController *splash_screen_subVC;
	
  //	NSTimer *igptimer;
	
	
	int splash_screen_display_mode;
	
	int splash_screen_SHADOW_X;
	int splash_screen_SHADOW_Y;
	//END IGP TOP DEALS SPECIFIC code
	//-----------------------------------------------------------------------------------------------------------------------------
}


//-----------------------------------------------------------------------------------------------------------------------------
//IGP TOP DEALS SPECIFIC code =====================================
@property(nonatomic,retain) UIWebView *splash_webview;
//@property(nonatomic,retain) UITextField *addressBar;
//@property(nonatomic,retain) UIActivityIndicatorView *activityIndicator;
//@property(nonatomic,retain) UIButton *m_backButton;
//@property(nonatomic,retain) NSTimer *igptimer;

-(IBAction) gotoAddress:(id)sender;
-(IBAction) goBack:(id)sender;
-(IBAction) goForward:(id)sender;

FILE* openFileSS(char* name);
void GetFileNameSplashScreen (const char* filename, bool bWrite, char*p);
- (void) quitSplash:(id) sender;
- (void)setLanguage:(NSString*)language;
void getSplashScreenLink();

double _degreesToRadianSS(int degrees);
-(IBAction)buttonPressed:(id)sender;
NSString* macaddressSPLASH();

-(void) drawSPLASHSCREENBrowserWindow:(int)orientation:(int) yoffset;

- (void)connection:(NSURLConnection*)connection didFailWithError:(NSError*)error;
- (void)connection:(NSURLConnection*)connection didReceiveData:(NSData*)data;
- (void)connection:(NSURLConnection*)connection didReceiveResponse:(NSURLResponse*)response;
- (void)connectionDidFinishLoading:(NSURLConnection*)connection;
//END IGP TOP DEALS SPECIFIC code
//-----------------------------------------------------------------------------------------------------------------------------

@end

