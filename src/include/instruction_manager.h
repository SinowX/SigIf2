// Instruction Manager
// 星志指令管理
// InsPack 打包指令，InsParser 解析指令
#ifndef INSTRUCTION_MANAGER_H
#define INSTRUCTION_MANAGER_H

#include <cstdlib>
#include <cstring>

/* #include "constants.h" */
#include "csv.h"
#include "instruction.h"
/* #include "utils.h" */

static const int MAX_BUFFER_LENGTH=1024;

class InsManager
{
  public:
    InsManager()
      :ins_type(0),sender_id(0),receiver_id(0),data_link_code(0)
    {
      reader = new csv::CSVReader("GB25280-2016.csv");

    };
    ~InsManager()
    {
      delete reader;
    }

    // 0 for success else for failure
    const int SetInsType(uint8_t instype);

    // -1(255) for failure
    const uint8_t GetSenderID() const{return sender_id;};
    const uint8_t GetReceiverID() const {return receiver_id;};
    const uint8_t GetDataLinkCode() const {return data_link_code;};
    const uint8_t GetOperationType() const {return operation_type;};
    const uint8_t GetObjectID() const {return object_id;};
    const uint8_t GetInsType(uint8_t operation, uint8_t object);

  private:
    csv::CSVReader *reader;
    char ins_name[50];
    uint8_t ins_type;
    uint8_t sender_id;
    uint8_t receiver_id;
    uint8_t data_link_code;
    uint8_t object_id;
    uint8_t operation_type;
};


class InsPack
{
  public:
    InsPack(ssize_t length=MAX_BUFFER_LENGTH)
      :buff(nullptr),buff_len(length),buff_current_len(0)
    {
      /* buff = new uint8_t[buff_len]; */
    }
    InsPack(const InsPack&) = delete; // C++11 non construction-copyable
    InsPack& operator=(const InsPack&) = delete; // C++11 non copyable
    ~InsPack()
    {
      /* delete [] buff; */
    }
    // Set() judge which type of package is to be used by arguements and set corrisponding infomation
    void Set(const uint8_t &instype, const uint8_t *reserve=RESERVE::DEFAULT,const uint8_t* content=nullptr, const ssize_t content_len=0);
    // Return Packedup instruction package, else -1 for unset state
    const uint8_t* GetBuff() const{return buff;};
    // Return instruction buff real length
    const ssize_t GetBuffLength() const{return buff_current_len;};

  private:
    uint8_t *buff;
    ssize_t buff_len;
    ssize_t buff_current_len;
    /* InsPack(const InsPack&); // non construction-copyable */
    /* InsPack& operator=(const InsPack&); // non copyable */
};

// Instruction Parser
class InsParser
{
  public:
    InsParser(ssize_t length=MAX_BUFFER_LENGTH)
      :buff(nullptr),buff_len(length),buff_current_len(0),
      SenderID(0),ReceiverID(0),AreaID(0),IntersectionID(0),
      content_len(0)
    {
      /* reader = new csv::CSVReader("./GB25280-2016.csv"); */
      memset(Reserve,0,sizeof(Reserve));
      Content = new uint8_t[buff_len];
      memset(Content,0,buff_len);
      // set it to the buffer_len to avoid segment fault
      buff = new uint8_t[buff_len];
      InsName = new char[128];
    };

    InsParser(const InsParser&) = delete;
    InsParser& operator=(const InsParser&) = delete;
    ~InsParser()
    {
      delete []buff;
      delete []Content;
      /* delete reader; */
    }
    // return Instruction Type Id
    // else return -1(65535) for error
    const uint8_t
      Parse(const uint8_t* instruction, const ssize_t length);
    const uint8_t
      GetSenderID() const {return SenderID;}
    const uint8_t
      GetReceiverID() const { return  ReceiverID;};
    const uint8_t
      GetDataLinkCode() const {return DataLinkCode;};
    const uint8_t
      GetAreaID() const { return AreaID;};
    const uint16_t
      GetInterSectionID() const { return IntersectionID;};
    const uint8_t
      GetOperationType() const { return OperationType;};
    const uint8_t
      GetObjectID() const { return ObjectId;};
    const uint8_t *GetReserve() const{ return Reserve;};
    const uint8_t *GetContent() const{ return Content;};
    const ssize_t GetContentLen() const{return content_len;};
    const char* GetInsName() const {return InsName;};
		const uint8_t GetInsType() const {return InsType;};
		inline void ShowParseInfo() const
		{
			uint8_t hex_buf;
			printf("InsName:        %s\n",GetInsName());
			printf("SenderID:       %02x\n",GetSenderID());
			printf("ReceiverID:     %02x\n",GetReceiverID());
			printf("DataLinkCode:   %02x\n",GetDataLinkCode());
			printf("AreaID:         %02x\n",GetAreaID());
			printf("IntersectionID: %04x\n",GetInterSectionID());
			printf("OperationType:  %02x\n",GetOperationType());
			printf("ObjectID:       %02x\n",GetObjectID());
			printf("Reserve:        ");
			for(int i=0;i<5;i++)
			{
				memcpy(&hex_buf,GetReserve()+i, 1);
				printf("%02x",hex_buf);
			}
			printf("\n");

		}
    // int (&arg)[4]
  private:
    // stored infomation
    uint8_t InsType;
    uint8_t ObjectId;
    uint8_t OperationType;
    uint8_t SenderID;
    uint8_t ReceiverID;
    uint8_t DataLinkCode;
    uint8_t AreaID;
    uint16_t IntersectionID;
    uint8_t Reserve[5];
    uint8_t *Content;
    ssize_t content_len;
    char *InsName;
    // uint8_t typeID;
    // stored buff
    uint8_t *buff;
    ssize_t buff_len;
    ssize_t buff_current_len;
    csv::CSVReader *reader;
};







#endif
