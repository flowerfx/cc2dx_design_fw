#include "IAPScreen.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wScrollView.h"
#include "UI/AdvanceUI/wPanel.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"
#include "Common/IwinListView.h"
#include "Common/IwinScrollView.h"
#include "Common/FocusScrollView.h"
#include "Network/Core/JsonMessage.h"
#include "Network/JsonObject/Payment/Payment.h"
#include "Network/Global/Globalservice.h"
#include "Common/SpriteIap.h"
#include "Platform/Platform.h"

#include "Network/JsonObject/captcha/CaptchaInfo.h"
#include "Network/JsonObject/captcha/CaptchaResult.h"

using namespace iwinmesage;
const char* s_default_img_name = "test.png";

IAPScreen::IAPScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;

	p_panel_iap_method = nullptr;
	p_icon_arrow_backward = nullptr;
	p_icon_arrow_forward = nullptr;

	p_list_choose_iap_method = nullptr;
	_cur_captcha = nullptr;

}


IAPScreen::~IAPScreen()
{
	p_panel_iap_method = nullptr;
	p_icon_arrow_backward = nullptr;
	p_icon_arrow_forward = nullptr;

	p_list_choose_iap_method = nullptr;
	CC_SAFE_DELETE(_choose_payment_method);
	delete _cur_captcha;
	_cur_captcha = nullptr;
}

int IAPScreen::Init()
{
	RKString _menu = "iap_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);

	p_scroll_item = GetWidgetChildByName("iap_ui.scroll_item");
	p_scroll_item_card_value = GetWidgetChildByName("iap_ui.scroll_item_card_value");

	CallInitComponent(false);

	WidgetEntity* captcha_bounding = GetWidgetChildByName("iap_ui.layout_card.input_cards.img_captcha");
	_spr_captcha = SpriteIap::create(s_default_img_name, "", "");
	_spr_captcha->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_spr_captcha->setContentSize(cocos2d::Size(captcha_bounding->GetSize().x, captcha_bounding->GetSize().y));
	captcha_bounding->GetResource()->addChild(_spr_captcha);

	_scroll_view_iap = new IwinScrollView();
	_scroll_view_iap->init((ScrollViewWidget*)(p_scroll_view_iap), p_scroll_item, CC_CALLBACK_3(IAPScreen::updateItemScrollView, this), CC_CALLBACK_3(IAPScreen::initItemScrollView, this));
	_scroll_view_iap->setMaxItem(3);
	_scroll_view_banking = new IwinScrollView();
	_scroll_view_banking->init((ScrollViewWidget*)(p_scroll_view_banking), p_scroll_item, CC_CALLBACK_3(IAPScreen::updateItemScrollView, this), CC_CALLBACK_3(IAPScreen::initItemScrollView, this));
	_scroll_view_banking->setMaxItem(3);
	_scroll_view_sms = new IwinScrollView();
	_scroll_view_sms->init((ScrollViewWidget*)(p_scroll_view_sms), p_scroll_item, CC_CALLBACK_3(IAPScreen::updateItemScrollView, this), CC_CALLBACK_3(IAPScreen::initItemScrollView, this));
	_scroll_view_sms->setMaxItem(3);
	_scroll_view_value_card = new IwinScrollView();
	_scroll_view_value_card->init((ScrollViewWidget*)(p_scroll_view_value_card), p_scroll_item_card_value, CC_CALLBACK_3(IAPScreen::updateItemScrollViewValueCard, this), nullptr);
	_scroll_view_value_card->setMaxItem(1);

	initScrollViewTelco(_scroll_view_telco_sms, GetWidgetChildByName("iap_ui.layout_sms.scrollview_choose_telco"));
	initScrollViewTelco(_scroll_view_telco_card, GetWidgetChildByName("iap_ui.layout_card.scrollview_choose_telco"));


	return 1;
}

int	IAPScreen::InitComponent()
{
	p_icon_arrow_backward = GetWidgetChildByName("iap_ui.icon_arrow_backward");
	p_icon_arrow_forward = GetWidgetChildByName("iap_ui.icon_arrow_forward");

	p_panel_iap_method = GetWidgetChildByName("iap_ui.layout_iap_method");
	//p_list_choose_iap_method = p_panel_iap_method->GetWidgetChildByName(".list_choose_payment_method");
	p_scroll_item_telco = GetWidgetChildByName("iap_ui.scroll_item_telco");
	p_scroll_view_iap = GetWidgetChildByName("iap_ui.layout_iap.scrollview_big");
	p_scroll_view_banking = GetWidgetChildByName("iap_ui.layout_banking.scrollview_big");
	p_scroll_view_sms = GetWidgetChildByName("iap_ui.layout_sms.scrollview_small");
	p_scroll_view_value_card = GetWidgetChildByName("iap_ui.layout_card.input_cards.scrollview_card_value");
	p_scroll_view_telco_sms = GetWidgetChildByName("iap_ui.layout_sms.scrollview_choose_telco");
	p_scroll_view_telco_card = GetWidgetChildByName("iap_ui.layout_card.scrollview_choose_telco");

	p_group_button_win = GetWidgetChildByName("iap_ui.top_panel.group_win");
	p_group_button_ruby = GetWidgetChildByName("iap_ui.top_panel.group_ruby");
	p_group_button_win->GetWidgetChildByName(".icon_select")->SetVisible(true);
	p_group_button_ruby->GetWidgetChildByName(".icon_select")->SetVisible(false);

	StateShowArrow(0);

	_choose_payment_method = new IwinListView();
	_choose_payment_method->init((ListViewWidget*)(GetWidgetChildByName("iap_ui.layout_iap_method.list_choose_payment_method")), GetWidgetChildByName("iap_ui.layout_iap_method.payment_method_item"), CC_CALLBACK_3(IAPScreen::updateItemPaymentMethod, this));
	p_captcha_text_field = GetWidgetChildByName("iap_ui.layout_card.input_cards.panel_captcha_input.text_input_captcha");

	_payment_win = nullptr;
	_payment_ruby = nullptr;
	_payment = nullptr;
	_cur_type_payment = -1;
	//updateTelcoList();
	//initFake();

	//_list_right_layouts.push_back(GetWidgetChildByName("iap_ui.layout_sms"));
	//_list_right_layouts.push_back(GetWidgetChildByName("iap_ui.layout_topup"));


	//onPaymentInfo();
	//showCards();
	if (!_cur_captcha)
	{
		_cur_captcha = new CaptchaInfo();
	}
	return 1;
}

void IAPScreen::reload()
{
	CC_SAFE_DELETE(_choose_payment_method);
	Init();
}

void IAPScreen::initScrollViewTelco(FocusScrollView* &view, WidgetEntity* box)
{
	Vec2 pos = box->GetPosition();
	Vec2 size = box->GetSize();
	cocos2d::Rect bounding_box = cocos2d::Rect(pos.x, pos.y, size.x, size.y);
	view = FocusScrollView::create();
	view->setDirection(ScrollView::Direction::HORIZONTAL);
	view->setSize(cocos2d::Size(size.x, size.y));
	view->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	view->setPosition(Vec2::ZERO);
	view->setUpdateItemFunc(CC_CALLBACK_3(IAPScreen::updateItemScrollViewTelco, this));
	view->setInitItemFunc(CC_CALLBACK_3(IAPScreen::initItemScrollViewTelco, this));
	view->setChangeFocusFunc(std::bind(&IAPScreen::updateChangeFocusTelco, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	box->GetResource()->addChild(view);
	view->setItemEntity(p_scroll_item_telco);
	//p_scroll_item_telco->p_scroll_item_telcoi
}
void IAPScreen::initFake()
{
	/*Layout* layout_container = Layout::create();
	std::vector<WidgetEntity*> list_clone;
	Vec2 size = p_scroll_item->GetSize();
	const int padding = 10;
	int num_item = 10;
	for (auto i = 0; i < num_item; i++)
	{
		WidgetEntity* entity = p_scroll_item->Clone();
		list_clone.push_back(entity);
		entity->SetPosition(Vec2((size.x + padding) * floor(i % 3), (size.y + padding) * floor(i / 3)));
		entity->SetVisible(true);
		layout_container->addChild(entity->GetResource());
	}
	cocos2d::Size container_size = cocos2d::Size((size.x + padding)*3, (floor(num_item/3) + 1) * (size.y + padding));
	layout_container->setSize(container_size);
	static_cast<ScrollView*>(p_scroll_view_big->GetResource())->setInnerContainerSize(container_size);
	p_scroll_view_big->GetResource()->addChild(layout_container);*/
}

int IAPScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}
	return 1;
}

void IAPScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void IAPScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void IAPScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_back")
		{
			//ScrMgr->SwitchToMenu(p_menu_come_from, IAP_SCREEN);
			//ReloadUI("xml/screens/IAPScreen.xml");
			//reload();
			ScrMgr->ShowShop(false);
		}
		else if( name == "btn_payment_method")
		{
			if(_cur_type_payment != _widget->GetResource()->getTag()) showPayment(_widget->GetResource()->getTag());
		}
		else if (name == "buy_btn")
		{
			GlobalService::sendCheckCaptcha(_cur_captcha->getId(), static_cast<TextFieldWidget*>(p_captcha_text_field)->GetText().GetString());
		}
		else if (name == "btn_item_sms")
		{
			Payment* p = (Payment*)_widget->GetResource()->getUserData();
			ScrMgr->OnShowDialog(LangMgr->GetString("notice"), p->data->getConfirmSMS(), TYPE_DIALOG::DIALOG_TWO_BUTTON, "confirm_sms", [&](const char * call_back, const char* btn_name)
			{
			});
		}
		else if (name == "btn_item_iap")
		{

		}
		else if (name == "btn_item_banking")
		{

		}
		else if(name == "btn_win")
		{ 
			if (!_payment_win)
			{
				GlobalService::requestPaymentInfo("win");
			}
			else
			{
				_payment = _payment_win;
				reloadPayment();
			}
			p_group_button_win->GetWidgetChildByName(".icon_select")->SetVisible(true);
			p_group_button_ruby->GetWidgetChildByName(".icon_select")->SetVisible(false);
		}
		else if (name == "btn_ruby")
		{
			if (!_payment_ruby)
			{
				GlobalService::requestPaymentInfo("ruby");
			}
			else
			{
				_payment = _payment_ruby;
				reloadPayment();
			}
			p_group_button_win->GetWidgetChildByName(".icon_select")->SetVisible(false);
			p_group_button_ruby->GetWidgetChildByName(".icon_select")->SetVisible(true);
		}
		else if (name == "btn_telco")
		{
			int index = _widget->GetResource()->getTag();

			if (_payment_method_data.at(_cur_type_payment)->getCharge_type() == "sms")
			{
				_scroll_view_telco_sms->focus(index);
			}
			else
			{
				_scroll_view_telco_card->focus(index);
			}
		}
		else if (name == "btn_help")
		{
			if (_payment_win)
			{
				Platform::showWeb(_payment_win->getHelpLink());
			}
		}
		else if (name == "btn_change_award")
		{
			ScrMgr->ShowChangeAward(true);
		}
	}
	if (type_widget == UI_TYPE::UI_LISTVIEW)
	{
		if (name == "list_choose_payment_method_SCROLLING")
		{
		}
	}
}

void IAPScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(IAP_SCREEN);
	p_menu_show_next = MENU_NONE;
}


void IAPScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void IAPScreen::OnShowArrowAsList(WidgetEntity * _list)
{
}

void IAPScreen::StateShowArrow(int state)
{
}

void IAPScreen::updateItemPaymentMethod(int eventType, WidgetEntity* entity, int idx)
{
	entity->GetWidgetChildByName(".btn_payment_method")->GetResource()->setTag(idx);
	static_cast<Button*>(entity->GetWidgetChildByName(".btn_payment_method")->GetResource())->setTouchEnabled(true);
	static_cast<Button*>(entity->GetWidgetChildByName(".btn_payment_method")->GetResource())->setVisible(true);
	if (entity->GetWidgetChildByName(".icon_payment")->GetResource()->getChildByName("sprite_on") == nullptr)
	{
		Vec2 sizeBG = entity->GetWidgetChildByName(".icon_payment")->GetSize();
		p_panel_iap_method->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, entity->GetWidgetChildByName(".btn_payment_method"));
		std::string url, save_path;
		parseImageUrl(_payment_method_data.at(idx)->getIcon() + "_off.png", url, save_path);
		SpriteIap* sprite_off = SpriteIap::create(s_default_img_name, url.c_str(), save_path.c_str());
		sprite_off->setName("sprite_off");
		sprite_off->setContentSize(cocos2d::Size(sizeBG.x, sizeBG.y));
		sprite_off->startDownload();
		sprite_off->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		entity->GetWidgetChildByName(".icon_payment")->GetResource()->addChild(sprite_off);

		parseImageUrl(_payment_method_data.at(idx)->getIcon() + "_on.png", url, save_path);
		SpriteIap* sprite_on = SpriteIap::create(s_default_img_name, url.c_str(), save_path.c_str());
		sprite_on->setName("sprite_on");
		sprite_on->setContentSize(cocos2d::Size(sizeBG.x, sizeBG.y));
		sprite_on->startDownload();
		sprite_on->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		entity->GetWidgetChildByName(".icon_payment")->GetResource()->addChild(sprite_on);
		sprite_on->setVisible(false);
		sprite_on->setPosition(Vec2(sizeBG.x / 2, sizeBG.y / 2));
		sprite_off->setPosition(Vec2(sizeBG.x / 2, sizeBG.y / 2));
	}
	((TextWidget*)(entity->GetWidgetChildByName(".lbl_method_name")))->SetText(_payment_method_data.at(idx)->getTitle());
}

void IAPScreen::setupScrollItem(WidgetEntity* entity)
{

}

void IAPScreen::updateItemScrollViewValueCard(IwinScrollView* scrollview, WidgetEntity* entity, int idx)
{
	bool is_coin = _payment == _payment_win;
	entity->GetWidgetChildByName(".icon_coin")->SetVisible(is_coin);
	entity->GetWidgetChildByName(".icon_ruby")->SetVisible(!is_coin);

	int item_focus = _scroll_view_telco_card->getItemFocus();
	std::vector<Payment*> list = _telco_data_card[item_focus]->data->getItems();
	int win = list.at(idx)->getWin();
	int money = list.at(idx)->getMoney();

	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_value_card"))->SetText(StringUtils::format("%d VND", money));
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_win"))->SetText(StringUtils::format("%d", win));
}

void IAPScreen::initItemScrollView(IwinScrollView* scrollview, WidgetEntity* entity, int idx)
{
	p_panel_iap_method->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, entity->GetWidgetChildByName(".btn_item"));
	if (scrollview == _scroll_view_sms)
	{
		entity->GetWidgetChildByName(".btn_item")->SetName("btn_item_sms");
		entity->GetWidgetChildByName(".icon_card")->SetVisible(false);
	}
	else if (scrollview == _scroll_view_iap)
	{
		entity->GetWidgetChildByName(".btn_item")->SetName("btn_item_iap");
		entity->GetWidgetChildByName(".icon_card")->SetVisible(false);

	}
	else if (scrollview == _scroll_view_banking)
	{
		entity->GetWidgetChildByName(".btn_item")->SetName("btn_item_banking");
		entity->GetWidgetChildByName(".icon_card")->SetVisible(true);
	}
}

void IAPScreen::updateItemScrollView(IwinScrollView* scrollview, WidgetEntity* entity, int idx)
{
	bool win = _payment == _payment_win;
	entity->GetWidgetChildByName(".icon_coin_bundle")->SetVisible(win);
	entity->GetWidgetChildByName(".icon_coin")->SetVisible(win);
	entity->GetWidgetChildByName(".icon_ruby_bundle")->SetVisible(!win);
	entity->GetWidgetChildByName(".icon_ruby")->SetVisible(!win);
	entity->GetWidgetChildByName(".icon_khoa_taikhoan")->SetVisible(false);
	if (scrollview == _scroll_view_sms)
	{
		int item_focus = _scroll_view_telco_sms->getItemFocus();
		std::vector<Payment*> list = _telco_data_sms[item_focus]->data->getList();
		int win = list.at(idx)->data->getWin();
		int money = list.at(idx)->data->getMoney();
		entity->GetWidgetChildByName(".btn_item_sms")->GetResource()->setUserData(list.at(idx));
		if (win != 0)
		{
			static_cast<TextWidget*>(entity->GetWidgetChildByName(".top_lbl"))->SetText(StringUtils::format("%d", win));
		}
		else
		{
			entity->GetWidgetChildByName(".icon_khoa_taikhoan")->SetVisible(true);
			entity->GetWidgetChildByName(".icon_coin_bundle")->SetVisible(false);
			entity->GetWidgetChildByName(".icon_coin")->SetVisible(false);
			entity->GetWidgetChildByName(".icon_ruby_bundle")->SetVisible(false);
			entity->GetWidgetChildByName(".icon_ruby")->SetVisible(false);
			static_cast<TextWidget*>(entity->GetWidgetChildByName(".top_lbl"))->SetText(list.at(idx)->getTitle());
		}

		static_cast<TextWidget*>(entity->GetWidgetChildByName(".bot_lbl"))->SetText(StringUtils::format("%d VND", money));
	}
	else if (scrollview == _scroll_view_banking)
	{
		Payment* data = _telco_data_banking[idx]->data;
		int win = data->getWin();
		int money = data->getMoney();

		entity->GetWidgetChildByName(".btn_item_banking")->GetResource()->setUserData(data);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".top_lbl"))->SetText(StringUtils::format("%d", win));
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".bot_lbl"))->SetText(StringUtils::format("%d VND", money));
		if (entity->GetWidgetChildByName(".icon_card")->GetResource()->getChildByName("icon") == nullptr)
		{
			std::string url, save_path;
			parseImageUrl(_telco_data_banking[idx]->getIcon(), url, save_path);
			SpriteIap* icon = SpriteIap::create(s_default_img_name, url.c_str(), save_path.c_str());
			icon->setName("icon");
			icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
			entity->GetWidgetChildByName(".icon_card")->GetResource()->addChild(icon);
			Vec2 layout_size = entity->GetWidgetChildByName(".icon_card")->GetSize();
			icon->startDownload();
			icon->setPosition(Vec2(layout_size.x / 2, .0f));
		}
	}
	else if (scrollview == _scroll_view_iap)
	{
		Payment* data = _telco_data_iap[idx]->data;
		int win = data->getWin();
		int money = data->getMoney();

		entity->GetWidgetChildByName(".btn_item_iap")->GetResource()->setUserData(data);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".top_lbl"))->SetText(StringUtils::format("%d", win));
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".bot_lbl"))->SetText(StringUtils::format("%f VND", money));
	}
}

void IAPScreen::updateItemScrollViewCard(WidgetEntity* entity, int idx)
{

}

void IAPScreen::initItemScrollViewTelco(FocusScrollView* scrollview, WidgetEntity* entity, int idx)
{
	p_panel_iap_method->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, entity->GetWidgetChildByName(".btn_telco"));
}

void IAPScreen::updateItemScrollViewTelco(FocusScrollView* scrollview, WidgetEntity* entity, int idx)
{
	Vec2 item_size = entity->GetWidgetChildByName(".icon_telco")->GetSize();
	Node* icon_telco = entity->GetWidgetChildByName(".icon_telco")->GetResource()->getChildByName("icon");
	entity->GetWidgetChildByName(".btn_telco")->GetResource()->setTag(idx);
	if (icon_telco == nullptr)
	{
		std::string url, save_path;
		if(scrollview == _scroll_view_telco_sms)
			parseImageUrl(_telco_data_sms.at(idx)->getIcon(), url, save_path);
		else
			parseImageUrl(_telco_data_card.at(idx)->getIcon(), url, save_path);
		SpriteIap* icon = SpriteIap::create(s_default_img_name, url.c_str(), save_path.c_str());
		icon->setName("icon");
		icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		entity->GetWidgetChildByName(".icon_telco")->GetResource()->addChild(icon);
		icon->setContentSize(cocos2d::Size(item_size.x, item_size.y));
		icon->setPosition(Vec2(item_size.x / 2, item_size.y / 2));
		icon->startDownload();
	}
	else
	{

	}
}


void IAPScreen::updateChangeFocusTelco(FocusScrollView* scrollview, WidgetEntity* entity, bool focus, int focus_idx)
{
	entity->GetWidgetChildByName(".hl_telco")->SetVisible(focus);
	Node* icon = entity->GetWidgetChildByName(".icon_telco")->GetResource()->getChildByName("icon");
	if (icon)
	{
		float scale = icon->getScale();
		cocos2d::Size icon_size = icon->getContentSize();
		entity->GetWidgetChildByName(".hl_telco")->SetSize(Vec2(icon_size.width * scale + 10, icon_size.height * scale + 10));
	}

		
	if (focus)
	{
		if (scrollview == _scroll_view_telco_sms)
		{
			reloadSms(focus_idx);
		}
		else
		{
			reloadCard(focus_idx);
		}
	}


}

void IAPScreen::showSms()
{
	GetWidgetChildByName("iap_ui.layout_sms")->SetVisible(true);
	GetWidgetChildByName("iap_ui.layout_card")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_banking")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_iap")->SetVisible(false);
	reloadTelcoListWithSms();
}

void IAPScreen::showBanking()
{
	GetWidgetChildByName("iap_ui.layout_sms")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_card")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_banking")->SetVisible(true);
	GetWidgetChildByName("iap_ui.layout_iap")->SetVisible(false);
	reloadBanking();
}

void IAPScreen::showCards()
{
	GetWidgetChildByName("iap_ui.layout_sms")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_card")->SetVisible(true);
	GetWidgetChildByName("iap_ui.layout_banking")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_iap")->SetVisible(false);
	reloadTelcoListWithCard();
}

void IAPScreen::showIap()
{
	GetWidgetChildByName("iap_ui.layout_sms")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_card")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_banking")->SetVisible(false);
	GetWidgetChildByName("iap_ui.layout_iap")->SetVisible(true);
	reloadIap();
}

void IAPScreen::onPaymentInfo()
{
	//const char* json = "{\"command\":\"REQUEST_PAYMENT_INFOR\",\"data\":{\"title\":\"Phương thức nạp win\",\"desc\":\"Vui lòng chọn phương thức nạp\",\"type\":\"list\",\"charge_type\":\"\",\"icon\":\"\",\"data\":{\"list\":[{\"title\":\"Nạp SMS Topup\",\"desc\":\"Gửi SMS để nạp WIN\",\"type\":\"list\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/sms\",\"data\":{\"list\":[{\"title\":\"Topup Viettel\",\"desc\":\"Topup Viettel\",\"type\":\"list\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\",\"data\":{\"list\":[{\"title\":\"SMS 50000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP50 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":50000,\"win\":5000000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 5000000 WIN với phí 50000 VND ?\"}},{\"title\":\"SMS 30000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP30 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":30000,\"win\":2550000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 2550000 WIN với phí 30000 VND ?\"}},{\"title\":\"SMS 20000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP20 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":20000,\"win\":1600000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 1600000 WIN với phí 20000 VND ?\"}},{\"title\":\"SMS 15000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP15 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":15000,\"win\":1050000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 1050000 WIN với phí 15000 VND ?\"}},{\"title\":\"SMS 10000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP10 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":10000,\"win\":650000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 650000 WIN với phí 10000 VND ?\"}},{\"title\":\"Khoá tài khoản\",\"desc\":\"Khoá tài khoản\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"WIN KHOA test226 \",\"phone\":\"7065\",\"money\":1000,\"win\":0,\"win_promotion\":0,\"charge_by_id\":0}}]}},{\"title\":\"Topup Vinaphone\",\"desc\":\"Topup Vinaphone\",\"type\":\"list\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\",\"data\":{\"list\":[{\"title\":\"SMS 50000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP50 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":50000,\"win\":5000000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 5000000 WIN với phí 50000 VND ?\"}},{\"title\":\"SMS 30000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP30 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":30000,\"win\":2550000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 2550000 WIN với phí 30000 VND ?\"}},{\"title\":\"SMS 20000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP20 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":20000,\"win\":1600000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 1600000 WIN với phí 20000 VND ?\"}},{\"title\":\"SMS 15000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP15 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":15000,\"win\":1050000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 1050000 WIN với phí 15000 VND ?\"}},{\"title\":\"SMS 10000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP10 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":10000,\"win\":650000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 650000 WIN với phí 10000 VND ?\"}},{\"title\":\"Khoá tài khoản\",\"desc\":\"Khoá tài khoản\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"WIN KHOA test226 \",\"phone\":\"7065\",\"money\":1000,\"win\":0,\"win_promotion\":0,\"charge_by_id\":0}}]}},{\"title\":\"Topup Mobifone\",\"desc\":\"Topup Mobifone\",\"type\":\"list\",\"charge_type\":\"sms\",\"icon\": \
	//	\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\",\"data\":{\"list\":[{\"title\":\"SMS 100000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP100 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":100000,\"win\":11500000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 11500000 WIN với phí 100000 VND ?\"}},{\"title\":\"SMS 50000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP50 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":50000,\"win\":5000000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 5000000 WIN với phí 50000 VND ?\"}},{\"title\":\"SMS 30000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP30 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":30000,\"win\":2550000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 2550000 WIN với phí 30000 VND ?\"}},{\"title\":\"SMS 20000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP20 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":20000,\"win\":1600000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 1600000 WIN với phí 20000 VND ?\"}},{\"title\":\"SMS 15000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP15 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":15000,\"win\":1050000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 1050000 WIN với phí 15000 VND ?\"}},{\"title\":\"SMS 10000 đ\",\"desc\":\"Hoặc soạn tin nhắn me iwin NAP test226 gửi 9029\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"me iwin NAP10 G1P0 test226 id160615sRaxYnZ4u \",\"phone\":\"9029\",\"money\":10000,\"win\":650000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 650000 WIN với phí 10000 VND ?\"}},{\"title\":\"Khoá tài khoản\",\"desc\":\"Khoá tài khoản\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"WIN KHOA test226 \",\"phone\":\"7065\",\"money\":1000,\"win\":0,\"win_promotion\":0,\"charge_by_id\":0}}]}},{\"title\":\"Nạp SMS\",\"desc\":\"Gửi SMS để nạp WIN\",\"type\":\"list\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"list\":[{\"title\":\"Kích hoạt tài khoản\",\"desc\":\"Kích hoạt tài khoản\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"win ACTIVE test226 c7f07690f8 id160615sRaxYnZ4u me \",\"phone\":\"7065\",\"money\":1000,\"win\":0,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn kích hoạt tài khoản với phí 1000 VND\"}},{\"title\":\"SMS 10000 đ\",\"desc\":\"Hoặc soạn tin nhắn win NAP test226 gửi 7665\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"win NAP test226 id160615sRaxYnZ4u me \",\"phone\":\"7665\",\"money\":10000,\"win\":550000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 110000 WIN với phí 10000 VND ?\"}},{\"title\":\"SMS 15000 đ\",\"desc\":\"Hoặc soạn tin nhắn win NAP test226 gửi 7765\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"win NAP test226 id160615sRaxYnZ4u me \",\"phone\":\"7765\",\"money\":15000,\"win\":900000,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Bạn có muốn gửi SMS nạp 180000 WIN với phí 15000 VND ?\"}},{\"title\":\"Khoá tài khoản\",\"desc\":\"Khoá tài khoản\",\"type\":\"action\",\"charge_type\":\"sms\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_sms.png\",\"data\":{\"content\":\"WIN KHOA test226 \",\"phone\":\"7065\",\"money\":1000,\"win\":0,\"win_promotion\":0,\"charge_by_id\":0,\"confirmSMS\":\"Khóa tài khoản, gửi SMS phí 1000 VND\"}}]},\"unit\":\"win\"}]},\"unit\":\"win\"},{\"title\":\"Nạp thẻ\",\"desc\":\"Chọn loại thẻ tương ứng muốn nạp\",\"type\":\"list\",\"charge_type\":\"input_2_field\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card\",\"data\":{\"list\":[{\"title\":\"Nạp thẻ mobifone\",\"desc\":\"Nạp thẻ mobifone\",\"type\":\"action\",\"charge_type\":\"input_2_field\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\",\"data\":{\"charge_id\":\"vms\",\"field_1_name\":\"Serial\",\"field_2_name\":\"Mã thẻ\",\"field_1_desc\":\"Vui lòng nhập số serial\",\"field_2_desc\":\"Vui lòng nhập mã thẻ\",\"charge_by_subid\":2,\"charge_by_id\":2,\"items\":[{\"type\":\"card\",\"card\":\"vms\",\"message\":\"Nạp thẻ Mobifone\",\"money\":10000,\"win\":1000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\"},{\"type\":\"card\",\"card\":\"vms\",\"message\":\"Nạp thẻ Mobifone\",\"money\":20000,\"win\":2000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\"},{\"type\":\
	//	\"card\",\"card\":\"vms\",\"message\":\"Nạp thẻ Mobifone\",\"money\":50000,\"win\":5500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\"},{\"type\":\"card\",\"card\":\"vms\",\"message\":\"Nạp thẻ Mobifone\",\"money\":100000,\"win\":12500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\"},{\"type\":\"card\",\"card\":\"vms\",\"message\":\"Nạp thẻ Mobifone\",\"money\":200000,\"win\":25000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\"},{\"type\":\"card\",\"card\":\"vms\",\"message\":\"Nạp thẻ Mobifone\",\"money\":300000,\"win\":37500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\"},{\"type\":\"card\",\"card\":\"vms\",\"message\":\"Nạp thẻ Mobifone\",\"money\":500000,\"win\":62500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_mobi.png\"}]}},{\"title\":\"Nạp thẻ vinaphone\",\"desc\":\"Nạp thẻ vinaphone\",\"type\":\"action\",\"charge_type\":\"input_2_field\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\",\"data\":{\"charge_id\":\"vina\",\"field_1_name\":\"Serial\",\"field_2_name\":\"Mã thẻ\",\"field_1_desc\":\"Vui lòng nhập số serial\",\"field_2_desc\":\"Vui lòng nhập mã thẻ\",\"charge_by_subid\":8,\"charge_by_id\":2,\"items\":[{\"type\":\"card\",\"card\":\"vina\",\"message\":\"Nạp thẻ Vinaphone\",\"money\":10000,\"win\":1000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\"},{\"type\":\"card\",\"card\":\"vina\",\"message\":\"Nạp thẻ Vinaphone\",\"money\":20000,\"win\":2000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\"},{\"type\":\"card\",\"card\":\"vina\",\"message\":\"Nạp thẻ Vinaphone\",\"money\":50000,\"win\":5500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\"},{\"type\":\"card\",\"card\":\"vina\",\"message\":\"Nạp thẻ Vinaphone\",\"money\":100000,\"win\":12500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\"},{\"type\":\"card\",\"card\":\"vina\",\"message\":\"Nạp thẻ Vinaphone\",\"money\":200000,\"win\":25000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\"},{\"type\":\"card\",\"card\":\"vina\",\"message\":\"Nạp thẻ Vinaphone\",\"money\":300000,\"win\":37500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\"},{\"type\":\"card\",\"card\":\"vina\",\"message\":\"Nạp thẻ Vinaphone\",\"money\":500000,\"win\":62500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vinaphone.png\"}]}},{\"title\":\"Nạp thẻ viettel\",\"desc\":\"Nạp thẻ viettel\",\"type\":\"action\",\"charge_type\":\"input_2_field\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\",\"data\":{\"charge_id\":\"viettel\",\"field_1_name\":\"Serial\",\"field_2_name\":\"Mã thẻ\",\"field_1_desc\":\"Vui lòng nhập số serial\",\"field_2_desc\":\"Vui lòng nhập mã thẻ\",\"charge_by_subid\":5,\"charge_by_id\":2,\"items\":[{\"type\":\"card\",\"card\":\"viettel\",\"message\":\"Nạp thẻ Viettel\",\"money\":10000,\"win\":1000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\"},{\"type\":\"card\",\"card\":\"viettel\",\"message\":\"Nạp thẻ Viettel\",\"money\":20000,\"win\":2000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\"},{\"type\":\"card\",\"card\":\"viettel\",\"message\":\"Nạp thẻ Viettel\",\"money\":50000,\"win\":5500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\"},{\"type\":\"card\",\"card\":\"viettel\",\"message\":\"Nạp thẻ Viettel\",\"money\":100000,\"win\":12500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\"},{\"type\":\"card\",\"card\":\"viettel\",\"message\":\"Nạp thẻ Viettel\",\"money\":200000,\"win\":25000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\"},{\"type\":\"card\",\"card\":\"viettel\",\"message\":\"Nạp thẻ Viettel\",\"money\":300000,\"win\":37500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\"},{\"type\":\"card\",\"card\":\"viettel\",\"message\":\"Nạp thẻ Viettel\",\"money\":500000,\"win\":62500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_viettel.png\"}]}},{\"title\":\"Nạp thẻ gate\",\"desc\":\"Nạp thẻ gate\",\"type\":\"action\",\"charge_type\":\"input_2_field\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\",\"data\":{\"charge_id\":\"gate\",\"field_1_name\":\"Serial\",\"field_2_name\":\"Mã thẻ\",\"field_1_desc\":\"Vui lòng nhập số serial\",\"field_2_desc\":\"Vui lòng nhập mã thẻ\",\"charge_by_subid\":1,\"charge_by_id\":2,\"items\":[{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":10000,\"win\":1100000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":20000,\"win\":2200000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":50000,\"win\":5900000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":100000,\"win\":13125000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\
	//	\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":200000,\"win\":26250000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":500000,\"win\":65625000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":1000000,\"win\":125000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":2000000,\"win\":250000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"},{\"type\":\"card\",\"card\":\"gate\",\"message\":\"Nạp thẻ Gate\",\"money\":5000000,\"win\":625000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_gate.png\"}]}},{\"title\":\"Nạp thẻ bit\",\"desc\":\"Nạp thẻ bit\",\"type\":\"action\",\"charge_type\":\"input_2_field\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\",\"data\":{\"charge_id\":\"bit\",\"field_1_name\":\"Serial\",\"field_2_name\":\"Mã thẻ\",\"field_1_desc\":\"Vui lòng nhập số serial\",\"field_2_desc\":\"Vui lòng nhập mã thẻ\",\"charge_by_subid\":11,\"charge_by_id\":2,\"items\":[{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":10000,\"win\":1000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":20000,\"win\":2000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":50000,\"win\":5500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":100000,\"win\":12500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":200000,\"win\":25000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":500000,\"win\":62500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":1000000,\"win\":125000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":2000000,\"win\":250000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"},{\"type\":\"card\",\"card\":\"bit\",\"message\":\"Nạp thẻ BIT\",\"money\":5000000,\"win\":625000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_bitcard.png\"}]}},{\"title\":\"Nạp thẻ vnm\",\"desc\":\"Nạp thẻ vnm\",\"type\":\"action\",\"charge_type\":\"input_2_field\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\",\"data\":{\"charge_id\":\"vnm\",\"field_1_name\":\"Serial\",\"field_2_name\":\"Mã thẻ\",\"field_1_desc\":\"Vui lòng nhập số serial\",\"field_2_desc\":\"Vui lòng nhập mã thẻ\",\"charge_by_subid\":12,\"charge_by_id\":2,\"items\":[{\"type\":\"card\",\"card\":\"vnm\",\"message\":\"Nạp thẻ VietNam Mobile\",\"money\":10000,\"win\":1000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\"},{\"type\":\"card\",\"card\":\"vnm\",\"message\":\"Nạp thẻ VietNam Mobile\",\"money\":20000,\"win\":2000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\"},{\"type\":\"card\",\"card\":\"vnm\",\"message\":\"Nạp thẻ VietNam Mobile\",\"money\":50000,\"win\":5500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\"},{\"type\":\"card\",\"card\":\"vnm\",\"message\":\"Nạp thẻ VietNam Mobile\",\"money\":100000,\"win\":12500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\"},{\"type\":\"card\",\"card\":\"vnm\",\"message\":\"Nạp thẻ VietNam Mobile\",\"money\":200000,\"win\":25000000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\"},{\"type\":\"card\",\"card\":\"vnm\",\"message\":\"Nạp thẻ VietNam Mobile\",\"money\":300000,\"win\":37500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\"},{\"type\":\"card\",\"card\":\"vnm\",\"message\":\"Nạp thẻ VietNam Mobile\",\"money\":500000,\"win\":62500000,\"win_promotion\":0,\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/thecao_5loaithe_logo_vietnamobile.png\"}]}}]},\"unit\":\"win\"},{\"title\":\"Nạp inapp\",\"desc\":\"Chọn mệnh giá nạp\",\"type\":\"list\",\"charge_type\":\"apple_macos_app_store\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/shop\",\"data\":{\"list\":[{\"title\":\"Nạp 0.99$ được 420000 WIN\",\"desc\":\"\",\"type\":\"action\",\"charge_type\":\"apple_macos_app_store\",\"data\":{\"money\":0.99,\"code\":\"vn.mecorp.mac.iwin.1\",\"win\":2520000,\"win_promotion\":0,\"charge_by_id\":5}},{\"title\":\"Nạp 4.99$ được 2100000 WIN\",\"desc\":\"\",\"type\":\"action\",\"charge_type\":\"apple_macos_app_store\",\"data\":{\"money\":4.99,\"code\":\"vn.mecorp.mac.iwin.5\",\"win\":12600000,\"win_promotion\":0,\"charge_by_id\":5}},{\"title\":\"Nạp 9.99$ được 4620000 WIN\",\"desc\":\"\",\"type\":\"action\",\
	//	\"charge_type\":\"apple_macos_app_store\",\"data\":{\"money\":9.99,\"code\":\"vn.mecorp.mac.iwin.10\",\"win\":27720000,\
	//	\"win_promotion\":0,\"charge_by_id\":5}},{\"title\":\"Nạp 49.99$ được 26250000 WIN\",\"desc\":\"\",\"type\":\"action\",\"charge_type\":\"apple_macos_app_store\",\"data\":{\"money\":49.99,\"code\":\"vn.mecorp.mac.iwin.50\",\"win\":157500000,\"win_promotion\":0,\"charge_by_id\":5}}]},\"unit\":\"win\"},{\"title\":\"Nạp BANKING\",\"desc\":\"Nạp Win thông qua cổng BANKING online\",\"type\":\"list\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/bank\",\"data\":{\"list\":[{\"title\":\"Banking 50k\",\"desc\":\"Test\",\"type\":\"action\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card_coin_bank.png\",\"data\":{\"link\":\"http://bank.mopay.vn/?data=eyJhbW91bnQiOjUwMDAwLCJ1bml0Ijoid2luIiwicGFydG5lcl9pZCI6MTAwOCwidXNlcm5hbWUiOiJ0ZXN0MjI2Iiwic3VjY2Vzc191cmwiOiJodHRwOi8vYmFua2luZy5pd2luLnZuL3N1Y2Nlc3MiLCJkYXRhIjoiZXlKMWMyVnlibUZ0WlNJNkluUmxjM1F5TWpZaUxDSmphR0Z5WVdOMFpYSWlPaUowWlhOME1qSTJJaXdpY0hKdmRtbGtaWElpT2pBc0luSmxabU52WkdVaU9pSWlMQ0poWjJWdWRDSTZJa1JsYzJ0MGIzQTdUV0ZqSUU5VElGZzdNQ0lzSW5abGNuTnBiMjRpT2lJMUxqQXVNQ0lzSW1sd0lqb2lNVEF1T0M0eE5DNHhPRGNpTENKallYSnlhV1Z5SWpvaUlpd2ljR3hoZEdadmNtMGlPaUp0WVdNaUxDSmphR0Z1Ym1Wc0lqb2lNSHh0Wlh3MUxqQXVNQ0o5IiwicGFydG5lcl90cmFuc19pZCI6IjE2MDYxNXNVVXdDMk5oaSIsImZhaWxfdXJsIjoiaHR0cDovL2JhbmtpbmcuaXdpbi52bi9mYWlsIiwiYmFua190eXBlIjoxfQ==&token=4b125500597d3de98458166bb29e3a6d\",\"money\":50000,\"win\":8125000,\"win_promotion\":0}},{\"title\":\"Banking 100k\",\"desc\":\"Test\",\"type\":\"action\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card_coin_bank.png\",\"data\":{\"link\":\"http://bank.mopay.vn/?data=eyJhbW91bnQiOjEwMDAwMCwidW5pdCI6IndpbiIsInBhcnRuZXJfaWQiOjEwMDgsInVzZXJuYW1lIjoidGVzdDIyNiIsInN1Y2Nlc3NfdXJsIjoiaHR0cDovL2JhbmtpbmcuaXdpbi52bi9zdWNjZXNzIiwiZGF0YSI6ImV5SjFjMlZ5Ym1GdFpTSTZJblJsYzNReU1qWWlMQ0pqYUdGeVlXTjBaWElpT2lKMFpYTjBNakkySWl3aWNISnZkbWxrWlhJaU9qQXNJbkpsWm1OdlpHVWlPaUlpTENKaFoyVnVkQ0k2SWtSbGMydDBiM0E3VFdGaklFOVRJRmc3TUNJc0luWmxjbk5wYjI0aU9pSTFMakF1TUNJc0ltbHdJam9pTVRBdU9DNHhOQzR4T0RjaUxDSmpZWEp5YVdWeUlqb2lJaXdpY0d4aGRHWnZjbTBpT2lKdFlXTWlMQ0pqYUdGdWJtVnNJam9pTUh4dFpYdzFMakF1TUNKOSIsInBhcnRuZXJfdHJhbnNfaWQiOiIxNjA2MTVzVVV3QzJOaGkiLCJmYWlsX3VybCI6Imh0dHA6Ly9iYW5raW5nLml3aW4udm4vZmFpbCIsImJhbmtfdHlwZSI6MX0=&token=0806d58c26bfb5bc08abd1096ddef8a1\",\"money\":100000,\"win\":16250000,\"win_promotion\":0}},{\"title\":\"Banking 200k\",\"desc\":\"Test\",\"type\":\"action\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card_coin_bank.png\",\"data\":{\"link\":\"http://bank.mopay.vn/?data=eyJhbW91bnQiOjIwMDAwMCwidW5pdCI6IndpbiIsInBhcnRuZXJfaWQiOjEwMDgsInVzZXJuYW1lIjoidGVzdDIyNiIsInN1Y2Nlc3NfdXJsIjoiaHR0cDovL2JhbmtpbmcuaXdpbi52bi9zdWNjZXNzIiwiZGF0YSI6ImV5SjFjMlZ5Ym1GdFpTSTZJblJsYzNReU1qWWlMQ0pqYUdGeVlXTjBaWElpT2lKMFpYTjBNakkySWl3aWNISnZkbWxrWlhJaU9qQXNJbkpsWm1OdlpHVWlPaUlpTENKaFoyVnVkQ0k2SWtSbGMydDBiM0E3VFdGaklFOVRJRmc3TUNJc0luWmxjbk5wYjI0aU9pSTFMakF1TUNJc0ltbHdJam9pTVRBdU9DNHhOQzR4T0RjaUxDSmpZWEp5YVdWeUlqb2lJaXdpY0d4aGRHWnZjbTBpT2lKdFlXTWlMQ0pqYUdGdWJtVnNJam9pTUh4dFpYdzFMakF1TUNKOSIsInBhcnRuZXJfdHJhbnNfaWQiOiIxNjA2MTVzVVV3QzJOaGkiLCJmYWlsX3VybCI6Imh0dHA6Ly9iYW5raW5nLml3aW4udm4vZmFpbCIsImJhbmtfdHlwZSI6MX0=&token=43707474d3890a6419c9b57e9de9b170\",\"money\":200000,\"win\":32500000,\"win_promotion\":0}},{\"title\":\"Banking 500k\",\"desc\":\"Test\",\"type\":\"action\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card_coin_bank.png\",\"data\":{\"link\":\"http://bank.mopay.vn/?data=eyJhbW91bnQiOjUwMDAwMCwidW5pdCI6IndpbiIsInBhcnRuZXJfaWQiOjEwMDgsInVzZXJuYW1lIjoidGVzdDIyNiIsInN1Y2Nlc3NfdXJsIjoiaHR0cDovL2JhbmtpbmcuaXdpbi52bi9zdWNjZXNzIiwiZGF0YSI6ImV5SjFjMlZ5Ym1GdFpTSTZJblJsYzNReU1qWWlMQ0pqYUdGeVlXTjBaWElpT2lKMFpYTjBNakkySWl3aWNISnZkbWxrWlhJaU9qQXNJbkpsWm1OdlpHVWlPaUlpTENKaFoyVnVkQ0k2SWtSbGMydDBiM0E3VFdGaklFOVRJRmc3TUNJc0luWmxjbk5wYjI0aU9pSTFMakF1TUNJc0ltbHdJam9pTVRBdU9DNHhOQzR4T0RjaUxDSmpZWEp5YVdWeUlqb2lJaXdpY0d4aGRHWnZjbTBpT2lKdFlXTWlMQ0pqYUdGdWJtVnNJam9pTUh4dFpYdzFMakF1TUNKOSIsInBhcnRuZXJfdHJhbnNfaWQiOiIxNjA2MTVzVVV3QzJOaGkiLCJmYWlsX3VybCI6Imh0dHA6Ly9iYW5raW5nLml3aW4udm4vZmFpbCIsImJhbmtfdHlwZSI6MX0=&token=60856e9674f8d7989b985fd029012289\",\"money\":500000,\"win\":81250000,\"win_promotion\":0}},{\"title\":\"Banking 1000k\",\"desc\":\"Test\",\"type\":\"action\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card_coin_bank.png\",\"data\":{\"link\":\"http://bank.mopay.vn/?data=eyJhbW91bnQiOjEwMDAwMDAsInVuaXQiOiJ3aW4iLCJwYXJ0bmVyX2lkIjoxMDA4LCJ1c2VybmFtZSI6InRlc3QyMjYiLCJzdWNjZXNzX3VybCI6Imh0dHA6Ly9iYW5raW5nLml3aW4udm4vc3VjY2VzcyIsImRhdGEiOiJleUoxYzJWeWJtRnRaU0k2SW5SbGMzUXlNallpTENKamFHRnlZV04wWlhJaU9pSjBaWE4wTWpJMklpd2ljSEp2ZG1sa1pYSWlPakFzSW5KbFptTnZaR1VpT2lJaUxDSmhaMlZ1ZENJNklrUmxjMnQwYjNBN1RXRmpJRTlUSUZnN01DSXNJblpsY25OcGIyNGlPaUkxTGpBdU1DSXNJbWx3SWpvaU1UQXVPQzR4TkM0eE9EY2lMQ0pqWVhKeWFXVnlJam9pSWl3aWNHeGhkR1p2Y20waU9pSnRZV01pTENKamFHRnVibVZzSWpvaU1IeHRaWHcxTGpBdU1DSjkiLCJwYXJ0bmVyX3RyYW5zX2lkIjoiMTYwNjE1c1VVd0MyTmhpIiwiZmFpbF91cmwiOiJodHRwOi8vYmFua2luZy5pd2luLnZuL2ZhaWwiLCJiYW5rX3R5cGUiOjF9&token=68f3a3f8924e7e772aa7e8c540fd3100\",\"money\":1000000,\"win\":162500000,\"win_promotion\":0}},{\"title\":\"Banking 2000k\",\"desc\":\"Test\",\"type\":\"action\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card_coin_bank.png\",\"data\":{\"link\":\"http://bank.mopay.vn/?data=eyJhbW91bnQiOjIwMDAwMDAsInVuaXQiOiJ3aW4iLCJwYXJ0bmVyX2lkIjoxMDA4LCJ1c2VybmFtZSI6InRlc3QyMjYiLCJzdWNjZXNzX3VybCI6Imh0dHA6Ly9iYW5raW5nLml3aW4udm4vc3VjY2VzcyIsImRhdGEiOiJleUoxYzJWeWJtRnRaU0k2SW5SbGMzUXlNallpTENKamFHRnlZV04wWlhJaU9pSjBaWE4wTWpJMklpd2ljSEp2ZG1sa1pYSWlPakFzSW5KbFptTnZaR1VpT2lJaUxDSmhaMlZ1ZENJNklrUmxjMnQwYjNBN1RXRmpJRTlUSUZnN01DSXNJblpsY25OcGIyNGlPaUkxTGpBdU1DSXNJbWx3SWpvaU1UQXVPQzR4TkM0eE9EY2lMQ0pqWVhKeWFXVnlJam9pSWl3aWNHeGhkR1p2Y20waU9pSnRZV01pTENKamFHRnVibVZzSWpvaU1IeHRaWHcxTGpBdU1DSjkiLCJwYXJ0bmVyX3RyYW5zX2lkIjoiMTYwNjE1c1VVd0MyTmhpIiwiZmFpbF91cmwiOiJodHRwOi8vYmFua2luZy5pd2luLnZuL2ZhaWwiLCJiYW5rX3R5cGUiOjF9&token=696675a10f901210b079b270c9e57246\",\"money\":2000000,\"win\":325000000,\"win_promotion\":0}},{\"title\":\"Banking 5000k\",\"desc\":\"Test\",\"type\":\"action\",\"charge_type\":\"web\",\"icon\":\"http://av.iwin.vn/wf/icon_nap2/***/card_coin_bank.png\",\"data\":{\"link\":\"http://bank.mopay.vn/?data=eyJhbW91bnQiOjUwMDAwMDAsInVuaXQiOiJ3aW4iLCJwYXJ0bmVyX2lkIjoxMDA4LCJ1c2VybmFtZSI6InRlc3QyMjYiLCJzdWNjZXNzX3VybCI6Imh0dHA6Ly9iYW5raW5nLml3aW4udm4vc3VjY2VzcyIsImRhdGEiOiJleUoxYzJWeWJtRnRaU0k2SW5SbGMzUXlNallpTENKamFHRnlZV04wWlhJaU9pSjBaWE4wTWpJMklpd2ljSEp2ZG1sa1pYSWlPakFzSW5KbFptTnZaR1VpT2lJaUxDSmhaMlZ1ZENJNklrUmxjMnQwYjNBN1RXRmpJRTlUSUZnN01DSXNJblpsY25OcGIyNGlPaUkxTGpBdU1DSXNJbWx3SWpvaU1UQXVPQzR4TkM0eE9EY2lMQ0pqWVhKeWFXVnlJam9pSWl3aWNHeGhkR1p2Y20waU9pSnRZV01pTENKamFHRnVibVZzSWpvaU1IeHRaWHcxTGpBdU1DSjkiLCJwYXJ0bmVyX3RyYW5zX2lkIjoiMTYwNjE1c1VVd0MyTmhpIiwiZmFpbF91cmwiOiJodHRwOi8vYmFua2luZy5pd2luLnZuL2ZhaWwiLCJiYW5rX3R5cGUiOjF9&token=c0d28e3bd841b146e77b90d1fca69fce\",\"money\":5000000,\"win\":812500000,\"win_promotion\":0}}]},\"unit\":\"win\"}]},\"unit\":\"win\"}}";
	rapidjson::Document			json;
	Data dataJson = FileUtils::getInstance()->getDataFromFile("shopscreen.txt");

	char* expand_data = new char[dataJson.getSize() + 1];
	memcpy(expand_data, dataJson.getBytes(), dataJson.getSize());
	expand_data[dataJson.getSize()] = '\0';

	JsonMessage* json_message = new JsonMessage();
	json_message->setData((const char*)expand_data);
	
	Payment* payment = new Payment();
	payment->toData(json_message->getData());
	
	parsePaymentInfo(payment, 0);

	delete expand_data;

	_choose_payment_method->reload(_payment_method_data.size());
}

void IAPScreen::parsePaymentInfo(Payment* payment, int level)
{
	int _level = level;
	if (payment->getType() == "list")
	{

		if (payment->getCharge_type() == "sms")
		{
			if (level == 1)
			{
				_payment_method_data.push_back(payment);
			}
			else if (level == 2)
			{
				_telco_data_sms.push_back(payment);
			}
		}
		else if (payment->getCharge_type() == "input_2_field")
		{
			if (level == 1)
			{
				_payment_method_data.push_back(payment);
			}
		
		}
		else if (payment->getCharge_type() == "apple_macos_app_store")
		{
			if (level == 1)
			{
				_payment_method_data.push_back(payment);
			}
		}
		else if (payment->getCharge_type() == "web")
		{
			if (level == 1)
			{
				_payment_method_data.push_back(payment);
			}
		}


		for (Payment* p : payment->data->getList())
		{
			parsePaymentInfo(p, _level + 1);
		}

	}
	else if (payment->getType() == "action")
	{
		if (payment->getCharge_type() == "input_2_field")
		{
			_telco_data_card.push_back(payment);
		}
		else if (payment->getCharge_type() == "web")
		{
			_telco_data_banking.push_back(payment);
		}
		else if (payment->getCharge_type() == "apple_macos_app_store")
		{
			_telco_data_iap.push_back(payment);
		}
	}
}

void IAPScreen::parseImageUrl(std::string ser_url, std::string& image_url, std::string& save_path)
{
	char resource_id = '1';
	image_url.clear();
	save_path.clear();
	int idx = ser_url.find("***");
	image_url = ser_url.substr(0, idx);
	save_path = resource_id + ser_url.substr(idx + 3, ser_url.size() - idx - 3);
	image_url += save_path;
}

void IAPScreen::updateFocusPaymentList(int idx)
{
	_choose_payment_method->handleItemList([idx](WidgetEntity* entity) {
		int cur_idx = entity->GetWidgetChildByName(".btn_payment_method")->GetResource()->getTag();
		entity->GetWidgetChildByName(".img_bg_selected")->SetVisible(cur_idx == idx);
		entity->GetWidgetChildByName(".icon_payment")->GetResource()->getChildByName("sprite_on")->setVisible(cur_idx == idx);
	});
}

void IAPScreen::showPayment(int idx)
{
	if (_payment_method_data.size() <= 0)
	{
		PASSERT2(_payment_method_data.size() > 0, "size of data is zero, check again");
		return;
	}

	if ((size_t)idx >= _payment_method_data.size())
	{
		idx = 0;
	}
	_cur_type_payment = idx;
	updateFocusPaymentList(idx);
	if (_payment_method_data.at(idx)->getCharge_type() == "sms")
	{
		showSms();
	}
	else if (_payment_method_data.at(idx)->getCharge_type() == "input_2_field")
	{
		showCards();
	}
	else if (_payment_method_data.at(idx)->getCharge_type() == "web")
	{
		showBanking();
	}
	else if (_payment_method_data.at(idx)->getCharge_type() == "apple_macos_app_store")
	{ 
		showIap();
	}
}

void IAPScreen::reloadTelcoListWithSms()
{
	_scroll_view_telco_sms->reload(_telco_data_sms.size());
	_scroll_view_telco_sms->focus(0);
}

void IAPScreen::reloadTelcoListWithCard()
{
	_scroll_view_telco_card->reload(_telco_data_card.size());
	_scroll_view_telco_card->focus(0);
}

void IAPScreen::reloadBanking()
{
	_scroll_view_banking->reload(_telco_data_banking.size());
}

void IAPScreen::reloadIap()
{
	_scroll_view_iap->reload(_telco_data_iap.size());
}

void IAPScreen::reloadSms(int telco_idx)
{
	_scroll_view_sms->reload(_telco_data_sms.at(telco_idx)->data->getList().size());
}

void IAPScreen::reloadCard(int telco_idx)
{
	_scroll_view_value_card->reload(_telco_data_card.at(telco_idx)->data->getItems().size());
}

void IAPScreen::OnBeginFadeIn()
{
	GlobalService::getCaptchaFirstTime();
	if (!_payment)
	{
		GlobalService::requestPaymentInfo("win");
	}
	GetWidgetChildByName("iap_ui.top_panel.btn_win")->SetVisible(GameController::enableRuby);
	GetWidgetChildByName("iap_ui.top_panel.group_win")->SetVisible(GameController::enableRuby);
	GetWidgetChildByName("iap_ui.top_panel.btn_ruby")->SetVisible(GameController::enableRuby);
	GetWidgetChildByName("iap_ui.top_panel.group_ruby")->SetVisible(GameController::enableRuby);
}

void IAPScreen::onReceiveCaptcha(CaptchaInfo info)
{
	if (!_cur_captcha)
	{
		_cur_captcha = new CaptchaInfo();
	}
	_cur_captcha->setId(info.getId());
	_cur_captcha->setImage(info.getImage());
	updateCaptchaImage();
}

void IAPScreen::onReceiveCaptchaResult(CaptchaResult result)
{
	if (result.getCode() == CaptchaResult::CaptchaCheckType::SUCCESS)
	{

	}
	else
	{
		ScrMgr->OnShowDialog(LangMgr->GetString("notice"), result.getDesc(), TYPE_DIALOG::DIALOG_ONE_BUTTON, "captcha_fail", [&](const char * call_back, const char* btn_name)
		{
		});
	}
}

void IAPScreen::updateCaptchaImage()
{
	_spr_captcha->startDownload(_cur_captcha->getImage());
}

void IAPScreen::onReceivePayment(iwinmesage::Payment* payment)
{
	if (payment->getUnit() == "win")
	{
		_payment_win = payment;
	}
	else
	{
		_payment_ruby = payment;
	}
	_payment = payment;
	reloadPayment();
}


void IAPScreen::reloadPayment()
{
	_telco_data_sms.clear();
	_telco_data_card.clear();
	_telco_data_banking.clear();
	_telco_data_iap.clear();
	_payment_method_data.clear();
	parsePaymentInfo(_payment, 0);
	_choose_payment_method->reload(_payment_method_data.size());
	if (_cur_type_payment == -1) _cur_type_payment = 0;
	showPayment(_cur_type_payment);
}
