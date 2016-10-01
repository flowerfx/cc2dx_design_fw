#ifndef __CONFIRM_CODE_LAYER_H__
#define __CONFIRM_CODE_LAYER_H__

#include "Screens/ScreenManager.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class ConfirmCodeLayer
{
public:
	ConfirmCodeLayer();
	virtual ~ConfirmCodeLayer();
private:
	BaseScreen * _common_screen;

	/*
	use : show confirm box
	*/
	WidgetEntity * p_confirm_code_panel;
public:
	/*
	handle show confirm box
	*/
	void InitConfirmCode(BaseScreen * screen);
	bool ProcessWidgetConfirmBox(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
	void OnShowConfirmCode();
};

#endif //__ANNOUCE_SERVER_LAYER_H__

