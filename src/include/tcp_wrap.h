#ifndef TCP_WRAP_H
#define TCP_WRAP_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <memory>

#include <mutex>
#include <condition_variable>
#include <cstring>

#include "type_safe.h"
#include "log_mgr.h"

class Buffer
{
public:
	Buffer()=default;
	Buffer(Buffer&)=delete;
	void operator=(Buffer&)=delete;
	~Buffer()=default;
	// return nullptr if cant take a json
	const char* get_One_Json()
	{
		
		return nullptr;
	}

	int Append(const char* str, const ssize_t len)
	{
		buff_->append(str, len);
		return len;
	}
	ssize_t get_Size() const
	{
		return buff_->size();
	}

private:
	StringUnq buff_;
	std::mutex mtx_;
	bool no_json_for_now_{true};
};

class TcpWrap
{
public:
	TcpWrap(const char* ipv4_addr, const uint16_t port, const int fd=-1)
		:port_(port),fd_(fd),
		w_buffer_(std::make_unique<Buffer>()),
		r_buffer_(std::make_unique<Buffer>())
	{
		strncpy(ipv4_addr_, ipv4_addr, 16);
	}

	TcpWrap(TcpWrap&)=delete;
	void operator=(TcpWrap&)=delete;
	~TcpWrap()
	{
		if(fd_!=-1) shutdown(fd_, SHUT_WR);
	}

	int get_Fd()const 
	{
		return fd_;
	}

	int Listen()
	{
		if(fd_>=0)
		{
			LOGWARN("try to listen with used conn");
			return -1;
		}
		if((fd_ = socket(AF_INET, SOCK_STREAM, 0))<0)
		{
			LOGWARN("init socket fd failed");
			Reset();
			return -1;
		}
		if(fcntl(fd_, F_SETFD, O_NONBLOCK)<0)
		{
			LOGWARN("set nonblocking failed");
			Reset();
			return -1;
		}
		struct sockaddr_in addr;
		bzero(&addr, sizeof addr);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port_);
		addr.sin_addr.s_addr = inet_addr(ipv4_addr_);

		if(bind(fd_,(struct sockaddr*)&addr, sizeof addr)<0)
		{
			LOGWARN("bind failed");
			close(fd_);
			fd_ = -1;
			return -1;
		}
		return listen(fd_, 1024);
	}

	int Accept()
	{
		if(fd_<0)
		{
			LOGWARN("try to accept without listening");
			return -1;
		}
		return accept(fd_, NULL, NULL);
	}
	int Connect()
	{
		if(fd_>=0)
		{
			LOGWARN("try to connect with used conn");
			return -1;
		}
		;
		if((fd_ = socket(AF_INET, SOCK_STREAM, 0))<0)
		{
			LOGWARN("init socket fd failed");
			Reset();
			return -1;
		}
		if(fcntl(fd_, F_SETFD, O_NONBLOCK)<0)
		{
			LOGWARN("set nonblocking failed");
			Reset();
			return -1;
		}
		struct sockaddr_in addr;
		bzero(&addr, sizeof addr);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port_);
		addr.sin_addr.s_addr = inet_addr(ipv4_addr_);

		if(connect(fd_,(struct sockaddr*)&addr, sizeof addr )<0)
		{
			LOGWARN("connect failed");
			Reset();
			return -1;
		}
		return 0;
	}

	int ReceiveAll()
	{
		static char read_buf[1000];
		int n = read(fd_, read_buf, 1000);
		while((n = read(fd_, read_buf, 1000))>0)
		{
			if(n<0)
			{
				LOGWARN("socket read failed");
				return -1;
			}
			r_buffer_->Append(read_buf, n);
		}
		return 0;
	}
	int Send(const char* data, const std::size_t len)
	{
		int n = write(fd_, data, len);
		if(n!=len)
		{
			LOGWARN("socket write failed");
			return -1;
		}
		return 0;
	}

	bool is_Connected() const {return isConnected_;}
	const char* get_Ipv4() const {return ipv4_addr_;}
	const uint16_t get_Port() const {return port_;}

	std::unique_ptr<Buffer>& get_r_Buffer() {return r_buffer_;}
	std::unique_ptr<Buffer>& get_w_Buffer() {return w_buffer_;}

private:
	int Reset()
	{
		if(fd_<0)
		{
			LOGWARN("try to reset unset conn");
			return -1;
		}
		close(fd_);
		fd_=-1;
		return 0;
	}
	
	// bind fd
	/* int Initialize(); */
	bool isConnected_{false};
	int fd_{-1};
	char ipv4_addr_[16]{0};
	/* StringShr ipv4_addr_; */
	
	uint16_t port_{0};
	
	std::unique_ptr<Buffer> w_buffer_{nullptr};	
	std::unique_ptr<Buffer> r_buffer_{nullptr};	
};

#endif
