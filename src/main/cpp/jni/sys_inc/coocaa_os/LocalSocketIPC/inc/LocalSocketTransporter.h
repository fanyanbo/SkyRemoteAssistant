#ifndef __LOCALSOCKET_TRANSPORTER_H_
#define __LOCALSOCKET_TRANSPORTER_H_
#include "CoocaaDef.h"
#include "IIPCHandler.h"
#include "IPCTransporter.h"
#include "ISocket.h"
#include <thread>

NS_COOCAA_BEGIN
class LocalSocketTransporter:public IPCTransporter
{
public:
	LocalSocketTransporter(std::string socketpath);
	virtual ~LocalSocketTransporter();

	virtual void send(const IPCMessage& msg);
	virtual IPCMessage exec(const IPCMessage& msg);

protected:
	void startSyncReceiveThread();
	void processServerSocket(void* pServerSocket);
	
	void startAsyncReceiveThread(); 
private:
	std::shared_ptr<ILocalServerSocket> _spSyncReceiveSocket;
	std::shared_ptr<ILocalServerSocket> _spAsyncReceiveSocket;
	std::shared_ptr<ILocalSocket> _spAsyncClientSocket;

	bool _isServerSocktThreadEnd;
	bool _isAsyncReceiveThreadEnd;

    std::thread t1;
    std::thread t2;
};
NS_COOCAA_END

#endif//__LOCALSOCKET_TRANSPORTER_H_