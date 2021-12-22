#ifndef CONN_Q_H
#define CONN_Q_H

#include "conn_info.h"
#include <mutex>
#include <queue>
#include <condition_variable>


class Conn_Q
{
	public:
		Conn_Q()
		{}
		/* static Conn_Q& getInstance() */
		/* { */
		/* 	static Conn_Q instance_; */
		/* 	return instance_; */
		/* } */
		Conn_Q(Conn_Q&)=delete;
		void operator=(Conn_Q&)=delete;
	
		// return 0 for success, -1 for error	
		int Push(const ConnInfoPtr conn)
		{
			{
				std::lock_guard<std::mutex> lock(mtx_);
				conn_q_.push(conn);
			}
			condi_.notify_one();
			return 0;
		}

		ConnInfoPtr Pop()
		{
			std::unique_lock<std::mutex> lk(mtx_);
			if(conn_q_.empty())
				condi_.wait(lk);
			ConnInfoPtr ptr = conn_q_.front();
			conn_q_.pop();
			lk.unlock();
			return ptr;
		}
		
		ConnInfoPtr TryPop()
		{
			if(Size()==0) return nullptr;
			else return Pop();
		}

		int Size() const
		{
			return conn_q_.size();
		}

	private:

		std::queue<ConnInfoPtr, std::deque<ConnInfoPtr>> conn_q_;
		std::mutex mtx_;
		std::condition_variable condi_;
};

/* using NewConnQ = Conn_Q; */
static Conn_Q& NewConnQ()
{
	static Conn_Q conn_q_;
	return conn_q_;
}

/* using TaskQ = Conn_Q; */
static Conn_Q& TaskQ()
{
	static Conn_Q conn_q_;
	return conn_q_;
}

#include <map>

class TaskWrap
{
	public:
		TaskWrap(ConnInfoPtr machine, ConnInfoPtr client)
			:machine_conn(machine), client_conn(client)
		{}
		~TaskWrap()=default;
		uint8_t instype;
		ConnInfoPtr machine_conn;
		// if task is a query, use next var to write back result
		ConnInfoPtr client_conn;
};

using TaskWrapPtr = std::shared_ptr<TaskWrap>;
// need multi-map
class HupTaskMap
{
	public:
		static HupTaskMap& getInstance()
		{
			static HupTaskMap instance_;
			return instance_;
		}	
		HupTaskMap(HupTaskMap&)=delete;
		void operator=(HupTaskMap&)=delete;
		
		int AddTask(TaskWrapPtr task)
		{
			std::lock_guard<std::mutex> lk(mtx_);

			char tmpstr[30]{'\0'};
			snprintf(tmpstr, 29,"%s-%u",task->machine_conn->get_ipv4(),task->instype); 
			map_.insert({tmpstr, task});
			return 0;
		}
		// key is "${machine_ip}-${instype}"
		int DropTask(const char* key)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			map_.erase(key);
			return 0;
		}
		// key is "${machine_ip}-${instype}"
		TaskWrapPtr FindTask(const char* key)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			auto res = map_.find(key);
			return res->second;
		}

	private:
		class cmp_str
		{
			bool operator()(const char* a, const char* b) const
			{ return std::strcmp(a,b)<0; }
		};
		HupTaskMap(){};
		std::multimap<const char*, TaskWrapPtr, cmp_str> map_;
		/* std::map<const char*, TaskWrapPtr, cmp_str> map_; */
		std::mutex mtx_;
};


#endif
