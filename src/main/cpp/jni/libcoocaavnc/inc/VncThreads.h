#include <pthread.h>
#include <list>
#ifndef COOCAA_OS_VNC_THREADS_H_
#define COOCAA_OS_VNC_THREADS_H_

struct ThreadInfo
{
    pthread_t           thread_id;
    void                (*notify_exit)();
};

class VncThreads
{
public:
    VncThreads();
    ~VncThreads();

    int addThread(pthread_t *ptid, const pthread_attr_t *attr, void*(*proc)(void*), void *arg, void(*notify_exit)());
    int exitThread(pthread_t pid, void* prval);
    void stopAll();
private:
    bool                        m_freeze;                   // 是否冻结线程组,冻结后不能再添加新线程
    std::list <ThreadInfo>      m_pthreads;
    pthread_mutex_t             m_mutex;
};

#endif 
