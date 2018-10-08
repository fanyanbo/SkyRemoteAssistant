#ifndef __OSAPI_LISTENER_CONTROLLER_H
#define __OSAPI_LISTENER_CONTROLLER_H
#include "CoocaaDef.h"
#include <string>
#include <map>

#include "IPCHandlerUsage.h"

NS_COOCAA_BEGIN

typedef std::function<IPCMessage (const IPCMessage&,bool& canProcess)> OSMSGPROCESSOR;
/*
* 一个进程里面，只需要有一个Controller。
* 请注意程序的完整逻辑。
*/

class CoocaaHandlerListener;
class OSApiListenerController
{
	friend class OSApi_Base;
	friend class CoocaaHandlerListener;
	friend class OSApiListnerControllerInner;

public:
	static OSApiListenerController* getInstance(std::string myName);

	static void destroyInstance();

	std::string getApplicationName();

	void registerProcessor(OSMSGPROCESSOR processor);

private:

	OSApiListenerController(std::string name);
	virtual ~OSApiListenerController();
	OSApiListenerController(const OSApiListenerController&);
	OSApiListenerController& operator = (const OSApiListenerController&);

	static OSApiListenerController* _sharedInstance;

	std::shared_ptr<CoocaaHandlerListener> _handlerListener;
	std::shared_ptr<IPCHandleUsage> _handleUsage;

	std::string _applicationname;

	void send(const IPCMessage& msg,std::shared_ptr<IIPCResHandlerListener> resListener);
	IPCMessage exec(const IPCMessage& msg);

	IPCMessage callProcess(const IPCMessage& msg);
	std::function<IPCMessage (const IPCMessage&,bool& canProcess)> _processor;
};

class OSApiListnerControllerInner
{
public:
	OSApiListnerControllerInner(std::string name);
	virtual ~OSApiListnerControllerInner();

	OSApiListenerController* getController();
private:
	OSApiListenerController* newController;
};

class CoocaaHandlerListener:public IIPCHandlerListener
{
public:
	CoocaaHandlerListener(OSApiListenerController* sp)
	{
		_controller = sp;
	}
	virtual ~CoocaaHandlerListener()
	{
	}

	virtual IPCMessage onHandler(const IPCMessage& msg)
	{
		return _controller->callProcess(msg);
	}

private:
	OSApiListenerController* _controller;
};


NS_COOCAA_END
#endif //__OSAPI_LISTENER_CONTROLLER_H