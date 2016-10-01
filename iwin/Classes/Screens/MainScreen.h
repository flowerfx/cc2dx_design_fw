#ifndef __MAIN_SCREEN_H__
#define __MAIN_SCREEN_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

namespace iwinmesage
{
	class GameList;
}
class GameObjectData;
class MainScreen : public BaseScreen
{
private:
	WidgetEntity * p_bottom_menu;
	WidgetEntity * p_panel_setting;
	WidgetEntity * p_panel_list_game;
	std::vector<int> p_list_game_win;
	std::vector<int> p_list_game_ruby;


	iwinmesage::GameList*		p_gamelist;

	WidgetEntity * p_list_server;
	WidgetEntity * p_panel_choose_server;
	WidgetEntity * p_panel_list_btn_choose_game;
	WidgetEntity * panel_avatar_user;
	WidgetEntity * p_list_lang_choose;

	WidgetEntity * p_panel_list_btn;

	WidgetEntity * p_panel_notice_12;
	WidgetEntity * p_layout_anoun_server;

	WidgetEntity* p_panel_info_game;

	int p_stage_get_data;

	bool p_have_announcement;
	RKString p_announce_text;
	bool p_annouce_is_running;

	std::map<RKString, xml::UILayerWidgetDec*> p_list_sample_dec;
	std::map<int, GameObjectData*> p_list_game_id;
	int current_state_panel_game;

	float p_current_time_request_list_wait;
	bool  p_already_parse_list_wait;
	//merge from libgdx
	bool  p_isCallRequestFriend;
	//
	RKString p_msg_server;
	//
	bool p_have_layout_logout_appear;
protected:
	void SetAnnouncementAction();
	void OnShowHLBtnGameAtIdx(int idx);
	void OnParsePlayerNumberToEachGame();

public:
	MainScreen();
	virtual ~MainScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)		override;

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)						override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnFadeInComplete()															override;
	virtual void OnTouchMenuBegin(cocos2d::Point p)											override;
	virtual void ParseUserUI()																override;
	virtual void OnReceiveFriendList()														override;
	void onLoginSuccess();
	void onReceivedGameList(void* gamelist);

	void LoadXMLListGame(TiXmlDocument * p_objectXML);
	void OnParseCustomUIDec(vector<int> list_game, RKString name ,std::map<RKString, xml::UILayerWidgetDec*> p_list_sample_dec);
	
	void OnReceiveAnnouncementFromServer(RKString text);
	iwinmesage::GameList* GetGameList();

	void onReceiveAvatarCatList(void * data);
	void onReceiveAvatarList(void * data);
	void onBuyingAvatarResult(void * data);
	void onReceiveListWait(void * data1, void* data2, void* data3);

	void forceStopAnnouce(bool value);
	void SetNumWinfree(int number);
	void SetNumUnReadMsg(int number);
	void SetMsgServer(RKString msg_server);

	void SetNeedToReloadFriend();

	void SetStateLoadData(int state);

	bool IsRubyState();

	int GetStateMainScreen();
};

#endif //__MAIN_SCREEN_H__

