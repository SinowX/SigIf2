#include "conn_info.h"

ConnInfo::ConnInfo(const int conntype, const bool isConnected,
		const int fd, const char* ipv4_addr, const uint16_t port)
	:conntype_(conntype)
{
	switch (conntype_)
	{
		case ConnType::TCP:
			{
				tcp_conn_ = std::make_unique<TcpWrap>(ipv4_addr, port, fd);
				isConnected_ = isConnected;
				break;
			}
		case ConnType::UDP:
			{
				udp_conn_ = std::make_unique<UdpWrap>(ipv4_addr, port);
				isConnected_ =false;
				break;
			}
		case ConnType::SNMP:
			{
				snmp_conn_ = std::make_unique<SnmpWrap>(ipv4_addr, port);
				isConnected_ =false;
				break;
			}
		default:
			{ LOGWARN("conninfo constructor failed"); }
	}
}

const char* ConnInfo::get_Ipv4()
{
	switch (conntype_)
	{
		case ConnType::TCP:
			{
				return tcp_conn_->get_Ipv4();
			}
		case ConnType::UDP:
			{
				return udp_conn_->get_Ipv4();
			}
		case ConnType::SNMP:
			{
				return snmp_conn_->get_Ipv4();
				break;
			}
		default:
			{ LOGWARN("conninfo broken");
				return nullptr; 
			}
	}

}

const uint16_t ConnInfo::get_Port()
{
	switch (conntype_)
	{
		case ConnType::TCP:
			{
				return tcp_conn_->get_Port();
			}
		case ConnType::UDP:
			{
				return udp_conn_->get_Port();
			}
		case ConnType::SNMP:
			{
				return snmp_conn_->get_Port();
				break;
			}
		default:
			{ LOGWARN("conninfo broken");
				return 0; 
			}
	}

}

int ConnInfo::get_Fd()
{
	switch (conntype_)
	{
		case ConnType::TCP:
			{
				return tcp_conn_->get_Fd();
			}
		case ConnType::UDP:
			{
				return udp_conn_->get_Fd();
			}
		case ConnType::SNMP:
			{
				return snmp_conn_->get_Fd();
				break;
			}
		default:
			{ LOGWARN("conninfo broken");
				return 0; 
			}
	}

}
