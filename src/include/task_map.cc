#include "task_map.h"


int HupTaskMap::AddTask(TaskWrapPtr task)
{
	std::lock_guard<std::mutex> lk(mtx_);

	char tmpstr[30]{'\0'};
	snprintf(tmpstr, 29,"%s-%u",task->machine_conn->get_Ipv4(),task->instype); 
	map_.insert({tmpstr, task});
	return 0;
}

int HupTaskMap::DropTask(const char *key)
{
	std::lock_guard<std::mutex> lk(mtx_);
	map_.erase(key);
	return 0;
}

TaskWrapPtr HupTaskMap::FindTask(const char *key)
{
	std::lock_guard<std::mutex> lk(mtx_);
	auto res = map_.find(key);
	return res->second;
}

