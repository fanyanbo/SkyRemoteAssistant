#ifndef __OSAPI_ADAPTERSERVICE_SHELL_H_
#define __OSAPI_ADAPTERSERVICE_SHELL_H_

#include "CoocaaDef.h"
#include <string>
#include "OSApiListenerController.h"
#include "OSApi_Base.h"
#include "ISocket.h"

NS_COOCAA_BEGIN
class OSApi_AdapterServiceShell:public OSApi_Base
{
public:

	OSApi_AdapterServiceShell(OSApiListenerController* controller);
	virtual ~OSApi_AdapterServiceShell();

	//notify started
	static const std::string NOTIFY_STARTED;

    static const std::string GET_VOLUME;
    static const std::string GET_IMAGE_MODE;

	void notifyStarted();
    bool getVolume(int value);
    bool getImageMode(int mode);
	
private:
	OSApi_AdapterServiceShell(const OSApi_AdapterServiceShell&);
	OSApi_AdapterServiceShell& operator = (const OSApi_AdapterServiceShell&);
};

NS_COOCAA_END

#endif//__OSAPI_ADAPTERSERVICE_SHELL_H_
