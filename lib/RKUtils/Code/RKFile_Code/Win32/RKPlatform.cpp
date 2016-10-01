#include "RK.h"
#include "RKPlatform.h"
#include "Hardware.h"

static uint32 RKPlatform_CoreCount = 1;

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKPlatform_InitModule()
{
#if defined(OS_WP8) || defined(OS_W8)
  SYSTEM_INFO sysinfo;
  GetNativeSystemInfo(&sysinfo);
  RKPlatform_CoreCount = sysinfo.dwNumberOfProcessors;
  if (CasualCore::Hardware::GetInstance()->GetCpuRating() <= CasualCore::Hardware::CPU_MEDIUM)
  {
	  if (RKPlatform_CoreCount >= 2)
		  RKPlatform_CoreCount = RKPlatform_CoreCount / 2;
  }
#else
  RKPlatform_CoreCount = 4;
#endif  
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKPlatform_DeinitModule()
{
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
uint32 RKPlatform_GetCoreCount()
{
  return RKPlatform_CoreCount;
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKPlatform_SetCoreCount(int newCnt)
{
  //[RKModelViewer : to avoid multi thread anim..]
  RKPlatform_CoreCount = newCnt;
}
