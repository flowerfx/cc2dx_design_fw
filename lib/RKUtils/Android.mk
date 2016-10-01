LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := rkutils_static

LOCAL_MODULE_FILENAME := librkutils 

LOCAL_SRC_FILES := \
./Code/Common/RKAssert.cpp \
./Code/Common/RKHeap.cpp \
./Code/Common/RKStdSAFE.cpp \
./Code/Common/RKUtil.cpp \
./Code/Logon/src/api.cpp \
./Code/Logon/src/checkpoint.cpp \
./Code/Logon/src/formatter.cpp \
./Code/Logon/src/lobject.cpp \
./Code/Logon/src/lstring.cpp \
./Code/Logon/src/message.cpp \
./Code/Logon/src/mutex.cpp \
./Code/Logon/src/node.cpp \
./Code/Logon/src/platform.cpp \
./Code/Logon/src/socket.cpp \
./Code/Logon/src/statics.cpp \
./Code/Logon/src/target.cpp \
./Code/Logon/src/timer.cpp \
./Code/Logon/src/topic.cpp \
./Code/Logon/src/unittest.cpp \
./Code/RKBaseLog/Assert.cpp \
./Code/RKBaseLog/Log.cpp \
./Code/RKLog_Code/RKLog.cpp \
./Code/RKFile_Code/Source/Ark.cpp \
./Code/RKFile_Code/Source/ArkFileSystem.cpp \
./Code/RKFile_Code/Source/ArkManager.cpp \
./Code/RKFile_Code/Source/MemoryFileSystem.cpp \
./Code/RKFile_Code/Source/NativeFileSystem.cpp \
./Code/RKFile_Code/Android/RKFile.cpp \
./Code/RKString_Code/RKString.cpp \
./Code/RKString_Code/RKStringFunctions.cpp \
./Code/RKString_Code/RKStringUTF.cpp \
./Code/RKThread_Code/RKThreadWrapper.cpp \
./Code/RKThread_Code/Android/RKTimer.cpp \
./Code/RKThread_Code/Android/RKThread.cpp \
./Code/RKTool_Code/Encryption.cpp \
./Code/RKVector_Code/RKMath.cpp \
./Code/RKVector_Code/RKMathTrig.cpp \
./Code/RKVector_Code/RKMathTrig_NEON.cpp \
./Code/RKVector_Code/RKMatrix.cpp \
./Code/RKVector_Code/RKQuaternion.cpp \
./Code/RKVector_Code/RKVector.cpp 

LOCAL_CFLAGS   :=  -DGAME_ANDROID -DOS_ANDROID_
LOCAL_EXPORT_CFLAGS   :=  -DGAME_ANDROID -DOS_ANDROID_

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Code

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Code
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Code/Common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Code/RKString_Code
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Code/RKThread_Code
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Code/RKFile_Code
                                 
include $(BUILD_STATIC_LIBRARY)