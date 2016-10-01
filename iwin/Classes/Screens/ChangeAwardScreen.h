#ifndef __CHANGE_AWARD_SCREEN_H__
#define __CHANGE_AWARD_SCREEN_H__

#include "ScreenManager.h"

using namespace iwinmesage;

namespace  iwinmesage
{
	class RubyToVNDInfo;
	class PaymentHistoryList;
	class CaptchaInfo;
	class CaptchaResult;
	class PaymentHistory;
	class CancelResult;
}
class CustomSelectedBox;
class IwinListView;
class SpriteIap;

class ChangeAwardScreen : public BaseScreen
{
private:
	WidgetEntity*			_telco_entity;
	WidgetEntity*			_price_entity;
	WidgetEntity*			_price_listview;
	WidgetEntity*			_price_item;
	WidgetEntity*			_captcha_text_field;

	CustomSelectedBox*		_telco_selected_box;
	CustomSelectedBox*		_price_selected_box;

	IwinListView*			_price_list;

	CaptchaInfo*			_cur_captcha;

	SpriteIap*				_spr_captcha;

	RubyToVNDInfo*			_rubyToVND;
	PaymentHistoryList*		_history_list;

	WidgetEntity*			_list_history;

	std::vector<PaymentHistory> _history_data;

public:
	ChangeAwardScreen();
	virtual ~ChangeAwardScreen();

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
	
	void updateSelectBoxTelco(int eventType, WidgetEntity* entity, int idx);
	void updateSelectBoxPrice(int eventType, WidgetEntity* entity, int idx);
	void setTelcoItem(WidgetEntity* entity, int idx);
	void setPriceItem(WidgetEntity* entity, int idx);
	void updateListviewItem(int eventType, WidgetEntity* entity, int idx);
	void updateItemHistory(WidgetEntity* entity, int idx);


	void updateUserRuby();
	void updateTotalRuby();

	void onReceiveRubyToCard(RubyToVNDInfo* robyToCard);
	void refreshHistory();
	void onReceiveListHistory(PaymentHistoryList* list);
	void onReceiveCancelCashout(CancelResult* cancel);
	void onReceiveCaptcha(CaptchaInfo info);
	void onReceiveCaptchaResult(CaptchaResult captcharesult);

	void updatePriceList();
	void updateCaptchaImage();

	void requestHistory();

	void chargeRuby();
	void showReward(bool visible);

};

#endif //__CHANGE_AWARD_SCREEN_H__

