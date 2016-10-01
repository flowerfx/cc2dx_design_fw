#include "Profiler.h"

PDTProfiler* PDTProfiler::s_instance = nullptr;

PDTProfiler* PDTProfiler::getInstance()
{
	if (!s_instance)
	{
		s_instance = new PDTProfiler();
	}
	return s_instance;
}


PDTProfiler::PDTProfiler()
{

}

PDTProfiler::~PDTProfiler()
{

}


void PDTProfiler::startProfile(const char* name)
{
	ProfileInfo* info;
	if (_activeTimers.find(name) != _activeTimers.end())
	{
		info = _activeTimers[name];
	}
	else
	{
		info = new ProfileInfo();
		_activeTimers.insert(std::pair<std::string, ProfileInfo*>(name, info));
	}

	info->_startTime = std::chrono::high_resolution_clock::now();
}

void PDTProfiler::endProfile(const char* name)
{
	ProfileInfo* info;
	if (_activeTimers.find(name) != _activeTimers.end())
	{
		info = _activeTimers[name];
	}
    auto now = std::chrono::high_resolution_clock::now();
	info->_totalTime = static_cast<long>(std::chrono::duration_cast<std::chrono::microseconds>(now - info->_startTime).count());
}

std::string PDTProfiler::getResult()
{
	std::string rs = "";
	char buf[128];

	for (auto it = _activeTimers.begin(); it != _activeTimers.end(); it++)
	{
		sprintf(buf, "%s %ld \n", it->first.c_str(), it->second->_totalTime);
		rs += buf;
	}

	return rs;
}

void PDTProfiler::reset()
{
	for (auto it = _activeTimers.begin(); it != _activeTimers.end(); it++)
	{
		delete it->second;
	}
	_activeTimers.clear();
}