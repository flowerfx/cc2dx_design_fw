#ifndef _GUIDE_SERVICE_H__
#define _GUIDE_SERVICE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "network/JsonObject/Guides.h"

using namespace iwinmesage;

class GuideService
{
private:
	const char* URL_REQUEST_ALL_HELP = "http://54.169.9.224:8090/wf/help/help_link";
	static	GuideService*		s_instance;
	Guides						_guides;
	CC_SYNTHESIZE(bool, _isLoadDataFromServerOK, IsLoadDataFromServerOK)
public:
	static GuideService* getInstance();
	GuideService();
	void onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);

	void requestAllHelp();
};

#define GetGuideService() GuideService::getInstance()

#endif //_GUIDE_SERVICE_H__
