#ifndef __TAIXIU_LAYER_H__
#define __TAIXIU_LAYER_H__
#include "Screens/ScreenManager.h"
using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class TaiXiuGameInfo;
	class TaiXiuBetResult;
	class TaiXiuUserOrderList;
	class TaiXiuUserOrder;
	class TaiXiuUserHistoryList;
	class TaiXiuUserHistory;
}

class TaiXiuLayer
{
private:
	BaseScreen * _common_screen;

	WidgetEntity * p_layout_taixiu;
	WidgetEntity * p_layout_tai_xiu_mini_btn;
	WidgetEntity * p_layout_tai_xiu_panel;

	WidgetEntity * p_layout_taixiu_leaderboard;
	WidgetEntity * p_taixiu_list_player_leaderboard;

	WidgetEntity * p_layout_taixiu_myhistory;
	WidgetEntity * p_taixiu_list_myhistory;

	WidgetEntity * p_main_taixiu_layout;
	WidgetEntity * p_panel_taixiu_bg_top_layout;
	WidgetEntity * p_panel_taixiu_bg_history;

	WidgetEntity * p_layout_taixiu_bet;
	WidgetEntity * p_text_effect_bet_money;
	WidgetEntity * p_text_effect_bonus_money;
	WidgetEntity * p_text_effect_message_server;

	WidgetEntity * p_taixiu_circle_layout;
	WidgetEntity * p_taixiu_circle_result_layout;
	WidgetEntity * p_taixiu_circle_dice_layout;
	WidgetEntity * p_taixiu_circle_small_result_layout;
	WidgetEntity * p_taixiu_circle_cd_layout;

	Vec2 _previous_pos_touch;

	iwinmesage::TaiXiuGameInfo *p_taixiu_gameinfo;
	iwinmesage::TaiXiuBetResult *p_taixiu_bet_result;
	iwinmesage::TaiXiuUserOrderList *p_taixiu_userorder_list;
	iwinmesage::TaiXiuUserHistoryList *p_taixiu_history_list;

	//0 is win 1 is ruby
	int current_stage_taixiu_panel;
	//-1 is default 0 is tai 1 is xiu
	int current_stage_bet_taixiu;
	//
	int		p_result_taixiu;
	int		p_previous_result_taixiu;
	float	p_delta_move_btn_tx;
	bool	p_taixiu_already_have_result;
protected:
	void OnDragBtnTaiXiu();
	void ResetAndSendSerVer();
	void ChangeButtonWinRubyTaiXiu();
	void OnCheckTextInputMoney(WidgetEntity * _widget);
	void OnShowPanelBetTaixiu(bool show, CallFunc * call_back = nullptr);
	WidgetEntity * GetSliderBet();
	void SetInputMoneyTextBet(s64 money);
	s64 GetCurrentMoney();
	void BetMoney(int stage);
	void SetNumberBetResult(s64 money, int state);
	void OnShowResultTaiXiu(int result, bool show);
	void OnParseTaiXiuLeaderboard(WidgetEntity* _widget, iwinmesage::TaiXiuUserOrder * user, int idx);
	void OnParseTaiXiuHistory(WidgetEntity* _widget, iwinmesage::TaiXiuUserHistory * his, int idx);

	void OnShowResultCirlceWithTimer(bool show, double long total_time, double long current_time);
	void OnShowResultCircle();
public:
	TaiXiuLayer();
	virtual ~TaiXiuLayer();

	void InitTaiXiu(BaseScreen* screen);
	bool ProcessWidgetTaixiu(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
	void OnShowBtnTaiXiu(bool show);
	void OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type);
	void OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult);
	void OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList);
	void OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList);
	void OnReceiveTransferMoney(s64 money, RKString info, int type);
	bool HaveTaixiuLayout();
	void ParseUserUI();
	int Update(float dt);
	int GetStateTaiXiu();

};

#endif //__TAIXIU_LAYER_H__

