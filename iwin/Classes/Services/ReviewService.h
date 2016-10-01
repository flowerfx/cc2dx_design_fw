#ifndef _REVIEW_SERVICE_H__
#define _REIVEW_SERVICE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
//using namespace iwinmesage;

class ReviewService 
{
private:
	//const char* URL_REQUEST_ALL_HELP = "http://54.169.9.224:8090/wf/help/help_link";
	static	ReviewService*		s_instance;
	//Guides						_guides;
	CC_SYNTHESIZE(bool, _isLoadDataFromServerOK, IsLoadDataFromServerOK)
public:
	static ReviewService* getInstance();
	ReviewService();
	void onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);

	void requestAllHelp();
};

#define GetReviewService() ReviewService::getInstance()

#endif //_GUIDE_SERVICE_H__
