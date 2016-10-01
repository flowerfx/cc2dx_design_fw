#ifndef __COMMON_SCREEN_H__
#define __COMMON_SCREEN_H__

#include "ScreenManager.h"

using namespace iwinmesage;
using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
class Player;
class TaiXiuLayer;
class LuckyCircleScreen;
class LeaderboardLayer;
class DialogLayer;
class PlayerInfoLayer;
class ChangePassLayer;
class InviteLayer;
class AnnouceServerLayer;
class ConfirmCodeLayer;
namespace iwinmesage
{
	class TaiXiuGameInfo;
	class TaiXiuBetResult;
	class TaiXiuUserOrderList;
	class TaiXiuUserHistoryList;

	class LuckyCircleResultList;
	class LuckyCircleQuestList;
}
class CommonScreen : public BaseScreen
{
private:
	TaiXiuLayer *			_taixiu_layer;
	LeaderboardLayer *		_leaderboard_layer;
	LuckyCircleScreen*		_lucky_screen;
	DialogLayer *			_dialog_layer;
	PlayerInfoLayer *		_player_info_layer;
	ChangePassLayer *		_change_pass_layer;
	InviteLayer		*		_invite_layer;
	AnnouceServerLayer *	_annouce_server_layer;
	ConfirmCodeLayer	*   _confirm_code_layer;
private:
	bool		 p_have_layout_interface;
	bool		 p_have_dialog_appear;
	BaseScreen * p_screen_come_from;
	
public:
	CommonScreen();
	virtual ~CommonScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)		override;

	virtual void	OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void	OnDeactiveCurrentMenu()													override;
	virtual void	OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)					override;
	virtual void	OnBeginFadeIn()															override;
	virtual void	OnFadeInComplete()														override;
	virtual void	OnTouchMenuBegin(cocos2d::Point p)										override;
	virtual void	ParseUserUI()															override;

	void			OnScreenComeFromEnable();
	void			OnSetScreenComeFrom(bool enable , bool disable_update = true);
	void			SetHaveLayoutInteractive(bool value , int state = 0);

	void			SetScreenComeFrom(BaseScreen * scr);
	bool			HaveLayoutInter();
	int				GetStateCommonScr();
public:
	//confirm code
	void OnShowConfirmCode();

	//annouce server
	void OnReceiveAnnouncementFromServer(RKString text);
	void ForceStopAnnouce(bool value);
	//invite layer
	void OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar);

	//change pass layer
	void OnShowChangePassLayout();

	//dialog layer
	void OnShowDialog(RKString str_title = "",
		RKString str_message = "",
		TYPE_DIALOG type = TYPE_DIALOG::DIALOG_NONE,
		const char * strCB = "",
		const std::function<void(const char * call_back, const char* btn_name)> & func = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func1 = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func2 = nullptr,
		RKString title_btn_0 = "", RKString title_btn_1 = ""
	);
	void OnHideDialog();

	//player info
	bool IsPlayerInfoVisible();
	void OnShowPlayerInfo(Player * player);
	void OnSetUserPlayer(User * user);
	void OnReceiveAchievement(int id, void * list_achie);

	//leaderboard
	void LB_ResetLeaderboard(bool cleanup = true);
	void LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player);
	void LB_ShowRichestWin();
	void LB_ShowRichestRuby();
	void LB_ShowStrongest(ubyte current_game_id);
	bool LB_SetInteractive();

	//lucky circle
	void LuckyCricleVisible(bool isShow);
	void OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_quest_list);
	void OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item);

	//taixiu
	void OnShowBtnTaiXiu(bool show);
	void OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type);
	void OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult);
	void OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList);
	void OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList);
	void OnReceiveTransferMoney(s64 money, RKString info, int type);
	bool HaveTaixiuLayout();
};

#endif //__COMMON_SCREEN_H__
