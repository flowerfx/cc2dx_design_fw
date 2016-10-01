#ifndef __PROFILE_SCREEN_H__
#define __PROFILE_SCREEN_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
class Achievement;
class Item;
namespace iwinmesage
{
	class AvatarCat;
	class Avatar;
	class AvatarList;
}
class ProfileScreen : public BaseScreen
{
private:
	int p_previous_page_idx;

	WidgetEntity * p_top_panel;

	WidgetEntity * p_page_view_profile;

	WidgetEntity * p_page_profile_left;
	WidgetEntity * p_page_profile_name;
	WidgetEntity * p_page_view_achieve;
	WidgetEntity * p_page_trans_history;

	WidgetEntity * p_list_choose_sx;
	WidgetEntity * p_list_choose_day;
	WidgetEntity * p_list_choose_month;
	WidgetEntity * p_list_choose_year;

	WidgetEntity * p_panel_avatar;
	WidgetEntity * p_layout_edit_profile;


	std::vector<Achievement*> p_list_achieve;
	ThreadWrapper * p_thread_check_time;

	bool p_user_dirty;
	bool p_status_dirty;
	bool p_is_create_list_choose;

	int p_cur_day;
	int p_cur_month;
	int p_cur_year;

	cocos2d::Map<std::string, Sprite*>	p_list_avatar_download;
	WidgetEntity * p_shop_avatar_layout;
	WidgetEntity * p_shop_avatar_list_cat;
	WidgetEntity * p_shop_avatar_list;
	WidgetEntity * p_confirm_buy_avatar_layout;
	AvatarList   * p_avatar_list;
	int			   p_current_avatar_idx_buy;
	//determine when change state avatar cat to know download file or not;
	int			   p_stage_avatar_cat_list;

	WidgetEntity * p_layout_send_gift;
	WidgetEntity * p_textfield_gift_account;
	WidgetEntity * p_textfield_gift_number;
protected:
	void OnShowOrHideListChoose(WidgetEntity * list, bool Show);
	
	bool p_receive_data_achievement;

	void OnParseInfoAvaCatToLayout(WidgetEntity * widget, iwinmesage::AvatarCat * info);
	void OnParseInfoAvaToLayout(WidgetEntity * widget, iwinmesage::Avatar * info , bool request_avatar = false);
	void CleanUpListAvatar();

	void OnShowIdxBtnTopPanel(int idx);
public:
	ProfileScreen();
	virtual ~ProfileScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)		override;

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)						override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnTouchMenuBegin(cocos2d::Point p)											override;
	virtual void ParseUserUI()																override;

	void ShowLayoutState(int state);

	bool CheckValidateDay(int &day, int &month, int &year, bool recorrect = false);

	void OnRecorrectDateTime();
	void RunThreadCheckDateTime();
	void RunThreadParseLB();

	void CreateListDate();

	void onSetAchievement(void * data);

	void SetStatusPlayer(RKString status);
	void SetAccountName(RKString name);
	void SetPlayerName(RKString name);
	void SetSx(int id);
	void SetPhoneNumber(RKString number);
	void SetBirthDay(int day, int month, int year);
	void SetAddress(RKString address);
	void SetCMNDNumber(RKString number);
	void SetLevel(int level);
	void SetWinNumber(int number_win);
	void SetHourPlayed(RKString hour);
	void SetEmail(RKString email);
	void SetRubyNumber(int number_ruby);

	void UpdatePlayerName_ToServer(RKString name);
	void UpdateSx_ToServer(int id);
	void UpdatePhoneNumber_ToServer(RKString number);
	void UpdateBirthDay_ToServer(int day, int month, int year);
	void UpdateAddress_ToServer(RKString address);
	void UpdateCMNDNumber_ToServer(int number);
	void UpdateStatus_ToServer(RKString status);
	void UpdateEmail_ToServer(RKString email);

	void OnParseAchievementIntoGame(WidgetEntity* widget , Achievement * achie, int idx);
	void onReceiveAvatarCatList(void * data);
	void onReceiveAvatarList(void * data);
	void onBuyingAvatarResult(void * data);

	void onChangeMyAvatar();
	void onMyAvatars(std::vector<Item*> items);
	void onMyItems(std::vector<Item*> items, int cateId, int totalPage, ubyte gender);
	void onEventItems(std::vector<Item*> items);

	bool IsShopAvatarVisible();
};

#endif //__PROFILE_SCREEN_H__

