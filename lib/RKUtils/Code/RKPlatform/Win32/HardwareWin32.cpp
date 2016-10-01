#include "Platform/Hardware.h"

using namespace CasualCore;

Hardware::CpuRating Hardware::_PerformCpuBenchmark()
{
  return CPU_HIGH;
}

Hardware::GpuRating Hardware::_PerformGpuBenchmark()
{
  return GPU_HIGH;
}

Hardware::MemRating Hardware::_GetMemoryRating() 
{ 
  return MEM_HIGH; 
}

uint32 Hardware::_GetCpuCoreCount()
{
  SYSTEM_INFO sysinfo;
  GetSystemInfo( &sysinfo );
  return sysinfo.dwNumberOfProcessors;
}

RKString Hardware::_GetDeviceIdString()
{
  return "desktop";
}

RKString Hardware::_GetCacheFolder()
{
  char path[256];
  GetTempPath(ARRAYSIZE(path), path);
  return path;
}