#include "ListViewEntity.h"
#include "ButtonEntity.h"
namespace Utility
{
	namespace UI
	{
		ListViewEntity::ListViewEntity()
		{
			p_typeUI = UI_MENU_TYPE::MENU_LISTVIEW;
		}
		ListViewEntity::~ListViewEntity()
		{
		}

		
		bool ListViewEntity::Update(float dt)
		{
			return UpdateActionFade(dt);
		}
		void ListViewEntity::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			VisitResource(renderer, transform, flags);
		}
		void ListViewEntity::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			DrawResource(renderer, transform, flags);
		}

		void ListViewEntity::Init(RKString name_background, RKString name_source, Vec2 pos, Vec2 size, float rotate)
		{
			InitListView(name_source);
			p_Name = name_background;
			SetPositionResource(pos);
			p_originPos = pos;
			((ListView*)GetResource())->setContentSize(Size(size.x,size.y));
			SetSizeResource(size);
			SetRotateResource(rotate);
			p_target_scale_to = p_Resource->getScaleX();
			p_originAngle = rotate;
			p_typeUI = UI_MENU_TYPE::MENU_LISTVIEW;
		}

		void ListViewEntity::InitListView(RKString name_panel)
		{
			auto te = TextureMgr->GetSpriteByName(name_panel);

			p_listView = ListView::create();
			p_listView->retain();

			p_listView->setDirection(ui::ScrollView::Direction::VERTICAL);
			p_listView->setBounceEnabled(true);

			p_listView->setBackGroundImage("", Widget::TextureResType::LOCAL, te->getTextureRect());
			p_listView->setBackGroundImageScale9Enabled(true);

		}

		void ListViewEntity::PushDataToListView(xml::ResUIElement * res)
		{
			/*if (res->type == "button")
			{
				ButtonEntity * btn = new ButtonEntity();
				btn->Init("abc",Vec2(0,0),)
			}

			Layout *custom_item = Layout::create();*/

		}
	}
}