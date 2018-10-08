#ifndef __IPC_HANDLER_USAGE_H_
#define __IPC_HANDLER_USAGE_H_
#include "CoocaaDef.h"
#include "IIPCHandler.h"
#include <string>
#include "IPCMessageQueue.h"
#include <map>
#include <mutex>
#include <list>
#include <condition_variable>
#include <thread>

NS_COOCAA_BEGIN

class IIPCHandlerListener
{
public:
	virtual IPCMessage onHandler(const IPCMessage& msg) = 0;
	virtual ~IIPCHandlerListener(){}
};

class IIPCResHandlerListener
{
public:
	virtual void onResult(const IPCMessage& msg) = 0;
	virtual ~IIPCResHandlerListener(){}
};

class IPCTransporter;
class IPCHandleUsage:public IIPCMessageReceiver
{
public:
	IPCHandleUsage(std::string losocketpath);

	virtual ~IPCHandleUsage();

	void init();

	void cleanup();

	void send(const IPCMessage& msg,std::shared_ptr<IIPCResHandlerListener> resListener);

	IPCMessage exec(const IPCMessage& msg);

	virtual void onReceive(const IPCMessage& msg);

	virtual IPCMessage onReceiveSync(const IPCMessage& msg);

	void setListener(std::shared_ptr<IIPCHandlerListener> handlerListener);

	std::string getModulePath();

protected:
	void sendAck(const IPCMessage& msg);

	void startWaitMsgThread();

	void waitIPCMessage(bool needlock);

private:

	IPCHandleUsage(const IPCHandleUsage& src);

	IPCHandleUsage& operator = (const IPCHandleUsage& src);

	IPCMessageQueue _msgqueue;

	std::weak_ptr<IIPCHandlerListener> _spHandlerListener;

	std::string _losocetpath;

	bool _iscleanup;

	std::shared_ptr<IPCTransporter> _spIPcTransporter;

	std::map<std::string,std::shared_ptr<IIPCResHandlerListener>> _sendResHandlerqueue;

	std::mutex _mutex;

	std::mutex _mapmutex;

	std::condition_variable _sync;

    std::thread _t;
};

NS_COOCAA_END
#endif//__IPC_HANDLER_USAGE_H_