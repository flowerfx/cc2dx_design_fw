//
//  Header.h
//  CrystalNumberful
//
//  Created by qchien on 12/3/15.
//
//

#ifndef __IAP_WRAPPER_H__
#define __IAP_WRAPPER_H__

#include "DisplayView.h"
#include "cocos2d.h"
#include "RKString_Code/RKString.h"
#include "IAP/IAPManager.h"

#include <vector>
USING_NS_CC;
using namespace RKUtils;

namespace Utility
{
    namespace IAP
    {
#if USE_IAP
        void IOS_LoadTheStore();
        
        void IOS_PurchaseItem(RKString iap_key);
        
        STATE_IAP_LOAD GetStateLoadListItem();
        
        STATE_IAP_LOAD GetStatePurchaseItem();
        
        void SetStateLoadListItem(STATE_IAP_LOAD state);
        
        void SetStatePurchaseItem(STATE_IAP_LOAD state);
        
        void RestorePurchase();
        
        int IsRestorePurchaseComplete();
        
        void ResetRestorePurchaseComplete();
        
        std::vector<RKString> GetListIAPPurchaseRestore();
        
        RKString GetPriceWithIDItem(RKString id_item);
#endif
    }
}



#endif //__IAP_WRAPPER_H__
