#include <android/log.h>
#include <string>
#ifndef COOCAA_OS_VNC_UTILS_H_
#define COOCAA_OS_VNC_UTILS_H_

#define VNC_LOG_TAG "libcoocaavnc"
#define VNCLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, VNC_LOG_TAG, __VA_ARGS__)
#define VNCLOGE(...)  __android_log_print(ANDROID_LOG_ERROR, VNC_LOG_TAG, __VA_ARGS__)

#define PROP_NAME_PREFIX    "coocaa.optsrv"

int filelogd(const char * format, ...);
int fileloge(const char * format, ...);
int fileloginit();
int filelogfinish();

unsigned int crc32_hash(const unsigned char* pbData, size_t size);
int sendCtrlCmd(const char* cmdstring);
int execAsRoot(const char* cmdstring);
int curlDownloadFile(const char* url, const char* saveAtFile, void(*callback)(const char*));
int setnonblocking(int fd);
char * getmacaddress();

#endif 

