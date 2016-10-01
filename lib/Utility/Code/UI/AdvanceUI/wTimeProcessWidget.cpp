#include "wTimeProcessWidget.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		TimeProcessWidget::TimeProcessWidget()
		{
			p_typeUI = UI_TYPE::UI_TIME_PROCESS;
			p_ObjectTimer = nullptr;
			p_timer = 1.0f;
			p_current_timer = 0.0f;
			p_already_to_zero = true;
			_eventCallback = nullptr;
			previous_percent = 0.f;
			MoveFromZero = false;
			p_xml_value = nullptr;
		}

		TimeProcessWidget::~TimeProcessWidget()
		{
			if (p_ObjectTimer)
			{
				p_ObjectTimer->removeAllChildren();
				//delete p_ObjectTimer;
				p_ObjectTimer = nullptr;
			}
			p_xml_value = nullptr;
			_eventCallback = nullptr;
		}
		
		cocos2d::Node * TimeProcessWidget::GetResource() {
			return p_ObjectTimer;
		}
		bool TimeProcessWidget::SetNewResource(cocos2d::Node * resource)
		{
			if (p_ObjectTimer)
			{
				//delete p_Resource;
				//p_Resource = nullptr;
				p_ObjectTimer = static_cast<ProgressTimer*>(resource);
			}
			else
			{
				return false;
			}

			return true;
		}

		void TimeProcessWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<TimeProcessWidget*>(value);
			WidgetEntity::CopyValue(value);
			//
			p_ObjectTimer = CreateTimer(val->p_xml_value);
			SetSize(val->GetSize());
			SetPosition(val->GetPosition());
			p_timer = val->p_timer;
			_eventCallback = val->_eventCallback;
			p_xml_value = val->p_xml_value;
		}

		WidgetEntity * TimeProcessWidget::Clone()
		{
			TimeProcessWidget * p_clone = new TimeProcessWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->GetResource()->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void TimeProcessWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		cocos2d::ProgressTimer * TimeProcessWidget::CreateTimer(xml::UILayerWidgetDec * xml_value)
		{
			auto source_texture = xml_value->GeneralValue->GetDataChar("source");
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);
			MoveFromZero = (xml_value->GeneralValue->GetDataInt("move_from_zero") == 1);
			Sprite* object = nullptr;
			if (IsUseAtlas)
			{
				object = Sprite::createWithSpriteFrame(
					SpriteFrameCache::getInstance()->getSpriteFrameByName(source_texture)
				);
			}
			else
			{
				object = Sprite::create(source_texture);
			}
			PASSERT2(object != nullptr, "object cannot be created !");



			auto _ObjectTimer = ProgressTimer::create(object);
			_ObjectTimer->retain();

			int TypeProgress = xml_value->GeneralValue->GetDataInt("type_progress");

			_ObjectTimer->setType((ProgressTimer::Type)TypeProgress);
			if (MoveFromZero)
			{
				_ObjectTimer->setPercentage(0.0f);
			}
			else
			{
				_ObjectTimer->setPercentage(100.f);
			}
			_ObjectTimer->setReverseDirection(xml_value->GeneralValue->GetDataInt("reserve") == 1);
			return _ObjectTimer;
		}

		void TimeProcessWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			p_xml_value = xml_value;
			p_ObjectTimer  = CreateTimer(xml_value);

		}
		
		void TimeProcessWidget::SetPercent(float per)
		{
			p_ObjectTimer->setPercentage(per);
		}

		bool TimeProcessWidget::Update(float dt)
		{
			if (!p_Visible)
				return false;

			if (MoveFromZero)
			{
				p_current_timer += dt;
			}
			else
			{
				p_current_timer -= dt;
			}
			if (MoveFromZero ? (p_current_timer >= p_timer) : (p_current_timer < 0))
			{

				if (MoveFromZero)
				{
					p_current_timer = p_timer;
				}
				else
				{
					p_current_timer = 0;
				}

				if (!p_already_to_zero)
				{
					p_already_to_zero = true;

					if (_eventCallback)
					{
						_eventCallback(this, EventType_Process::ON_TIMER_ZERO);
					}
				}
			}
			float percent = ((p_current_timer / p_timer) * 100.f);

			if (previous_percent != percent)
			{
				SetPercent(percent);
				previous_percent = percent;
			}
			return true;
		}

		void TimeProcessWidget::SetTimer(float time, float current_time, const wProgressTimerCallBack& callback)
		{
			p_current_timer = current_time;
			p_timer = time;
			PASSERT2(p_timer > 0 && p_current_timer >= 0, "timer is not a valid value!");
			if (p_timer <= 0)
			{
				p_timer = 1;
			}
			if (p_current_timer < 0)
			{
				p_current_timer = 0;
			}
			Visible(true);
			p_already_to_zero = false;

			if (callback)
			{
				_eventCallback = callback;
			}
		}

		void TimeProcessWidget::addEventListener(const wProgressTimerCallBack& callback)
		{
			_eventCallback = callback;
		}

		void TimeProcessWidget::StopTimeProcess()
		{
			p_already_to_zero = true;
			p_current_timer = 0;
			Visible(false);
		}

		float TimeProcessWidget::GetCurrentTimer()
		{
			return p_current_timer;
		}

	}
}