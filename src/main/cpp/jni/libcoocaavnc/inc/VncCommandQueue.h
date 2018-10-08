#include <pthread.h>
#include <queue>
#ifndef COOCAA_OS_VNC_COMMAND_LIST_H_
#define COOCAA_OS_VNC_COMMAND_LIST_H_

class VncDataPackage;

class VncCommandQueue
{
public:
    VncCommandQueue();
    ~VncCommandQueue();

    int addPackage(VncDataPackage * pContent);
    int getPackage(VncDataPackage ** ppContent);

private:
    std::queue<VncDataPackage*> m_data;
    pthread_mutex_t             m_mutex;
    pthread_cond_t              m_condvar;
};




#endif // COOCAA_OS_VNC_COMMAND_LIST_H_

