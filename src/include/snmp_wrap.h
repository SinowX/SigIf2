#ifndef SNMP_WRAP_H
#define SNMP_WRAP_H

#include <cstdint>
#include <string>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
/* #include "type_safe.h" */

class SnmpWrap
{
public:
	SnmpWrap(const char* ipv4_addr, const uint16_t port)
		: port_(port)
	{
		/* strncpy(ipv4_addr_, ipv4_addr, 16); */
		ipv4_addr_ = ipv4_addr;

		snmp_sess_init(&session);
		session.peername = new char[30];
		sprintf(session.peername, "%s:%u", ipv4_addr, port);
		session.version = SNMP_VERSION_1;
		session.community = reinterpret_cast<unsigned char*>(new char[]{"public"});
		session.community_len = 
			strlen(reinterpret_cast<char*>(session.community));

		SOCK_STARTUP;
		ss = snmp_open(&session);
		/* if(!ss){printf("Error\n")}; */
	}
	SnmpWrap(SnmpWrap&)=delete;
	void operator=(SnmpWrap&)=delete;
	~SnmpWrap()
	{
		if(response)
			snmp_free_pdu(response);
		snmp_close(ss);

		SOCK_CLEANUP;
	}
	
	const char* get_Ipv4() const {return ipv4_addr_;}
	const uint16_t get_Port() const {return port_;}

	// sync get response
	/* StringShr Get(const char* oid) */
	/* { */
	/* 	anOID_len=MAX_OID_LEN; */
	/* 	pdu = snmp_pdu_create(SNMP_MSG_GET); */
	/* 	snmp_parse_oid(oid->c_str(),anOID, &anOID_len); */
	/* 	snmp_add_null_var(pdu, anOID, anOID_len); */

	/* 	int status = snmp_synch_response(ss, pdu, &response); */

	/* 	if(status == STAT_SUCCESS */ 
	/* 			&& response->errstat == SNMP_ERR_NOERROR) */
	/* 	{ */

	/* 	} */
	/* } */
	/* // sync set response */
	/* StringShr Set(std::string data); */
	
private:
	StringShr ipv4_addr_{nullptr};
	uint16_t port_{0};
	netsnmp_session session, *ss;
	netsnmp_pdu *pdu;
	netsnmp_pdu *response;
	oid anOID[MAX_OID_LEN];
	size_t anOID_len{MAX_OID_LEN};

};

#endif
