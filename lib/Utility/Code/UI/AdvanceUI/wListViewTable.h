#ifndef __LIST_WIDGET_TABLE_H__
#define __LIST_WIDGET_TABLE_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		enum class EventType_SetCell
		{
			ON_SET_CELL,
			ON_INIT_CELL,

			ON_BEGIN_TOUCH_CELL,
			ON_MOVE_TOUCH_CELL,
			ON_END_TOUCH_CELL,

			ON_BEGIN_DRAG_CELL,
			ON_DRAG_CELL,
			ON_END_DRAG_CELL,

			ON_BEGIN_DROP_CELL,
			ON_DROP_CELL,
			ON_END_DROP_CELL,
		};
		enum class EventType_DragCell
		{
			DRAG_TOP_DOWN = 0,
			DRAG_BOT_UP,
			DRAG_TOP_UP,
			DRAG_BOT_DOWN
		};
		class LayoutWidget;
		class ListViewTableWidget : public WidgetEntity ,public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate
		{
		private:
			LayoutWidget * p_refer_child;
			int current_idx_selected;
			int current_idx_to;
			Vec2 p_size_child;
			size_t number_size_of_cell;
			std::vector<WidgetEntity *> p_list_widget;

			xml::UILayerWidgetDec * p_xml_value;
			extension::TableView *  p_table;

			WidgetEntity * p_widget_on_touch;

			EventType_DragCell     _state_drag_first_end; //0 is top 1 is bot
			int		_state_drag_cell; // 0 is begin 1 is ondrag 2 is end
			bool	_on_end_drag;
			Vec2	_previous_pos;
			Vec2	_delta_pos;
			//track distance move
			Vec2	_current_distance_move;
			Vec2	_first_distance_move;
		public:

			ListViewTableWidget();
			virtual ~ListViewTableWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
		public:

			virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view)override {};
			virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view)override {}
			virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)override;
			virtual void tableCellTouchedBegin(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)override;
			virtual void tableCellTouchedMove(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)override;

			virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx)override;
			virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx)override;
			virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table)override;

			virtual void OnEndDragList(cocos2d::extension::TableView * table, int stage) override;
			virtual void OnBeginDragList(cocos2d::extension::TableView * table, Vec2 pos, int stage) override;
			virtual void OnDragList(cocos2d::extension::TableView * table, Vec2 pos, int stage) override;

			virtual void OnEndDropList(cocos2d::extension::TableView * table, int stage) override;
			virtual void OnBeginDropList(cocos2d::extension::TableView * table, Vec2 pos, int stage) override;
			virtual void OnDropList(cocos2d::extension::TableView * table, Vec2 pos, int stage) override;

			virtual void SetInteractive(bool value) override;

		public:
			WidgetEntity * CreateWidgetRefer(int idx);

			int GetCurrentIdxSelected();
			int GetCurrentIdxTo();
			void SetCurrentIdxSelected(int idx);
			void SetNumberSizeOfCell(size_t number, bool reload_ = false);
			size_t GetNumberSizeOfCell();
			void JumpToItemIdx(int idx);
			void ResetListView();
			void RefreshListView();
			Vec2 GetDeltaPos();
			EventType_DragCell current_stage_drag();
			float GetPercentMoveDistance();
			bool HaveEndDrag();

			void InitTable();

			typedef std::function<void(Ref*, EventType_SetCell, WidgetEntity*)> wSetCellListViewTable;
			void addEventListener(const wSetCellListViewTable& callback);

			std::vector<WidgetEntity *> GetListWidgetSample();
		protected:
			void CycleList(bool forward);
			wSetCellListViewTable _event_callback;
		};

	}
}

#endif //__LIST_WIDGET_H__

