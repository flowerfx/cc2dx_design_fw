//
//  IAP_IOS_.m
//  CrystalNumberful
//
//  Created by qchien on 12/3/15.
//
//

#import "IAP_IOS_Wrapper.h"
#import "IAP_Helper.h"
namespace Utility
{
    namespace IAP
    {
        void IOS_LoadTheStore()
        {
            std::vector<RKString> list_key_iap = GetIAPManager()->GetListKeyIAP();
            
            NSMutableArray * list_ = [[NSMutableArray alloc]init];
            for(int i = 0 ; i < list_key_iap.size() ; i ++)
            {
                NSString * str_id = [NSString stringWithUTF8String:list_key_iap.at(i).GetString()];
                [list_ addObject:str_id];
            }
            
            [[IAPHelper defaultHelper]loadStore : list_];
            
            [[IAPHelper defaultHelper]SetStateLoadListItem:(int)STATE_IAP_LOAD::IAP_PROCESSING];
        }
        
        void IOS_PurchaseItem(RKString iap_key)
        {
            bool iap_key_contain = false;
            std::vector<RKString> list_key_iap = GetIAPManager()->GetListKeyIAP();
            for(int i = 0 ; i < list_key_iap.size() ; i ++)
            {
                if(iap_key == list_key_iap.at(i))
                {
                    iap_key_contain = true;
                    break;
                }
            }
            
            if(iap_key_contain)
            {
                 NSString * str_id = [NSString stringWithUTF8String:iap_key.GetString()];
                [[IAPHelper defaultHelper]purchaseItem:str_id];
                
                [[IAPHelper defaultHelper]SetStatePurchaseItem:(int)STATE_IAP_LOAD::IAP_PROCESSING];
            }
        }
        
        STATE_IAP_LOAD GetStateLoadListItem()
        {
            return (STATE_IAP_LOAD)[[IAPHelper defaultHelper]GetStateLoadListItem];
        }
        
        STATE_IAP_LOAD GetStatePurchaseItem()
        {
            return (STATE_IAP_LOAD)[[IAPHelper defaultHelper]GetStatePurchaseItem];
        }
        
        void SetStateLoadListItem(STATE_IAP_LOAD state)
        {
            [[IAPHelper defaultHelper]SetStateLoadListItem:(int)state];
        }
        
        void SetStatePurchaseItem(STATE_IAP_LOAD state)
        {
           [[IAPHelper defaultHelper]SetStatePurchaseItem:(int)state];
        }
        
        void RestorePurchase()
        {
            [[IAPHelper defaultHelper]restoreCompleteTransaction];
        }
        
        int IsRestorePurchaseComplete()
        {
            return [[IAPHelper defaultHelper]IsLoadRestorePurchaseItemComplete];
        }
        
        void ResetRestorePurchaseComplete()
        {
            [[IAPHelper defaultHelper]ResetLoadRestorePurchase];
        }
        
        std::vector<RKString> GetListIAPPurchaseRestore()
        {
            return [[IAPHelper defaultHelper]GetListProductIDComplete];
        }
        
        RKString GetPriceWithIDItem(RKString id_item)
        {
            return [[IAPHelper defaultHelper]GetFormatPriceWithID:id_item];
        }
    }
}