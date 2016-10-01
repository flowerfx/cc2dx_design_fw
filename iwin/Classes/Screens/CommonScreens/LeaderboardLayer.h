#ifndef __LEADERBOARD_LAYER_H__
#define __LEADERBOARD_LAYER_H__
#include "Screens/ScreenManager.h"
using namespace Utility;
using namespace Utility::UI_Widget;
using namespace iwinmesage;

class Player;
class LeaderboardLayer
{
private:
	BaseScreen * _common_screen;
	/*
	use : leaderboard layout
	*/
	WidgetEntity * p_lb_layout_leaderboard;
	WidgetEntity * p_lb_layout_leaderboard_player;
	WidgetEntity * p_lb_layout_pull_update_player;
	WidgetEntity * p_lb_layout_pull_loadmore_player;

	std::vector<Player*>				p_lb_current_list_player_;
	cocos2d::Map<int, Sprite*>			p_lb_list_avatar_player_download;

	size_t		p_lb_current_page_lb;

	int			p_lb_current_rank_lb_idx;
	int			p_lb_current_stage_lb;
	int			p_lb_previous_state_lb;

	int			p_current_game_id;
protected:
	Player* LB_GetPlayerByIdx(int idx);

	void LB_OnSetPlayerEachLB(ubyte page,
		std::vector<Player * > list_player,
		int & idx_rank,
		WidgetEntity * list_table,
		std::vector<Player*> * list_player_receive,
		cocos2d::Map<int, Sprite*> * list_avatar);
	void LB_OnParsePlayerToLeaderboard(WidgetEntity * widget, int idx, bool isPlayer = true);

public:
	LeaderboardLayer();
	virtual ~LeaderboardLayer();

	void LB_InitValueLB(BaseScreen * common_screen);
	void LB_ResetLeaderboard(bool cleanup = true);
	void LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player);
	void LB_ShowRichestWin();
	void LB_ShowRichestRuby();
	void LB_ShowStrongest(ubyte current_game_id);
	void LB_OnTouchMenuBegin(cocos2d::Point p);
	bool LB_SetInteractive();
	int GetStateLB();
};

#endif //__LEADERBOARD_LAYER_H__

