#if 0
#import "FreemiumIGPViewController.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/socket.h> // Per msqr
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include "Game.h"


//-----------------------------------------------------------------------------------------------------------------------------
//FREEMIUM IGP SPECIFIC code
#if defined (USE_UDID)
#define K_LINK_TOPDEALS @"http://ingameads.gameloft.com/redir/freemium/ifreemium.php?from=FROM&country=COUNTRY&lg=LANG&udid=UDIDPHONE&mac=MACADDRESS&ver=VERSION&d=DEVICE&f=FIRMWARE"
#else // Use mac address only
#define K_LINK_TOPDEALS @"http://ingameads.gameloft.com/redir/freemium/ifreemium.php?from=FROM&country=COUNTRY&lg=LANG&udid=MACADDRESS&ver=VERSION&d=DEVICE&f=FIRMWARE"
#endif

#define YOUR_IGP_GAMECODE @"TEST"

NSString *TXT_LANGUAGE_LIST[] = {
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

NSString *TXT_RES_LIST[] = {
	@"window_en.png",
	@"window_fr.png",
	@"window_de.png",
	@"window_it.png",
	@"window_sp.png",
	@"window_jp.png",
	@"window_kr.png",
	@"window_cn.png",
	@"window_br.png",
	@"window_ru.png"
};

NSString *TXT_RES_IPAD_LIST[] = {
	@"window_ipad_en.png",
	@"window_ipad_fr.png",
	@"window_ipad_de.png",
	@"window_ipad_it.png",
	@"window_ipad_sp.png",
	@"window_ipad_jp.png",
	@"window_ipad_kr.png",
	@"window_ipad_cn.png",
	@"window_ipad_br.png",
	@"window_ipad_ru.png"
};


NSString *TXT_RES_PORTRAIT_LIST[] = {
	@"window_portrait_en.png",
	@"window_portrait_fr.png",
	@"window_portrait_de.png",
	@"window_portrait_it.png",
	@"window_portrait_sp.png",
	@"window_portrait_jp.png",
	@"window_portrait_kr.png",
	@"window_portrait_cn.png",
	@"window_portrait_br.png",
	@"window_portrait_ru.png"
};

NSString *TXT_RES_PORTRAIT_IPAD_LIST[] = {
	@"window_ipad_portrait_en.png",
	@"window_ipad_portrait_fr.png",
	@"window_ipad_portrait_de.png",
	@"window_ipad_portrait_it.png",
	@"window_ipad_portrait_sp.png",
	@"window_ipad_portrait_jp.png",
	@"window_ipad_portrait_kr.png",
	@"window_ipad_portrait_cn.png",
	@"window_ipad_portrait_br.png",
	@"window_ipad_portrait_ru.png"
};



NSString *TXT_NET_ERROR_FREEMIUM[] =
{
    @"No internet connection available. Please make sure your device is connected to the internet.",
    @"Aucune connexion Internet disponible. Veuillez vous assurer que votre appareil est connecté à Internet",
    @"Keine Internetverbindung verfügbar. Versichere dich bitte, dass dein Gerät mit dem Internet verbunden ist.",
    @"Connessione internet non disponibile. Assicurati che il tuo dispositivo sia connesso a internet.",
    @"No hay ninguna conexión a internet disponible. Por favor, asegúrate de que tu dispositivo esté conectado a internet.",
    @"インターネット接続を利用できません\nデバイスがインターネットに接続されているかご確認ください",
    @"인터넷에 연결되어 있지 않습니다. 장치가 인터넷에 연결되어 있는지 확인하세요.",
    @"无网络连接。请确认您的设备已经连入网络。", //CN
    @"Não há conexão disponível. Certifique-se de que seu aparelho está conectado à internet.", //BR
    @"Невозможно соединиться с сервером. Сеть не найдена. Пожалуйста, попробуйте позже." //RU	
};

NSString *TXT_OK_IGP_FREEMIUM[] =
{
    @"OK",
    @"OK",
    @"OK",
    @"OK",
    @"Acep.",
    @"OK",
    @"OK",
    @"OK",
    @"OK",
    @"OK"
};



static bool _bActived = false; // Checks whether the IGP is running, For stopping doulbe display the IGP dialog.
int _gamelanguage = 0;

NSString*		urlAddress;

@implementation FreemiumIGPViewController

@synthesize igp_td_webView, addressBar, activityIndicator;
@synthesize m_backButton;
@synthesize igptimer;
//END FREEMIUM IGP SPECIFIC code =====================================
//-----------------------------------------------------------------------------------------------------------------------------

// Implement loadView to create a view hierarchy programmatically,
// without using a nib.
- (void)loadView 
{
  if(_bActived)
    return;
	//-----------------------------------------------------------------------------------------------------------------------------
	//FREEMIUM IGP SPECIFIC code
	igp_td_display_mode = IGP_SET_IN_LANDSCAPE_LEFT_MODE;
	
	igp_td_SHADOW_X = 0;
	igp_td_SHADOW_Y = 0;
	
	
	NSString *firmware = [[UIDevice currentDevice] systemVersion];
	device_firmware = 3;
	
	if([firmware hasPrefix:@"2"])
	{
		device_firmware = 2;
	}
	
	b_is_in_mm = true;
	screenOrientation = UIInterfaceOrientationLandscapeLeft;
    
	
	//END FREEMIUM IGP SPECIFIC code
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
		self.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 1024, 768)];
	}
	else
	{
		self.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 480, 320)];
	}
	////set the view's background color
	//self.view.backgroundColor = [UIColor orangeColor];
    
	//for(int i = 0; i < NO_LANGS; i++)
	//{
	//	lang_buttons[i] = [UIButton buttonWithType:UIButtonTypeRoundedRect];
	//	lang_buttons[i].frame = CGRectMake(10 + 30*i, 0, 30, 30);
	//	[lang_buttons[i] setTitle:TXT_LANGUAGE_LIST[i] forState:UIControlStateNormal];
	//	[lang_buttons[i] addTarget:self action:@selector(buttonPressed:) forControlEvents:UIControlEventTouchUpInside];
	//	[self.view addSubview:lang_buttons[i]];
	//}
	
	
	//-----------------------------------------------------------------------------------------------------------------------------
	//FREEMIUM IGP SPECIFIC code =====================================
	b_drawing_done = false;
	keyboard_is_shown = false;
	
	if (igp_td_display_mode == IGP_SET_IN_LANDSCAPE_RIGHT_MODE)
	{
		screenOrientation = UIInterfaceOrientationLandscapeRight;
	}
	
	if (igp_td_display_mode == IGP_SET_IN_LANDSCAPE_LEFT_MODE)
	{
		screenOrientation = UIInterfaceOrientationLandscapeLeft;
	}
	
	if (igp_td_display_mode == IGP_SET_IN_PORTRAIT_MODE)
	{
		screenOrientation = UIInterfaceOrientationPortrait;
	}
    
    if (igp_td_display_mode == IGP_SET_IN_PORTRAIT_UPSIDEDOWN_MODE)
	{
		screenOrientation = UIInterfaceOrientationPortraitUpsideDown;
	}
    
	
	
	//END FREEMIUM IGP SPECIFIC code =====================================
	//-----------------------------------------------------------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------------------------------------------------------
//FREEMIUM IGP SPECIFIC code =====================================
-(void) drawIGPTDBrowserWindow:(int)orientation: (int) yoffset
{
	
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
	free(machine);
	if([platform hasPrefix:@"iPad"])
	{
		
		int x = 119;
        int y = 125;
        if((orientation == UIInterfaceOrientationPortrait)||(orientation == UIInterfaceOrientationPortraitUpsideDown))
		{
            x = 125;
            y = 119;
            
            m_headerView.image = m_headerImagePortrait;
            [m_headerView setFrame:CGRectMake(y, x, 517, 768)];
            [m_backButton setFrame:CGRectMake(y + 442, x + 10, 64, 57)];		
            [igp_td_webView setFrame:CGRectMake( y + 15,x+85, 410 + 78,730 - 70)];
            [activityIndicator setFrame:CGRectMake( y+226,x+352, 64, 64)];
            [my_image2 setFrame:CGRectMake( igp_td_SHADOW_Y,igp_td_SHADOW_X, 768, 1024)];            
        }
        else
        {
            m_headerView.image = m_headerImageLandscape;
            [m_headerView setFrame:CGRectMake(x, y, 768, 517)];
            [m_backButton setFrame:CGRectMake(x+691, y + 13, 64, 57)];		
            [igp_td_webView setFrame:CGRectMake(x+20, y+88, 730, 410)];
            [activityIndicator setFrame:CGRectMake(x+352, y+226, 64, 64)];
            [my_image2 setFrame:CGRectMake(igp_td_SHADOW_X, igp_td_SHADOW_Y, 1024, 768)];
        }
        
        
        
	}
	else 
	{
		if(orientation == UIInterfaceOrientationLandscapeRight)
		{
			int x = 12;
			int y = LANDSCAPE_Y - yoffset + 162 - 240;
			
			m_headerView.image = m_headerImageLandscape;
            
			[m_headerView setFrame:CGRectMake(x, y, 456, 300)];
			[m_backButton setFrame:CGRectMake(x+406, y+5, 45, 34)];		
			[igp_td_webView setFrame:CGRectMake(x+10, y+48, 437, 242)];
			[activityIndicator setFrame:CGRectMake(x+208, y+130, 32, 32)];
			[my_image2 setFrame:CGRectMake(igp_td_SHADOW_X, igp_td_SHADOW_Y, 480, 320)];
			
		}
		
		if(orientation == UIInterfaceOrientationLandscapeLeft)
		{
			int x = 12 ;//- 80;
			int y = LANDSCAPE_Y - yoffset + 162 - 240;//- 78 + 80;
            
            m_headerView.image = m_headerImageLandscape;
			
			[m_headerView setFrame:CGRectMake(x, y, 456, 300)];
			[m_backButton setFrame:CGRectMake(x+406, y+5, 45, 34)];
			[igp_td_webView setFrame:CGRectMake(x+10, y+48, 437, 242)];
			[activityIndicator setFrame:CGRectMake(x+208, y+130, 32, 32)];
			
			[my_image2 setFrame:CGRectMake(igp_td_SHADOW_X , igp_td_SHADOW_Y, 480, 320)];
		}
		
		if((orientation == UIInterfaceOrientationPortrait)||(orientation == UIInterfaceOrientationPortraitUpsideDown))
		{
			
            
            int x = 12;
			int y = LANDSCAPE_Y - yoffset + 162 - 240;
			
			m_headerView.image = m_headerImagePortrait;
			[m_headerView setFrame:CGRectMake(y, x, 300, 456)];
            [m_backButton setFrame:CGRectMake(270, 20, 30, 30)];
			[igp_td_webView setFrame:CGRectMake( y + 10,x+55, 242 + 38, 437 - 45)];
			[activityIndicator setFrame:CGRectMake(y+130,x+208, 32, 32)];
            [my_image2 setFrame:CGRectMake(igp_td_SHADOW_X, igp_td_SHADOW_Y, 320, 480)];
		}
		
	}
}

double _degreesToRadian(int degrees)
{
	double _PI = 3.14159265;
	return degrees*_PI/180;
}

-(IBAction)buttonPressed:(id)sender
{	
  if(_bActived)
    return;
  _bActived = true;
  
	_gamelanguage = CasualCore::Game::GetInstance()->GetLanguage();
	{
        connection_was_ok = false;
        
        if(!b_is_in_mm)
        {
            return;
        }
        
        //timeout after 20 seconds
        igptimer = [NSTimer scheduledTimerWithTimeInterval: 20 target:self 
                                                  selector:@selector(targetMethod:) userInfo:nil repeats:YES];
        
        igp_td_SHADOW_X = 0;
        igp_td_SHADOW_Y = 0;
		
        if (igp_td_display_mode == IGP_SET_IN_LANDSCAPE_RIGHT_MODE)
        {
            [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
        }
		
        if (igp_td_display_mode == IGP_SET_IN_LANDSCAPE_LEFT_MODE)
        {
            [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
        }
		
        if (igp_td_display_mode == IGP_SET_IN_PORTRAIT_MODE)
        {
            [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortrait;
        }
        if (igp_td_display_mode == IGP_SET_IN_PORTRAIT_UPSIDEDOWN_MODE)
        {
            [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortraitUpsideDown;
        }
        
        NSLog(@"Button Pressed!");
        
        m_subVC = [[UIViewController alloc] init];
        
        
        
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
        
        my_image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        
        my_image2 = [[UIImageView alloc] initWithImage:my_image];
        [my_image2 setBounds:CGRectMake(0, 0, 768, 1024)];
        if([platform hasPrefix:@"iPad"])
        {
            m_subVC.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 768, 1024)];
        }
        else
        {
            m_subVC.view = [[UIView alloc] init];
            [m_subVC.view setCenter:CGPointMake(0, 0)];
            [m_subVC.view setBounds:CGRectMake(0, 0, 320, 480)];
        }
        
        
        [m_subVC.view addSubview:my_image2];
        
        
        //window background
        char * buff;
        int len;
        
        FILE* ptr;
        if([platform hasPrefix:@"iPad"])
        {
            ptr= openFile((char*)[TXT_RES_IPAD_LIST[_gamelanguage] UTF8String]);
        }
        else
        {
            ptr= openFile((char*)[TXT_RES_LIST[_gamelanguage] UTF8String]);
        }
        
        
        len = ftell(ptr);
        fseek( ptr, 0L, SEEK_END);
        len = ftell(ptr) - len + 1;
        fseek( ptr, 0L, SEEK_SET);
        
        buff = (char *)malloc(len * sizeof(char));
        fread(buff, sizeof(char), len, ptr);
        
        NSData* data = [[NSData alloc]initWithBytes:buff length:len];
        m_headerImageLandscape = [[UIImage alloc] initWithData:data];
        
        [data release];
        free(buff);
        fclose(ptr);
        
        
        if([platform hasPrefix:@"iPad"])
        {
            ptr= openFile((char*)[TXT_RES_PORTRAIT_IPAD_LIST[_gamelanguage] UTF8String]);
        }
        else
        {
            ptr= openFile((char*)[TXT_RES_PORTRAIT_LIST[_gamelanguage] UTF8String]);
        }
        
        len = ftell(ptr);
        fseek( ptr, 0L, SEEK_END);
        len = ftell(ptr) - len + 1;
        fseek( ptr, 0L, SEEK_SET);
        
        buff = (char *)malloc(len * sizeof(char));
        fread(buff, sizeof(char), len, ptr);
        
        data = [[NSData alloc]initWithBytes:buff length:len];
        m_headerImagePortrait = [[UIImage alloc] initWithData:data];
        
        [data release];
        free(buff);
        fclose(ptr);
        
        
        m_headerView = [[UIImageView alloc] initWithImage:m_headerImageLandscape];
        [m_headerView setFrame:CGRectMake(20.0, 130.0-25.0, 280.0, 220.0)];
        [m_subVC.view addSubview:m_headerView];
        
        
        
        //close button image
        if([platform hasPrefix:@"iPad"])
        {
            ptr = openFile("close_ipad.png");
    	}
    	else
    	{
        	if([[[UIDevice currentDevice] systemVersion] hasPrefix:@"3."])
        	{
                ptr = openFile("close_but_low.png");
        	}
        	else
        	{
                ptr = openFile("close_but.png");
        	}
        }
        len = ftell(ptr);
        fseek( ptr, 0L, SEEK_END);
        len = ftell(ptr) - len + 1;
        fseek( ptr, 0L, SEEK_SET);
        
        buff = (char *)malloc(len * sizeof(char));
        fread(buff, sizeof(char), len, ptr);
        
        NSData* data_close_button = [[NSData alloc]initWithBytes:buff length:len];
        UIImage* img_close_button = [UIImage imageWithData:data_close_button];
        
        [data_close_button release];
        free(buff);
        fclose(ptr);
        
        //close button object
        m_backButton = [[UIButton alloc] initWithFrame:CGRectMake(250.0, 152.0-25.0, 35.0, 39.0)];
        [m_backButton setImage: img_close_button forState:UIControlStateNormal];
        [m_backButton addTarget:self action:@selector(quitIGPTD:) forControlEvents:UIControlEventTouchUpInside];
        [m_subVC.view addSubview:m_backButton];
        
        //web browser
        CGRect webFrame;
        
        webFrame = CGRectMake(23.0, 130.0 + 59.0-25.0, 274.0, 155.0);
		
        igp_td_webView = [[UIWebView alloc] initWithFrame:webFrame];
        [m_subVC.view addSubview:igp_td_webView];
        
        [igp_td_webView setBackgroundColor:[UIColor clearColor]];
        getTopDealsLink();
        NSURL *url = [NSURL URLWithString:urlAddress];
        NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
        [igp_td_webView loadRequest:requestObj];
        [igp_td_webView setDelegate:self];
        
        //activity indicator - loading animation
        activityIndicator = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
        [m_subVC.view addSubview:activityIndicator];
        [activityIndicator setFrame:CGRectMake(145.0, 240.0-25.0, 32, 32)];
        [activityIndicator setHidden:NO];
        
        [self.view addSubview:m_subVC.view];
    
        // [Rong] fixing the offset issue without massing the view bounds
        if([platform hasPrefix:@"iPad"])
        {
          m_subVC.view.frame = CGRectMake(0, 0, 1024, 768);
        }
        else
        {
          m_subVC.view.frame = CGRectMake(0,0,480,320);
        }
        
        b_is_in_mm = false;	
        
        
        if(screenOrientation == UIInterfaceOrientationLandscapeRight)
        {
            [self drawIGPTDBrowserWindow:UIInterfaceOrientationLandscapeRight:0];
        }
        
        if(screenOrientation == UIInterfaceOrientationLandscapeLeft)
        {
            [self drawIGPTDBrowserWindow:UIInterfaceOrientationLandscapeLeft:0];
        }
        
        if(screenOrientation == UIInterfaceOrientationPortrait)
        {
            [self drawIGPTDBrowserWindow:UIInterfaceOrientationPortrait:0];
        }
        if(screenOrientation == UIInterfaceOrientationPortraitUpsideDown)
        {
            [self drawIGPTDBrowserWindow:UIInterfaceOrientationPortraitUpsideDown:0];
        }
        
#ifndef USE_AUTOROTATE
		if (screenOrientation == UIInterfaceOrientationLandscapeRight)
		{
			NSLog(@"\nROTATE LANDSCAPE RIGHT\n");
			m_subVC.view.transform = CGAffineTransformMakeRotation(M_PI/2);
			if([platform hasPrefix:@"iPad"])
			{
				[m_subVC.view setCenter:CGPointMake(384, 512)];
				[m_subVC.view setBounds:CGRectMake(0, 0, 1024, 768)];
			}
			else
			{
				[m_subVC.view setCenter:CGPointMake(160, 240 )];
				[m_subVC.view setBounds:CGRectMake(0, 0, 480, 320)];
			}
			return;
		}
		
		if (screenOrientation == UIInterfaceOrientationLandscapeLeft)
		{
			NSLog(@"\nROTATE LANDSCAPE LEFT\n");
			//m_subVC.view.transform = CGAffineTransformMakeRotation(-M_PI/2);
			
			if([platform hasPrefix:@"iPad"])
			{
                [m_subVC.view setCenter:CGPointMake(512, 384)];
				[m_subVC.view setBounds:CGRectMake(0, 0, 1024, 768)];
				
			}
			else
			{
				[m_subVC.view setCenter:CGPointMake(160, 240)];
				[m_subVC.view setBounds:CGRectMake(0, 0, 480, 320)];
			}
			return;
		}
		
		if (screenOrientation == UIInterfaceOrientationPortrait)
		{
			NSLog(@"\nPORTRAIT\n");
			m_subVC.view.transform = CGAffineTransformMakeRotation(0);
            if([platform hasPrefix:@"iPad"])
			{
				[m_subVC.view setCenter:CGPointMake(384, 512)];
				[m_subVC.view setBounds:CGRectMake(0, 0, 768, 1024)];
			}
			else
			{
				[m_subVC.view setCenter:CGPointMake(160, 240 )];
				[m_subVC.view setBounds:CGRectMake(0, 0, 320, 480)];
			}
			return;
		}
        
        if (screenOrientation == UIInterfaceOrientationPortraitUpsideDown)
		{
			NSLog(@"\nPORTRAIT\n");
			m_subVC.view.transform = CGAffineTransformMakeRotation(M_PI);
            if([platform hasPrefix:@"iPad"])
			{
				[m_subVC.view setCenter:CGPointMake(384, 512)];
				[m_subVC.view setBounds:CGRectMake(0, 0, 768, 1024)];
			}
			else
			{
				[m_subVC.view setCenter:CGPointMake(160, 240 )];
				[m_subVC.view setBounds:CGRectMake(0, 0, 320, 480)];
			}
			return;
		}
		
		if (screenOrientation == UIInterfaceOrientationPortraitUpsideDown)
		{
			NSLog(@"\nUPSIDE DOWN\n");
			m_subVC.view.transform = CGAffineTransformMakeRotation(M_PI);
			return;
		}
#endif
	}
}


- (void)viewDidAppear {
    [super viewDidLoad];
	
	NSLog(@"%@\n", urlAddress);
	
	NSURL *url = [NSURL URLWithString:urlAddress];
	NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
	
	[igp_td_webView loadRequest:requestObj];
	[addressBar setText:urlAddress];
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
		[addressBar setText:urlAddress];
		return NO;
	}
	
	if([urlString hasPrefix:@"http://phobos.apple.com"]||[urlString hasPrefix:@"http://itunes.apple.com"])
	{
		[activityIndicator stopAnimating];
		connection_was_ok = true;
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
	}
	if([urlString hasPrefix:@"play:"])
	{
		[activityIndicator stopAnimating];
		connection_was_ok = true;
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:[urlString substringFromIndex:5]]];
	}
	return YES;
}





- (void)webViewDidStartLoad:(UIWebView *)webView {
	[activityIndicator startAnimating];
}

- (void)webViewDidFinishLoad:(UIWebView *)webView {
	[activityIndicator stopAnimating];
	connection_was_ok = true;
}


- (void)webView:(UIWebView *)webView didFailWithError:(NSError *)error
{
	NSLog(@"Error:%@", error);
	[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible: NO];
}


// Get path to a file in resources or documents folder
void GetFileNameFreemiumIGP (const char* filename, bool bWrite, char*p)
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
			path = [NSString stringWithFormat:@"%@%@%@",[[NSBundle mainBundle] resourcePath],@"/FREEMIUMIGP_RES/",path];
		}
	}
	if (path != nil) sprintf(p, "%s", [path UTF8String]);
	
	[pool release];
}


FILE* openFile(const char* name)
{
	FILE* f;
	char path[256];
	GetFileNameFreemiumIGP(name, false, path);
	f = fopen(path, "rb");
	if (f) return f;
	GetFileNameFreemiumIGP(name, true, path);
	f = fopen(path, "rb");
	return f;	
}

FILE* openFile(char* name)
{
  const char* const constName = name;
  return openFile(constName);
}


- (void) targetMethod: (NSTimer*)theTimer
{
	if(connection_was_ok)
	{
		return;
	}
	
	NSString *_message_lang = TXT_NET_ERROR_FREEMIUM[_gamelanguage];
	NSString *_ok_lang = TXT_OK_IGP_FREEMIUM[_gamelanguage];
	
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:_message_lang delegate:self cancelButtonTitle:_ok_lang otherButtonTitles:nil];
	[alert show];
	[alert release];
	
	
	[igptimer invalidate];
	
	NSLog(@"Quitting IGP...n");
    
	
	[m_headerView removeFromSuperview];
	[m_headerView release];
	
    [m_headerImagePortrait release];
    [m_headerImageLandscape release];
    
	[m_backButton removeFromSuperview];
	[m_backButton release];
	
	[my_image2 removeFromSuperview];
	[my_image2 release];
	
    //	free(my_image);
	
	[igp_td_webView removeFromSuperview];
	[igp_td_webView release];
	
	[activityIndicator removeFromSuperview];
	[activityIndicator release];
	
#ifdef USE_AUTOROTATE
    screenOrientation = UIInterfaceOrientationLandscapeLeft;
#endif
	
	b_is_in_mm = true;
	
	[igptimer invalidate];
	if (m_subVC)
	{
		[m_subVC.view removeFromSuperview];
		[m_subVC release];
        m_subVC = nil;
	}
	if(self.view)
  {
    [self.view removeFromSuperview];
    [self.view release];
    self.view = nil;
  }
  _bActived = false;
}

- (void) quitIGPTD:(id) sender
{	
	if(keyboard_is_shown)
	{
		return;
	}
	
	NSLog(@"Quitting IGP...n");
	
	
	[m_headerView removeFromSuperview];
	[m_headerView release];
	m_headerView = nil;
	
    [m_headerImagePortrait release];
    [m_headerImageLandscape release];
    
	[m_backButton removeFromSuperview];
	[m_backButton release];
	m_backButton = nil;
	
	[my_image2 removeFromSuperview];
	[my_image2 release];
	my_image2 = nil;
	
    //	free(my_image);
	
	[igp_td_webView removeFromSuperview];
	[igp_td_webView release];
	igp_td_webView = nil;
	
	[activityIndicator removeFromSuperview];
	[activityIndicator release];
	activityIndicator = nil;
	
#ifdef USE_AUTOROTATE
    screenOrientation = UIInterfaceOrientationLandscapeLeft;
#endif
	
	b_is_in_mm = true;
	
	[igptimer invalidate];
    
	if (igp_td_display_mode == IGP_SET_IN_LANDSCAPE_RIGHT_MODE)
	{
		[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
	}
    
	if (igp_td_display_mode == IGP_SET_IN_LANDSCAPE_LEFT_MODE)
	{
		[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
	}
    
	if (igp_td_display_mode == IGP_SET_IN_PORTRAIT_MODE)
	{
		[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortrait;
	}
    if (igp_td_display_mode == IGP_SET_IN_PORTRAIT_UPSIDEDOWN_MODE)
	{
		[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortraitUpsideDown;
	}
	
	if (m_subVC)
	{
        [m_subVC.view removeFromSuperview];
        [m_subVC release];
        m_subVC = nil;
	}

    // [Rong] remove the view
    //self.view = nil;
    if(self.view)
    {
      [self.view removeFromSuperview];
      [self.view release];
      self.view = nil;
    }
  _bActived = false;
}

void getTopDealsLink()
{
	urlAddress = K_LINK_TOPDEALS;
  NSString* gameVersion = [NSString stringWithUTF8String:(CasualCore::Game::GetInstance()->GetGameVersion())];
	urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"VERSION" withString:gameVersion];
    NSString* IGP_GAMECODE = [NSString stringWithUTF8String:CasualCore::Game::GetInstance()->GetGameCode()];
	urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"FROM"  withString:IGP_GAMECODE];
	urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"LANG"  withString:TXT_LANGUAGE_LIST[_gamelanguage]];
	urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"COUNTRY"  withString:[[NSLocale currentLocale] objectForKey:NSLocaleCountryCode]];
	
	size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = (char*)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
	free(machine);
	
  NSString* deviceUDID = [[UIDevice currentDevice] uniqueIdentifier];
	urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"DEVICE"  withString: platform];
	urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"FIRMWARE"  withString:[[UIDevice currentDevice] systemVersion]];
  urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"UDIDPHONE"  withString:deviceUDID];
	urlAddress = [urlAddress stringByReplacingOccurrencesOfString:@"MACADDRESS"  withString:macaddressIGP()];
	
	NSLog(@"%@\n", urlAddress);
}
NSString* macaddressIGP()
{
    
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
//END FREEMIUM IGP SPECIFIC code =====================================

//-----------------------------------------------------------------------------------------------------------------------------
- (void) didRotate:(NSNotification *)notification
{
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	
	if((orientation == UIDeviceOrientationPortrait)||
	   (orientation == UIDeviceOrientationPortraitUpsideDown)||
	   (orientation == UIDeviceOrientationLandscapeLeft)||
	   (orientation == UIDeviceOrientationLandscapeRight))
		;
	else {
		return;
	}
	
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
	free(machine);
	
	if(orientation == UIInterfaceOrientationLandscapeRight)
	{
		[self drawIGPTDBrowserWindow:UIInterfaceOrientationLandscapeRight:0];
	}
	
	if(orientation == UIInterfaceOrientationLandscapeLeft)
	{
		[self drawIGPTDBrowserWindow:UIInterfaceOrientationLandscapeLeft:0];
	}
    if(orientation == UIInterfaceOrientationPortrait)
	{
		[self drawIGPTDBrowserWindow:UIInterfaceOrientationPortrait:0];
	}
    if(orientation == UIInterfaceOrientationPortraitUpsideDown)
	{
		[self drawIGPTDBrowserWindow:UIInterfaceOrientationPortraitUpsideDown:0];
	}
	
	
	if (orientation == UIInterfaceOrientationLandscapeRight)
	{
		NSLog(@"\nROTATE LANDSCAPE RIGHT\n");
		m_subVC.view.transform = CGAffineTransformMakeRotation(M_PI/2);
		if([platform hasPrefix:@"iPad"])
		{
			[m_subVC.view setCenter:CGPointMake(384, 512)];
			[m_subVC.view setBounds:CGRectMake(0, 0, 1024, 768)];
		}
		else
		{
			[m_subVC.view setCenter:CGPointMake(160, 240 )];
			[m_subVC.view setBounds:CGRectMake(0, 0, 480, 320)];
		}
		return;
	}
	
	if (orientation == UIInterfaceOrientationLandscapeLeft)
	{
		NSLog(@"\nROTATE LANDSCAPE LEFT\n");
		m_subVC.view.transform = CGAffineTransformMakeRotation(-M_PI/2);
		
		if([platform hasPrefix:@"iPad"])
		{
			[m_subVC.view setBounds:CGRectMake(0, 0, 1024, 768)];
			
		}
		else
		{
			[m_subVC.view setCenter:CGPointMake(160, 240)];
			[m_subVC.view setBounds:CGRectMake(0, 0, 480, 320)];
		}
		return;
	}
    if (orientation == UIInterfaceOrientationPortrait)
    {
        m_subVC.view.transform = CGAffineTransformMakeRotation(0);
        if([platform hasPrefix:@"iPad"])
        {
            [m_subVC.view setCenter:CGPointMake(384, 512)];
            [m_subVC.view setBounds:CGRectMake(0, 0, 768, 1024)];
        }
        else
        {
            [m_subVC.view setCenter:CGPointMake(160, 240 )];
            [m_subVC.view setBounds:CGRectMake(0, 0, 320, 480)];
        }
    }
    
    if (orientation == UIInterfaceOrientationPortraitUpsideDown)
    {
        m_subVC.view.transform = CGAffineTransformMakeRotation(M_PI);
        if([platform hasPrefix:@"iPad"])
        {
            [m_subVC.view setCenter:CGPointMake(384, 512)];
            [m_subVC.view setBounds:CGRectMake(0, 0, 768, 1024)];
        }
        else
        {
            [m_subVC.view setCenter:CGPointMake(160, 240 )];
            [m_subVC.view setBounds:CGRectMake(0, 0, 320, 480)];
        }
    }
    
	
}
@end

#endif
