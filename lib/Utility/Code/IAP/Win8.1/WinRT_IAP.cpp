#include "WinRT_IAP.h"
#if defined OS_W8 || defined WP8 || defined OS_W10
#if USE_IAP
#include "../../FXFlatform/XFlatform.h"

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Foundation;
using namespace Windows::Storage;
#define USE_STORE_SIMULATE

#ifdef USE_STORE_SIMULATE
#define CurrentApp CurrentAppSimulator
#else
#define CurrentApp CurrentApp
#endif

namespace Utility
{
	extern Platform::String ^stops(std::string s);
	namespace IAP
	{
		WinRT_IAP * WinRT_IAP::m_instance = NULL;
			
		WinRT_IAP::WinRT_IAP()
		{
			p_state_load_list_item = STATE_IAP_LOAD::IAP_NONE;
			p_state_purchase_item = STATE_IAP_LOAD::IAP_NONE;

			p_current_id_iap_in_process = nullptr;

			p_list_item_iap.clear();
		}

		WinRT_IAP::~WinRT_IAP()
		{
			p_list_item_iap.clear();
		}

		STATE_IAP_LOAD WinRT_IAP::GetStateLoadListItem()
		{
			return p_state_load_list_item;
		}
		void WinRT_IAP::SetStateLoadListItem(STATE_IAP_LOAD state)
		{
			p_state_load_list_item = state;
		}

		STATE_IAP_LOAD WinRT_IAP::GetStatePurchaseItem()
		{
			return p_state_purchase_item;
		}

		void WinRT_IAP::SetStatePurchaseItem(STATE_IAP_LOAD state)
		{
			p_state_purchase_item = state;
		}

		void WinRT_IAP::LoadListItem(std::vector<RKString> list_item)
		{

			p_list_item_iap = list_item;
			p_state_load_list_item = STATE_IAP_LOAD::IAP_PROCESSING;
			try{
#ifdef USE_STORE_SIMULATE
				auto localFolder = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;

				localFolder = localFolder->Concat(localFolder, L"\\");
				localFolder = localFolder->Concat(localFolder, L"Data\\xml\\in-app-purchase.xml");

				create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(localFolder)).then([this](StorageFile^ proxyFile)
						{
							create_task(CurrentAppSimulator::ReloadSimulatorAsync(proxyFile)).then([this]()
							{
#endif
								try
								{
									create_task(CurrentApp::LoadListingInformationAsync()).then([this](ListingInformation^ current_task)
									{
										Platform::String^ currentMarket = current_task->CurrentMarket;
										Platform::String^ formattedPrice = current_task->FormattedPrice;

									}).then([this](concurrency::task<void> t)
									{
										try
										{
											t.get();
											/*if (listing->ProductListings->Size != p_list_item_iap.size())
											{
												p_state_load_list_item = STATE_IAP_LOAD::IAP_FAIL;
											}
											else
											{
												p_state_load_list_item = STATE_IAP_LOAD::IAP_SUCCEED;
											}*/

										}
										catch (Platform::Exception^ exception)
										{
											p_state_load_list_item = STATE_IAP_LOAD::IAP_FAIL;
										}
									});
								}
								catch (Platform::Exception^ exception)
								{
									p_state_load_list_item = STATE_IAP_LOAD::IAP_FAIL;
								}
#ifdef USE_STORE_SIMULATE
					});
				});
#endif
			}
			catch (Platform::Exception^ ex)
			{
				p_state_load_list_item = STATE_IAP_LOAD::IAP_FAIL;
			}
		}

		void WinRT_IAP::PurchaseItem(RKString iap_key_id)
		{
			p_current_id_iap_in_process = stops(iap_key_id.GetString());
			p_state_purchase_item = STATE_IAP_LOAD::IAP_PROCESSING;

			try
			{
				create_task(CurrentApp::RequestProductPurchaseAsync(p_current_id_iap_in_process)).then([this](task<PurchaseResults^> currentTask)
				{
					try
					{
						PurchaseResults^ results = currentTask.get();
						switch (results->Status)
						{
						case ProductPurchaseStatus::Succeeded:
							p_state_purchase_item = STATE_IAP_LOAD::IAP_SUCCEED;
							FulfullmentAfterPurchase(p_current_id_iap_in_process, results->TransactionId);
							break;
						case ProductPurchaseStatus::NotFulfilled:
							p_state_purchase_item = STATE_IAP_LOAD::IAP_SUCCEED;
							FulfullmentAfterPurchase(p_current_id_iap_in_process, results->TransactionId);
							break;
						case ProductPurchaseStatus::NotPurchased:
							//Log("Product 1 was not purchased.", NotifyType::StatusMessage);
							p_state_purchase_item = STATE_IAP_LOAD::IAP_FAIL;
							break;
						}
					}
					catch (Platform::Exception^ exception)
					{
						//Log("Unable to buy Product 1.", NotifyType::ErrorMessage);
						p_state_purchase_item = STATE_IAP_LOAD::IAP_FAIL;
					}

					p_current_id_iap_in_process = L"";
				});
			}
			catch (Platform::Exception^ ex)
			{
				p_state_purchase_item = STATE_IAP_LOAD::IAP_FAIL;
			}
		}

		void WinRT_IAP::FulfullmentAfterPurchase(Platform::String ^productId, Platform::Guid transactionId)
		{
			create_task(CurrentApp::ReportConsumableFulfillmentAsync(productId, transactionId)).then([this](task<FulfillmentResult> currentTask)
			{
				try
				{
					FulfillmentResult result = currentTask.get();
					switch (result)
					{
					case FulfillmentResult::Succeeded:
						//Log("You bought and fulfilled product 1.", NotifyType::StatusMessage);
						break;
					case FulfillmentResult::NothingToFulfill:
						//Log("There is no purchased product 1 to fulfill.", NotifyType::StatusMessage);
						break;
					case FulfillmentResult::PurchasePending:
						//Log("You bought product 1. The purchase is pending so we cannot fulfill the product.", NotifyType::StatusMessage);
						break;
					case FulfillmentResult::PurchaseReverted:
						//Log("You bought product 1. But your purchase has been reverted.", NotifyType::StatusMessage);
						// Since the user's purchase was revoked, they got their money back.
						// You may want to revoke the user's access to the consumable content that was granted.
						break;
					case FulfillmentResult::ServerError:
						//Log("You bought product 1. There was an error when fulfilling.", NotifyType::StatusMessage);
						break;
					}
				}
				catch (Platform::Exception^ exception)
				{
					//Log("You bought Product 1. There was an error when fulfilling.", NotifyType::ErrorMessage);
				}
			});
		}

		void WinRT_IAP::RequestListItem()
		{
			std::vector<RKString> list_key_iap = IAPMgr->GetListKeyIAP();
			LoadListItem(list_key_iap);
		}
	}
}
#endif // USE_IAP
#endif //  OS_W8 || defined WP8