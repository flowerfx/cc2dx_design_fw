#ifndef __SCREEN_LEADERBOARD_H__
#define __SCREEN_LEADERBOARD_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

struct ListGameDetail;
class Player;
class LeaderboardScreen : public BaseScreen
{
private:

	WidgetEntity * p_panel_list_choose;
	WidgetEntity * p_panel_list_player;

	WidgetEntity * p_list_choose_leaderboard;
	WidgetEntity * p_list_player_leaderboard;
	WidgetEntity * p_list_friend_leaderboard;

	WidgetEntity * p_layout_pull_update_player;
	WidgetEntity * p_layout_pull_loadmore_player;
	WidgetEntity * p_layout_pull_update_friend;

	WidgetEntity * p_layout_top_list_choose;
	WidgetEntity * p_layout_top_list_player;

	WidgetEntity *	p_tab_change_btn;

	WidgetEntity * p_panel_user;

	bool			p_is_on_tab_world; //otherwise is tab friend;

	bool			p_already_set_listgamelb;

	std::vector<Player*>				p_current_list_player_lb;
	std::vector<Player*>				p_current_list_friend_lb;
	cocos2d::Map<int, Sprite*>			p_list_avatar_download;
	cocos2d::Map<int, Sprite*>			p_list_avatar_friend;

	size_t		p_current_page_;

	int			p_current_rank_idx;
	int			p_current_rank_idx_friend;

	int			p_current_stage_lb;
	int			p_current_game_id;
protected:

	void OnParsePlayerToLeaderboard(WidgetEntity * widget, int idx , bool isPlayer = true);
	void CleanUpListAvatar();
	void ResetLeaderboard(int state);
public:
	LeaderboardScreen();
	virtual ~LeaderboardScreen();

	virtual  int	Init()																			override;
	virtual  int	InitComponent()																	override;
	virtual  int	Update(float dt)																override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)			override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)				override;

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)	override;
	virtual void OnDeactiveCurrentMenu()															override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)								override;
	virtual void OnBeginFadeIn()																	override;

	virtual void ParseUserUI()																		override;

	void CreateListGameLeaderboard(ListGameDetail lg , int idx);

	void OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player);

	void OnReceiveListFriend(int id, ubyte page, std::vector<Player * > list_player);

	int CurrentStateLeaderboard();
};

#endif //__SCREEN_LEADERBOARD_H__

