#ifndef CONF_MGR_H
#define CONF_MGR_H

#include <cstdint>
#include "variant_const.h"
#include <cstring>

class ConfMgr
{
	public:
		static ConfMgr& getInstance()
		{
			static ConfMgr instance_;
			return instance_;
		}

		~ConfMgr()=default;

		ConfMgr(ConfMgr&)=delete;
		void operator=(ConfMgr&)=delete;
		
		// read config. return 0 for success, -1 for error
		int ReadConfig(const char* path);

		// if has not read config ,return 0 or nullptr
		inline const int get_worker_amount() const
			{if(read_success_) return work_amount_; else return 0;}
		inline const int get_io_rcvr_amount() const 
			{if(read_success_) return io_rcvr_amount_; else return 0;}
		inline const Machine& get_machine() const
		  {return machine_config_;}


		inline const uint16_t get_port() const
			{if(read_success_) return port_; else return 0;}
		inline const char* get_listen_addr() const
			{if(read_success_) return listen_addr_; else return nullptr;}


	private:
		ConfMgr()
		{
			memset(&machine_config_, 0, sizeof(machine_config_));			
		}
		bool read_success_{false};
		int work_amount_{0};
		int io_rcvr_amount_{0};
		Machine machine_config_;
		char listen_addr_[16]{0};
		uint16_t port_{0};

};

#endif
