#ifndef CONN_INFO_H
#define CONN_INFO_H

#include <string>
#include <memory>
#include <cstring>
#include <ctime>

#include "tcp_wrap.h"
#include "udp_wrap.h"
enum ConnStatus
{
	CONNECTED=1,
	DISCONNECTED
};

enum ConnType
{
	NORMAL_CONN=1,
	SVR_LISTEN,
	MACHINE
};


class ConnInfo
{
	public:
		// ipv4_addr has to be ensured valid, else it will be set zero
		ConnInfo(const int type, const char* ipv4_addr,
				const uint16_t port, const int fd=-1,
			 	const int status = ConnStatus::DISCONNECTED)
			:type_(type), status_(status)
		{
			
			if(type==ConnType::MACHINE)
			{
				udp_conn_ = std::unique_ptr<UdpWrap>(
						new UdpWrap(ipv4_addr,port,fd));
			}else{
				tcp_conn_ = std::unique_ptr<TcpWrap>(
						new TcpWrap(ipv4_addr,port,fd));
			}
			
			/* tcp_conn_ = new TcpWrap(ipv4_addr, port); */
		}
		ConnInfo(ConnInfo&)=delete;
		void operator=(ConnInfo&)=delete;
		
		~ConnInfo()=default;

		const int get_type() const {return type_;}
		const int get_status() const {return status_;}
		const char* get_ipv4()
		{
			if(get_type()==ConnType::MACHINE)
				return get_udp_conn()->get_ipaddr();
			else
				return get_tcp_conn()->get_ipaddr();
		}	
		std::unique_ptr<TcpWrap>& get_tcp_conn(){return tcp_conn_;}
		std::unique_ptr<UdpWrap>& get_udp_conn(){return udp_conn_;}	
	private:
		int type_;
		int status_;
		// for heart beat
		std::time_t last_heartbeat{0};
		
		/* TcpWrap* tcp_conn_{nullptr}; */
		/* std::unique_ptr<TcpWrap> tcp_conn_{nullptr}; */
		std::unique_ptr<TcpWrap> tcp_conn_{nullptr};
		std::unique_ptr<UdpWrap> udp_conn_{nullptr};

		
};

using ConnInfoPtr = std::shared_ptr<ConnInfo>;

#endif
