#ifndef INSTRUCTION_H
#define INSTRUCTION_H
// instruction.h
// This file declares Instruction Constances accordding to
// GB25280-2016 doc provided by XingZhi.
// Also declares InsPack Class for Set() and Get() Instruction Package.

#include <cstdint>
/* #include <type_traits> */

enum InsType:uint8_t
{
OnlineRequest
,OnlineRequestReply
,OnlineQuery
,OnlineQueryReply
,TrafficInfoUpload
,WorkStatusQuery
,WorkStatusQueryReply
,WorkStatusUpload
,LightStatusQuery
,LightStatusQueryReply
,LightStatusUpload
,TimeQuery
,TimeQueryReply
,TimeSetting
,TimeSettingReply
,SemaphoreGroupQuery
,SemaphoreGroupQueryReply
,SemaphoreGroupSetting
,SemaphoreGroupSettingReply
,PhaseQuery
,PhaseQueryReply
,PhaseSetting
,PhaseSettingReply
,TimingSchemeQuery
,TimingSchemeQueryReply
,TimingSchemeSetting
,TimingSchemeSettingReply
,ScheduleQuery
,ScheduleQueryReply
,ScheduleSetting
,ScheduleSettingReply
,WorkModeQuery
,WorkModeQueryReply
,WorkModeSetting
,WorkModeSettingReply
,FailureQuery
,FailureQueryReply
,FailureUpload
,VersionQuery
,VersionQueryReply
,VersionUpload
,CharacterParameterVersionQuery
,CharacterParameterVersionQueryReply
,CharacterParameterVersionSetting
,CharacterParameterVersionSettingReply
,IdentificationCodeQuery
,IdentificationCodeQueryReply
,RemoteControlSetting
,RemoteControlSettingReply
,DetectorQuery
,DetectorQueryReply
,DetectorSetting
,DetectorSettingReply
,PhaseSequenceTableQuery
,PhaseSequenceTableQueryReply
,PhaseSequenceTableSetting
,PhaseSequenceTableSettingReply
,SchemaTableQuery
,SchemaTableQueryReply
,SchemaTableSetting
,SchemaTableSettingReply
,ActionTableQuery
,ActionTableQueryReply
,ActionTableSetting
,ActionTableSettingReply
,TimeTableQuery
,TimeTableQueryReply
,TimeTableSetting
,TimeTableSettingReply
,FollowPhaseTableQuery
,FollowPhaseTableQueryReply
,FollowPhaseTableSetting
,FollowPhaseTableSettingReply
,UnitParameterQuery
,UnitParameterQueryReply
,UnitParameterSetting
,UnitParameterSettingReply
,PedestrianDetectorQuery
,PedestrianDetectorQueryReply
,PedestrianDetectorSetting
,PedestrianDetectorSettingReply
,FailureConfigQuery
,FailureConfigQueryReply
,FailureConfigSetting
,FailureConfigSettingReply
,PriorityConfigQuery
,PriorityConfigQueryReply
,PriorityConfigSetting
,PriorityConfigSettingReply
};

const uint8_t FRAME_BEGIN=0xc0;
const uint8_t VERSION_CODE=0x10;
namespace SENDER_ID
{
	const uint8_t SIGNAL=0x10;
	const uint8_t SERVER=0x20;
};
namespace RECEIVER_ID
{
	const uint8_t SIGNAL=0x10;
	const uint8_t SERVER=0x20;
};
namespace DATA_LINK_CODE
{
	const uint8_t ONLINE=0x01;
	const uint8_t TRAFFIC=0x02;
	const uint8_t WORK_STATUS=0x02;
	const uint8_t LIGHT_STATUS=0x02;
	const uint8_t TIME=0x02;
	const uint8_t SEMAPHORE_GROUP=0x03;
	const uint8_t PHASE=0x03;
	const uint8_t TIMING_SCHEME=0x03;
	const uint8_t SCHEDULE=0x03;
	const uint8_t WORK_MODE=0x04;
	const uint8_t FAILURE=0x02;
	const uint8_t VERSION=0x02;
	const uint8_t CHARACTER_PARAMETER_VERSION=0x04;
	const uint8_t IDENTIFICATION_CODE=0x04;
	const uint8_t REMOTE_CONTROL=0x04;
	const uint8_t DETECTOR=0x04;
	const uint8_t PHASE_SEQUENCE_TABLE=0x03;
	const uint8_t SCHEMA_TABLE=0x03;
	const uint8_t ACTION_TABLE=0x03;
	const uint8_t TIME_TABLE=0x03;
	const uint8_t FOLLOW_PHASE_TABLE=0x03;
	const uint8_t UNIT_PARAMETER=0x03;
	const uint8_t PEDESTRIAN_DETECTOR=0x03;
	const uint8_t FAILURE_CONFIG=0x03;
	const uint8_t PRIORITY_CONFIG=0x03;
};
const uint8_t AREA_ID=0x00;
const uint16_t INTERSECTION_ID=0x0001;

namespace OPERATION_TYPE
{
	const uint8_t QUERY=0x80;
	const uint8_t QUERY_REPLY=0x83;
	const uint8_t SETTING=0x81; // including Online Request
	const uint8_t SETTING_REPLY=0x84; // including Online Reply
	const uint8_t UPLOAD=0x82;
};

namespace OBJECT_ID
{
	const uint8_t ONLINE=0x01;
	const uint8_t TRAFFIC=0x02;
	const uint8_t WORK_STATUS=0x03;
	const uint8_t LIGHT_STATUS=0x04;
	const uint8_t TIME=0x05;
	const uint8_t SEMAPHORE_GROUP=0x06;
	const uint8_t PHASE=0x07;
	const uint8_t TIMING_SCHEME=0x08;
	const uint8_t SCHEDULE=0x09;
	const uint8_t WORK_MODE=0x0a;
	const uint8_t FAILURE=0x0b;
	const uint8_t VERSION=0x0c;
	const uint8_t CHARACTER_PARAMETER_VERSION=0x0d;
	const uint8_t IDENTIFICATION_CODE=0x0e;
	const uint8_t REMOTE_CONTROL=0x0f;
	const uint8_t DETECTOR=0x10;
	const uint8_t PHASE_SEQUENCE_TABLE=0x20;
	const uint8_t SCHEMA_TABLE=0x21;
	const uint8_t ACTION_TABLE=0x22;
	const uint8_t TIME_TABLE=0x23;
	const uint8_t FOLLOW_PHASE_TABLE=0x24;
	const uint8_t UNIT_PARAMETER=0x25;
	const uint8_t PEDESTRIAN_DETECTOR=0x26;
	const uint8_t FAILURE_CONFIG=0x27;
	const uint8_t PRIORITY_CONFIG=0x28;
};

namespace RESERVE
{
	uint8_t DEFAULT[5]={0x01,0x01,0x01,0x01,0x01};
	uint8_t SETTING[5]={0x05,0x04,0x03,0x02,0x01};
};


const uint16_t INSTRUCTION_MINIMAL_SIZE=17;

const uint8_t FRAME_CHECK=0x00;
const uint8_t FRAME_END=0xc0;

namespace DATA_CONTENT
{
	namespace TrafficInfo{
		typedef struct{
			uint8_t detector_id;
			uint16_t detect_interval;
			uint16_t data_flow;
			uint8_t occupation;
		}__attribute__((packed)) para; // 6 bytes

	}
	namespace WorkStatus{
		typedef struct{
			uint8_t manual_auto;
			uint8_t work_mode;
			uint8_t plan_id;
			uint8_t scheme_id;
			uint8_t phase_sqe_id;
			uint8_t green_sig_rate_id;
		}__attribute__((packed)) para; // 6 bytes

	}
	namespace LightStatus{
		typedef struct{
			uint16_t light_1;
			uint16_t light_2;
			uint16_t light_3;
			uint16_t light_4;
			uint16_t light_5;
			uint16_t light_6;
		}__attribute__((packed)) para; // 12 bytes
	}
	namespace Time{
		typedef uint32_t para; // 4 bytes timestamp
	}
	namespace SemaphoreGroup{
		typedef struct{
			uint8_t grp_id;
			uint8_t ctrl_type;
			uint8_t ctrl_id;
			uint8_t flash;
			uint8_t brightness;
			uint16_t location_x;
			uint16_t location_y;
			uint8_t timer_id;
			uint16_t reserved;
		}__attribute__((packed)) para; // 12 bytes

	}
	namespace Phase{
		typedef struct{ // problem
			uint8_t phase_id;
			uint8_t pedastrain_pass;
			uint8_t pedastrain_clear;
			uint8_t min_green;
			uint8_t lag_green;
			uint8_t max_green_1;
			uint8_t max_green_2;
			uint8_t yellow_transition;
			uint8_t red_clear;
			uint8_t protection;
			uint8_t inc_init;
			uint8_t inc_max;
			uint8_t dec_previous_time;
			uint8_t dec_previous_traffic;
			uint8_t dec_time;
			uint8_t dec_rate;
			uint8_t min_interval;
			uint8_t dynamic_max;
			uint8_t dynamic_step;
			uint8_t initial_para;
			uint8_t option;
			uint8_t loop_id;
			uint8_t cocurrency_phase;
		}__attribute__((packed)) para; // 24 bytes

	}
	namespace TimingScheme{
		typedef struct{
			uint8_t time_id;
			uint8_t phase;
			uint8_t time_len;
			uint8_t mode;
			uint8_t option;
			uint64_t reserved_1; // padding 8 bytes
			uint64_t reserved_2; // padding 8 bytes
			uint16_t reserved_3; // padding 2 bytes
			uint8_t reserved_4; // padding 1 bytes
		}__attribute__((packed)) para; // 24 bytes

	}
	namespace Schedule{
		typedef struct{
			// problem
			uint8_t plan_id;
			uint16_t month_data;
			uint8_t week_data;
			uint32_t day_data;
			uint8_t time_schedule_id;
		}__attribute__((packed)) para; // 

	}
	namespace WorkMode{
		typedef  uint8_t para; // where is values ?
	}
	namespace Failure{
		typedef struct{
			uint8_t type;
			uint64_t time;
			uint32_t content_1;
			uint16_t content_2;
			uint8_t content_3;
		}__attribute__((packed)) para; // 12 bytes

	}
	namespace Version{
		typedef struct
		{
			char info[21]; // 20 bytes cstring
		}para;
		/* typedef char[20] para; */
	}
	namespace CharacterParameterVersion{
			typedef uint8_t para; // 1 byte
	}
	namespace IdentificationCode{
		typedef struct{
		//problem
		}__attribute__((packed)) para;

	}
	namespace RemoteControl{
		typedef uint8_t para; // 1 byte
		namespace CMD_VALUE{
			const uint8_t REBOOT=0;
			const uint8_t CANCEL_MANUAL=16;
			const uint8_t PHASE_LINGER_1=17;
			const uint8_t PHASE_LINGER_2=18;
			const uint8_t PHASE_LINGER_3=19;
			const uint8_t PHASE_LINGER_4=20;
			const uint8_t STEP_IN=31;
			const uint8_t PRIORITY_TRIGGER__MIN=33; // what is 1-16 mentioned in doc
			const uint8_t PRIORITY_TRIGGER__MAX=48;
			const uint8_t PHASE_SECRET__MIN=129;
			const uint8_t PHASE_SECRET__MAX=160;
			const uint8_t SENSE_CHANNEL__MIN=192;
			const uint8_t SENSE_CHANNEL__MAX=240;
		}	
	}
	namespace Detector{
		typedef struct{
			uint8_t ID;
			uint8_t OPTION;
			uint8_t OPEN_PHASE;
			uint8_t CLOSE_PHASE;
			uint8_t DELAY_TIME;
			uint8_t DELAY;
			uint8_t QUEUE_LIMIT;
			uint8_t NO_RESPONSE;
			uint8_t MAX_EXSIT;
			uint8_t ERROR_COUNT;
			uint8_t FALURE_TIME;
			uint8_t WARNING;
			uint8_t WARNING_LOG;
			uint8_t RESET;
			uint8_t RESERVED; // reserved ? space in doc
		}__attribute__((packed)) para; // 15 bytes

	}
	namespace PhaseSequenceTable{
		typedef struct{
			uint8_t TABLE_ID;
			uint8_t LOOP_ID;
			/* uint64_t PHASE_SEQUENCE_1; */
			/* uint64_t PHASE_SEQUENCE_2; */
			/* uint32_t PHASE_SEQUENCE_3; */
			/* uint16_t PHASE_SEQUENCE_4; */
			uint8_t PHASE_SEQUENCE[22];
		}__attribute__((packed)) para; // 24 bytes

	}
	namespace SchemaTable{
		typedef struct{
			// doc problem
			uint8_t id;
			uint16_t cycle_length;
			uint8_t phase_difference;
			uint8_t green_sig_rate_table_id;
			uint8_t phase_sequence_id;
		}__attribute__((packed)) para;

	}
	namespace ActionTable{
		typedef struct{
			uint8_t action_id;
			uint8_t plan_id;
			uint8_t assist_function_1;
			uint8_t assist_function_2;
			uint16_t reserved;
		}__attribute__((packed)) para; // 6 bytes

	}
	namespace TimeTable{
		typedef struct{
			// doc problem
			uint8_t time_table_id;
			uint8_t time_segment_id;
			uint8_t time_segment_begin_hour;
			uint8_t time_segment_begin_minu;
			uint8_t action_id;
			uint8_t failure_plan_id;
		}__attribute__((packed)) para;

	}
	namespace FollowPhaseTable{
		typedef struct{
			// doc problem
			uint8_t id;
			uint8_t type;
			uint32_t father_phase;
			uint32_t correct_phase;
			uint8_t green_time;
			uint8_t yellow_time;
			uint8_t red_time;
			uint8_t green_flash;
			uint8_t reserve[10];
		}__attribute__((packed)) para;

	}
	namespace UnitParameter{
		typedef struct{
			uint8_t boot_yellow;
			uint8_t boot_red;
			uint8_t gps_sync_time;
			uint8_t dec_time;
			uint8_t speed_factor;
			uint8_t comunication_overtime;
			uint8_t flash_frequence;
			uint8_t cross_street_time_lag;
			uint8_t cross_reverse_street_time_lag;
			uint8_t transition_cycle;
			uint8_t data_flow_collect_interval;
			uint8_t data_flow_interval_unit;
			uint8_t min_red;
			uint8_t pedastrain_auto_clear;
			/* uint64_t reserved_1; */
			/* uint16_t reserved_2; */
			uint8_t reserve[10];
		}__attribute__((packed)) para; // 24 bytes

	}
	namespace PedestrianDetector{
		typedef struct{
			uint8_t detector_id;
			uint8_t trigger_phase;
			uint8_t no_response;
			uint8_t max_trigger_time;
			uint8_t error_count;
			uint8_t warning;
		}__attribute__((packed)) para; //6 bytes

	}
	namespace FailureConfig{
		typedef struct{
			uint8_t id;
			uint8_t mode;
			uint8_t operation;
			uint8_t operation_value;
		}__attribute__((packed)) para; //4 bytes

	}
	namespace PriorityConfig{
		typedef struct{
			uint8_t id;
			uint8_t control_interface;
			uint8_t link;
			uint8_t delay;
			uint8_t min_existence;
			uint8_t min_green;
			uint8_t motor_linger;
			uint8_t pedastrain_linger;
			uint8_t pass_time;
			uint8_t min_interval;
			uint8_t max_continue;
			uint16_t track_phase;
			uint16_t motor_linger_phase;
			uint16_t pedastrain_linger_phase;
			uint16_t exit_phase;
			uint8_t priority_status;
			uint64_t reserved;
		}__attribute__((packed)) para; //24 bytes

	}
}


typedef struct{
	uint8_t frame_begin;
	uint8_t version_code;
	uint8_t sender_id;
	uint8_t receiver_id;
	uint8_t data_link_code;
	uint8_t area_id;
	uint16_t intersection_id;
	uint8_t operation_type;
	uint8_t object_id;
	uint8_t reserve[5];
}__attribute__((packed)) frame_before_content;

typedef struct
{
	uint8_t frame_check;
	uint8_t frame_end;
}__attribute__((packed)) frame_after_content;


#endif
