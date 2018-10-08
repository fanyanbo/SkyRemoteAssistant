#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <errno.h>

#include "VncTelnetAgent.h"
#include "VncUtils.h"
#include "VncCtrl.h"
#include "VncDataPackage.h"
#include "CommandId.h"

#define TELNETD_PORT  4149

int TelnetAgent::telnetfd = -1;
volatile int TelnetAgent::exitRecv = 0;

int TelnetAgent::startTelnetService()
{
    int ret, fd;
    int tryTimes = 8;
    struct sockaddr_in sin;
    bool connectedFlag;
    ret = sendCtrlCmd("start_telnetd");
    if (ret != 0)
    {
        VNCLOGE("start_telnetd error %d", ret);
        return -1;
    }

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        VNCLOGE("%s() - call socket() error %d.", __FUNCTION__, fd);
        return -1;
    }

    connectedFlag = false;
    while (tryTimes > 0)
    {
        tryTimes--;
        if (tryTimes <= 0)
        {
            return -1;
        }
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET; 
        sin.sin_port = htons(TELNETD_PORT);
        inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);

        ret = connect(fd, (struct sockaddr *) &sin, sizeof(sin));
        if (ret < 0)
        {
			VNCLOGE("%s() - call connect() ret %d. errorno %d", __FUNCTION__, ret, errno);
            usleep(500 * 1000);
            continue;
        }
        connectedFlag = true;
        break;
    }

    telnetfd = fd;
    if (connectedFlag == false)
    {
        VNCLOGE("%s() - cannot connect to telnetd.", __FUNCTION__);
        close(fd);
        return -1;
    }
    
    pthread_t handle;
    exitRecv = 0;
    addThread(&handle, nullptr, recvData, nullptr, exitRecvTelnetdDataThread);
    return ret;
}

int TelnetAgent::stopTelnetService()
{
    int ret;
    ret = sendCtrlCmd("stop_telnetd");
    if (ret != 0)
    {
        VNCLOGE("stop_telnetd error %d", ret);
    }
    exitRecvTelnetdDataThread();
    return ret;
}

int TelnetAgent::sendData(const unsigned char* data, int size)
{
    int n;
    if (telnetfd < 0)
    {
        VNCLOGE("%s() telnetfd < 0", __FUNCTION__);
		int ret = startTelnetService();
		if (ret != 0)
           return -1;
    }
    
    while (1)
    {
        n = write(telnetfd, data, size);
        size -= n;
        if (size <= 0)
            break;
    }

    return 0;
}

void * TelnetAgent::recvData(void * args)
{
    int n;
    fd_set rdfds;
    struct timeval tv;
    char buffer[2048];

    exitRecv = 0;
    while (1)
    {
        FD_ZERO(&rdfds);
        FD_SET(telnetfd, &rdfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100 * 1000;
        n = select(telnetfd + 1, &rdfds, nullptr, nullptr, &tv);
        if (n > 0)
        {
            n = read(telnetfd, buffer, sizeof(buffer));
            if (n > 0)
            {
                sendPackage(VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_CMD_TELNET_DATA, 0, std::string(buffer, n)));
            }
            else if (n < 0)
            {
                VNCLOGE("%s() call read() error %d", __FUNCTION__, n);
                break;
            }
            else
            {
                VNCLOGE("%s() call read() 0 bytes", __FUNCTION__);
            }
        }
        else if (n < 0)
        {
            VNCLOGE("%s() call select() error %d", __FUNCTION__, n);
            break;
        }
            
        if (exitRecv)
        {
            VNCLOGD("%s() exit thread", __FUNCTION__);
            break;
        }
    }

    if (telnetfd > 0)
    {
        shutdown(telnetfd, SHUT_RDWR);
        close(telnetfd);
        telnetfd = -1;
        exitRecv = 0;
    }
    
    exitThread(pthread_self(), nullptr);

    return nullptr;
}

void TelnetAgent::exitRecvTelnetdDataThread()
{
    exitRecv = 1;
}


