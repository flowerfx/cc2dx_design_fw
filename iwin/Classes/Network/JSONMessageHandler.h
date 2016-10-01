#ifndef JSONMESSAGEHANDLER_H
#define JSONMESSAGEHANDLER_H

#include "Network/Global/GlobalLogicHandler.h"
using namespace std;

class JSONMessageHandler
{
private:
	GlobalLogicHandler* globalLogicHandler;
public:
	JSONMessageHandler(GlobalLogicHandler* globallogic);
	~JSONMessageHandler();
	void onMessage(std::string json);
	
};
#endif