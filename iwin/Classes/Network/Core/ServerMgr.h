#ifndef __SERVER_MGR_H__
#define __SERVER_MGR_H__

#include <string>
#include <vector>

struct Server
{
	std::string _name;
	std::string _ip;
	std::string _port;
	Server()
	{

	}
	Server(const char* name, const char* ip, const char* port)
	{
		_name = name;
		_ip = ip;
		_port = port;
	}
};

class ServerMgr
{

private:
	std::vector<Server> _servers;

	static ServerMgr*	s_instance;
	Server		_curServer;
	void addServerTests();
public:
	static ServerMgr* getInstance();

	ServerMgr();
	virtual ~ServerMgr();

	void initFromString(std::string serverListStr);
	
	Server getCurServer();
	void selectServer(Server server);
	void selectServer(int idx);
	std::vector<Server> getServerList();
};

#define GetServerMgr() ServerMgr::getInstance()

#endif