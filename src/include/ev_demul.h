#ifndef EV_DEMUL_H
#define EV_DEMUL_H

#include <memory>
#include <functional>
#include "ev_hdlr.h"
#include <sys/epoll.h>
#include "tcp_wrap.h"
#include "conn_info.h"
#include "conn_map.h"
#include "conf_mgr.h"

class EvDemul
{
	public:
		EvDemul()
		{
			ev_tpl.events=EPOLLIN|EPOLLET|EPOLLERR|EPOLLRDHUP;
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
			if(ev_hdlr_!=nullptr)
			{
				ev_hdlr_ = std::make_shared<EvHdlr>(accept_func, error_func, read_func, write_func, query_func);
				return 0;
			}else{
				return -1;
			}
		}

		int AddListen(int fd, struct epoll_event* ev=nullptr)
		{
			if(ev==nullptr)	ev = &ev_tpl;
			return epoll_ctl(epfd_, EPOLL_CTL_ADD , fd, ev);
		}
		int DropListen(int fd, struct epoll_event* ev=nullptr)
		{
			if(ev==nullptr)	ev = &ev_tpl;
			return epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, ev);
		}

		int WaitEvents()
		{
			struct epoll_event active_ev[1024];
			int active_num = epoll_wait(epfd_, active_ev, 1024, -1);
			int fd{-1};
			uint32_t events{0};
			ConnInfoPtr conn{nullptr};

			for(int i=0; i<active_num; i++)
			{
				fd = active_ev[i].data.fd;
				events = active_ev[i].events;
				conn=ConnMap::getInstance().Find(fd);
				if(events & EPOLLIN)
				{
					if(fd == acpt_fd_)
					{
						ev_hdlr_->Handle_Accept(fd);
					}else{
						ev_hdlr_->Handle_Read(fd);
						/* conn->get_tcp_conn()->ReceiveAll(); */
						/* BufferPtr buff = conn->get_tcp_conn()->get_r_buffer(); */
						/* while(!buff->empty()) */
						/* { */
						/* 	std::string task; */
						/* 	bool brace_match{false}; */
						/* 	int cnt{-1}; */
						/* 	bool buffer_error{false}; */

						/* 	for(std::string::iterator it = buff->begin();it!=buff->end();it++) */
						/* 	{ */
						/* 		if(*it=='{') */
						/* 		{ */
						/* 			if(cnt==-1) cnt=1; */
						/* 			else cnt++; */
						/* 		}else{ */
						/* 			if(cnt==-1) */
						/* 			{ */
						/* 				buffer_error=true; */
						/* 				break; */
						/* 			}else{ */
						/* 				cnt--; */
						/* 				if(cnt==0) */
						/* 				{ */
						/* 					brace_match=true; */
						/* 					// this is one pack */
						/* 					task=std::string(buff->begin(),it); */
						/* 					buff->erase(buff->begin(), it); */
						/* 				} */
						/* 			} */
						/* 		} */
						/* 	} */
						/* 	if(buffer_error) */
						/* 	{ */
						/* 		ev_hdlr_->Handle_ConnError(fd); */
						/* 		break; */
						/* 	} */
						/* 	else if(brace_match) */
						/* 	{ */
						/* 		// read buffer */
						/* 		ev_hdlr_->Handle_Query(fd,task.c_str()); */
						/* 		continue; */
						/* 	} */
						/* 	else break; */
						/* 	// if did not get one pack ,then just break */
						/* } */
					}
				}
				if(events & EPOLLERR|| events & EPOLLRDHUP)
				{
					ev_hdlr_->Handle_Error(fd);
				}

			}
		}


	private:
		std::shared_ptr<EvHdlr> ev_hdlr_{nullptr};
		int epfd_;
		struct epoll_event ev_tpl;
		int acpt_fd_;
};

#endif
