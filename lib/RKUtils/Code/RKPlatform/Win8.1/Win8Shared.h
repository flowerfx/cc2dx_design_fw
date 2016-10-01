
#ifndef __WIN8_SHARED_API_H
#define __WIN8_SHARED_API_H

#if defined OS_W8 || defined OS_W10

#define WIN8_APP_VER 0.71

#ifdef WIN8_TOOLKIT_SIDE
#define W8TK_EXPORTED 
#define W8TK_IMPORTED extern
#else
#define W8TK_EXPORTED extern
#define W8TK_IMPORTED 
#endif

#include <string>

class LiveTileHandler;

namespace Win8Toolkit
{
	// holds the full path to the application's package folder
	// will be filed by MetroApp
	// must be defined in the game, preferably the main.cpp
	//W8TK_EXPORTED char PackageFolderPath[4 << 10];

	W8TK_EXPORTED	int			GetWindowWidth();
	W8TK_EXPORTED	int			GetWindowHeight();

	// Virtual Keyboard
	W8TK_EXPORTED	void		ShowVirtualKeyboardToolkit(bool visible, const char* text);
	W8TK_EXPORTED	bool		isShowVirtualKeyboardToolkit();
	W8TK_EXPORTED	std::string	GetVirtualKeyboardTextToolKit();


///////////////////////////////////
	W8TK_EXPORTED	void		IGBrowser_Init(std::string gameCode, std::string gameVersion, std::string oper, std::string hdidfv);
	W8TK_EXPORTED	void		IGBrowser_ShowCustomerCare();
///////////////////////////////////

	// System info
	std::string	GetCarrierName();
	std::string	GetDeviceName();
	std::string GetCountryCodeIso3166();
	std::string GetDeviceLanguageIso639();

	// DEPRECATED - please use GetDeviceName instead of GetDeviceNameToolkit
	W8TK_EXPORTED	const char* GetDeviceNameToolkit();
	W8TK_EXPORTED	const char* GetDeviceFirmwareToolkit();
	W8TK_EXPORTED	const char* GetDeviceManufacturerToolkit();
	W8TK_EXPORTED	const char*	GetProcessorDetailsToolkit();
	W8TK_EXPORTED	float		GetMaxAvailableRamToolkit();
	W8TK_EXPORTED	float		GetFreeRamToolkit();
	W8TK_EXPORTED	float		GetFreeDiskSpaceToolkit();
	W8TK_EXPORTED	int			GetNumberOfCoresToolkit();
	W8TK_EXPORTED std::string	GetOSVersionToolkit();

	// Input info
	W8TK_EXPORTED	const char*	HasHardwareKeyboardInputToolkit();
	W8TK_EXPORTED	const char*	HasMouseInputToolkit();
	W8TK_EXPORTED	const char*	HasTouchInputToolkit();

	// Internet Connection
	enum InternetConnectionType
	{
		NoConnection = 0,
		EthernetConnection = 1,
		WirelessConnection = 2,

		MobileConnection = 3,
		Mobile2GConnection = 4,
		Mobile3GConnection = 5,
		Mobile4GConnection = 6,
		
		UnknownConnection,
	};

	struct InternetConnectionInfo
	{
		InternetConnectionType connectionType;
		bool isRoaming;

		const char* toString();
	};

	W8TK_EXPORTED	InternetConnectionInfo	GetInternetConnectionToolkit();

	W8TK_EXPORTED bool EnableGyroscope(bool enable, float frequency);

	W8TK_EXPORTED bool EnableAccelerometer(bool enable, float frequency);


	W8TK_EXPORTED const wchar_t* GetSaveGameFolderW();
	W8TK_EXPORTED const char* GetSaveGameFolder();


	W8TK_EXPORTED int RemoveFileW(const wchar_t * filename);
	W8TK_EXPORTED int RemoveFile(const char * filename);

	W8TK_EXPORTED int RenameFileW(const wchar_t * filename1, const wchar_t * filename2);
	W8TK_EXPORTED int RenameFile(const char * filename1,const char * filename2);

	W8TK_EXPORTED LiveTileHandler* GetTileNotificationHandler();

	std::string GetGraphicsLevelFeature();
	std::string GetDeviceGPU();
	std::string GetAdvertisingIdentifier();
	
	
	// W8 added
	W8TK_EXPORTED std::wstring ConvertUnicodedFolderPath(const char* fake_path);
};

#endif

#endif // __SHARED_API_H
