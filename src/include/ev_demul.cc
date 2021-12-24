#include "ev_demul.h"


int EvDemul::WaitEvents()
{
	static struct epoll_event active_ev[1024];
	int active_num = epoll_wait(epfd_, active_ev, 1024, -1);
	int fd{-1};
	uint32_t events{0};

	for(int i=0; i<active_num; i++)
	{
		fd = active_ev[i].data.fd;
		events = active_ev[i].events;

		if(events & EPOLLIN)
		{
			if(fd == acpt_fd_)
			{
				ev_hdlr_->Handle_Accept(fd);
			}else{
				ev_hdlr_->Handle_Read(fd);
			}
		}
		if(events & EPOLLERR|| events & EPOLLRDHUP)
		{
			ev_hdlr_->Handle_Error(fd);
		}
	}
}
