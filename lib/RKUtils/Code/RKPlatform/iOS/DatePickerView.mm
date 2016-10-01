//
//  DatePickerView.m
//  CasualCore
//
//  Created by Andrew McArthur on 17/12/13.
//
//

#import "DatePickerView.h"

#include "Game.h"
#include "DateTime.h"

@implementation DatePickerView

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
    [self closeDatePicker];
    
    [super dealloc];
}

- (void) openDatePicker:(int)mode
{
    if (datePicker == nil)
    {
        CGRect rootViewFrame = rootViewController.view.frame;
        
        // the root view frame is inverted. use the inverse of the root view for the date picker.
        CGRect rDatePickerFrame = CGRectMake(0, rootViewFrame.size.width, rootViewFrame.size.height, rootViewFrame.size.width * 0.5f);
        
        // create
        datePicker = [[UIDatePicker alloc] initWithFrame:rDatePickerFrame];
        [datePicker setDate:[NSDate date] animated:YES];
        [datePicker setMinimumDate:[NSDate distantPast]];
        [datePicker setMaximumDate:[NSDate date]];
        [datePicker setDatePickerMode:mode];
        datePicker.backgroundColor = [UIColor whiteColor];
        [datePicker addTarget:self action:@selector(onDateChanged:) forControlEvents:UIControlEventValueChanged];
        
        // anchor the datepicker to the bottom of the screen.
        CGRect rFrame = datePicker.frame;
        rFrame.origin.y = rootViewFrame.size.width - rFrame.size.height;
        datePicker.frame = rFrame;
        
        // add done button.
        int iToolbarHeight = 30;
        toolbar = [[UIToolbar alloc] initWithFrame:CGRectMake(0, rFrame.origin.y - iToolbarHeight, rootViewFrame.size.height, iToolbarHeight)];
        toolbar.barStyle = UIBarStyleBlackOpaque;
        toolbar.backgroundColor = [UIColor blackColor];
        
        btnDoneLocalizsed = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(doneButtonPressed:)];
        
        toolbar.items = [NSArray arrayWithObject:btnDoneLocalizsed];
        
        [rootViewController.view addSubview:toolbar];
        
        // add to main view.
        [rootViewController.view addSubview:datePicker];
        [datePicker becomeFirstResponder];
    }
}

- (void) doneButtonPressed:(id)sender
{
    [self closeDatePicker];
}

- (void) closeDatePicker
{
    if (datePicker)
    {
        [btnDoneLocalizsed release];
        btnDoneLocalizsed = nil;
        
        [toolbar removeFromSuperview];
        [toolbar release];
        toolbar = nil;
        
        [datePicker resignFirstResponder];
        [datePicker removeFromSuperview];
        [datePicker release];
        datePicker = nil;
    }
}

// called when the value in datepicker has changed.
- (void) onDateChanged:(id)sender
{
    NSDate* currentDate = datePicker.date;
    [datePicker setDate:currentDate animated:YES];
    
    NSDateComponents* components = [[NSCalendar currentCalendar] components:NSCalendarUnitDay | NSCalendarUnitMonth | NSCalendarUnitYear fromDate:currentDate];
    
    CasualCore::DateTime date;
    date.day = [components day];
    date.month = [components month];
    date.year = [components year];
    date.hour = [components hour];
    date.minute = [components minute];
    date.second = [components second];
    
    CasualCore::Game::GetInstance()->GetPlatform()->OnDatePickerEvent(0, date);
}

@end
