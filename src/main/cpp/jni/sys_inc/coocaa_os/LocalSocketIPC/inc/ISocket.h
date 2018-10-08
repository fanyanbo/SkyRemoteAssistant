#ifndef __ISOCKET_H
#define __ISOCKET_H
#include "CoocaaDef.h"
#include <string>
#include "DataPacket.h"

NS_COOCAA_BEGIN

#define _MAX_SOCKET_LENGTH 20480
#define _MAX_SOCKET_STREAM_LISTEN 10

class NetUtil
{
public:
	static std::string getHostByName(const std::string& host);

	static const unsigned int ANY_PORT = 0x0u;
	static const std::string ANY_ADDR;
	static const unsigned int TIMEOUT_INFINITE = 0xFFFFFFFFu;
	enum SocketType
	{
		SCK_TCP,
		SCK_UDP
	};
};

class SocketAddress
{
public:
	SocketAddress(const std::string& address,unsigned int port =0x0u)
	{
		_adddress = NetUtil::getHostByName(address);
		_port = port;
	}

    SocketAddress(const unsigned int port=0x0u) : _adddress("0.0.0.0"), _hostname(""), _port(port)
    {
    }

	virtual ~SocketAddress()
	{

	}

	void set(const std::string& address, const int port=0x0u)
	{
		_adddress = NetUtil::getHostByName(address);
		_port = port;
	}

	void setHostName(const std::string& hostname)
	{
		_hostname = hostname;
	}
	inline std::string getHostAddress() const
	{
		return _adddress;
	}
	inline std::string getHostName() const
	{
		return _adddress;
	}
	inline int getPort() const
	{
		return _port;
	}

private:
	std::string _adddress;
	std::string _hostname;
	unsigned int _port;
};

class LocalSocketAddress
{
public:
	LocalSocketAddress(const std::string& address,int port =0x0u)
	{
		_adddress = address;
		_port = port;
	}

	LocalSocketAddress() : _adddress(""), _hostname(""), _port(0)
	{
	}

	virtual ~LocalSocketAddress()
	{

	}

	void set(const std::string& address, const int port=0x0u)
	{
		_adddress = address;
	}

	void setHostName(const std::string& hostname)
	{
		_hostname = hostname;
	}
	inline std::string getHostAddress() const
	{
		return _adddress;
	}
	inline std::string getHostName() const
	{
		return _adddress;
	}
	inline int getPort() const
	{
		return _port;
	}

private:
	std::string _adddress;
	std::string _hostname;
	int _port;
};

class DatagramPacket:public DataPacket
{
public:
	DatagramPacket()
		: DataPacket(),
		_address()
	{

	}

	DatagramPacket(const std::string& data)
		: DataPacket(data),
		_address()
	{
		_dataLen = DataPacket::getSize();
	}

	DatagramPacket(const unsigned int dataSize)
		: DataPacket(dataSize),
		_address(),
		_dataLen(0u)
	{

	}

	DatagramPacket(const unsigned char* srcbuffer,const unsigned int length)
		:DataPacket(srcbuffer,length),
		_address(),
		_dataLen(length)
	{

	}

	virtual ~DatagramPacket()
	{
	}

	inline void setAddress(const SocketAddress& target)
	{
		_address = target;
	}

	inline void setAddress(const std::string& address, const unsigned int  port)
	{
		_address.set(address, port);
	}

	inline SocketAddress getAddress() const
	{
		return _address;
	}

	inline void setLength(const unsigned& length)
	{
		_dataLen = length;
	}

	inline unsigned int getLength() const
	{
		return _dataLen;
	}

private:
	SocketAddress _address;
	unsigned int _dataLen;
};

class ISocket
{
public:

	virtual ~ISocket(){}

    //if is stream ,need listen and accept,like tcp;
    //if is dgram, can receive directly,like udp;

	virtual int open(bool isStream) = 0;

	virtual int bind(const SocketAddress& address) = 0;

	virtual int close() = 0;

	virtual void setTimeOut(unsigned int time) = 0;

	virtual int setSendBufferSize(const unsigned int bufferSize) = 0;

    virtual int setReceiveBufferSize(const unsigned int bufferSize) = 0;

	virtual bool getSendBufferSize(unsigned int& bufferSize) = 0;

	virtual bool getReceiveBufferSize(unsigned int& bufferSize) = 0;

	virtual SocketAddress getSocketAddress() = 0;

};

class IILocalSocket
{
public:

	virtual ~IILocalSocket(){}

	//if is stream ,need listen and accept,like tcp;
	//if is dgram, can receive directly,like udp;

	virtual int open(bool isStream) = 0;

	virtual int bind(const LocalSocketAddress& address) = 0;

	virtual int close() = 0;

	virtual void setTimeOut(unsigned int time) = 0;

	virtual int setSendBufferSize(const unsigned int bufferSize) = 0;

	virtual int setReceiveBufferSize(const unsigned int bufferSize) = 0;

	virtual bool getSendBufferSize(unsigned int& bufferSize) = 0;

	virtual bool getReceiveBufferSize(unsigned int& bufferSize) = 0;

	virtual LocalSocketAddress getSocketAddress() = 0;

};


class ILocalSocket :public IILocalSocket
{
public:

    virtual ~ILocalSocket(){}

	virtual unsigned int send(const DataPacket& datapacket) = 0;

	virtual unsigned int receive(DataPacket& data, const unsigned timeout = 0xFFFFFFFFu) = 0;

	virtual LocalSocketAddress getRemoteAddress() = 0;

	virtual int connect(const LocalSocketAddress& address) = 0;

	virtual int disconnect() = 0;
};

class ILocalServerSocket : public ILocalSocket
{
public:

    virtual ~ILocalServerSocket(){}

	virtual int listen() = 0;

    virtual ILocalSocket* accept() = 0;
};

class LocalSocket : public ILocalSocket
{
public:
    ~LocalSocket(){}
};

class LocalServerSocket : public ILocalServerSocket
{
public:
    ~LocalServerSocket(){}
};


class IUDPSocket:public ISocket
{
public:
	virtual ~IUDPSocket(){};
	virtual unsigned int send(const DatagramPacket& packet, const bool nodelay = true) = 0;
	virtual unsigned int receive(DatagramPacket& packet, const unsigned int timeout = NetUtil::TIMEOUT_INFINITE) = 0;  
};

class IMulticastSocket : public IUDPSocket
{
public:
	virtual int setTTL(const unsigned int ttl) = 0;
	virtual int joinGroup(const SocketAddress & groupAddr, const SocketAddress & localAddr) = 0;
	virtual int joinGroup(const SocketAddress& address) = 0;
	virtual int leaveGroup(const SocketAddress &address) = 0;
	virtual unsigned int getIpAddressCount() = 0;
	virtual std::string getIpAddressByIndex(unsigned int index) = 0;
		
};

class ITCPSocket:public ISocket
{
public:

	virtual ~ITCPSocket(){}

	virtual unsigned int send(const DatagramPacket& datapacket) = 0;

	virtual  int receive(DatagramPacket& data, const unsigned timeout = 0xFFFFFFFFu) = 0;

	virtual SocketAddress getRemoteAddress() = 0;

	virtual int connect(const SocketAddress& address) = 0;

	virtual int disconnect() = 0;

};

class ITCPServerSocket:public ITCPSocket
{
public:

	virtual ~ITCPServerSocket(){}

	virtual int listen() = 0;

	virtual ITCPSocket* accept() = 0;
};

NS_COOCAA_END



#endif //__ISOCKET_H
