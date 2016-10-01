#include "stdafx.h"
#include "LocalDataMgr.h"
#include "common/Tea.h"

#include "GLLegacy/Commons.h"
#include "game/platform/Platform.h"

std::map<std::string , std::string> CLocalDataMgr::s_localCfgMap;
Mutex CLocalDataMgr::s_mutexLocalCfgMap;
CLocalDataMgr::CLocalDataMgr(void)
{
}

CLocalDataMgr::~CLocalDataMgr(void)
{
}

void CLocalDataMgr::SetLocalConfig(const char *mask, const char *value)
{
	CAutoLock lock(&s_mutexLocalCfgMap);
	std::string key = mask;
	std::string rvalue = value;
	std::map<string, string>::iterator it= s_localCfgMap.find(key);
	if(it != s_localCfgMap.end())
	{
		it->second = rvalue;
	}
	else
	{
		s_localCfgMap[key] = rvalue;
	}
}

void CLocalDataMgr::SetLocalConfig(const char *mask, const int value)
{
	char strValue[64] = {0};
	sprintf(strValue, "%d", value);
	strValue[63] = '\0';
	SetLocalConfig(mask, strValue);
}

void CLocalDataMgr::SetLocalConfig(const char *mask, const float value)
{
	char strValue[64] = {0};
	sprintf(strValue, "%f", value);
	strValue[63] = '\0';
	SetLocalConfig(mask, strValue);
}

void CLocalDataMgr::SetLocalConfig(const char *mask, const bool value)
{
	char strValue[64] = {0};
	sprintf(strValue, "%d", value ? 1 : 0);
	strValue[63] = '\0';
	SetLocalConfig(mask, strValue);
}

bool CLocalDataMgr::GetLocalConfig(const char *mask, char *result)
{
	CAutoLock lock(&s_mutexLocalCfgMap);
	result[0] = 0;
	std::string key = mask;
	std::map<string, string>::iterator it= s_localCfgMap.find(key);
	if(it != s_localCfgMap.end())
	{
		sprintf(result, "%s", it->second.c_str());
		int len = it->second.size();
		if(result[len - 1] == 0x0a || result[len - 1] == 0x0d)
			result[len - 1] = 0;
		return true;
	}
	return false;
}

bool CLocalDataMgr::GetLocalConfig(const char *mask, int &result)
{
	char strValue[64] = {0};
	if (GetLocalConfig(mask, strValue)) {
		result = atoi(strValue);
		return true;
	}
	return false;
}

bool CLocalDataMgr::GetLocalConfig(const char *mask, float &result)
{
	char strValue[64] = {0};
	if (GetLocalConfig(mask, strValue)) {
		result = atof(strValue);
		return true;
	}
	return false;
}

bool CLocalDataMgr::GetLocalConfig(const char *mask, bool &result)
{
	char strValue[64] = {0};
	if (GetLocalConfig(mask, strValue)) {
		result = (atoi(strValue) > 0);
		return true;
	}
	return false;
}

bool CLocalDataMgr::RemoveLocalConfig(const char *mask)
{
	std::map<string, string>::iterator it = s_localCfgMap.begin();
	if(strcmp(mask, "ALL_CLEAR") == 0)
	{
		//// remove all
		for(it; it!= s_localCfgMap.end();)
		{
			s_localCfgMap.erase(it++);
		}
		return true;
	}
	else
	{
		std::string key = mask;
		std::map<string, string>::iterator result = s_localCfgMap.find(key);
		if(result != s_localCfgMap.end())
		{
			s_localCfgMap.erase(result);
			return true;
		}
	}
	return false;
}

const int CONFIG_FILE_SIZE = 2048;
void CLocalDataMgr::LoadLocalConfig()
{
	CAutoLock lock(&s_mutexLocalCfgMap);
	ReleaseLocalConfig();

	FILE *file;
	char fileName[256] = {0};
	sprintf(fileName, "%sLocalConfig.cnfg", CPlatform::GetDocumentsDir().c_str());
	DLOG("Saving: %s", fileName);
    DEBUG_OUT("%s fileName=%s", __FUNCTION__, fileName);

	file = fopen(fileName, "rb");
	if(!file)
		return;

	u32 readNum;

	byte buffer[CONFIG_FILE_SIZE] = {0};
	readNum = fread(buffer, sizeof(char), CONFIG_FILE_SIZE, file);

	if(readNum != CONFIG_FILE_SIZE)
	{
		fclose(file);
		return ;
	}

	char tellchar; 
	readNum = fread(&tellchar, sizeof(char), 1, file); //try to read more test if someone add sth after normal file

	if(readNum != 0)
	{
		fclose(file);
		return ;
	}

	fclose(file);

	//decrypt
	TEA tea(16, false);
	const int CRYPT_STEP = 8;
	byte tmp[CRYPT_STEP] = {0};

	int index = 0;
	byte deCryptesult[CONFIG_FILE_SIZE] = {0};
	while(index < CONFIG_FILE_SIZE)
	{
		tea.decrypt((buffer + index), tmp);
		memcpy(deCryptesult + index, tmp, CRYPT_STEP);
		index += CRYPT_STEP;
	}

	std::string conf = (char *)deCryptesult;
	std::string::size_type begin = 0;
	std::string::size_type end = conf.find('\n', begin);
	while(  end != std::string::npos)
	{
		std::string line = conf.substr(begin, end - begin);
		begin = end + 1;
		end = conf.find('\n', begin);

		if(line.length() == 0)
			continue;

		std::string::size_type pos = line.find(' ', 0);
		if(pos != std::string::npos)
		{
			std::string mask = line.substr(0, pos);
			std::string::size_type posn = line.find('\n', pos);
			if(posn == std::string::npos)
				posn = line.find("\x0d", pos);
			std::string value = line.substr(pos + 1, posn == std::string::npos? line.length(): posn - (pos + 1));
			s_localCfgMap[mask] = value;
		}
	}
}

void CLocalDataMgr::SaveLocalConfig()
{
	CAutoLock lock(&s_mutexLocalCfgMap);
	
	FILE *file;
	char fileName[256];
	sprintf(fileName, "%sLocalConfig.cnfg", CPlatform::GetDocumentsDir().c_str());
    DEBUG_OUT("%s fileName=%s", __FUNCTION__, fileName);

	file = fopen(fileName, "w");
	if(!file)
		return;
	//save string first
	char line[256] = {0};
	std::map<std::string, std::string>::iterator it = s_localCfgMap.begin();
	for(; it != s_localCfgMap.end(); it++)
	{
		sprintf(line, "%s %s\n", it->first.c_str(), it->second.c_str());
		fputs(line, file);
	}
	fclose(file);

	file = fopen(fileName, "rb");
	if(!file)
		return;

	byte buffer[CONFIG_FILE_SIZE] = {0};
	fread(buffer, sizeof(char), CONFIG_FILE_SIZE, file);
	fclose(file);
	byte cryptResult[CONFIG_FILE_SIZE] = {0};
	//encrypt

	TEA tea(16, false);
	int index = 0;
	const int CRYPT_STEP = 8;
	byte tmp[CRYPT_STEP] = {0};
	while(index < CONFIG_FILE_SIZE)
	{
		tea.encrypt((buffer + index), (byte *)tmp);
		memcpy(cryptResult + index, tmp, CRYPT_STEP);
		index += CRYPT_STEP;
	}

	////////////test decrypt/////////
	//index = 0;
	//byte testResult[CONFIG_FILE_SIZE] = {0};
	//while(index < CONFIG_FILE_SIZE)
	//{
	//	tea.decrypt((cryptResult + index), tmp);
	//	memcpy(testResult + index, tmp, CRYPT_STEP);
	//	index += CRYPT_STEP;
	//}
	//////////////////////////////////

	file = fopen(fileName, "wb");
	if (!file)//add by zhaoyu, avoid crash
		return;
	fwrite(cryptResult, CONFIG_FILE_SIZE, 1, file);
	fclose(file);
}

void CLocalDataMgr::ReleaseLocalConfig()
{
	CAutoLock lock(&s_mutexLocalCfgMap);
	s_localCfgMap.clear();
}
