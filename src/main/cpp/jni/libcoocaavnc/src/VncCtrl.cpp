#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <android/log.h>
#include "curl/curl.h"

#include "CommandId.h"
#include "VncCtrl.h"
#include "VncUtils.h"
#include "VncCommandQueue.h"
#include "VncBufferReader.h"
#include "VncDataPackage.h"
#include "VncCommandProcessor.h"
#include "VncThreads.h"
#include "VncTelnetAgent.h"
#include "VirtualKey.h"
#include "KeyDispatchManager.h"
#include "com_coocaa_remotectrlservice_JniUtils.h"

VncThreads * threads = nullptr;

pthread_t readprot_thread_handle = -1;
pthread_t writeprot_thread_handle = -1;
pthread_t cmdprocessor_thread_handle = -1;

volatile int readprot_thread_exitflag = 0;
volatile int writeprot_thread_exitflag = 0;
volatile int cmdprocessor_thread_exitflag = 0;

int message_socket_id = -1;
VncCommandQueue * cmdin = nullptr;
VncCommandQueue * cmdout = nullptr;
char pushid_string[256] = { 0 };
char serverid_string[256] = { 0 };
char pcid_string[256] = { 0 };
unsigned pushid_crc32 = 0;
unsigned serverid_crc32 = 0;
unsigned pcid_crc32 = 0;
std::string ipaddr = "";

static void * vnc_write_channel(void * args);
static void * vnc_read_channel(void * args);
static void * vnc_command_processor(void * args);
//static void * cleanup_thread(void * args);
static void * try_to_connect(void * args);
static void   readprot_thread_exit();
static void   writeprot_thread_exit();
static void   cmdprocessor_thread_exit();
static void   handle_sig_child(int arg);
static void   modify_oom_value();

struct TryConnectData
{
    std::string pushId;
    std::string serverIp;
    bool boAccept;
};

int startVnc(const char* pushId, std::string serverIp, bool boAccept)
{
    static struct TryConnectData threaddata;

    VNCLOGD("startVnc v1.0");
    
    // 修改OOM值, 避免被杀
    modify_oom_value();

    // 初始化随机数种子
    srand(time(nullptr));

    // 初始化curl库
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // 捕捉子进程退出信号
    signal(SIGCHLD, handle_sig_child);

    VNCLOGD("new connect push id  = %s", pushId);
    VNCLOGD("new connect push crc = 0x%08x", crc32_hash((const unsigned char*)pushId, strlen(pushId)));
    VNCLOGD("new connect server ip = %s", serverIp.c_str());

    threaddata.pushId = pushId;
    threaddata.serverIp = serverIp;
    threaddata.boAccept = boAccept;

    pthread_t tid;
    int ret = pthread_create(&tid, nullptr, try_to_connect, &threaddata);
	if (ret == -1)
	{
		VNCLOGD("create thread try_to_connect failed erron= %d/n", errno);
		return -1;
	}

	VNCLOGD("create thread try_to_connect success");

    return 0;
}

static void * try_to_connect(void * args)
{
    int trytimes;
    struct sockaddr_in  servaddr;
    int newsocketid;
    std::string pushId;
    std::string serverIp;
    bool boAccept;
    const TryConnectData * pdata = (const TryConnectData*) args;

    pushId = pdata->pushId;
    serverIp = pdata->serverIp;
    boAccept = pdata->boAccept;

    newsocketid = socket(AF_INET, SOCK_STREAM, 0);
    if (newsocketid < 0)
    {
        VNCLOGE("Error socket()");
        return (void*)-1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIp.c_str(), & servaddr.sin_addr) < 0)
    {
        VNCLOGE("Error inet_pton()");
        return (void*)-1;
    }

    trytimes = boAccept ? 5 : 3;

    while (1)
    {
        if (connect(newsocketid, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
        {
            VNCLOGE("Error connect(), trytime = %d , errorno = %d", trytimes, errno);
            trytimes--;
            if (trytimes <= 0)
            {
                return (void*)-1;
            }
			usleep(2000 * 1000);
            continue;
        }
        break;
    }

    VNCLOGD("connect() ok");

    if (boAccept == false)                          // 如果用户是拒绝
    {
        auto rejectpkg = VncDataPackage::create(crc32_hash((const unsigned char*)pushId.c_str(), strlen(pushId.c_str())),
                            serverid_crc32, T2S_CMD_REJECT_HELP, 0, std::string(pushId));
        std::string & ptcbuf = rejectpkg->getProtocolBuffer();
        write(newsocketid, ptcbuf.c_str(), ptcbuf.length());
        delete rejectpkg;
        shutdown(newsocketid, SHUT_RDWR);
        close(newsocketid);
        return (void*)0;
    }

    if (message_socket_id >= 0)         // 之前的socket正常,表示有上一台PC已经连接控制了，这时要通知它被挤掉了
    {
        VNCLOGD("notify server to close old connect ");
        auto pkg = VncDataPackage::create(pushid_crc32, serverid_crc32, T2S_CMD_ANOTHER_PC_CTRL, 0, pushid_string);
        sendPackage(pkg);
        usleep(500 * 1000);         // 等待命令发出去
    }

	KeyDispatchManager::getInstance()->init();
    virtual_key_init();
    cleanup();

    message_socket_id = newsocketid;
    setnonblocking(message_socket_id);

    // 保存新的Push-ID
    strncpy(pushid_string, pushId.c_str(), sizeof(pushid_string) - 1);
    pushid_string[sizeof(pushid_string) - 1] = 0;
    if (pushid_string[0] == 0)
    {
        strcpy(pushid_string, getmacaddress());
    }
    pushid_crc32 = crc32_hash((const unsigned char*)pushid_string, strlen(pushid_string));
    ipaddr = serverIp;

    cmdin = new VncCommandQueue;
    cmdout = new VncCommandQueue;

    threads = new VncThreads;
    addThread(&readprot_thread_handle, nullptr, vnc_read_channel, nullptr, readprot_thread_exit);
    addThread(&writeprot_thread_handle, nullptr, vnc_write_channel, nullptr, writeprot_thread_exit);
    addThread(&cmdprocessor_thread_handle, nullptr, vnc_command_processor, nullptr, cmdprocessor_thread_exit);

    return (void*)0;
}

static int writeback(int socket, const unsigned char * buffer, int writesize)
{
    int err;
    int size = writesize;
    int cnt = 0, n = 0;
    while (size > 0)
    {
        int wrtsize = (size > 1024) ? 1024 : size;
        n = send(socket, &buffer[cnt], wrtsize, MSG_NOSIGNAL);
        if (n <= 0)
        {
            err = errno;
            if (err != EINTR && err != EAGAIN && err != EWOULDBLOCK)
            {
                VNCLOGE("%s() call write() return %d, errno = %d", __FUNCTION__, n, errno);
                return -1;
            }
            usleep(20);
        }
        else
        {
            size -= n;
            cnt += n;
            // VNCLOGD("write byte = %d", n);
        }
    }
    return 0;
}

static void * vnc_write_channel(void * args)
{
    VncDataPackage * package = nullptr;
    writeprot_thread_exitflag = 0;
    int size, ret;

    VNCLOGD("fyb,enter %s()", __FUNCTION__);

    while (1)
    {
        package = nullptr;
        if (0 == cmdout->getPackage(&package))
        {
            if (package)
            {
                std::string & buf = package->getProtocolBuffer();
                size = buf.length();
                VNCLOGD("out package size = %d", buf.length());

                ret = writeback(message_socket_id, (unsigned char*) buf.c_str(), size);
                if (ret < 0)    // 写出错
                {
                    VNCLOGE("send package error, exit");
                    break;
                }
                //dump_package(package);

                delete package;
                package = nullptr;
            }
        }

        if (writeprot_thread_exitflag)
        {
            VNCLOGD("%s() exit thread", __FUNCTION__);
            break;
        }
    }

    exitThread(pthread_self(), nullptr);

    return nullptr;
}

static void * vnc_read_channel(void * args)
{
    int n, err, select_ret;
    fd_set rdfds;
    struct timeval tv;
    time_t now;
    time_t oldtime;
    VncBufferReader  bufreader(cmdin);
    char buffer[2048];

    VNCLOGD("fyb,enter %s()", __FUNCTION__);
    readprot_thread_exitflag = 0;

    time(&oldtime);
    while (1)
    {
        FD_ZERO(&rdfds);
        FD_SET(message_socket_id, &rdfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        select_ret = select(message_socket_id + 1, &rdfds, nullptr, nullptr, &tv);

        if (select_ret > 0)
        {
            if (FD_ISSET(message_socket_id, &rdfds))
            {
                n = recv(message_socket_id, buffer, sizeof(buffer), MSG_NOSIGNAL);
                if (n < 0)
                {
                    err = errno;
                    if (err != EINTR && err != EAGAIN && err != EWOULDBLOCK)
                    {
                        VNCLOGE("%s() call read() return %d, errno = %d", __FUNCTION__, n, errno);
						execJCommand("control_exit", "", ""); //add by fyb
                        break;
                    }
                }
                else if (n == 0)    // disconnect
                {
                    VNCLOGE("%s() call read() return %d", __FUNCTION__, n);
					execJCommand("control_exit", "", ""); //add by fyb
                    break;
                }
                else
                {
                    VNCLOGD("read byte = %d", n);
                    bufreader.addBuffer((unsigned char*)buffer, n);
                    //dump_data((unsigned char*)buffer, n);
                }
            }
        }
        else if (select_ret < 0)
        {
            err = errno;
            if (err != EINTR && err != EAGAIN && err != EWOULDBLOCK)
            {
                VNCLOGE("%s() call select() return %d, errno = %d", __FUNCTION__, select_ret, errno);
                break;
            }
        }
        else
        {   
            // 超时
        }
        //-----------------------------
        time(&now);
        if ((now - oldtime) > 60 * 3)
        {
            // 发送心跳包
            auto heartbeat_package = VncDataPackage::create(pushid_crc32, serverid_crc32, T2S_CMD_HEARTBEAT, 0);
            sendPackage(heartbeat_package);
            oldtime = now;
        }

        //-----------------------------
        if (readprot_thread_exitflag)
        {
            VNCLOGD("%s() exit thread", __FUNCTION__);
            break;
        }
    }

	pthread_t cleanup_handle;
	pthread_create(&cleanup_handle, nullptr, cleanup_thread, nullptr);

    exitThread(pthread_self(), nullptr);

    return nullptr;
}

static void * vnc_command_processor(void * args)
{
    VncDataPackage * package = nullptr;

    VNCLOGD("fyb,enter %s()", __FUNCTION__);

    // 告诉服务器,我自己的电视机TV-ID
    auto outpkg = VncDataPackage::create(pushid_crc32, 0x00000000, T2S_CMD_REG_TV, 0, std::string(pushid_string));
    sendPackage(outpkg);

    cmdprocessor_thread_exitflag = 0;
    while (1)
    {
        package = nullptr;
		if (0 == cmdin->getPackage(&package))
        {
            if (package)
            {
                processDataPackage(package);
                VNCLOGD("process byte = %d finish.", package->getProtocolBuffer().length());

                delete package;
                package = nullptr;
            }
        }

        if (cmdprocessor_thread_exitflag)
        {
            VNCLOGD("%s() exit thread", __FUNCTION__);
            break;
        }
    }

    exitThread(pthread_self(), nullptr);
    return nullptr;
}

void cleanup()
{
    VNCLOGD("%s()", __FUNCTION__);
    if (threads)
    {
        delete threads;
        threads = nullptr;
    }

    if (cmdin)
    {
        delete cmdin;
        cmdin = nullptr;
    }

    if (cmdout)
    {
        delete cmdout;
        cmdout = nullptr;
    }

    if (message_socket_id > 0)
    {
        shutdown(message_socket_id, SHUT_RDWR);
        close(message_socket_id);
        VNCLOGD("%s() call close() socketid=%d", __FUNCTION__, message_socket_id);
        message_socket_id = -1;
    }
}

void * cleanup_thread(void * args)
{
	usleep(100 * 1000); 
    cleanup();
    return nullptr;
}

static void readprot_thread_exit()
{
    readprot_thread_exitflag = 1;
}

static void writeprot_thread_exit()
{
    writeprot_thread_exitflag = 1;
}

static void cmdprocessor_thread_exit()
{
    cmdprocessor_thread_exitflag = 1;
}

void stopVncSession()
{
    writeprot_thread_exit();
    cmdprocessor_thread_exit();
    readprot_thread_exit();
}

void sendPackage(VncDataPackage * package)
{
    if (cmdout != nullptr)
    {
        cmdout->addPackage(package);
    }
    else
    {
        delete package;
    }
}

void setPCID(unsigned ipcid)
{
    char buffer[64];
    pcid_crc32 = ipcid;
    sprintf(buffer, "%d", pcid_crc32);
    strncpy(pcid_string, buffer, sizeof(pcid_string) - 1);
    pcid_string[sizeof(pcid_string) - 1] = 0;
    VNCLOGD("PC ID = %s, CRC32 = 0x%08X", pcid_string, pcid_crc32);
}

void setServerID(unsigned iserverid)
{
    char buffer[64];
    serverid_crc32 = iserverid;
    sprintf(buffer, "%d", serverid_crc32);
    strncpy(serverid_string, buffer, sizeof(serverid_string) - 1);
    serverid_string[sizeof(serverid_string) - 1] = 0;
    VNCLOGD("Server ID = %s, CRC32 = 0x%08X", serverid_string, serverid_crc32);
}


int addThread(pthread_t *ptid, const pthread_attr_t *attr, void*(*start_rtn)(void*), void *arg, void(*notify_exit)())
{
    if (threads)
    {
        threads->addThread(ptid, attr, start_rtn, arg, notify_exit);
    }
    return 0;
}

int exitThread(pthread_t pid, void* prval)
{
    if (threads)
    {
        threads->exitThread(pid, prval);
    }
    return 0;
}

static void handle_sig_child(int arg)
{
    pid_t   pid;
    int     stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        VNCLOGD("process %d terminated. \n", pid);
    }
    return;
}

void dump_data(unsigned char * data, int size)
{
    const unsigned char * p;
    p = data;

    while (size > 0)
    {
        if (size >= 16)
        {
            VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", 
                p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07], 
                p[ 8], p[ 9], p[10], p[11], p[12], p[13], p[14], p[15]);
            size -= 16;
            if (size <= 0)
                break;
            p += 16;
        }
        else
        {
            switch (size)
            {
            case 15:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07],
                    p[8], p[9], p[10], p[11], p[12], p[13], p[14]);
                break;
            case 14:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07],
                    p[8], p[9], p[10], p[11], p[12], p[13]);
                break;
            case 13:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07],
                    p[8], p[9], p[10], p[11], p[12]);
                break;
            case 12:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07],
                    p[8], p[9], p[10], p[11]);
                break;
            case 11:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07],
                    p[8], p[9], p[10]);
                break;
            case 10:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07],
                    p[8], p[9]);
                break;
            case 9:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07],
                    p[8]);
                break;
            case 8:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06], p[07]);
                break;
            case 7:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05], p[06]);
                break;
            case 6:
                VNCLOGD("      %02x %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04], p[05]);
                break;
            case 5:
                VNCLOGD("      %02x %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03], p[04]);
                break;
            case 4:
                VNCLOGD("      %02x %02x %02x %02x",
                    p[00], p[01], p[02], p[03]);
                break;
            case 3:
                VNCLOGD("      %02x %02x %02x",
                    p[00], p[01], p[02]);
                break;
            case 2:
                VNCLOGD("      %02x %02x",
                    p[00], p[01]);
                break;
            case 1:
                VNCLOGD("      %02x",
                    p[00]);
                break;
            }
            break;
        }
    }
    return;
}

void dump_package(VncDataPackage * package)
{
    unsigned char * p;
    int size;
    p = (unsigned char *)package->getProtocolBuffer().c_str();
    size = package->getProtocolBuffer().length();

    dump_data(p, size);
}

static void * delete_modify_oom_shell_file(void * args)
{
    sleep(1);
    unlink("/data/tmp1.sh");
}

static void   modify_oom_value()
{
    int fd;
    int pid;
    char cmd[256];

    pid = getpid();
    sprintf(cmd, "echo -900 > /proc/%d/oom_score_adj\n", pid);

    fd = open("/data/tmp1.sh", O_WRONLY | O_CREAT | O_TRUNC, 
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP /*| S_IROTH | S_IWOTH | S_IXOTH*/);

    if (fd < 0)
    {
        VNCLOGD("cannot create temp file.");
        return;
    }

    write(fd, cmd, strlen(cmd));
    close(fd);

    execAsRoot("/system/bin/sh -c /data/tmp1.sh");

    pthread_t remove_script_thread_handle;
    pthread_create(&remove_script_thread_handle, NULL, delete_modify_oom_shell_file, NULL);

    return;
}



