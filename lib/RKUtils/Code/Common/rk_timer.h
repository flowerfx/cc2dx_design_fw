
#include <windows.h>
#include <time.h>
#include "RKTypes.h"
namespace RKUtils
{
	long long GetTicks();
	int64 GetTicksPerSecond();
	void OffsetTimer(int64 ticks);
	inline double GetSecondsDouble(int64 offset = 0) {
		return double(GetTicks() - offset) / double(GetTicksPerSecond());
	}
	int64 GetMilliseconds();
	int64 GetMicroseconds();
	int64 GetNanoseconds();
}