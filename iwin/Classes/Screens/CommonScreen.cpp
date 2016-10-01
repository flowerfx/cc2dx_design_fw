
#include "CommonScreen.h"

#include "Network/Global/Globalservice.h"
#include "Utils.h"

#include "Screens/CommonScreens/TaiXiuLayer.h"
#include "Screens/CommonScreens/LuckyCircleScreen.h"
#include "Screens/CommonScreens/LeaderboardLayer.h"
#include "Screens/CommonScreens/DialogLayer.h"
#include "Screens/CommonScreens/PlayerInfoLayer.h"
#include "Screens/CommonScreens/ChangePassLayer.h"
#include "Screens/CommonScreens/InviteLayer.h"
#include "Screens/CommonScreens/AnnouceServerLayer.h"
#include "Screens/CommonScreens/ConfirmCodeLayer.h"
CommonScreen::CommonScreen()
{

	p_screen_come_from = nullptr;
	p_have_layout_interface = false;

	_lucky_screen = nullptr;
	_leaderboard_layer = nullptr;
	_taixiu_layer = nullptr;
	_dialog_layer = nullptr;
	_change_pass_layer = nullptr;
	_invite_layer = nullptr;
	_annouce_server_layer = nullptr;
	_confirm_code_layer = nullptr;
}

CommonScreen::~CommonScreen()
{
	p_screen_come_from = nullptr;

	SAFE_DELETE(_lucky_screen);
	SAFE_DELETE(_taixiu_layer);
	SAFE_DELETE(_leaderboard_layer);
	SAFE_DELETE(_dialog_layer);
	SAFE_DELETE(_player_info_layer);
	SAFE_DELETE(_change_pass_layer);
	SAFE_DELETE(_invite_layer);
	SAFE_DELETE(_annouce_server_layer);
	SAFE_DELETE(_confirm_code_layer);
}

int CommonScreen::Init()
{
	RKString _menu = "common_screen";
	InitMenuWidgetEntity(_menu);
	CallInitComponent(true);
	return 1;
}

int	CommonScreen::InitComponent()
{
	//
	p_IdxMenuData = 0;
	//init weak pointer change pass
	_change_pass_layer = new ChangePassLayer();
	_change_pass_layer->InitChangePass(this);

	//weak pointer annouce server layout
	_annouce_server_layer = new AnnouceServerLayer();
	_annouce_server_layer->InitAnnouceServer(this);

	//weak pointer layout info player
	_player_info_layer = new PlayerInfoLayer();
	_player_info_layer->InitInfoPlayer(this);

	//weak pointer confirm code panel
	_confirm_code_layer = new ConfirmCodeLayer();
	_confirm_code_layer->InitConfirmCode(this);

	//init dialog screen
	_dialog_layer = new DialogLayer();
	_dialog_layer->InitDialogLayout(this);

	//init inivte banner
	_invite_layer = new InviteLayer();
	_invite_layer->OnInitInviteBanner(this);

	//init lucky circle
	_lucky_screen = new LuckyCircleScreen();
	_lucky_screen->InitLuckyCircle(this);

	//init leaderboard
	_leaderboard_layer = new LeaderboardLayer();
	_leaderboard_layer->LB_InitValueLB(this);

	//init taixiu
	_taixiu_layer = new TaiXiuLayer();
	_taixiu_layer->InitTaiXiu(this);

	return 1;
}


int CommonScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	_annouce_server_layer->Update(dt);

	_dialog_layer->Update(dt);
	
	_lucky_screen->Update(dt);

	_taixiu_layer->Update(dt);

	return 1;
}

void CommonScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void CommonScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void CommonScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	bool res = _dialog_layer->ProcessWidgetDialog(name, type_widget, _widget);
	if (res)
	{
		return;
	}

	res = _annouce_server_layer->ProcessWidgetAnnounce(name, type_widget, _widget);
	if (res)
	{
		return;
	}
	res = _change_pass_layer->ProcessWidgetChangePass(name, type_widget, _widget);
	if (res)
	{
		return;
	}
	res = _player_info_layer->ProcessWidgetShowPlayerInfo(name, type_widget, _widget);
	if (res)
	{
		return;
	}
	res = _confirm_code_layer->ProcessWidgetConfirmBox(name, type_widget, _widget);
	if (res)
	{
		return;
	}
	res = _invite_layer->ProcessWidgetInvitePanel(name, type_widget, _widget);
	if (res)
	{
		return;
	}
	res = _taixiu_layer->ProcessWidgetTaixiu(name, type_widget, _widget);
	if (res)
	{
		return;
	}

	_lucky_screen->OnProcessChildTouch(name, type_widget, _widget);
	

}

void CommonScreen::OnDeactiveCurrentMenu()
{
	PASSERT2(false, "this is the specific screen that always show on top screen and not list in the list screen UI of screen manager");
	return;

	ScrMgr->CloseCurrentMenu(COMMON_SCREEN);

	p_current_idx_add_ = -1;
	p_menu_show_next = MENU_NONE;

}

void CommonScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void CommonScreen::OnBeginFadeIn()
{
}

void CommonScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
}

void CommonScreen::OnTouchMenuBegin(cocos2d::Point p)
{
	_leaderboard_layer->LB_OnTouchMenuBegin(p);
}

bool CommonScreen::HaveLayoutInter() 
{
	return (p_have_layout_interface || p_have_dialog_appear);
}

int CommonScreen::GetStateCommonScr()
{
	int stlb = _leaderboard_layer->GetStateLB();
	if (stlb != -1)
	{
		return stlb;
	}
	else
	{
		int st = _taixiu_layer->GetStateTaiXiu();
		if (st != -1)
		{
			return st;
		}
	}
	return 5;
}

void CommonScreen::SetScreenComeFrom(BaseScreen * scr)
{
	p_screen_come_from = scr;
}

void CommonScreen::OnScreenComeFromEnable()
{
	if (!p_have_dialog_appear && !p_have_layout_interface && p_screen_come_from)
	{
		p_screen_come_from->SetLayerInteractive(true);
		p_screen_come_from->SetStopUpdate(false);
		p_screen_come_from = nullptr;
	}
}

void CommonScreen::OnSetScreenComeFrom(bool enable, bool disable_update)
{
	if (p_screen_come_from == nullptr)
	{
		p_screen_come_from = ScrMgr->GetMenuUI(ScrMgr->GetCurrentMenuUI());
	}

	p_screen_come_from->SetLayerInteractive(enable);
    if(disable_update)
    {
        p_screen_come_from->SetStopUpdate(!enable);
    }
}

void CommonScreen::SetHaveLayoutInteractive(bool value, int state)
{
	if (state == 0)
	{
		p_have_layout_interface = value;
	}
	else
	{
		p_have_dialog_appear = value;
	}
}

//announce server
void CommonScreen::OnReceiveAnnouncementFromServer(RKString text)
{
	_annouce_server_layer->OnReceiveAnnouncementFromServer(text);
}
void CommonScreen::ForceStopAnnouce(bool value)
{
	_annouce_server_layer->forceStopAnnouce(value);
}
//confirm code

void CommonScreen::OnShowConfirmCode()
{
	_confirm_code_layer->OnShowConfirmCode();
}

//change pass layer
void CommonScreen::OnShowChangePassLayout()
{
	_change_pass_layer->OnShowChangePassLayout();
}

//player info
bool CommonScreen::IsPlayerInfoVisible()
{
	return _player_info_layer->IsPlayerInfoVisible();
}

void CommonScreen::OnShowPlayerInfo(Player * player)
{
	_player_info_layer->OnShowPlayerInfo(player);
}

void CommonScreen::OnSetUserPlayer(User * user)
{
	_player_info_layer->OnSetUserPlayer(user);
}

void CommonScreen::OnReceiveAchievement(int id, void * list_achie)
{
	_player_info_layer->OnReceiveAchievement(id , list_achie);
}


//call popup method
void CommonScreen::OnShowDialog(RKString str_title,
	RKString str_message,
	TYPE_DIALOG type,
	const char * strCB,
	const std::function<void(const char *, const char*)> & func,
	const std::function<void(const char *, const char*)> & func1,
	const std::function<void(const char *, const char*)> & func2,
	RKString title_btn_0,
	RKString title_btn_1
)
{
	_dialog_layer->OnShowDialog(
		str_title,
		str_message,
		type,
		strCB,
		func,
		func1,
		func2,
		title_btn_0,
		title_btn_1
		);
}


void CommonScreen::OnHideDialog()
{
	_dialog_layer->OnHideDialog();
}

//lucky circle

void CommonScreen::LuckyCricleVisible(bool isShow)
{
	_lucky_screen->SetVisible(isShow);

}

void CommonScreen::OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_quest_list)
{
	_lucky_screen->OnRecieveLuckyCircleResultList(lucky_quest_list);
}

void CommonScreen::OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item)
{
	_lucky_screen->OnRecieveLuckyCircleResultItem(lucky_result_item);
}

//inivte banner here
void CommonScreen::OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar)
{
	_invite_layer->OnSetDataInvite(gameid, roomid, boardid, pass, message, idavatar);
}

//leaderboard
bool CommonScreen::LB_SetInteractive()
{
	return _leaderboard_layer->LB_SetInteractive();
}

void CommonScreen::LB_ResetLeaderboard(bool cleanup)
{
	_leaderboard_layer->LB_ResetLeaderboard(cleanup);
}

void CommonScreen::LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player)
{
	_leaderboard_layer->LB_OnReceiveListPlayer(id,page, list_player);
}

void CommonScreen::LB_ShowRichestWin()
{
	_leaderboard_layer->LB_ShowRichestWin();
}

void CommonScreen::LB_ShowRichestRuby()
{
	_leaderboard_layer->LB_ShowRichestRuby();
}

void CommonScreen::LB_ShowStrongest(ubyte current_game_id)
{
	_leaderboard_layer->LB_ShowStrongest(current_game_id);
}

//taixiu

void CommonScreen::ParseUserUI()
{
	_taixiu_layer->ParseUserUI();
}

void CommonScreen::OnShowBtnTaiXiu(bool show)
{
	_taixiu_layer->OnShowBtnTaiXiu(show);
	if (!show)
	{
		this->p_have_layout_interface = false;
		this->OnScreenComeFromEnable();
	}
}

bool CommonScreen::HaveTaixiuLayout()
{
	return _taixiu_layer->HaveTaixiuLayout();
}

void CommonScreen::OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type)
{
	_taixiu_layer->OnReceivedTaiXiuInfo(taixiu_info, type);
}

void CommonScreen::OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult)
{
	_taixiu_layer->OnReceivedTaiXiuBetResult(taiXiuBetResult);
}

void CommonScreen::OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList)
{
	_taixiu_layer->OnReceivedTaiXiuUserOrderList(taixiuOderList);
}

void CommonScreen::OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList)
{
	_taixiu_layer->OnReceivedTaiXiuUserHistoryList(taixiuHistoryList);
}

void CommonScreen::OnReceiveTransferMoney(s64 bonus_money, RKString info, int type)
{
	_taixiu_layer->OnReceiveTransferMoney(bonus_money , info , type);
}