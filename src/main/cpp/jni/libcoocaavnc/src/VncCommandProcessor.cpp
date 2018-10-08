#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "cutils/properties.h"

#include "CommandId.h"
#include "VncCtrl.h"
#include "VncUtils.h"
#include "VncCommandProcessor.h"
#include "VncDataPackage.h"
#include "VncTelnetAgent.h"
#include "VncLogcatAgent.h"
#include "com_coocaa_remotectrlservice_JniUtils.h"

// 截取屏幕图片
static void * catchScreenPhotoThread(void*);
// 截取连续图片
static void * catchScreenMultiPhotoThread(void* args);
// 退出截取连续图片
static void exitScreenMultiPhotoThread();
// 抓取数段时间的LOGCAT
static void * catchLogcatThread(void*);
// 退出抓取LOG
static void exitcatchLogcatThread();

static void * pushFileThread(void*);

static void * pullFileThread(void*);

bool is_rev_pcid = false;

void my_alarm_handler(int value)
{
	VNCLOGD("my_alarm_handler is_rev_pcid = %d ", is_rev_pcid);
	if (!is_rev_pcid)
	{
	//	execJCommand("control_exit", "", "");
		auto pkg = VncDataPackage::create(pushid_crc32, serverid_crc32, T2S_CMD_TV_EXIT_CTRL, 0, 0);
		sendPackage(pkg);	
		usleep(300 * 1000); 
	//	cleanup();	

		pthread_t cleanup_handle;
		pthread_create(&cleanup_handle, nullptr, cleanup_thread, nullptr);

		exitThread(pthread_self(), nullptr);
	}
}

// 处理命令数据包
int processDataPackage(VncDataPackage * package)
{
    unsigned cmdid;

    cmdid = package->getCommandId();
    VNCLOGD("command id = 0x%08x ", cmdid);

    switch (cmdid)
    {
    case T2S_CMD_HEARTBEAT:
        break;
    case T2S_RET_REG_TV:
        break;
    case T2S_CMD_TELLME_SRV_ID:
    {
        unsigned id = package->getInt32Param();
        setServerID(id);
        auto retpkg = VncDataPackage::create(pushid_crc32, serverid_crc32, T2S_RET_TELLME_SRV_ID, package->getTag(), 0);
        sendPackage(retpkg);

		is_rev_pcid = false;
		signal(SIGALRM,my_alarm_handler);
		alarm(40);
    }
    break;

    case T2S_CMD_TELLME_PC_ID:
    {
		is_rev_pcid = true;
        unsigned id = package->getInt32Param();
        setPCID(id);
        auto retpkg = VncDataPackage::create(pushid_crc32, serverid_crc32, T2S_RET_TELLME_PC_ID, package->getTag(), 0);
        sendPackage(retpkg);
		execJCommand("control_success", "", "");
    }
    break;

	case T2P_CMD_GET_TV_BASEINFO:
		{
			std::string result = execJCommandR("GET_TV_BASEINFO","","");
			VNCLOGD("execJCommandR result = %s\n",result.c_str());
			auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_GET_TV_BASEINFO, package->getTag(), result);
			sendPackage(retpkg);
		}
		break; 

    case T2S_CMD_NOTIFY_PC_OFFLINE:
    {
		exitScreenMultiPhotoThread(); 
		exitcatchLogcatThread();
	//	execJCommand("notify_pc_offline", "", "");
        stopVncSession();
    }
    break;

    case T2S_CMD_SEND_VIRKEY:
    {
        unsigned key;
        key = package->getInt32Param();
        VNCLOGD("key = %d", key);
        embVirtualKeyClick(key);
        auto retpkg = VncDataPackage::create(pushid_crc32, serverid_crc32, T2S_RET_SEND_VIRKEY, package->getTag(), 0);
        sendPackage(retpkg);
    }
    break;

    case T2P_CMD_START_TELNETD:
    {
        int res = TelnetAgent::startTelnetService();
        auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_START_TELNETD, package->getTag(), res);
        sendPackage(retpkg);
    }
    break;

    case T2P_CMD_STOP_TELNETD:
    {
        TelnetAgent::stopTelnetService();
        auto retpkg = VncDataPackage::create(pushid_crc32, serverid_crc32, T2P_RET_STOP_TELNETD, package->getTag(), 0);
        sendPackage(retpkg);
    }
    break;

    case T2P_CMD_TELNET_DATA:
    {
        std::string str = package->getStringParam();
        if (str.length())
        {
            TelnetAgent::sendData((const unsigned char*)str.c_str(), str.length());
        }
    }
    break;

    case T2P_CMD_SNATCH_LOG:
    {
        // 抓取一段时间日志并保存到文件中,然后发回PC
        static CatchLogInfo info;
        pthread_t id;
        info.m_tag = package->getTag();
        info.m_second = package->getInt32Param();
        addThread(&id, nullptr, catchLogcatThread, (void*)&info, exitcatchLogcatThread);

        auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_SNATCH_LOG, package->getTag(), 0);
        sendPackage(retpkg);
    }
    break;

    case T2P_CMD_START_SNATCH_LOG:
    {
        // 抓取一段时间日志并保存到文件中,然后发回PC
        static CatchLogInfo info;
        pthread_t id;
        info.m_tag = package->getTag();
        info.m_second = 5 * 60;
        addThread(&id, nullptr, catchLogcatThread, (void*)&info, exitcatchLogcatThread);
    }
    break;

    case T2P_CMD_STOP_SNATCH_LOG:
    {
        exitcatchLogcatThread();
    }
    break;

	case T2P_CMD_LOGCAT_START_SCREEN:
	{
        LogcatAgent::startConnect();
	}
	break;

	case T2P_CMD_LOGCAT_PARAM_SCREEN:
	{
		static PrintLogInfo info;						
		info.m_tag = package->getTag();
		info.m_cmd = package->getStringParam();
        LogcatAgent::sendCommand(info.m_cmd.c_str(), info.m_tag);
	}
	break;

	case T2P_CMD_LOGCAT_STOP_SCREEN:
	{
        LogcatAgent::stopConnect();
	}
	break;

    case T2P_CMD_PRINTE_SCREEN:
    {
        pthread_t id;
        unsigned tag = package->getTag();
        addThread(&id, nullptr, catchScreenPhotoThread, (void*)tag, nullptr);
    }
    break;

    case T2P_CMD_PRINTE_CONTINUE_SCREEN:
    {
        pthread_t id;
        static CatchPhotoInfo info;
        info.m_tag = package->getTag();
        info.m_second = package->getInt32Param();
        addThread(&id, nullptr, catchScreenMultiPhotoThread, (void*)&info, exitScreenMultiPhotoThread);
    }
    break;

    case T2P_CMD_STOP_SCREEN:
    {
        exitScreenMultiPhotoThread();
    }
    break;

	case T2P_CMD_REMOTE_PUSH_FILE:
	{
		pthread_t id;
		static PushFileInfo info;
		info.m_tag = package->getTag();
        info.m_downloadUrl = package->getStringValue("server-url");
		VNCLOGD("command m_downloadUrl=%s", info.m_downloadUrl.c_str());
        info.m_tvpath = package->getStringValue("tv-path");
		VNCLOGD("command  m_tvpath=%s", info.m_tvpath.c_str());
		int value = 0;
		int result = package->getIntValue("file-size", value);
		info.m_filesize = value;
		VNCLOGD("command result = %d, m_filesize=%d", result, info.m_filesize);
		if (info.m_downloadUrl == "" || info.m_tvpath == "" || info.m_filesize == 0)
		{
			auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_REMOTE_PUSH_FILE, info.m_tag, -4);
			sendPackage(retpkg);
			break;
		}		
		addThread(&id, nullptr, pushFileThread, (void*)&info, nullptr);
	}
	break;

	case T2P_CMD_REMOTE_PULL_FILE:
	{
		pthread_t id;
		static PullFileInfo info;
		info.m_tag = package->getTag();
		info.m_tvpath = package->getStringParam();
		if (info.m_tvpath == "")
		{
			auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_REMOTE_PULL_FILE, info.m_tag, -4);
			sendPackage(retpkg);
			break;
		}
		addThread(&id, nullptr, pullFileThread, (void*)&info, nullptr);
	}
	break;

	case T2P_CMD_UPGRADE_CHECK:
		{
			execJCommand("upgrade_check", "", "");
		}
		break;

    default:
        break;
    }

    return 0;
}

volatile int exitScreenFlag = 0;                // 标志，是否应该退出抓图程序

// 抓取屏幕图片
static void * catchScreenPhotoThread(void* args)
{
    bool ret;
	char fullname[256] = {0};
	char basename[64] = {0};
    unsigned tag = (unsigned)(args);

    exitScreenFlag = 0;
    snprintf(basename, sizeof(basename), "catch_photo_%08x_%08x.jpg", pushid_crc32, tag);
    snprintf(fullname, sizeof(fullname), "/data/%s", basename);

    ret = execJCommand("catch_screen", fullname, "");
    if (ret == true)
    {
        // 截图成功，传图片到中转服务器
        ret = simpleFtpFile(SIMFTP_OPT_PUT_FILE, basename, fullname, SIMFTP_FILE_TYPE_SCREEN, pushid_crc32, pcid_crc32, tag);
        if (ret == 0)
        {
            // 传成功后，是否要告诉服务器与PC ?
			auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_PRINTE_SCREEN, tag, ret);
			sendPackage(retpkg);
        }
        else
        {
            // 传失败后，是否要告诉服务器与PC ?
			auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_PRINTE_SCREEN, tag, ret);
			sendPackage(retpkg);
        }
		unlink(fullname);
    }

    exitThread(pthread_self(), nullptr);

    return nullptr;
}

// 抓取屏幕图片(连续)
static void * catchScreenMultiPhotoThread(void* args)
{
    bool ret;
    char fullname[128];
    char basename[64];
    int counter = 0;
    CatchPhotoInfo info = *((CatchPhotoInfo*)args);
    unsigned tag = info.m_tag;
    unsigned second = info.m_second;
    time_t old, now;

    exitScreenFlag = 0;
    time(&old);
    while (1)
    {
        snprintf(basename, sizeof(basename), "catch_photo_%08x_%08x_%d.jpg", pushid_crc32, tag, counter);
        snprintf(fullname, sizeof(fullname), "/data/%s", basename);

        ret = execJCommand("catch_screen", fullname, "");
        if (ret == true)
        {
            // 截图成功，传图片到中转服务器
            ret = simpleFtpFile(SIMFTP_OPT_PUT_FILE, basename, fullname, SIMFTP_FILE_TYPE_SCREEN, pushid_crc32, pcid_crc32, tag);
            if (ret == 0)
            {
                // 传成功后，是否要告诉服务器与PC ?
            }
            else
            {
                // 传失败后，是否要告诉服务器与PC ?
            }
			unlink(fullname);
        }
        counter++;

		if (counter > 200)
		{
			counter = 0;
			goto end;
		}

        while (1)
        {
            if (exitScreenFlag)
            {
                VNCLOGD("exit %s()", __FUNCTION__);
                goto end;
            }

            time(&now);
            if (now - old >= second)
            {
                old = now;
                break;
            }
            else
            {
                usleep(100*1000);
            }

            if (exitScreenFlag)
            {
                VNCLOGD("exit %s()", __FUNCTION__);
                goto end;
            }
        }

        
    }
end:

    exitThread(pthread_self(), nullptr);

    return nullptr;
}

static void exitScreenMultiPhotoThread()
{
    exitScreenFlag = 1;
}

volatile int exitLogcatThreadFlag = 0;

// static void * catchLogcatThread(void* args)
// {
//     char basename[64];
//     char filename[64];
//     int pid;
//     time_t old, now;
//     CatchLogInfo info = *((CatchLogInfo*)args);
// 
//     VNCLOGD("%s()", __FUNCTION__);
// 
//     if (info.m_second == 0)
//         info.m_second = 1000;
// 
//     sprintf(basename, "logcat_%d.log", info.m_tag);
//     sprintf(filename, "/data/%s", basename);
// 
//     const char *exec_argv[4];
//     exec_argv[0] = "vnc";
//     exec_argv[1] = "logcat";
//     exec_argv[2] = filename;
//     exec_argv[3] = nullptr;
// 
//     pid = fork();
//     if (pid < 0)
//     {
//         VNCLOGD("fork error");
//         exitThread(pthread_self(), nullptr);
//         return nullptr;
//     }
//     else if (pid == 0)
//     {
//         execv("/system/vendor/bin/vnc", (char* const*)exec_argv);
//         exit(-1);
//     }
// 
//     VNCLOGD("%s() fork ok, pid = %d", __FUNCTION__, pid);
// 
//     exitLogcatThreadFlag = 0;
//     time(&old);
//     while (1)
//     {
//         time(&now);
//         if (now - old > info.m_second)
//         {
//             VNCLOGD("timeout, exit %s()", __FUNCTION__);
//             break;
//         }
// 
//         usleep(100 * 1000);
// 
//         if (exitLogcatThreadFlag)
//         {
//             VNCLOGD("exit %s()", __FUNCTION__);
//             break;
//         }
//     }
// 
//     kill(pid, SIGKILL);
//     int state;
//     waitpid(pid, &state, 0);
// 
//     VNCLOGD("waitpid end in %s()", __FUNCTION__);
//     simpleFtpFile(SIMFTP_OPT_PUT_FILE, basename, filename, SIMFTP_FILE_TYPE_LOG, pushid_crc32, pcid_crc32, info.m_tag);
// 
//     exitThread(pthread_self(), nullptr);
//     return nullptr;
// }

static void * catchLogcatThread(void* args)
{
    char basename[64];
    char filename[64];
    int pid;
    time_t old, now;
    CatchLogInfo info = *((CatchLogInfo*)args);

    VNCLOGD("%s()", __FUNCTION__);

    if (info.m_second == 0)
        info.m_second = 300;

    sprintf(basename, "logcat_%08x_to_%08x_%d.log", pushid_crc32, pcid_crc32, info.m_tag);
    sprintf(filename, "/data/%s", basename);

    execJCommand("logcat_start", "/data/", basename);

    exitLogcatThreadFlag = 0;
    time(&old);
    while (1)
    {
        time(&now);
        if (now - old > info.m_second)
        {
            VNCLOGD("timeout, exit %s()", __FUNCTION__);
            break;
        }

        usleep(100 * 1000);

        if (exitLogcatThreadFlag)
        {
            VNCLOGD("exit %s()", __FUNCTION__);
            break;
        }
    }

    execJCommand("logcat_stop", "/data/", basename);

    int ret = simpleFtpFile(SIMFTP_OPT_PUT_FILE, basename, filename, SIMFTP_FILE_TYPE_LOG, pushid_crc32, pcid_crc32, info.m_tag);

	auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_SNATCH_LOG, info.m_tag, ret);
	sendPackage(retpkg);

	unlink(filename);

    exitThread(pthread_self(), nullptr);
    return nullptr;
}

// 退出抓取LOG
static void exitcatchLogcatThread()
{
    exitLogcatThreadFlag = 1;
}

static void * pushFileThread(void * args)
{
	char tmpname[64] = {0};
	char fullurl[512] = {0};
	PushFileInfo info = *((PushFileInfo*)args);

	VNCLOGD("%s(),tag=%d, path=%s, target_path=%s", __FUNCTION__, info.m_tag, info.m_downloadUrl.c_str(), info.m_tvpath.c_str());
	sprintf(tmpname, "/data/%d.bin", rand());
	sprintf(fullurl, "http://%s/%s", ipaddr.c_str(), info.m_downloadUrl.c_str());
	int res = curlDownloadFile(fullurl, tmpname, nullptr);
    if (res < 0)    // fail
    {
		VNCLOGD("%s(),curlDownloadFile error", __FUNCTION__);
		auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_REMOTE_PUSH_FILE, info.m_tag, res);
        sendPackage(retpkg);
    }
    else if (res == 0)      // download ok
    {
        bool needRoot = true;
        int execret;
		char cmdstring[512] = { 0 };

		snprintf(cmdstring, sizeof(cmdstring)-1, "busybox mv -f %s %s; chmod 755 %s", tmpname, info.m_tvpath.c_str(), info.m_tvpath.c_str());

		VNCLOGD("pushFileThread cmdstring = %s", cmdstring);

        if (needRoot)
            execret = execAsRoot(cmdstring);
        else
            execret = system(cmdstring);

		VNCLOGD("%s(),exec=%d", __FUNCTION__, execret);
      
		//unlink(tmpname);

        auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_REMOTE_PUSH_FILE, info.m_tag, execret);
        sendPackage(retpkg);
    }

	exitThread(pthread_self(), nullptr);
	return nullptr;
}

static void * pullFileThread(void * args)
{
	char basename[64] = { 0 };
	PullFileInfo info = *((PullFileInfo*)args);

	VNCLOGD("%s(),tag=%d, path=%s", __FUNCTION__, info.m_tag, info.m_tvpath.c_str());
	sprintf(basename, "common_%08x_to_%08x_%d", pushid_crc32, pcid_crc32, info.m_tag);

	int ret = simpleFtpFile(SIMFTP_OPT_PUT_FILE, basename, info.m_tvpath.c_str(), SIMFTP_FILE_TYPE_COMMON, pushid_crc32, pcid_crc32, info.m_tag);

	auto retpkg = VncDataPackage::create(pushid_crc32, pcid_crc32, T2P_RET_REMOTE_PULL_FILE, info.m_tag, ret);
	sendPackage(retpkg);

	exitThread(pthread_self(), nullptr);
	return nullptr;
}


#define SIMFTP_HEAD_FIXED_SIZE              (40)

// 利用简单文件传输协议跟服务器传输文件
int simpleFtpFile(int opt, const char * remoteFileName, const char * localFileName, int filetype, unsigned source, unsigned target, unsigned tag)
{
	int ret = -1;
    int skid;
    int trytimes;
    struct sockaddr_in  servaddr;
    unsigned filesize = 0, cnt;
    unsigned v32;                                   // big-endian值
    unsigned char * filebuf = nullptr;
    unsigned char * headbuf = nullptr;
    unsigned filecrc32 = 0;
    int size, n;                                    // 读写计数变量
    unsigned rsp;                                   // 服务器应答

    skid = socket(AF_INET, SOCK_STREAM, 0);
    if (skid < 0)
    {
        VNCLOGE("Error socket()");
        return errno;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(fport);
    if (inet_pton(AF_INET, ipaddr.c_str(), &servaddr.sin_addr) < 0)
    {
        VNCLOGE("Error inet_pton()");
		return errno;
    }

    trytimes = 10;
    while (1)
    {
        if (connect(skid, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
        {
			VNCLOGE("Error connect(), trytime = %d errorno = %d", trytimes, errno);
            trytimes--;
            if (trytimes <= 0)
            {
                return -1;
            }
            if (exitScreenFlag)
            {
                return -1;
            }
            continue;
        }
        break;
    }

    VNCLOGD("connect() ok");

    if (opt == SIMFTP_OPT_PUT_FILE)
    {
		if (access(localFileName, F_OK) != 0)
		{
			VNCLOGD("file is not exist!!");
			ret = -2; 
			goto Fail;
		}	
		VNCLOGD("ready to open file...");
		FILE * fp = nullptr;
        fp = fopen(localFileName, "r");
        if (fp)
        {
			VNCLOGD("ready to read file to buffer...");
			struct stat filestat;
			filesize = stat(localFileName, &filestat);
			filesize = (unsigned)filestat.st_size;
			filebuf = new unsigned char[filesize];
			VNCLOGD("filestat.st_size=%lld,filesize = %u", filestat.st_size, filesize);


            size = filesize;
            cnt = 0;
            while (size > 0)
            {
                n = fread(&filebuf[cnt], 1, size, fp);
                size -= n;
                cnt += n;
            }
            fclose(fp);
        }
        else
        {
			VNCLOGD("open file error");
			ret = -3; 
            delete[] filebuf;
			filebuf = nullptr;
            goto Fail;
        }
        filecrc32 = crc32_hash(filebuf, filesize);
    }

    headbuf = new unsigned char[SIMFTP_HEAD_FIXED_SIZE + strlen(remoteFileName)];
    
    cnt = 0;
    //协议头部大小
    v32 = htonl(SIMFTP_HEAD_FIXED_SIZE + strlen(remoteFileName));
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //操作类型
    v32 = htonl(opt);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //文件类型
    v32 = htonl(filetype);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //目标标识
    v32 = htonl(pcid_crc32);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //源标识
    v32 = htonl(pushid_crc32);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //命令字的标签
    v32 = htonl(tag);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //文件大小
    v32 = htonl(filesize);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //校验和
    v32 = htonl(filecrc32);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //保留
    v32 = htonl(0);
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    //文件名长度
    v32 = htonl(strlen(remoteFileName));
    memcpy(&headbuf[cnt], &v32, 4);
    cnt += 4;

    // 文件名
    memcpy(&headbuf[cnt], remoteFileName, strlen(remoteFileName));

    //////////////////////////////////////////////
    // 写入简单文件协议头部
    size = SIMFTP_HEAD_FIXED_SIZE + strlen(remoteFileName);
    cnt = 0;
    while (size > 0)
    {
        n = write(skid, &headbuf[cnt], size);
		VNCLOGD("write request to server n = %d", n);
        if (n == 0)
        {
            VNCLOGD("write simple ftp header error.");
			ret = errno;
            goto Fail;
        }
        size -= n;
        cnt += n;
        //if (exitScreenFlag)
        //    goto Fail;
    }

    delete[] headbuf;
    headbuf = nullptr;

	VNCLOGD("wait server response...exitScreenFlag = %d",exitScreenFlag);

    // 读取服务器应答
    size = 4;
    cnt = 0;
    while (size > 0)
    {
        n = read(skid, &(((char*)&rsp)[cnt]), size);
		VNCLOGD("read response from server n = %d", n);
        if (n == 0)
        {
            VNCLOGD("get simple ftp server rsp error.");
			ret = errno;
            goto Fail;
        }
        size -= n;
        cnt += n;
        //if (exitScreenFlag)
        //    goto Fail;
    }

    // 判断服务器是否拒绝
    if (rsp == 0)
    {
        VNCLOGD("simple ftp server say no.");
		ret = -5;
        goto Fail;
    }

    if (opt == SIMFTP_OPT_PUT_FILE)                 // 发送文件
    {
        fd_set wrfds;
        struct timeval tv;
        int select_ret;
        size = filesize;
        cnt = 0;
        while (size > 0)
        {
            FD_ZERO(&wrfds);
            FD_SET(skid, &wrfds);
            tv.tv_sec = 0;
            tv.tv_usec = 100 * 1000;
            select_ret = select(skid + 1, nullptr, &wrfds, nullptr, &tv);
            if (select_ret > 0)
            {
                if (FD_ISSET(skid, &wrfds))
                {
                    int wrsize = (size > 1024) ? 1024 : size;
                    n = write(skid, &filebuf[cnt], wrsize);
                    size -= n;
                    cnt += n;
                }
            }
            else if (select_ret < 0)
            {
                VNCLOGD("write file simple ftp server select() error %d. ", select_ret);
				ret = errno;
                goto Fail;
                break;
            }

	/*		if (exitScreenFlag)
			{
				goto Fail;
			}         */
        }

        if (filebuf)
        {
            delete[] filebuf;
            filebuf = nullptr;
        }
    }
    else                                            // 接收文件
    {
        FILE * lfile;
        char tmpbuffer[1024];
        filesize = rsp;
        if (filesize >= 1024 * 1024 * 1024)          // 大于这个值表示文件太大，不处理 
        {
            VNCLOGD("get file to large, size = %d", filesize);
            goto Fail;
        }

        lfile = fopen(localFileName, "w");
        if (lfile == nullptr)
        {
            VNCLOGD("open file error.  %s", localFileName);
            goto Fail;
        }

        size = filesize;
        while (size > 0)
        {
            int rdsize = (size > 1024) ? 1024 : size;
            n = read(skid, tmpbuffer, rdsize);
            if (n)
                fwrite(tmpbuffer, 1, n, lfile);
            if (n == 0)
            {
                fclose(lfile);
                VNCLOGD("get file from simple ftp server error.");
                goto Fail;
            }
            size -= n;

			if (exitScreenFlag){
				goto Fail;
			}
                
        }
        fclose(lfile);

    }


    if (filebuf != nullptr)
    {
        delete[] filebuf;
        filebuf = nullptr;
    }
    if (headbuf != nullptr)
    {
        delete[] headbuf;
        headbuf = nullptr;
    }

    shutdown(skid, SHUT_RDWR);
    close(skid);

	VNCLOGD("success end");
    return 0;

Fail:
	VNCLOGD("go to fail");
    if (filebuf != nullptr)
    {
        delete[] filebuf;
        filebuf = nullptr;
    }
    if (headbuf)
    {
        delete[] headbuf;
        headbuf = nullptr;
    }
    shutdown(skid, SHUT_RDWR);
    close(skid);
    return ret;
}



