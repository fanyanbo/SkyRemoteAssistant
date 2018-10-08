#include <pthread.h>
#ifndef COOCAA_VNC_CTRL_H_
#define COOCAA_VNC_CTRL_H_
#include <string>
#ifdef __cplusplus
extern "C" {
#endif

    class VncDataPackage;
	int startVnc(const char* pushId, std::string serverIp, bool boAccept);
    void stopVncSession();
    void setPCID(unsigned ipcid);
    void setServerID(unsigned iserverid);
    void sendPackage(VncDataPackage * package);
	void cleanup();

    int addThread(pthread_t *ptid, const pthread_attr_t *attr, void*(*start_rtn)(void*), void *arg, void(*notify_exit)());
    int exitThread(pthread_t pid, void* prval);
    void dump_package(VncDataPackage * package);
    void dump_data(unsigned char * data, int size);
    void keymain();
    int embVirtualKeyClick(unsigned code);
	void * cleanup_thread(void * args);

    extern unsigned pushid_crc32;
    extern unsigned serverid_crc32;
    extern unsigned pcid_crc32;
    extern char pushid_string[256];
    extern char serverid_string[256];
    extern char pcid_string[256];
	extern std::string ipaddr;

   // static const char * ipaddr = "172.20.115.104";
    //static const char * ipaddr = "192.168.1.199";
    static const unsigned short port = 9001;
    static const unsigned short fport = 9003;
    static const unsigned short logcat_port = 9004;

#ifdef __cplusplus
}
#endif

#endif  // COOCAA_VNC_CTRL_H_

