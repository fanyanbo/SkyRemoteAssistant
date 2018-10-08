#ifndef __TCPSOCKET_H_
#define __TCPSOCKET_H_
#include "CoocaaDef.h"
#include <string>
#include "ISocket.h"
#ifdef _WIN32
#include<Windows.h>
#elif ANDROID
#include <netinet/in.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h> 
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif
NS_COOCAA_BEGIN

class TCPSocket:public ITCPSocket
{
public:
	TCPSocket();
	TCPSocket(SOCKET sck,const SocketAddress &address);

	virtual ~TCPSocket();

	virtual int open(bool isStream);

	virtual int bind(const SocketAddress& address);

	virtual int close();

	virtual void setTimeOut(unsigned int time);

	virtual int setSendBufferSize(const unsigned int bufferSize);

	virtual int setReceiveBufferSize(const unsigned int bufferSize);

	virtual bool getSendBufferSize(unsigned int& bufferSize);

	virtual bool getReceiveBufferSize(unsigned int& bufferSize);

	virtual SocketAddress getSocketAddress();

	virtual  unsigned int send(const DatagramPacket& datapacket);

	virtual  int receive(DatagramPacket& data, const unsigned timeout = 0xFFFFFFFFu);

	virtual SocketAddress getRemoteAddress();

	virtual int connect(const SocketAddress& address);

	virtual int disconnect();

protected:
	sockaddr_in convertTCPAddress(const SocketAddress& address);
	SOCKET _socket;
	SocketAddress remoteAddress;
};


class TCPServerSocket :public TCPSocket
{

public:
	TCPServerSocket();

	virtual ~TCPServerSocket();
	virtual int bind(const SocketAddress& address);

	virtual int listen();

	virtual ITCPSocket* accept();

	virtual int setReuseAddr();

private:

	sockaddr_in converServerTCPAddress(const SocketAddress& address);
};

NS_COOCAA_END

#endif//__TCPSOCKET_H_