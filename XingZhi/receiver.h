#ifndef RECEIVER_H
#define RECEIVER_H

#include "connection.h"
#include "sys/epoll.h"
#include <iostream>

std::string Fd2Ipv4(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof addr;
	if(getpeername(fd, (struct sockaddr*)&addr,&addr_len)<0)
	{
		std::cout<<"try to use fake socket fd"<<std::endl;
		return nullptr;
	}
	return std::string(inet_ntoa(addr.sin_addr));
}

void receiver()
{
	int epfd = epoll_create(1024);
	struct epoll_event ev_tpl;
	bzero(&ev_tpl, sizeof(ev_tpl));
	ev_tpl.events=EPOLLIN|EPOLLET|EPOLLERR|EPOLLRDHUP;

	struct epoll_event active_ev[1024];
	int active_number = 0;
	
	while(true)
	{
		active_number = epoll_wait(epfd, active_ev, 1024, -1);
		for(int i=0; i<active_number; i++)
		{
			int fd = active_ev[i].data.fd;
			uint32_t events = active_ev[i].events;

			if(events & EPOLLIN){
				auto conn = ConnMap::getInstance().Get(Fd2Ipv4(fd));		
				conn->ReceiveAll();
				while(!conn->GetPackQ().empty())
				{
					auto pack = conn->GetPackQ().front();
					std::shared_ptr<InsParser> parser =
						std::make_shared<InsParser>();
					parser->Parse(
							reinterpret_cast<const unsigned char*>(pack.c_str()),
						 	pack.size());

					TaskMap::ItBool task;
	
					while(!(task = TaskMap::getInstance()
								.Get(parser->GetInsName())).valid)
					{
						// 10s to be expired
						if(time(nullptr)-task.it->second->timestamp<10){
							task.it->second->mtx->lock();
							task.it->second->parser=parser;
							task.it->second->mtx->unlock();
							task.it->second->condi->notify_one();
						}
						TaskMap::getInstance().Drop(task.it);
					}
				}
			}

			if(events & EPOLLERR){
				ConnMap::getInstance().Drop(Fd2Ipv4(fd));
			}

		}
	}
}

#endif
