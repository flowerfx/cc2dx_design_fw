//
//  DatePickerView.h
//  CasualCore
//
//  Created by Andrew McArthur on 17/12/13.
//
//

#import <UIKit/UIKit.h>

@interface DatePickerView : NSObject//<UIPickerViewDelegate>
{
    UIViewController* rootViewController;
    UIDatePicker* datePicker;
    
    UIToolbar* toolbar;
    UIBarButtonItem* btnDoneLocalizsed;
}

- (id) initWithViewController : (UIViewController*) viewController;

- (void) openDatePicker:(int)mode;
- (void) closeDatePicker;

@end
