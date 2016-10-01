//
//  Header.h
//  CrystalNumberful
//
//  Created by qchien on 12/3/15.
//
//

#ifndef __IAP_HELPER_H__
#define __IAP_HELPER_H__

#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

#include <vector>
#include <map>
#include "RKString_Code/RKString.h"
using namespace RKUtils;

#define kInAppPurchaseManagerProductsFetchedNotification @"kInAppPurchaseManagerProductsFetchedNotification"

#define kInAppPurchaseManagerTransactionFailedNotification @"kInAppPurchaseManagerTransactionFailedNotification"
#define kInAppPurchaseManagerTransactionSucceededNotification @"kInAppPurchaseManagerTransactionSucceededNotification"


@interface IAPHelper : UIViewController<SKPaymentTransactionObserver, SKProductsRequestDelegate>
{
    SKProduct * _product;
    SKProductsRequest * _productsRequest;
    
    int iap_state_get_list_item;
    int iap_state_purchase_item;
    int number_item_load_iap;
    
    int p_IsloadRestorePurchaseItemComplete;
    
    std::vector<RKString> p_listProductIdRestore;
    std::map<RKString,RKString> p_list_price_format_currency;
    
}
@property (assign, nonatomic) std::map<RKString,RKString> p_list_price_format_currency;
@property (assign, nonatomic) std::vector<RKString> p_listProductIdRestore;
@property (strong, nonatomic) SKProduct * _product;

+ (IAPHelper*)defaultHelper;

- (id)init;
- (void)purchaseItem:(NSString*)id_item;
- (void)restoreCompleteTransaction;

// public methods
- (void)loadProductsInfo : (NSArray *)id_key_iap;
- (void)loadStore:(NSArray*)list_item_store;
- (BOOL)canMakePurchases;

//get set state load iap
- (int) GetStateLoadListItem;
- (void)SetStateLoadListItem:(int)state;

- (int) GetStatePurchaseItem;
- (void)SetStatePurchaseItem:(int)state;

//get product id complete;
- (void) AddItemToListRestorePurchase : (NSString *) id_item;
- (std::vector<RKString>) GetListProductIDComplete;
- (int)  IsLoadRestorePurchaseItemComplete;
- (void) ResetLoadRestorePurchase;

- (RKString) GetFormatPriceWithID: (RKString) id_item;
@end



#endif //__IAP_HELPER_H__
