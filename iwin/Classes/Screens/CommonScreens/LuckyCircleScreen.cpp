#include "LuckyCircleScreen.h"

#include "Network/Global/Globalservice.h"
#include "network/JsonObject/LuckyCircle/LuckyCircleQuestList.h"
#include "network/JsonObject/LuckyCircle/LuckyCircleResultList.h"

#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wPanel.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wCheckBox.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "Utils.h"

USING_NS_CC;

#define NAME_IMG_RESULT "ImageResult"

LuckyCircleScreen::LuckyCircleScreen()
{
	p_lucky_quest_list = nullptr;
	p_lucky_result = nullptr;
	index_result = -1;
	isRotation = false;
	
}

LuckyCircleScreen::~LuckyCircleScreen()
{
	SAFE_DELETE(p_lucky_quest_list);
	SAFE_DELETE(p_lucky_result);
}

int LuckyCircleScreen::InitLuckyCircle(BaseScreen * screen)
{
	_common_screen = screen;
	p_layout_main = _common_screen->GetWidgetChildByName("common_screen.layout_crilce");
	p_img_circle =  p_layout_main->GetWidgetChildByName(".cricle_main.cricle_img");
	p_arow_tick =   p_layout_main->GetWidgetChildByName(".cricle_main.img_arow");
	p_list_view_result = p_layout_main->GetWidgetChildByName(".cricle_main.bg_result.scroll_content");
	p_popup_result = p_layout_main->GetWidgetChildByName(".layout_result");
	
	p_lb_money_win = p_layout_main->GetWidgetChildByName(".cricle_main.btn_purchase_win.title_");
	p_lb_money_start = p_layout_main->GetWidgetChildByName(".cricle_main.btn_purchase_rubby.title_");
	p_check_auto = p_layout_main->GetWidgetChildByName(".cricle_main.check_box_auto");
	p_anim_money =p_layout_main->GetWidgetChildByName(".animate_effect_bonus");
	return 1;
}

bool LuckyCircleScreen::IsVisible()
{
	if (p_layout_main)
	{
		return p_layout_main->IsVisible();
	}
	return false;
}

void LuckyCircleScreen::OnProcessChildTouch(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!p_layout_main || !p_layout_main->IsVisible())
	{
		return;
	}
	if (type_widget == UI_BUTTON)
	{
		if (name == "btn_close")
		{
			SetVisible(false);
			static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false);
			static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();

		}
		else if (name == "btn_spin")
		{
			if (isRotation || !p_lucky_quest_list)
			{
				return;
			}
			GlobalService::RequestTurningLuckyCircle(p_lucky_quest_list->getCircleId());
		}
	}
	
}

void LuckyCircleScreen::SetVisible(bool isShow)
{
	if (!p_layout_main || p_layout_main->IsVisible() == isShow)
	{
		return;
	}
	p_layout_main->SetVisible(isShow);
}

int LuckyCircleScreen::Update(float dt)
{
	if (!IsVisible())
		return 0;

	if (isRotation)
	{
		float angleTick =360 / p_lucky_quest_list->getList().size();
		float angle = p_img_circle->GetRotate() - angleTick/2;
		float detal = fmod(angle, angleTick);
		if (detal <= 15)
		{
			p_arow_tick->SetRotate(-detal*2);
		}else if (detal <=17)
		{
			//p_arow_tick->SetRotate(0);
		}
	}
	return 1;
}

void LuckyCircleScreen::OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_result_list)
{
	if (p_lb_money_win)
	{
		static_cast<TextWidget*>(p_lb_money_win)->SetText(Utils::formatNumber(GameController::myInfo->getMoney()));
	}
	if (p_lb_money_start)
	{
		static_cast<TextWidget*>(p_lb_money_start)->SetText(Utils::formatNumber(lucky_result_list->getUserPoint()));
	}
	RemoveImageFinish();
	GameMgr->HandleURLImgDownload(
		lucky_result_list->getUrlImg(),"", [this](void * data, void * str, int tag)
	{
		Image * img = (Image *)data;
		Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
		sprite_img->setTag(99);
		//sprite_img->setContentSize(cocos2d::Size(this->p_img_circle->GetSize().x, this->p_img_circle->GetSize().y));
		sprite_img->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		sprite_img->removeFromParent();
		sprite_img->setPosition(cocos2d::Vec2(this->p_img_circle->GetSize().x / 2, this->p_img_circle->GetSize().y / 2));
		sprite_img->setZOrder(0);
		this->p_img_circle->ClearChild();
		this->p_img_circle->GetResource()->addChild(sprite_img);
	});
	lucky_result_list->getUrlImg();
	SetVisible(true);
	p_lucky_quest_list = lucky_result_list;
	static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);
	static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
}

void LuckyCircleScreen::OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item)
{
	
	int index = 0;
	if (!lucky_result_item|| lucky_result_item->getList().size() <= 0)
	{
		return;
	}
	RemoveImageFinish();
	p_lucky_result = lucky_result_item;
	int _size = p_lucky_quest_list->getList().size();
	for (int i = 0; i <_size; i++)
	{
		if (p_lucky_quest_list->getList()[i].getId() == lucky_result_item->getList()[lucky_result_item->getList().size()-1].getItemId())
		{
			index = i;
			break;
		}
	}

	isRotation = true;
	float _angle = p_img_circle->GetRotate();
	float angleMain = fmod(_angle ,360);
	double angleItem = (index * (360 / _size)) % 360;
	int rand =fmod(std::rand(),5);
	float _sRotation =(15+ rand )* 360 + (360 - ((angleItem + angleMain)));
	
	if (_angle > std::numeric_limits<int>::max())
	{
		_angle = 0;
	}
	
	float angle = (360 / _size);
	double r = (((double)(std::rand())*(2)) / RAND_MAX) - 1;
	_sRotation += r * (angle/3);
	ActionRotation(_sRotation);
	index_result = index;
}
void LuckyCircleScreen::ActionRotation(float angle)
{
	isRotation = true;
	auto rotateTo = RotateBy::create(14.0f, angle);
	//auto rotateTo_ease_in = EaseExponentialOut::create(rotateTo);
	auto rotateTo_ease_in =EaseSineOut::create(rotateTo);
	
	auto sequence = Sequence::create(rotateTo_ease_in, CallFunc::create([this]() {
		isRotation = false;
		p_arow_tick->GetResource()->setRotation(0);
		p_img_circle->GetResource()->stopAllActions();
		OnRotationDone();
	}), nullptr);
	p_img_circle->GetResource()->runAction(sequence);

}

void LuckyCircleScreen::OnRotationDone()
{
	ShowResult();
	

	int size = p_lucky_quest_list->getList().size();
	string file_name = "gamescreen-newgui-game-common/luckycircle_win" + std::to_string(size) + ".png";
	ImageView* p_im_result = ImageView::create(file_name, Widget::TextureResType::PLIST);
	p_im_result->setName(NAME_IMG_RESULT);
	p_im_result->setAnchorPoint(Vec2(0.5, 0));
	p_im_result->setPosition(cocos2d::Vec2(this->p_img_circle->GetSize().x / 2, this->p_img_circle->GetSize().y / 2));
	p_im_result->setRotation(index_result*(360/size));
	this->p_img_circle->GetResource()->addChild(p_im_result);
	auto fadeOut =FadeOut::create(1.0f);
	auto fadeIn = FadeIn::create(0);
	auto repeat = RepeatForever::create(Sequence::create(fadeOut,fadeIn,nullptr));
	p_im_result->runAction(repeat);
	
	
}

void LuckyCircleScreen::UpdateMoney()
{
	if (p_lb_money_win)
	{
		static_cast<TextWidget*>(p_lb_money_win)->SetText(Utils::formatNumber(GameController::myInfo->getMoney()));
	}
	if (p_lb_money_start)
	{
		static_cast<TextWidget*>(p_lb_money_start)->SetText(Utils::formatNumber(p_lucky_result->getUserPoint()));
	}
}

void LuckyCircleScreen::RemoveImageFinish()
{
	if (!p_img_circle)
		return;
	auto node =p_img_circle->GetResource()->getChildByName(NAME_IMG_RESULT);
	if(node)
	{
		node->stopAllActions();
		node->removeFromParent();
	}
	p_popup_result->SetVisible(false);

}

void LuckyCircleScreen::ShowResult()
{
	if (p_anim_money)
	{
		p_anim_money->Visible(true);
		static_cast<AnimWidget*>(p_anim_money)->RunAnim(CallFunc::create([this]() {
			this->p_anim_money->Visible(false);
		}));
		//playsound
		GetSound->PlaySoundEffect("SFX_COIN_DROP");
	}
	UpdateMoney();
	p_popup_result->SetVisible(true);
	static_cast<TextWidget*>(p_popup_result->GetWidgetChildByName(".title"))->SetText(p_lucky_result->getResultString(), true);
	p_popup_result->GetResource()->setScaleY(0);
	auto actionScaleShow = ScaleTo::create(0.5,1,1);
	auto actionScaleHide = ScaleTo::create(0.5,1,0);
	p_popup_result->GetResource()->runAction(Sequence::create(actionScaleShow, DelayTime::create(3), actionScaleHide, CallFunc::create([this]() {
		if (this->p_check_auto && static_cast<CheckBox*>(this->p_check_auto->GetResource())->isSelected())
		{
			GlobalService::RequestTurningLuckyCircle(p_lucky_quest_list->getCircleId());
		}
	}), nullptr));

	ListViewWidget* listView = static_cast<ListViewWidget*>(p_list_view_result);
	auto w = listView->CreateWidgetRefer(listView->GetNumberChildWidget());
	(static_cast<TextWidget*>(w->GetWidgetChildByName(".notice_content")))->SetText("-"+p_lucky_result->getResultString(), true);
}

