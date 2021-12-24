#ifndef UDP_WRAP_H
#define UDP_WRAP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <cstring>
/* #include "type_safe.h" */
#include "log_mgr.h"

class UdpWrap
{
public:
	UdpWrap(const char* ipv4_addr, const uint16_t port)
		:port_(port),
		pack_q_(std::make_unique<std::queue<std::string>>())
	{
		strncpy(ipv4_addr_, ipv4_addr, 16);	
			
		fd_ = InitUdpSocket();
		addr_ = InitSocketAddr(port, ipv4_addr);
	}

	UdpWrap(UdpWrap&)=delete;
	void operator=(UdpWrap&)=delete;
	~UdpWrap()=default;

	int ReceiveAll()
	{

		static char read_buff[1000];
		int n=-1;
		while((n=recvfrom(fd_, read_buff, 1000, 0, NULL, 0)>0))
		{
			pack_q_->push(std::string(read_buff,n));
		}
		return 0;

	}
	int Send(const char* data, const ssize_t len)
	{
		if(len!=sendto(fd_,data, len, 0, (sockaddr*)&addr_, sizeof(addr_)))
		{
			if(len<0)
			{
				LOGWARN("send failed");
				return -1;
			}else{
				LOGWARN("send packet unfinished");
				return -1;
			}
		}else{
			return len;
		}
	}
	const char* get_Ipv4() const {return ipv4_addr_;}
	const uint16_t get_Port() const {return port_;}
	int get_Fd() const {return fd_;}
	
	// return nullptr if no pack
	const std::shared_ptr<std::string> try_Get_Pack()
	{
		if(pack_q_->empty())
		{
			LOGINFO("try to get from empty pack queue");
			return nullptr;
		}
		/* char* pack_buff = new char[pack_q_->front().size()+1]; */
		std::shared_ptr<std::string> pack_ptr = std::make_shared<std::string>(pack_q_->front());
		/* std::string pack_buff = pack_q_->front(); */
		/* strcpy(pack_buff, pack_q_->front().c_str()); */
		pack_q_->pop();
		
		return pack_ptr;
	}

private:
	int InitUdpSocket(uint16_t port=0,const char* ipaddr=nullptr)
	{
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if(sockfd<0)
		{
			LOGWARN("init socket fd failed");
			Reset();
			return -1;
		}
		struct sockaddr_in addr = InitSocketAddr(port, ipaddr);

		if(bind(sockfd, (struct sockaddr*)&addr,sizeof(addr))==-1)
		{
			LOGWARN("bind failed");
			Reset();
			return -1;
		}	

		return sockfd;
	}

	struct sockaddr_in InitSocketAddr(uint16_t port=0,const char* ipaddr=nullptr)
	{
		struct sockaddr_in addr;
		/* socklen_t addr_len = sizeof(addr); */
		memset(&addr, 0, sizeof(struct sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_port=htons(port);
		if(ipaddr!=nullptr)
			addr.sin_addr.s_addr = inet_addr(ipaddr);
		else
			/* addr.sin_addr.s_addr = INADDR_ANY; */
			addr.sin_addr.s_addr = inet_addr("0.0.0.0");
		return addr;
	}	

  int Reset()
  {
    if(fd_<0)
    {
      LOGWARN("try to reset unset conn");
      return -1;
    }
    close(fd_);
    fd_=-1;
    return 0;
  }
	
	int fd_{-1};
	/* StringShr ipv4_addr_{nullptr}; */
	char ipv4_addr_[16]{0};
	uint16_t port_{0};
	struct sockaddr_in addr_;
	std::unique_ptr<std::queue<std::string>> pack_q_;	
	std::mutex mtx_;
};

#endif
