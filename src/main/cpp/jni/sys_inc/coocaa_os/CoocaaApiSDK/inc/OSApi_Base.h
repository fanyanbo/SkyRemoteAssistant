#ifndef __OSAPI_BASE_H
#define __OSAPI_BASE_H

#include "CoocaaDef.h"
#include <memory>
#include <thread>
#include <string>
#include <map>
#include "IPCHandlerUsage.h"

NS_COOCAA_BEGIN

class OSApiListenerController;
class IPCMessage;


typedef std::function<void (const IPCMessage&)> OSAPICALLBACK;

class OSApi_Base
{
	friend class CoocaaHandlerResListner;
public:
	OSApi_Base(OSApiListenerController* controller);
	virtual ~OSApi_Base();

	std::string getApplicationName();
protected:
	void send(const IPCMessage& msg,OSAPICALLBACK callback);

	IPCMessage exec(const IPCMessage& msg);
protected:
	OSApiListenerController* _controller;
};

class CoocaaHandlerResListner:public IIPCResHandlerListener
{
public:
	CoocaaHandlerResListner(OSAPICALLBACK callback)
	{
		_callback = callback;
	}

	virtual ~CoocaaHandlerResListner()
	{

	}

	virtual void onResult(const IPCMessage& msg)
	{
		if(_callback)
		{
			_callback(msg);
		}
	}

private:
	OSAPICALLBACK _callback;
};

NS_COOCAA_END

#endif //__OSAPI_BASE_H