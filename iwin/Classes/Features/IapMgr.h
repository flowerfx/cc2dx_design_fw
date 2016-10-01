#ifndef __IAP_MGR_H__
#define __IAP_MGR_H__
#include "cocos2d.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "Java_org_cocos2dx_cpp_IwinIapHelper.h"
#endif

class IapMgr
{
private:
	static IapMgr* s_instance;
public:
	static IapMgr* getInstance();

	void init();

	void onPurchaseSuccess(std::string store_name, std::string transaction_data, std::string transaction_data_sign);
	void onPurchaseFailed(std::string message);
	void purchase(std::string sku, std::string payload);
    
    void checkFinishLoad(float dt);
    void checkFinishPurchase(float dt);

};

#define GetIapMgr() IapMgr::getInstance()

#endif