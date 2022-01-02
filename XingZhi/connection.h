#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <cstdint>
#include <cstring>
#include <string>
/* #include <vector> */
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>
#include <memory>
#include "instruction_manager.h"
/* using std::string; */

class noncopyable
{
	public:
	noncopyable(const noncopyable&)=delete;
	void operator=(const noncopyable&)=delete;

	protected:
	noncopyable() = default;
	~noncopyable() = default;

};



class PackQ: noncopyable
{
	public:
		std::string Get()
		{
			std::lock_guard<std::mutex> lk(mtx_);
			std::string res = pack_q_.front();
			pack_q_.pop();
			return res;
		}
		// return pack_q size
		int Push(std::string& pack)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			pack_q_.push(pack);
			return pack_q_.size();
		}

		int Size() const {return pack_q_.size();}

	private:
		std::queue<std::string> pack_q_;
		std::mutex mtx_;
};

class ConnInfo: noncopyable
{
	public:
		ConnInfo(std::string ipv4_addr, const uint16_t port)
			: port_(port), ipv4_addr_(ipv4_addr)
		{}
		
		// wait for the 1st pack for wait seconds
		// return success received pack number
		int ReceiveAll(int wait=0);

		int Send(std::string);

		std::queue<std::string>& GetPackQ()
		{
			return recv_pack_q_;
		}

		const std::string& GetIpv4() const {return ipv4_addr_;}
		uint16_t GetPort() const {return port_;}

	private:
		std::queue<std::string> recv_pack_q_;
		std::string ipv4_addr_;
		uint16_t port_{0};
};

using ConnInfoPtr = std::shared_ptr<ConnInfo>;

class ConnMap :noncopyable
{
	public:
		static ConnMap& getInstance()
		{
			static ConnMap instance_;
			return instance_;
		}
		ConnInfoPtr Get(std::string key)
		{
			return conn_map_.find(key)->second;
		}
		int Add(ConnInfoPtr& ptr)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			conn_map_.insert({ptr->GetIpv4(), ptr});
			return conn_map_.size();
		}

		int Drop(std::string key)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			conn_map_.erase(key);
			return conn_map_.size();
		}
	private:
		ConnMap()=default;
		// ipv4addr-type (name or id) be the key
		std::map<std::string, ConnInfoPtr> conn_map_;
		std::mutex mtx_;
};


class Task: noncopyable
{
	public:
		Task(std::string ip, std::string type, std::time_t time,
				std::shared_ptr<std::mutex> mt,
				std::shared_ptr<std::condition_variable> cond)
			: ipv4_addr(ip), task_type(type), timestamp(time),
				mtx(mt), condi(cond), parser{nullptr} {}
		std::string ipv4_addr;
		std::string task_type; // see API Wiki
		std::time_t timestamp; // upload timestamp, to check if outdated(5s)
		std::shared_ptr<std::mutex> mtx;
		std::shared_ptr<std::condition_variable> condi;
		std::shared_ptr<InsParser> parser;
};

using TaskPtr = std::shared_ptr<Task>;



class TaskMap: noncopyable
{
	public:
		static TaskMap& getInstance()
		{
			static TaskMap instance_;
			return instance_;
		}
		void Add(TaskPtr ptr)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			task_map_.insert({ptr->ipv4_addr,ptr});
		}
		

	private:
		TaskMap()=default;
		std::multimap<std::string, TaskPtr> task_map_;
		std::mutex mtx_;
	public:
		using ItBool = struct{
			decltype(task_map_)::iterator it;
			bool valid;
		};
		
		ItBool Get(std::string key)
		{
			/* std::lock_guard<std::mutex> lk(mtx_); */
			mtx_.lock();
			auto it = task_map_.find(key);
			mtx_.unlock();

			if(it!=task_map_.end())
				return ItBool{it,true};
			else return ItBool{it,false};
		}
		void Drop(decltype(task_map_)::iterator it)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			task_map_.erase(it);
		}
};

#endif
