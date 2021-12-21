#include "instruction_manager.h"

const int InsManager::SetInsType(uint8_t instype)
{
	for(csv::CSVRow& row: *reader)
	{
		if(row["Number"].get<uint8_t>()==instype)
		{
			strcpy(ins_name,row["InsType"].get<>().c_str());
			sender_id=static_cast<uint8_t>(std::stoul(
						row["SenderID"].get<>(),nullptr,16
						));
			receiver_id=static_cast<uint8_t>(std::stoul(
						row["ReceiverID"].get<>(),nullptr,16
						));
			data_link_code=static_cast<uint8_t>(std::stoul(
						row["DataLinkCode"].get<>(),nullptr,16
						));
			object_id=static_cast<uint8_t>(std::stoul(
						row["ObjectID"].get<>(),nullptr,16
						));
			operation_type=static_cast<uint8_t>(std::stoul(
						row["OperationType"].get<>(),nullptr,16
						));
			ins_type=instype;
			
			return 0;
		}
	}
	return -1;
	// catch exception to be done
};

const uint8_t InsManager::GetInsType(uint8_t operation, uint8_t object)
{
	for(csv::CSVRow& row: *reader)
	{
		if(row["ObjectID"].get<uint8_t>()==object &&
				row["OperationType"].get<uint8_t>()==operation)
		{
			strcpy(ins_name,row["InsType"].get<>().c_str());
			sender_id=static_cast<uint8_t>(std::stoul(
						row["SenderID"].get<>(),nullptr,16
						));
			receiver_id=static_cast<uint8_t>(std::stoul(
						row["ReceiverID"].get<>(),nullptr,16
						));
			data_link_code=static_cast<uint8_t>(std::stoul(
						row["DataLinkCode"].get<>(),nullptr,16
						));
			object_id=object;
			operation_type=operation;
			ins_type=static_cast<uint8_t>(std::stoul(
						row["DataLinkCode"].get<>(),nullptr,16
						));
		}
	}
	return ins_type;
}


void InsPack::Set(const uint8_t &instype, const uint8_t *reserve, const uint8_t *content, const ssize_t content_len)
{
	buff = new uint8_t[MAX_BUFFER_LENGTH];
	InsManager handle;
	handle.SetInsType(instype);
	//set frame_begin
	memcpy(buff, &FRAME_BEGIN, sizeof(uint8_t));
	buff_current_len=sizeof(uint8_t);
	//set version_code
	memcpy(buff+buff_current_len,&VERSION_CODE ,sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set sender_id
	uint8_t tmp = handle.GetSenderID();
	memcpy(buff+buff_current_len,&tmp,sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set receiver_id
	tmp = handle.GetReceiverID();
	memcpy(buff+buff_current_len,&tmp,sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set data_link_code
	tmp = handle.GetDataLinkCode();
	memcpy(buff+buff_current_len,&tmp,sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set area_id
	memcpy(buff+buff_current_len,&AREA_ID,sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set intersection_id
	memcpy(buff+buff_current_len,&INTERSECTION_ID,sizeof(uint16_t));
	buff_current_len+=sizeof(uint16_t);
	//set operation_type
	tmp = handle.GetOperationType();
	memcpy(buff+buff_current_len,&tmp,sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set object_id
	tmp = handle.GetObjectID();
	memcpy(buff+buff_current_len,&tmp,sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set reserve
	memcpy(buff+buff_current_len,reserve,5*sizeof(uint8_t));
	buff_current_len+=5*sizeof(uint8_t);
	//set content
	memcpy(buff+buff_current_len, content, content_len);	
	buff_current_len+=content_len;
	//set frame_check
	memcpy(buff+buff_current_len, &FRAME_CHECK, sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	//set frame_end
	memcpy(buff+buff_current_len, &FRAME_END, sizeof(uint8_t));
	buff_current_len+=sizeof(uint8_t);
	return;	
}



const uint8_t 
InsParser::Parse(const uint8_t *instruction, const ssize_t length)
{
	// read  instruction front and end parts
	// check instruction healthy state by 
	// checking length if not less than min size,
	//  then FrameBegin, Version, FrameCheck 
	//  and FrameEnd if equal the const values, then SenderID, ReceiverID, AreaID, IntersectionID if reasonable, 
	// distinguish instruction type by ObjectID and OperationType
	// then verificate it by DataLinkCode
	// if no error, we know which Instruction it is,
	// store above info and return its Instruction ID
	/* std::cout<<"InsParser: get length "<<length<<std::endl; */
	if(length<INSTRUCTION_MINIMAL_SIZE) return -1;

	frame_before_content front_checker;
	frame_after_content back_checker;
	/* printf("%p",instruction); */

	/* uint8_t hex_buf; */
	/* for(int i=0;i<length;i++) */
      /* { */
        /* memcpy(&hex_buf, instruction+i, 1); */
        /* printf("%02x",hex_buf); */
      /* } */
	/* std::cout<<std::endl; */	
	memcpy(&front_checker, instruction, sizeof(front_checker));
	/* for(int i=0;i<sizeof(front_checker);i++) */
      /* { */
        /* memcpy(&hex_buf, &front_checker+i, 1); */
        /* printf("%02x",hex_buf); */
      /* } */
	/* printf("%02x",front_checker.frame_begin); */
	/* printf("%02x",front_checker.version_code); */
	/* printf("%02x",front_checker.sender_id); */
	/* printf("%02x",front_checker.receiver_id); */
	/* printf("%02x",front_checker.data_link_code); */
	/* printf("%02x",front_checker.area_id); */
	/* printf("%04x",front_checker.intersection_id); */
	/* printf("%02x",front_checker.operation_type); */
	/* printf("%02x",front_checker.object_id); */

	/* std::cout<<std::endl; */	
	/* printf("sizeof back_checker: %ld\n",sizeof(back_checker)); */
	memcpy(&back_checker, instruction+length-sizeof(back_checker), sizeof(back_checker));
	/* for(int i=0;i<sizeof(back_checker);i++) */
      /* { */
        /* memcpy(&hex_buf, &back_checker+i, 1); */
        /* printf("%02x",hex_buf); */
      /* } */	
	/* printf("%02x",back_checker.frame_check); */
	/* printf("%02x",back_checker.frame_end); */
	/* std::cout<<std::endl; */	
	if(memcmp(&front_checker.frame_begin,&FRAME_BEGIN,1)!=0) return -1;
	/* std::cout<<"1"<<std::endl; */
	if(memcmp(&front_checker.version_code,&VERSION_CODE,1)!=0) return -1;
	/* std::cout<<"2"<<std::endl; */
	/* printf("%02x\n",back_checker.frame_check); */
	if(memcmp(&back_checker.frame_check,&FRAME_CHECK,1)!=0) return -1;
	/* std::cout<<"3"<<std::endl; */
	/* printf("%02x\n",back_checker.frame_end); */
	if(memcmp(&back_checker.frame_end,&FRAME_END,1)!=0) return -1;
	/* std::cout<<"4"<<std::endl; */
	/* std::cout<<"InsParser: object id "<<front_checker.object_id<<std::endl; */
	/* std::cout<<"InsParser: operation_type "<<front_checker.operation_type<<std::endl; */
	uint8_t row_objid, row_opeid;
	/* printf("Packet ObjectId: %02x ; OperationType: %02x\n", */
	/* 		front_checker.object_id,front_checker.operation_type); */
	reader = new csv::CSVReader("./GB25280-2016.csv");
	for(csv::CSVRow& row: *reader)
	{
			/* printf("here0\n"); */
			/* printf("ObjectID:%02x\n",front_checker.object_id); */
			/* printf("ObjectID:%s\n",row["DataLinkCode"].get<>()); */
		/* std::cout<<row["AreaID"].get<>()<<std::endl; */
		row_objid = static_cast<uint8_t>(std::stoul(row["ObjectID"].get<>(),nullptr,16));
		row_opeid = static_cast<uint8_t>(std::stoul(row["OperationType"].get<>(),nullptr,16));

		/* printf("This Row ObjecID: %02x; OperationType: %02x\n",row_objid,row_opeid); */
		if(row_objid==front_checker.object_id
				&& row_opeid==front_checker.operation_type)
		{
			/* printf("This Row ObjecID: %02x; OperationType: %02x\n",row_objid,row_opeid); */
			strcpy(InsName,row["InsType"].get<>().c_str());
			/* printf("%s\n",InsName); */
			InsType=row["Number"].get<uint8_t>();
			ObjectId=front_checker.object_id;
			OperationType=front_checker.operation_type;
			SenderID=front_checker.sender_id;
			ReceiverID=front_checker.receiver_id;
			DataLinkCode=front_checker.data_link_code;
			AreaID=front_checker.area_id;
			IntersectionID=front_checker.intersection_id;
			memcpy(Reserve,front_checker.reserve,5);
			content_len=length-sizeof(front_checker)-sizeof(back_checker);
			memcpy(Content, instruction+sizeof(front_checker), content_len);
			/* printf("%02x\n",InsType); */
			/* printf("%02x\n",OperationType); */
			/* printf("%02x\n",SenderID); */
			/* printf("%02x\n",ReceiverID); */
			/* printf("%02x\n",DataLinkCode); */
			/* printf("%02x\n",AreaID); */
			/* printf("%04x\n",IntersectionID); */
			/* printf("%02x\n",Reserve); */
			/* printf("%02x\n",InsType); */
			delete reader;	
			return InsType;	

		}
	}
	printf("Not Found Instruction\n");
			delete reader;	
	return -1;


};
