# 项目--Signal Machine Interface

## 运行及编译环境

### 开源库：

- [nlohmann/json](https://github.com/nlohmann/json)
- [csv-parser](https://github.com/vincentlaucsb/csv-parser)
- [inih](https://github.com/benhoyt/inih)

### 编译平台:

- GNU/Linux amd64
- 

## IO模型及线程描述

- 本项目采用Reactor多线程模型，一个Leader线程，一个Acceptor线程，一个或多个IOReceiver线程，多个Worker线程，多个IOSender，多个HBKeeper线程

### Leader线程
- 创建Acceptor、IOReceiver、Worker、IOSender，HBKeeper线程
- 创建完所有线程后可Idle（目前）或者作为日志线程

### Acceptor线程
- 监听服务器端口，接受来自客户端的TCP连接
- 将套接字封装后发送给某个IOReceiver

### IOReceiver线程
- 利用epoll调用监听所有来自Acceptor和与信号机建立的连接
- epoll检测到events之后，读取连接中的内容，将对应的连接及相关信息打包成TaskInfo，追加到TaskQ队列

### Worker线程
- 从TaskQ队列获取TaskInfo，处理其中的buffer，由于是采用Json接口，分包没有问题，当遇到了半包的情况，则将其遗留在buffer中，等待下一次IOReceiver读取成功
- 对于每一个任务，Worker对其解析：
  - 如果是来自后端的请求，则将对应请求发送到信号机，然后添加到HupTaskMap
  - 如果是来自信号机的信息且不是HeartBeat包，则查找HupTaskMap，将其发送回对应的请求端
  - 如果是来自信号机的HeartBeat包，则更新对应的LastHeartBeat

### HBKeeper线程
- 即 HeartBeatKeeper
- 一种信号机有对应一个HBKeeper
- 每隔一定时间对所有的信号机都发送一个HeartBeat包
- 若发现LastHeartBeat距离此刻时间长过一个阈值，则向所有后端发送Warning消息



## 接口描述

- 接口采用json格式

- ```json
  {
  	"meta":{
  		"type":"SemaphoreGroup",
  		"action":"response",
  		"ipv4":""
  	},
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
  }
  ```

- 接口分为两个部分，"meta"与"data"

- 其中"meta"包含3项数据

  - "type": 指令类型，取值范围见[variant_const.h](src/include/variant_const.h)中的IFTYPE，字符串
  - "action": 操作类型，取值范围"query"、"query_res"、"setting"、"setting_res"，字符串
  - "ipv4": 信号机IPv4地址，字符串

- 其中"data"包含1项数据，详见[design/interface/](design/interface)中的示例，各字段的详细含义参考见[GB 25280-2016 20171128_山东星志机器端协议.pdf](design/GB 25280-2016 20171128_山东星志机器端协议.pdf)

## 其余内容见代码注释