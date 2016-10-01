#ifndef __ANNOUCE_SERVER_LAYER_H__
#define __ANNOUCE_SERVER_LAYER_H__

#include "Screens/ScreenManager.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class AnnouceServerLayer
{
public:
	AnnouceServerLayer();
	virtual ~AnnouceServerLayer();
private:
	BaseScreen * _common_screen;
	/*
	handle announce server
	*/

	WidgetEntity * p_annouce_server_layout;
	bool		   p_have_annouce;
	RKString	   p_announce_text;
	bool		   p_annouce_is_running;

public:
	/*
	handle announce server
	*/
	int Update(float dt);
	void InitAnnouceServer(BaseScreen * screen);
	void OnReceiveAnnouncementFromServer(RKString text);
	void forceStopAnnouce(bool value);
	bool ProcessWidgetAnnounce(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
};

#endif //__ANNOUCE_SERVER_LAYER_H__

