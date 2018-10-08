#ifndef __UDPSOCKET_H_
#define __UDPSOCKET_H_
#include "CoocaaDef.h"
#include "ISocket.h"

#ifdef _WIN32
#include <Windows.h>
#elif ANDROID
#include <netinet/in.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h> 
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

NS_COOCAA_BEGIN

class UDPSocket:public IUDPSocket
{
public:
	UDPSocket();
	virtual ~UDPSocket();

	int open(bool isStream);

	int bind(const SocketAddress& address);

	int close();

	void setTimeOut(unsigned int time);

	int setSendBufferSize(const unsigned int bufferSize);

	int setReceiveBufferSize(const unsigned int bufferSize);

	bool getSendBufferSize(unsigned int& bufferSize);

	bool getReceiveBufferSize(unsigned int& bufferSize);

	unsigned int send(const DatagramPacket& packet, const bool nodelay = true);

	unsigned int receive(DatagramPacket& packet, const unsigned int timeout = NetUtil::TIMEOUT_INFINITE);

	SocketAddress getSocketAddress();

protected:
	 SOCKET _socket;
};


NS_COOCAA_END

#endif//__UDPSOCKET_H_