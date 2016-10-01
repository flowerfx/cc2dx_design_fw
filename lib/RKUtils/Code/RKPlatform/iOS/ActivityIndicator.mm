#import "ActivityIndicator.h"

@implementation ActivityIndicator

- (id) initWithViewController : (UIViewController*) viewController
{
  self = [super init];
  if (self)
  {
    rootViewController = viewController;
  }
  return self;
}

- (void)dealloc
{
  [self hide];
  
  [super dealloc];
}

- (void) show
{
  if(indicatorView == nil)
  {
    CGRect rect = CGRectMake(0, 0,
                             [UIScreen mainScreen].bounds.size.height,
                             [UIScreen mainScreen].bounds.size.width);
    indicatorView = [[UIView alloc] initWithFrame:rect];
    indicatorView.backgroundColor = [[UIColor grayColor] colorWithAlphaComponent:0.6f];
    [rootViewController.view addSubview:indicatorView];
  }
  
  if (indicator == nil)
  {
    indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    CGRect position = indicator.frame;
    position.origin.x = (indicatorView.frame.size.width - indicator.frame.size.width) / 2;
    position.origin.y = (indicatorView.frame.size.height - indicator.frame.size.height) / 2;
    indicator.frame = position;
    [indicatorView addSubview:indicator];
  }
  
  [indicator startAnimating];
}

- (void) hide
{
  [indicator removeFromSuperview];
  [indicator release], indicator = nil;
  
  [indicatorView removeFromSuperview];
  [indicatorView release], indicatorView = nil;
}

@end