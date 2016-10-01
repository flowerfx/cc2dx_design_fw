#pragma once
#include "../Logon/include/logog.hpp"
#include "../RKString_Code/RKString.h"
namespace RKUtils
{
	namespace logog
	{
		class Cout;
		class LogFile;
		class LogBuffer;
	}

	static struct Log
	{
		Log() : enableLogFile(false), cout(NULL), fileBuffer(NULL), fileTarget(NULL) {}
		bool enableLogFile;
		RKUtils::RKString filename;
		RKUtils::logog::Target* cout;
		RKUtils::logog::LogBuffer* fileBuffer;
		RKUtils::logog::LogFile* fileTarget;
	} s_log;
	//

	void InitLogging();

	void SetLoggingEnabled(bool yes);

	void FreeLoggin();
}

///////////////////////////////////////////////////////////////////////////////
