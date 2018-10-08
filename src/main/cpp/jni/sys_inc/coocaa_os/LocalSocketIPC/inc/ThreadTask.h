#ifndef __THREAD_TASK_H
#define __THREAD_TASK_H
#include "CoocaaDef.h"
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
//#include <iostream>

NS_COOCAA_BEGIN


class ThreadPool;
class ThreadTask:public std::enable_shared_from_this<ThreadTask>
{
public:
	ThreadTask(std::shared_ptr<ThreadPool> threadpool):exit(false){
		_tasks = threadpool;
	}

	~ThreadTask(){
		//std::cout<<"dealloc ThreadTask" <<std::endl;
	}

	void start();
	void stop(){
		exit = true;
		_sync.notify_one();
	}

	void setJob(const std::function<void(void*)> & job, void* ptr,const std::string & file, int line)
	{
		{  
            std::unique_lock<std::mutex> lock(_mutex);  
            _job = job;  
            _file = file;  
            _line = line;  
			_ptr = ptr;
        }  
        _sync.notify_one();//notify
	}

	void print_job(){
		//std::cout<<"sumbit from:"<<_file<<":"<<_line<<std::endl;
	}

private:
	bool exit;

	ThreadTask(const ThreadTask& src);
	ThreadTask & operator =( const ThreadTask &src);

	std::mutex _mutex;
	std::condition_variable _sync;
	std::function<void(void*)> _job;
	std::thread::id _tid;

	std::string _file;
	int _line;
	void* _ptr;

	std::weak_ptr<ThreadPool> _tasks;

};

NS_COOCAA_END

#endif//__THREAD_TASK_H
