#ifndef __IIPCHANDLER_H_
#define __IIPCHANDLER_H_
#include "CoocaaDef.h"
#include <memory>

NS_COOCAA_BEGIN

class IPCMessage;
class IIPCResHandlerListener;

class IIPCHandler
{
public:
	virtual void send(const IPCMessage& msg) = 0;

	virtual IPCMessage exec(const IPCMessage& msg) = 0;

	virtual ~IIPCHandler(){}
};

class IIPCMessageReceiver
{
public:
	virtual void onReceive(const IPCMessage& msg) = 0;

	virtual IPCMessage onReceiveSync(const IPCMessage& msg) = 0;

	virtual ~IIPCMessageReceiver(){}
};

NS_COOCAA_END
#endif //__IIPCHANDLER_H_