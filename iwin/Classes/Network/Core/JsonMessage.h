#pragma once
#include <string>
#include "json/document.h"

class JsonMessage
{
private:
	std::string		_command;
	rapidjson::Document			_json;
public:
	JsonMessage();
	JsonMessage(std::string command);
	virtual ~JsonMessage();

	void setData(rapidjson::Document value);
	void setData(const char* json);
	std::string toJson();
	std::string getCommand();
	std::string getData();

};