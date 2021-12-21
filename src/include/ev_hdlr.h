#ifndef EV_HDLR_H
#define EV_HDLR_H

#include <functional>
#include <nlohmann/json.hpp>
#include <mutex>
#include "conn_map.h"

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
		{};
		~EvHdlr();


		int Handle_Accept(const int fd)
		{
			if(accept_hdlr_!=nullptr)
		 		return accept_hdlr_(fd);
			else
				return -1;
		}
		
		// close this fd and free relative ConnMap item
		int Handle_Error(const int fd)
		{
			ConnMap::getInstance().Drop(fd);
			close(fd);
			return 0;
		}

		int Handle_Read(const int fd)
		{
			if(read_hdlr_!=nullptr)
		 		return read_hdlr_(fd);
			else
				return -1;
		}

		int Handle_Query(const int fd)
		{ 
			if(query_hdlr_!=nullptr)
		 		return query_hdlr_(fd);
			else
				return -1;
		}

		int Handle_Write(int fd, const char* str)
		{ 
			/* std::lock_guard<std::mutex> lk(mtx_); */
			if(write_hdlr_!=nullptr)
		 		return write_hdlr_(fd, str);
			else
				return -1;
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
