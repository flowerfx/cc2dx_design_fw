
#import "Email_iOS_Helper.h"

@implementation Email_iOS_Helper

static Email_iOS_Helper *m_instance_email = nil;

+ (Email_iOS_Helper*)defaultHelper {
    // dispatch_once will ensure that the method is only called once (thread-safe)
    static dispatch_once_t pred = 0;
    dispatch_once(&pred, ^{
        m_instance_email = [[Email_iOS_Helper alloc] init];
    });
    return m_instance_email;
}

- (id)init {
    if ((self = [super init])) {
    }
    return self;
    
}

- (void)ShowEmail:(NSString*)email_to withTitle:(NSString*)title andContent:(NSString*)content; {
    // Email Subject
    NSString *emailTitle = title;
    // Email Content
    NSString *messageBody = content;
    // To address
    NSArray *toRecipents = [NSArray arrayWithObject:email_to];
    
    
    mc = [[MFMailComposeViewController alloc] init];
    mc.mailComposeDelegate = self;
    [mc setSubject:emailTitle];
    [mc setMessageBody:messageBody isHTML:NO];
    [mc setToRecipients:toRecipents];
    
    // Present mail view controller on screen
    [self presentViewController:mc animated:YES completion:nil];
    
    self.view.hidden = NO;
    
    //add to gl view
     UIWindow *window = [UIApplication sharedApplication].keyWindow;
    [window addSubview:self.view];
    
}

- (void) mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError *)error
{
    switch (result)
    {
        case MFMailComposeResultCancelled:
            NSLog(@"Mail cancelled");
            break;
        case MFMailComposeResultSaved:
            NSLog(@"Mail saved");
            break;
        case MFMailComposeResultSent:
            NSLog(@"Mail sent");
            break;
        case MFMailComposeResultFailed:
            NSLog(@"Mail sent failure: %@", [error localizedDescription]);
            break;
        default:
            break;
    }
    
    // Close the Mail Interface
    [self dismissViewControllerAnimated:YES completion:nil];
    [self.view removeFromSuperview];
    self.view.hidden = YES;
     mc.delegate = nil;
    [mc release];
    mc = nil;
}

- (void)dealloc {

    [super dealloc];
}


@end
