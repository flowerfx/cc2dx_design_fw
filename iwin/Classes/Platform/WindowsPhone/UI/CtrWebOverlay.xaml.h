//
// CtrWebOverlay.xaml.h
// Declaration of the CtrWebOverlay class
//

#pragma once

#include "Platform/WindowsPhone/UI/CtrWebOverlay.g.h"


namespace CocosAppWinRT
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CtrWebOverlay sealed
	{
	public:
		
		CtrWebOverlay();
		void setParent(Windows::UI::Xaml::Controls::Grid^ parent);
	internal:
		void openURL(Windows::Foundation::Uri^ uri, std::function<void(void*, std::string)> callback);
	private:
		std::function<void(void*,std::string)> actionCallback;
		Windows::UI::Xaml::Controls::Grid^ popup_webview;
		void browser_NavigationFailed(Platform::Object^ sender, Windows::UI::Xaml::Controls::WebViewNavigationFailedEventArgs^ e);
		void Image_Tap(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void browser_NavigationStarting(Windows::UI::Xaml::Controls::WebView^ sender, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ args);
		void browser_NavigationCompleted(Windows::UI::Xaml::Controls::WebView^ sender, Windows::UI::Xaml::Controls::WebViewNavigationCompletedEventArgs^ args);
		
	};
}
