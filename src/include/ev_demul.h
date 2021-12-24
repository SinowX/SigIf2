// Event Demultiplexer
// 事件驱动复用器
// 注册回调函数，Epoll等待事件、调用回调函数
#ifndef EV_DEMUL_H
#define EV_DEMUL_H

#include <memory>
#include <functional>
#include <sys/epoll.h>

#include "conn_map.h"
#include "conf_mgr.h"
#include "ev_hdlr.h"
#include "log_mgr.h"

class EvDemul
{
	public:
		EvDemul(const int accept_fd=-1)
			:acpt_fd_(accept_fd)
		{
			bzero(&ev_tpl, sizeof ev_tpl);
			ev_tpl.events=EPOLLIN|EPOLLET|EPOLLERR|EPOLLRDHUP;
			epfd_ = epoll_create(1024);
		}
		~EvDemul(){};
		EvDemul(EvDemul&)=delete;
		void operator=(EvDemul&)=delete;	

		int RegisterCallback(
				std::function<int(const int)> accept_func=nullptr,
				std::function<int(const int)> error_func=nullptr,
				std::function<int(const int)> read_func=nullptr,
				std::function<int(const int, const char*)> write_func=nullptr,
				std::function<int(const int)> query_func=nullptr
				)
		{
			if(ev_hdlr_==nullptr)
			{
				ev_hdlr_ = std::make_shared<EvHdlr>(
						accept_func, error_func, read_func,
					 	write_func, query_func);
				return 0;
			}else{
				LOGWARN("trying to register a registered ev_hdlr");
				return -1;
			}
		}

		int AddEvent(int fd, struct epoll_event* ev=nullptr)
		{
			if(ev==nullptr)	ev = &ev_tpl;
			return epoll_ctl(epfd_, EPOLL_CTL_ADD , fd, ev);
		}
		int DropEvent(int fd, struct epoll_event* ev=nullptr)
		{
			if(ev==nullptr)	ev = &ev_tpl;
			return epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, ev);
		}
		int WaitEvents();

	private:
		std::shared_ptr<EvHdlr> ev_hdlr_{nullptr};
		int epfd_;
		struct epoll_event ev_tpl;
		int acpt_fd_;
};

#endif
