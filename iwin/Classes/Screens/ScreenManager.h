#ifndef __MENU_MANAGER_H__
#define __MENU_MANAGER_H__

#include "BaseScreen.h"
#include "MainGame.h"
#include "Factory.h"
#include "cocos2d.h"
#include "Utils.h"

#include "CommonScreen.h"
using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

enum LOGIN_TYPE
{
	LOGIN_NONE = -1,
	LOGIN_IWIN = 0,
	LOGIN_FACEBOOK,
	LOGIN_GOOGLE,
	LOGIN_APPLE,
	LOGIN_GUEST,
	LOGIN_MOBO,
	LOGIN_COUNT
};

class GamePlayMgr;
#define cv_void(a) (void*)&a
#define string_void(a) (void*)(a.c_str)
#define to_data(type,var) *(type*)var
#define to_str(var)	std::string((char *)var)

struct ListGameDetail
{
	GameType id;
	RKString name;
	RKString dec;
	RKString str;
	RKString img;
	std::vector<RKString> texture_load;
	ListGameDetail(GameType ID, RKString Name, RKString Dec, RKString Str, RKString Img)
	{
		id = ID;
		name = Name;
		dec = Dec;
		str = Str;
		img = Img;
		texture_load.clear();
	}
	ListGameDetail()
	{
		id = GameType::NULL_TYPE;
		name = "";
		dec = "";
		str = "";
		img = "";
		texture_load.clear();
	}
	virtual ~ListGameDetail()
	{
		texture_load.clear();
	}
};

class ChatMessage;
namespace iwinmesage
{
	class TaiXiuUnsupportedList;
	enum class SCREEN_ID;
}
class ScreenManager
{
private:
	static ScreenManager * m_Instance;

	//
	bool p_IsPopUpAppear;
	//menu list
	std::vector<int>						p_table_screen_load;

	std::vector<MENU_LAYER>					p_CurretUIActive;
	std::map<MENU_LAYER, BaseScreen*>		pListLayer;
	Factory::FactoryWithIndex<BaseScreen>	s_factory_menu_entity;
	
	GamePlayMgr * p_gameplay_mgr;
	Sprite *	  p_avatar_default;
	//create a weak pointer to handle common screen
	BaseScreen*	  p_common_screen;
	BaseScreen*	  p_shop_screen;
	BaseScreen*	  p_change_award_screen;

	//general list game load from xml
	std::map<GameType, ListGameDetail> p_gamelist_detail;

	//store the user of game
	User * p_user;

	bool		  p_mark_deinit_gamescreen;

	std::vector<RKString> p_texture_have_load;

	bool p_have_screen_id_dirty;
protected:

	void SetActiveUI(MENU_LAYER layer, int idx = -1);
	void DeActiveUI(MENU_LAYER layer);
	void RegisterMenuUI();
	void LoadTextureAsGame(int id_game);
public:
	//load game list xml
	void LoadXMLListGame(TiXmlDocument * p_objectXML);

	ListGameDetail GetGameDetailByType(GameType type);
	ListGameDetail GetGameDetailByIdx(int idx);

	int GetSizeListGameDetail();

	void onSetUser(void * user);
	User* GetMyUser();
public:
	ScreenManager();
	virtual ~ScreenManager();
	static ScreenManager * GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new ScreenManager();
		}
		return m_Instance;
	}

	MENU_LAYER GetCurrentMenuUI(int idx = 0);
	void  SwitchToMenu(MENU_LAYER target_menu, MENU_LAYER from_menu = MENU_LAYER::MENU_NONE);
	void  CloseCurrentMenu(MENU_LAYER m);
    void  ShowCurrentMenu(MENU_LAYER m, int idx =-1);

	BaseScreen* GetMenuUI(MENU_LAYER name);
	BaseScreen* GetMenuUIByName(RKString name);


	int		Init();
	int		InitScreen(int state);
	int		InitDetailScreen(int id_screen);
	void	Update(float dt);
	void	Visit(Renderer *renderer, const Mat4& transform, uint32_t flags);
	void	Draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
	//call popup from menu mgr
	void OnShowDialog(RKString title = "", 
					  RKString str = "", 
					  TYPE_DIALOG type = (TYPE_DIALOG)0, 
					  const char * strCB = "",
					  const std::function<void(const char * str, const char* btn_name)> & func = nullptr,
					  const std::function<void(const char * str, const char* btn_name)> & func1 = nullptr,
					  const std::function<void(const char * str, const char* btn_name)> & func2 = nullptr,
		RKString title_1 = "", RKString title_2 = "");
	void OnHideDialog();

	void PushMenuToTop(MENU_LAYER layer);

	//change language all menu
	bool ChangeLanguage(LANGUAGE_TYPE lang);

	//state loading
	int GetStateLoading();
	void SetStateLoading(int state);

	//communicate specific with screen
	void SendDataToScreen(MENU_LAYER screen, int id, void * data, 
		LIST_AGR_H(  nullptr));

	GamePlayMgr * GetGamePlayMgr();
	void InitGamePlay(int game_id);
	void DeInitGamePlay();

	void SwithToGameScreen(GameType game_type, int max_player);
	Sprite * GetAvatarDefault() { return p_avatar_default; }
	void SetAvatarDefault(void * data , void * str);
	BaseScreen * GetGameScreen();

	void ShowShop(bool visible);
	void ShowChangeAward(bool visible);

	void DeinitGameScreen() { p_mark_deinit_gamescreen = true; }
	void ForceLogOutToLoginScreen();

	void SaveDataInt(RKString name, int data);
	bool  GetDataSaveInt(RKString name, int &result);
	void SaveDataFloat(RKString name, float data);
	bool GetDataSaveFloat(RKString name , float &result);
	void SaveDataString(RKString name, RKString data);
	bool GetDataSaveString(RKString name , RKString & result);
	void SaveTheGame();
	void showScreen(int screenId, int gameId);

	//notify state change
	void MyInfoChanged();
	//friend list receive
	void onReceiveFriendList();
	//
	BaseScreen * GetCommonScreen();

	std::vector<ChatMessage*> GET_MESSAGE(int state);
	void PUSH_MESSAGE(int state , std::vector<ChatMessage*> vec);

	std::vector<int> GetTableScreenLoad();

	bool IsRubyState();

	iwinmesage::SCREEN_ID ConvertCurrentMenuToScreenID();
	void SetScreenIDDirty();

	void OnReceiveTransferMoney(s64 newMoney, std::string info, int moneyType);
	/*
	tai xiu state
	*/
private:
	bool	btn_taixiu_dirty;
    bool    p_have_connect_server;
	iwinmesage::TaiXiuUnsupportedList * p_taixiu_unsupportlist;
public:
	void setTaiXiuUnsupportedList(iwinmesage::TaiXiuUnsupportedList * unsupport);
	void updateButtonTaiXiu();
	void setBtnTaixiuDirty();
	void LuckyCircleVisiable(bool isShow);
    
    //happen when connect to server succeed;
    void OnConnectToServer();
};

#define ScrMgr ScreenManager::GetInstance()
#define GamePlay	ScreenManager::GetInstance()->GetGamePlayMgr()
#define NODE(x)		x->GetResource()
#define GetCommonScr static_cast<CommonScreen*>(ScreenManager::GetInstance()->GetCommonScreen())
#define GetUser		ScreenManager::GetInstance()->GetMyUser()

#define ADDFRIEND_MESSAGE ScreenManager::GetInstance()->GET_MESSAGE(0)
#define ADMIN_MESSAGE ScreenManager::GetInstance()->GET_MESSAGE(1)
#define PUSH_ADDFRIEND_MESSAGE(p) ScreenManager::GetInstance()->PUSH_MESSAGE(0 , p)
#define PUSH_ADMIN_MESSAGE(p) ScreenManager::GetInstance()->PUSH_MESSAGE(1 , p)

#endif //__MENU_MANAGER_H__

