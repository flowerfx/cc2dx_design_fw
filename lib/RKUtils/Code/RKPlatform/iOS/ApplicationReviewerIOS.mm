#include "ApplicationReviewerIOS.h"
#include <sys/types.h>
#include <sys/sysctl.h>

#import <UIKit/UIKit.h>
#include "RKNSLog.h"

@interface AppReviewerViewController : UIViewController
{
	NSMutableData* data; // Received data
}

@property (nonatomic, retain) NSMutableData* data;
@end

@implementation AppReviewerViewController

@synthesize data;

- (void)reviewURL:(NSString*)link
{
    const float kTimeOutInterval = 15.0f;

    NSURLRequest* request = [NSURLRequest requestWithURL:[NSURL URLWithString:link] cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:kTimeOutInterval];
    NSURLConnection* connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];

    if (connection)
	{
		self.data = [[NSMutableData data] retain];
    }
}

- (void)connection:(NSURLConnection*)connection didReceiveResponse:(NSURLResponse*)response
{
    // It can be called multiple times, for example in the case of a redirect, so each time we reset the data.
    [self.data setLength:0];
}

- (void)connection:(NSURLConnection*)connection didReceiveData:(NSData*)data
{
    [self.data appendData:data];
}

- (void)connection:(NSURLConnection*)connection didFailWithError:(NSError*)error
{
    [connection release];
    [self.data release];
    RKNSLOG(@"Connection failed: %@ %@", [error localizedDescription], [[error userInfo] objectForKey:NSErrorFailingURLStringKey]);
}

- (void)connectionDidFinishLoading:(NSURLConnection*)connection
{
	NSString* url = [[NSString alloc] initWithData:self.data encoding:NSUTF8StringEncoding];
	[connection release];
	[self.data release];
    
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
}

- (void)dealloc
{
    //[self.data release]; // NOTE(Diego): no need to release, it should have been release in the other messages.
    [super dealloc];
}

@end

AppReviewerViewController* appReviewerApi = nil;

namespace CasualCore
{
    void ApplicationReviewerIOS::Review(const char* gameCode, const char* gameVersion, const char* language, const char* country)
    {
        if (appReviewerApi == nil)
        {
            appReviewerApi = [AppReviewerViewController alloc];
        }
        
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        
        NSString* link = @"http://ingameads.gameloft.com/redir/?from=FROMGAME&op=IPHO&pp=1&t=review&game=TOGAME&ver=VERSION&lg=LANG&country=COUNTRY&d=DEVICE&f=FIRMWARE&udid=UDIDPHONE&ctg=GAME_REVIEW";
        NSString* nsGameCode = [NSString stringWithUTF8String:gameCode];

        link = [link stringByReplacingOccurrencesOfString:@"FROMGAME" withString:nsGameCode];
        link = [link stringByReplacingOccurrencesOfString:@"TOGAME" withString:nsGameCode]; // NOTE(Diego): same code
        link = [link stringByReplacingOccurrencesOfString:@"VERSION" withString:[NSString stringWithUTF8String:gameVersion]];
        link = [link stringByReplacingOccurrencesOfString:@"LANG" withString:[NSString stringWithUTF8String:language]];
        link = [link stringByReplacingOccurrencesOfString:@"COUNTRY" withString:[NSString stringWithUTF8String:country]];
        
        size_t size = 0;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char* machine = (char*)malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        NSString* platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];
        free(machine);
        
        link = [link stringByReplacingOccurrencesOfString:@"DEVICE" withString:platform];
        link = [link stringByReplacingOccurrencesOfString:@"FIRMWARE" withString:[[UIDevice currentDevice] systemVersion]];
        //link = [link stringByReplacingOccurrencesOfString:@"UDIDPHONE" withString:[[UIDevice currentDevice] uniqueIdentifier]];
        // [dzmitry.mazouka 05.10.2012] As this file doesn't seem to be in the project, I cannot provide correct implementation of the
        // replacement for the string above, and yet I disabled it as uniqueIdentifier is forbidden for usage and you should use mac address
        // instead. For your convinience I make sure you notice this message once you decide to compile this source file.
#error "A part of the functionality has been disabled and should be reimplemeted. Please see the comments for more information."

        [appReviewerApi performSelectorOnMainThread:@selector(reviewURL:) withObject:link waitUntilDone:YES];
        [pool drain];
    }

    void ApplicationReviewerIOS::Release()
    {
        [appReviewerApi release];
    }
}
