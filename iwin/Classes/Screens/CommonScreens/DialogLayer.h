#ifndef __DIALOG_LAYER_H__
#define __DIALOG_LAYER_H__
#include "Screens/ScreenManager.h"
using namespace Utility;
using namespace Utility::UI_Widget;
using namespace iwinmesage;

class DialogLayer
{
private:
	/*
	use : dialog screen here
	*/
	TYPE_DIALOG		p_current_dialog;
	RKString		p_nameBtnProcess;
	//for the call back

	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function0;
	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function1;
	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function2;

	float		   p_time_show_button;

	WidgetEntity * p_dialog_layout;
	WidgetEntity * p_panel_1_btn;
	WidgetEntity * p_panel_2_btn;
	WidgetEntity * p_panel_0_btn;
	WidgetEntity * p_panel_fb_btn;

	WidgetEntity * p_text_title;
	WidgetEntity * p_text_content;

	BaseScreen * _common_screen;
protected:
	void AligneSizePanel();
	void OnProcessBtnFunc();
public:
	DialogLayer();
	virtual ~DialogLayer();
	void InitDialogLayout(BaseScreen * screen);
	bool ProcessWidgetDialog(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
	//call popup method
	void OnShowDialog(RKString str_title = "",
		RKString str_message = "",
		TYPE_DIALOG type = TYPE_DIALOG::DIALOG_NONE,
		const char * strCB = "",
		const std::function<void(const char * call_back, const char* btn_name)> & func = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func1 = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func2 = nullptr,
		RKString title_btn_0 = "", RKString title_btn_1 = ""
	);
	void OnHideDialog();
	int Update(float dt);

};


#endif  //__DIALOG_LAYER_H__

