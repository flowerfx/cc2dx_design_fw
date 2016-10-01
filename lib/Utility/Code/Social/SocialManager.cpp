#include "SocialManager.h"
#include "../LoadDataXML.h"
#include "../FXFlatform/XFlatform.h"
using namespace cocos2d;
namespace Utility
{
	namespace Social
    {
       
        SocialManager * SocialManager::m_Instance = NULL;
        
        SocialManager::SocialManager()
        {
            p_state_login_fb = FACEBOOK_STATE_LOGIN::FB_NONE;
            
            p_state_download_handler = STATUS_DOWNLOAD::D_NONE;
            
           // func
            
            p_downloader = NULL;
            
            
            //func
        }
        
        SocialManager::~SocialManager()
        {
            if(p_downloader)
            {
                delete p_downloader;
                p_downloader = NULL;
            }
        }
        
        void SocialManager::OnUpdate()
        {
            
        }
        
        void SocialManager::Init()
        {
            p_downloader = new Downloader::DownloadHandler();
            p_downloader->initDownloader();
            //
           // XFlatform::FB_GetProfile();
        }
        
         void SocialManager::DownloadFile(RKString url, RKString name_save, const std::function<void(void * data, void * str, int tag)> & call_back)
        {
			CCLOG("Download url: %s", url.GetString());
            std::vector<RKString> ListUrl;
            std::vector<RKString> ListnameSave;
            ListUrl.push_back(url);
            ListnameSave.push_back(name_save);
            p_downloader->OnDownloadListObject(ListUrl, ListnameSave, call_back);
			RegisterFunction(name_save.GetString(), call_back);
        }

		 void SocialManager::RemoveCallBackWithName(RKString name)
		 {
			 return;
			 if (p_list_register_function_callback.size() > 0 && p_list_register_function_callback.find(name) != p_list_register_function_callback.end())
			 {
				 p_list_register_function_callback.erase(name);
			 }
		 }

		 void SocialManager::RemoveAllCallBackExcept(std::vector<RKString> list_name)
		 {
			 return;
			 std::map<RKString, std::function<void(void * data, void * str, int tag)>> save_value;
			 bool have_value = false;
			 for (auto it = p_list_register_function_callback.begin(); it != p_list_register_function_callback.end(); it++)
			 {
				 for (size_t i = 0; i < list_name.size(); i++)
				 {
					 if (it->first == list_name.at(i))
					 {
						 save_value.insert(std::pair<RKString, std::function<void(void * data, void * str, int tag)>>(it->first, it->second));
						 have_value = true;
					 }
				 }
			 }
			 p_list_register_function_callback.clear();
			 if (have_value)
			 {
				 p_list_register_function_callback = save_value;
			 }
		 }

		 void SocialManager::RegisterFunction(const char * strCB, const std::function<void(void * data, void * str, int tag)> & call_back)
		 {
			 return;
			 if (p_list_register_function_callback.size() >0 && p_list_register_function_callback.find(strCB) != p_list_register_function_callback.end())
			 {
				 PASSERT2(false, "name already exist!");
			 }

			 p_list_register_function_callback.insert(
				 std::pair<RKString, std::function<void(void * data, void * str, int tag)>>(strCB, call_back)
			 );
		 }

		 bool SocialManager::processRegistedFunction(RKString call_command, void * data, int tag)
		 {
			 return true;
			 if (p_list_register_function_callback.size() > 0 && p_list_register_function_callback.find(call_command) != p_list_register_function_callback.end())
			 {
				 auto fu = p_list_register_function_callback.at(call_command);
				 if (fu)
				 {
					 fu(data, (void *)call_command.GetString(), tag);
				 }
				 p_list_register_function_callback.erase(call_command);
				 return true;
			 }
			 PASSERT2(false, "not find name");
			 return false;
		 }
       
       
    }
 }