#ifndef TASK_WRAP_H
#define TASK_WRAP_H

// Hang Up Task Map
// TaskWrap 是对任务涉及的客户与信号机之间(UDP)的基本信息
// 由于可能会出现重复的任务，因而使用了std::multimap
// 若不同客户端向同一台信号机发送相同指令，则会导致key重复
// 由于无法鉴别信号机的响应是对于哪一次请求
// 因而对于每一个响应，将会在HubTaskMap中取出一个Task，并回复客户端
#include "conn_info.h"
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
		
		int AddTask(TaskWrapPtr task);
		/* { */
		/* 	std::lock_guard<std::mutex> lk(mtx_); */

		/* 	char tmpstr[30]{'\0'}; */
		/* 	snprintf(tmpstr, 29,"%s-%u",task->machine_conn->get_Ipv4(),task->instype); */ 
		/* 	map_.insert({tmpstr, task}); */
		/* 	return 0; */
		/* } */
		// key is "${machine_ip}-${instype}"
		int DropTask(const char* key);
		/* { */
		/* 	std::lock_guard<std::mutex> lk(mtx_); */
		/* 	map_.erase(key); */
		/* 	return 0; */
		/* } */
		// key is "${machine_ip}-${instype}"
		TaskWrapPtr FindTask(const char* key);
		/* { */
		/* 	std::lock_guard<std::mutex> lk(mtx_); */
		/* 	auto res = map_.find(key); */
		/* 	return res->second; */
		/* } */

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
