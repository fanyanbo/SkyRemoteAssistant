#ifndef __IPCMESSAGE_QUEUE_
#define __IPCMESSAGE_QUEUE_
#include "CoocaaDef.h"
#include <queue>
#include "IPCMessage.h"

NS_COOCAA_BEGIN

class IPCMessageQueue
{
public:
	IPCMessageQueue();
	virtual ~IPCMessageQueue();

	void putMessage(const IPCMessage& msg);
	bool getMessage(IPCMessage& msg);

	unsigned int getSize();
private:
	IPCMessageQueue(const IPCMessageQueue& src);
	IPCMessageQueue& operator = (const IPCMessageQueue& src);

	std::queue<IPCMessage> cmdQueue;
};
NS_COOCAA_END
#endif//__IPCMESSAGE_QUEUE_