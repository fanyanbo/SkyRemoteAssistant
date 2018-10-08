#ifndef __MULTICASTSOCKET_H_
#define __MULTICASTSOCKET_H_
#include "CoocaaDef.h"
#include "ISocket.h"
#include "UDPSocket.h"

NS_COOCAA_BEGIN
class MulticastSocket:public IMulticastSocket,public UDPSocket
{
public:
	MulticastSocket():UDPSocket(){}
	virtual ~MulticastSocket(){}
	int open(bool isStream){
		return UDPSocket::open(isStream);
	}
	int bind(const SocketAddress& address)
	{
		return UDPSocket::bind(address);
	}
	int close()
	{
		return UDPSocket::close();
	}
	void setTimeOut(unsigned int time)
	{
		UDPSocket::setTimeOut(time);
	}
	int setSendBufferSize(const unsigned int bufferSize)
	{
		return UDPSocket::setReceiveBufferSize(bufferSize);
	}
	int setReceiveBufferSize(const unsigned int bufferSize)
	{
		return UDPSocket::setReceiveBufferSize(bufferSize);
	}
	bool getSendBufferSize(unsigned int& bufferSize)
	{
		return UDPSocket::getSendBufferSize(bufferSize);
	}
	bool getReceiveBufferSize(unsigned int& bufferSize)
	{
		return UDPSocket::getReceiveBufferSize(bufferSize);
	}
	SocketAddress getSocketAddress()
	{
		return UDPSocket::getSocketAddress();
	}
	unsigned int send(const DatagramPacket& packet, const bool nodelay = true)
	{
		return UDPSocket::send(packet,nodelay);
	}
	unsigned int receive(DatagramPacket& packet, const unsigned int timeout = NetUtil::TIMEOUT_INFINITE)
	{
		return UDPSocket::receive(packet,timeout);
	}

	int setTTL(const unsigned int ttl);
	int joinGroup(const SocketAddress & groupAddr, const SocketAddress & localAddr);
	int joinGroup(const SocketAddress& address);
	int leaveGroup(const SocketAddress &address);
	unsigned int getIpAddressCount();
	std::string getIpAddressByIndex(unsigned int index);

private:
	unsigned int getLocalIPAddressList(int **pp_int);
};

NS_COOCAA_END

#endif//__MULTICASTSOCKET_H_