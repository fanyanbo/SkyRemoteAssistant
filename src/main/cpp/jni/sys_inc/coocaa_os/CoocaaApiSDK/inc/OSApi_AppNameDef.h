#ifndef  __OSAPI_APPNAME_DEF_H_
#define  __OSAPI_APPNAME_DEF_H_

#include "CoocaaDef.h"
#include <string>

NS_COOCAA_BEGIN

/*
* 请确保系统所有的appname都唯一,并且不要有 - 分隔符出现。
*/
class OSApi_AppNameDef
{
public:
	/*
	* 酷开系统5.0首页应用进程名称,bin程序
	*/
	static const std::string PATH_TO_MAINPAGE_APPNAME_5_0;

	/*
	*酷开系统5.0首页壳程序应用进程名称，apk程序
	*/
	static const std::string PATH_TO_MAINPAGESHELL_APPNAME_5_0;

	/*
	*酷开系统5.0虚拟外设节点进程，bin程序
	*/
	static const std::string PATH_TO_VITTUAL_INPUT_EVENT_NODE;

	/*
	*酷开系统5.0三屏互动进程，bin程序
	*/
	static const std::string PATH_TO_DESERVICE_BIN;

	/*
	*酷开系统5.0 DEService，apk程序
	*/
	static const std::string PATH_TO_DESERVICE_APPNAME_5_0;

	/*
	*酷开系统5.0长连接推送进程，bin程序
	*/
	static const std::string PATH_TO_PUSHSERVICE_BIN;

	/*
	*酷开系统5.0 PushService，apk程序
	*/
	static const std::string PATH_TO_PUSHSERVICE_APPNAME_5_0;

    /*
    * 酷开系统5.0 AirplayService，bin程序
    */
    static const std::string PATH_TO_AIRPLAY_BIN;
	
	/*
    * 酷开系统5.0 AdapterService，bin程序
    */
    static const std::string PATH_TO_ADAPTERSERVICE_BIN;
	
	/*
	*酷开系统5.0 AdapterService，apk程序
	*/
	static const std::string PATH_TO_ADAPTERSERVICE_APPNAME_5_0;
};

NS_COOCAA_END

#endif //__OSAPI_APPNAME_DEF_H