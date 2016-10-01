
#ifndef __CUSTOM_WEBVIEW_H_
#define __CUSTOM_WEBVIEW_H_

namespace Platform {
    namespace ios{
        void nativeShowWebView(std::string url);
        void nativeHideWebView();
        void nativeMakeCall(std::string phoneNumber);
        void nativeShowSMSComposer(std::string phoneNumber, std::string body);
        std::string nativeGetBundleId();
        std::string nativeGetAppName();
    }
}


#endif //__CUSTOM_WEBVIEW_H__
