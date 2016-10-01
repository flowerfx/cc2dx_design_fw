#include "wRadioBtn.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		RadioBtnWidget::RadioBtnWidget()
		{
			p_typeUI = UI_TYPE::UI_RADIO_BTN;
			p_IsInteractive = true;

			p_on_state = "";
			p_off_state = "";
			p_distance = 0.f;
			p_scale = 1.f;
		}
		RadioBtnWidget::~RadioBtnWidget()
		{
		}
		
		void RadioBtnWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<RadioBtnWidget*>(value);
			WidgetEntity::CopyValue(value);
			//
			p_on_state = val->p_on_state;
			p_off_state = val->p_off_state;
			p_distance = val->p_distance;
			p_direction = val->p_direction;
			p_scale = val->p_scale;
		}

		WidgetEntity * RadioBtnWidget::Clone()
		{
			RadioBtnWidget * p_clone = new RadioBtnWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;

		}

		/////////////////////////////////////////////////////////////
		void RadioBtnWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void RadioBtnWidget::create_btn_radio()
		{

		}

		void RadioBtnWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{

			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);

			p_on_state = xml_value->GeneralValue->GetDataChar("on_state");
			p_off_state = xml_value->GeneralValue->GetDataChar("off_state");
			int number_widget = xml_value->GeneralValue->GetDataInt("number");
			PASSERT2(number_widget >= 1, "number radio btn must bigger or equal 1");

			float delta_distance = xml_value->GeneralValue->GetDatafloat("delta_distance");
			p_distance = delta_distance * GetGameSize().width / GetMenuParent()->GetDesignSize().x;
			auto _radioButtonGroup = RadioButtonGroup::create();

			if (xml_value->GeneralValue->HaveDatafloat("scale_btn"))
			{
				p_scale = xml_value->GeneralValue->GetDatafloat("scale_btn");
			}
			p_direction = xml_value->GeneralValue->GetDataInt("direction");

			for (int i = 0; i < number_widget; i++)
			{
				auto p_button_refer = RadioButton::create(
					p_off_state.GetString(),
					p_on_state.GetString(),
					p_on_state.GetString(),
					p_off_state.GetString(),
					p_off_state.GetString(), IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);
				
				Vec2 pos_ = Vec2(p_direction == 0 ? (i  * p_distance) : 0, p_direction == 0 ? 0 : (i  * p_distance));
				p_button_refer->setPosition(pos_);
				
				int add = 1.0f;
				if (GetGameSize().width < p_min_size_scale && p_min_size_scale > 0)
				{
					add = (p_min_size_scale / GetGameSize().width) ;
				}
				else
				{
					add = GetGameSize().width / GetMenuParent()->GetDesignSize().x;
				}
				p_button_refer->setScale(p_scale * add);

				_radioButtonGroup->addRadioButton(p_button_refer);
				_radioButtonGroup->addChild(p_button_refer);
			}
			Vec2 pos = _radioButtonGroup->getPosition();
			if (p_direction == 0)
			{
				pos.x -= (number_widget / 2)*(p_distance);
			}
			else if (p_direction == 1)
			{
				pos.y -= (number_widget / 2)*(p_distance);
			}

			_radioButtonGroup->setPosition(pos);


			p_Resource = _radioButtonGroup;
		}
		

		void RadioBtnWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<Widget*>(p_Resource)->setEnabled(value);
		}

		void RadioBtnWidget::SetSize(Vec2 size)
		{
			return;
		}

		void RadioBtnWidget::AddRadioBtn()
		{
			auto _RadioButtonGroup = static_cast<RadioButtonGroup*>(p_Resource);
			int idx = _RadioButtonGroup->getChildrenCount();

			auto p_button_refer = RadioButton::create(
				p_off_state.GetString(),
				p_on_state.GetString(),
				p_on_state.GetString(),
				p_off_state.GetString(),
				p_off_state.GetString(), true ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);
			
			Vec2 pos_ = Vec2(p_direction == 0 ? (idx  * p_distance) : 0, p_direction == 0 ? 0 : (idx  * p_distance));
			p_button_refer->setPosition(pos_);
			
			int add = 1.0f;
			if (GetGameSize().width < p_min_size_scale && p_min_size_scale > 0)
			{
				add = (p_min_size_scale / GetGameSize().width);
			}
			else
			{
				add = GetGameSize().width / GetMenuParent()->GetDesignSize().x;
			}
			p_button_refer->setScale(p_scale * add);

			static_cast<RadioButtonGroup*>(p_Resource)->addRadioButton(p_button_refer);
			static_cast<RadioButtonGroup*>(p_Resource)->addChild(p_button_refer);

			Vec2 pos = _RadioButtonGroup->getPosition();

			if (p_direction == 0)
			{
				pos.x -= (p_distance / 2);
			}
			else if (p_direction == 1)
			{
				pos.y -= (p_distance / 2);
			}
			_RadioButtonGroup->setPosition(pos);
		}

		void RadioBtnWidget::RemoveRadioBtn()
		{
			auto _RadioButtonGroup = static_cast<RadioButtonGroup*>(p_Resource);
			int idx = _RadioButtonGroup->getChildrenCount();

			RadioButton* radioButton = _RadioButtonGroup->getRadioButtonByIndex(idx - 1);
			_RadioButtonGroup->removeRadioButton(radioButton);
			_RadioButtonGroup->removeChild(radioButton);

			Vec2 pos = _RadioButtonGroup->getPosition();

			if (p_direction == 0)
			{
				pos.x += (p_distance / 2);
			}
			else if (p_direction == 1)
			{
				pos.y += (p_distance / 2);
			}

			_RadioButtonGroup->setPosition(pos);
		}

		void RadioBtnWidget::SetNumberRadioBtn(int number)
		{
			PASSERT2(number >= 1, "incorrect number btn");

			auto _RadioButtonGroup = static_cast<RadioButtonGroup*>(p_Resource);
			int number_radio_btn = _RadioButtonGroup->getChildrenCount();

			if (number_radio_btn == number)
			{
				return;
			}

			if (number_radio_btn > number)
			{
				for (int i = 0; i < number_radio_btn - number; i++)
				{
					RemoveRadioBtn();
				}
			}
			else if (number_radio_btn < number)
			{
				for (int i = 0; i < number - number_radio_btn; i++)
				{
					AddRadioBtn();
				}
			}
		}

		void RadioBtnWidget::MoveToNextBtn()
		{
			auto _RadioButtonGroup = static_cast<RadioButtonGroup*>(p_Resource);
			int number_radio_btn = _RadioButtonGroup->getChildrenCount();
			int current_idx_btn = _RadioButtonGroup->getSelectedButtonIndex();
			current_idx_btn++;
			if (current_idx_btn >= number_radio_btn)
			{
				return;
			}
			_RadioButtonGroup->setSelectedButtonWithoutEvent(current_idx_btn);
		}

		void RadioBtnWidget::MovePreviousBtn()
		{
			auto _RadioButtonGroup = static_cast<RadioButtonGroup*>(p_Resource);
			int current_idx_btn = _RadioButtonGroup->getSelectedButtonIndex();
			current_idx_btn--;
			if (current_idx_btn < 0)
			{
				return;
			}
			_RadioButtonGroup->setSelectedButtonWithoutEvent(current_idx_btn);
		}

		void RadioBtnWidget::MoveToBtnIdx(int idx)
		{
			auto _RadioButtonGroup = static_cast<RadioButtonGroup*>(p_Resource);
			int number_radio_btn = _RadioButtonGroup->getChildrenCount();
			if (idx >= number_radio_btn)
			{
				idx = number_radio_btn;
			}
			else if (idx <= 0)
			{
				idx = 0;
			}
			_RadioButtonGroup->setSelectedButtonWithoutEvent(idx);
		}

	}
}