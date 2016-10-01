#ifndef __NEW_SCREEN_LOBBY_H__
#define __NEW_SCREEN_LOBBY_H__
#include "Network/JsonObject/Game/BoardOwner.h"
#include "Network/JsonObject/lobby/LobbyList.h"
#include "Network/JsonObject/lobby/ChatLobbyList.h"
#include "ScreenManager.h"

using namespace iwinmesage;

class IwinScrollView;
class IwinListView;
class EmotionBoard;
class ChatBoard;

class NewLobbyScreen : public BaseScreen
{
public:
	static const int RUBY_TYPE = 2;
	static const int WIN_TYPE = 1;
private:
	
	WidgetEntity*		p_scrollview_bet;
	WidgetEntity*		p_scrollitem_bet;
	WidgetEntity*		p_scrollview_chat;
	WidgetEntity*		p_scrollitem_chat;
	WidgetEntity*		p_panel_right_lobby;
	WidgetEntity*		p_panel_right_board;
	WidgetEntity*		p_panel_user_info;
	WidgetEntity*		p_chat_text_field;
	WidgetEntity*		p_list_player_lobby;
	WidgetEntity*		p_money_value;
	WidgetEntity*		p_bet_slider;

	IwinScrollView*		_scroll_view_with_bet;
	IwinListView*		_listview_board;

	ChatBoard*			_chat_board;
	EmotionBoard*		_emotion_board;
	LobbyList*			_cur_lobby_list_data;
	std::vector<BoardOwner>	_cur_list_board;

	s64			_max_money_slider;
	s64			_current_bet;

	cocos2d::Map<std::string, Sprite*>	p_list_avatar_download;
	std::vector<ChatLobbyItem*>			p_list_text_chat;
public:
	NewLobbyScreen();
	virtual ~NewLobbyScreen();

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
	virtual void OnBeginFadeOut()															override;
	virtual void ParseUserUI()																override;
	void onSetGameTypeSuccessed();

	void updateItemScrollViewBet(IwinScrollView* scrollview, WidgetEntity* entity, int idx);
	void initItemScrollViewBet(IwinScrollView* scrollview, WidgetEntity* entity, int idx);

	void updateItemListViewBoard(WidgetEntity* entity, int idx);
	void onReceiveLobbyList(LobbyList* lobby_list);
	void onReceiveOwnerBoardList(std::vector<BoardOwner> boards, int win, int lose, int draw, int leave);
	void onReceiveChatList(ChatLobbyList* chatLobbyList);
	void handleChatEmotion(int idx);
	void setTitle(std::string text);
	void SetCurrentGameID(int id);

	int GetStateNewLobbyMenu();
private:
	void showListBoard(bool visible);
	void showLobby(int type);
	void updateNoBoardLabel(s64 money);

	int p_current_game_id;
	int p_previous_game_id;
protected:

	void ResetLeaderboard(bool cleanup = true);

	void ShowRichestWin();
	void ShowRichestRuby();
	void ShowStrongest();
};

#endif //__NEW_SCREEN_LOBBY_H__
