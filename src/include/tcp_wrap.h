#ifndef TCP_WRAP_H
#define TCP_WRAP_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "variant_const.h"

#include <mutex>
#include <condition_variable>
#include <cstring>


using BufferPtr = std::shared_ptr<std::string>;

class TcpWrap
{
  public:
    /* TcpWrap()=default; */
		TcpWrap(const char* ipv4_addr, const uint16_t port, const int fd=-1)
			:port_(port)
		{
			/* strcpy(ipv4_addr_, ipv4_addr); */
			memcpy(ipv4_addr_,ipv4_addr, 16);
			w_buffer_ = std::make_shared<std::string>();
			r_buffer_ = std::make_shared<std::string>();
		}
		TcpWrap(TcpWrap&)=delete;
		void operator=(TcpWrap&)=delete;
      /* :ipv4_addr_(new char[20]),fd_(-1),port_(-1) */
    ~TcpWrap()
    {
    };

    int Listen(const int max_listen_queue=1024);
    int Accept();
    int Connect(const char* ip, const int port);
		// receive all to buffer
		int ReceiveAll();
		int Receive(char* buffer, ssize_t len);
		int Send(char* buffer, ssize_t len);

    inline const int get_fd()const {return fd_;}
		/* inline const int get_op_fd() const {return op_fd_;} */
		inline const char* get_ipaddr() const {return ipv4_addr_;}
		inline const int get_port() const {return port_;}
		inline bool is_connected() const {return connected_;}
		inline BufferPtr get_r_buffer();
		inline BufferPtr get_w_buffer();
  private:
    int fd_{-1}; // self fd
    char ipv4_addr_[16]{0};
    uint16_t port_{0};
		bool connected_{false};

		BufferPtr w_buffer_{nullptr};	
		std::mutex r_mtx_;
		std::condition_variable r_condi;

		BufferPtr r_buffer_{nullptr};
		std::mutex w_mtx_;
		std::condition_variable w_condi;
};



#endif
