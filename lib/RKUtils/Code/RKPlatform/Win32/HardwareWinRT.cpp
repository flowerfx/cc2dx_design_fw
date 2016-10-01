#if defined(OS_W8) || defined(OS_WP8)
#include "Platform/Hardware.h"
#include "WinRTAPIsEmul.h"
//#ifdef OS_W8
#include <DXGI.h>
using namespace std;
DXGI_ADAPTER_DESC * gAdapterInformation = NULL;
extern vector<std::string> ListLowDevice;
std::wstring stows(const std::string& str);
//#endif
using namespace CasualCore;
//Created by Vo Van Khoa.
Hardware::CpuRating Hardware::_PerformCpuBenchmark()
{
  //There is no API to get CPU speed on Windows Phone and Windows 8 :((		
	if (gAdapterInformation == NULL)
	{
		extern D3D_FEATURE_LEVEL			g_d3dFeatureLevel;
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,       // There is no need to create a real hardware device.
			0,
			0,						  // Check for the SDK layers.
			featureLevels,                    // Any feature level will do.
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			nullptr,                    // No need to keep the D3D device reference.
			&g_d3dFeatureLevel,                    // No need to know the feature level.
			nullptr                     // No need to keep the D3D device context reference.
			);

		IDXGIFactory* factory = NULL;
		HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
		if (FAILED(result))
		{
			return CPU_LOW;
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		IDXGIAdapter* adapter = NULL;
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			return CPU_LOW;
		}

		gAdapterInformation = new DXGI_ADAPTER_DESC();
		adapter->GetDesc(gAdapterInformation);

	}

	if (gAdapterInformation)
	{
		wchar_t * dec = gAdapterInformation->Description;

		for (int i = 0; i < ListLowDevice.size(); i++)
		{
			if (wcsstr(dec, stows(ListLowDevice.at(i)).c_str()) > 0)
			{
				return CPU_LOW;
			}
		}

		if (wcsstr(dec, L"Adreno 225") > 0)
		{
			return CPU_MEDIUM;
		}
	}

#ifdef OS_W8
	return CPU_HIGH;
#else
  if (_GetCpuCoreCount() > 2)
  {
	return CPU_HIGH;
  }
  else
  {
	  if (_GetMemoryRating() == MEM_LOW)
	  {
		return CPU_LOW;
	  }
	  else
	  {
		return CPU_MEDIUM;
	  }
  }
#endif
  return CPU_MEDIUM;
}

Hardware::GpuRating Hardware::_PerformGpuBenchmark()
{
  //There is no API to get GPU speed on Windows Phone and Windows 8 :((
  //We assuming that GPU and CPU level are same
  CpuRating cpuRate = _PerformCpuBenchmark();
  if (cpuRate == CPU_HIGH)
  {
	  return GPU_HIGH;
  }
  else if (cpuRate == CPU_LOW)
  {
	  return GPU_LOW;
  }

  return GPU_MEDIUM;
}

Hardware::MemRating Hardware::_GetMemoryRating() 
{ 
#ifdef OS_WP8
	long mem = Windows::Phone::System::Memory::MemoryManager::ProcessCommittedLimit / 1000000;
	if(mem < 190)
		return MEM_LOW; 
#endif
	return MEM_HIGH;
}

uint32 Hardware::_GetCpuCoreCount()
{
  SYSTEM_INFO sysinfo;
  GetNativeSystemInfo( &sysinfo );
  return sysinfo.dwNumberOfProcessors;
}

RKString Hardware::_GetDeviceIdString()
{
  if (gAdapterInformation == NULL)
  {
    _PerformCpuBenchmark();
  }
  if (gAdapterInformation != NULL)
  {
    std::wstring wsDec(gAdapterInformation->Description);
	std::string strDec(wsDec.begin(), wsDec.end());
	return strDec;
  }
  return "desktop";
}

RKString Hardware::_GetCacheFolder()
{
  char path[256];
  GetTempPathEmul(ARRAYSIZE(path), path);
  return path;
}
#endif