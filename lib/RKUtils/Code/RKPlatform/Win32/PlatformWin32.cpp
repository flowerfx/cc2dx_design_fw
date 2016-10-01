#include "CasualCoreConfig.h"

#include "Platform.h"
#include "Game.h"
#include "Scene.h"
#include "Win32CasualFile.h"
#include "Win32fbo.h"

#ifdef WINAPI_FAMILY
#	include "WinRTAPIsEmul.h"
#	include <d3d11_1.h>
#	include <d3d11.h>
#	include <wrl\client.h>
using namespace Microsoft::WRL;
#ifdef OS_W8
#	include <windows.ui.xaml.media.dxinterop.h>
#	include <D3D11_2.h>
#	include <DXGI1_3.h>
#	include "metrosockdef.h"
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
#elif defined(OS_WP8)
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")
#	include <D3D11_1.h>
#	include <DXGI1_2.h>
#include <winsock2.h>
#endif
#endif

#include "Graphics/ResourceMap.h"
#include "RK.h"
#include "RKCore.h"
#include "RKDevice.h"
#include "RKHeap.h"
#include "RKIniFile.h"
#include "RKFile.h"
#include "RKRender.h"
#include "RKCamera.h"
#include "RKApplication.h"
#include "DateTime.h"
#include "RKProfileManager.h"
#include "Platform/Hardware.h"
#include "DebugConnection/DebugHost.h"
#if CC_USE_GAMESWF==1
#  include "SWF/SWFManager.h"
#endif
#include "Win32DebugWindow.h"     //AppDebugWindow  CC_USE_DEBUG_WINDOW

#include "RKFederation.h"
#include "InAppPurchase.h"

#include <direct.h>
#include <io.h> //dirent.h
#include <iostream>
#include <fstream>

#include <windowsx.h>
#include <Winsock2.h>
#include <conio.h>
#include <sys/stat.h>

//#define RUN_PATH_TESTS

#include "OnlineService.h"

#ifdef RKDEVICE_GLES
static EGLDisplay g_eglDisplay = EGL_NO_DISPLAY;
static EGLSurface g_eglSurface = EGL_NO_SURFACE;

bool InitRenderingBackend(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
  uint32 glContextTarget = CasualCore::Hardware::GetInstance()->GetGLContextTarget();
	EGLint    eglAttributes[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLint    eglContextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, glContextTarget,
		EGL_NONE
	};
	EGLConfig eglConfig[1];
	EGLint nConfigs;
  RKDevice_SetGLContextLevel(glContextTarget);
	g_eglDisplay = eglGetDisplay(hdc);
	eglInitialize(g_eglDisplay, NULL, NULL);
	eglChooseConfig(g_eglDisplay, eglAttributes, eglConfig, 1, &nConfigs);
	g_eglSurface = eglCreateWindowSurface(g_eglDisplay, eglConfig[0], (NativeWindowType)hwnd, 0); 
	EGLContext eglContext = eglCreateContext(g_eglDisplay, eglConfig[0], EGL_NO_CONTEXT, eglContextAttributes); 
	eglMakeCurrent(g_eglDisplay, g_eglSurface, g_eglSurface, eglContext);

	return true;
}

void SwapRenderingBackend()
{
	eglSwapBuffers(g_eglDisplay, g_eglSurface);
}
#endif

#ifdef RKDEVICE_DX11
ID3D11Device*           g_pD3DDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D*        g_pDepthStencil = NULL;
ID3D11DepthStencilView* g_pDepthStencilView = NULL;
D3D_FEATURE_LEVEL			g_d3dFeatureLevel = D3D_FEATURE_LEVEL_9_1;

#ifdef OS_W8
Microsoft::WRL::ComPtr<IDXGISwapChain1>				g_cpSwapChain = NULL;
SwapChainBackgroundPanel^				g_swapChainPanel = nullptr;
#endif

#ifdef WINAPI_FAMILY
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// Set a breakpoint on this line to catch Win32 API errors.
		throw Platform::Exception::CreateException(hr);
	}
}
#endif

#ifdef WINAPI_FAMILY
bool InitRenderingBackend()
#else
bool InitRenderingBackend(HWND hWnd)
#endif
{

	HRESULT hr = S_OK;
#ifndef WINAPI_FAMILY
	RECT rc; GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
#else
	Windows::Foundation::Size size = Windows::Foundation::Size();
	UINT width = size.Width;
	UINT height = size.Height;
#endif
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

#ifndef WINAPI_FAMILY
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 2;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = TRUE;
#else

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

	swapChainDesc.Width = width; // Match the size of the window.
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
	swapChainDesc.Flags = 0;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;


	// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
	ComPtr<IDXGIDevice3> dxgiDevice;
	ComPtr<ID3D11Device> cpD3DDevice = g_pD3DDevice;
	ThrowIfFailed(
		cpD3DDevice.As(&dxgiDevice)
		);

	ComPtr<IDXGIAdapter> dxgiAdapter;
	ThrowIfFailed(
		dxgiDevice->GetAdapter(&dxgiAdapter)
		);

	ComPtr<IDXGIFactory2> dxgiFactory;
	ThrowIfFailed(
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
		);

	// When using XAML interop, the swap chain must be created for composition.
	ThrowIfFailed(
		dxgiFactory->CreateSwapChainForComposition(
		g_pD3DDevice,
		&swapChainDesc,
		nullptr,
		&g_cpSwapChain
		)
		);

	// Associate swap chain with SwapChainPanel
	// UI changes will need to be dispatched back to the UI thread
	g_swapChainPanel->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([=]()
	{
		// Get backing native interface for SwapChainPanel
		ComPtr<ISwapChainBackgroundPanelNative> panelNative;
		ThrowIfFailed(
			reinterpret_cast<IUnknown*>(g_swapChainPanel)->QueryInterface(IID_PPV_ARGS(&panelNative))
			);

		ThrowIfFailed(
			panelNative->SetSwapChain(g_cpSwapChain.Get())
			);
	}, Platform::CallbackContext::Any));

	// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
	// ensures that the application will only render after each VSync, minimizing power consumption.
	ThrowIfFailed(
		dxgiDevice->SetMaximumFrameLatency(1)
		);

#endif
	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
#ifndef WINAPI_FAMILY
		hr = D3D11CreateDeviceAndSwapChain( NULL, driverTypes[driverTypeIndex], NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pD3DDevice, NULL, &g_pImmediateContext );
#else
		hr = D3D11CreateDevice(
			nullptr,							// Specify nullptr to use the default adapter.
			driverTypes[driverTypeIndex],		// Create a device using the hardware graphics driver.
			nullptr,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
			createDeviceFlags,					// Set debug and Direct2D compatibility flags.
			featureLevels,						// List of feature levels this app can support.
			ARRAYSIZE(featureLevels),			// Size of the list above.
			D3D11_SDK_VERSION,					// Always set this to D3D11_SDK_VERSION for Windows Store apps.
			&g_pD3DDevice,						// Returns the Direct3D device created.
			&g_d3dFeatureLevel,					// Returns feature level of device created.
			&g_pImmediateContext				// Returns the device immediate context.
			);
#endif
		if( SUCCEEDED( hr ) )
			break;
	}
	if( FAILED( hr ) )
		return false;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
		return false;

	hr = g_pD3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) )
		return false;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof(descDepth) );
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
#if defined(OS_WP8) || defined(OS_W8)
	if (g_d3dFeatureLevel < D3D_FEATURE_LEVEL_9_3)
	{
		descDepth.Format = DXGI_FORMAT_D16_UNORM;
	}
#endif
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pD3DDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
	if( FAILED( hr ) )
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pD3DDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
	if( FAILED( hr ) )
		return false;

	RKFramebufferHandle framebuffer;
	framebuffer.pRenderTargetView = g_pRenderTargetView;
	framebuffer.pDepthStencilView = g_pDepthStencilView;
	RKDevice_SetFrameBuffer(framebuffer);
	
	RKDevice_SetViewport(0, 0, width, height);

	return true;

}

void SwapRenderingBackend()
{
	g_pSwapChain->Present(0, 0);
}
#endif

namespace CasualCore
{
  Platform::Platform()
    : m_bKeyboardOpen(false)
    , m_pKeyboardCallback(0)
    , m_pKeyboardCallingObject(0)
    , m_sKeyboardValue("")
    , m_bIsMultiLine(false)
    , m_eVideoState(VIDEO_IDLE)
    , m_pVideoCallback(0)
    , m_pVideoCallingObject(0)
    , depthtexture_(0)
  {
    RKLOG("Platform::Platform()");
    _set_error_mode(_OUT_TO_MSGBOX);
    SetViewportScale(1);
  }

  Platform::~Platform()
  {
#if CC_USE_DEBUG_WINDOW==1
    //not yet.. still needed to log leak info to logfile as long as possible..   AppDebugWindow::dbwDestroy();
#endif
    RKLOG("Platform::~Platform()");
  }

  size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) 
  {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
  }

  void Platform::GetDeviceUDID ( char* pUDID )
  {
    assert(NULL != pUDID);
    strcpy(pUDID, "9daec769ab0ec1d4ada5fb2a560418c7600aa01a");
  }

  // network reachability function
  // on windows we just assume lan is connected and provide default values
  // Connected for Wifi(LAN) and Internet, but not 3G
  bool Platform::IsConnected(int networkType)
  {
    unsigned int uiNet = (unsigned int)networkType;
      unsigned int uiNetworkMask = NET_WIFI | NET_INTERNET;/*| (isHostReachableHeartbeat ? NET_INTERNET : 0)*/;
    return (uiNet & uiNetworkMask)!=0;
  }

  void Platform::LockOrientation( bool lock )
  {
    // do nothing on windows;
  }

  bool Platform::IsOrientationLocked() const
  {
    return false;
  }

  void Platform::OnKeyDown(unsigned char key, int x, int y)
  {
    // OJW - 20120515 - Adding functionality that mimics a single text field input ala the iOS build
    if (m_bKeyboardOpen)
    {
      if (key == 8) // backspace
      {
        if (!m_sKeyboardValue.empty())
        {
          std::string::iterator it = m_sKeyboardValue.end();
          it--;
          m_sKeyboardValue.erase(it);
        }
      }
      else if (key == 13) // CR
      {
        if (m_bIsMultiLine)
        {
          Platform::m_sKeyboardValue += "\n";
        }
        else
        {
          CloseKeyboard();
          return;
        }
      }
      else
      {
        m_sKeyboardValue += key;
      }

      OnKeyboardEvent(KEYBOARD_CHANGED, m_sKeyboardValue.c_str());
    }
  }

  enum Win32Flags
  {
    LEFTBUTTONDOWN = 1,
    RIGHTBUTTONDOWN = 1 << 1
  };
  int flags = 0;


#ifndef WINAPI_FAMILY
  bool GetKeyFromWPARAM(WPARAM& wParam)
  {

    // skip certain control keys
    if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_CAPITAL || wParam == VK_MENU)
      return false;

    // get caps and shift state
    int iCaps = GetAsyncKeyState(VK_CAPITAL);
    int iShift = GetAsyncKeyState(VK_SHIFT);
    // fix lowercase letters
    if (wParam >= 65 && wParam <= 90)
    {
      if ((iCaps | iShift) == 0)
        wParam += 32;
      return true;
    }
    // fix SHIFT-numbers
    else if (wParam >= 48 || wParam <= 57)
    {
      if (iShift != 0)
      {
        switch(wParam)
        {
        case '1': wParam = '!'; break;
        case '2': wParam = '@'; break;
        case '3': wParam = '#'; break;
        case '4': wParam = '$'; break;
        case '5': wParam = '%'; break;
        case '6': wParam = '^'; break;
        case '7': wParam = '&'; break;
        case '8': wParam = '*'; break;
        case '9': wParam = '('; break;
        case '0': wParam = ')'; break;
        };
      }

      return true;
    }

    return false;

  }

  LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    static float lasty = 0.0f;
    float x, y;
    switch (msg)
    {
    case WM_CLOSE:
      PostQuitMessage(0);
      return 0;

    case WM_KEYDOWN:
      Game::GetInstance()->GetScene()->InjectKeyDown( wParam );
      break;

    case WM_KEYUP:
      {
        Game::GetInstance()->GetScene()->InjectKeyUp( wParam );
      }
      break;

    case WM_LBUTTONDOWN:
      x = (float)GET_X_LPARAM(lParam);
      y = (float)GET_Y_LPARAM(lParam);
      Game::GetInstance()->GetScene()->InjectTouchDown(x, y);
      flags |= LEFTBUTTONDOWN;
      break;

    case WM_RBUTTONDOWN:
      lasty = (float)GET_Y_LPARAM(lParam);
      flags |= RIGHTBUTTONDOWN;
      break;

    case WM_LBUTTONUP:
      x = (float)GET_X_LPARAM(lParam);
      y = (float)GET_Y_LPARAM(lParam);
      Game::GetInstance()->GetScene()->InjectTouchUp(x, y);
      flags &= ~LEFTBUTTONDOWN;
      break;

    case WM_RBUTTONUP:
      flags &= ~RIGHTBUTTONDOWN;
      break;

    case WM_MOUSEWHEEL:
      x = (float)GET_X_LPARAM(lParam);
      y = (float)GET_Y_LPARAM(lParam);
      Game::GetInstance()->GetScene()->InjectPinch(x, y, y, y - GET_WHEEL_DELTA_WPARAM(wParam) / 2);
      break;

    case WM_MOUSEMOVE:
      x = (float)GET_X_LPARAM(lParam);
      y = (float)GET_Y_LPARAM(lParam);
      if (flags & LEFTBUTTONDOWN)
        Game::GetInstance()->GetScene()->InjectTouchDrag(x, y);
      else if (flags & RIGHTBUTTONDOWN)
      {
        Game::GetInstance()->GetScene()->InjectPinch(x, y, y, lasty);
        lasty = y;
      }
      break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);

  }

  inline HWND CreateGameWindow(int width, int height)
  {
    WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon   = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		wc.hIconSm = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "CasualCore";

    RegisterClassEx(&wc);

    RECT wRect;
    wRect.left = 0L;
    wRect.right = (long)width;
    wRect.top = 0L;
    wRect.bottom = (long)height;

    AdjustWindowRectEx(&wRect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

    HWND hwnd = CreateWindowEx(
      WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
      "CasualCore", "CasualCore Viewer",
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      0, 0,
      wRect.right - wRect.left, wRect.bottom - wRect.top, 
      NULL, NULL,
      GetModuleHandle(NULL),
      NULL
      );

    return hwnd;
  }

#endif

  static void GetDeviceEmulationSize(const char* pDeviceType, int& iOutWidth, int& iOutHeight)
  {
    bool bSelected = false;
    if (_stricmp("DEVICE_IPHONE3G", pDeviceType)==0) {
      iOutWidth = 480; iOutHeight = 320; bSelected = true;
    }
    if (strcmp("DEVICE_IPHONE3GS", pDeviceType)==0) {
      iOutWidth = 480; iOutHeight = 320; bSelected = true;
    }
    if (strcmp("DEVICE_IPHONE4", pDeviceType)==0) {
      iOutWidth = 960; iOutHeight = 640; bSelected = true;
    }
    if (strcmp("DEVICE_IPHONE4S", pDeviceType)==0) {
      iOutWidth = 960; iOutHeight = 640; bSelected = true;
    }
    if (strcmp("DEVICE_IPHONE5", pDeviceType)==0) {
      iOutWidth = 1136; iOutHeight = 640; bSelected = true;
    }
    if (strcmp("DEVICE_IPOD1", pDeviceType)==0) {
      iOutWidth = 480; iOutHeight = 320; bSelected = true;
    }
    if (strcmp("DEVICE_IPOD2", pDeviceType)==0) {
      iOutWidth = 480; iOutHeight = 320; bSelected = true;
    }
    if (strcmp("DEVICE_IPOD3", pDeviceType)==0) {
      iOutWidth = 480; iOutHeight = 320; bSelected = true;
    }
    if (strcmp("DEVICE_IPOD4", pDeviceType)==0) {
      iOutWidth = 960; iOutHeight = 640; bSelected = true;
    }
    if (strcmp("DEVICE_IPOD5", pDeviceType)==0) {
      iOutWidth = 1136; iOutHeight = 640; bSelected = true;
    }
    if (strcmp("DEVICE_IPAD1", pDeviceType)==0) {
      iOutWidth = 1024; iOutHeight = 768; bSelected = true;
    }
    if (strcmp("DEVICE_IPAD2", pDeviceType)==0) {
      iOutWidth = 1024; iOutHeight = 768; bSelected = true;
    }
    if (strcmp("DEVICE_IPAD3", pDeviceType)==0) {
      iOutWidth = 2048; iOutHeight = 1536; bSelected = true;
    }
    if (strcmp("DEVICE_IPAD4", pDeviceType)==0) {
      iOutWidth = 2048; iOutHeight = 1536; bSelected = true;
    }
    if (strcmp("APPLETV_720p", pDeviceType)==0) {
      iOutWidth = 1280; iOutHeight = 720; bSelected = true;
    }
    if (strcmp("APPLETV_1080p", pDeviceType)==0) {
      iOutWidth = 1920; iOutHeight = 1080; bSelected = true;
    }
    if (!bSelected) {
      iOutWidth = 1024; iOutHeight = 768;
    }
  }


 void Platform::Start(int argc, char** argv)
 {
   CasualCoreOnline::CCOnlineService::Get()->OnAppLaunch();
    int width = 1024, height = 768;

    // First command line parameter is device type to emulate. 
    // If this is present we do not use the debug window to select screen resolution
    if (argc > 1)
    {
      CasualCore::GetDeviceEmulationSize(argv[1], width, height);
    }
    else
    {
    // [Liam] Launch window requests starting resolution before game window is launched. 
#if CC_USE_DEBUG_WINDOW == 1
      AppDebugWindow* dbw = AppDebugWindow::dbwGetInstance();
      if (dbw->GameWindowLaunchSizeSelected())
        dbw->GetSelectedLaunchSize(width, height);
      else
#endif
      {
        ReadScreenResolutionFromIni(width, height);
      }
    }

    RKTexture_SetDeviceNeedHalfSize(Hardware::GetInstance()->GetMemoryRating() <= Hardware::MEM_LOW);   //to set to true by CasualCore if PLATFORM->GetDeviceCalibre() <= CasualCore::LOW_CALIBRE
    RKTexture_SetDeviceNeedSimplifiedShaders(Hardware::GetInstance()->GetGpuRating() < Hardware::GPU_MEDIUM);
    Game::GetInstance()->UpdateClientId();

    RKDevice_SetWidth(width);
    RKDevice_SetHeight(height);
#ifndef WINAPI_FAMILY
    if (HWND hwnd = CreateGameWindow(width, height))
    {
      // Init
      ShowWindow(hwnd, SW_SHOW);      

			const bool bInited = InitRenderingBackend(hwnd);
			RKASSERT(bInited, "Unable to init renderer, make sure you have DirectX debug runtime installed and enabled");

#if CC_USE_DEBUG_WINDOW==1
      AppDebugWindow::dbwCreate(hwnd, argc, argv);  // Redundant, this gets called from AppDebugWindow::dbwRedirectStdio, before the game window gets created
      AppDebugWindow::dbwGetInstance()->SetGameWindowHandle(hwnd);  // Because the weirdness where the AppDebugWindow gets created before the game window gets created
#endif

      Initialize();

      Win32FBO win32fbo;
      win32fbo.Init();
      depthtexture_ = win32fbo.GetDepthTexture();

      // Loop
      MSG msg;
      ZeroMemory(&msg, sizeof(msg));
      while (msg.message != WM_QUIT)
      {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
#if CC_USE_GAMESWF==1
        // Render GameSWF objects
        SWFManager* pFlashMgr = Game::GetInstance()->GetFlashManager();
        if (pFlashMgr)
          pFlashMgr->Render();
#endif
        Game::GetInstance()->Update();
        win32fbo.BufferRenders();
        RKCore_OnDraw();
        win32fbo.BlitToScreen();



				SwapRenderingBackend();

#if CC_USE_DEBUG_WINDOW==1
        AppDebugWindow::dbwUpdate();
#endif
      }

      RKHeapError_EnableReport(false);    //avoid full mem dump now (eg double release), will be re-enabled at the last moment..
      //moved in ~Game(), same as iOS..  RKApplication_SetExiting();

      win32fbo.Deinit();

      // Cleanup
      RKLOG_INFO("Platform::Start(): End of Windows loop: quitting...");

      Game::DestroyInstance(); //will do RKDELETE(Game::GetInstance());

      DestroyWindow(hwnd);

#if CC_USE_DEBUG_WINDOW==1
        AppDebugWindow::dbwDestroy();
#endif
    }
#else
#	ifdef OS_W8
		const bool bInited = InitRenderingBackend();
		RKASSERT(bInited, "Unable to init renderer, make sure you have DirectX debug runtime installed and enabled");
#	endif
	
#endif
  }

  ScreenOrientation Platform::GetScreenOrientation() const
  {
    return orientation_;
  }

  DeviceOrientation Platform::GetDeviceOrientation() const
  {
    return m_deviceOrientation;
  }

  void Platform::GetFile(const char* fileName, CasualFile** outputCasualFile)
  {
    Win32CasualFile* pFile = RKNEW(Win32CasualFile(fileName));
    *outputCasualFile = static_cast<CasualFile*>(pFile);
  }

  void Platform::PostLocalNotification(const char* message, const char* action, int delayInSeconds, int a_iTrackingId, bool showBadge)
  {
    // TODO: not done
  }

  void Platform::CancelLocalNotifications()
  {
    // TODO: not done
  }

  void Platform::OpenURL(const char* url)
  {
    // TODO: not done
  }

  void Platform::PlayVideo(const char* pFilename, const char* pSubtitles, VideoCallback pCallback, void* pCallingObject)
  {
    // NOT IMPLEMENTED ON WINDOWS
    m_eVideoState = VIDEO_COMPLETE;
    if( pCallback )
      pCallback(m_eVideoState, pCallingObject ); 
  }

  void Platform::StopVideo()
  {
    // NOT IMPLEMENTED ON WINDOWS
    m_eVideoState = VIDEO_STOPPED;
  }

  void Platform::SetVideoSkipButton(const char* sTextKey, const char* sBtnUpImage, const char* sBtnDownImage, int x, int y, int w, int h)
  {
    // NOT IMPLEMENTED ON WINDOWS
  }

  void Platform::SetVideoDownloadOverlay(const char* sLocalisedText, int progress)
  {
    // NOT IMPLEMENTED ON WINDOWS
  }

  void Platform::OpenKeyboard(KeyboardCallback pCallback, void* pCallingObject, const char* sInitialTextValue, bool bMultiLine, bool bNumeric)
  {
    // OJW - 20120515 - Adding functionality that mimics a single text field input ala the iOS build
    //if (!m_bKeyboardOpen)
    //{
    m_sKeyboardValue = sInitialTextValue;

    m_pKeyboardCallback = pCallback;
    m_pKeyboardCallingObject = pCallingObject;

    m_bIsMultiLine = bMultiLine;

    m_bKeyboardOpen = true;

    OnKeyboardEvent(KEYBOARD_OPEN, m_sKeyboardValue.c_str());
    //}
  }
  
  void Platform::setKeyboardInputText(const char* a_sText)
  {
    m_sKeyboardValue = a_sText;
  }
  
  void Platform::CloseKeyboard()
  {
    // OJW - 20120515 - Adding functionality that mimics a single text field input ala the iOS build
    if (m_bKeyboardOpen)
    {
      m_bKeyboardOpen = false;
      OnKeyboardEvent(KEYBOARD_CLOSED, m_sKeyboardValue.c_str());
    }
  }

  /**
  * IOS Keyboard Updating, Native code can call this via the singleton
  */
  void Platform::OnKeyboardEvent(int keyboardStatus, const char* sTextValue)
  {
    if (m_pKeyboardCallback)
    {
      m_pKeyboardCallback(keyboardStatus, sTextValue, m_pKeyboardCallingObject);
    }
  }

  void Platform::OpenDatePicker(DatePickerCallback pCallback, void* pCallingObject, DATE_PICKER_MODE eMode)
  {

  }
    
  void Platform::CloseDatePicker()
  {

  }
      
  void Platform::OnDatePickerEvent(int datePickerStatus, DateTime dateInfo)
  {

  }

void Platform::PrintStringFromLocale(float amount, const char* localeID, char* buffer, int& bufferLen) const
  {
    // This is the default implementation on Win32
    bufferLen = snprintf(buffer, bufferLen, "$%.2f", amount);
    LOG(buffer);
  }

  void Platform::PrintStringFromLocaleW(float amount, const char* localeID, wchar_t* buffer, int& bufferLen) const
  {
    // This is the default implementation on Win32
    bufferLen = swprintf(buffer, bufferLen, L"$%.2f", amount);    
  }

  void Platform::PrintStringFromCurrency(float amount, const char* currency,const char* defaultCode, char* buffer, int& bufferLen) const
  {
    // This is the default implementation on Win32
    bufferLen = snprintf(buffer, bufferLen, "$%.2f", amount);
    LOG(buffer);
  }

  void Platform::ShowActivityIndicator(){}
  void Platform ::HideActivityIndicator()
  {}
  void Platform::SetIconBadgeNumer(int number)
  {
    // Does nothing on Win32
  }

  std::string Platform::GetBundleVersion() const
  {
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#if defined(CC_GAME_VERSION)
    std::string sVersion = ""STRINGIZE(CC_GAME_VERSION);
    return sVersion;
#else
    std::string sVersion = "0.0.3";
    return sVersion;
#endif
  }

  void Platform::GetMacAddress(const size_t stringLength, char* outMacAddress) const
  {
    // In GLOT it uses this mac address for windows platform 
    strncpy(outMacAddress, "AA:AA:AA:AA:AA:AA", 64);
  }

  /*
  *This Function is used to show text using the ios dialog box
  * tag sucess dialog = 1 ,error dialog= 2
  */
  void Platform::showDialog(const char* a_UTF8Msg, const char* a_UTF8Titlemsg,const char* a_UTF8BtnText, const char* a_UTF8Btn2Text, DialogCallback callback)
  {
  }

  /*
  * Returns nothing since we do not need windows device information
  */
  void Platform::GetDeviceInfo(const DeviceInfo infoKey, const size_t stringLength, char* outInfo)
  {
  }

  long long Platform::GetFreeDiskSpace()
  {
    return 10485761024;
  }

  void Platform::SubscribeGesture(unsigned int gesture)
  {
  }

  void Platform::UnsubscribeGesture(unsigned int gesture)
  {
  }

  void Platform::UnsubscribeAllGestures()
  {
  }

#if defined GAME_WIN32 && !defined(WINAPI_FAMILY)
  void Platform::ReadScreenResolutionFromIni(int& iOutWidth, int& iOutHeight)
  {
    bool askRes = false;
    if (RKIniFile* inifile = RKIniFile_Open("w32config.ini"))
    {
      RKIniFile_GetBoolean(inifile, "askRes", askRes);
      if (!RKIniFile_GetInt(inifile, "width", iOutWidth))
        iOutWidth = 1024;
      if (!RKIniFile_GetInt(inifile, "height", iOutHeight))
        iOutHeight = 768;
      RKIniFile_Close(&inifile);
    }

    // if askRes=true in w32config.ini, ask for resolution when app starts
    if (askRes)
    {
      char menuRes;

      RKLOG("A. 480 x 320");
      RKLOG("B. 800 x 480");
      RKLOG("C. 854 x 480");
      RKLOG("D. 960 x 540");
      RKLOG("E. 1024 x 580");
      RKLOG("F. 1024 x 600");
      RKLOG("G. 1196 x 720");
      RKLOG("H. 1280 x 720");
      RKLOG("I. 1280 x 752");
      RKLOG("J. 1280 x 800");
      RKLOG("K. 1024 x 768 [master]");
      RKLOG("Choose screen resolution: ");
      menuRes=_getch();

      switch(menuRes){
      case 'a':
      case 'A':iOutWidth=480;iOutHeight=320;break;
      case 'b':
      case 'B':iOutWidth=800;iOutHeight=480;break;
      case 'c':
      case 'C':iOutWidth=854;iOutHeight=480;break;
      case 'd':
      case 'D':iOutWidth=960;iOutHeight=540;break;
      case 'e':
      case 'E':iOutWidth=1024;iOutHeight=580;break;
      case 'f':
      case 'F':iOutWidth=1024;iOutHeight=600;break;
      case 'g':
      case 'G':iOutWidth=1196;iOutHeight=720;break;
      case 'h':
      case 'H':iOutWidth=1280;iOutHeight=720;break;
      case 'i':
      case 'I':iOutWidth=1280;iOutHeight=752;break;
      case 'j':
      case 'J':iOutWidth=1280;iOutHeight=800;break;
      case 'k':
      case 'K':iOutWidth=1024;iOutHeight=768;break;
      default :iOutWidth=480;iOutHeight=320;break;
      }
    }
  }
#endif

  void Platform::CopyToClipboard(const char* a_string)
  {

  }

  void Platform::EnableMSAA(bool enabled)
  {
  }

	extern const char* GetGameDebugName();
}


#ifndef WINAPI_FAMILY
void _InitWinsock()
{
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed: %d\n", iResult);
    exit(1);
  }
}

bool IsExistingDir(const char* path)
{
	struct stat status;
	if (stat(path, &status) == 0)
	{
		return (status.st_mode & S_IFDIR) != 0;
	}
	return false;
}

RKString GetCurrentWorkDir()
{
	char currentDirChars[1024];
	_getcwd(currentDirChars, ARRAYSIZE(currentDirChars));
	return RKString(currentDirChars);
}

void FindWorkDir(const char* workDirName)
{	
	RKString currentDir = GetCurrentWorkDir();
	while (true)
	{
		RKString dataDir = currentDir + "/" + workDirName;
		if (IsExistingDir(dataDir.GetString()))
		{
			_chdir(dataDir.GetString());

			// calling getcwd again to get a clean, absolute path
			RKLOG("Using data folder: %s", GetCurrentWorkDir().GetString());
			return;
		}

		{
			RKString oldDir = currentDir;

			_chdir("..");
			currentDir = GetCurrentWorkDir();

			// if we reached root folder
			if (currentDir == oldDir)
			{
				break;
			}
		}
	}

	RKLOG_FATAL("Unable to find working directory with name: %s", workDirName);
	RKASSERT(false, "Unable to find suitable working directory");
}

extern "C" int CasualMain(int argc, char** argv);

int main(int argc, char** argv)
{
  RKHeap_InitModule();
  CasualCore::Hardware::Create();
  _InitWinsock();

	FindWorkDir("data");

#if CC_USE_DEBUG_HOST==1	
  dbg::DebugHost::Create(CasualCore::GetGameDebugName(), "PC", CC_DEBUG_HOST_META_IP, CC_DEBUG_HOST_META_PORT, CC_DEBUG_HOST_LISTENING_PORT);
#endif

#if CC_USE_DEBUG_WINDOW==1
  AppDebugWindow::dbwRedirectStdio(argc, argv);
  // always ask for window size 
  AppDebugWindow::dbwGetInstance()->WaitForLaunchSizeSelection();
#endif

  CasualCore::Game::GetInstance(); // instantiate first to thwart naughty behavior

  int rc = CasualMain(argc, argv);
  
#if CC_USE_DEBUG_HOST==1
  if (0) //to fix!
  {
    dbg::DebugHost::Destroy();      //hang trying to stop threads ... skip with debugger to continue / memory leak hunt [or set CC_USE_DEBUG_HOST to 0]
  }
#endif

  CasualCore::Hardware::Destroy();
  //RKHeap_DeinitModule();   can't release elephant now.. still global ctor allocations.. will be released in doexit()..   
  RKHeapError_EnableReport(true); //re-enable full memory dump if leaks

  return rc;
}
#endif
