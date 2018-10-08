#include <string>
#ifndef COOCAA_OS_VNC_TELNET_AGENT_H_
#define COOCAA_OS_VNC_TELNET_AGENT_H_

class TelnetAgent
{
public:
    static int startTelnetService();
    static int stopTelnetService();
    static int sendData(const unsigned char* data, int size);
private:
    static void * recvData(void * args);
    static void exitRecvTelnetdDataThread();
    static int telnetfd;
    static volatile int exitRecv;
};


#endif // !COOCAA_OS_VNC_TELNET_AGENT_H_




