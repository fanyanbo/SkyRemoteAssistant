#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H
#include "CoocaaDef.h"
#include <list>
#include <set>
#include <queue>

#include "ThreadTask.h"

NS_COOCAA_BEGIN

class ThreadPool:public std::enable_shared_from_this<ThreadPool>
{
public:
	ThreadPool(unsigned long pool_size = 10):_maxsize(pool_size>0?pool_size:10),_stopall(true){
	}
	virtual ~ThreadPool()
	{
		//std::cout<<"dealloc threadpool" <<std::endl;
	}

	void job_completed( const std::shared_ptr<ThreadTask> & t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		bool need_to_notify = _idle_tasks.empty()&&(!_wait_for_running_job.empty());
		_busying_tasks.erase(t);
		_idle_tasks.push_back(t);
		//std::cout<<"after job_completed, current idle tasks size:"<< _idle_tasks.size() <<" busying tasks size:"<<_busying_tasks.size()<<" wait for running jobs size:"<<_wait_for_running_job.size() <<std::endl;
		if( !_busying_tasks.empty()){
			(*_busying_tasks.begin())->print_job();
		}
		if(need_to_notify)
		{
			_sync.notify_one();
		}
	}

	void submit_job( const std::function<void(void*)> & job, void* ptr, const std::string file, int line)
	{
		if(_stopall)
		{
			return;
		}
		std::lock_guard<std::mutex> lock(_mutex);
		bool need_notify =  _wait_for_running_job.empty();
		_wait_for_running_job.push(std::make_tuple(job,ptr,file,line));

		if( need_notify )//when no running jpb
		{
			_sync.notify_one();
		}
	}

	void execute_job()
	{
		while(true)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			while(!_stopall && _wait_for_running_job.empty())
			{
				//wait for submit job
				_sync.wait(lock);
			}
			if(_stopall)
			{
				return;
			}

			while(!_stopall && _idle_tasks.empty())
			{
				//wait for idle
				_sync.wait(lock);
			}

			if(_stopall)
			{
				return;
			}

			auto t = get_task();
			auto job =_wait_for_running_job.front();
			_wait_for_running_job.pop();
			//set job to t thread
			t->setJob(std::get<0>(job), std::get<1>(job), std::get<2>(job),std::get<3>(job));
		}
	}


	void start()
	{
		_stopall = false;
		try{
			std::thread t(
				[this]{execute_job();}
			);
			t.detach();

			allocate_tasks();

		}catch(std::exception& e)
		{
			//std::cout<<"start task pool ... error" << e.what() <<std::endl;
		}
	}

	void stopAll()
	{

		std::lock_guard<std::mutex> lock(_mutex);
		_stopall = true;

		for(auto t : _idle_tasks)
		{
			t->stop();
		}
		_idle_tasks.clear();
		for( auto t : _busying_tasks )
		{
			t->stop();
		}

		while (!_wait_for_running_job.empty())
		{
			_wait_for_running_job.pop();
		}

		_sync.notify_one();
	}

protected:
	std::shared_ptr<ThreadTask> get_task()
	{
		//get task ptr
		if( ! _idle_tasks.empty() )
		{
			auto t = *_idle_tasks.begin();
			_idle_tasks.pop_front();  //remove from idle
			_busying_tasks.insert(t); //insert to busy

			return t;
		}

		return std::shared_ptr<ThreadTask>();

	}

	void allocate_tasks()
	{
		for( int i = 0 ; i < _maxsize; i ++ )
		{
			std::shared_ptr<ThreadTask> t( new ThreadTask(shared_from_this()));
			try{
				t->start();
				_idle_tasks.push_back(t);
			}catch( std::exception & e) 
			{	//exception when _maxsize > system max thread	
				//std::cout<<"alloc task error" <<e.what()<<std::endl;
				break;
			}
		}
	}

private:
	unsigned int _maxsize;
	bool _stopall;

	std::list <std::shared_ptr<ThreadTask>> _idle_tasks;
	std::set <std::shared_ptr<ThreadTask>> _busying_tasks;
	std::queue <std::tuple< std::function<void(void*)> ,void*, std::string, int >> _wait_for_running_job;
	std::mutex _mutex;
	std::condition_variable _sync;

};

NS_COOCAA_END
#endif//__THREAD_POOL_H