#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <string>
#include <chrono>
#include <map>

struct ProfileInfo
{
	std::string _profile_name;
	std::chrono::high_resolution_clock::time_point _startTime;
	long _totalTime;
};

class __declspec(dllexport) PDTProfiler
{
private:
	std::map<std::string, ProfileInfo*> _activeTimers;

	static PDTProfiler* s_instance;
public:

	static PDTProfiler* getInstance();
	
	void startProfile(const char* name);
	void endProfile(const char* name);

	std::string getResult();

	void reset();

	PDTProfiler();
	virtual ~PDTProfiler();
};

#define GetProfiler() PDTProfiler::getInstance()

#endif