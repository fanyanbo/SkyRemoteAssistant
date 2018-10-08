#ifndef __THREAD_POOL_USAGE_H
#define __THREAD_POOL_USAGE_H
#include "CoocaaDef.h"
#include <memory>
#include "ThreadPool.h"

NS_COOCAA_BEGIN

class ThreadPoolUsage
{
public:
	static void initThreadPool(unsigned int max_task_size)
	{
		if(!sp)
		{
			std::call_once(_init_flag,[max_task_size]{
				sp = std::make_shared<ThreadPool>(max_task_size);
				sp->start();
			});
		}
	}

	static void finishThreadPool()
	{
		if(sp)
		{
			std::call_once(_finish_flag,[]{sp->stopAll();});
		}
	}

	static void submitJob(const std::function<void(void*)> & job ,void* ptr, const std::string &  file, int line)
	{
		if(sp)
		{
			sp->submit_job(job,ptr,file,line);
		}
	}

private:
	static std::shared_ptr<ThreadPool> sp;
	static std::once_flag _init_flag;
	static std::once_flag _finish_flag;
};
NS_COOCAA_END


#endif //__THREAD_POOL_USAGE_H