


#include "ChangeAwardScreen.h"
#include "Common/CustomSelectedBox.h"
#include "Common/SpriteIap.h"
#include "Social/SocialManager.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "Network/Global/Globalservice.h"
#include "Network/JsonObject/cashoutruby/RubyToVNDInfo.h"
#include "Network/JsonObject/cashoutruby/PaymentHistoryList.h"
#include "Network/JsonObject/cashoutruby/PaymentHistory.h"
#include "Network/JsonObject/captcha/CaptchaInfo.h"
#include "Network/JsonObject/captcha/CaptchaResult.h"
#include "Common/IwinListView.h"

ChangeAwardScreen::ChangeAwardScreen():
_rubyToVND(nullptr),
_cur_captcha(nullptr)
{
}

ChangeAwardScreen::~ChangeAwardScreen()
{
	CC_SAFE_DELETE(_telco_selected_box);
	CC_SAFE_DELETE(_rubyToVND);
}

int ChangeAwardScreen::Init()
{
	RKString _menu = "changeaward_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);

	WidgetEntity* captcha_bounding = GetWidgetChildByName("changeaward_ui.body_panel.panel_captcha");
	_spr_captcha = SpriteIap::create("test.png", "", "");
	_spr_captcha->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_spr_captcha->setContentSize(cocos2d::Size(captcha_bounding->GetSize().x, captcha_bounding->GetSize().y));
	captcha_bounding->GetResource()->addChild(_spr_captcha);

	CallInitComponent(true);
	return 1;
}

int	ChangeAwardScreen::InitComponent()
{
	_telco_entity = GetWidgetChildByName("changeaward_ui.body_panel.select_box_telco");
	_price_entity = GetWidgetChildByName("changeaward_ui.body_panel.select_box_price");
	_price_listview = GetWidgetChildByName("changeaward_ui.body_panel.bg_right.listview_price");
	_price_item = GetWidgetChildByName("changeaward_ui.body_panel.bg_right.panel_price_item");

	_captcha_text_field = GetWidgetChildByName("changeaward_ui.body_panel.textfield_captcha");

	_telco_selected_box = new CustomSelectedBox();
	_telco_selected_box->init(_telco_entity, CC_CALLBACK_3(ChangeAwardScreen::updateSelectBoxTelco, this));

	_price_selected_box = new CustomSelectedBox();
	_price_selected_box->init(_price_entity, CC_CALLBACK_3(ChangeAwardScreen::updateSelectBoxPrice, this));

	_price_list = new IwinListView();
	_price_list->init(static_cast<ListViewWidget*>(_price_listview), _price_item, CC_CALLBACK_3(ChangeAwardScreen::updateListviewItem, this));


	_list_history = GetWidgetChildByName("changeaward_ui.body_history.tableview_board");
	static_cast<ListViewTableWidget*>(_list_history)->addEventListener(
		[this](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			_widget->Visible(true);
			int current_idx_to = static_cast<ListViewTableWidget*>(this->_list_history)->GetCurrentIdxTo();
			updateItemHistory(_widget, current_idx_to);
		}
		else if (type == EventType_SetCell::ON_INIT_CELL)
		{
			_telco_entity->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, _widget->GetWidgetChildByName(".btn_detail"));
			_telco_entity->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, _widget->GetWidgetChildByName(".btn_cancel"));
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			auto ldb = static_cast<ListViewTableWidget*>(this->_list_history);
			size_t current_idx = ldb->GetCurrentIdxSelected();
			/*	if (current_idx >= 0 && current_idx < _cur_list_board.size())
			{
			auto idx_player = _cur_list_board.at(current_idx);

			int room_id = idx_player.getRoomId();
			int board_id = idx_player.getBoardId();
			bool password = idx_player.getPassword();
			if (!password)
			{
			GlobalService::joinBoard(room_id, board_id, "");
			}
			}
			else
			{
			PASSERT2(false, "have problem here!");
			}
			*/
		}
		else if (type == EventType_SetCell::ON_END_DROP_CELL)
		{
			auto state_drag = static_cast<ListViewTableWidget*>(_list_history)->current_stage_drag();
			if (state_drag == EventType_DragCell::DRAG_BOT_UP)
			{
				requestHistory();
			}
		}
	});

	static_cast<ListViewTableWidget*>(_list_history)->InitTable();
	return 1;
}

int ChangeAwardScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}
	return 1;
}

void ChangeAwardScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void ChangeAwardScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void ChangeAwardScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_BUTTON)
	{
		if (name == "btn_select")
		{
			if (_widget->GetParentWidget() == _telco_entity)
			{
				_telco_selected_box->showListView(true);
			}
			else
			{
				_price_selected_box->showListView(true);
			}
		}
		else if (name == "btn_select_item")
		{
			if (_widget->GetResource()->getUserData() == _telco_selected_box)
			{
				int idx = _widget->GetResource()->getTag();
				_telco_selected_box->showListView(false);
				setTelcoItem(_telco_entity->GetWidgetChildByName(".select_item"), idx);
				_telco_selected_box->setSelectedID(idx);
				updatePriceList();
			}
			else {
				int idx = _widget->GetResource()->getTag();
				_price_selected_box->showListView(false);
				setPriceItem(_price_entity->GetWidgetChildByName(".select_item"), idx);
				_price_selected_box->setSelectedID(idx);
			}
			updateTotalRuby();
		}
		else if (name == "btn_refresh_captcha")
		{
			if (_cur_captcha)
				GlobalService::getCaptchaRefresh(_cur_captcha->getId());
			else
				GlobalService::getCaptchaFirstTime();
		}
		else if (name == "btn_pay")
		{
			if (_cur_captcha)
			{
				std::string anwser = static_cast<TextFieldWidget*>(GetWidgetChildByName("changeaward_ui.body_panel.textfield_captcha"))->GetText().GetString();
				GlobalService::sendCheckCaptcha(_cur_captcha->getId(), anwser);
			}
		}
		else if (name == "btn_reward")
		{
			showReward(true);
		}
		else if (name == "btn_history")
		{
			showReward(false);
		}
		else if (name == "btn_cancel")
		{
			int idx = _widget->GetResource()->getTag();
			GlobalService::requestCancleCashout(_history_data.at(idx).getLogId());
		}
		else if (name == "btn_detail")
		{
			int idx = _widget->GetResource()->getTag();
			Utils::showDialogNotice(_history_data.at(idx).getDetail());
		}
		else if (name == "btn_back")
		{
			ScrMgr->ShowChangeAward(false);
		}

	}
	else if (name == "textfield_amount_DETACH_IME")
	{
		updateTotalRuby();
	}
}

void ChangeAwardScreen::OnDeactiveCurrentMenu()
{
	if (p_menu_show_next == HUD_SCREEN)
	{
		vector<RKString> name_list;
		SocialMgr->RemoveAllCallBackExcept(name_list);
	}

	ScrMgr->CloseCurrentMenu(CHANGE_AWARD_SCREEN);

	p_current_idx_add_ = -1;
	p_menu_show_next = MENU_NONE;

}


void ChangeAwardScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void ChangeAwardScreen::OnBeginFadeIn()
{
	GlobalService::getCaptchaFirstTime();
	GlobalService::requestListRubyToVND();
	updateUserRuby();
	showReward(true);
	requestHistory();
}

void ChangeAwardScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
}

void ChangeAwardScreen::updateSelectBoxTelco(int eventType, WidgetEntity* entity, int idx)
{
	if (eventType == 0)
	{
		entity->GetWidgetChildByName(".btn_select_item")->GetResource()->setTag(idx);
		_telco_entity->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, entity->GetWidgetChildByName(".btn_select_item"));
	}
	else
	{
		setTelcoItem(entity, idx);
		entity->GetWidgetChildByName(".img_selected")->SetVisible(_telco_selected_box->getSelectedID() == idx);
	}
}

void ChangeAwardScreen::updateSelectBoxPrice(int eventType, WidgetEntity* entity, int idx)
{
	if (eventType == 0)
	{
		entity->GetWidgetChildByName(".btn_select_item")->GetResource()->setTag(idx);
		_telco_entity->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, entity->GetWidgetChildByName(".btn_select_item"));
	}
	else
	{
		setPriceItem(entity, idx);
		entity->GetWidgetChildByName(".img_selected")->SetVisible(_price_selected_box->getSelectedID() == idx);
	}
}

void ChangeAwardScreen::setTelcoItem(WidgetEntity* entity, int idx)
{
	static_cast<TextWidget*>( entity->GetWidgetChildByName(".lbl_content"))->SetText(_rubyToVND->getRates().at(idx).getName());
}

void ChangeAwardScreen::setPriceItem(WidgetEntity* entity, int idx)
{
	int telco_idx = _telco_selected_box->getSelectedID();
	iwinmesage::Card value = _rubyToVND->getRates().at(telco_idx).getCards().at(idx);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_content"))->SetText(StringUtils::format("%d", value.getVnd()));
}
void ChangeAwardScreen::onReceiveRubyToCard(RubyToVNDInfo* rubyToCard)
{
	CC_SAFE_DELETE(_rubyToVND);
	_rubyToVND = rubyToCard;
	int telco_idx = 0;
	_telco_selected_box->setSelectedID(0);
	_telco_selected_box->reload(_rubyToVND->getRates().size());
	_price_selected_box->setSelectedID(0);
	_price_selected_box->reload(_rubyToVND->getRates().at(telco_idx).getCards().size());

	setTelcoItem(_telco_entity->GetWidgetChildByName(".select_item"), 0);
	setPriceItem(_price_entity->GetWidgetChildByName(".select_item"), 0);

	static_cast<TextWidget*>(GetWidgetChildByName("changeaward_ui.body_panel.lbl_notice"))->SetText(StringUtils::format(LangMgr->GetString("ruby_hint").GetString(), Utils::formatNUmber_dot(_rubyToVND->getMinRubyLeft()).c_str(), _rubyToVND->getMaxCardChange()));
	updatePriceList();
}

void ChangeAwardScreen::refreshHistory()
{

}

void ChangeAwardScreen::onReceiveListHistory(PaymentHistoryList* list)
{
	std::vector<PaymentHistory> list_history = list->getHistoryList();
	for (auto it : list_history)
	{
		_history_data.push_back(it);
	}
	static_cast<ListViewTableWidget*>(_list_history)->SetNumberSizeOfCell(_history_data.size());
}
void ChangeAwardScreen::onReceiveCancelCashout(CancelResult* cancel)
{
	_history_data.clear();
	static_cast<ListViewTableWidget*>(_list_history)->ResetListView();
	GlobalService::requestListHistoryCashOut(0);
}
void ChangeAwardScreen::updateUserRuby()
{
	TextWidget* lblRuby = static_cast<TextWidget*>(GetWidgetChildByName("changeaward_ui.lbl_ruby"));
	lblRuby->SetText(StringUtils::format("YOU HAVE %ld", GameController::myInfo->ruby));
	float scale = static_cast<TextBMFont*>(lblRuby->GetResource())->getScale();
	int str_width = static_cast<Label*>(static_cast<TextBMFont*>(lblRuby->GetResource())->getVirtualRenderer())->getContentSize().width;
	WidgetEntity* entity = GetWidgetChildByName("changeaward_ui.icon_ruby");
	cocos2d::Size game_size = GetGameSize();
	Vec2 cur_pos = lblRuby->GetPosition();
	int offset_ruby = cur_pos.x + str_width / 2 * scale;
	cur_pos = entity->GetPosition();
	cur_pos.x = offset_ruby;
	entity->SetPosition(cur_pos);
}

void ChangeAwardScreen::updateTotalRuby()
{
	int num_ruby = atoi(static_cast<TextFieldWidget*>(GetWidgetChildByName("changeaward_ui.body_panel.textfield_amount"))->GetText().GetString());
	int cur_telco = _telco_selected_box->getSelectedID();
	int cur_vnd = _price_selected_box->getSelectedID();
	int need_ruby = _rubyToVND->getRates().at(cur_telco).getCards().at(cur_vnd).getRuby() * num_ruby;
	static_cast<TextWidget*>(GetWidgetChildByName("changeaward_ui.body_panel.lbl_need_ruby"))->SetText(StringUtils::format("%d", need_ruby).c_str());
}

void ChangeAwardScreen::updateListviewItem(int eventType, WidgetEntity* entity, int idx)
{
	if(eventType == 1)
	{
		int cur_telco = _telco_selected_box->getSelectedID();
		iwinmesage::Card value = _rubyToVND->getRates().at(cur_telco).getCards().at(idx);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_vnd"))->SetText(StringUtils::format("%d VND", value.getVnd()));
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_ruby"))->SetText(StringUtils::format("%d", value.getRuby()));
	}
}

void ChangeAwardScreen::updatePriceList()
{
	int cur_telco = _telco_selected_box->getSelectedID();
	_price_list->reload(_rubyToVND->getRates().at(cur_telco).getCards().size());
}


void ChangeAwardScreen::onReceiveCaptcha(CaptchaInfo info)
{
	if (!_cur_captcha)
	{
		_cur_captcha = new CaptchaInfo();
	}
	_cur_captcha->setId(info.getId());
	_cur_captcha->setImage(info.getImage());
	updateCaptchaImage();
}
void ChangeAwardScreen::onReceiveCaptchaResult(CaptchaResult captcharesult) {

	if (captcharesult.getCode() == CaptchaResult::CaptchaCheckType::SUCCESS) { // Captcha Sucess
		GlobalService::getCaptchaRefresh(_cur_captcha->getId());
		// thuc hien doi the
		chargeRuby();
	}
	else if (captcharesult.getCode() == CaptchaResult::CaptchaCheckType::ERROR1 || captcharesult.getCode() == CaptchaResult::CaptchaCheckType::EXPIRED
		|| captcharesult.getCode() == CaptchaResult::CaptchaCheckType::WRONG_ANSWER) {
		Utils::showDialogNotice(captcharesult.getDesc());
	}
}
void ChangeAwardScreen::updateCaptchaImage()
{
	_spr_captcha->startDownload(_cur_captcha->getImage());
}

void ChangeAwardScreen::chargeRuby()
{
	int amount = atoi(static_cast<TextFieldWidget*>(GetWidgetChildByName("changeaward_ui.body_panel.textfield_amount"))->GetText().GetString());
	int cur_telco = _telco_selected_box->getSelectedID();
	int cur_vnd = _price_selected_box->getSelectedID();
	if (amount > 0)
	{
		PaymentInfo* payment = new PaymentInfo();
		payment->setAmount(amount);
		payment->setVnd(cur_vnd);
		payment->setCardId(cur_telco);
		GlobalService::requestCharge(payment);
		delete payment;
	}
}

void ChangeAwardScreen::showReward(bool visible)
{
	GetWidgetChildByName("changeaward_ui.body_panel")->SetVisible(visible);
	GetWidgetChildByName("changeaward_ui.top_panel.group_reward.icon_select")->SetVisible(visible);
	GetWidgetChildByName("changeaward_ui.body_history")->SetVisible(!visible);
	GetWidgetChildByName("changeaward_ui.top_panel.group_history.icon_select")->SetVisible(!visible);
}

void ChangeAwardScreen::updateItemHistory(WidgetEntity* entity, int idx)
{
	PaymentHistory history = _history_data.at(idx);
	entity->SetVisible(true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_time"))->SetText(history.getTime());
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_item"))->SetText(history.getItem());
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_status"))->SetText(history.getStatusDes());
	entity->GetWidgetChildByName(".btn_cancel")->SetVisible(history.getStatus() == 0);
	entity->GetWidgetChildByName(".btn_detail")->SetVisible(history.getStatus() != 0);
	entity->GetWidgetChildByName(".btn_cancel")->GetResource()->setTag(idx);
	entity->GetWidgetChildByName(".btn_detail")->GetResource()->setTag(idx);

}

void ChangeAwardScreen::requestHistory()
{
	int count = _history_data.size();
	const int max_item_in_page = 10;
	GlobalService::requestListHistoryCashOut(count % max_item_in_page != 0 ? (int)(count / max_item_in_page) + 1 : (int)(count / max_item_in_page));
}