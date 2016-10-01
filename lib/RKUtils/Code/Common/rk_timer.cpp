#include "rk_timer.h"
namespace RKUtils
{

	int64 GetTicks() {
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);

		return ticks.QuadPart;
	}

	int64 GetTicksPerSecond() { //maybe cache this value !
		LARGE_INTEGER tickPerSeconds;
		QueryPerformanceFrequency(&tickPerSeconds);

		return tickPerSeconds.QuadPart;
	}

	int64 sTimerTickOffset = 0;

	void OffsetTimer(int64 ticks) {
		sTimerTickOffset += ticks;
	}

	int64 GetMilliseconds() {
		return int64(GetSecondsDouble() * 1000);
	}

	int64 GetMicroseconds() {
		return int64(GetSecondsDouble() * 1000 * 1000);
	}
	int64 GetNanoseconds() {
		return int64(GetSecondsDouble() * 1000 * 1000 * 1000);
	}
}
