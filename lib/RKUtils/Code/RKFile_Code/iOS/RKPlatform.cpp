#ifdef OS_IOS
#include "RK.h"
#include "RKPlatform.h"
namespace RKUtils
{
static uint32 RKPlatform_CoreCount = 1;

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKPlatform_InitModule()
{
  host_basic_info_data_t hostInfo;
  mach_msg_type_number_t infoCount = HOST_BASIC_INFO_COUNT;
  host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostInfo, &infoCount);
  RKPlatform_CoreCount = hostInfo.max_cpus;
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
}
#endif