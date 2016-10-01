#ifndef __SCREEN_EVENT_H__
#define __SCREEN_EVENT_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class QuestCategoryList;
	class QuestCategory;
	class Quest;
	class ConfirmBox;
	class ResponseQuestList;
}
class EventScreen : public BaseScreen
{
private:
	iwinmesage::QuestCategoryList * p_list_quest_category;
	iwinmesage::ResponseQuestList * p_current_quest_list;

	WidgetEntity * _list_quest_catalogue;
	WidgetEntity * _list_quest_sample;
	WidgetEntity * _list_quest_empty;

	int			   p_current_idx_quest;
public:
	EventScreen();
	virtual ~EventScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)		override;

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)						override;
	virtual void OnBeginFadeIn()															override;

	void OnReceiveListQuestCataloryJson(std::string str );
	void OnReceiveQuestByCalaloryJson(std::string str_json);
	void OnRequestDataJson();
protected:
	void ShowQuestAtId(int id);
	void ClearListAndDeleteTexture(WidgetEntity * list_clear,RKString path_panel, RKString name_delete);
	int GetTypeID(int action_type);
	int GetNumberLine(RKString str);
	void OnHideAllHLCatagoryList();

	void OnResizeAndInitQuestWidget(WidgetEntity * e, iwinmesage::Quest q);
	int GetLineWithLongString(RKString str, WidgetEntity * _widget);
};

#endif //__SCREEN_EVENT_H__

