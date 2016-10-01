#ifndef __SCREEN_DIALOG_H__
#define __SCREEN_DIALOG_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

class DialogScreen : public BaseScreen
{
private:
	TYPE_DIALOG		p_current_dialog;

	RKString		p_nameBtnProcess;
	//for the call back
	RKString																	p_strCallback;
	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function;
	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function1;
	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function2;

	float		   p_time_show_button;
	
	WidgetEntity * p_panel_1_btn;
	WidgetEntity * p_panel_2_btn;
	WidgetEntity * p_panel_0_btn;
	WidgetEntity * p_panel_fb_btn;
	WidgetEntity * p_panel_contain;

	WidgetEntity * p_text_title;
	WidgetEntity * p_text_content;

protected:
	void AligneSizePanel();
public:
	DialogScreen();
	virtual ~DialogScreen();

	virtual  int	Init()																override;

	virtual int		Update(float dt)													override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)override;

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)override;
    
	//void functionCallBack
	void callFunction();
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

	virtual void push_layer_to_main_scene(int order = 0) override;


};

#endif //__SCREEN_DIALOG_H__

