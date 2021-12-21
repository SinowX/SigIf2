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
typedef struct MachineInfo
{
  char ipv4[16];
	uint16_t port;
  int id;
  struct MachineInfo *next;
}MachineInfo;


//
typedef struct EachMachineType
{
  int number;
  MachineInfo* head;
}EachMachineType;

//
typedef struct
{
  EachMachineType XINGZHI;
  EachMachineType HAIXIN;
  EachMachineType HAIKANG;
  EachMachineType XIMENZI;
  EachMachineType DEVICE5;
}Machine;


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

namespace IFACTION{
	const char* Query="query";
	const char* QueryRes="query_res";
	const char* Setting="setting";
	const char* SettingRes="setting_res";
};


#endif
