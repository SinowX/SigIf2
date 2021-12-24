#ifndef CONN_INFO_H
#define CONN_INFO_H

#include "tcp_wrap.h"
#include "udp_wrap.h"
#include "snmp_wrap.h"
/* #include */ 

enum ConnType{
	TCP,
	UDP,
	SNMP
};

class ConnInfo
{
public:
	ConnInfo(const int conntype, const bool isConnected,const int fd=-1,
		 const char* ipv4_addr=nullptr, const uint16_t port=0);
	
	ConnInfo(ConnInfo&)=delete;
	void operator=(ConnInfo&)=delete;
	~ConnInfo()=default;

	bool is_Connected() const {return isConnected_;}
	int get_ConnType() const {return conntype_;}
	std::unique_ptr<TcpWrap>& get_Tcp_Conn(){return tcp_conn_;}
	std::unique_ptr<UdpWrap>& get_Udp_Conn(){return udp_conn_;}
	const char* get_Ipv4();
	const uint16_t get_Port();
	int get_Fd();
	void UpdateHeartBeat()
	{
		last_heartbeat_ = std::time(nullptr);
	}

private:
	/* bool isTCP_{false}; */
	int conntype_{-1};
	// used for tcp
	bool isConnected_{false};

	// used for udp and snmp
	std::time_t last_heartbeat_{0};

	int fd_{-1};
	std::unique_ptr<TcpWrap> tcp_conn_{nullptr};
	std::unique_ptr<UdpWrap> udp_conn_{nullptr};
	std::unique_ptr<SnmpWrap> snmp_conn_{nullptr};
};

using ConnInfoPtr = std::shared_ptr<ConnInfo>;


#endif
