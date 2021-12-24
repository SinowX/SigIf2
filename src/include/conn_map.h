// Connection Map
// 提供由fd或ipv4_addr 查询对应Connection Info功能
#ifndef CONN_MAP_H
#define CONN_MAP_H

#include "conn_info.h"
#include <map>
#include <memory>


class ConnMap
{
	public:
		static ConnMap& getInstance()
		{
			static ConnMap instance_;
			return instance_;
		}
		~ConnMap()=default;
		ConnMap(ConnMap&)=delete;
		void operator=(ConnMap&)=delete;

		int Insert(const char* ipv4_addr, ConnInfoPtr conn)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			map_ipv4_.insert({ipv4_addr, conn});
			return 0;
		}

		int Drop(const char* ipv4_addr)
		{
			if(ipv4_addr==nullptr)
			{
				LOGWARN("invalid ipv4_addr(nullptr)");
				return -1;
			}
			std::lock_guard<std::mutex> lk(mtx_);
			if(map_ipv4_.erase(ipv4_addr)==0)
			{
				LOGWARN("not found such ipv4_addr in Conn_Map");
				return -1;
			}else{
				return 0;
			}
		}
		
		int Drop(const int fd)
		{
			return Drop(Fd2Ipv4(fd));
		}
		
		const ConnInfoPtr Find(const char* ipv4_addr)
		{
			if(ipv4_addr==nullptr)
			{
				return nullptr;
			}

			auto it = map_ipv4_.find(ipv4_addr);
			if(it==map_ipv4_.end())
				return nullptr;
			else
				return it->second;
		}

		const ConnInfoPtr Find(const int fd)
		{
			return Find(Fd2Ipv4(fd));
		}

	private:
		const char* Fd2Ipv4(int fd)
		{
			struct sockaddr_in addr;
			socklen_t addr_len = sizeof addr;
			if(getpeername(fd, (struct sockaddr*)&addr,&addr_len)<0)
			{
				LOGWARN("try to use fake socket fd");
				return nullptr;
			}
			char* ipv4_addr = new char[16];
			strncpy(ipv4_addr,inet_ntoa(addr.sin_addr),16);
			return ipv4_addr;
		}
		class cmp_str
		{
			bool operator()(const char*a, const char*b) const
			{ return std::strcmp(a,b)<0;}
		};
		ConnMap()=default;
		std::map<const char*, ConnInfoPtr, cmp_str> map_ipv4_;
		std::mutex mtx_;
};

#endif
