#ifndef _PROVINCE_H__
#define _PROWINCE_H__
#include<string>
#include<vector>

class Province
{
public:
	Province(); 
	virtual ~Province();
	int id;
	std::string name;
	//std::string provinceNames[];
	static std::vector<Province*> list;
	static std::string getProviceName(int id);
	
	
};

#endif //_PLAYER_H__