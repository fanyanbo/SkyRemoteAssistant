#include <string>
#ifndef COOCAA_OS_VNC_LOGCAT_AGENT_H_
#define COOCAA_OS_VNC_LOGCAT_AGENT_H_

class LogcatAgent
{
public:
    static int startConnect();
    static int stopConnect();
    static int sendCommand(const char* cmd, int tag);

private:
    static void * logcatThread(void* args);
    static void exitLogcatThread();
    static int  connectToServer();
    static int  closeCurrCommandSession();
    static void killLogcatProcess();
    static void shutdownSocket();
    static void shutdownPipe();

    static int  isThreadExist();
    static void setThreadExist(int flag);

private:
    static int  logcat_socket;              // logcat 与服务器连接的TCP连接socket-id
    static int  logcat_pid;                 // logcat进程号
    static int  logcat_pipe;                // 读取logcat输出的管道
    static volatile int exitLogcatFlag;     // 退出LOGCAT数据处理线程标志 (此标志置1, 会让 LOGCAT 线程退出)
    static volatile int threadExist;        // 该标志表示线程是否存在
};

class LogcatBuffer : public std::string
{
public:
    LogcatBuffer(char * buff, size_t length);
    size_t sendsize;
};



#endif // !COOCAA_OS_VNC_LOGCAT_AGENT_H_




