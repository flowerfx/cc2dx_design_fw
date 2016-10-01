#ifndef _DOWNLOAD_HANDLER_H_
#define _DOWNLOAD_HANDLER_H_


#include "../../DisplayView.h"
#include "../../FileManager.h"

#include "RKThread_Code/RKThread.h"
#include "RKThread_Code/RKInterlock.h"

//#define USE_HTTP_REQUEST

#ifndef USE_HTTP_REQUEST
#include "network/CCDownloader.h"
#else
#include "network/HttpClient.h"
#endif
using namespace cocos2d;
using namespace network;
using namespace RKUtils;
namespace Utility
{
	namespace Social
	{
		namespace Downloader
		{

			class DownloadHandler
			{
			private:

				struct state_save_url
				{
					RKString url_download;
					RKString name_save;
					std::function<void(void * data, void * str, int tag)> call_back;

					state_save_url()
					{
						url_download = "";
						name_save = "";
						call_back = nullptr;
					}
					state_save_url(RKString p_url_download, RKString p_name_save)
					{
						url_download = p_url_download;
						name_save = p_name_save;
						call_back = nullptr;
					}
					state_save_url(RKString p_url_download, RKString p_name_save, const std::function<void(void * data, void * str, int tag)> & func)
					{
						url_download = p_url_download;
						name_save = p_name_save;
						call_back = func;
					}

				};

				std::vector<state_save_url> p_list_queue_url_download;


				RKUtils::RKThread *			  p_ThreadUpdateDownload;
				RKUtils::RKInterlock*         m_exitDownload;
				RKUtils::RKCriticalSection*   m_criticalSectionDownload;
				RKUtils::RKCriticalSection*   m_callback_mutex;
				RKUtils::RKThreadCondition*   m_conditionDownload;

				volatile bool p_IsOnUpdateThreadDownload;

				static uint32 ThreadUpdateDownload(void* pThreadData);
				void UpdateDownloadThread();
				void downLoadObject(RKString url, RKString nameSave, std::function<void(void * data, void * str, int tag)>  call_back);

#ifndef USE_HTTP_REQUEST
				std::unique_ptr<network::Downloader> p_downloader;
#else
				void onObjectDownLoaded(HttpClient* pSender, HttpResponse* pResponse);
#endif
			public:
				DownloadHandler();
				~DownloadHandler();

				void initDownloader();

				void OnDownloadListObject(std::vector<RKString> ListUrl, std::vector<RKString> ListnameSave , const std::function<void(void * data, void * str, int tag)> & call_back);
			};
		}
	}
}

#endif//_DOWNLOAD_HANDLER_H_