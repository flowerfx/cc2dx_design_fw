#include "IAPManager.h"
#if USE_IAP
#include "../FileManager.h"
#include "..//DisplayView.h"
//#include "Menu/IAPMenu.h"
#include "../FXFlatform/XFlatform.h"

#ifdef GAME_IOS
#include "IOS/IAP_IOS_Wrapper.h"
#endif

namespace Utility
{
	namespace IAP
	{
		IAPManager * IAPManager::m_Instance = NULL;


		STATE_IAP_LOAD IAPManager::StateGetPurchaseItemProcess()
		{
			STATE_IAP_LOAD state = XFlatform::IAP_GetStatePurchaseItem();
			return state;
		}
		STATE_IAP_LOAD IAPManager::StateGetAllItemProcess()
		{
			STATE_IAP_LOAD state = XFlatform::IAP_GetStateLoadItem();
			if (state == STATE_IAP_LOAD::IAP_SUCCEED && !p_IsGetAllPriceValue)
			{
				std::wstring w_str = XFlatform::GetPriceValue();
				p_IsGetAllPriceValue = true;
			}
			return state;
		}
        
        STATE_IAP_LOAD IAPManager::StateGetRestorePurchase()
        {
            return p_state_restore_purchase;
        }

		void IAPManager::ClearStateLoadItemProcess()
		{
			XFlatform::IAP_SetStateLoadItem(STATE_IAP_LOAD::IAP_NONE);
		}

		void IAPManager::ClearStatePurchaseItemProcess()
		{
			XFlatform::IAP_SetStatePurchaseItem(STATE_IAP_LOAD::IAP_NONE);
		}
        
        void IAPManager::ClearStateRestorePurchase()
        {
            p_state_restore_purchase = STATE_IAP_LOAD::IAP_NONE;
        }

        IAPManager::IAPManager() :
            p_CheckRestorePurchase(0),
            m_exit(0),
            m_criticalSection(0),
            m_condition(0)
		{
			p_IsRemoveAds = false;
			p_IsGetAllPriceValue = false;
            
            p_IsGetAllItemProcess = false;
            
            p_IsOnRestoringPurchase = false;
            
            p_IsGetPurchaseComplete = false;
            
            p_state_restore_purchase = STATE_IAP_LOAD::IAP_NONE;
            
            id_pack_remove_ads = "";
            
		}
		IAPManager::~IAPManager()
		{
			p_listIAPItem.clear();
            
            DestroyAllThread();
            
		}
		/////////////////////////////////////////
		void IAPManager::OnInitialize()
		{
			//
			InitValue();
        }

		bool IAPManager::OnGetInventoryStore()
		{
			
            if (p_IsGetAllItemProcess) {
                return true;
            }
			XFlatform::IAP_RequestListItem();
            p_IsGetAllItemProcess = true;
			return true;
		}

		void IAPManager::OnPurchaseItem(RKString idx)
		{
			RKString id = p_listIAPItem.at(idx)->p_key;
			XFlatform::IAP_PurchaseItemWithID(id.GetString());
		}

		void IAPManager::DoFulfillmentItem(RKString key)
		{
			XFlatform::IAP_DoFulfillmentItemId(key.GetString());
		}

		void IAPManager::OnpurchaseResult(STATE_IAP_LOAD result, RKString name_pack)
		{
//			if (result == STATE_IAP_LOAD::IAP_SUCCEED && name_pack != "")
//			{
//				//
//				auto GetSave = File::SaveMgr;
//				if (p_listIAPItem.at(name_pack)->p_type == IAP_TYPE::IAP_DURABLE) // is bought remove ads
//				{
//					p_IsRemoveAds = true;
//					XFlatform::Ads_HideBanner();
//					GetSave->GetDataSave()->SetCharAtName(SAVE_STR_REMOVE_ADS, "1");
//				}
//				else
//				{
//					int cur_coin = GetSave->GetDataSave()->GetDataByName(SAVE_STR_COIN);
//					cur_coin = cur_coin + p_listIAPItem.at(name_pack)->p_GainValue;
//					GetSave->GetDataSave()->SetDataAtName(SAVE_STR_COIN, cur_coin);
//				}
//				GetSave->SaveTheGameSave();
				//
				//
//				DoFulfillmentItem(p_listIAPItem.at(name_pack)->p_key);
//				//
//				//clear state purchase iap
//				ClearStatePurchaseItemProcess();
//			}
//			else if (result == STATE_IAP_LOAD::IAP_FAIL)
//			{
//				//
//				//clear state purchase iap
//				ClearStatePurchaseItemProcess();
//			}
		}
		///////////////////////////////////////////
		void IAPManager::InitValue()
		{
			LoadXMLData();
            OnGetInventoryStore();
		}
		void IAPManager::LoadXMLData()
		{
            
//            p_listIAPItem
            auto item = new IAPItem();
            item->p_key = "vn.mecorp.iwin.1";
            p_listIAPItem.insert(std::pair<RKString, IAPItem*>(item->p_key, item));
            item = new IAPItem();
            item->p_key = "vn.mecorp.iwin.5";
            p_listIAPItem.insert(std::pair<RKString, IAPItem*>(item->p_key, item));
            item = new IAPItem();
            item->p_key = "vn.mecorp.iwin.10";
            p_listIAPItem.insert(std::pair<RKString, IAPItem*>(item->p_key, item));
            
//			TiXmlDocument * p_ItemXML = new TiXmlDocument();
//			bool IsUseArk = false;
//#if USE_ARK_ARCHIVE
//			IsUseArk = true;
//			if (IsUseArk)
//			{
//				tinyxml2::XMLError error = p_ItemXML->LoadFileArk(std::string(XML_DIR) + std::string(ITEM_XML));
//				if (error == tinyxml2::XMLError::XML_NO_ERROR)
//				{
//					OnParseData(p_ItemXML);
//				}
//				else
//				{
//					IsUseArk = false;
//				}
//			}
//#endif
//			if (!IsUseArk)
//			{
//				FILE * f = File::GetFile((std::string(XML_DIR) + std::string(ITEM_XML)).c_str(), "rb");
//				if (f)
//				{
//					p_ItemXML->LoadFile(f);
//					OnParseData(p_ItemXML);
//
//					fclose(f);
//				}
//			}
//
//			delete p_ItemXML;
//			p_ItemXML = NULL;
			
		}


		IAPItem* IAPManager::GetItem(RKString nameItem)
		{
			if (p_listIAPItem.size() > 0 && p_listIAPItem.find(nameItem) != p_listIAPItem.end())
			{
				return p_listIAPItem.at(nameItem);
			}
			return NULL;
		}

		void IAPManager::OnParseData(TiXmlDocument * p_ItemXML)
		{
			auto root = p_ItemXML->FirstChildElement("GAME");
			if (!root)
			{
				CCASSERT(false, "root data not found !");
				return;
			}
			auto ListIAPItem = root->FirstChildElement("IAPItem");
			if (ListIAPItem)
			{
				auto detail = ListIAPItem->FirstChildElement("Item");
				do
				{
					if (detail)
					{
						auto item = new IAPItem();
						item->p_name = detail->Attribute("name");

						auto _child_element = detail->FirstChildElement("key");
						if (_child_element)
						{
							item->p_key = _child_element->Attribute("value");
						}
						
						_child_element = detail->FirstChildElement("displayName");
						if (_child_element)
						{
							item->p_DisplayName = _child_element->Attribute("value");
						}
						
						_child_element = detail->FirstChildElement("cost");
						if (_child_element)
						{
							item->p_cost = _child_element->Attribute("value");
						}

						_child_element = detail->FirstChildElement("type");
						if (_child_element)
						{
							item->p_type = (IAP_TYPE)atoi(_child_element->Attribute("value"));
                            
                            if(item->p_type == IAP_TYPE::IAP_DURABLE)
                            {
                                id_pack_remove_ads = item->p_key;
                            }
						}

						_child_element = detail->FirstChildElement("value");
						if (_child_element)
						{
							if (item->p_type == IAP_TYPE::IAP_CONSUMABLE)
							{
								item->p_GainValue = atoi(_child_element->Attribute("value"));
								item->p_des = _child_element->Attribute("value");
							}
							else
							{
								item->p_GainValue = 1;
								item->p_des = _child_element->Attribute("value");
							}
						}			
	
						p_listIAPItem.insert(std::pair<RKString, IAPItem*>(item->p_name,item));

						detail = detail->NextSiblingElement();
					}

				} while (detail);
			}
		}
        
		void IAPManager::CheckItemFulfillmentAtInit()
		{
					}

		void IAPManager::ShowAds()
		{
			if (!p_IsRemoveAds)
			{
				XFlatform::Ads_ShowBanner();
			}
		}
        
        bool IAPManager::ShowVideoAds()
        {
            return XFlatform::Ads_ShowVideo();
        }
        
        void IAPManager::InitAds()
        {
            if (!p_IsRemoveAds)
            {
                XFlatform::Ads_InitBanner();
            }
            
            XFlatform::Ads_InitVideo();
        }
        
        int  IAPManager::GetStateVideoAds()
        {
            return XFlatform::Ads_GetVideoState();
        }
        void IAPManager::ResetStateVideoAds()
        {
            XFlatform::Ads_ResetVideoState();
        }
    
        std::vector<RKString> IAPManager::GetListKeyIAP()
        {
            std::vector<RKString> list_key;
            
            for(auto it = p_listIAPItem.begin(); it != p_listIAPItem.end();it++)
            {
                RKString key_iap = it->second->p_key;
                list_key.push_back(key_iap);
            }
            
            return list_key;
        }
        
        int IAPManager::RestorePurchase()
        {
            if(p_IsOnRestoringPurchase)
                return -1;
            
            if(p_IsRemoveAds)
                return 1;
            
            p_IsGetPurchaseComplete = false;
            
            XFlatform::IAP_CheckFulfillmentIfNotFulfill("");
            
            if (!m_criticalSection)
            {
                m_criticalSection = RKCriticalSection_Create("cs_update_res_pur");
            }
            
            if (!m_condition)
            {
                m_condition = RKThreadCondition_Create("Update_res_pur_Cond");
            }
            
            if (!m_exit)
            {
                m_exit = RKInterlock_Create("Update_res_pur_Exit");
            }
            RKInterlock_Reset(m_exit);
            
            p_IsOnRestoringPurchase = true;
            
            if(!p_CheckRestorePurchase)
            {
                p_CheckRestorePurchase = RKUtils::RKThread_Create("thread_init_restore_purchase", &IAPManager::checkRestorePurchase_thread, (void*)this, RKThreadPriority::RKThreadPriority_Lowest);
                RKThread_Start(p_CheckRestorePurchase);
            }
            
            
            return 0;
        }
        
        void IAPManager::OnCheckRestorePurchaseComplete()
        {
            
            // keep the thread running until we want to stop it
            bool bExit = RKInterlock_Query(m_exit) != 0;
            while (!bExit)
            {
                // wait for work to be added
                RKCriticalSection_Enter(m_criticalSection);
                bExit = RKInterlock_Query(m_exit) != 0;
                
                while (!p_IsGetPurchaseComplete && !bExit)
                {
                    RKThreadCondition_Sleep(m_condition, m_criticalSection , 0xFFFFFF); //sleep 1000 ms aka 1 sec
                    bExit = RKInterlock_Query(m_exit) != 0;
                }
                
                if (bExit)
                {
                    RKCriticalSection_Leave(m_criticalSection);
                    break;
                }
                
#if defined GAME_IOS
                int str_id_complete = IsRestorePurchaseComplete();
                if(str_id_complete != -1)
                {
                    if(str_id_complete == 1 || GetListIAPPurchaseRestore().size() <= 0)
                    {
                        p_state_restore_purchase = STATE_IAP_LOAD::IAP_FAIL;
                    }
                    else
                    {
                        for(int i = 0 ; i < GetListIAPPurchaseRestore().size();i++)
                        {
                            if(GetListIAPPurchaseRestore().at(i) == id_pack_remove_ads)
                            {
                                p_IsRemoveAds = true;
                                //cannot change banner state at another thread, push it to main thread
                                p_state_restore_purchase = STATE_IAP_LOAD::IAP_SUCCEED;
                                //XFlatform::Ads_HideBanner();
//                                File::SaveMgr->GetDataSave()->SetCharAtName(SAVE_STR_REMOVE_ADS,"1");
//                                File::SaveMgr->SaveTheGameSave();
                                break;
                            }
                            
                        }
                    }
                    ResetRestorePurchaseComplete();
                }
#endif
                
                p_IsGetPurchaseComplete = false;
                p_IsOnRestoringPurchase = false;
                
                RKCriticalSection_Leave(m_criticalSection);
            }
            RKInterlock_Reset(m_exit);
        }
        
        uint32 IAPManager::checkRestorePurchase_thread(void * data)
        {
            IAPManager * iap = static_cast<IAPManager *>(data);
            if(iap)
            {
                iap->OnCheckRestorePurchaseComplete();
            }
            
            return 0;
        }
        
        void IAPManager::WakeUpThreadCheckPurchase()
        {
            p_IsGetPurchaseComplete = true;
            RKThreadCondition_WakeAll(m_condition);
            
        }
        
        void IAPManager::Update(float dt)
        {
            if(p_IsRemoveAds == true && p_IsOnRestoringPurchase == true)
            {
                
                XFlatform::Ads_HideBanner();
                p_IsOnRestoringPurchase = false;
                DestroyAllThread();
            }
        }
        
        void IAPManager::DestroyAllThread()
        {
            
            bool bExit = false;
            if (m_exit)
            {
                bExit = RKInterlock_Query(m_exit) != 0;
            }
            
            if (!bExit)
            {
                // tell the work thread to exit
                RKInterlock_Lock(m_exit);
                
                // wake work thread
                if (m_condition)
                {
                    RKThreadCondition_WakeAll(m_condition);
                }
                
                if (p_CheckRestorePurchase)
                {
                    RKUtils::RKThread_WaitForExit(p_CheckRestorePurchase);
                    RKUtils::RKThread_Destroy(&p_CheckRestorePurchase);
                }
                
                if (m_condition)
                {
                    RKThreadCondition_Destroy(&m_condition);
                }
                
                if (m_criticalSection)
                {
                    RKCriticalSection_Destroy(&m_criticalSection);
                }
                
                if (!m_exit)
                {
                    RKInterlock_Destroy(&m_exit);
                }
            }
        }
        
        RKString IAPManager::GetPriceFormatWithID(RKString id)
        {
#ifdef GAME_IOS
            return IAP::GetPriceWithIDItem(id);
#else
            return "";
#endif
        }
        
        RKString IAPManager::GetLastTransactionID()
        {
            return m_lastTransactionID;
        }
        
        void IAPManager::SetLastTransactionID(RKUtils::RKString transactionID)
        {
            m_lastTransactionID = transactionID;
        }
	}
}

#endif //USE_IAP