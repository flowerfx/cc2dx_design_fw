#ifndef WINRT_IAP
#define WINRT_IAP
#if defined USE_IAP && (defined OS_W8 || defined WP8 || defined OS_W10)
#include "../../LoadDataXML.h"
#include "../../DisplayView.h"
#include "../IAPManager.h"
using namespace RKUtils;
namespace Utility
{
	namespace IAP
	{
		class WinRT_IAP
		{
		private:

			static WinRT_IAP * m_instance;

			std::vector<RKString> p_list_item_iap;

			STATE_IAP_LOAD p_state_load_list_item;
			STATE_IAP_LOAD p_state_purchase_item;

			Platform::String^ p_current_id_iap_in_process;
		public:
			static WinRT_IAP * DefaultHelper()
			{
				if (!m_instance)
				{
					m_instance = new WinRT_IAP();
				}
				return m_instance;
			}

			WinRT_IAP();
			~WinRT_IAP();

			STATE_IAP_LOAD GetStateLoadListItem();
			void SetStateLoadListItem(STATE_IAP_LOAD state);

			STATE_IAP_LOAD GetStatePurchaseItem();
			void SetStatePurchaseItem(STATE_IAP_LOAD state);

			void LoadListItem(std::vector<RKString> list_item);
			void PurchaseItem(RKString iap_key_id);

			void FulfullmentAfterPurchase(Platform::String ^productId, Platform::Guid transactionId);

			void RequestListItem();
 
		};
	}
}
#endif //OS_W8 || defined WP8 || defined OS_W10

#endif //WINRT_IAP