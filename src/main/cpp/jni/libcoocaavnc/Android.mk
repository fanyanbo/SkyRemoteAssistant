LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libcoocaavnc
LOCAL_SRC_FILES :=  src/com_coocaa_remotectrlservice_JniUtils.cpp           \
					src/JniHelper.cpp                                    \
					src/jsoncpp.cpp                                      \
					src/KeyDispatchManager.cpp                           \
					src/VirtualKey.cpp                                   \
					src/VncBufferReader.cpp                              \
					src/VncCommandProcessor.cpp                          \
					src/VncCommandQueue.cpp                              \
					src/VncCtrl.cpp                                      \
					src/VncDataPackage.cpp                               \
					src/VncLogcatAgent.cpp                               \
					src/VncTelnetAgent.cpp                               \
					src/VncThreads.cpp                                   \
					src/VncUtils.cpp                                     \
					

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc								\
					$(LOCAL_PATH)/../sys_inc/system/core/include	\
					$(LOCAL_PATH)/../sys_inc/coocaa_os/LocalSocketIPC/inc	\
					$(LOCAL_PATH)/../sys_inc/coocaa_os/CoocaaApiSDK/inc		\
					$(LOCAL_PATH)/../sys_inc/external						\
					

LOCAL_CFLAGS += -std=c++11
LOCAL_CPPFLAGS += -fexceptions
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/../sys_so
LOCAL_LDFLAGS += -lcutils -lz -lcurl -lssl -lcrypto -lCoocaaApiSDK -lLocalSocketIPC

include $(BUILD_SHARED_LIBRARY)
