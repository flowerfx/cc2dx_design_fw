#ifndef __CHANGE_PASS_LAYER_H__
#define __CHANGE_PASS_LAYER_H__

#include "Screens/ScreenManager.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class ChangePassLayer
{
public:
	ChangePassLayer();
	virtual ~ChangePassLayer();
private:
	BaseScreen * _common_screen;
	/*
	use : change pass layout
	*/

	WidgetEntity * p_layout_change_pass;
	WidgetEntity * p_panel_check_pass_input;
	WidgetEntity * p_panel_check_repass_input;
	WidgetEntity * p_textfield_pass_input;
	WidgetEntity * p_textfield_repass_input;
	WidgetEntity * p_textfield_oldpass_input;
	WidgetEntity * p_btn_changepass;
protected:
	/*
	use : change pass layout
	*/
	int IsValidPass(std::string  str);
	void CheckInputPass(RKString str_pass, RKString str_username);
	void OnCheckEnableBtnChangePass();
	bool IsValidInputPass();
public:
	void InitChangePass(BaseScreen* screen);
	bool ProcessWidgetChangePass(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
	void OnShowChangePassLayout();

};

#endif //__CHANGE_PASS_LAYER_H__

