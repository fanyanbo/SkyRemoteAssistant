#ifndef __LOCAL_SOCKET_ANDROID_H
#define __LOCAL_SOCKET_ANDROID_H

#include "CoocaaDef.h"
#include "ISocket.h"


#ifdef ANDROID

#include <stddef.h>
#include <string.h>
#define INVALID_SOCKET      (-1)

NS_COOCAA_BEGIN

class LocalSocket_Android : public LocalSocket
{
public:
    LocalSocket_Android();

    LocalSocket_Android(const LocalSocketAddress & local, const LocalSocketAddress & remote, int socket);

    ~LocalSocket_Android();

    virtual int open(bool isStream);

    virtual int bind(const LocalSocketAddress& address);

    virtual int close();

    virtual void setTimeOut(unsigned int timeout);

    virtual int setSendBufferSize(const unsigned int bufferSize);

    virtual int setReceiveBufferSize(const unsigned int bufferSize);

    virtual bool getSendBufferSize(unsigned int& bufferSize);

    virtual bool getReceiveBufferSize(unsigned int& bufferSize);

    virtual unsigned int send(const DataPacket& datapacket);

    virtual unsigned int receive(DataPacket& data, const unsigned timeout = 0xFFFFFFFFu);

    virtual LocalSocketAddress getSocketAddress();

    virtual LocalSocketAddress getRemoteAddress();

    virtual int connect(const LocalSocketAddress& address);

    virtual int disconnect();

protected:
    int socket;
    int type;
    LocalSocketAddress socketAddress;
    LocalSocketAddress remoteAddress;
    size_t    timeout;
    size_t    recvBuffSize;
    char *    recvbuff;
};


class LocalServerSocket_Android : public LocalServerSocket, public LocalSocket_Android
{
public:

    LocalServerSocket_Android();

    ~LocalServerSocket_Android();

    virtual int open(bool isStream);

    virtual int bind(const LocalSocketAddress& address);

    virtual int close();

    virtual void setTimeOut(unsigned int time);

    virtual int setSendBufferSize(const unsigned int bufferSize);

    virtual int setReceiveBufferSize(const unsigned int bufferSize);

    virtual bool getSendBufferSize(unsigned int& bufferSize);

    virtual bool getReceiveBufferSize(unsigned int& bufferSize);

    virtual unsigned int send(const DataPacket& datapacket);

    virtual unsigned int receive(DataPacket& data, const unsigned timeout = 0xFFFFFFFFu);

    virtual LocalSocketAddress getSocketAddress();

    virtual LocalSocketAddress getRemoteAddress();

    virtual int connect(const LocalSocketAddress& address);

    virtual int disconnect();

    virtual int listen();

    virtual ILocalSocket* accept();

private:
    

};

NS_COOCAA_END

#endif // ANDROID

#endif //__LOCAL_SOCKET_ANDROID_H
