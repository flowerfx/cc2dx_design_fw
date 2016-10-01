#ifndef __CASUALCORE_PLATFORMWIN32_H_
#define __CASUALCORE_PLATFORMWIN32_H_

#include "../Platform.h"

namespace CasualCore
{
  class Win32Input;
  class Win32Gui;
  class CasualFile;

  class PlatformWin32 : public Platform
  {
    Win32Input* input_;
    Win32Gui* gui_;
    ScreenOrientation orientation_;
    int width_;
    int height_;

  public:
    PlatformWin32();
    ~PlatformWin32();

    virtual void Initialize();

    // network reachability function
    // on windows we just assume lan is connected and provide default values
    // Connected for Wifi(LAN) and Internet, but not 3G
    virtual bool IsConnected(int networkType) { return (networkType & (NET_WIFI | NET_INTERNET)) != 0; }		

    virtual void GetDeviceUDID ( char* pUDID );
    virtual void StartOrx(int argc, char** argv);

    virtual void SetDefaultScreenOrientation(ScreenOrientation orientation);
    virtual ScreenOrientation GetScreenOrientation();

    virtual float GetSliderValue(int menuID, int sliderID);
    virtual int GetCheckBoxState(int menuID, int checkBoxID);
    virtual int GetRadioButtonState(int menuID, int radioButtonID);
    virtual void SetMenuComponentVisibility(int menuID, int componentType, int componentID, bool visible);
    virtual void LaunchMenu(int menuID);
    virtual void CloseMenu(int menuID);
    virtual int AddListBoxItem(int menuID, int listBoxID, const char* text);
    virtual int GetSelectedListBoxItem(int menuID, int listBoxID);
    virtual void ClearListBox(int menuID, int listBoxID);
    virtual void GetEditBoxText(int menuID, int editBoxID, std::string& text);

    virtual void GetFile(const char* fileName, CasualFile** outputCasualFile);

    virtual bool SetupAlternatePaths(const char* _URL);

    void UpdateInput();

    virtual void ShowBanner(const int x, const int y);

    virtual void RemoveBanner();

    virtual void LoadFreeCash();

    virtual bool CheckFreeCashReady();

    virtual void ShowFreeCash();

    virtual void RetrieveTapJoyRewardItem(const int ggi, const char* userinfo);

    virtual void LoadIGPView();

    virtual void ShowIGPView();

    virtual void Debug(const char* debugline);

    Win32Gui* GetGui();
  };
}

#endif
