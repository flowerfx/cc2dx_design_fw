#ifndef __ANIMATION_WIDGET_H__
#define __ANIMATION_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		enum class EventType_Anim
		{
			ON_FINISH_ACTION
		};
		class AnimWidget : public WidgetEntity
		{
		private:
			Sprite *			_sprite_anim;
			Animation *			_store_animate_list;
			RKList<RKString>	_list_frame_name;
			int		_begin_idx;
			int		_end_idx;
			int		_sprite_idx;
		public:

			typedef std::function<void(Ref*, EventType_Anim)> wAnimEventCallback;

			AnimWidget();
			virtual ~AnimWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual WidgetEntity * Clone()												override;
			virtual cocos2d::Node * GetResource()										override;
			virtual bool SetNewResource(cocos2d::Node * resource)						override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual void CloneChild(WidgetEntity * p_clone)								override;

			void RunAnim(CallFunc * call_back = nullptr);
			void FinishAction();

			void addEventListener(const wAnimEventCallback& callback);

			void ShowSpriteAtFrameIdx(int idx);
		private:
			wAnimEventCallback  _eventCallback;
		};

	}
}
#endif //__ANIMATION_WIDGET_H__

