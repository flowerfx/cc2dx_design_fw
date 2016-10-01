#include "LoadingScreen.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "Network/Core/Message.h"
#include "Network/Game/GameMessageHandler.h"

#include "RKThread_Code/RKThreadWrapper.h"
#include "SoundManager.h"
LoadingScreen::LoadingScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;

	p_current_state_loading = -1;
	p_state_load_resource = 0;
	p_state_handle_return_server = 0;

	p_message_reconnect = nullptr;
	p_thread_load_xml = nullptr;
	finish_load_xml = false;
	state_load = 0;
}


LoadingScreen::~LoadingScreen()
{
	p_current_state_loading = -1;
	p_state_load_resource = 0;
	p_state_handle_return_server = 0;
	if (p_message_reconnect)
	{
		delete p_message_reconnect;
		p_message_reconnect = nullptr;
	}
	if (p_thread_load_xml)
	{
		delete p_thread_load_xml;
		p_thread_load_xml = nullptr;
	}
}

int LoadingScreen::Init()
{
	RKString _menu = "loading_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}
	//create 2 icon on load and off load
	auto prite_load = SpriteFrameCache::getInstance()->getSpriteFrameByName("loadingscreen/load.png");
	cocos2d::Rect rec = prite_load->getRect();
	auto fm = SpriteFrame::createWithTexture(prite_load->getTexture(), cocos2d::Rect(rec.origin.x, rec.origin.y, rec.size.width / 2, rec.size.height));
	SpriteFrameCache::getInstance()->addSpriteFrame(fm, "loadingscreen/load_1.png");
	fm = SpriteFrame::createWithTexture(prite_load->getTexture(), cocos2d::Rect(rec.origin.x + (rec.size.width / 2), rec.origin.y, rec.size.width / 2, rec.size.height));
	SpriteFrameCache::getInstance()->addSpriteFrame(fm, "loadingscreen/load_2.png");
	//
	InitMenuWidgetEntity(_menu);
	
	CallInitComponent(true);

	return 1;
}

int	LoadingScreen::InitComponent()
{
	p_panel_loading_process = GetWidgetChildByName("loading_screen.loading_process");
	return 1;
}

void LoadingScreen::LoadXMLFile()
{
	PWARN1("THREAD : LOAD SOUND");
	XMLMgr->OnLoadXMLData1("sound_dec", [&](TiXmlDocument * objectXMl) { 
		XMLMgr->OnLoadListSoundDecXML(objectXMl); });

    PWARN1("THREAD : LOAD LANGUAGE");
    //set language
    int lang_idx_select = LANGUAGE_TYPE::LANG_VI;
    ScrMgr->GetDataSaveInt(STR_SAVE_LANGUAGE, lang_idx_select);
    LangMgr->SetNewLanguage((LANGUAGE_TYPE)lang_idx_select);
    //
    PWARN1("THREAD : LOAD LIST GAME DEC");
    XMLMgr->OnLoadXMLData1("ListGameDec", [&](TiXmlDocument * objectXMl) { ScrMgr->LoadXMLListGame(objectXMl); });
    //
    PWARN1("THREAD: LOAD XML SET LOGIN ");
    RKString set_load = "login";
    XMLMgr->OnLoadXMLData1("UIWidget_table",
                           [&, set_load](TiXmlDocument * objectXMl)
                           {
                               XMLMgr->OnLoadUIWidgetDecTableXML(objectXMl, set_load);
                           });
    //
    PWARN1("THREAD: LOAD XML SET main_0 ");
    set_load = "main_0";
    XMLMgr->OnLoadXMLData1("UIWidget_table",
                           [&, set_load](TiXmlDocument * objectXMl)
                           {
                               XMLMgr->OnLoadUIWidgetDecTableXML(objectXMl, set_load);
                           });
    
    PWARN1("THREAD: LOAD XML SET main_1 ");
    set_load = "main_1";
    XMLMgr->OnLoadXMLData1("UIWidget_table",
                           [&, set_load](TiXmlDocument * objectXMl)
                           {
                               XMLMgr->OnLoadUIWidgetDecTableXML(objectXMl, set_load);
                           });
    
    PWARN1("THREAD: LOAD XML SET main_2 ");
    set_load = "main_2";
    XMLMgr->OnLoadXMLData1("UIWidget_table",
                           [&, set_load](TiXmlDocument * objectXMl)
                           {
                               XMLMgr->OnLoadUIWidgetDecTableXML(objectXMl, set_load);
                           });
    PWARN1("THREAD: LOAD XML FINISH");
    finish_load_xml = true;
}

void LoadingScreen::StartThreadLoadXML()
{
	if (p_thread_load_xml)
	{
		delete p_thread_load_xml;
		p_thread_load_xml = nullptr;
	}
	p_thread_load_xml = new ThreadWrapper();
	p_thread_load_xml->CreateThreadWrapper("thread_load_xml",
		([](void * data) -> uint32
	{
		LoadingScreen * scr = (LoadingScreen*)data;
		scr->p_thread_load_xml->OnCheckUpdateThreadWrapper
		([scr]()
		{
            scr->LoadXMLFile();
		});
		return 1;
	}), (void*)this);
}

int LoadingScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_current_state_loading < 5)
	{
		// 5 frame first is show the flash screen
		p_current_state_loading++;
	}
	else if (p_current_state_loading == 5)
	{  
		TextureMgr->LoadTextureAtlas("atlas_main");
		SetStateLoadIdx(state_load, true);
		state_load++;
        p_current_state_loading++;
	}
	else if (p_current_state_loading == 6)
	{
		p_state_load_resource = 1;
		//StartThreadLoadXML();
		SetStateLoadIdx(state_load, true);
		state_load++;
		p_current_state_loading++;
    }
	else if (p_current_state_loading == 7)
	{
        if(!finish_load_xml)
        {
            LoadXMLFile();
        }
		auto table_xml = XMLMgr->GetTableXMLLoad();
		auto table_screen = ScrMgr->GetTableScreenLoad();

		if (table_screen.size() > table_xml.size())
		{
			PASSERT2(false, "table_screen must be smaller or equal with table_xml");
		}
		else
		{
			if (table_screen.size() == table_xml.size())
			{
				if (finish_load_xml)
				{
					p_state_load_resource = 2;
					p_current_state_loading++;
				}
				else
				{
					CCLOG("MAIN THREAD : ON WAITING XML LOAD");
				}
			}
			else
			{
				int last_screen_init = table_screen.size();
				int next_screen_init = table_xml[last_screen_init];
				ScrMgr->InitDetailScreen(next_screen_init);
				SetStateLoadIdx(state_load, true);
				state_load++;
			}
		}
	}
	else if (p_current_state_loading == 8)
	{
		GetSound->InitSoundEngine();
		p_current_state_loading ++ ;
	}
	else if (p_current_state_loading == 9)
	{
		p_current_state_loading ++;
		float value_data = 0.0f;
		if (ScrMgr->GetDataSaveFloat(STR_SAVE_SOUND_VOLUME, value_data))
		{
			GetSound->SetVolume(value_data / 100.f, true);
		}
		if (ScrMgr->GetDataSaveFloat(STR_SAVE_MUSIC_VOLUME, value_data))
		{
			GetSound->SetVolume(value_data / 100.f, false);
		}

		ScrMgr->SwitchToMenu(LOGIN_SCREEN, LOADING_SCREEN);
	}
    else if (p_current_state_loading == 11)
	{
		p_current_state_loading ++;
	}
	else if (p_current_state_loading == 12)
	{
		p_current_state_loading++;
	}
	else if (p_current_state_loading == 13)
	{ 
		if (p_state_handle_return_server == 0)
		{
			ScrMgr->SwitchToMenu(MAIN_SCREEN, LOADING_SCREEN);
			//GlobalService::getInfo(19);
		}
		else if (p_state_handle_return_server == 1)
		{
			ScrMgr->SwitchToMenu(LOBBY_SCREEN, LOADING_SCREEN);
			ScrMgr->SendDataToScreen(MENU_LAYER::LOBBY_SCREEN, -1, (void*)"SET_GAME_TYPE", nullptr);
			//GlobalService::getInfo(19);
			p_state_handle_return_server = 0;
		}
		else if (p_state_handle_return_server == 2)
		{
			GetGameMessageHandler()->onMessage((iwincore::Message*)p_message_reconnect);
			SAFE_DELETE(p_message_reconnect);
		}
		p_state_handle_return_server = 0;
		p_current_state_loading = 100;
	}

	return 1;
}

void LoadingScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void LoadingScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void LoadingScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
}

void LoadingScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(LOADING_SCREEN);
	//p_menu_show_next = MENU_NONE;
}


void LoadingScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

int LoadingScreen::GetStateLoading()
{
	return p_current_state_loading;
}

void LoadingScreen::SetStateLoading(int state)
{
	p_current_state_loading = state;
}

void LoadingScreen::SetMessageHandle(void * msg)
{
	if (p_message_reconnect)
	{
		delete p_message_reconnect;
		p_message_reconnect = nullptr;
	}
	p_message_reconnect = msg;
}

void LoadingScreen::SetStateLoadIdx(int idx, bool load)
{
	if ((idx * 2) >= p_panel_loading_process->GetNumberChildWidget())
		return;
	p_panel_loading_process->GetWidgetChildAtIdx(idx * 2)->Visible(!load);
	p_panel_loading_process->GetWidgetChildAtIdx((idx * 2) + 1)->Visible(load);
}