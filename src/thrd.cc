#include "thrd.h"
#include "handler.h"
#include "conn_q.h"
/* #include <unistd.h> */
#include "instruction_manager.h"

void Leader()
{
	std::thread thrd_acpt,thrd_iorcvr[10],
		thrd_wokr[10], thrd_hbkpr[10];
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

	const Machine& info = ConfMgr::getInstance().get_machine();
	if(info.XINGZHI.number>0)thrd_hbkpr[0]=std::thread(HBKeeper, MachineType::XINGZHI);
	if(info.HAIXIN.number>0) thrd_hbkpr[1]=std::thread(HBKeeper, MachineType::HAIXIN);
	if(info.HAIKANG.number>0)thrd_hbkpr[2]=std::thread(HBKeeper, MachineType::HAIKANG);
	if(info.XIMENZI.number>0)thrd_hbkpr[3]=std::thread(HBKeeper, MachineType::XIMENZI);
	if(info.DEVICE5.number>0)thrd_hbkpr[4]=std::thread(HBKeeper, MachineType::DEVICE5);

	while(true)
	{
		pause();
	}
}

void Acceptor()
{
	ConfMgr& confmgr = ConfMgr::getInstance();

	ConnInfoPtr server = std::make_shared<ConnInfo>(ConnType::NORMAL_CONN,confmgr.get_listen_addr(),
			confmgr.get_port());
	server->get_tcp_conn()->Listen();
	
	ConnMap::getInstance().Insert(server->get_tcp_conn()->get_fd(),server);
	
	EvDemul demultiplex;
	demultiplex.RegisterCallback(
			accept_fd_hdlr, error_fd_hdlr);

	demultiplex.AddListen(server->get_tcp_conn()->get_fd());

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
			demultiplex.AddListen(newconn->get_tcp_conn()->get_fd());
		}
		demultiplex.WaitEvents();
	}
}





static std::shared_ptr<std::string>
GetOneJsonStr(BufferPtr buffer)
{

}

void Worker(int worker_id)
{
	/* ConfMgr& confmgr = ConfMgr::getInstance(); */
	while(true)
	{
		ConnInfoPtr newtask = TaskQ().Pop();
		
		if(newtask->get_type()==ConnType::MACHINE)
		{
			// from machine
			machine_msg_hdlr(newtask);
			/* auto pack = newtask->get_udp_conn()->GetOnePack(); */
			/* InsParser parser; */
			/* parser.Parse(reinterpret_cast<const unsigned char*> */
			/* 		(pack->c_str()),pack->size()); */
			
			// write back

		}else{
			// from client
			
			auto json_str = GetOneJsonStr(newtask->get_tcp_conn()->get_r_buffer());

			client_query_hdlr(newtask, json_str);
		}

		
	}
}

void HBKeeper(int machine_type)
{
	ConfMgr& confmgr = ConfMgr::getInstance();
	std::list<ConnInfoPtr> conn_list;
	MachineInfo *machine_info = confmgr.get_machine().XINGZHI.head;

	switch(machine_type)
	{
		case MachineType::XINGZHI:
			{
				while(machine_info!=nullptr)
				{
					ConnInfoPtr conn = std::make_shared<ConnInfo>(
							ConnType::MACHINE,machine_info->ipv4,
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
