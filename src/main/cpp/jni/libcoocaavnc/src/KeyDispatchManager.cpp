#include "KeyDispatchManager.h"
#include "VncUtils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <pthread.h>
#include "cutils/properties.h"

#include "com_coocaa_remotectrlservice_JniUtils.h"
#include "VncCtrl.h"
#include "VncDataPackage.h"
#include "CommandId.h"
#include "VncUtils.h"

//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

static void get_bt_event(char ** DEVICE_PATH);

KeyDispatchManager::Garbo KeyDispatchManager::garbo;
KeyDispatchManager* KeyDispatchManager::m_instance = nullptr;

int device_fd = -1;

static int is_cancel_control(int code, int value, int now)
{
	static int set[] = { 158, 158, 158 };
	static int codes[] = { 0, 0, 0 };

	static int idx = 0;
	static int t = 0;

	if (value == 1) return -1;

	if (code != 158)
	{
		idx = 0;
		return -1;
	}

	if (idx > 0 && now - t > 2000) idx = 0;

	t = now;

	if (idx == 3) idx = 0;

	codes[idx++] = code;
	if (idx == 3)
	{
		idx = 0;

		return memcmp(codes, set, sizeof(set)) == 0 ? 0 : -1;
	}

	return -1;
}

static int is_initiative_request(int code, int value, int now)
{
//	VNCLOGD("is_initiative_request code = %d, value = %d, now = %d\n",code,value,now);
	static int set[] = { 103, 108, 105, 106, 103};
	static int codes[] = { 0, 0, 0, 0,0};

	static int idx = 0;
	static int t = 0;

	if (value == 1) return -1;

	if (code != 103 && code != 108 && code != 105 && code != 106)
	{
		idx = 0;
		return -1;
	}

	if (idx > 0 && now - t > 2000) idx = 0;

	t = now;

	if (idx == 5) idx = 0;

	codes[idx++] = code;

	if (idx == 5)
	{
		idx = 0;

		return memcmp(codes, set, sizeof(set)) == 0 ? 0 : -1;
	}

	return -1;
}

static int is_initiative_request2(int code, int value, int now)
{
//	VNCLOGD("is_initiative_request code = %d, value = %d, now = %d\n",code,value,now);
	static int set[] = {105, 106, 103, 108};
	static int codes[] = { 0, 0, 0, 0};

	static int idx = 0;
	static int t = 0;

	if (value == 1) return -1;

	if (code != 103 && code != 108 && code != 105 && code != 106)
	{
		idx = 0;
		return -1;
	}

	if (idx > 0 && now - t > 2000) idx = 0;

	t = now;

	if (idx == 4) idx = 0;

	codes[idx++] = code;

	if (idx == 4)
	{
		idx = 0;

		return memcmp(codes, set, sizeof(set)) == 0 ? 0 : -1;
	}

	return -1;
}

static int handle_ir_event(int code, int value, int now)
{
	VNCLOGD("receive key-------------->ir event: code=%d, value=%d\n", code, value);
	if(value == 0){
		if(is_cancel_control(code,value,now) == 0)
		{
			VNCLOGD("cancel remote control\n");
			execJCommand("control_exit", "", "");
			auto pkg = VncDataPackage::create(pushid_crc32, serverid_crc32, T2S_CMD_TV_EXIT_CTRL, 0, 0);
			sendPackage(pkg);
			usleep(50 * 1000);
		//	device_fd = -1;
		//	cleanup();		
		}
	/*	if (is_initiative_request(code,value,now) == 0)
		{
			VNCLOGD("is_initiative_request start\n");
			std::string cmdstring = "am broadcast -a com.coocaa.initiative.request";
			system(cmdstring.c_str());
			VNCLOGD("is_initiative_request end\n");
		}
		if (is_initiative_request2(code,value,now) == 0)
		{
			VNCLOGD("is_initiative_request2 start\n");
			std::string cmdstring = "am broadcast -a com.coocaa.initiative.request2";
			system(cmdstring.c_str());
			VNCLOGD("is_initiative_request2 end\n");
		} */

	}
	return 0;
}

#define SKYMODEL        "ro.build.skymodel"
#define SKYTYPE         "ro.build.skytype"

static void *ir_hook_proc(void *arg)
{
    int fd;
    int retry = 3;
    char skymodel[PROPERTY_VALUE_MAX];
    char skytype[PROPERTY_VALUE_MAX];
    const char *DEVICE_PATH;
    char sendcmd[128];

    property_get(SKYMODEL, skymodel, "");
    property_get(SKYTYPE, skytype, "");
	
	sendCtrlCmd("get_all_input_devices_info");
	usleep(500*1000);
	
	get_bt_event((char**)&DEVICE_PATH);
	//DEVICE_PATH = "/dev/input/event7";
	
	VNCLOGD("DEVICE_PATH = %s", DEVICE_PATH);

	/*
    if (0 == strcmp(skymodel, "GHD08") && 0 == strcmp(skytype, "HC2910"))
    {
        DEVICE_PATH = "/dev/input/event8";
        snprintf(sendcmd, sizeof(sendcmd), "accredit_read_input_event %s", DEVICE_PATH);
    }
    else
    {
        DEVICE_PATH = "/dev/input/event0";
        strcpy(sendcmd, "accredit_read_input_event");
    }*/
	snprintf(sendcmd, sizeof(sendcmd), "accredit_read_input_event %s", DEVICE_PATH);

    sendCtrlCmd(sendcmd);
    usleep(100 * 1000);

	struct epoll_event ev, events[32];
	struct input_event ev_buf[32];
	int ep_fd = epoll_create(256);
    while (retry > 0)
    {
        fd = open(DEVICE_PATH, O_RDONLY | O_CLOEXEC);
        if (fd >= 0)
            break;
        usleep(100 * 1000);
        retry--;
    }
	
	VNCLOGD("ir_hook_proc open-------------->fd=%d\n", fd);
	
	device_fd = fd; //add by fyb

	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &ev);

	for (;;)
	{
		int i, j;
		int nfds = epoll_wait(ep_fd, events, sizeof(events) / sizeof(struct epoll_event), -1);
		if (nfds < 0) continue;
		for (i = 0; i < nfds; i++)
		{
			if (events[i].data.fd != fd)
				continue;

			if ((j = read(fd, ev_buf, sizeof(ev_buf) / sizeof(struct input_event))) <= 0)
			{
				if (errno == EAGAIN || errno == EINTR)
					continue;

				close(fd);
				events[i].data.fd = -1;

				VNCLOGD("ir_hook_proc error\n");

				break;
			}
			else
			{
				int m, n;
				m = j / sizeof(struct input_event);
				for (n = 0; n < m; n++)
				{
					struct input_event ir_ev = ev_buf[n];
					//printf("event: time=%d.%06d, type=%d, code=%d, value=%d\n", (int)ir_ev.time.tv_sec, (int)ir_ev.time.tv_usec, ir_ev.type, ir_ev.code, ir_ev.value);

					if (ir_ev.type != EV_KEY) continue;

					handle_ir_event(ir_ev.code, ir_ev.value, (int)(ir_ev.time.tv_sec * 1000 + ir_ev.time.tv_usec / 1000));
				}

				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &ev);
			}
		}
	}

	VNCLOGD("ir_hook_proc closed\n");
	close(ep_fd);

	return 0;
}

KeyDispatchManager::KeyDispatchManager()
{

}

KeyDispatchManager::~KeyDispatchManager()
{
	VNCLOGD("KeyDispatchManager::~KeyDispatchManager\n");
}

KeyDispatchManager * KeyDispatchManager::getInstance()
{
	if(m_instance == nullptr)
	{
		m_instance = new KeyDispatchManager();
	} 
	return m_instance;
}

void KeyDispatchManager::init()
{
	VNCLOGD("KeyDispatchManager::init() device_fd= %d\n",device_fd);
	if (device_fd == -1)
	{
		pthread_t tid;
		pthread_create(&tid, NULL, ir_hook_proc, NULL);
	}

}

static void get_bt_event(char ** DEVICE_PATH)
{
	FILE * fp;
	bool found_bt = false;
	int i;
	char linedata[512];
	
	fp = fopen("/data/input_devices.txt", "r");
	if (fp == NULL)
	{
		VNCLOGD("/data/input_devices.txt  fp == NULL\n");
		*DEVICE_PATH = (char*)("/dev/input/event0");
		return;
	}
	
	memset(linedata, 0, sizeof(linedata));
	fgets(linedata, sizeof(linedata) - 1, fp);
	while (1)
	{
		if (feof(fp))
			break;
		
		for (i = 0; ; i++)
		{
			if (linedata[i] == '\r' || linedata[i] == '\n')
				linedata[i] = 0;
			if (linedata[i] == 0)
				break;
		}
		
		if (strstr(linedata, "Name=\"Skyworth_BT_RC"))
		{
			found_bt = true;
		}
		
		if (found_bt)
		{
			if (strstr(linedata, "Handlers="))
			{
				char * start;
				start = strstr(linedata, "event");
				if (start)
				{
					char * p = strchr(start, ' ');
					if (p)
						*p = 0;
					static char inputname[64];
					strcpy(inputname, "/dev/input/");
					strcat(inputname, start);
					*DEVICE_PATH = inputname;
					return ;
				}
			}
		}
		
		memset(linedata, 0, sizeof(linedata));
		fgets(linedata, sizeof(linedata) - 1, fp);
	}
	
	fseek(fp, 0, SEEK_SET);
	
	memset(linedata, 0, sizeof(linedata));
	fgets(linedata, sizeof(linedata) - 1, fp);
	while (1)
	{
		if (feof(fp))
			break;
		
		for (i = 0; ; i++)
		{
			if (linedata[i] == '\r' || linedata[i] == '\n')
				linedata[i] = 0;
			if (linedata[i] == 0)
				break;
		}
		
		if (strstr(linedata, "Name=\"Skyworth_RC"))
		{
			found_bt = true;
		}
		
		if (found_bt)
		{
			if (strstr(linedata, "Handlers="))
			{
				char * start;
				start = strstr(linedata, "event");
				if (start)
				{
					char * p = strchr(start, ' ');
					if (p)
						*p = 0;
					static char inputname[64];
					strcpy(inputname, "/dev/input/");
					strcat(inputname, start);
					*DEVICE_PATH = inputname;
					return ;
				}
			}
		}
		
		memset(linedata, 0, sizeof(linedata));
		fgets(linedata, sizeof(linedata) - 1, fp);
	}
	
	
	fclose(fp);
	*DEVICE_PATH = (char*)("/dev/input/event0");
	
	return;
}





