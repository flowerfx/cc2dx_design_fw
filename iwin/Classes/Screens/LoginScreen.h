#ifndef __SCREEN_LOGIN_H__
#define __SCREEN_LOGIN_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

namespace iwinmesage
{
	class CaptchaInfo;
	class CaptchaResult;
}

class LoginScreen : public BaseScreen
{
private:
	WidgetEntity * p_panel_first_choose_login;

	WidgetEntity * p_panel_choose_server;
	WidgetEntity * p_panel_register_user;
	WidgetEntity * p_panel_login_layout;
	WidgetEntity * p_panel_forget_pass;

	WidgetEntity * p_list_server;

	WidgetEntity * p_panel_check_name_1;
	WidgetEntity * p_panel_check_name_input;
	WidgetEntity * p_panel_check_pass_input;
	WidgetEntity * p_panel_check_repass_input;

	WidgetEntity * p_btn_login;
	WidgetEntity * p_btn_create_user;

	WidgetEntity * p_panel_captcha;

	bool		p_already_auto_login;
	RKString	p_id_captcha;
	RKString	p_password;
    RKString    p_username;
	int			p_stage_choose_method_getpass;
protected:
	bool	IsValidString(std::string  str);
	int		IsValidPass(std::string  str);
	void	CheckInputPass(RKString str_pass, RKString str_username);
	bool	IsValidInputUser();
	void	OnCheckEnableBtnCreateUser();
public:
	LoginScreen();
	virtual ~LoginScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)		override;

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)						override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnFadeInComplete()															override;
	virtual void OnTouchMenuBegin(cocos2d::Point p)											override;

	void onSmsSyntaxInfo(s16 subId, std::string content, std::string address, std::string confirmMsg);
	
	void startWaitingDialog();

	void onCallBackCheckUserName(RKString msg);
	void onAutoLogin();
	void onSetServer(int idx_server);

	void SetIDCaptcha(RKString id);
	void onReceiveCaptcha(iwinmesage::CaptchaInfo info);
	void onReceiveCaptchaResult(iwinmesage::CaptchaResult info);
    
	RKString GetPassword();
    RKString GetUsername();

	int     GetStateLoginMenu();
};

#endif //__MENU_LOGIN_H__

