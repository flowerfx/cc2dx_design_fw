#include "BaseScreen.h"
#include "RKThread_Code/RKThreadWrapper.h"
#include "MainGame.h"

#include "ScreenManager.h"

BaseScreen::BaseScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
    p_menu_show_next= MENU_LAYER::MENU_NONE;

	p_current_idx_add_ = -1;
	p_time_add_ = 0;
	p_origin_time_add_ = 0.f;
	p_size_object_need_to_add_ = 0;
	p_have_info_change = false;
	p_thread_init_component = nullptr;
	p_init_component_complete = false;
}

BaseScreen::~BaseScreen()
{
	p_current_idx_add_ = -1;
	p_time_add_ = 0;
	if (p_thread_init_component)
	{
		delete p_thread_init_component;
		p_thread_init_component = nullptr;
	}
}

int BaseScreen::Init()
{
	return 1;
}

int	BaseScreen::InitComponent()
{
	return 1;
}

void BaseScreen::CallInitComponent(bool use_thread)
{
	use_thread = false;
	if (!use_thread)
	{
		InitComponent();
		p_init_component_complete = true;
	}
	else
	{
		if (p_thread_init_component)
		{
			delete p_thread_init_component;
			p_thread_init_component = nullptr;
		}
		//create thread
		p_thread_init_component = new ThreadWrapper();
		p_thread_init_component->CreateThreadWrapper("thread_init_component",
			([](void * data) -> uint32
		{
			BaseScreen * scr = (BaseScreen*)data;
			scr->p_thread_init_component->OnCheckUpdateThreadWrapper
			([scr]()
			{
				scr->InitComponent();
				scr->p_init_component_complete = true;
			});
			return 1;
		}),
		(void*)this);
	}
}

int BaseScreen::Update(float dt)
{
	if (!p_init_component_complete)
		return 0;

	if (p_IsStopUpdate)
	{
		return 0;
	}

	Screen::onUpdateMenuWidget(dt);

	OnHandleInfoChange();

	return 1;
}

void BaseScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{

}

void BaseScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{

}

void BaseScreen::OnDeactiveCurrentMenu()
{
	
}


void BaseScreen::OnBeginFadeIn()
{

}

void BaseScreen::OnFadeInComplete()
{
	ScrMgr->SetScreenIDDirty();
}

void BaseScreen::OnCallBackProcess(RKString str)
{
	OnProcess(str);
}
void BaseScreen::OnProcess(RKString str)
{
	//abstract class not handle this function
}

void BaseScreen::ParseList()
{
	//abstract class not handle this function
}


void BaseScreen::LoadListAsTime(float dt)
{
	if (p_time_add_ >= 0.f && p_current_idx_add_ > -1)
	{
		p_time_add_ -= dt;
		if (p_time_add_ <= 0.f)
		{
			p_time_add_ = 0.f;
			if (p_current_idx_add_ > -1)
			{
				ParseList();
				p_current_idx_add_++;
				p_time_add_ = p_origin_time_add_;
				if (p_current_idx_add_ >= p_size_object_need_to_add_)
				{
					p_current_idx_add_ = -1;
				}
			}
		}

	}
}

RKString BaseScreen::NorValue(s64 value)
{
	if (value < 1000)
	{
		return std::to_string(value);
	}
	else
	{
		std::vector<RKString> list_refix;

		while (value > 0)
		{
			int value_hun = value % 1000;
			RKString val = "";
			if (value > 1000)
			{
				if (value_hun >= 100)
				{
					val = std::to_string(value_hun);
				}
				else if (value_hun < 100 && value_hun >= 10)
				{
					val = "0" + std::to_string(value_hun);
				}
				else if (value_hun < 10)
				{
					val = "00" + std::to_string(value_hun);
				}
			}
			else
			{
				val = std::to_string(value_hun);
			}
			list_refix.push_back(val);
			value = value / 1000;
		}

		RKString value_nor = "";
		PASSERT2(list_refix.size() > 1, "list refix error !");
		for (size_t i = list_refix.size() - 1; i >= 0; i--)
		{
			value_nor += list_refix[i] + (i > 0 ? "." : "");
		}

		return value_nor;
	}
	return "";
}

RKString BaseScreen::NormalValue(s64 value)
{
	if (value < 1000)
	{
		return std::to_string(value);
	}
	else if (value >= 1000 && value < 1000000)
	{
		int value_thou = value / 1000;
		int value_hun = value % 1000;
		RKString val = "";
		if (value_hun >= 100)
		{
			val = std::to_string(value_hun);
		}
		else if (value_hun < 100 && value_hun >= 10)
		{
			val = "0" + std::to_string(value_hun);
		}
		else if (value_hun < 10)
		{
			val = "00" + std::to_string(value_hun);
		}
		return std::to_string(value_thou) + "." + val;
	}
	else if (value >= 1000000 && value < 1000000000)
	{
		int value_thou = value / 1000000;

		int value_hun = value % 1000000;
		value_hun = value_hun / 10000;
		RKString val = "";
		if (value_hun < 10 && value_hun > 0)
		{
			val = "0" + std::to_string(value_hun);
		}
		else if (value_hun >= 10)
		{
			val = std::to_string(value_hun);
		}

		return std::to_string(value_thou) + val + "M";
	}
	else if (value >= 1000000000 )
	{
		int value_thou = value / 1000000000;
		int value_hun = value % 1000000000;
		value_hun = value_hun / 10000000;
		RKString val = "";
		if (value_hun < 10 && value_hun > 0)
		{
			val = "0" + std::to_string(value_hun);
		}
		else if (value_hun >= 10)
		{
			val = std::to_string(value_hun);
		}
		return std::to_string(value_thou) + val + "B";
	}
	return std::to_string(value);
}

void BaseScreen::SetInteractiveInsteadOf(bool value, WidgetEntity * exp)
{
	exp->GetParentLayer()->SetInteractive(value);
	exp->SetInteractive(true);
}

void BaseScreen::CheckTouchOutSideWidget(cocos2d::Point p, WidgetEntity * panel, const std::function<void(void)> & func)
{
	if (panel->Visible())
	{
        ActionProcess * ac = panel->GetParentWidget();
        if(ac == nullptr)
        {
            ac = panel->GetParentLayer();
            PASSERT2(ac != nullptr , "the layer parent is null, check the code again !");
        }
        
		Vec2 world_pos = ac->GetResource()->convertToWorldSpace(panel->GetPosition_Middle());
		cocos2d::Rect rect_panel_ =
			cocos2d::Rect(
				world_pos.x - panel->GetSize().x / 2,
				world_pos.y - panel->GetSize().y / 2,
				panel->GetSize().x, panel->GetSize().y);
		if (!rect_panel_.containsPoint(p))
		{
			func();
			SetInteractiveInsteadOf(true, panel);
		}
	}
}

void BaseScreen::onMyInfoChanged()
{
	p_have_info_change = true;
}

void BaseScreen::ParseUserUI()
{
	//abstrast class not handle this function!
	return;
}

void BaseScreen::ParseImgToPanel(WidgetEntity * panel, void * data, void * url, int z_order)
{
	Image * img = (Image *)data;
	Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, url);
	sprite_img->setTag(99);
	if (z_order == 0)
	{
		z_order = 100;
	}
	ParseSpriteToPanel(panel, sprite_img, z_order);
}

void BaseScreen::ParseSpriteToPanel(WidgetEntity * panel, Sprite * sprite_img, int z_order)
{
	auto parent_panel_resource = panel->GetResource();
	sprite_img->setAnchorPoint(Vec2(0.5, 0.5));
	auto avatar_default = panel->GetWidgetChildByName(".avatar_default");
	//if (avatar_default)
	//{
		//avatar_default->Visible(false);
	//}

	Vec2 size_panel_clipping = panel->GetSize();
	Vec2 scale_panel_clipping = panel->GetScale();
	sprite_img->setAnchorPoint(Vec2(0.5, 0.5));
	sprite_img->setPosition(Vec2(size_panel_clipping.x / (2 * scale_panel_clipping.x), size_panel_clipping.y / (2 * scale_panel_clipping.y)));
	Vec2 contain_size = sprite_img->getContentSize();
	sprite_img->setScale(size_panel_clipping.x / (contain_size.x * scale_panel_clipping.x), size_panel_clipping.y / (contain_size.y * scale_panel_clipping.y));	
	
	sprite_img->removeFromParent();
	sprite_img->setTag(99);
	auto child = parent_panel_resource->getChildByTag(99);
	if (child)
	{
		parent_panel_resource->removeChildByTag(99);
	}
	parent_panel_resource->addChild(sprite_img, z_order);
}

void BaseScreen::OnHandleInfoChange()
{
	if (p_have_info_change)
	{
		ParseUserUI();
		p_have_info_change = false;
	}
}

void BaseScreen::OnReceiveFriendList()
{
	//abstrast class not handle this function
}