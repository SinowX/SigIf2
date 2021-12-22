// UDP Wrap
// UDP连接封装
// 提供UDP连接相关操作
#ifndef UDP_WRAP
#define UDP_WRAP

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <memory>
#include <queue>

class UdpWrap
{
	public:
		UdpWrap(const char* ipv4_addr, const uint16_t port,
			const int fd=-1)
		{};
		UdpWrap(UdpWrap&)=delete;
		void operator=(UdpWrap)=delete;
		~UdpWrap(){};

		int Send(const char* buffer, ssize_t len)
		{}

		int Receive(char* buffer, ssize_t len)
		{}
		
		inline const char* get_ipaddr()
		{
			return ipv4_addr_;
		}
		
		inline const uint16_t get_port()
		{
			return port_;
		}

		int ReceiveAll();

		std::shared_ptr<std::string> GetOnePack();

	private:
		int fd_{-1};
		char ipv4_addr_[16]{0};
		uint16_t port_{0};
		std::queue<std::string> pack_q_;	
};

#endif
