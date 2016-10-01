#ifndef __SCREEN_FRIEND_H__
#define __SCREEN_FRIEND_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

class iWinPlayer;
class ChatMessage;
class Player;
class FriendScreen : public BaseScreen
{
private:
	WidgetEntity * p_search_player_layout;
	WidgetEntity * p_list_friend_main;
	WidgetEntity * p_layout_pull_update;

	WidgetEntity * p_search_myfriend_layout;
	WidgetEntity * p_chat_friend_layout;

	//from newfriendscreen
	std::vector<ChatMessage*>	p_ADDFRIEND_MESSAGE ;
	std::vector<ChatMessage*>	p_ADMIN_MESSAGE ;

	std::vector<iWinPlayer*>	p_listRecentChat;
	std::vector<iWinPlayer*>	p_listFriendChat;
	std::vector<Player*>		p_listPlayerFriend;
	//
	cocos2d::Map<int, Sprite*>	p_list_avatar_download;
	//
	int current_number_parse_avatar;
	int p_stage_get_list_friend;
	int p_current_idx_search_parse;
	//
	RKString p_current_friend_search;
protected:
	void OnParseListFriendMain(WidgetEntity* _widget, int idx , RKString name_fill);
	int GetNumberFriend();

	void ParseListFriend(WidgetEntity* _widget, Player * pl);
	void ParseListWaitingFriend(WidgetEntity* _widget, ChatMessage* player);
	size_t GetNumberFriendContainName(RKString name);
public:
	FriendScreen();
	virtual ~FriendScreen();

	virtual  int	Init()																				override;
	virtual  int	InitComponent()																		override;
	virtual  int	Update(float dt)																	override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)				override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)					override;

	virtual void	OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)	override;
	virtual void	OnDeactiveCurrentMenu()																override;
	virtual void	OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)								override;

	virtual void	OnReceiveFriendList()																override;
public:
	std::vector<ChatMessage*> GET_MESSAGE(int state);
	void PUSH_MESSAGE(int state, std::vector<ChatMessage*> vec);
	void CLEAR_LIST_WHEN_LOGOUT();

	int GetStateFriendMenu();
	void onReceiveServerWarning(RKString msg);

};

#endif //__SCREEN_FRIEND_H__

