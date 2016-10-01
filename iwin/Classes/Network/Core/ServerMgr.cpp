#include <sstream>
#include "cocos2d.h"
#include "RKString_Code/RKString.h"
#include "Utils.h"
#include "ServerMgr.h"
using namespace RKUtils;
using namespace std;

ServerMgr* ServerMgr::s_instance = nullptr;

ServerMgr* ServerMgr::getInstance()
{
	if (!s_instance)
	{
		s_instance = new ServerMgr();
	}
	return s_instance;
}


ServerMgr::ServerMgr()
{

}

ServerMgr::~ServerMgr()
{

}

void ServerMgr::initFromString(std::string serverListStr)
{
	//serverListStr = "Kim Chau | 203.162.51.38:19124;Kim Khanh | 203.162.51.39:19124";
	
	auto list_server = RKString(serverListStr).Split(";");

	for (size_t i = 0; i < list_server.Size(); i++)
	{
		auto sepa = list_server.GetAt(i).Split("|");

		RKString name_server = sepa.GetAt(0);

		auto sepa1 = sepa.GetAt(1).Split(":");
		RKString ip_server = sepa1.GetAt(0);
		RKString port_server = sepa1.GetAt(1);
		_servers.push_back(Server(name_server.GetString(), ip_server.GetString(), port_server.GetString()));
	}

	addServerTests();

	selectServer(0);
}

void ServerMgr::addServerTests()
{
	/*_servers.push_back(Server("Server 34.5", "10.8.34.5", "8888"));
	_servers.push_back(Server("Kim Chau", "203.162.51.38", "8888"));*/
	_servers.push_back(Server("Tuan test", "wss://203.162.51.38", "8888"));
	
}

Server ServerMgr::getCurServer()
{
	return _curServer;
}

void ServerMgr::selectServer(Server server)
{
	_curServer = server;
}

void ServerMgr::selectServer(int idx)
{
	if ((size_t)idx >= _servers.size())
	{
		CCLOG("Wrong server index");
		return;
	}
	_curServer = _servers.at(idx);
}

std::vector<Server> ServerMgr::getServerList()
{
	return _servers;
}