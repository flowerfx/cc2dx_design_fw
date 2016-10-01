#ifndef __HUD_SCREEN_H__
#define __HUD_SCREEN_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
class Player;
class PlayerGroup;
class ChatBoard;
class Category;
class Item;
class HUDScreen : public BaseScreen
{
protected:
	WidgetEntity * p_list_player_layout;
	WidgetEntity * p_panel_igm;
	WidgetEntity * p_panel_ig_setting;
	WidgetEntity * p_panel_show_profile;
	WidgetEntity * p_panel_send_gift;
	WidgetEntity * p_panel_bottom;

	WidgetEntity * p_panel_chat_root;
	WidgetEntity * p_btn_recent_chat;
	WidgetEntity * p_btn_emo_chat;
	WidgetEntity * p_list_recent_chat;
	WidgetEntity * p_list_emoticon_chat;
	WidgetEntity * p_text_input_chat;

	WidgetEntity * p_btn_purchase_iwin;
	WidgetEntity * p_btn_purchase_ruby;

	WidgetEntity * p_layout_invite_player;
	WidgetEntity * p_list_player_invite;
	WidgetEntity * p_top_title_invite;

	std::vector<PlayerGroup *>		p_list_player_group;
	WidgetEntity *					p_panel_count_down;

	//WidgetEntity *					p_panet_time_bar;
	float							p_timer_count_down;
	bool							p_on_count_down;
	int								p_max_player;
	ChatBoard*						_chat_board;
	std::map<int, xml::BasicDec*>	p_list_pos_dec;
	cocos2d::Map<long, Sprite*>		p_list_avatar_download;
private:
	xml::UILayerWidgetDec* xml_icon_dec;
	xml::UILayerWidgetDec* xml_chat_text_dec;
	ThreadWrapper *		  p_thread_parse_ui;
	void start_thread_parse_ui();

	struct PlayerWaiting
	{
		Player* player;
		bool is_check;

		PlayerWaiting(Player* p) {
			player = p;
			is_check = true;
		}
		virtual ~PlayerWaiting()
		{
			if (player)
			{
				delete player;
			}
			is_check = false;
		}
	};
	std::vector<PlayerWaiting*> p_list_player_waiting;
public:
	HUDScreen();
	virtual ~HUDScreen();

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
	virtual void OnShow()																	override;
	virtual void PushTextChat(RKString name, RKString msg);
	PlayerGroup * GetPlayerGroupAtIdx(int idx);
	virtual void SetMaxPlayer(int max_player);
	
	void showGameScreen(s16 roomID, s16 boardID, int ownerID, int money, std::vector<Player*> players);
	void OnCreateBtnRecentChat(RKString text);
	void OnCreateBtnEmoticonChat(int idx , xml::UILayerWidgetDec* xml_dec);

	void OnHideCountDown();
	void OnShowCountDown(float time, float cur_time);
	void onSetData(void* data);

	void onReceiveAvatarCatList(void * data);
	void onReceiveAvatarList(void * data);
	void onBuyingAvatarResult(void * data);

	void OnParsePlayerToInviteList(WidgetEntity * widget, int idx, bool isPlayer = true);
	void OnReceiveWaitingList(std::vector<Player*> list);
	void CheckAllWaitingList(bool ischeck);
	bool ModAllCheck(bool is_check);
	void HandleChatText(RKString text);

	void OnShowProfileLayout();

	void setGiftCategories(std::vector<Category*> categories);
	void setGiftItems(int categoryID, int totalpage, std::vector<Item*> items);
	void sendGiftTo(int from, std::vector<int> tos, int itemID);

	int GetStateHUDScreen();
protected:

	void LoadXMLEmotion(TiXmlDocument * p_objectXML);
	void ShowGameScreen(GameType game_type);

protected:
	int		p_current_idx_add_1;
	float	p_time_add_1;
	float   p_origin_time_add_1;
	int		p_size_object_need_to_add_1;
	xml::UILayerWidgetDec* p_sample_dec;
	virtual void LoadListAsTime1(float dt);

	int		p_current_idx_add_2;
	float	p_time_add_2;
	float   p_origin_time_add_2;
	int		p_size_object_need_to_add_2;
	virtual void LoadListAsTime2(float dt);

};

#endif //__HUD_SCREEN_H__

