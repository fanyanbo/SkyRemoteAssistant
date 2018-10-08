#include <pthread.h>
#include <queue>
#include "VncCommandQueue.h"
#include "VncDataPackage.h"
#include "VncUtils.h"

VncCommandQueue::VncCommandQueue()
{
    pthread_mutex_init(&m_mutex, nullptr);
    pthread_cond_init(&m_condvar, nullptr);
}

VncCommandQueue::~VncCommandQueue()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_condvar);
}

int VncCommandQueue::addPackage(VncDataPackage * pContent)
{
    pthread_mutex_lock(&m_mutex);
    m_data.push(pContent);
    pthread_mutex_unlock(&m_mutex);
    pthread_cond_signal(&m_condvar);
    return 0;
}

int VncCommandQueue::getPackage(VncDataPackage ** ppContent)
{
    VncDataPackage * package;
    struct timespec waittime;
	struct timeval now;

	bool empty;
	int size;
    
    package = nullptr;
    pthread_mutex_lock(&m_mutex);
    if (!(empty = m_data.empty()))
    {
        package = m_data.front();
        m_data.pop();
    }
	size = m_data.size();
    pthread_mutex_unlock(&m_mutex);

//	VNCLOGD("empty = %d, size = %d", empty, size);

    if (package != nullptr)
    {
        *ppContent = package;
        return 0;
    }

	gettimeofday(&now,NULL);
    waittime.tv_sec = now.tv_sec;
    waittime.tv_nsec = now.tv_usec * 1000 + 200 * 1000 * 1000;

    pthread_mutex_lock(&m_mutex);
    pthread_cond_timedwait(&m_condvar, &m_mutex, &waittime);
    pthread_mutex_unlock(&m_mutex);

    return -1;
}




