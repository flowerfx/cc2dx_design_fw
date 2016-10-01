#ifndef _SOCIAL_MANAGER_H_
#define _SOCIAL_MANAGER_H_

#include "../DisplayView.h"
#include "cocos2d.h"
#include "RKString_Code/RKString.h"
#include "Downloader/DownloadHandler.h"
USING_NS_CC;
using namespace RKUtils;
namespace Utility
{
    namespace Social
    {
        enum FACEBOOK_STATE_LOGIN
        {
            FB_NONE = 0,
            FB_ON_LOGGING,
            FB_FAILED_LOGIN,
            FB_LOGIN_SUCCEED
        };
        
        enum STATUS_DOWNLOAD
        {
            D_NONE = 0,
            D_DOWNLOADING,
            D_FAIL,
            D_SUCCESS
        };
        

        class SocialManager
        {
        private:

			struct funct
			{
				void * refer;
				void(*func)(void* , void *);

				funct(void * ref ,void(*funct)(void*, void *))
				{
					refer = ref;
					func = funct;
				}
				~funct()
				{
					refer = nullptr;
					func = nullptr;
				}
			};
            
            static SocialManager* m_Instance;
            
            FACEBOOK_STATE_LOGIN p_state_login_fb ;
            
            STATUS_DOWNLOAD     p_state_download_handler;
            
            Downloader::DownloadHandler * p_downloader;

		std::map<RKString, std::function<void(void * data, void * str, int tag)> > p_list_register_function_callback;
            
        public:

			void RegisterFunction(const char * strCB, const std::function<void(void * data, void * str, int tag)> & call_back);

			bool processRegistedFunction(RKString call_command, void * data, int tag);
            
            static SocialManager* GetInstance()
            {
                if(!m_Instance)
                {
                    m_Instance = new SocialManager();
                }
                return m_Instance;
            }
            
            SocialManager();
            ~SocialManager();
            
            
            FACEBOOK_STATE_LOGIN CurrentStateLogin() { return p_state_login_fb;}
            void SetStateLoginFB(FACEBOOK_STATE_LOGIN state) { p_state_login_fb = state;}
            
            STATUS_DOWNLOAD GetStateDownload() { return p_state_download_handler;}
            void SetStateDownload(STATUS_DOWNLOAD val) { p_state_download_handler = val;}
            
            void OnUpdate();
            void Init();
            
            void DownloadFile(RKString url, RKString name_save , const std::function<void(void * data, void * str, int tag)> & call_back);
			void RemoveCallBackWithName(RKString name);
			void RemoveAllCallBackExcept(std::vector<RKString> list_name);
        };
    }
    #define SocialMgr Social::SocialManager::GetInstance()
}
#endif//_SOCIAL_MANAGER_H_