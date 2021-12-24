#ifndef VARIANT_CONST_H
#define VARIANT_CONST_H
#include <cstdint>
//
enum MachineType
{
  XINGZHI,
  HAIXIN,
  HAIKANG,
  XIMENZI,
  DEVICE5
};

//

// 接口指令类型字符串，用于匹配客户端的json请求
namespace  IFTYPE{
  const char* ActionTable="ActionTable";
  const char* CharacterParameterVersion="CharacterParameterVersion";
  const char* Detector="Detector";
  const char* FailureConfig="FailureConfig";
  const char* Failure="Failure";
  const char* FollowPhaseTable="FollowPhaseTable";
  const char* IdentificationCode="IdentificationCode";
  const char* LightStatus="LightStatus";
  const char* PedestrianDetector="PedestrianDetector";
  const char* Phase="Phase";
  const char* PhaseSequenceTable="PhaseSequenceTable";
  const char* PriorityConfig="PriorityConfig";
  const char* RemoteControl="RemoteControl";
  const char* Schedule="Schedule";
  const char* SchemaTable="SchemaTable";
  const char* SemaphoreGroup="SemaphoreGroup";
  const char* Time="Time";
  const char* TimeTable="TimeTable";
  const char* TimingScheme="TimingScheme";
  const char* TrafficInfo="TrafficInfo";
  const char* UnitParameter="UnitParameter";
  const char* Version="Version";
  const char* WorkMode="WorkMode";
  const char* WorkStatus="WorkStatus";
};

// 接口操作类型字符串，用于匹配客户端、写回客户端的json请求
namespace IFACTION{
	const char* Query="query";
	const char* QueryRes="query_res";
	const char* Setting="setting";
	const char* SettingRes="setting_res";
};


#endif
