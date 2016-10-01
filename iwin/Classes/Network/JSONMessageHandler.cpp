#include "cocos2d.h"
#include "JSONMessageHandler.h"
#include "IwinCommand.h"
#include "Network/Core/JsonMessage.h"

#include "Network/JsonObject/Game/GameList.h"

using namespace iwinmesage;

//namespace util_hash
//{
//
//	typedef std::uint64_t hash_t;
//
//	constexpr hash_t prime = 0x100000001B3ull;
//	constexpr hash_t basis = 0xCBF29CE484222325ull;
//
//	constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
//	{
//		return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
//	}
//	constexpr hash_t _hash(char const* str, hash_t last_value = basis)
//	{
//		return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
//	}
//
//	hash_t hash(char const* str)
//	{
//		hash_t ret{ basis };
//
//		while (*str) {
//			ret ^= *str;
//			ret *= prime;
//			str++;
//		}
//
//		return ret;
//	}
//
//}
JSONMessageHandler::JSONMessageHandler(GlobalLogicHandler* globallogic)
{
	globalLogicHandler = globallogic;
}
JSONMessageHandler::~JSONMessageHandler()
{
	if (globalLogicHandler)
		delete globalLogicHandler;
}
void JSONMessageHandler::onMessage(string json)
{
	JsonMessage* json_message = new JsonMessage();
	json_message->setData(json.c_str());
	string command = json_message->getCommand();
	if (command.empty())
		return;

	CCLOG("JsonMessage: %s", json.c_str());
	/*switch (util_hash::hash(comand.c_str()))
	{*/

	if (command.compare(IwinCommand::SET_CLIENT_INFOR) == 0) {
		globalLogicHandler->onSetClientInfoOK("");

	}else{
		globalLogicHandler->onReceiveMessageJson(json_message->getData(), json_message->getCommand());
	}
}
