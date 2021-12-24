// Event Handler
// 事件处理器
// 封装绑定的处理函数，由EvDemul调用
#ifndef EV_HDLR_H
#define EV_HDLR_H

#include <functional>
#include <nlohmann/json.hpp>
#include <mutex>
#include "conn_map.h"
#include "log_mgr.h"


class EvHdlr
{
	public:
		EvHdlr(
				std::function<int(const int)> accept_hdlr=nullptr,
				std::function<int(const int)> error_hdlr=nullptr,
				std::function<int(const int)> read_hdlr=nullptr,
				std::function<int(const int, const char*)> write_hdlr=nullptr,
				std::function<int(const int)> query_hdlr=nullptr
				)
			:accept_hdlr_(accept_hdlr), error_hdlr_(error_hdlr),
			read_hdlr_(read_hdlr), write_hdlr_(write_hdlr), query_hdlr_(query_hdlr)
		{}

		~EvHdlr()=default;


		int Handle(std::function<int(const int)>& hdlr, const int fd)
		{
			if(hdlr!=nullptr)
				return hdlr(fd);
			else
			{
				LOGWARN("trying to invoke unset hldr");
				return -1;
			}
		}

		int Handle(std::function<int(const int, const char*)>& hdlr, const int fd, const char* str)
		{
			if(hdlr!=nullptr)
				return hdlr(fd,str);
			else
			{
				LOGWARN("trying to invoke unset hldr");
				return -1;
			}
		}
		
		int Handle_Accept(const int fd)
		{
			return Handle(accept_hdlr_,fd);
		}
		
		// close this fd and free relative ConnMap item
		int Handle_Error(const int fd)
		{
			return Handle(error_hdlr_,fd);
		}

		int Handle_Read(const int fd)
		{
			return Handle(read_hdlr_,fd);
		}

		int Handle_Query(const int fd)
		{ 
			return Handle(query_hdlr_,fd);
		}

		int Handle_Write(int fd, const char* str)
		{ 
			return Handle(write_hdlr_,fd, str);
		}

	private:
		std::function<int(const int)> accept_hdlr_;
		std::function<int(const int)> error_hdlr_;
		std::function<int(const int)> read_hdlr_;
		std::function<int(const int, const char*)> write_hdlr_;
		std::function<int(const int)> query_hdlr_;
		/* std::mutex mtx_; */
};

#endif
