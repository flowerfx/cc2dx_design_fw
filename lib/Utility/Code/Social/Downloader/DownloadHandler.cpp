#include "DownloadHandler.h"
#include "../../FileManager.h"

#include "../../FXFlatform/XFlatform.h"
#include "../SocialManager.h"

namespace Utility
{
	namespace Social
	{
		namespace Downloader
		{
			DownloadHandler::DownloadHandler() :
				p_ThreadUpdateDownload(0)
				, m_exitDownload(0)
				, m_criticalSectionDownload(0)
				, m_callback_mutex(0)
				, m_conditionDownload(0)
			{
#ifndef USE_HTTP_REQUEST
				p_downloader.reset(new network::Downloader());
#endif
			}

			DownloadHandler::~DownloadHandler()
			{
				bool bExit = false;
				if (m_exitDownload)
				{
					bExit = RKInterlock_Query(m_exitDownload) != 0;
				}

				if (!bExit)
				{
					// tell the work thread to exit
					RKInterlock_Lock(m_exitDownload);

					// wake work thread
					if (m_conditionDownload)
					{
						RKThreadCondition_WakeAll(m_conditionDownload);
					}

					if (p_ThreadUpdateDownload)
					{
						RKUtils::RKThread_WaitForExit(p_ThreadUpdateDownload);
						RKUtils::RKThread_Destroy(&p_ThreadUpdateDownload);
					}

					if (m_conditionDownload)
					{
						RKThreadCondition_Destroy(&m_conditionDownload);
					}

					if (m_criticalSectionDownload)
					{
						RKCriticalSection_Destroy(&m_criticalSectionDownload);
					}

					if (m_callback_mutex)
					{
						RKCriticalSection_Destroy(&m_callback_mutex);
					}


					if (!m_exitDownload)
					{
						RKInterlock_Destroy(&m_exitDownload);
					}
				}
			}

			void DownloadHandler::initDownloader()
			{
				p_IsOnUpdateThreadDownload = false;

				if (!m_criticalSectionDownload)
				{
					m_criticalSectionDownload = RKCriticalSection_Create("cs_update_download");
				}

				if (!m_callback_mutex)
				{
					m_callback_mutex = RKCriticalSection_Create("cs_callback_mutex");
				}

				if (!m_conditionDownload)
				{
					m_conditionDownload = RKThreadCondition_Create("Update_download_Cond");
				}

				if (!m_exitDownload)
				{
					m_exitDownload = RKInterlock_Create("Update_download_Exit");
				}
				RKInterlock_Reset(m_exitDownload);

				if (!p_ThreadUpdateDownload)
				{
					p_ThreadUpdateDownload = RKThread_Create("thread_update_downloader", &DownloadHandler::ThreadUpdateDownload, (void*)this, RKThreadPriority::RKThreadPriority_Highest);
					RKUtils::RKThread_Start(p_ThreadUpdateDownload);
				}

				p_list_queue_url_download.clear();
			}

			uint32 DownloadHandler::ThreadUpdateDownload(void* pThreadData)
			{
				DownloadHandler * download_ = static_cast<DownloadHandler*>(pThreadData);
				if (download_)
				{
					download_->UpdateDownloadThread();
				}
				return 0;
			}

			void DownloadHandler::OnDownloadListObject(std::vector<RKString> ListUrl, std::vector<RKString> ListnameSave , const std::function<void(void * data, void * str, int tag)> & call_back)
			{
				bool wake_up_thread = false;
				Social::STATUS_DOWNLOAD state = SocialMgr->GetStateDownload();
				if (p_list_queue_url_download.size() == 0 )
				{
					wake_up_thread = true;
				}
				
                CCASSERT(ListUrl.size() == ListnameSave.size(), "2 list size must be equal");

                //RKCriticalSection_Enter(m_criticalSectionDownload); //enter critical section

                for (size_t i = 0; i < ListUrl.size(); i++)
                {
                    p_list_queue_url_download.push_back(state_save_url(ListUrl.at(i), ListnameSave.at(i), call_back));
                }

                //RKCriticalSection_Leave(m_criticalSectionDownload); //leave critical section
				

				if (wake_up_thread)
				{
					p_IsOnUpdateThreadDownload = true;
					RKThreadCondition_WakeAll(m_conditionDownload);
				}
			}

			void DownloadHandler::UpdateDownloadThread()
			{
				// keep the thread running until we want to stop it
				bool bExit = RKInterlock_Query(m_exitDownload) != 0;
				while (!bExit)
				{
					// wait for work to be added
					RKCriticalSection_Enter(m_criticalSectionDownload);
					bExit = RKInterlock_Query(m_exitDownload) != 0;

					while (!p_IsOnUpdateThreadDownload && !bExit)
					{
						RKThreadCondition_Sleep(m_conditionDownload, m_criticalSectionDownload, 0xFFFFFF); //sleep infinity and wait wake up
						bExit = RKInterlock_Query(m_exitDownload) != 0;
					}

					if (bExit)
					{
						RKCriticalSection_Leave(m_criticalSectionDownload);
						break;
					}

					if (p_list_queue_url_download.size() > 0)
					{
						auto url_download = p_list_queue_url_download.at(0);
						downLoadObject(url_download.url_download, url_download.name_save, url_download.call_back);
					}

					p_IsOnUpdateThreadDownload = false;

					RKCriticalSection_Leave(m_criticalSectionDownload);
				}
				RKInterlock_Reset(m_exitDownload);
			}

			void DownloadHandler::downLoadObject(RKString url, RKString nameSave, std::function<void(void * data, void * str, int tag)> call_back)
			{
#ifdef USE_HTTP_REQUEST
				HttpRequest* request = new (std::nothrow) HttpRequest();
				request->setUrl(url.GetString());
				request->setRequestType(HttpRequest::Type::GET);
				const network::ccHttpRequestCallback& call_back = ([=](network::HttpClient * client, network::HttpResponse * pResponse)
				{
					onObjectDownLoaded(client, pResponse);
				});
				request->setResponseCallback(call_back);
				request->setTag(nameSave.GetString());
				HttpClient::getInstance()->send(request);                  
				request->release();
#else
				p_downloader->createDownloadDataTask(url.GetString(), nameSave.GetString());
				// define success callback
				p_downloader->onDataTaskSuccess = [this , url , call_back](const cocos2d::network::DownloadTask& task,
					std::vector<unsigned char>& data)
				{

					Image * img = new Image();
					img->initWithImageData(data.data(), data.size());

					RKString name_img = url; //task.identifier;
					//
					if (call_back)
					{
						RKCriticalSection_Enter(m_callback_mutex); //enter critical section

						call_back((void*)img, (void *)name_img.GetString(), 0);

						RKCriticalSection_Leave(m_callback_mutex);
					}

					//bool result = SocialMgr->processRegistedFunction(name_img, (void*)img, 0);
					//if (!result)
					{
						//CC_SAFE_DELETE(img);
					}
					//                  
					SocialMgr->SetStateDownload(D_SUCCESS);

					//if (SocialMgr->GetStateDownload() == D_FAIL || SocialMgr->GetStateDownload() == D_SUCCESS)
					{
						p_list_queue_url_download.erase(p_list_queue_url_download.begin());//remove the url downloaded or fail ...
						if (p_list_queue_url_download.size() > 0)
						{
							p_IsOnUpdateThreadDownload = true;
							RKThreadCondition_WakeAll(m_conditionDownload);
							SocialMgr->SetStateDownload(D_DOWNLOADING);
							//continue download when their is the object on the list
						}
						else
						{
							//download all succeed
							SocialMgr->SetStateDownload(D_NONE);
						}
					}
				};

				p_downloader->onTaskError = [this, call_back](const cocos2d::network::DownloadTask& task, int errorCode, int errorExternalCode, const std::string & error_str)
				{
					CCLOG("download error with code : %d , external code: %d , msg: %s \n", errorCode, errorExternalCode, error_str.c_str());
					p_list_queue_url_download.erase(p_list_queue_url_download.begin());//remove the url downloaded or fail ...
					if (p_list_queue_url_download.size() > 0)
					{
						p_IsOnUpdateThreadDownload = true;
						RKThreadCondition_WakeAll(m_conditionDownload);
						SocialMgr->SetStateDownload(D_DOWNLOADING);
						//continue download when their is the object on the list
					}
					else
					{
						//download all succeed
						SocialMgr->SetStateDownload(D_NONE);
					}
				};
#endif
				SocialMgr->SetStateDownload(D_DOWNLOADING);
			}


#ifdef USE_HTTP_REQUEST
			void DownloadHandler::onObjectDownLoaded(HttpClient* pSender, HttpResponse* pResponse)
			{
				HttpResponse* response = pResponse;

				if (!response)
				{
					SocialMgr->SetStateDownload(D_FAIL);
					cocos2d::log("No Response");
				}
				else
				{
					int statusCode = (int)response->getResponseCode();

					char statusString[64] = {};
					sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
					cocos2d::log("response code: %d", statusCode);

					if (!response->isSucceed())
					{
                        SocialMgr->SetStateDownload(D_FAIL);
						cocos2d::log("response failed");
						cocos2d::log("error buffer: %s", response->getErrorBuffer());
					}
					else
					{
						std::vector<char>*buffer = response->getResponseData();

						Image * img = new Image();
						img->initWithImageData((unsigned char *)&(buffer->front()), buffer->size());

						RKString name_img = response->getHttpRequest()->getTag();
                        
                        //
                        bool result = SocialMgr->processRegistedFunction(name_img, (void*)img, 0);  
						if (!result)
						{
							CC_SAFE_DELETE(img);
						}
						//                  
                        SocialMgr->SetStateDownload(D_SUCCESS);
					}
				}

				if (SocialMgr->GetStateDownload() == D_FAIL || SocialMgr->GetStateDownload()  == D_SUCCESS)
				{
					p_list_queue_url_download.erase(p_list_queue_url_download.begin());//remove the url downloaded or fail ...
					if (p_list_queue_url_download.size() > 0)
					{
						p_IsOnUpdateThreadDownload = true;
						RKThreadCondition_WakeAll(m_conditionDownload);
                        SocialMgr->SetStateDownload(D_DOWNLOADING);
						//continue download when their is the object on the list
					}
					else
					{
						//download all succeed
						SocialMgr->SetStateDownload(D_NONE);
					}
				}
			}
#endif
		}
	}
}