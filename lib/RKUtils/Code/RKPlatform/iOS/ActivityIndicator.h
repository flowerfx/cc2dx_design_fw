#import "UIKit/UIKit.h"

@interface ActivityIndicator : NSObject
{
  UIViewController*        rootViewController;
  UIView*                  indicatorView;
  UIActivityIndicatorView* indicator;
}

- (id) initWithViewController : (UIViewController*) viewController;
- (void) show;
- (void) hide;

@end