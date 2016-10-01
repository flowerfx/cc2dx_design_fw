#import <UIKit/UIKit.h>

@interface KeyboardView : NSObject<UITextViewDelegate>
{
  UIViewController* rootViewController;
  UITextView* keyboardTextView;
  bool keyboardMultiLine;
  int keyboardMaxLength;
}

- (id) initWithViewController : (UIViewController*) viewController;

- (void) openKeyboard:(const char*)initialValue withMultiLine:(BOOL)isMultiLine withNumber:(BOOL)isNumber withMaxLength:(int)iMaxLength;
- (void) closeKeyboard;
- (void) KeyboardText:(const char*)cValue;

@end

