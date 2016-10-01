//
// CtrWebOverlay.xaml.cpp
// Implementation of the CtrWebOverlay class
//

#include "pch.h"
#include "CtrWebOverlay.xaml.h"
#include "WinRTStringUtils.h"

//using namespace CocosAppWinRT;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236
namespace CocosAppWinRT
{
	CtrWebOverlay::CtrWebOverlay()
	{
		InitializeComponent();
		float screen_w = (float)Window::Current->Bounds.Width;
		float screen_h = (float)Window::Current->Bounds.Height;

		this->Width = screen_w;
		this->Height = screen_h;
		loading->Width = screen_w;
	}


	void CtrWebOverlay::browser_NavigationFailed(Platform::Object^ sender, Windows::UI::Xaml::Controls::WebViewNavigationFailedEventArgs^ e)
	{
		loading->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		if (actionCallback)
		{
			std::string sstr = pstos(e->Uri->AbsoluteUri);
			actionCallback((void*)this, sstr);
		}
	}


	void CtrWebOverlay::Image_Tap(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
	{
		if (popup_webview) {
			popup_webview->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		
	}


	void CtrWebOverlay::browser_NavigationStarting(Windows::UI::Xaml::Controls::WebView^ sender, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ args)
	{
		loading->Visibility = Windows::UI::Xaml::Visibility::Visible;
		if (actionCallback)
		{
			std::string sstr  = pstos(args->Uri->AbsoluteUri);
			actionCallback((void*)this, sstr);
		}
	}


	void CtrWebOverlay::browser_NavigationCompleted(Windows::UI::Xaml::Controls::WebView^ sender, Windows::UI::Xaml::Controls::WebViewNavigationCompletedEventArgs^ args)
	{
		loading->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}

	void CtrWebOverlay::openURL(Windows::Foundation::Uri^ uri, std::function<void(void*, std::string)> callback)
	{
		actionCallback = callback;
		browser->Navigate(uri);
	}

	void CtrWebOverlay::setParent(Windows::UI::Xaml::Controls::Grid^ parent)
	{
		popup_webview = parent;
	}


}