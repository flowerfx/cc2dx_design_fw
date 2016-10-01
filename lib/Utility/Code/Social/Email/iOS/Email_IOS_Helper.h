

#ifndef _EMAIL_IOS_HELPER_H_
#define _EMAIL_IOS_HELPER_H_

#import <Foundation/Foundation.h>
#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>


@interface Email_iOS_Helper : UIViewController<MFMailComposeViewControllerDelegate>
{
    MFMailComposeViewController *mc ;
}

+(instancetype)defaultHelper;

-(void)ShowEmail:(NSString*)email_to withTitle:(NSString*)title andContent:(NSString*)content;

@end

#endif // _EMAIL_IOS_HELPER_H_

