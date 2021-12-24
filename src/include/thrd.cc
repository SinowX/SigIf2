#include "thrd.h"
#include "handler.h"
#include "conn_q.h"
#include "instruction_manager.h"
#include "log_mgr.h"

void Leader()
{

	std::thread thrd_acpt,thrd_iorcvr[10],
		thrd_wokr[10], thrd_hbkpr[10], thrd_log;
	thrd_acpt = std::thread(Acceptor);
	/* thrd_iorcvr = std::thread(IOReceiver); */
	
	int iorcvr_amount = ConfMgr::getInstance().get_io_rcvr_amount();
	iorcvr_amount = iorcvr_amount>10 ? 10 : iorcvr_amount;
	for(int i=0;i<iorcvr_amount ;i++)
	{
		thrd_iorcvr[i] = std::thread(IOReceiver,i);
	}

	int worker_amount = ConfMgr::getInstance().get_worker_amount();
	worker_amount = worker_amount>10 ? 10 : worker_amount;
	for(int i=0;i<worker_amount ;i++)
	{
		thrd_wokr[i] = std::thread(Worker,i);
	}
	
	const auto& info = ConfMgr::getInstance().get_machine();

	/* const Machine& info = ConfMgr::getInstance().get_machine(); */
	if(info.XINGZHI.number>0)thrd_hbkpr[0]=std::thread(HBKeeper, MachineType::XINGZHI);
	if(info.HAIXIN.number>0) thrd_hbkpr[1]=std::thread(HBKeeper, MachineType::HAIXIN);
	if(info.HAIKANG.number>0)thrd_hbkpr[2]=std::thread(HBKeeper, MachineType::HAIKANG);
	if(info.XIMENZI.number>0)thrd_hbkpr[3]=std::thread(HBKeeper, MachineType::XIMENZI);
	if(info.DEVICE5.number>0)thrd_hbkpr[4]=std::thread(HBKeeper, MachineType::DEVICE5);

	thrd_log = std::thread(Logger);

	while(true)
	{
		pause();
	}
}

void Acceptor()
{
	ConfMgr& confmgr = ConfMgr::getInstance();

	/* ConnInfoPtr server = std::make_shared<ConnInfo>(ConnType::NORMAL_CONN,confmgr.get_listen_addr(), */
	/* 		confmgr.get_listen_port()); */
	ConnInfoPtr server = std::make_shared<ConnInfo>(ConnType::TCP, false, -1,
			confmgr.get_listen_addr(), confmgr.get_listen_port());
	server->get_Tcp_Conn()->Listen();

	ConnMap::getInstance().Insert(server->get_Ipv4(),server);
	
	EvDemul demultiplex(server->get_Tcp_Conn()->get_Fd());
	demultiplex.RegisterCallback(
			accept_fd_hdlr, error_fd_hdlr);

	demultiplex.AddEvent(server->get_Tcp_Conn()->get_Fd());
	/* demultiplex.AddListen(server->get_tcp_conn()->get_fd()); */

	while(true)
	{
		demultiplex.WaitEvents();
	}
	
}

void IOReceiver()
{
	ConfMgr& confmgr = ConfMgr::getInstance();

	EvDemul demultiplex;
	demultiplex.RegisterCallback(
			accept_fd_hdlr, error_fd_hdlr, read_all_hdlr);

	while(true)
	{
		ConnInfoPtr newconn = NewConnQ().TryPop();
		if(newconn!=nullptr)
		{
			demultiplex.AddEvent(newconn->get_Fd());
		}
		demultiplex.WaitEvents();
	}
}

void Worker(int worker_id)
{
	/* ConfMgr& confmgr = ConfMgr::getInstance(); */
	while(true)
	{
		ConnInfoPtr newtask = TaskQ().Pop();

		switch(newtask->get_ConnType())
		{
			case ConnType::UDP:
			{
				machine_msg_hdlr(newtask);
				break;
			}
			case ConnType::TCP:
			{
				const char* read_buff=nullptr;
				while(!(read_buff=newtask->get_Tcp_Conn()
							->get_r_Buffer()->get_One_Json()))
				{
					client_query_hdlr(newtask, read_buff);
					delete[] read_buff;
				}
			}
		}
	}
}

void HBKeeper(int machine_type)
{
	ConfMgr& confmgr = ConfMgr::getInstance();
	std::list<ConnInfoPtr> conn_list;

	switch(machine_type)
	{
		case MachineType::XINGZHI:
			{
				MachineInfo *machine_info = confmgr.get_machine().XINGZHI.head;
				while(machine_info!=nullptr)
				{
					ConnInfoPtr conn = std::make_shared<ConnInfo>(
							ConnType::UDP, false, -1 ,machine_info->ipv4,
							machine_info->port);
					conn_list.push_back(conn);
				}
				
				while(true)
				{
					for(auto it=conn_list.begin();it!=conn_list.end();it++)
					{
						SendHB_XingZhi(*it);
					}
					sleep(3);
				}
				break;
			}
		case MachineType::XIMENZI:
			{
				break;
			}
		case MachineType::HAIXIN:
			{
				break;
			}
		case MachineType::HAIKANG:
			{
				break;
			}
		case MachineType::DEVICE5:
			{
				break;
			}
		default:
			{
				// unknown MachineType
				break;
			}
	}
}
