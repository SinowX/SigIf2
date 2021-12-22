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

		int Insert(const int fd, ConnInfoPtr conn)
		{
			map_.insert({fd, conn});
			map_ipv4_.insert({conn->get_ipv4(),conn});
			return 0;
		}	

		int Drop(const int fd)
		{
			map_ipv4_.erase(map_.find(fd)->second->get_ipv4());
			map_.erase(fd);
			return 0;
		}

		const ConnInfoPtr Find(const int fd)
		{
			auto it = map_.find(fd);
			if(it==map_.end())
				return nullptr;
			else
				return it->second;
		}
		
		const ConnInfoPtr Find(const char* ipv4_addr)
		{
			auto it = map_ipv4_.find(ipv4_addr);
			if(it==map_ipv4_.end())
				return nullptr;
			else
				return it->second;
		}



	private:
		class cmp_str
		{
			bool operator()(const char*a, const char*b) const
			{ return std::strcmp(a,b)<0;}
		};
		ConnMap()=default;
		std::map<int, ConnInfoPtr> map_;
		std::map<const char*, ConnInfoPtr, cmp_str> map_ipv4_;
};

#endif
