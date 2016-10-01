///----------------------------------------------------------------------
///----------------------------------------------------------------------
#if defined(ANDROID_OS)
#include "RK.h"
#include "RKPlatform.h"

#include <sys/stat.h>

///----------------------------------------------------------------------
///----------------------------------------------------------------------
static uint32 RKPlatform_CoreCount = 1;

///----------------------------------------------------------------------
///----------------------------------------------------------------------
uint32 RKPlatformInternal_GetCoreCount()
{
  struct stat st;
  int nbOfCores = 0;
  int i = 0;
  while (1)
  {
    char folder_name[32];
    sprintf(folder_name, "/sys/devices/system/cpu/cpu%d/", i);
    if (stat(folder_name, &st) == 0)
    {
      nbOfCores++;
    }
    else
    {
      return nbOfCores;
    }
    i++;
  }
}

///----------------------------------------------------------------------
///----------------------------------------------------------------------
void RKPlatform_InitModule()
{
  // RKPlatform_CoreCount = sysconf(_SC_NPROCESSORS_CONF);
  RKPlatform_CoreCount = RKPlatformInternal_GetCoreCount();
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
#endif