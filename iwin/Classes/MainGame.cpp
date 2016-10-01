#include "MainGame.h"
#include "InputManager.h"
#if USE_CC_AUDIO
#include "SoundManager.h"
#endif
#include "FXFlatform/XFlatform.h" //for the cross flatform wp8/android/ios
#include "ObjectTexture.h"

#include "Screens/ScreenManager.h"

#include "FileManager.h"
#include "Features/FacebookMgr.h"
#include "Network/Core/ServerMgr.h"
#include "Network/JsonObject/Heartbeat/HeartBeatConfiguration.h"

#include "Network/Global/Globalservice.h"
#include "Network/Global/GlobalMessageHandler.h"

#include "Social/SocialManager.h"

//#include "Profiler.h"

using namespace Utility;
USING_NS_CC;

MainGame * MainGame::p_Instance = NULL;
MainGame::MainGame()
{
	this->retain();
    PauseTheGame = false;
	p_loadingStep = 0;
	p_stage_error_message = 0;
	p_list_texture_avatar_download.clear();
	p_heartBeat = new iwinmesage::HeartBeatConfiguration();
}


MainGame::~MainGame()
{
	//_profile_font->release();
	this->autorelease();

	for (auto a : p_list_texture_avatar_download)
	{
		delete a.second;
	}
	p_list_texture_avatar_download.clear();
}

bool MainGame::init()
{
	std::srand(std::time(0));
	GetFacebookMgr()->init();

	p_IsInitialized = false;
	//
	Utility::InitSizeGame();
	//
    InitTheGame(0.0f);
	//
	GetServerMgr()->initFromString("Test 34.5|10.8.34.5:8888;Kim Chau|203.162.51.38:8888;Kim Khanh|203.162.51.39:8888");
	this->scheduleUpdate();
	return true;
}
//virtual bool update();
// there's no 'id' in cpp, so we recommend returning the class instance pointer
cocos2d::Scene* MainGame::scene()
{
	// 'scene' is an autorelease object
	cocos2d::Scene *scene = cocos2d::Scene::create();
	// 'layer' is an autorelease object
	MainGame *layer = MainGame::create();
	layer->setName("main_game");
	// add layer as a child to scene
	scene->addChild(layer);
	//
	//scene->addChild(Control::InputManager::create() , -1);
	// return the scene
	return scene;
}

bool MainGame::InitLoadingScreen()
{
	return true;
}

bool MainGame::InitTheGame(float dt)
{

	if (p_loadingStep == 0)
	{
		InputMgr->init();

#if defined WP8 || defined OS_W8
		DataDir("/Assets/Resources/iWin/");
#elif defined OS_W10
		DataDir("/iWin/");
#else
		DataDir("iWin/");
#endif
        //Director::getInstance()->InitModuleRKFile();
		InitValue();
		XMLMgr->OnLoaderXML();

		XMLMgr->OnLoadXMLData<Utility::xml::LoadDataXML>("language", 
			std::mem_fn(&Utility::xml::LoadDataXML::OnLoadListLangXML), XMLMgr);
        
		//load ark file
		File::SaveMgr->LoadTheGameSave();
		//load texture
		XMLMgr->OnLoadXMLData<Utility::xml::LoadDataXML>("texture_atlas", 
			std::mem_fn(&Utility::xml::LoadDataXML::OnLoadTextureAtlasDecXML), XMLMgr);

		TextureMgr->LoadTextureAtlas("atlas_GUI");
		TextureMgr->LoadTextureAtlas("atlas_Init");
		TextureMgr->LoadTextureFrame("frame_sheet");
		TextureMgr->LoadFont();

		ScrMgr->Init();

		RKString set_load = "loading";
		XMLMgr->OnLoadXMLData1("UIWidget_table",
			[&, set_load](TiXmlDocument * objectXMl) 
		{
			XMLMgr->OnLoadUIWidgetDecTableXML(objectXMl, set_load); 
		});	
		ScrMgr->InitDetailScreen(1);
		//
		p_loadingStep++;

		ScrMgr->SetStateLoading(p_loadingStep);
		return false;
	}
	else
	{
		p_loadingStep = ScrMgr->GetStateLoading();
        if(p_loadingStep == 1)
        {
            ScrMgr->SwitchToMenu(LOADING_SCREEN);
			SocialMgr->Init();
        }
		if (p_loadingStep < 10)
		{
			return false;
		}
	}

	return true;
}


void MainGame::update(float dt)
{
#if CC_REF_LEAK_DETECTION
	Ref::printLeaks();
#endif
	if (p_heartBeat && p_heartBeat->getOn() && _allowSendHeartBeat) {
		_heartbeatIntervalCounter += (dt * 1000);
		if (_heartbeatIntervalCounter > p_heartBeat->getInterval()) {
			_heartbeatIntervalCounter = 0;
			_allowSendHeartBeat = false;
			_allowDisconnectWhenTimeOut = true;
			_timeOutIntervalCounter = 0;
			GlobalService::sendHeartBeat();
		}
	}
	if (_allowDisconnectWhenTimeOut) {
		_timeOutIntervalCounter += (dt * 1000);
		if (_timeOutIntervalCounter >= p_heartBeat->getTimeout()) {
			_allowDisconnectWhenTimeOut = false;
			_timeOutIntervalCounter = 0;
			//@PDT::handle logout game and close network
			//GlobalService.closeNetwork();
			//GlobalMessageHandler.getInstance().isShowDisconnectedDialog = true;
			//GlobalMessageHandler.getInstance().onDisconnected();
		}
	}

    if(PauseTheGame)
        return;

	if (!p_IsInitialized)
	{
		if (XFlatform::IsAtLogoState() == true)
		{
			return;
		}
		//	
		bool res = InitTheGame(dt);

		if (res)
		{
			p_IsInitialized = true;


#ifdef USE_AUDIO_ON_THREAD
			GetSound->OnStartThreadPushSound();
#endif
		}
	}

	if(p_loadingStep >= 1)
	{	

		ScrMgr->Update(dt);
		/*if (!_profile_font->getParent())
			getParent()->addChild(_profile_font, 9999)*/;
		//_profile_font->setText(GetProfiler()->getResult());

		InputMgr->update(dt);
	}
}


void MainGame::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
    //
	if (p_loadingStep < 1)
	{
		
	}
	else
	{
		ScrMgr->Visit(renderer, transform, flags);
	}
}

void MainGame::EndScence()
{
	this->unscheduleUpdate();
}

void MainGame::HandleAvatarDownload(int id, RKString name, const std::function<void(void * data, void * str, int tag)> & func, bool is_ID)
{
	RKString url = Utils::getAvataUrlPlayer(id, name.GetString());

	if (p_list_texture_avatar_download.find(url) == p_list_texture_avatar_download.end())
	{
		SocialMgr->DownloadFile(url, name, func);
	}
	else
	{
		func(nullptr, (void*)url.GetString(), 0);
	}
}

void MainGame::HandleAvatarDownload(std::string url, const std::function<void(void * data, void * str, int tag)> & func)
{
	if (p_list_texture_avatar_download.find(url) == p_list_texture_avatar_download.end())
	{
		SocialMgr->DownloadFile(url, url, func);
	}
	else
	{
		func(nullptr, (void*)url.c_str(), 0);
	}
}

void MainGame::HandleURLImgDownload(RKString url, RKUtils::RKString name, const std::function<void(void * data, void * str, int tag)> & func)
{
	if (p_list_texture_avatar_download.find(url) == p_list_texture_avatar_download.end())
	{
		SocialMgr->DownloadFile(url, name, func);
	}
	else
	{
		func(nullptr, (void*)url.GetString(), 0);
	}
}

Sprite * MainGame::CreateAvatarFromImg(void * data, void * str_url)
{
	Image * img = (Image *)data;
	const char * url = (const char *)str_url;
	Texture2D * tex = nullptr;

	if (!img)
	{
		tex = p_list_texture_avatar_download.at(url);
	}
	else
	{
		tex = new Texture2D();
		tex->initWithImage(img, cocos2d::Texture2D::PixelFormat::RGB5A1);
		tex->retain();
		p_list_texture_avatar_download.insert({ url , tex});
	}
	Sprite * sprite_img = Sprite::createWithTexture(tex);
	sprite_img->retain();

	SAFE_DELETE(img);

	return sprite_img;
}

RKString MainGame::GetUrlAvatarFrom(int id, bool is_ID)
{
	RKString template_url = "";
	if (is_ID)
	{
		template_url = "http://203.162.51.37/a/items/";
	}
	else
	{
		template_url = "http://203.162.51.37/a/i/";
	}
	template_url += std::to_string(id);
	return template_url;
}


void MainGame::Network_CheckUserNameExist(RKUtils::RKString name)
{
	Server curServer = GetServerMgr()->getCurServer();

	GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(),
		([name]()
	{
		GlobalService::CheckUserName(name.GetString());
	})
	);
}

void MainGame::Network_LoginToGame(RKString name, RKString pass)
{
	//if user login failed, server will close connect to client and user must be reconnect
	Server curServer = GetServerMgr()->getCurServer();

		GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(), 
			([name, pass]()
			{
                ScrMgr->OnConnectToServer();
			})
	);
}

void MainGame::Network_RegisterNewAccount(RKUtils::RKString name, RKUtils::RKString pass)
{
	Server curServer = GetServerMgr()->getCurServer();

	GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(), ([name, pass]()
	{
		GlobalService::requestRegister(name.GetString(), pass.GetString());
	}));

}

void MainGame::Network_GetCaptchaInfo(RKString id)
{
	Server curServer = GetServerMgr()->getCurServer();
	GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(), ([id]()
	{
		if (id == "")
			GlobalService::getCaptchaFirstTime();
		else
			GlobalService::getCaptchaRefresh(id.GetString());
	}));
}

void MainGame::Network_CheckCaptchaResult(RKString id , RKString captcha)
{
	Server curServer = GetServerMgr()->getCurServer();
	GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(), ([id, captcha]()
	{

		GlobalService::sendCheckCaptcha(id.GetString(),captcha.GetString());

	}));
}

void MainGame::Network_getSMSInfoForgotPassword(RKUtils::RKString name)
{
	Server curServer = GetServerMgr()->getCurServer();
	GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(), ([name]()
	{
		GlobalService::getSMSInfoForgotPassword(name.GetString());
	}));
}

void MainGame::Network_loginFacebook(std::string token)
{
	Server curServer = GetServerMgr()->getCurServer();
	GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(), ([token]()
	{
		GlobalService::loginWithFacebookToken(token, "4.9.4");
	}));
}

void MainGame::Network_HandleGetForgottenPassword(ubyte type, RKString user_name)
{
	Server curServer = GetServerMgr()->getCurServer();
	GlobalService::connect(curServer._ip.c_str(), curServer._port.c_str(), ([type,user_name]()
	{
		GlobalService::getForgotPassword(type, user_name.GetString());
	}));
}

void MainGame::Network_MessageHandler(int id, void * data, LIST_AGR())
{
	switch (id)
	{
		case -1: //the id that only have message
		{
			PASSERT2(data != nullptr, "arg is null");

			if (RKString((const char*)data) == "GET_GAME_LIST")
			{
				ScrMgr->SendDataToScreen(MENU_LAYER::MAIN_SCREEN, -1, data, data1, nullptr);
			}
			else if (RKString((const char*)data) == "SHOW_GAME_SCREEN")
			{

			}
			else if (RKString((const char*)data) == "SET_GAME_TYPE")
			{
				ScrMgr->SendDataToScreen(MENU_LAYER::LOBBY_SCREEN, -1, data, nullptr);
			}
			else if (RKString((const char*)data) == "COUNT_QUEST_LIST")
			{
				ScrMgr->SendDataToScreen(MENU_LAYER::MAIN_SCREEN, -1,data, data1, nullptr);
			}
			else if (RKString((const char*)data) == "LEADERBOARD_FRIEND")
			{
				ScrMgr->SendDataToScreen(MENU_LAYER::LEADERBOARD_SCREEN, -1, data, data1, data2 , data3,nullptr);
			}
			break;
		}
		case IwinProtocol::LOGIN_SUCESS:
		{
			
            ScrMgr->OnHideDialog();
            PWARN1("LOGIN SUCCEED !");
			ScrMgr->SendDataToScreen(MAIN_SCREEN, IwinProtocol::LOGIN_SUCESS, nullptr);
            
            if (ScrMgr->GetCurrentMenuUI() == MENU_LAYER::LOGIN_SCREEN
                //handle case reconnect
                && ScrMgr->GetMenuUI(LOGIN_SCREEN)->GetMenuShowNext() != HUD_SCREEN
                )
            {
                ScrMgr->SwitchToMenu(MENU_LAYER::MAIN_SCREEN, LOGIN_SCREEN);
            }
            ScrMgr->SetStateLoading(100);
			break;
		}
		case IwinProtocol::GET_PROFILE:
		{
			if (((User*)data)->id == GameController::myInfo->IDDB)
			{
				if (ScrMgr->GetCurrentMenuUI() != HUD_SCREEN)
				{
					ScrMgr->onSetUser(GameController::myInfo->userProfile);
				}
				else
				{
					ScrMgr->SendDataToScreen(MENU_LAYER::HUD_SCREEN, id, data, nullptr);
				}
			}
			else
			{
				GetCommonScr->OnSetUserPlayer(((User*)data));
			}
			break;
		}
		case IwinProtocol::USER_DATA:
		{
			int id_user = to_data(int, data);
			int current_id_user = GameController::myInfo->IDDB;

			if (id_user == current_id_user)
			{
				ScrMgr->SendDataToScreen(MENU_LAYER::PROFILE_SCREEN, id, data, data1);
			}
			else
			{
				GetCommonScr->OnReceiveAchievement(to_data(int, data), data1);
				//PASSERT2(false, "not handle this session, throw assert to remind!");
			}
			break; 
		}
		case IwinProtocol::LOGIN_FAIL:
		{
			ScrMgr->OnShowDialog(
				"error",
				RKString((const char *)data),
				TYPE_DIALOG::DIALOG_ONE_BUTTON
			);
            PWARN1("LOGIN FAILED !");
			break;
		}
		case IwinProtocol::ERROR_MESSAGE:
		{
			ScrMgr->OnShowDialog(
				"notice",
				RKString((const char *)data),
				TYPE_DIALOG::DIALOG_ONE_BUTTON, "log_out", 
				[&](const char * str, const char* btn_name) {
					//ScrMgr->ForceLogOutToLoginScreen();
				});
			p_stage_error_message = 1;
			break;
		}
		break;
		case IwinProtocol::GET_SMS_SYNTAX:
		{
			break;
		}
		case IwinProtocol::SERVER_INFO:
		{
			ScrMgr->SendDataToScreen(MENU_LAYER::MAIN_SCREEN, IwinProtocol::SERVER_INFO, data);
			break;
		}
		case 19:
		{
			//
			auto list_item = RKString((const char*)data).Split(",");
			auto avatar_ = list_item.GetAt(list_item.Size() - 1).Split(":");
			RKString name_save = avatar_.GetAt(0);

			RKString url = "";
			for (size_t i = 1; i < avatar_.Size(); i ++)
			{
				url += avatar_.GetAt(i);
				if (i != avatar_.Size() - 1)
				{
					url += ":";
				}

			}
			url.ReplaceAll("\"","");
			url.ReplaceAll("}", "");

			HandleAvatarDownload(0,"default_avatar",
				[&](void * data, void *str, int tag)
			{
				ScrMgr->SetAvatarDefault(data , str);
			});
			break;
		}
		case IwinProtocol::STRONGEST_LIST:
		case IwinProtocol::RICHEST_LIST:
		case IwinProtocol::RICHEST_RUBY_LIST:
		{
			auto current_menu = ScrMgr->GetCurrentMenuUI();
			switch (current_menu)
			{
			case MENU_LAYER::LEADERBOARD_SCREEN:
			case MENU_LAYER::LOBBY_SCREEN:
			case MENU_LAYER::NEW_LOBBY_SCREEN:
			{
				if (current_menu == MENU_LAYER::LEADERBOARD_SCREEN)
				{
					ScrMgr->SendDataToScreen(current_menu, id, data, data1);
				}
				else
				{
					GetCommonScr->LB_OnReceiveListPlayer(id, *((unsigned char*)data), *(std::vector<Player * >*)data1);
				}

				break;
			}
			default:
				break;
			}
		}
		default:
			break;
	}
}

void MainGame::updateUnreadMsg()
{
	if (GameController::myInfo->IDDB == -1)
	{
		PASSERT2(false, "problem here!");
		return;
	}

	int num = 0;
	int numChat = num;
	int numActiveAdmin = ADDFRIEND_MESSAGE.size();
	num += numActiveAdmin;
	PASSERT2(false, "need to implement sql lite here to get unread msg!");
	ScrMgr->SendDataToScreen(MAIN_SCREEN, -1, cv_void("UPDATE_UNREAD_MSG"), cv_void(num));
}

void MainGame::setUserData(void *data)
{
	InputMgr->DispathTouchEvent((cocos2d::EventTouch*)data);
}

void MainGame::startHeartBeat(HeartBeatConfiguration* config)
{
	CC_SAFE_DELETE(p_heartBeat);
	p_heartBeat = config;
	_heartbeatIntervalCounter = 0;
	_allowSendHeartBeat = true;
}

void MainGame::stopHeartBeat()
{
	p_heartBeat->setOn(false);
	_allowSendHeartBeat = false;
	_heartbeatIntervalCounter = 0;
	_allowDisconnectWhenTimeOut = false;
	_timeOutIntervalCounter = 0;
}

void MainGame::onHeartBeatResponse()
{
	_allowSendHeartBeat = true;
	_allowDisconnectWhenTimeOut = false;
	_timeOutIntervalCounter = 0;
}