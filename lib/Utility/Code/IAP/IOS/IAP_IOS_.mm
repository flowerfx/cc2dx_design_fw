//
//  IAP_IOS_.m
//  CrystalNumberful
//
//  Created by qchien on 12/3/15.
//
//

#import <Foundation/Foundation.h>
#import "IAP_Helper.h"
#include "../IAPManager.h"

//std::vector<RKString> p_listProductIdRestore;

@implementation IAPHelper

@synthesize p_listProductIdRestore;
@synthesize p_list_price_format_currency;
@synthesize _product;


static IAPHelper *_sharedHelper = nil;

+ (IAPHelper*)defaultHelper {
    // dispatch_once will ensure that the method is only called once (thread-safe)
    static dispatch_once_t pred = 0;
    dispatch_once(&pred, ^{
        _sharedHelper = [[IAPHelper alloc] init];
    });
    return _sharedHelper;
}

-(id)init
{
    if(self = [super init])
    {
        
    }
    
   // self.iap_state_get_list_item = Utility::IAP::STATE_IAP_LOAD::IAP_NONE;
    
    iap_state_get_list_item = 0 ; // no load idle
    iap_state_purchase_item = 0 ;
    number_item_load_iap = 0;

    p_IsloadRestorePurchaseItemComplete = -1;
    p_listProductIdRestore.clear();
    p_list_price_format_currency.clear();
    return self;
}

- (void) loadProductsInfo : (NSArray *)list_id_key_iap
{
    if ([self canMakePurchases])
    {
        SKProductsRequest *request = [[SKProductsRequest alloc]initWithProductIdentifiers: [NSSet setWithArray:list_id_key_iap]];
    
        [request setDelegate: self];
        [request start];
    }
    else
    {
        NSLog(@"[ERROR!] : enable in app purchase in setting");
    }
}

-(void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    
    NSArray *products = response.products;
    
    
    if ([products count] != number_item_load_iap)
    {
        iap_state_get_list_item = -1 ;// fail
    }
    else
    {
        bool load_all_succeed = true;
        for (int i = 0 ; i< [products count]; i++)
        {
            _product = [[products objectAtIndex:i] retain];
            if (_product)
            {
                NSLog(@"Product title: %@" , _product.localizedTitle);
                NSLog(@"Product description: %@" , _product.localizedDescription);
                NSLog(@"Product price: %@" , _product.price);
                NSLog(@"Product id: %@" , _product.productIdentifier);
                
                NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
                [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
                [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
                [numberFormatter setLocale:_product.priceLocale];
                
                NSString *formattedPrice = [numberFormatter stringFromNumber:_product.price];
                
                p_list_price_format_currency.insert(std::pair<RKString,RKString>(
                                                    RKString([_product.productIdentifier UTF8String]),
                                                    RKString([formattedPrice UTF8String] )
                                                    ));
                
                NSLog(@"format price :%@", formattedPrice);
            }
        }
        
        for (NSString *invalidProductId in response.invalidProductIdentifiers)
        {
            NSLog(@"Invalid product id: %@" , invalidProductId);
            //load_all_succeed = false;
        }
        
        if(load_all_succeed)
        {
            iap_state_get_list_item = 2; //succeedful
        }
        else
        {
            iap_state_get_list_item = -1; //fail
        }
    }
    
    // finally release the reqest we alloc/init’ed in requestProUpgradeProductData
    [_productsRequest release];
    
    //[[NSNotificationCenter defaultCenter] postNotificationName:kInAppPurchaseManagerProductsFetchedNotification object:self userInfo:nil];
}

- (void)loadStore : (NSArray*)list_item_store
{
    // restarts any purchases if they were interrupted last time the app was open
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    //
    number_item_load_iap = (int)[list_item_store count];
    
    // get the product description (defined in early sections)
    [self loadProductsInfo:list_item_store];
}

-(void)purchaseItem:(NSString *)id_item
{
    SKPayment *payment = [SKPayment paymentWithProductIdentifier:id_item];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

- (void)restoreCompleteTransaction
{
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (BOOL)canMakePurchases
{
    return [SKPaymentQueue canMakePayments];
}
//
// saves a record of the transaction by storing the receipt to disk
//
- (void)recordTransaction:(SKPaymentTransaction *)transaction
{
    //if ([transaction.payment.productIdentifier isEqualToString:kInAppPurchaseProUpgradeProductId])
    {
        // save the transaction receipt to disk
        //[[NSUserDefaults standardUserDefaults] setValue:transaction.transactionReceipt forKey:@"proUpgradeTransactionReceipt" ];
//        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

//
// removes the transaction from the queue and posts a notification with the transaction result
//
- (void)finishTransaction:(SKPaymentTransaction *)transaction wasSuccessful:(BOOL)wasSuccessful
{
    // remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    
    NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:transaction, @"transaction" , nil];
    if (wasSuccessful)
    {
//        transaction.transactionReceipt
//        Utility::IAP::IAPManager::GetInstance()->SetLastTransactionID()
        std::string version = Application::getInstance()->getVersion();
        version.shrink_to_fit();
        if(atoi(version.substr(0, 1).c_str()) > 7)
        {
            NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
            NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
            if (receipt)
            {
                NSString *base64Encoded = [receipt base64EncodedStringWithOptions:0];
                Utility::IAP::IAPManager::GetInstance()->SetLastTransactionID([base64Encoded UTF8String]);

            }
        }
        else
        {
            NSData *receipt = [transaction transactionReceipt];
            NSString *base64Encoded = [receipt base64EncodedStringWithOptions:0];
            Utility::IAP::IAPManager::GetInstance()->SetLastTransactionID([base64Encoded UTF8String]);
        }
        
        iap_state_purchase_item = 2; //Succeedful transaction
    }
    else
    {        
        iap_state_purchase_item = -1; //fail transaction
    }
}
//
// called when the transaction was successful
//
- (void)completeTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction];
    [self finishTransaction:transaction wasSuccessful:YES];
}
//
// called when a transaction has been restored and and successfully completed
//
- (void)restoreTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction.originalTransaction];
    [self finishTransaction:transaction wasSuccessful:YES];
}
//
// called when a transaction has failed
//
- (void)failedTransaction:(SKPaymentTransaction *)transaction
{
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
        // error!
        [self finishTransaction:transaction wasSuccessful:NO];
    }
    else
    {
        // this is fine, the user just cancelled, so don’t notify
        [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        iap_state_purchase_item = -1; //fail transaction
    }
}
//
// called when the transaction status is updated
//
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
                break;
            default:
                break;
        }
    }
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    NSLog(@"%@",[error description]);
    p_IsloadRestorePurchaseItemComplete = 1;
    
//    Utility::IAP::IAPManager::GetInstance()->WakeUpThreadCheckPurchase();
}

//Then this delegate Function Will be fired
- (void) paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    //NSMutableArray * purchasedItemIDs = [[NSMutableArray alloc] init];
    
    NSLog(@"received restored transactions: %lu", (unsigned long)queue.transactions.count);
    for (SKPaymentTransaction *transaction in queue.transactions)
    {
        NSLog(@"%@",transaction.payment.productIdentifier);
        [self AddItemToListRestorePurchase : transaction.payment.productIdentifier ];
    }
    p_IsloadRestorePurchaseItemComplete = 0;
    
//    Utility::IAP::IAPManager::GetInstance()->WakeUpThreadCheckPurchase();
}

- (int) GetStateLoadListItem
{
    return iap_state_get_list_item  ;
}

- (void)SetStateLoadListItem:(int)state
{
    iap_state_get_list_item = state;
}

- (int) GetStatePurchaseItem
{
    return iap_state_purchase_item;
}

- (void)SetStatePurchaseItem:(int)state
{
    iap_state_purchase_item = state;
}

- (std::vector<RKString>) GetListProductIDComplete
{
    return p_listProductIdRestore;
}

- (int) IsLoadRestorePurchaseItemComplete
{
    return p_IsloadRestorePurchaseItemComplete ;
}

- (void) ResetLoadRestorePurchase
{
    p_IsloadRestorePurchaseItemComplete = -1;
}

- (void) AddItemToListRestorePurchase : (NSString *) id_item
{
    RKString item = [id_item UTF8String];
    p_listProductIdRestore.insert(p_listProductIdRestore.begin(), item);
}

- (RKString) GetFormatPriceWithID: (RKString) id_item
{
    if(p_list_price_format_currency.size() <= 0 || p_list_price_format_currency.find(id_item) == p_list_price_format_currency.end())
        return "";
    return p_list_price_format_currency.at(id_item);
}
@end