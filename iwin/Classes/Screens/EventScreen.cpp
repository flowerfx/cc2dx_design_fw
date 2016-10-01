#include "EventScreen.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wTimeDigitalWidget.h"
#include "UI/AdvanceUI/wLoadingBar.h"

#include "Network/JsonObject/Quest/ConfirmBox.h"
#include "Network/JsonObject/Quest/Quest.h"
#include "Network/JsonObject/Quest/QuestCategory.h"
#include "Network/JsonObject/Quest/QuestCategoryList.h"
#include "Network/JsonObject/Quest/ResponseQuestList.h"

#include "Network/Global/Globalservice.h"
#include "FileManager.h"


EventScreen::EventScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_list_quest_category = nullptr;
	p_current_quest_list = nullptr;
	p_current_idx_quest = 0;
}


EventScreen::~EventScreen()
{
	if (p_list_quest_category)
	{
		delete p_list_quest_category;
		p_list_quest_category = nullptr;
	}
	if (p_current_quest_list)
	{
		delete p_current_quest_list;
		p_current_quest_list = nullptr;
	}
}

int EventScreen::Init()
{
	RKString _menu = "event_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);
	CallInitComponent(true);
	return 1;
}

int	EventScreen::InitComponent()
{
	p_list_quest_category = new iwinmesage::QuestCategoryList();

	auto layout_quest_catalogue = GetWidgetChildByName("event_ui.quest_catalogue");
	_list_quest_catalogue = layout_quest_catalogue->GetWidgetChildByName(".list_quest_catalogue");

	auto quest_ = GetWidgetChildByName("event_ui.quest_");
	_list_quest_sample = quest_->GetWidgetChildByName(".list_quest_sample");
	_list_quest_empty = quest_->GetWidgetChildByName(".list_quest_");

	return 1;
}

int EventScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}
	return 1;
}

void EventScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void EventScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void EventScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close")
		{
			ScrMgr->SwitchToMenu(p_menu_come_from, EVENT_SCREEN);
		}
		if (name == "btn_show_quest")
		{
			if (!_widget->GetParentWidget()->GetWidgetChildAtIdx(0)->Visible())
			{
				OnHideAllHLCatagoryList();
				_widget->GetParentWidget()->GetWidgetChildAtIdx(0)->Visible(true);
				RKString name_list = _widget->GetParentWidget()->GetName();
				RKString idx = name_list.SplitLast("_").GetAt(1);
				auto list_category = p_list_quest_category->getCategoryList();
				GlobalService::GetQuestByCategoryID(list_category[atoi(idx.GetString())].getId());
			}

		}
	}
}

void EventScreen::OnBeginFadeIn()
{
	OnRequestDataJson();
}

void EventScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(EVENT_SCREEN);
	p_menu_show_next = MENU_NONE;
}


void EventScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}


void EventScreen::OnReceiveListQuestCataloryJson(std::string str_json)
{
	if (!p_list_quest_category)
	{
		PASSERT2(p_list_quest_category != nullptr, "p_list_quest_category is null");
		return;
	}
	delete p_list_quest_category;
	p_list_quest_category = new iwinmesage::QuestCategoryList();

	p_list_quest_category->toData(str_json);

	auto list_category = p_list_quest_category->getCategoryList();
	int current_number_child = _list_quest_catalogue->GetNumberChildWidget();

	for (size_t i = 0; i < list_category.size(); i++)
	{
		WidgetEntity * _widget = nullptr;
		if (i < (size_t)current_number_child)
		{
			_widget = _list_quest_catalogue->GetWidgetChildAtIdx(i);
		}
		else
		{
			 _widget = static_cast<ListViewWidget*>(_list_quest_catalogue)->CreateWidgetRefer(i);
			 _list_quest_catalogue->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, _widget->GetWidgetChildByName(".btn_show_quest"));
		}
		_widget->Visible(true);
		RKString previous_name = static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->GetText();
		if (previous_name != list_category[i].getName())
		{
			static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(list_category[i].getName());
		}
		static_cast<TextWidget*>(_widget->GetWidgetChildByName(".layout_.number_quest"))->SetText(std::to_string(list_category[i].getItemNo()));

		//if (previous_name != list_category[i].getName())
		{
			auto panel = _widget->GetWidgetChildByName(".layout_.panel_icon");
			panel->retain();
			GameMgr->HandleURLImgDownload(
				list_category[i].getImgUrl(), list_category[i].getName(), [this, panel, i](void * data, void * str, int tag)
			{
				this->ParseImgToPanel(panel, data, str, i);
			});
		}
		int current_id_cata = p_list_quest_category->getCurrentCategory();
		if (list_category[i].getId() == ((current_id_cata > 1 )? current_id_cata : 1))
		{
			GlobalService::GetQuestByCategoryID(list_category[i].getId());
			_widget->GetWidgetChildAtIdx(0)->Visible(true);
			ShowQuestAtId(list_category[i].getId());
		}
	}

	//hide the list if not use
	if (list_category.size() < (size_t)current_number_child)
	{
		for (size_t i = list_category.size(); i < (size_t)current_number_child; i++)
		{
			auto _widget = _list_quest_catalogue->GetWidgetChildAtIdx(i);
			_widget->Visible(false);
		}
	}

}

void EventScreen::OnRequestDataJson()
{
	GlobalService::GetQuestCategoryList();
}

void EventScreen::OnReceiveQuestByCalaloryJson(std::string str_json)
{
	if (p_current_quest_list)
	{
		delete p_current_quest_list;
	}
	p_current_quest_list = new iwinmesage::ResponseQuestList();

	p_current_quest_list->toData(str_json);

	p_current_idx_quest = p_current_quest_list->getCategoryId();
	int current_idx_q = p_current_idx_quest;
	auto _widget_list = _list_quest_empty;
	ClearListAndDeleteTexture(_widget_list, ".icon", "img_url");
	//int current_number_child = _widget_list->GetNumberChildWidget();
	auto list_quest = p_current_quest_list->getQuests();

	for (size_t i = 0; i < list_quest.size(); i++)
	{
		Quest q = list_quest[i];
		//if (q.getActionType() <= iwinmesage::QuestType::TYPE_OPEN_URL)
		{
			auto _widget = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(0);
			OnResizeAndInitQuestWidget(_widget, q);
			static_cast<ListViewWidget*>(_widget_list)->PushBackItem(_widget);
			//
			auto panel = _widget->GetWidgetChildByName(".icon");
			panel->retain();
			GameMgr->HandleURLImgDownload(
					q.getUrlImage(), q.getId() + "_" + std::to_string(i), [this, panel, i , current_idx_q](void * data, void * str, int tag)
				{
					Image * img = (Image *)data;
					if (ScrMgr->GetCurrentMenuUI() == EVENT_SCREEN && current_idx_q == this->p_current_idx_quest)
					{
						this->ParseImgToPanel(panel, data, str, i);
						panel->release();
					}
					else
					{
						//PASSERT2(current_idx_q == this->p_current_idx_quest, "it's safe to continue!");
						SAFE_DELETE(img);
					}
				});
		}
	}
}

void EventScreen::ShowQuestAtId(int id)
{
	return;
}

int EventScreen::GetTypeID(int action_type)
{
	iwinmesage::QuestType type = (iwinmesage::QuestType)action_type;
	switch (type)
	{
	case iwinmesage::TYPE_ACCEPT_QUEST:
	case iwinmesage::TYPE_OPEN_URL:
	{
		return 0;
		break;
	}
	case iwinmesage::TYPE_INSTALL_APP:
		break;
	case iwinmesage::TYPE_SUBMIT_QUEST:
		break;
	case iwinmesage::TYPE_OPEN_SCREEN:
		break;
	case iwinmesage::TYPE_RATE_APP:
		break;
	case iwinmesage::TYPE_LIKE_FAN_PAGE:
		break;
	case iwinmesage::TYPE_POST_WALL:
		break;
	case iwinmesage::TYPE_WIN_FREE:
		break;
	case iwinmesage::TYPE_ENABLE_CONTACT:
		break;
	case iwinmesage::TYPE_ENABLE_NOTIFICATION:
		break;
	case iwinmesage::TYPE_OPEN_FACEBOOK_REQUEST:
		break;
	case iwinmesage::TYPE_SHARE_LINK_FACEBOOK:
		break;
	case iwinmesage::TYPE_OPEN_ADMIN_ROLE:
		break;
	case iwinmesage::TYPE_DICE:
		break;
	case iwinmesage::TYPE_LIKE_PAGE_GOOGLE:
		break;
	case iwinmesage::TYPE_MOON_FESTIVAL:
		break;
	case iwinmesage::TYPE_INDEPENDENCE:
		break;
	case iwinmesage::TYPE_HALLOWEEN:
		break;
	case iwinmesage::TYPE_SEND_USER_INFO:
		break;
	case iwinmesage::TYPE_INVITE_FACEBOOK_FRIENDS:
		break;
	case iwinmesage::TYPE_NEW_LUCKY_CIRCLE:
		break;
	case iwinmesage::TYPE_OPEN_NATIVE_URL:
		break;
	default:
		break;
	}
	return 1;
}


void EventScreen::ClearListAndDeleteTexture(WidgetEntity * list_clear, RKString path_panel, RKString name_delete)
{
	for (size_t i = 0; i < list_clear->GetNumberChildWidget(); i++)
	{
		auto _panel = list_clear->GetWidgetChildByName(path_panel);
		if (_panel)
		{
			auto child = _panel->GetResource()->getChildByName(name_delete.GetString());
			if (child)
			{
				_panel->GetResource()->removeChildByName(name_delete.GetString());
			}
		}
	}
	list_clear->ClearChild();
}

int EventScreen::GetNumberLine(RKString str)
{
	if (str == "")
		return 0;
	size_t number_end_line = Utils::countStringLine(std::string(str.GetString()));
	if (number_end_line < 1)
		return 1;
	else
		return number_end_line;
}

void EventScreen::OnHideAllHLCatagoryList()
{
	for (size_t i = 0; i < _list_quest_catalogue->GetNumberChildWidget(); i++)
	{
		auto _widget = _list_quest_catalogue->GetWidgetChildAtIdx(i);
		_widget->GetWidgetChildAtIdx(0)->Visible(false);
	}
}

void EventScreen::OnResizeAndInitQuestWidget(WidgetEntity * _widget, iwinmesage::Quest q)
{
	cocos2d::Size current_size = _widget->GetResource()->getContentSize();
	float current_size_y = current_size.height;
	auto _text_dec = _widget->GetWidgetChildByName(".title_content");
	auto _text_title = _widget->GetWidgetChildByName(".title_");

	int type_widget = GetTypeID((int)q.getActionType());
	//int number_line = GetNumberLine(q.getDescription());//GetLineWithLongString(q.getDescription(), _text_dec);
	int origin_delta = 10;
	float size_delta = origin_delta * GetGameSize().width / this->GetDesignSize().x;


	static_cast<TextWidget*>(_text_title)->SetText(q.getTitle());
	static_cast<TextWidget*>(_text_dec)->SetText(q.getDescription());

	Vec2 pos_bot_left_text_title = _text_title->GetPosition_BottomLeft();
	_text_dec->SetPosition(Vec2(pos_bot_left_text_title.x, pos_bot_left_text_title.y - size_delta));

	auto _w_btn = _widget->GetWidgetChildByName(".btn_action");
	if (q.getButtonTitle() == "")
	{
		_w_btn->Visible(false);
	}
	else
	{
		static_cast<TextWidget*>(_w_btn->GetWidgetChildAtIdx(0))->SetText(q.getButtonTitle(), true);
		static_cast<ButtonWidget*>(_w_btn)->SetCallBack(
			[q](Ref* ref, Widget::TouchEventType touch_type)
		{
			if (touch_type == Widget::TouchEventType::ENDED)
			{
				if (q.getconfirmBox().getConfirmMsg() != "")
				{
					iwinmesage::ConfirmBox cofirm_box = q.getconfirmBox();
					ScrMgr->OnShowDialog(
						cofirm_box.getConfirmTitle(), cofirm_box.getConfirmMsg(), DIALOG_TWO_BUTTON, "str_call_back",
						[](const char * call_back, const char * btn_name)
					{
						//function here
					} , nullptr,nullptr, cofirm_box.getYesButton() , cofirm_box.getNoButton());
				}
				else
				{
					if(q.getActionType() == QuestType::TYPE_NEW_LUCKY_CIRCLE)
					{
						GlobalService::GetLuckyCircleInfo(q.getBonusEventId(), 1);
						//ScrMgr->LuckyCircleVisiable(true);
					}
				}
			}
		});
	}

	Vec2 pos_bot_left_text = _text_dec->GetPosition_BottomLeft();
	if(pos_bot_left_text.y < 0 )
	{
		current_size_y -= pos_bot_left_text.y;
	}
	current_size_y += size_delta;

	Vec2 size_layout_options = Vec2(0, 0);
	size_t number_option = q.getoptionList().getOptionList().size();
	if (number_option > 0)
	{
		for (size_t i = 0; i < number_option; i++)
		{
			auto option_detail = q.getoptionList().getOptionList()[i];
			int type_option = q.getoptionList().getListType();
			int type_widget = 2;
			if (type_option == 1)
			{
				type_widget = 4;
			}
			auto _widget_options = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(type_widget);
			size_layout_options = _widget_options->GetSize();
			_widget_options->Visible(true);
			_widget_options->SetPosition(Vec2(pos_bot_left_text.x, pos_bot_left_text.y - size_delta - (i * size_layout_options.y)));
			current_size_y += size_layout_options.y;
			_widget->InsertChildWidget(_widget_options);
			static_cast<TextWidget*>(_widget_options->GetWidgetChildAtIdx(0))->SetText(option_detail.getText());
			_widget_options->GetWidgetChildAtIdx(2)->Visible(option_detail.getIsSelected());
			//add event for btn
			static_cast<ButtonWidget*>(_widget_options->GetWidgetChildAtIdx(1))->SetCallBack(
				[_widget_options, type_option](Ref* ref, Widget::TouchEventType touch_type)
			{	
				if (touch_type == Widget::TouchEventType::ENDED)
				{
					if (type_option == 0)
					{
						for (size_t i = 0; i < _widget_options->GetParentWidget()->GetNumberChildWidget(); i++)
						{
							auto _w = _widget_options->GetParentWidget()->GetWidgetChildAtIdx(i);
							if (_w->GetName() == "layout_option_")
							{
								_w->GetWidgetChildAtIdx(2)->Visible(false);
							}
						}
					}

					_widget_options->GetWidgetChildAtIdx(2)->Visible(true);
				}
			});
		}
		current_size_y += size_delta;
	}

	_widget->GetResource()->setContentSize(cocos2d::Size(current_size.width, current_size_y));
	_widget->GetWidgetChildAtIdx(0)->GetResource()->setContentSize(cocos2d::Size(current_size.width, current_size_y));

	Vec2 pos_top_left_icon = _widget->GetWidgetChildAtIdx(1)->GetPosition_TopLeft();
	int delta = current_size_y - pos_top_left_icon.y - size_delta;

	if (delta >= 0)
	{
		for (size_t i = 1; i < _widget->GetNumberChildWidget(); i++)
		{
			Vec2 current_pos = _widget->GetWidgetChildAtIdx(i)->GetPosition();
			current_pos.y += delta;
			_widget->GetWidgetChildAtIdx(i)->SetPosition(current_pos);
		}
	}

	_w_btn->SetPosition(Vec2(_w_btn->GetPosition().x, current_size_y / 2));

	int current_process = q.getCurrentProgress();
	int max_process = q.getMaxProgressIndex();
	if (q.getMaxProgressIndex() > 0)
	{
		auto _widget_process = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(1);
		Vec2 pos_bot_left = _w_btn->GetPosition_BottomLeft();
		_widget_process->SetPosition(Vec2(pos_bot_left.x, pos_bot_left.y - size_delta));
		_widget_process->Visible(true);
		_widget->InsertChildWidget(_widget_process);
		auto widget_process_panel = _widget_process->GetWidgetChildAtIdx(0);
		static_cast<LoadingBarWidget*>(widget_process_panel->GetWidgetChildAtIdx(0))->SetPercent(((float)current_process / max_process) * 100);
		static_cast<TextWidget*>(widget_process_panel->GetWidgetChildAtIdx(1))->SetText(std::to_string(current_process) + "/" + std::to_string(max_process));
	}

	RKUtils::int64 total_time = q.getTotalTime();
	RKUtils::int64 remain_time = q.getRemainTime();
	if (total_time > 0 && remain_time > 0)
	{
		auto _widget_time = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(3);
		_widget_time->Visible(true);
		_widget->InsertChildWidget(_widget_time);

		Vec2 pos_mid_btn = _w_btn->GetPosition_Middle();
		Vec2 pos_bot_left = _w_btn->GetPosition_BottomLeft();

		_widget_time->SetPosition(Vec2(pos_bot_left.x - size_delta, pos_mid_btn.y));

		RKUtils::int64 total_time_sec = total_time / 1000;
		RKUtils::int64 remain_time_sec = remain_time / 1000;
		static_cast<TimeProcessWidget*>(_widget_time->GetWidgetChildAtIdx(1))->SetTimer(total_time_sec, remain_time_sec);
		static_cast<TimeDigitalWidget*>(_widget_time->GetWidgetChildAtIdx(3))->SetTimer(total_time_sec, remain_time_sec);
	}
}

int EventScreen::GetLineWithLongString(RKString str, WidgetEntity * _widget)
{
	auto list_number_line = str.Split("\n", true);

	int width_size = static_cast<TextWidget*>(_widget)->GetTextAreaSize().x;
	int size_text = static_cast<ui::Text*>(_widget->GetResource())->getFontSize();

	int line_addition = 0;
	for (size_t i = 0; i < list_number_line.Size(); i++)
	{
		if ((float)(list_number_line[i].LengthUTF() * size_text) > width_size)
		{
			line_addition = (list_number_line[i].LengthUTF() * size_text) / width_size;
		}
	}

	return list_number_line.Size() + line_addition;

}