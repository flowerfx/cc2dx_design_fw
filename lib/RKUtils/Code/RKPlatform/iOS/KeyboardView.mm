#import "KeyboardView.h"
//#include "Game.h"

@implementation KeyboardView

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
  [self closeKeyboard];
  
  [super dealloc];
}

- (void) openKeyboard:(const char *)initialValue withMultiLine:(BOOL)isMultiLine withNumber:(BOOL)isNumber withMaxLength:(int)iMaxLength
{
  // create a hidden text view
  CGRect aRect = CGRectMake(0, 0, 0, 0);
  
  keyboardMultiLine = isMultiLine;
  keyboardMaxLength = iMaxLength;
  if (keyboardTextView == nil)
  {
    keyboardTextView = [[UITextView alloc] initWithFrame:aRect];
    keyboardTextView.delegate = self;
    [rootViewController.view addSubview:keyboardTextView];
  }
  
  if(isNumber)
  {
   [keyboardTextView setKeyboardType:UIKeyboardTypeNumberPad];
  }
  else
  {
    [keyboardTextView setKeyboardType:UIKeyboardTypeDefault];
  }
  
  [keyboardTextView setText:[NSString stringWithUTF8String:initialValue]];
  
  // opens the keyboard
  [keyboardTextView becomeFirstResponder];
}

- (void)closeKeyboard
{
  // close the keyboard
  if (keyboardTextView)
  {
    [keyboardTextView resignFirstResponder];
    [keyboardTextView removeFromSuperview];
    keyboardTextView.delegate = nil;
    [keyboardTextView release];
    keyboardTextView = nil;
  }
}

- (void) KeyboardText:(const char*)cValue
{
  // set the keyboard test view
  if (keyboardTextView)
  {
    [keyboardTextView setText:[NSString stringWithUTF8String:cValue]];
  }
}

// keyboard text input handling
-(BOOL) textView:(UITextView*)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
  if (!keyboardMultiLine)
  {
    if (strcmp([text UTF8String],"\n")==0)
    {
      // block enter and return event
      CasualCore::Game::GetInstance()->GetPlatform()->OnKeyboardEvent(CasualCore::KEYBOARD_RETURN, [textView.text UTF8String]);
      return false;
    }
    else if(keyboardMaxLength >-1)
    {
      NSUInteger newLength = (textView.text.length - range.length) + text.length;
      if(newLength > keyboardMaxLength)
      {
        return false;
      }
    }
  }
  else
  {
    if(keyboardMaxLength >-1)
    {
      NSUInteger newLength = (textView.text.length - range.length) + text.length;
      if(newLength > keyboardMaxLength)
      {
        return false;
      }
    }
  }
  // perform text change
  return true;
}

-(void) textViewDidBeginEditing:(UITextView *)textView
{
  CasualCore::Game::GetInstance()->GetPlatform()->OnKeyboardEvent(CasualCore::KEYBOARD_OPEN, [textView.text UTF8String]);
}

-(void) textViewDidEndEditing:(UITextView *)textView
{
  CasualCore::Game::GetInstance()->GetPlatform()->OnKeyboardEvent(CasualCore::KEYBOARD_CLOSED, [textView.text UTF8String]);
}

-(void) textViewDidChange:(UITextView *)textView
{
  // ANLA: crash bug fix 6533972, 6496362
  if (textView.markedTextRange == nil)
    CasualCore::Game::GetInstance()->GetPlatform()->OnKeyboardEvent(CasualCore::KEYBOARD_CHANGED, [textView.text UTF8String]);
}

@end