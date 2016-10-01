#include "wAnimationWidget.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		AnimWidget::AnimWidget()
		{
			p_typeUI = UI_TYPE::UI_ANIMATE;
			_eventCallback = nullptr;
			_begin_idx = _end_idx = _sprite_idx;

		}
		AnimWidget::~AnimWidget()
		{
			_eventCallback = nullptr;
			_list_frame_name.Clear();
		}

		WidgetEntity * AnimWidget::Clone()
		{
			AnimWidget * p_clone = new AnimWidget();
			//copy all value (excep pointer)
#if defined(OS_IOS) || defined(OS_ANDROID_)
			memcpy(p_clone, this, sizeof(p_clone));
#else
			memcpy_s(p_clone, sizeof(p_clone), this, sizeof(this));
#endif
			//copy resource
			p_clone->_sprite_anim = Sprite::createWithTexture(_sprite_anim->getTexture(), _sprite_anim->getTextureRect());
			p_clone->_store_animate_list = _store_animate_list->clone();
			p_clone->_list_frame_name = _list_frame_name;
			//remove child remain
			p_clone->_sprite_anim->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void AnimWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void AnimWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			_sprite_anim = nullptr;

			auto source_texture = xml_value->GeneralValue->GetDataChar("source");
			Vec2 slice_sprite_anim = xml_value->GeneralValue->GetDataVector2("slice");
			Vector<SpriteFrame*> animFrames;

			bool have_multi_sprite = (xml_value->GeneralValue->GetDataInt("multi_sprite") == 1);


			_begin_idx = xml_value->GeneralValue->GetDataInt("begin_idx");
			_end_idx = xml_value->GeneralValue->GetDataInt("end_idx");
			if (_end_idx == 0)
			{
				_end_idx = slice_sprite_anim.x * slice_sprite_anim.y - 1;
			}
			_sprite_idx = xml_value->GeneralValue->GetDataInt("sprite_idx");

			if (!have_multi_sprite)
			{
				if (slice_sprite_anim.x <= 0 || slice_sprite_anim.y <= 0)
				{
					auto list_tex_name = xml_value->GeneralValue->GetDataChar("list_tex");
					auto list_frame = XMLMgr->GetTextureFrameXML()->GetTexFrameByName(list_tex_name);
					auto detail_frame = list_frame.GetTexFrameDetailByName(source_texture);

					slice_sprite_anim = detail_frame.value.GetDataVector2("slice");
					_begin_idx = detail_frame.value.GetDataInt("begin_idx");
					_end_idx = detail_frame.value.GetDataInt("end_idx");
					_sprite_idx = detail_frame.value.GetDataInt("sprite_idx");
				}

				size_t number_slice = (size_t)(slice_sprite_anim.x * slice_sprite_anim.y);

				std::vector<cocos2d::Rect> list_rec;
				auto sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(source_texture);
				auto rect_texture = sp->getRect();
				cocos2d::Size size_each_slice = cocos2d::Size(rect_texture.size.width / slice_sprite_anim.x, rect_texture.size.height / slice_sprite_anim.y);
				Vec2 begin_pos = rect_texture.origin;
				int count_idx = 0;
				for (int i = 0; i < slice_sprite_anim.y; i++)
				{
					for (int j = 0; j < slice_sprite_anim.x; j++)
					{
						if (count_idx >= _begin_idx && count_idx <= _end_idx)
						{
							cocos2d::Rect actual_rect = cocos2d::Rect(
								begin_pos.x + j* size_each_slice.width, begin_pos.y + i* size_each_slice.height,
								size_each_slice.width, size_each_slice.height
							);
							list_rec.push_back(actual_rect);
						}
						count_idx++;
					}
				}

				Sprite * temp = Sprite::createWithSpriteFrame(sp);
				for (auto rec : list_rec)
				{
					auto frame = SpriteFrame::createWithTexture(temp->getTexture(), rec);
					animFrames.pushBack(frame);
				}
				_sprite_anim = Sprite::createWithTexture(temp->getTexture(), list_rec[_sprite_idx]);
			}
			else
			{
				_list_frame_name = RKString(source_texture).Split("_");
				for (size_t i = _begin_idx; i <= _end_idx; i ++)
				{
					RKString tex = _list_frame_name.GetAt(i);
					tex.ReplaceAll("\t", "");
					tex.ReplaceAll("\n", "");
					auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(tex.GetString());
					animFrames.pushBack(frame);
				}
				RKString tex_frame_sprite = _list_frame_name.GetAt(_sprite_idx);
				tex_frame_sprite.ReplaceAll("\t", "");
				tex_frame_sprite.ReplaceAll("\n", "");
				_sprite_anim = Sprite::createWithSpriteFrameName(tex_frame_sprite.GetString());
			}

			float delay_time = xml_value->GeneralValue->GetDatafloat("delay");
			int loop = xml_value->GeneralValue->GetDataInt("loop");

			_store_animate_list = CCAnimation::createWithSpriteFrames(animFrames, delay_time, loop);
			_store_animate_list->retain();
		}

		cocos2d::Node * AnimWidget::GetResource()
		{
			return _sprite_anim;
		}

		bool AnimWidget::SetNewResource(cocos2d::Node * resource)
		{
			if (_sprite_anim)
			{
				_sprite_anim = static_cast<Sprite*>(resource);
			}
			else
			{
				return false;
			}

			return true;
		}

		void AnimWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			WidgetEntity::FindAndInsertChildWidget(xml_value);
		}

		void AnimWidget::CloneChild(WidgetEntity * p_clone)
		{
			return;
		}
		
		void AnimWidget::RunAnim(CallFunc * call_back)
		{
			if (_store_animate_list)
			{
				auto t = CCAnimate::create(_store_animate_list);
				Sequence * ac = Sequence::create(t, call_back != nullptr ? call_back : CallFunc::create(CC_CALLBACK_0(AnimWidget::FinishAction , this)), nullptr);
				static_cast<Sprite*>(GetResource())->runAction(ac);
			}
		}

		void AnimWidget::addEventListener(const wAnimEventCallback& callback)
		{
			_eventCallback = callback;
		}

		void AnimWidget::FinishAction()
		{
			if (_eventCallback)
			{
				_eventCallback(this, EventType_Anim::ON_FINISH_ACTION);
			}
		}

		void AnimWidget::ShowSpriteAtFrameIdx(int idx)
		{
			if (idx < 0 || idx >= _list_frame_name.Size())
				return;
			RKString tex = _list_frame_name.GetAt(idx);
			tex.ReplaceAll("\t", "");
			tex.ReplaceAll("\n", "");
			auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(tex.GetString());
			static_cast<Sprite*>(GetResource())->setTextureRect(frame->getRect());
		}
	}
}