// 任务处理函数的实现
#include "handler.h"
#include "conn_map.h"
#include "conn_q.h"
#include "instruction_manager.h"



// accept handlers
int accept_fd_hdlr(const int fd)
{
	int newfd = accept(fd,nullptr,nullptr);
	
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getpeername(fd,(struct sockaddr*)&addr,
			&addr_len);
	
	ConnInfoPtr newconn = std::make_shared<ConnInfo>(
			ConnType::NORMAL_CONN, inet_ntoa(addr.sin_addr),
			ntohs(addr.sin_port), fd, ConnStatus::CONNECTED);
	
	ConnMap::getInstance().Insert(fd, newconn);
	
	NewConnQ().Push(newconn);
	return 0;
}

// 已实现，在另外的一个仓库，还未加进来
bool CheckIfValid(nlohmann::basic_json<>& rqs);
//
bool CheckIfQuery(nlohmann::basic_json<>& rqs);
/* bool CheckIfQueryRes(nlohmann::basic_json<>& rqs); */
bool CheckIfSetting(nlohmann::basic_json<>& rqs);
/* bool CheckIfSettingRes(nlohmann::basic_json<>& rqs); */

// query handlers
// parse json_str, translate to udp pack, send udp to machine and Upload HupTask
int client_query_hdlr(ConnInfoPtr client_conn, BufferPtr json_str)
{
	auto rqs = json::parse(json_str);
	std::string udp_pack;
	if(CheckIfValid(rqs))
	{
			// only process Query and Setting, others should be check here
		InsPack packer;
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::ActionTable))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::ActionTableQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				DATA_CONTENT::ActionTable::para content;
				bzero(&content, sizeof(content));
				
				content.action_id = data["action_id"].get<uint8_t>();
				content.plan_id = data["plan_id"].get<uint8_t>();

				packer.Set(InsType::ActionTableSetting,
						RESERVE::DEFAULT,reinterpret_cast<const unsigned char*>(&content),sizeof content);
			}
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::CharacterParameterVersion))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::CharacterParameterVersionQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				DATA_CONTENT::CharacterParameterVersion::para content;
				bzero(&content, sizeof(content));
				
				content = data["version"].get<uint8_t>();

				packer.Set(InsType::CharacterParameterVersionSetting,
						RESERVE::DEFAULT,reinterpret_cast<const unsigned char*>(&content),sizeof content);
			}
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Detector))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::DetectorQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["content"];
				using cn_t = DATA_CONTENT::Detector::para;
				cn_t* content	= new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					content[i].ID = j_arr[i]["id"].get<uint8_t>();
					content[i].OPTION = j_arr[i]["option"].get<uint8_t>();
					content[i].OPEN_PHASE = j_arr[i]["open_phase"].get<uint8_t>();
					content[i].CLOSE_PHASE = j_arr[i]["close_phase"].get<uint8_t>();
					content[i].DELAY_TIME = j_arr[i]["delay_time"].get<uint8_t>();
					content[i].DELAY = j_arr[i]["delay"].get<uint8_t>();
					content[i].QUEUE_LIMIT = j_arr[i]["queue_limit"].get<uint8_t>();
					content[i].NO_RESPONSE = j_arr[i]["no_response"].get<uint8_t>();
					content[i].MAX_EXSIT = j_arr[i]["max_exist"].get<uint8_t>();
					content[i].ERROR_COUNT = j_arr[i]["error_count"].get<uint8_t>();
					content[i].FALURE_TIME = j_arr[i]["failure_time"].get<uint8_t>();
					content[i].WARNING = j_arr[i]["warning"].get<uint8_t>();
					content[i].WARNING_LOG = j_arr[i]["warning_log"].get<uint8_t>();
					content[i].RESET = j_arr[i]["reset"].get<uint8_t>();
				}
				
				packer.Set(InsType::DetectorSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::FailureConfig))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::FailureQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["config"];
				using cn_t = DATA_CONTENT::FailureConfig::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					content[i].id = j_arr[i]["id"].get<uint8_t>();
					content[i].mode = j_arr[i]["mode"].get<uint8_t>();
					content[i].operation = j_arr[i]["operation"].get<uint8_t>();
					content[i].operation_value = j_arr[i]["operation_value"].get<uint8_t>();
				}
				
				packer.Set(InsType::DetectorSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Failure))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::FailureQuery);
			}
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::FollowPhaseTable))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::FollowPhaseTableQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["table"];
				using cn_t = DATA_CONTENT::FollowPhaseTable::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					content[i].id = j_arr[i]["id"].get<uint8_t>();
					content[i].type = j_arr[i]["type"].get<uint8_t>();
					content[i].father_phase = j_arr[i]["father_phase"].get<uint32_t>();
					content[i].correct_phase = j_arr[i]["correct_phase"].get<uint32_t>();
					content[i].green_time = j_arr[i]["green_time"].get<uint8_t>();
					content[i].yellow_time = j_arr[i]["yellow_time"].get<uint8_t>();
					content[i].red_time = j_arr[i]["red_time"].get<uint8_t>();
					content[i].green_flash = j_arr[i]["green_flash"].get<uint8_t>();
				}
				
				packer.Set(InsType::FollowPhaseTableSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::IdentificationCode))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::IdentificationCodeQuery);
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::LightStatus))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::LightStatusQuery);
			}
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PedestrianDetector))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::PedestrianDetectorQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["content"];
				using cn_t = DATA_CONTENT::PedestrianDetector::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					/* content[i].id = j_arr[i]["id"].get<uint8_t>(); */
					content[i].detector_id = j_arr[i]["detector_id"].get<uint8_t>();
					content[i].trigger_phase = j_arr[i]["trigger_phase"].get<uint8_t>();
					content[i].no_response = j_arr[i]["no_response"].get<uint8_t>();
					content[i].max_trigger_time = j_arr[i]["max_trigger_time"].get<uint8_t>();
					content[i].error_count = j_arr[i]["error_count"].get<uint8_t>();
					content[i].warning = j_arr[i]["warning"].get<uint8_t>();
				}
				
				packer.Set(InsType::PedestrianDetectorSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Phase))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::PhaseQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["status"];
				using cn_t = DATA_CONTENT::Phase::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					/* content[i].id = j_arr[i]["id"].get<uint8_t>(); */
					content[i].phase_id = j_arr[i]["phase_id"].get<uint8_t>();
					content[i].pedastrain_pass = j_arr[i]["pedastrain_pass"].get<uint8_t>();
					content[i].pedastrain_clear = j_arr[i]["pedastrain_clear"].get<uint8_t>();
					content[i].min_green = j_arr[i]["lag_green"].get<uint8_t>();
					content[i].max_green_1= j_arr[i]["max_green_1"].get<uint8_t>();
					content[i].max_green_2= j_arr[i]["max_green_2"].get<uint8_t>();
					content[i].yellow_transition = j_arr[i]["yellow_transition"].get<uint8_t>();
					content[i].red_clear= j_arr[i]["red_clear"].get<uint8_t>();
					content[i].protection= j_arr[i]["protection"].get<uint8_t>();
					content[i].inc_init= j_arr[i]["inc_init"].get<uint8_t>();
					content[i].inc_max= j_arr[i]["inc_max"].get<uint8_t>();
					content[i].dec_previous_time = j_arr[i]["dec_previous_time"].get<uint8_t>();
					content[i].dec_previous_traffic = j_arr[i]["dec_previous_traffic"].get<uint8_t>();
					content[i].dec_time= j_arr[i]["dec_time"].get<uint8_t>();
					content[i].dec_rate= j_arr[i]["dec_rate"].get<uint8_t>();
					content[i].min_interval= j_arr[i]["min_interval"].get<uint8_t>();
					content[i].dynamic_max= j_arr[i]["dynamic_max"].get<uint8_t>();
					content[i].dynamic_step= j_arr[i]["dynamic_step"].get<uint8_t>();
					content[i].initial_para= j_arr[i]["initial_para"].get<uint8_t>();
					content[i].option= j_arr[i]["option"].get<uint8_t>();
					content[i].loop_id= j_arr[i]["loop_id"].get<uint8_t>();
					content[i].cocurrency_phase= j_arr[i]["cocurrency_phase"].get<uint8_t>();
				}
				
				packer.Set(InsType::PhaseSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PhaseSequenceTable))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::PhaseSequenceTableQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["phasesequence"];
				using cn_t = DATA_CONTENT::PhaseSequenceTable::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					/* content[i].id = j_arr[i]["id"].get<uint8_t>(); */
					content[i].TABLE_ID = j_arr[i]["table_number"].get<uint8_t>();
					content[i].LOOP_ID = j_arr[i]["loop_number"].get<uint8_t>();
				}
				
				packer.Set(InsType::PhaseSequenceTableSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PriorityConfig))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::PriorityConfigQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["config"];
				using cn_t = DATA_CONTENT::PriorityConfig::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					/* content[i].id = j_arr[i]["id"].get<uint8_t>(); */
					content[i].id = j_arr[i]["id"].get<uint8_t>();
					content[i].control_interface = j_arr[i]["control_interface"].get<uint8_t>();
					content[i].link = j_arr[i]["link"].get<uint8_t>();
					content[i].delay = j_arr[i]["delay"].get<uint8_t>();
					content[i].min_existence = j_arr[i]["min_existence"].get<uint8_t>();
					content[i].min_green = j_arr[i]["min_green"].get<uint8_t>();
					content[i].motor_linger = j_arr[i]["motor_linger"].get<uint8_t>();
					content[i].pedastrain_linger = j_arr[i]["pedastrain_linger"].get<uint8_t>();
					content[i].pass_time = j_arr[i]["pass_time"].get<uint8_t>();
					content[i].min_interval = j_arr[i]["min_interval"].get<uint8_t>();
					content[i].max_continue = j_arr[i]["max_continue"].get<uint8_t>();
					content[i].track_phase = j_arr[i]["track_phase"].get<uint8_t>();
					content[i].motor_linger_phase = j_arr[i]["motor_linger_phase"].get<uint8_t>();
					content[i].pedastrain_linger_phase = j_arr[i]["pedastrain_linger_phase"].get<uint8_t>();
					content[i].exit_phase = j_arr[i]["exit_phase"].get<uint8_t>();
					content[i].priority_status = j_arr[i]["priority_status"].get<uint8_t>();
				}
				
				packer.Set(InsType::PriorityConfigSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::RemoteControl))
		{
			if(CheckIfSetting(rqs))
			{

				auto data = rqs["data"];
				DATA_CONTENT::RemoteControl::para content;
				bzero(&content, sizeof(content));
			
				content = data["control_type"].get<uint8_t>();

				packer.Set(InsType::RemoteControlSetting,
						RESERVE::DEFAULT,reinterpret_cast<const unsigned char*>(&content),sizeof content);
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Schedule))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::ScheduleQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["plan"];
				using cn_t = DATA_CONTENT::Schedule::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					/* content[i].id = j_arr[i]["id"].get<uint8_t>(); */
					content[i].plan_id = j_arr[i]["plan_id"].get<uint8_t>();
					content[i].month_data = j_arr[i]["month_data"].get<uint8_t>();
					content[i].week_data = j_arr[i]["week_data"].get<uint8_t>();
					content[i].day_data = j_arr[i]["day_data"].get<uint8_t>();
					content[i].time_schedule_id = j_arr[i]["time_schedule_number"].get<uint8_t>();
				}
				
				packer.Set(InsType::ScheduleSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::SchemaTable))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::SchemaTableQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["schema"];
				using cn_t = DATA_CONTENT::SchemaTable::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					content[i].id = j_arr[i]["id"].get<uint8_t>();
					content[i].cycle_length = j_arr[i]["cycle_length"].get<uint8_t>();
					content[i].phase_difference = j_arr[i]["phase_difference"].get<uint8_t>();
					content[i].green_sig_rate_table_id = j_arr[i]["green_sig_rate_table_id"].get<uint8_t>();
					content[i].phase_sequence_id = j_arr[i]["phase_sequence_id"].get<uint8_t>();
				}
				
				packer.Set(InsType::SchemaTableSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::SemaphoreGroup))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::SemaphoreGroupQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["group"];
				using cn_t = DATA_CONTENT::SemaphoreGroup::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					content[i].grp_id= j_arr[i]["group_number"].get<uint8_t>();
					content[i].ctrl_type= j_arr[i]["control_type"].get<uint8_t>();
					content[i].ctrl_id = j_arr[i]["control_number"].get<uint8_t>();
					content[i].flash = j_arr[i]["flash"].get<uint8_t>();
					content[i].brightness= j_arr[i]["brightness"].get<uint8_t>();
					content[i].location_x= j_arr[i]["location_x"].get<uint16_t>();
					content[i].location_y= j_arr[i]["location_y"].get<uint16_t>();
					content[i].timer_id= j_arr[i]["timer_number"].get<uint8_t>();
				}
				
				packer.Set(InsType::SemaphoreGroupSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Time))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::TimeQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				DATA_CONTENT::Time::para content;
				bzero(&content, sizeof(content));
			
				content = data["timestamp"].get<uint32_t>();

				packer.Set(InsType::TimeSetting,
						RESERVE::DEFAULT,reinterpret_cast<const unsigned char*>(&content),sizeof content);
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TimeTable))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::TimeTableQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["table"];
				using cn_t = DATA_CONTENT::TimeTable::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					content[i].time_table_id= j_arr[i]["time_table_id"].get<uint8_t>();
					content[i].time_segment_id= j_arr[i]["time_segment_id"].get<uint8_t>();
					content[i].time_segment_begin_hour= j_arr[i]["time_segment_begin_hour"].get<uint8_t>();
					content[i].time_segment_begin_minu= j_arr[i]["time_segment_begin_minu"].get<uint8_t>();
					content[i].action_id= j_arr[i]["action_id"].get<uint8_t>();
					content[i].failure_plan_id= j_arr[i]["failure_plan_id"].get<uint8_t>();
				}
				
				packer.Set(InsType::TimeTableSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TimingScheme))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::TimingSchemeQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				auto j_arr = data["scheme"];
				using cn_t = DATA_CONTENT::TimingScheme::para;
				cn_t* content = new cn_t[j_arr.size()];

				bzero(&content, j_arr.size()*sizeof(cn_t));
				
				for(int i=0; i<j_arr.size();i++)
				{
					content[i].time_id= j_arr[i]["time_id"].get<uint8_t>();
					content[i].phase= j_arr[i]["phase"].get<uint8_t>();
					content[i].time_len= j_arr[i]["time_len"].get<uint8_t>();
					content[i].mode= j_arr[i]["mode"].get<uint8_t>();
					content[i].option= j_arr[i]["option"].get<uint8_t>();
				}
				
				packer.Set(InsType::TimingSchemeSetting,RESERVE::DEFAULT,
						reinterpret_cast<const unsigned char*>(&content),
						j_arr.size()*sizeof(cn_t));

				delete[] content;
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::UnitParameter))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::UnitParameterQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				DATA_CONTENT::UnitParameter::para content;
				bzero(&content, sizeof(content));
			
				content.boot_yellow = data["boot_yellow"].get<uint8_t>();
				content.boot_red = data["boot_red"].get<uint8_t>();
				content.gps_sync_time = data["gps_sync_time"].get<uint8_t>();
				content.dec_time = data["dec_time"].get<uint8_t>();
				content.speed_factor = data["speed_factor"].get<uint8_t>();
				content.comunication_overtime = data["communication_overtime"].get<uint8_t>();
				content.flash_frequence = data["flash_frequence"].get<uint8_t>();
				content.cross_street_time_lag = data["cross_street_time_lag"].get<uint8_t>();
				content.cross_reverse_street_time_lag = data["cross_reverse_street_time_lag"].get<uint8_t>();
				content.transition_cycle = data["transition_cycle"].get<uint8_t>();
				content.data_flow_collect_interval = data["data_flow_collect_interval"].get<uint8_t>();
				content.data_flow_interval_unit = data["data_flow_interval_unit"].get<uint8_t>();
				content.min_red = data["min_red"].get<uint8_t>();
				content.pedastrain_auto_clear = data["pedastrain_auto_clear"].get<uint8_t>();

				packer.Set(InsType::UnitParameterSetting,
						RESERVE::DEFAULT,reinterpret_cast<const unsigned char*>(&content),sizeof content);
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Version))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::VersionQuery);
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::WorkMode))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::WorkModeQuery);
			}else if(CheckIfSetting(rqs))
			{
				auto data = rqs["data"];
				DATA_CONTENT::WorkMode::para content;
				bzero(&content, sizeof(content));
				
				if(data["constant_cycle"].get<uint8_t>()!=0)
					content++;
					content=content<<1;
				if(data["reaction"].get<uint8_t>()!=0)
					content++;
					content=content<<1;
				if(data["mannual"].get<uint8_t>()!=0)
					content++;
					content=content<<1;
				if(data["off"].get<uint8_t>()!=0)
					content++;
					content=content<<1;
				if(data["yellow_flash"].get<uint8_t>()!=0)
					content++;
					content=content<<1;
				if(data["phase_lock"].get<uint8_t>()!=0)
					content++;
					content=content<<1;
				if(data["certain_phase"].get<uint8_t>()!=0)
					content++;

				packer.Set(InsType::WorkModeSetting,
						RESERVE::DEFAULT,reinterpret_cast<const unsigned char*>(&content),sizeof content);
			}

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::WorkStatus))
		{
			if(CheckIfQuery(rqs))
			{
				packer.Set(InsType::WorkStatusQuery);
			}
		}

		auto machine_conn = ConnMap::getInstance().Find(rqs["meta"]["ipv4"].get<std::string>().c_str());
		
		TaskWrapPtr task = std::make_shared<TaskWrap>(client_conn, machine_conn);

		HupTaskMap::getInstance().AddTask(task);

		machine_conn->get_udp_conn()->Send(
				reinterpret_cast<const char*>(packer.GetBuff()),
				packer.GetBuffLength());
			
		return 0;
	}else{
		return -1;
	}
}

#include <bitset>

int machine_msg_hdlr(ConnInfoPtr machine_conn)
{
	auto pack = machine_conn->get_udp_conn()->GetOnePack();
	InsParser parser;
	parser.Parse(reinterpret_cast<const unsigned char*>
			(pack->c_str()),pack->size());
	
	

	json j_res;
	json j_meta;
	j_meta["action"]="response";
	j_meta["ipv4"]=machine_conn->get_ipv4();
	json j_data;
	
	#define GETCONTENT reinterpret_cast<const cn_t*>(parser.GetContent())
	#define GETCONTENTARR reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t))

	switch(parser.GetInsType())
	{
		case InsType::OnlineRequest:
		case InsType::OnlineQuery:
		{
			break;
		}
		case InsType::TrafficInfoUpload:
		{
			using cn_t = DATA_CONTENT::TrafficInfo::para;
			j_meta["type"]=IFTYPE::TrafficInfo;
			json j_content, j_content_ele;
			j_content_ele["detector_id"]=0;
			j_content_ele["detect_interval"]=0;
			j_content_ele["flow_data"]=0;
			j_content_ele["occupation"]=0;
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["detector_id"]=GETCONTENTARR->detector_id;
				j_content_ele["detect_interval"]=GETCONTENTARR->detect_interval;
				j_content_ele["flow_data"]=GETCONTENTARR->data_flow;
				j_content_ele["occupation"]=GETCONTENTARR->occupation;
				
				j_content.push_back(j_content_ele);
			}
			
			j_data["detector"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::WorkStatusQueryReply:
		{
			using cn_t = DATA_CONTENT::WorkStatus::para;
			j_meta["type"]=IFTYPE::WorkStatus;
			
			j_data["is_auto"]=GETCONTENT->manual_auto;
			j_data["workmode"]=GETCONTENT->work_mode;
			j_data["plan_number"]=GETCONTENT->plan_id;
			j_data["scheme"]=GETCONTENT->scheme_id;
			j_data["green_signal_number"]=GETCONTENT->green_sig_rate_id;
			
			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			/* j_meta["type"]=IFTYPE::WorkStatus; */
			break;
		}
		case InsType::WorkStatusUpload:
		{
			using cn_t = DATA_CONTENT::WorkStatus::para;
			j_meta["type"]=IFTYPE::WorkStatus;
			
			j_data["is_auto"]=GETCONTENT->manual_auto;
			j_data["workmode"]=GETCONTENT->work_mode;
			j_data["plan_number"]=GETCONTENT->plan_id;
			j_data["scheme"]=GETCONTENT->scheme_id;
			j_data["green_signal_number"]=GETCONTENT->green_sig_rate_id;
			
			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			/* j_meta["type"]=IFTYPE::WorkStatus; */
			break;
		}
		case InsType::LightStatusQueryReply:
		case InsType::LightStatusUpload:
		{
			j_meta["type"]=IFTYPE::LightStatus;
			using cn_t = DATA_CONTENT::LightStatus::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["status"]=GETCONTENTARR->light_1;
				j_content.push_back(j_content_ele);
				j_content_ele["status"]=GETCONTENTARR->light_2;
				j_content.push_back(j_content_ele);
				j_content_ele["status"]=GETCONTENTARR->light_3;
				j_content.push_back(j_content_ele);
				j_content_ele["status"]=GETCONTENTARR->light_4;
				j_content.push_back(j_content_ele);
				j_content_ele["status"]=GETCONTENTARR->light_5;
				j_content.push_back(j_content_ele);
				j_content_ele["status"]=GETCONTENTARR->light_6;
				j_content.push_back(j_content_ele);
			}
			
			j_data["light_group"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::TimeQueryReply:
		case InsType::TimeSettingReply:
		{
			using cn_t = DATA_CONTENT::Time::para;
			j_meta["type"]=IFTYPE::Time;
			
			j_data["timestamp"]=*GETCONTENT;
			
			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::SemaphoreGroupQueryReply:
		case InsType::SemaphoreGroupSettingReply:
		{
			j_meta["type"]=IFTYPE::SemaphoreGroup;
			using cn_t = DATA_CONTENT::SemaphoreGroup::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["group_number"]=GETCONTENTARR->grp_id;
				j_content.push_back(j_content_ele);
				j_content_ele["control_type"]=GETCONTENTARR->ctrl_type;
				j_content.push_back(j_content_ele);
				j_content_ele["control_number"]=GETCONTENTARR->ctrl_id;
				j_content.push_back(j_content_ele);
				j_content_ele["flash"]=GETCONTENTARR->flash;
				j_content.push_back(j_content_ele);
				j_content_ele["brightness"]=GETCONTENTARR->brightness;
				j_content.push_back(j_content_ele);
				j_content_ele["location_x"]=GETCONTENTARR->location_x;
				j_content.push_back(j_content_ele);
				j_content_ele["location_y"]=GETCONTENTARR->location_y;
				j_content.push_back(j_content_ele);
				j_content_ele["timer_number"]=GETCONTENTARR->timer_id;
				j_content.push_back(j_content_ele);

			}
			
			j_data["group"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::PhaseQueryReply:
		case InsType::PhaseSettingReply:
		{
			j_meta["type"]=IFTYPE::Phase;
			using cn_t = DATA_CONTENT::Phase::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["phase_id"]=GETCONTENTARR->phase_id;
				j_content.push_back(j_content_ele);
				j_content_ele["pedastrain_pass"]=GETCONTENTARR->pedastrain_pass;
				j_content.push_back(j_content_ele);
				j_content_ele["pedastrain_clear"]=GETCONTENTARR->pedastrain_clear;
				j_content.push_back(j_content_ele);
				j_content_ele["min_green"]=GETCONTENTARR->min_green;
				j_content.push_back(j_content_ele);
				j_content_ele["lag_green"]=GETCONTENTARR->lag_green;
				j_content.push_back(j_content_ele);
				j_content_ele["max_green_1"]=GETCONTENTARR->max_green_1;
				j_content.push_back(j_content_ele);
				j_content_ele["max_green_2"]=GETCONTENTARR->max_green_2;
				j_content.push_back(j_content_ele);
				j_content_ele["yellow_transition"]=GETCONTENTARR->yellow_transition;
				j_content.push_back(j_content_ele);
				j_content_ele["red_clear"]=GETCONTENTARR->red_clear;
				j_content.push_back(j_content_ele);
				j_content_ele["protection"]=GETCONTENTARR->protection;
				j_content.push_back(j_content_ele);
				j_content_ele["inc_init"]=GETCONTENTARR->inc_init;
				j_content.push_back(j_content_ele);
				j_content_ele["inc_max"]=GETCONTENTARR->inc_max;
				j_content.push_back(j_content_ele);
				j_content_ele["dec_previous_time"]=GETCONTENTARR->dec_previous_time;
				j_content.push_back(j_content_ele);
				j_content_ele["dec_previous_traffic"]=GETCONTENTARR->dec_previous_traffic;
				j_content.push_back(j_content_ele);
				j_content_ele["dec_time"]=GETCONTENTARR->dec_time;
				j_content.push_back(j_content_ele);
				j_content_ele["dec_rate"]=GETCONTENTARR->dec_rate;
				j_content.push_back(j_content_ele);
				j_content_ele["min_interval"]=GETCONTENTARR->min_interval;
				j_content.push_back(j_content_ele);
				j_content_ele["dynamic_max"]=GETCONTENTARR->dynamic_max;
				j_content.push_back(j_content_ele);
				j_content_ele["dynamic_step"]=GETCONTENTARR->dynamic_step;
				j_content.push_back(j_content_ele);
				j_content_ele["initial_para"]=GETCONTENTARR->initial_para;
				j_content.push_back(j_content_ele);
				j_content_ele["option"]=GETCONTENTARR->option;
				j_content.push_back(j_content_ele);
				j_content_ele["loop_id"]=GETCONTENTARR->loop_id;
				j_content.push_back(j_content_ele);
				j_content_ele["cocurrency_phase"]=GETCONTENTARR->cocurrency_phase;
				j_content.push_back(j_content_ele);

			}
			
			j_data["status"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::TimingSchemeQueryReply:
		case InsType::TimingSchemeSettingReply:
		{
			j_meta["type"]=IFTYPE::TimingScheme;
			using cn_t = DATA_CONTENT::TimingScheme::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["time_id"]=GETCONTENTARR->time_id;
				j_content.push_back(j_content_ele);
				j_content_ele["phase"]=GETCONTENTARR->phase;
				j_content.push_back(j_content_ele);
				j_content_ele["time_len"]=GETCONTENTARR->time_len;
				j_content.push_back(j_content_ele);
				j_content_ele["mode"]=GETCONTENTARR->mode;
				j_content.push_back(j_content_ele);
				j_content_ele["option"]=GETCONTENTARR->option;
				j_content.push_back(j_content_ele);

			}
			
			j_data["scheme"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::ScheduleQueryReply:
		case InsType::ScheduleSettingReply:
		{
			j_meta["type"]=IFTYPE::Schedule;
			using cn_t = DATA_CONTENT::Schedule::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["plan_id"]=GETCONTENTARR->plan_id;
				j_content.push_back(j_content_ele);
				j_content_ele["month_data"]=GETCONTENTARR->month_data;
				j_content.push_back(j_content_ele);
				j_content_ele["week_data"]=GETCONTENTARR->week_data;
				j_content.push_back(j_content_ele);
				j_content_ele["day_data"]=GETCONTENTARR->day_data;
				j_content.push_back(j_content_ele);
				j_content_ele["time_schedule_number"]=GETCONTENTARR->time_schedule_id;
				j_content.push_back(j_content_ele);

			}
			
			j_data["plan"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::WorkModeQueryReply:
		case InsType::WorkModeSettingReply:
		{
			using cn_t = DATA_CONTENT::Time::para;
			j_meta["type"]=IFTYPE::WorkMode;
			/* uint8_t abyte = reinterpret_cast<const unsigned char*>(parser.GetContent()); */
			std::bitset<8> abyte(*const_cast<uint8_t*>(parser.GetContent()));
			
			j_data["constant_cycle"]=static_cast<int>(abyte[0]);
			j_data["reaction"]=static_cast<int>(abyte[1]);
			j_data["mannual"]=static_cast<int>(abyte[2]);
			j_data["off"]=static_cast<int>(abyte[3]);
			j_data["yellow_flash"]=static_cast<int>(abyte[4]);
			j_data["phase_lock"]=static_cast<int>(abyte[5]);
			j_data["certain_phase"]=static_cast<int>(abyte[6]);

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::FailureQueryReply:
		case InsType::FailureUpload:
		{
			j_meta["type"]=IFTYPE::Failure;
			using cn_t = DATA_CONTENT::Failure::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["type"]=GETCONTENTARR->type;
				j_content.push_back(j_content_ele);
				j_content_ele["timestamp"]=GETCONTENTARR->time;
				j_content.push_back(j_content_ele);
				char tmpstr[8]{'\0'};
				strncpy(tmpstr, GETCONTENTARR->content, 7);
				j_content_ele["content"]=tmpstr;
				j_content.push_back(j_content_ele);

			}
			
			j_data["failure"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::VersionQueryReply:
		case InsType::VersionUpload:
		{
			using cn_t = DATA_CONTENT::Version::para;
			j_meta["type"]=IFTYPE::Version;
			
			j_data["version"]=GETCONTENT->info;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::CharacterParameterVersionQueryReply:
		case InsType::CharacterParameterVersionSettingReply:
		{
			using cn_t = DATA_CONTENT::CharacterParameterVersion::para;
			j_meta["type"]=IFTYPE::CharacterParameterVersion;
			
			j_data["version"]= *const_cast<uint8_t*>(parser.GetContent());

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::IdentificationCodeQueryReply:
		{
			break;
		}
		case InsType::DetectorQueryReply:
		case InsType::DetectorSettingReply:
		{
			j_meta["type"]=IFTYPE::Detector;
			using cn_t = DATA_CONTENT::Detector::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["id"]=GETCONTENTARR->ID;
				j_content.push_back(j_content_ele);
				j_content_ele["option"]=GETCONTENTARR->OPTION;
				j_content.push_back(j_content_ele);
				j_content_ele["open_phase"]=GETCONTENTARR->OPEN_PHASE;
				j_content.push_back(j_content_ele);
				j_content_ele["close_phase"]=GETCONTENTARR->CLOSE_PHASE;
				j_content.push_back(j_content_ele);
				j_content_ele["delay_time"]=GETCONTENTARR->DELAY_TIME;
				j_content.push_back(j_content_ele);
				j_content_ele["delay"]=GETCONTENTARR->DELAY;
				j_content.push_back(j_content_ele);
				j_content_ele["queue_limit"]=GETCONTENTARR->QUEUE_LIMIT;
				j_content.push_back(j_content_ele);
				j_content_ele["no_response"]=GETCONTENTARR->NO_RESPONSE;
				j_content.push_back(j_content_ele);
				j_content_ele["max_exist"]=GETCONTENTARR->MAX_EXSIT;
				j_content.push_back(j_content_ele);
				j_content_ele["error_count"]=GETCONTENTARR->ERROR_COUNT;
				j_content.push_back(j_content_ele);
				j_content_ele["failure_time"]=GETCONTENTARR->FALURE_TIME;
				j_content.push_back(j_content_ele);
				j_content_ele["warning"]=GETCONTENTARR->WARNING;
				j_content.push_back(j_content_ele);
				j_content_ele["warning_log"]=GETCONTENTARR->WARNING_LOG;
				j_content.push_back(j_content_ele);
				j_content_ele["reset"]=GETCONTENTARR->RESET;
				j_content.push_back(j_content_ele);

			}
			
			j_data["content"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::PhaseSequenceTableQueryReply:
		case InsType::PhaseSequenceTableSettingReply:
		{
			j_meta["type"]=IFTYPE::PhaseSequenceTable;
			using cn_t = DATA_CONTENT::PhaseSequenceTable::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["table_number"]=GETCONTENTARR->TABLE_ID;
				j_content.push_back(j_content_ele);
				j_content_ele["loop_number"]=GETCONTENTARR->LOOP_ID;
				j_content.push_back(j_content_ele);

			}
			
			j_data["phasesequence"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::SchemaTableQueryReply:
		case InsType::SchemaTableSettingReply:
		{
			j_meta["type"]=IFTYPE::SchemaTable;
			using cn_t = DATA_CONTENT::SchemaTable::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["id"]=GETCONTENTARR->id;
				j_content.push_back(j_content_ele);
				j_content_ele["cycle_length"]=GETCONTENTARR->cycle_length;
				j_content.push_back(j_content_ele);
				j_content_ele["phase_difference"]=GETCONTENTARR->phase_difference;
				j_content.push_back(j_content_ele);
				j_content_ele["green_sig_rate_table_id"]=GETCONTENTARR->green_sig_rate_table_id;
				j_content.push_back(j_content_ele);
				j_content_ele["phase_sequence_id"]=GETCONTENTARR->phase_sequence_id;
				j_content.push_back(j_content_ele);

			}
			
			j_data["schema"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::ActionTableQueryReply:
		case InsType::ActionTableSettingReply:
		{
			using cn_t = DATA_CONTENT::ActionTable::para;
			j_meta["type"]=IFTYPE::ActionTable;
			
			j_data["version"]=GETCONTENT->action_id;
			j_data["plan_id"]=GETCONTENT->plan_id;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;

			break;
		}
		case InsType::TimeTableQueryReply:
		case InsType::TimeTableSettingReply:
		{
			j_meta["type"]=IFTYPE::TimeTable;
			using cn_t = DATA_CONTENT::TimeTable::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["time_table_id"]=GETCONTENTARR->time_table_id;
				j_content.push_back(j_content_ele);
				j_content_ele["time_segment_id"]=GETCONTENTARR->time_segment_id;
				j_content.push_back(j_content_ele);
				j_content_ele["time_segment_begin_hour"]=GETCONTENTARR->time_segment_begin_hour;
				j_content.push_back(j_content_ele);
				j_content_ele["time_segment_begin_minu"]=GETCONTENTARR->time_segment_begin_minu;
				j_content.push_back(j_content_ele);
				j_content_ele["action_id"]=GETCONTENTARR->action_id;
				j_content.push_back(j_content_ele);
				j_content_ele["failure_plan_id"]=GETCONTENTARR->failure_plan_id;
				j_content.push_back(j_content_ele);

			}
			
			j_data["table"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::FollowPhaseTableQueryReply:
		case InsType::FollowPhaseTableSettingReply:
		{
			j_meta["type"]=IFTYPE::FollowPhaseTable;
			using cn_t = DATA_CONTENT::FollowPhaseTable::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["id"]=GETCONTENTARR->id;
				j_content.push_back(j_content_ele);
				j_content_ele["type"]=GETCONTENTARR->type;
				j_content.push_back(j_content_ele);
				j_content_ele["father_phase"]=GETCONTENTARR->father_phase;
				j_content.push_back(j_content_ele);
				j_content_ele["correct_phase"]=GETCONTENTARR->correct_phase;
				j_content.push_back(j_content_ele);
				j_content_ele["green_time"]=GETCONTENTARR->green_time;
				j_content.push_back(j_content_ele);
				j_content_ele["yellow_time"]=GETCONTENTARR->yellow_time;
				j_content.push_back(j_content_ele);
				j_content_ele["red_time"]=GETCONTENTARR->red_time;
				j_content.push_back(j_content_ele);
				j_content_ele["green_flash"]=GETCONTENTARR->green_flash;
				j_content.push_back(j_content_ele);

			}
			
			j_data["table"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::UnitParameterQueryReply:
		case InsType::UnitParameterSettingReply:
		{
			using cn_t = DATA_CONTENT::UnitParameter::para;
			j_meta["type"]=IFTYPE::UnitParameter;
			
			j_data["boot_yellow"]=GETCONTENT->boot_yellow;
			j_data["boot_red"]=GETCONTENT->boot_red;
			j_data["gps_sync_time"]=GETCONTENT->gps_sync_time;
			j_data["dec_time"]=GETCONTENT->dec_time;
			j_data["speed_factor"]=GETCONTENT->speed_factor;
			j_data["communication_overtime"]=GETCONTENT->comunication_overtime;
			j_data["flash_frequence"]=GETCONTENT->flash_frequence;
			j_data["cross_street_time_lag"]=GETCONTENT->cross_street_time_lag;
			j_data["cross_reverse_street_time_lag"]=GETCONTENT->cross_reverse_street_time_lag;
			j_data["transition_cycle"]=GETCONTENT->transition_cycle;
			j_data["data_flow_collect_interval"]=GETCONTENT->data_flow_collect_interval;
			j_data["data_flow_interval_unit"]=GETCONTENT->data_flow_interval_unit;
			j_data["min_red"]=GETCONTENT->min_red;
			j_data["pedastrain_auto_clear"]=GETCONTENT->pedastrain_auto_clear;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::PedestrianDetectorQueryReply:
		case InsType::PedestrianDetectorSettingReply:
		{
			j_meta["type"]=IFTYPE::PedestrianDetector;
			using cn_t = DATA_CONTENT::PedestrianDetector::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["detector_id"]=GETCONTENTARR->detector_id;
				j_content.push_back(j_content_ele);
				j_content_ele["trigger_phase"]=GETCONTENTARR->trigger_phase;
				j_content.push_back(j_content_ele);
				j_content_ele["no_response"]=GETCONTENTARR->no_response;
				j_content.push_back(j_content_ele);
				j_content_ele["max_trigger_time"]=GETCONTENTARR->max_trigger_time;
				j_content.push_back(j_content_ele);
				j_content_ele["error_count"]=GETCONTENTARR->error_count;
				j_content.push_back(j_content_ele);
				j_content_ele["warning"]=GETCONTENTARR->warning;
				j_content.push_back(j_content_ele);

			}
			
			j_data["content"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::FailureConfigQueryReply:
		case InsType::FailureConfigSettingReply:
		{
			j_meta["type"]=IFTYPE::FailureConfig;
			using cn_t = DATA_CONTENT::FailureConfig::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["id"]=GETCONTENTARR->id;
				j_content.push_back(j_content_ele);
				j_content_ele["mode"]=GETCONTENTARR->mode;
				j_content.push_back(j_content_ele);
				j_content_ele["operation"]=GETCONTENTARR->operation;
				j_content.push_back(j_content_ele);
				j_content_ele["operation_value"]=GETCONTENTARR->operation_value;
				j_content.push_back(j_content_ele);

			}
			
			j_data["config"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		case InsType::PriorityConfigQueryReply:
		case InsType::PriorityConfigSettingReply:
		{
			j_meta["type"]=IFTYPE::PriorityConfig;
			using cn_t = DATA_CONTENT::PriorityConfig::para;
			json j_content, j_content_ele;
			
			for(int i=0;i<parser.GetContentLen()/sizeof(cn_t);i++)
			{
				/* reinterpret_cast<const cn_t*>(parser.GetContent()+i*sizeof(cn_t)); */
				j_content_ele["id"]=GETCONTENTARR->id;
				j_content.push_back(j_content_ele);
				j_content_ele["control_interface"]=GETCONTENTARR->control_interface;
				j_content.push_back(j_content_ele);
				j_content_ele["link"]=GETCONTENTARR->link;
				j_content.push_back(j_content_ele);
				j_content_ele["delay"]=GETCONTENTARR->delay;
				j_content.push_back(j_content_ele);
				j_content_ele["min_green"]=GETCONTENTARR->link;
				j_content.push_back(j_content_ele);
				j_content_ele["motor_linger"]=GETCONTENTARR->motor_linger;
				j_content.push_back(j_content_ele);
				j_content_ele["pedastrain_linger"]=GETCONTENTARR->pedastrain_linger;
				j_content.push_back(j_content_ele);
				j_content_ele["pass_time"]=GETCONTENTARR->pass_time;
				j_content.push_back(j_content_ele);
				j_content_ele["min_interval"]=GETCONTENTARR->min_interval;
				j_content.push_back(j_content_ele);
				j_content_ele["max_continue"]=GETCONTENTARR->max_continue;
				j_content.push_back(j_content_ele);
				j_content_ele["track_phase"]=GETCONTENTARR->track_phase;
				j_content.push_back(j_content_ele);
				j_content_ele["motor_linger_phase"]=GETCONTENTARR->motor_linger_phase;
				j_content.push_back(j_content_ele);
				j_content_ele["pedastrain_linger_phase"]=GETCONTENTARR->pedastrain_linger_phase;
				j_content.push_back(j_content_ele);
				j_content_ele["exit_phase"]=GETCONTENTARR->exit_phase;
				j_content.push_back(j_content_ele);
				j_content_ele["priority_status"]=GETCONTENTARR->priority_status;
				j_content.push_back(j_content_ele);

			}
			
			j_data["config"]=j_content;

			j_res["meta"]=j_meta;
			j_res["data"]=j_data;
			break;
		}
		default:
		{
			break;
		}
	}


	char task_key[20];
	snprintf(task_key, 20, "%s-%u",
			machine_conn->get_ipv4(), parser.GetInsType());
	auto task = HupTaskMap::getInstance().FindTask(task_key);
	auto res_str = j_res.dump();
	task->client_conn->get_tcp_conn()->Send(res_str.c_str(),res_str.size());
	HupTaskMap::getInstance().DropTask(task_key);
	
}

// write handlers

// error handlers
int error_fd_hdlr(const int fd);
