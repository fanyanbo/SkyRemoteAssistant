#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <list>
#include "VncThreads.h"
#include "VncUtils.h"

VncThreads::VncThreads()
{
    m_freeze = false;
    pthread_mutex_init(&m_mutex, nullptr);
}

VncThreads::~VncThreads()
{
    unsigned threadcount;
    stopAll();
    
    while (1)
    {
        pthread_mutex_lock(&m_mutex);
        threadcount = m_pthreads.size();
        pthread_mutex_unlock(&m_mutex);

        if (threadcount == 0)
            break;

        VNCLOGD("%s() wait all threads end.", __FUNCTION__);
        usleep(500 * 1000);
    }

    pthread_mutex_destroy(&m_mutex);
}

int VncThreads::addThread(pthread_t *ptid, const pthread_attr_t *attr, void*(*proc)(void*), void *arg, void(*notify_exit)())
{
    int ret;

    if (m_freeze)
        return -1;

    pthread_mutex_lock(&m_mutex);

    ThreadInfo info;
    ret = pthread_create(ptid, attr, proc, arg);
    if (ret == 0)
    {
        info.thread_id = *ptid;
        info.notify_exit = notify_exit;
        m_pthreads.push_back(info);
    }

    pthread_mutex_unlock(&m_mutex);
    return ret;
}

int VncThreads::exitThread(pthread_t pid, void* prval)
{
    bool found;
    ThreadInfo info;
    pthread_mutex_lock(&m_mutex);

    found = false;
    std::list <ThreadInfo>::iterator it;
    for (it = m_pthreads.begin(); it != m_pthreads.end(); it++)
    {
        if (it->thread_id == pid)
        {
            found = true;
            info = *it;
            m_pthreads.erase(it);
            break;
        }
    }

    pthread_mutex_unlock(&m_mutex);

    if (found)
    {
        pthread_exit(prval);
    }

    return 0;
}

void VncThreads::stopAll()
{
    int status;

    pthread_mutex_lock(&m_mutex);
    std::list <ThreadInfo>::iterator it;
    for (it = m_pthreads.begin(); it != m_pthreads.end(); it++)
    {
        if (it->notify_exit != nullptr)
        {
            it->notify_exit();
        }
    }
    m_freeze = true;
    pthread_mutex_unlock(&m_mutex);
}


