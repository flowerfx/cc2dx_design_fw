

#import <UIKit/UIKit.h>

#define IGP_SET_IN_LANDSCAPE_RIGHT_MODE		1
#define IGP_SET_IN_LANDSCAPE_LEFT_MODE		2
#define IGP_SET_IN_PORTRAIT_MODE			3
#define IGP_SET_IN_PORTRAIT_UPSIDEDOWN_MODE	4

#define LANDSCAPE_Y 88

#define USE_AUTOROTATE

@interface FreemiumIGPViewController : UIViewController <UIWebViewDelegate> {
    
	//-----------------------------------------------------------------------------------------------------------------------------
	//IGP TOP DEALS SPECIFIC code =====================================
	IBOutlet UIWebView	*igp_td_webView;
	IBOutlet UITextField *addressBar;
	IBOutlet UIActivityIndicatorView *activityIndicator;
	
	UIImageView*	m_headerView;
   	UIImage*        m_headerImagePortrait;
    UIImage*        m_headerImageLandscape;
	UILabel*		m_titleLabel;
	UIButton*		m_backButton;
	UIImage*		my_image;
	UIImageView*	my_image2;
	
	bool b_thread_done;
	bool b_open_browser_automatically;
	bool b_drawing_done;
	
#define NO_LANGS 10
	
	UIButton *lang_buttons[NO_LANGS];
	UIButton *click_me_button;
	
	int screenOrientation;
	int keyboardOrientation;
	bool b_is_in_mm;
	
	bool connection_was_ok;
	
	bool keyboard_is_shown;
	
	UIViewController *m_subVC;
	
	NSTimer *igptimer;
	
	NSNotificationCenter *kbd_notification;
	
	int device_firmware;
	
	int igp_td_tries_no;
	
	int igp_td_display_mode;
	
	int igp_td_SHADOW_X;
	int igp_td_SHADOW_Y;
	//END IGP TOP DEALS SPECIFIC code
	//-----------------------------------------------------------------------------------------------------------------------------
}


//-----------------------------------------------------------------------------------------------------------------------------
//IGP TOP DEALS SPECIFIC code =====================================
@property(nonatomic,retain) UIWebView *igp_td_webView;
@property(nonatomic,retain) UITextField *addressBar;
@property(nonatomic,retain) UIActivityIndicatorView *activityIndicator;
@property(nonatomic,retain) UIButton *m_backButton;
@property(nonatomic,retain) NSTimer *igptimer;

-(IBAction) gotoAddress:(id)sender;
-(IBAction) goBack:(id)sender;
-(IBAction) goForward:(id)sender;

FILE* openFile(char* name);
void GetFileNameFreemiumIGP (const char* filename, bool bWrite, char*p);
- (void) quitIGPTD:(id) sender;
void getTopDealsLink();
NSString* macaddressIGP();

double _degreesToRadian(int degrees);
-(void) showIGPView;
-(IBAction)buttonPressed:(id)sender;
void timeoutConnectionIGP();

-(void) drawIGPTDBrowserWindow:(int)orientation:(int) yoffset;

//END IGP TOP DEALS SPECIFIC code
//-----------------------------------------------------------------------------------------------------------------------------

@end

