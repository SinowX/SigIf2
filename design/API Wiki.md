## API Wiki

### Basic Data Structure

#### Query

| Argument | Type   | Description      | example |
| -------- | ------ | ---------------- | ------- |
| meta     | Object | see detail below |         |



#### Query Response

| Argument | Type   | Description                 | example |
| -------- | ------ | --------------------------- | ------- |
| code     | Number | -1 for error, 0 for success |         |
| meta     | Object | see detail below            |         |
| data     | Object | see detail below            |         |



#### Setting

| Argument | Type   | Description      | example |
| -------- | ------ | ---------------- | ------- |
| meta     | Object | see detail below |         |
| data     | Object | see detail below |         |



#### Setting Response

| Argument | Type   | Description                 | example |
| -------- | ------ | --------------------------- | ------- |
| code     | Number | -1 for error, 0 for success |         |
| meta     | Object | see detail below            |         |
| data     | Object | see detail below            |         |



---

#### meta structure

| Argument | Type   | Description                                           | example         |
| -------- | ------ | ----------------------------------------------------- | --------------- |
| action   | String | range: "query", "query_res", "setting", "setting_res" |                 |
| type     | String | see rangement below                                   |                 |
| ipv4     | String | dot divided IPv4 address                              | "192.168.1.102" |



#### type rangment

```
ActionTable
CharacterParameterVersion
Detector
FailureConfig
Failure
FollowPhaseTable
IdentificationCode
LightStatus
PedestrianDetector
Phase
PhaseSequenceTable
PriorityConfig
RemoteControl
Schedule
SchemaTable
SemaphoreGroup
Time
TimeTable
TimingScheme
TrafficInfo
UnitParameter
Version
WorkMode
WorkStatus
```



### Data Structure

- Distinct by each type, see examples below


#### ActionTable
```json
"data":{
    "action_id":45,
    "plan_id":12
}
```
#### CharacterParameterVersion
```json
"data":{
    "version":213
  }
```
#### Detector
```json
"data":{
    "content":[
      {
        "id":23,
        "option":54,
        "open_phase":31,
        "close_phase":54,
        "delay_time":56,
        "delay":76,
        "queue_limit":12,
        "no_response":78,
        "max_exsit":23,
        "error_count":54,
        "failure_time":23,
        "warning":23,
        "warning_log":32,
        "reset":45
      }
    ]
  }
```
#### FailureConfig
```json
"data":{
    "config":[
      {
        "id":12,
        "mode":32,
        "operation":23,
        "operation_value":34
      }
    ]
  }
```
#### Failure
```json
"data":{
    "failure":[
      {
        "type":23,
        "timestamp":45421232,
        "content":3433212
      }
    ]
  }
```
#### FollowPhaseTable
```json
"data":{
    "table":[
      {
        "id":1,
        "type":32,
        "father_phase":53,
        "correct_phase":12,
        "green_time":32,
        "yellow_time":12,
        "red_time":34,
        "green_flash":21
      }
    ]
  }
```
#### IdentificationCode
```json
```
#### LightStatus
```json
"data":{
      "light_group":[
        {
        "status":0 // off
        },
        {
        "status":1 // red
        },
        {
        "status":2 // yellow
        },
        {
        "status":3 // green
        }
      ]
    }
```
#### PedestrianDetector
```json
"data":{
    "content":[
      {
        "detector_id":1,
        "trigger_phase":3,
        "no_response":1,
        "max_trigger_time":34,
        "error_count":32,
        "warning":21
      }
    ]
  }
```
#### Phase
```json
"data":{
    "status":[
      {
        "phase_id":43,
        "pedastrain_pass":65,
        "pedastrain_clear":54,
        "min_green":23,
        "lag_green":21,
        "max_green_1":36,
        "max_green_2":76,
        "yellow_transition":23,
        "red_clear":65,
        "protection":87,
        "inc_init":54,
        "inc_max":76,
        "dec_previous_time":32,
        "dec_previous_traffic":87,
        "dec_time":32,
        "dec_rate":76,
        "min_interval":65,
        "dynamic_max":43,
        "dynamic_step":99,
        "initial_para":33,
        "option":44,
        "loop_id":11,
        "cocurrency_phase":23
      }
    ]
  }
```
#### PhaseSequenceTable
```json
"data":{
    "phasesequence":[
      {
        "table_number":1,
        "loop_number":12
      }
    ]
  }
```
#### PriorityConfig
```json
"data":{
    "config":[
      {
        "id":1,
        "control_interface":32,
        "link":54,
        "delay":23,
        "min_existence":56,
        "min_green":56,
        "motor_linger":32,
        "pedastrain_linger":12,
        "pass_time":32,
        "min_interval":3,
        "max_continue":32,
        "track_phase":21,
        "motor_linger_phase":21,
        "pedastrain_linger_phase":23,
        "exit_phase":12,
        "priority_status":32
      }
    ]
  }
```
#### RemoteControl
```json
"data":{
    "control_type":16
  }
```
#### Schedule
```json
"data":{
    "plan":[
      {
        "plan_id":1,
        "month_data":43,
        "week_data":53,
        "day_data":343,
        "time_schedule_number":43
      }
    ]
  }
```
#### SchemaTable
```json
"data":{
    "schema":[
      {
        "id":31,
        "cycle_length":431,
        "phase_difference":23,
        "green_sig_rate_table_id":432,
        "phase_sequence_id":32
      }
    ]
  }
```
#### SemaphoreGroup
```json
"data":{
    "group":[
      {
        "group_number":23,
        "control_type":54,
        "control_number":65,
        "flash":64,
        "brightness":56,
        "location_x":4532,
        "location_y":3542,
        "timer_number":23
      }
    ]
  }
```
#### Time
```json
"data":{
    "timestamp":2194832
  }
```
#### TimeTable
```json
"data":{
    "table":[
      {
        "time_table_id":12,
        "time_segment_id":54,
        "time_segment_begin_hour":332,
        "time_segment_begin_minu":23,
        "action_id":32,
        "failure_plan_id":123
      }
    ]
  }
```
#### TimingScheme
```json
"data":{
    "table":[
      {
        "time_table_id":12,
        "time_segment_id":54,
        "time_segment_begin_hour":332,
        "time_segment_begin_minu":23,
        "action_id":32,
        "failure_plan_id":123
      }
    ]
  }
```
#### TrafficInfo
```json
"data":{
    "detector":[
      {
        "detector_id": 1,
        "detect_interval": 30,
        "flow_data": 60,
        "occupation": 80
      },
      {
        "detector_id": 2,
        "detect_interval": 30,
        "flow_data": 60,
        "occupation": 80
      },
      {
        "detector_id": 3,
        "detect_interval": 30,
        "flow_data": 60,
        "occupation": 80
      }
    ]
  }
```
#### UnitParameter
```json
"data":{
      "boot_yellow":1,
      "boot_red":0,
      "gps_sync_time":30,
      "dec_time":34,
      "speed_factor":312,
      "communication_overtime":23,
      "flash_frequence":34,
      "cross_street_time_lag":23,
      "cross_reverse_street_time_lag":43,
      "transition_cycle":43,
      "data_flow_collect_interval":12,
      "data_flow_interval_unit":32,
      "min_red":43,
      "pedastrain_auto_clear":1
  }
```
#### Version
```json
"data":{
    "version":""
  }
```
#### WorkMode
```json
"data":{
    "constant_cycle":1,
    "reaction":0,
    "mannual":1,
    "off":0,
    "yellow_flash":0,
    "phase_lock":1,
    "certain_phase":0
  }
```
#### WorkStatus
```json
"data":{
     "is_auto": 1,
     "workmode": 12,
     "plan_number": 42,
     "scheme":32,
     "green_signal_number":32
}
```

