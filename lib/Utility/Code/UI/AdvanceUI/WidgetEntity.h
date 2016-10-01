#ifndef __WIDGET_ENTITY_H__
#define __WIDGET_ENTITY_H__

#include "ActionProcess.h"
#include "../../LanguageMgr.h"
#include "RKBaseLog/Debug.h"
using namespace RKUtils;
using namespace ui;
namespace Utility
{
	namespace UI_Widget
	{

		class LayerEntity;
		class WidgetEntity : public ActionProcess
		{
		protected:
			//widget object
			Widget*			p_Resource;
			//list child widget
			std::vector<WidgetEntity*> p_listChildWidget;
			//
			bool			p_isScale9Enable;
			bool			p_mark_as_delete_resource;
			//state scale 9 //0 is x and y 1 is x 2 is y
			int				p_stage_scale9; 
            //min size scale
			int				p_min_size_scale;        
			//aligne res
			std::vector<int> p_aligne_res;
			bool			 p_state_aligne;
			//sound id
			xml::sound_click_dec *	ID_SOUND;
		public:
			virtual WidgetEntity * Clone() = 0;
			virtual void CloneChild(WidgetEntity * p_clone);
			virtual void CopyValue(WidgetEntity * data);
			WidgetEntity();
			virtual ~WidgetEntity();

			virtual void InitParamParent(RKString name, xml::UILayerWidgetDec * xml_value, LayerEntity * layer_parent, WidgetEntity * parent_widget);
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value) = 0;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value);
			virtual void InsertChildWidget(WidgetEntity* child_widget);

			virtual cocos2d::Node * GetResource()										override;
			virtual bool SetNewResource(cocos2d::Node * resource)						override;
			virtual bool Update(float dt)												override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void SetPosValueXML(xml::UILayerWidgetDec * xml_value)				override;

			virtual void SetInteractive(bool value)										override;
			virtual void SetDisable(bool value)											override;
			virtual void SetSize(Vec2 s)												override;
			virtual Vec2 GetSize()														override;
			virtual void ReloadText()													override;
			virtual void ClearChild(int count = 0)										override;
			virtual void SetPosition(Vec2 p)											override;	

			void ResizeAndPosNode(cocos2d::Node * resource, WidgetEntity * child_w = nullptr);
			xml::sound_click_dec* GetIdSound() { return ID_SOUND; }

			size_t GetNumberChildWidget() const { return (size_t)p_listChildWidget.size();  }
			bool ContainChildIdxAppear(unsigned int idx_appear);

			int SetProcessActionChildIdxAppear(STATUS_MENU fade_state, unsigned int current_idx);

			virtual WidgetEntity * GetWidgetChildByName(RKString name_path);
			virtual WidgetEntity * GetWidgetChildAtIdx(int idx);

			virtual void ResizeAndPosChildWidget(WidgetEntity * child);
			virtual void RemoveAllChild();
			virtual void AddChildWidget(WidgetEntity * widget_);
			virtual void SetResourceToNull()	;

			bool HaveAligneRes();

			void AddChildWidgetToList(WidgetEntity * child);
			void SetDeleteResource(bool value, bool allChild = true);

			int GetZOrder();
			void SetZOrder(int z);
			void SetVisible(bool visible);
			bool IsVisible();		
			float getWidth();
			float getHieght();
		};
	}
}

#endif //__WIDGET_ENTITY_H__

