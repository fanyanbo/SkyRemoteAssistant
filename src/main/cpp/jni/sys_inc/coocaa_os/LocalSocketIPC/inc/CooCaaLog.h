#ifndef __COOCAA_LOG_H
#define __COOCAA_LOG_H

#ifdef ANDROID
#include <android/log.h>
#define LOG_TAG "COOCAA"
#define COOCAALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define COOCAALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#elif _WIN32

#include <Windows.h>
#include <stdio.h>

#define COOCAALOGD(...)  logoutput(__VA_ARGS__)
#define COOCAALOGE(...)  logoutput(__VA_ARGS__)
inline void logoutput(const char * lpszFormat, ...)
{
	//#ifdef _DEBUG

	va_list argList;
	va_start(argList, lpszFormat);

	char chInput[20000] = { 0 };
	vsprintf_s(chInput, lpszFormat, argList);

	va_end(argList);

	OutputDebugStringA(chInput);
	OutputDebugStringA("\n");

	//#endif
}
#endif


#endif//__COOCAA_LOG_H