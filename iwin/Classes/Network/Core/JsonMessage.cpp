#include "JsonMessage.h"
#include "json/document.h"
#include "json/writer.h"
#include "cocos2d.h"

JsonMessage::JsonMessage()
{

}

JsonMessage::JsonMessage(std::string command)
{
	_json.SetObject();
	_command = command;
}


JsonMessage::~JsonMessage()
{

}


void JsonMessage::setData(rapidjson::Document value)
{
	_json.SetObject();
	rapidjson::Value commandValue(_command.c_str(), _command.size());
	_json.AddMember("command", commandValue, _json.GetAllocator());
	rapidjson::Value cmdData(value, _json.GetAllocator());
	_json.AddMember("data", cmdData, _json.GetAllocator());
	rapidjson::StringBuffer buffer;
	buffer.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_json.Accept(writer);
}

std::string JsonMessage::toJson()
{
	if (!_json.HasMember("command"))
	{
		rapidjson::Value commandValue(_command.c_str(), _command.size());
		_json.AddMember("command", commandValue, _json.GetAllocator());
	}
	rapidjson::StringBuffer buffer;
	buffer.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_json.Accept(writer);

	return buffer.GetString();
}
void JsonMessage::setData(const char* json)
{
	_json.Parse(json);
	_command = _json["command"].GetString();

}
std::string JsonMessage::getCommand()
{
	return _command;
}

std::string JsonMessage::getData()
{
	if (_json.HasMember("data") && !_json["data"].IsNull())
	{
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		_json["data"].Accept(writer);
		const char* json = buffer.GetString();
		return std::string(json);
	}
	return "";
}