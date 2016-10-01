#include "Win8Shared.h"

#if defined OS_W8 || defined OS_W10

#include <Windows.h>
#include <ppltasks.h>
#include <agents.h>
#include <stdio.h>
#include <collection.h>
#include  <thread>

#include <codecvt>
#include <cvt/wstring>

#include <d3d11.h>




static std::string *deviceName = nullptr;
static std::string *deviceManufacturer = nullptr;
static std::string *deviceProcessor = nullptr;

namespace Win8Toolkit
{
	std::string ConvertUtf16ToUtf8(const std::wstring &inputString)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.to_bytes(inputString);
	}

	std::string	GetCarrierName()
	{
		std::string carrierName;
		auto deviceAccountIds = Windows::Networking::NetworkOperators::MobileBroadbandAccount::AvailableNetworkAccountIds;
		if (deviceAccountIds->Size != 0)
		{
			auto mobileBroadbandAccount = Windows::Networking::NetworkOperators::MobileBroadbandAccount::CreateFromNetworkAccountId(deviceAccountIds->GetAt(0));
			carrierName = ConvertUtf16ToUtf8(mobileBroadbandAccount->ServiceProviderName->Data());
		}

		return carrierName;
	}
	
	std::string	GetDeviceName()
	{
		return std::string(GetDeviceNameToolkit());
	}

	std::string GetCountryCodeIso3166()
	{
		std::string countryCode;
		countryCode = ConvertUtf16ToUtf8((ref new Windows::Globalization::GeographicRegion())->CodeTwoLetter->Data());
		if (countryCode == "ZZ")
			countryCode = "N/A";
			
		return countryCode;
	}
	
	std::string GetDeviceLanguageIso639()
	{
		auto languageTag = (ref new Windows::Globalization::Language(Windows::System::UserProfile::GlobalizationPreferences::Languages->GetAt(0)))->LanguageTag;

		// Check
		if (!languageTag->IsEmpty())
		{
			// Convert it to std::string
			std::string languageTagStdString = ConvertUtf16ToUtf8(languageTag->Data());

			// Find if the '-'delimiter exists 
			std::size_t foundDelimiterAt = languageTagStdString.find_first_of('-');

			// Ckeck
			if (foundDelimiterAt != std::string::npos) {
				// Return (en from en-US)
				return languageTagStdString.substr(0, foundDelimiterAt);
			}
			else {
				// Return (en)
				return languageTagStdString;
			}
		}

		return "N/A";
	}
	
	/*
		START - Work in progress..
		Note: don't think the api supports these operations.
	*/
	const char* GetDeviceFirmwareToolkit()
	{
		// No api for such a thing.
		return "Not done.";
	}

	float GetMaxAvailableRamToolkit()
	{
		// Stop trying!
		return 0.0f;
	}

	float GetFreeRamToolkit()
	{
		// Can't you read?
		return 0.0f;
	}
	/*
		END - Work in progress..
	*/

	std::string gSaveGameFolder = "";
	const char* GetSaveGameFolder()
	{
		if (gSaveGameFolder.length() <= 0)
		{
			auto folder = Windows::Storage::ApplicationData::Current->LocalFolder;
			auto path = folder->Path;
			std::wstring wsPath = path->Data();
			gSaveGameFolder = std::string(wsPath.begin(), wsPath.end());
		}

		return gSaveGameFolder.c_str();
	}

	static std::wstring gSaveGameFolderW = L"";
	const wchar_t* GetSaveGameFolderW()
	{
		if (gSaveGameFolderW.length() <= 0)
		{
			auto path = Windows::Storage::ApplicationData::Current->LocalFolder->Path;

			gSaveGameFolderW = std::wstring(path->Data());
		}
		return gSaveGameFolderW.data();
	}

	const char* GetDeviceNameToolkit()
	{
		if ( deviceName == nullptr )
		{
			// Client device information.
			Windows::Security::ExchangeActiveSyncProvisioning::EasClientDeviceInformation info;
			
			// System product name ( aka device name ).
			Platform::String^ prodName = info.SystemProductName;

			deviceName = new std::string( prodName->Begin(), prodName->End() );
		}
		
		return deviceName->c_str();
	}
	
	const char* GetDeviceManufacturerToolkit()
	{
		if ( deviceManufacturer == nullptr )
		{
			// Client device information.
			Windows::Security::ExchangeActiveSyncProvisioning::EasClientDeviceInformation info;

			// System manufacturer ( aka device manufacturer ).
			Platform::String^ devManufacturer = info.SystemManufacturer;
			
			deviceManufacturer = new std::string( devManufacturer->Begin(), devManufacturer->End() );
		}
		
		return deviceManufacturer->c_str();
	}

	std::string GetGraphicsLevelFeature()
	{
		HRESULT hr = E_FAIL;
		D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_9_1;
		D3D_FEATURE_LEVEL FeatureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		hr = D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			FeatureLevels,
			ARRAYSIZE(FeatureLevels),
			D3D11_SDK_VERSION,
			NULL,
			&MaxSupportedFeatureLevel,
			NULL
			);

		std::string featureLevel;

		if (MaxSupportedFeatureLevel == D3D_FEATURE_LEVEL_11_0)
		{
			featureLevel = "11.0";
		}
		else if (MaxSupportedFeatureLevel == D3D_FEATURE_LEVEL_10_1)
		{
			featureLevel = "10.1";
		}
		else if (MaxSupportedFeatureLevel == D3D_FEATURE_LEVEL_10_0)
		{
			featureLevel = "10.0";
		}
		else if (MaxSupportedFeatureLevel == D3D_FEATURE_LEVEL_9_3)
		{
			featureLevel = "9.3";
		}
		else if (MaxSupportedFeatureLevel == D3D_FEATURE_LEVEL_9_2)
		{
			featureLevel = "9.2";
		}
		else if (MaxSupportedFeatureLevel == D3D_FEATURE_LEVEL_9_1)
		{
			featureLevel = "9.1";
		}
		return featureLevel;
	}

	std::string GetDeviceGPU()
	{
		Microsoft::WRL::ComPtr<IDXGIFactory> factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;

		DXGI_ADAPTER_DESC adapterDesc;

		CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&factory);
		factory->EnumAdapters(0, &adapter);
		adapter->GetDesc(&adapterDesc);
		Platform::String^ output = ref new Platform::String(adapterDesc.Description);

		std::wstring wResult(output->Begin());
		std::string result(wResult.begin(), wResult.end());
		return result;
	}

	const char* GetProcessorDetailsToolkit()
	{
		if( deviceProcessor == nullptr )
		{
			using namespace Concurrency;
			using namespace Windows::Foundation;
			using namespace Windows::Devices::Enumeration;

			class InnerClass : public agent
			{
			public:
				InnerClass() : output() {}

				void run()
				{
					
					// Processor GUID.	{97FADB10-4E33-40AE-359C-8BEF029DBDD0}
					// Memory GUID.		{3FD0F03D-92E0-45FB-B75C-5ED8FFB01021}

					std::string guidStr = "{97FADB10-4E33-40AE-359C-8BEF029DBDD0}";

					// Selector string.
					std::string sel = "System.Devices.InterfaceClassGuid:=\"" + guidStr + "\"";
					Platform::String^ selector = ref new Platform::String( std::wstring(sel.begin(), sel.end()).c_str() );

					try
					{
						task<DeviceInformationCollection^> ( DeviceInformation::FindAllAsync( selector, nullptr) ).
							then([this] (DeviceInformationCollection^ devices)
						{
							if( devices != nullptr )
							{
								
								if( devices->Size != 0 )
								{
									// The output is for each logical core, so just process the first one.
									DeviceInformation^ di = devices->GetAt( 0 );

									auto test = di->Id;

									auto it = di->Properties->First();

									while( it->HasCurrent )
									{
										auto key = it->Current->Key;

										if ( key->Equals( L"System.Devices.DeviceInstanceId" ) )
										{
											auto val = it->Current->Value->ToString();

											output = std::string( val->Begin(), val->End() );

											//std::replace( output.begin(), output.end(), '\\', '\n' );
											//std::replace( output.begin(), output.end(), '-', '\n' );
											//std::remove( output.begin(), output.end(), '_' );
										}

										it->MoveNext();
									}
								}
							}

							this->done();
						}
						);

					}
					catch(std::exception& e)
					{
						// Error handling.
						std::string s(e.what());
						std::wstring w(s.begin(), s.end());
						OutputDebugString(w.c_str());

						this->done();
					}
				}

				std::string output;
			};

			InnerClass* ag;

			ag = new InnerClass();
			
			for (;;)
			{
				if(ag == nullptr) 
				{
					OutputDebugString(L"ERROR - Could not create the task to get the processor details.");

					return "";
				}

				if( false == ag->start() )
				{
					OutputDebugString(L"ERROR - Could not start the task to get the processor details.");

					return "";
				}

				concurrency::agent::wait(ag);

				if ( !(ag->output.empty()) )
				{
					deviceProcessor = new std::string( ag->output );
				}

				break;
			}

			if ( ag )
			{
				delete ag;
			}

			return deviceProcessor->c_str();
		}
		else
		{
			return deviceProcessor->c_str();
		}
		
		return "";
		
	}

	int GetNumberOfCoresToolkit()
	{
		SYSTEM_INFO sysInfo;

		GetNativeSystemInfo(&sysInfo);
		
		return sysInfo.dwNumberOfProcessors;
	}

	float GetFreeDiskSpaceToolkit()
	{
		// Get local folder.
		const wchar_t *path = Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data();
		__int64 totalFreeBytes	= 0;
		
		// Get the number of free bytes on the drive containing the local folder.
		if( GetDiskFreeSpaceEx(path, NULL, NULL, (PULARGE_INTEGER) &totalFreeBytes) != 0 )
		{
			return totalFreeBytes / 1024.0f;
		}
		else
		{
			// Error handling.
			char errorMessage[255];
			
			// Get the error code.
			DWORD errorCode = GetLastError(); 
			
			sprintf_s(errorMessage, "ERROR - Could not get disk free space. Error code: %ui", errorCode);

			std::string s(errorMessage);
			std::wstring w(s.begin(), s.end());

			OutputDebugString(w.c_str());
		}
	
		return 0.0f;
	}

	const char* HasHardwareKeyboardInputToolkit()
	{
		return Windows::Devices::Input::KeyboardCapabilities().KeyboardPresent == 1 ? "YES" : "NO";
	};

	const char* HasMouseInputToolkit()
	{
		return Windows::Devices::Input::MouseCapabilities().MousePresent == 1 ? "YES" : "NO";
	}

	const char* HasTouchInputToolkit()
	{
		return Windows::Devices::Input::TouchCapabilities().TouchPresent == 1 ? "YES" : "NO";
	}


	const char* InternetConnectionInfo::toString()
	{
		static const char* InternetConnectionTypeAsString[] = {
			"OFF",
			"ETHERNET",
			"WIFI",

			"MOBILE",
			"2G"
			"3G",
			"4G"
			
			"UNKNOWN"
		};

		return InternetConnectionTypeAsString[connectionType];
	}

	InternetConnectionInfo GetInternetConnectionToolkit()
	{
		using namespace Windows::Networking::Connectivity;
		InternetConnectionInfo result;

		result.connectionType = InternetConnectionType::NoConnection;
		result.isRoaming = false;
		// Get the connection profile associated with the internet connection.
		ConnectionProfile^ profile = NetworkInformation::GetInternetConnectionProfile();

		if ( profile != nullptr)
		{
			/*
			 *	Required client version: 8.1
			*/

			if( profile->IsWlanConnectionProfile )
			{
				result.connectionType = InternetConnectionType::WirelessConnection;
			}
			else if( profile->IsWwanConnectionProfile )
			{
				result.connectionType = InternetConnectionType::MobileConnection;

				WwanConnectionProfileDetails^ details = profile->WwanConnectionProfileDetails;
				WwanDataClass dataClass = details->GetCurrentDataClass();
				ConnectionCost^ cost = profile->GetConnectionCost();

				switch (dataClass)
				{
				case Windows::Networking::Connectivity::WwanDataClass::None:
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Gprs:
					result.connectionType = InternetConnectionType::Mobile2GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Edge:
					result.connectionType = InternetConnectionType::Mobile2GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Umts:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Hsdpa:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Hsupa:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::LteAdvanced:
					result.connectionType = InternetConnectionType::Mobile4GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Cdma1xRtt:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Cdma1xEvdo:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Cdma1xEvdoRevA:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Cdma1xEvdv:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Cdma3xRtt:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Cdma1xEvdoRevB:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::CdmaUmb:
					result.connectionType = InternetConnectionType::Mobile3GConnection;
					break;
				case Windows::Networking::Connectivity::WwanDataClass::Custom:
					break;
				default:
					break;
				}

				result.isRoaming = cost->Roaming;
			}
			else if (profile->NetworkAdapter->IanaInterfaceType == 6)
			{
				result.connectionType = InternetConnectionType::EthernetConnection;
			}
			else
			{
				result.connectionType = InternetConnectionType::UnknownConnection;
			}
		}

		return result;

	}
	/*
	  Template for running an async thread
	*/
	template <typename F>
	auto run_now(F f)->decltype(f())
	{
		typedef decltype(f()) result_type;

		result_type result;
		std::thread operation_wrapper(
			[f, &result]()
		{
			concurrency::task<result_type> operation = concurrency::task<result_type>(f);
			result = operation.get();
		});
		operation_wrapper.join();

		return result;
	}
	/*
	  Some register defines
	*/

	Platform::String^ DEVICE_CLASS_KEY = "{A45C254E-DF1C-4EFD-8020-67D146A850E0},10";
	Platform::String^ DEVICE_DRIVER_VERSION_KEY = "{A8B865DD-2E3D-4094-AD97-E593A70C75D6},3";
	Platform::String^ ROOT_CONTAINER = "{00000000-0000-0000-FFFF-FFFFFFFFFFFF}";
	Platform::String^ ROOT_CONTAINER_QUERY = "System.Devices.ContainerId:=\"" + ROOT_CONTAINER + L"\"";
	Platform::String^ DEVICE_CLASS_KEY_NO_SEMICOLON = "{A45C254E-DF1C-4EFD-8020-67D146A850E0}10";
	Platform::String^ HAL_DEVICE_CLASS = "{4d36e966-e325-11ce-bfc1-08002be10318}";

	std::string GetOSVersionToolkit() {
		
		return run_now(
			[]() -> std::string
		{
			using namespace Platform::Collections;
			using namespace Windows::Devices::Enumeration::Pnp;
			using namespace Concurrency;

			auto properties = ref new Vector<Platform::String^>();
			properties->Append(DEVICE_DRIVER_VERSION_KEY);
			properties->Append(DEVICE_CLASS_KEY);

			auto t = task<PnpObjectCollection^>(PnpObject::FindAllAsync(PnpObjectType::Device, properties, ROOT_CONTAINER_QUERY))
				.then([&](PnpObjectCollection^ containers) -> std::string
			{
				char version[64] = "XX";
				for (unsigned int i = 0; i < containers->Size; i++)
				{
					PnpObject^ container = containers->GetAt(i);
					if (container != nullptr && container->Properties != nullptr)
					{
						auto prop = container->Properties->Lookup(DEVICE_CLASS_KEY_NO_SEMICOLON);
						if (prop != nullptr && prop->ToString()->Equals(HAL_DEVICE_CLASS))
						{
							auto ver = container->Properties->Lookup(DEVICE_DRIVER_VERSION_KEY);
							if (ver != nullptr)
							{
								auto v = ver->ToString();
								wcstombs_s(NULL, version, 64, v->Data(), v->Length());
								return version;
							}
						}
					}
				}
				return version;
			});
			if (t.get().find("6.3") == 0)
				return "8.1";
			if (t.get().find("6.2") == 0)
				return "8.0";
			return t.get();
		});
	}

	// W8 added
	std::wstring ConvertUnicodedFolderPath(const char* fake_path)
	{
		std::string strFakePath(fake_path);
		std::wstring path = L"";

		for (int i = 0; i < strFakePath.size(); i++)
		{
			if (strFakePath[i] == '/')
				strFakePath[i] = '\\';
		}

		int pos = strFakePath.find(".\\C:\\Users");
		if (pos != std::string::npos)
		{
			strFakePath = strFakePath.substr(2);
		}

		pos = strFakePath.find("C:\\Users");
		if (pos != std::string::npos)
		{
			int pos1 = strFakePath.find_first_of("/\\");
			std::string sStr1 = strFakePath.substr(pos1 + 1);
			int pos2 = sStr1.find_first_of("/\\");
			std::string sStr2 = sStr1.substr(pos2 + 1);
			int pos3 = sStr2.find_first_of("/\\");
			std::string sFilePath = sStr2.substr(pos3 + 1);
			std::wstring wsFilePath(sFilePath.begin(), sFilePath.end());

			std::wstring wsDeviceLocalFolder = Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data();
			pos1 = wsDeviceLocalFolder.find_first_of(L"/\\");
			std::wstring wsStr1 = wsDeviceLocalFolder.substr(pos1 + 1);
			pos2 = wsStr1.find_first_of(L"/\\");
			std::wstring wsStr2 = wsStr1.substr(pos2 + 1);
			pos3 = wsStr2.find_first_of(L"/\\");
			std::wstring wsStr3 = wsStr2.substr(pos3 + 1);
			std::wstring wsDirPath = wsDeviceLocalFolder.substr(0, wsDeviceLocalFolder.length() - wsStr3.length());

			path = wsDirPath + wsFilePath;
		}
		else
		{
			pos = strFakePath.find(".\\");
			if (pos != std::string::npos)
			{
				strFakePath = strFakePath.substr(1);
			}
			pos = strFakePath.find(":\\");
			if (pos != std::string::npos)
			{
				std::wstring wsFilePath(strFakePath.begin(), strFakePath.end());
				path = wsFilePath;
			}
			else
			{
				std::wstring wsDirPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Data();
				std::wstring wsFilePath(strFakePath.begin(), strFakePath.end());
				path = wsDirPath + wsFilePath;
			}
		}

		return path;
	}
	
	std::string GetAdvertisingIdentifier(){
		Platform::String^ AI = Windows::System::UserProfile::AdvertisingManager::AdvertisingId;
		return std::string(AI->Begin(), AI->End());
	}
};

#endif