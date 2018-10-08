#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <list>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>

#include "VncLogcatAgent.h"
#include "VncUtils.h"
#include "VncCtrl.h"
#include "VncDataPackage.h"
#include "CommandId.h"


int LogcatAgent::logcat_socket = -1;
int LogcatAgent::logcat_pid = -1;
int LogcatAgent::logcat_pipe = -1;
volatile int LogcatAgent::exitLogcatFlag = 0;
volatile int LogcatAgent::threadExist = 0;

int LogcatAgent::startConnect()
{
    // 如果之前的socket还没有关闭则先关闭
    if (logcat_socket > 0)
        shutdownSocket();

    connectToServer();
}

int LogcatAgent::connectToServer()
{
    int fd, ret = 0;
    int tryTimes;
    bool connectedFlag = false;
    struct sockaddr_in sin;

    VNCLOGD("%s()", __FUNCTION__);

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        VNCLOGE("%s() - call socket() error %d.", __FUNCTION__, fd);
        return -1;
    }

    tryTimes = 3;
    while (tryTimes > 0)
    {
        tryTimes--;
        if (tryTimes <= 0)
            break;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET; 
        sin.sin_port = htons(logcat_port);
        inet_pton(AF_INET, ipaddr.c_str(), &sin.sin_addr);

        ret = connect(fd, (struct sockaddr *) &sin, sizeof(sin));
        if (ret < 0)
        {
            VNCLOGE("%s() - call connect() error %d.", __FUNCTION__, ret);
            usleep(500 * 1000);
            continue;
        }
        connectedFlag = true;
        break;
    }

    if (connectedFlag == false)
    {
        VNCLOGE("%s() - cannot connect to logcat port.", __FUNCTION__);
        close(fd);
        logcat_socket = -1;
        return -1;
    }

    // 设置socket非阻塞
    ret = setnonblocking(fd);
    if (ret < 0)
    {
        VNCLOGE("%s() - fcntl error.", __FUNCTION__);
    }

    // 发送TV的ID,以及\r\n\r\n作为前缀
    unsigned char head[8];
    head[0] = (pushid_crc32 >> 24) & 0xff;
    head[1] = (pushid_crc32 >> 16) & 0xff;
    head[2] = (pushid_crc32 >> 8) & 0xff;
    head[3] = (pushid_crc32) & 0xff;
    head[4] = '\r';
    head[5] = '\n';
    head[6] = '\r';
    head[7] = '\n';
    int n = write(fd, head, 8);
    if (n != 8)
    {
        ret = -1;
    }

    logcat_socket = fd;
    return ret;
}

int LogcatAgent::stopConnect()
{
    VNCLOGD("%s()", __FUNCTION__);

    closeCurrCommandSession();

    // 如果之前的socket还没有关闭则先关闭
    if (logcat_socket > 0)
        shutdownSocket();

    return 0;
}

int LogcatAgent::closeCurrCommandSession()
{
    VNCLOGD("%s()", __FUNCTION__);

    // 退出当前的LOGCAT线程
    VNCLOGD("%s() clear thread", __FUNCTION__);
    if (isThreadExist())
    {
        VNCLOGD("exitLogcatThread");
        exitLogcatThread();
        while (isThreadExist())
            usleep(100 * 1000);
    }

    // 关闭当前的PIPE
    VNCLOGD("%s() clear pipe", __FUNCTION__);
    if (logcat_pipe > 0)
        shutdownPipe();

    // 停止当前的LOGCAT进程
    VNCLOGD("%s() clear process", __FUNCTION__);
    if (logcat_pid > 0)
        killLogcatProcess();

    // socket连接保留
}

int LogcatAgent::sendCommand(const char* cmd, int tag)
{
    int pid;
    int pipefd[2];
    int ret;
    const char * args[] = {"/system/bin/sh", "-c", cmd, nullptr};
 
    VNCLOGD("%s(\"%s\")", __FUNCTION__, cmd);

    if (cmd == nullptr || cmd[0] == 0)
    {
        VNCLOGE("%s() cmd = <null>", __FUNCTION__);
        return -1;
    }

    // 如果服务器 socket没有连, 则先连接服务器
    if (logcat_socket < 0)
    {
        ret = connectToServer();
        if (ret < 0)
        {
            VNCLOGE("%s() connectToServer error.", __FUNCTION__);
            return ret;
        }
    }

    // 如当前还有命令会话,则关闭命令会话
    closeCurrCommandSession();

    if (0 == strcmp("pause", cmd))
    {   
        // PC端用户按下了 Ctrl+C
        return 0;
    }

    ret = pipe(pipefd);
    if (ret < 0)
    {
        VNCLOGE("%s() - pipe error.", __FUNCTION__);
        return errno;
    }

    pid = fork();

    if (pid == 0)
    {
        setpgrp();
        if (pipefd[1] != STDOUT_FILENO)
        {
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }
        close(pipefd[0]);
        //fcntl(STDOUT_FILENO, F_SETFD, 0);
        execvp(args[0], (char* const*)args);
        exit(errno);
    }

    if (pid < 0)
    {
        VNCLOGE("%s() - fork error.", __FUNCTION__);
        return errno;
    }

    VNCLOGD("%s() - new pid = %d", __FUNCTION__, pid);

    logcat_pid = pid;
    close(pipefd[1]);
    logcat_pipe = pipefd[0];
    
    pthread_t id;
    exitLogcatFlag = 0;
    addThread(&id, nullptr, logcatThread, nullptr, exitLogcatThread);

    return 0;
}

void * LogcatAgent::logcatThread(void* args)
{
    int err;
    int flag, ret;
    std::list<LogcatBuffer*> buffers;   // 缓冲区
    std::list<LogcatBuffer*>::iterator it;
    int select_ret;
    int n;
    bool haveopt;                       // 是否产生读写操作
    char rdbuff[1024];

    VNCLOGD("%s()", __FUNCTION__);

    // 写入文件调试的方法
    //FILE * fp = nullptr;
    //if (fp == nullptr)
    //    fp = fopen("/data/zdebug.txt", "w");

    if (logcat_pipe < 0)
    {
        VNCLOGE("%s() logcat_pipe < 0", __FUNCTION__);
        goto end;
    }
    if (logcat_pid < 0)
    {
        VNCLOGE("%s() logcat_pid < 0", __FUNCTION__);
        goto end;
    }
    // 设置管道非阻塞
    ret = setnonblocking(logcat_pipe);
    if (ret < 0)
    {
        VNCLOGE("%s() - fcntl error.", __FUNCTION__);
        goto end;
    }

    // 该循环里面绝对不能打LOG，因为程序本身打LOG会触发LOGCAT的输出,LOGCAT有输出又会打LOG
    // 这样会导致魔镜中显示魔镜的无限循环模式
    setThreadExist(1);
    while (1)
    {
        if (exitLogcatFlag)                 // 该标志成立,表示外部需要停止该线程
        {
            VNCLOGE("%s() - get exitLogcatFlag == true.", __FUNCTION__);
            break;
        }
        if (logcat_pipe < 0)
            break;
        if (logcat_pid < 0)
            break;

        haveopt = false;                    // 是否有读写操作

        // 读取管道
        n = read(logcat_pipe, rdbuff, sizeof(rdbuff));
        if (n > 0)
        {
            auto logbuff = new LogcatBuffer(rdbuff, n);
            buffers.push_back(logbuff);
            haveopt = true;
            //fprintf(fp, "read %d bytes\n", n);
            //fflush(stdout);
        }
        else
        {
            err = errno;
            if ((n == 0) || (err != EINTR && err != EAGAIN && err != EWOULDBLOCK))
            {
                VNCLOGE("%s() - read pipe error return %d, errno = %d.", __FUNCTION__, n, err);
                goto pipe_fail;
            }
            //VNCLOGD("n = %d, errno = %d", n, err);
        }

        // 写socket
        if (buffers.size() && logcat_socket > 0)
        {
            it = buffers.begin();
            auto firstbuff = *it;
            const char * pbuf = firstbuff->c_str();
            n = send(logcat_socket, &pbuf[firstbuff->sendsize], firstbuff->length() - firstbuff->sendsize, MSG_NOSIGNAL);
            if (n > 0)
            {
                firstbuff->sendsize += n;
                if (firstbuff->sendsize >= firstbuff->length())
                {
                    delete *it;
                    buffers.erase(it);
                }
                //fprintf(fp, "write %d bytes\n", n);
                //fflush(stdout);
            }
            else
            {
                err = errno;
                if ((n == 0) || (err != EINTR && err != EAGAIN && err != EWOULDBLOCK))
                {
                    //fprintf(fp, "logcat socket disconnect. \n");
                    //fflush(stdout);
                    shutdownSocket();
                    VNCLOGE("%s() - send logcat data error return %d, errno = %d.", __FUNCTION__, n, err);
                    goto socket_fail;
                }
            }

            haveopt = true;
        }

        // 如果循环中没有产生读写操作,需要睡眠一下避免CPU占用过高
        if (haveopt == false)
        {
            //fprintf(fp, "buffers.size() = %d\n", buffers.size());
            //fflush(stdout);
            usleep(5 * 1000);
        }

        // 如果积累了太多数据没有发出去, 缓冲区直接清除
        if (buffers.size() > 30 * 1000) 
        {
            //fprintf(fp, "buffers.size() = %d\n", buffers.size());
            //fprintf(fp, "too many log, clear it. \n");
            //fflush(stdout);
            for (it = buffers.begin(); it != buffers.end(); it++)
                delete *it;
            buffers.clear();
        }
    }

///////// 正常退出线程
end:
    VNCLOGD("%s() - end", __FUNCTION__);
    // 清理缓冲区
    if (buffers.size())
    {
        for (it = buffers.begin(); it != buffers.end(); it++)
            delete *it;
        buffers.clear();
    }
    setThreadExist(0);
    exitThread(pthread_self(), nullptr);
    return nullptr;

///////// 管道出错退出线程
pipe_fail:
    VNCLOGE("%s() - pipe_fail", __FUNCTION__);
    // 清理缓冲区
    if (buffers.size())
    {
        for (it = buffers.begin(); it != buffers.end(); it++)
            delete *it;
        buffers.clear();
    }
    // 停止当前的LOGCAT进程运行
    if (logcat_pid > 0)
        killLogcatProcess();
    // 关闭当前的PIPE
    if (logcat_pipe > 0)
        shutdownPipe();
    setThreadExist(0);
    exitThread(pthread_self(), nullptr);
    return nullptr;

///////// socket出错退出线程
socket_fail:
    VNCLOGE("%s() - socket_fail", __FUNCTION__);
    // 清理缓冲区
    if (buffers.size())
    {
        for (it = buffers.begin(); it != buffers.end(); it++)
            delete *it;
        buffers.clear();
    }
    // 停止当前的LOGCAT进程运行
    if (logcat_pid > 0)
        killLogcatProcess();
    // 关闭当前的PIPE
    if (logcat_pipe > 0)
        shutdownPipe();
    // 关闭socket描述符
    if (logcat_socket > 0)
        shutdownSocket();
    setThreadExist(0);
    exitThread(pthread_self(), nullptr);
    return nullptr;
}

int LogcatAgent::isThreadExist()
{
    return threadExist;
}

void LogcatAgent::setThreadExist(int flag)
{
    threadExist = flag;
}

void LogcatAgent::killLogcatProcess()
{
    VNCLOGD("%s()", __FUNCTION__);
    if (logcat_pid > 0)
    {
        int pgid;
        pgid = getpgid(logcat_pid);
        VNCLOGD("kill process group = %d", pgid);
        kill(-pgid, SIGTERM);
        logcat_pid = -1;
    }
}

void LogcatAgent::shutdownSocket()
{
    if (logcat_socket > 0)
    {
        shutdown(logcat_socket, SHUT_RDWR);
        close(logcat_socket);
        logcat_socket = -1;
    }
}

void LogcatAgent::shutdownPipe()
{
    if (logcat_pipe > 0)
    {
        close(logcat_pipe);
        logcat_pipe = -1;
    }
}

void LogcatAgent::exitLogcatThread()
{
    exitLogcatFlag = 1;
}

LogcatBuffer::LogcatBuffer(char * buff, size_t length) : std::string(buff, length)
{
    sendsize = 0;
}


