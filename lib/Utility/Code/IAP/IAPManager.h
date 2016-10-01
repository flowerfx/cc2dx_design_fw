#ifndef _IAP_MANAGER_H_
#define _IAP_MANAGER_H_
#pragma once
#include "../LoadDataXML.h"
#include "../DisplayView.h"
#if USE_IAP
#include "../../../../LIB/RKUtils/Code/RKThread_Code/RKThread.h"
#include "../../../../LIB/RKUtils/Code/RKThread_Code/RKInterlock.h"
using namespace RKUtils;
namespace Utility
{
	namespace IAP
	{
		enum STATE_IAP_LOAD
		{
			IAP_FAIL = -1,
			IAP_NONE,
			IAP_PROCESSING,
			IAP_SUCCEED
		};
		enum IAP_TYPE
		{
			IAP_CONSUMABLE = 0,
			IAP_DURABLE
		};
		struct IAPItem
		{
			RKString p_name;
			RKString p_key;
			IAP_TYPE p_type;
			RKString p_des;
			RKString p_cost;
			RKString p_DisplayName;
			int p_GainValue;
		public:
			IAPItem() {
				p_name = "";
				p_key = "";
				p_cost = "";
				p_GainValue = 0;
				p_DisplayName = "";
				p_des = "";
				p_type = IAP_CONSUMABLE;
			}
		};


		class IAPManager
		{
		private:
			static IAPManager * m_Instance;
			std::map<RKString, IAPItem*> p_listIAPItem;
			
			bool p_IsRemoveAds;
			bool p_IsGetAllPriceValue;
            bool p_IsGetAllItemProcess;

            
            RKUtils::RKThread * p_CheckRestorePurchase;
            RKUtils::RKInterlock*         m_exit;
            RKUtils::RKCriticalSection*   m_criticalSection;
            RKUtils::RKThreadCondition*   m_condition;
            bool p_IsOnRestoringPurchase;
            bool p_IsGetPurchaseComplete;
            
            
            RKString m_lastTransactionID;
           
            static uint32 checkRestorePurchase_thread(void * data);
            
            void OnCheckRestorePurchaseComplete();
            
            void DestroyAllThread();
            
            STATE_IAP_LOAD p_state_restore_purchase;
            
            RKString id_pack_remove_ads;
		public:
			static IAPManager * GetInstance()
			{
				if (!m_Instance)
				{
					m_Instance = new IAPManager();
				}
				return m_Instance;
			}

			STATE_IAP_LOAD StateGetAllItemProcess();
			STATE_IAP_LOAD StateGetPurchaseItemProcess();
            STATE_IAP_LOAD StateGetRestorePurchase();
            
			void ClearStateLoadItemProcess();
			void ClearStatePurchaseItemProcess();
            void ClearStateRestorePurchase();

			IAPManager();
			~IAPManager();

			void OnInitialize();
			bool OnGetInventoryStore();
			void OnPurchaseItem(RKString idx);
			void DoFulfillmentItem(RKString key);

			void CheckItemFulfillmentAtInit();

			//result
			void OnpurchaseResult(STATE_IAP_LOAD result, RKString name_pack);

			void InitValue();
			void LoadXMLData();

			IAPItem* GetItem(RKString nameItem);
            std::vector<RKString> GetListKeyIAP();
			//
			void ShowAds();
            bool ShowVideoAds();
            int  GetStateVideoAds();
            void ResetStateVideoAds();
            void InitAds();
            int  RestorePurchase();
			//
			bool IsRemoveAds() { return p_IsRemoveAds; }
			void SetRemoveAds(bool val) { p_IsRemoveAds = val; }
        
            void WakeUpThreadCheckPurchase();
            
            void Update(float dt);
            
            RKString GetPriceFormatWithID(RKString id);
            
            void SetIdRemoveAds(RKString id) {id_pack_remove_ads = id;}
            
            RKString GetLastTransactionID();
            void SetLastTransactionID(RKString transactionID);
            
		protected:
			void OnParseData(TiXmlDocument * xmldata);

		};
	}
}
#define GetIAPManager() IAP::IAPManager::GetInstance()

#endif //USE_IAP
#endif //_IAP_MANAGER_H_