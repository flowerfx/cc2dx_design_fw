LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := jsoncpp_static

LOCAL_MODULE_FILENAME := libjsoncpp

LOCAL_SRC_FILES := \
./source/json_writer.cpp \
./source/json_value.cpp \
./source/json_reader.cpp 

LOCAL_CFLAGS   :=  -DOS_ANDROID_ -DGAME_ANDROID
LOCAL_EXPORT_CFLAGS   :=  -DOS_ANDROID_ -DGAME_ANDROID
LOCAL_CPPFLAGS += -fexceptions

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)