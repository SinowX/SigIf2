#include "conf_mgr.h"
/* #include "INIReader.h" */
#include "log_mgr.h"

/* ConfMgr::ConfMgr() */
/* { */
/* 	bzero(&machine_config_, sizeof(machine_config_)); */
/* } */


static const char* ReadFile(const char* path)
{
	
	int fd = -1;
	if((fd = open(path, O_RDONLY))<0)
	{
		LOGERROR("open config file failed");
		abort();
	}

	int file_len = lseek(fd, 0, SEEK_END);
	if((file_len = lseek(fd, 0, SEEK_END))<0)
	{
		LOGERROR("cant get file size");
		abort();
	}

	lseek(fd,0, SEEK_SET);
	char* content = new char[file_len+1];

	int n = read(fd, content, file_len);

	if(n!=file_len)
	{
		LOGERROR("read file failed");
	}

	content[file_len]='\0';
	
	return content;
}

static int ValidateConfig(const nlohmann::basic_json<>& j)
{
	if(j["basic"].is_null()
			||j["basic"]["listen_ipv4"].is_null()
			||j["basic"]["listen_port"].is_null()
			||j["basic"]["log_path"].is_null())
		return false;

	if(!j["basic"]["listen_ipv4"].is_string()
			||!j["basic"]["listen_port"].is_string()
			||!j["basic"]["log_path"].is_string())
		return false;

	if(j["io_receiver"].is_null()
			||j["io_receiver"]["amount"].is_null())
		return false;
	if(!j["io_receiver"]["amount"].is_number_integer())
		return false;

	if(j["worker"].is_null()
			||j["worker"]["amount"].is_null())
		return false;
	if(!j["worker"]["amount"].is_number_integer())
		return false;

	if(j["machine"].is_null()
			||!j["machine"].is_array())
		return false;

	for(int i=0; i<j["machine"].size();i++)
	{
		if(j["machine"][i]["type"].is_null()
				||j["machine"][i]["ipv4_addr"].is_null()
				||j["machine"][i]["port"].is_null())
			return false;
		if(!j["machine"][i]["type"].is_string()
				||!j["machine"][i]["ipv4_addr"].is_string()
				||!j["machine"][i]["port"].is_number_unsigned())
			return false;
	}

	return true;
}

//
int ConfMgr::ReadConfig(const char *path)
{
	auto j = json::parse(ReadFile(path));
	if(ValidateConfig(j)<0)
	{
		LOGERROR("parse config failed");
		abort();
	}

	auto basic = j["basic"];
	
	strncpy(listen_addr_, basic["listen_ipv4"]
			.get<std::string>().c_str(), sizeof(listen_addr_));
	port_ = basic["listen_port"].get<uint16_t>();
	auto logpath_str = basic["log_path"].get<std::string>();
	
	logpath_str = new char[logpath_str.size()+1];
	strncpy(log_path, logpath_str.c_str(), logpath_str.size());

	io_rcvr_amount_ = j["io_receiver"]["amount"].get<int>();
	work_amount_ = j["worker"]["amount"].get<int>();

	auto machine_arr = j["machine"];
	
	for(int i=0; i<machine_arr.size(); i++)
	{
		auto machine_type = machine_arr[i]["type"].get<std::string>();
		auto Add_Device = [](MachineInfo* info,
				nlohmann::basic_json<>& machine ){
			while(info!=nullptr) info=info->next;
			info = new MachineInfo;
			info->id = 0;
			strncpy(info->ipv4, machine["ipv4"]
					.get<std::string>().c_str(), 16);
			info->port = machine["port"].get<uint16_t>();
		};

		if(machine_type.compare("XINGZHI"))
		{
			MachineInfo* info=machine_config_.XINGZHI.head;
			Add_Device(info, machine_arr[i]);
			machine_config_.XINGZHI.number++;
		}else
			if(machine_type.compare("HAIXIN"))
		{
			MachineInfo* info=machine_config_.HAIXIN.head;
			Add_Device(info, machine_arr[i]);
			machine_config_.HAIXIN.number++;
		}else
		{
			LOGWARN("Unknown machine Type");
		}
	}
	return 0;
}
