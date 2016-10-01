#ifndef __SCREEN_LOADING_H__
#define __SCREEN_LOADING_H__


#include "ScreenManager.h"


class LoadingScreen : public BaseScreen
{
private:
	int p_current_state_loading;
	int p_state_load_resource;

	int p_state_handle_return_server; //0 is none , 1 is set game type lobby , 2 is set into game screen
	void * p_message_reconnect;
private:
	ThreadWrapper *		  p_thread_load_xml;
	void StartThreadLoadXML();
	bool finish_load_xml;

	int state_load;
	WidgetEntity * p_panel_loading_process;
	void SetStateLoadIdx(int idx, bool load);
public:
	LoadingScreen();
	virtual ~LoadingScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)		override;

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)						override;

	int GetStateLoading();
	void SetStateLoading(int state);
	void SetStateServerReturn(int id) { p_state_handle_return_server = id; }
	void SetMessageHandle(void * msg);
    
    void LoadXMLFile();

};

#endif //__SCREEN_LOADING_H__

