LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := utility_static

LOCAL_MODULE_FILENAME := libutility 

LOCAL_SRC_FILES := \
./Code/DisplayView.cpp \
./Code/FileManager.cpp \
./Code/InputManager.cpp \
./Code/LanguageMgr.cpp \
./Code/LoadDataXML.cpp \
./Code/ObjectTexture.cpp \
./Code/SoundManager.cpp \
./Code/wTool.cpp \
./Code/Background/BGObject.cpp \
./Code/Background/ObjectBGEntity.cpp \
./Code/FXFlatform/XFlatform.cpp \
./Code/Object/wObject.cpp \
./Code/Social/SocialManager.cpp \
./Code/Social/Downloader/DownloadHandler.cpp \
./Code/Social/GameCenter/GameCenterController.cpp \
./Code/UI/AdvanceUI/ActionCommand.cpp \
./Code/UI/AdvanceUI/ActionProcess.cpp \
./Code/UI/AdvanceUI/LayerEntity.cpp \
./Code/UI/AdvanceUI/Screen.cpp \
./Code/UI/AdvanceUI/wAnimationWidget.cpp \
./Code/UI/AdvanceUI/wButton.cpp \
./Code/UI/AdvanceUI/wCheckBox.cpp \
./Code/UI/AdvanceUI/wEditBox.cpp \
./Code/UI/AdvanceUI/WidgetEntity.cpp \
./Code/UI/AdvanceUI/wLayout.cpp \
./Code/UI/AdvanceUI/wListView.cpp \
./Code/UI/AdvanceUI/wListViewTable.cpp \
./Code/UI/AdvanceUI/wLoadingBar.cpp \
./Code/UI/AdvanceUI/wPageView.cpp \
./Code/UI/AdvanceUI/wPanel.cpp \
./Code/UI/AdvanceUI/wPanelClipping.cpp \
./Code/UI/AdvanceUI/wParticle.cpp \
./Code/UI/AdvanceUI/wRadioBtn.cpp \
./Code/UI/AdvanceUI/wScrollView.cpp \
./Code/UI/AdvanceUI/wSlider.cpp \
./Code/UI/AdvanceUI/wText.cpp \
./Code/UI/AdvanceUI/wTextField.cpp \
./Code/UI/AdvanceUI/wTimeDigitalWidget.cpp \
./Code/UI/AdvanceUI/wTimeProcessWidget.cpp \
./../json/source/json_writer.cpp \
./../json/source/json_value.cpp \
./../json/source/json_reader.cpp 

LOCAL_CFLAGS   :=  -DOS_ANDROID_ -DGAME_ANDROID
LOCAL_EXPORT_CFLAGS   :=  -DOS_ANDROID_ -DGAME_ANDROID
LOCAL_CPPFLAGS += -fexceptions
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Code

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Code
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../json/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../RKUtils/Code
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2dx-lib
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2dx-lib/cocos
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2dx-lib/cocos/editor-support 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2dx-lib/external
include $(BUILD_STATIC_LIBRARY)