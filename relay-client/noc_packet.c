

//发送的请求内容
#define RelayQueryConnectionRequest  0
#define RelayQueryConnectionResponse 1
#define RelayRegisterRequest 		 2
#define RelayRegisterResponse		 3
#define RelayMsgConnectionRequest 	 4
#define RelayMsgConnectionResponse   5
#define RelayDataIndication 		 6



/******************************************************************************************/
//处理返回报文内容
//参数:sock 为socket number
//     buf 要处理的buffer name
// 	   msg_ty_rec 要查找的返回msg_ty
// 	   des 返回的数据内容
// ************************************************************************************/
void _receive(char *sock, char *buf, int msg_ty, char *des)
{	
	char *recvbuf;
	char *reclen;
	char msg_type_rec ;
	int i,recvlen= 0;

    //lrs_set_recv_timeout(600,0);//设置超时时间300s

	//返回msg_type判断
	lrs_receive_ex(sock,buf, "NumberOfBytesToRecv=2", LrsLastArg);//返回包头中的长度位置
	lrs_get_last_received_buffer(sock,&recvbuf,&recvlen); //获得报文内容
	lr_output_message("$$$$$$$$$$$$$$$$$$$$$$$$$$$$ len is %x %x %x %x", recvbuf[0],recvbuf[1]);
	msg_type_rec =  recvbuf[1];
	if( (char)msg_ty == msg_type_rec){ 
		lr_output_message("msg_type_rec 为预期结果");
	}else{
		//非预期返回类型，继续等待，未处理
		lr_output_message("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~非预期返回类型，继续等待，未处理");
	}

	lr_output_message("len is %d", recvlen);

	//返回包长度
	lrs_receive_ex(sock,buf, "NumberOfBytesToRecv=2", LrsLastArg);//返回包头中的长度位置
	lrs_get_last_received_buffer(sock,&recvbuf,&recvlen); //获得报文内容
	lr_output_message("$$$$$$$$$$$$$$$$$$$$$$$$$$$$ len is %x %x %x %x", recvbuf[0],recvbuf[1]);
	if(0x00 == recvbuf[0]) {
		recvlen = recvbuf[1];
	}else {
		//高字节未处理
		;
	}
	lr_output_message("len is %d", recvlen);

	//接受返回包
	lr_save_int(recvlen,"recvlen");
	reclen = lr_eval_string("NumberOfBytesToRecv=<recvlen>");
	lrs_receive_ex(sock,buf,reclen, LrsLastArg); //返回指定长度的报文
	lrs_get_last_received_buffer(sock,&recvbuf,&recvlen); //获得报文内容
	
	for (i=0 ; i <recvlen; i++) {
		des[i] = recvbuf[i];
		//lr_output_message("receive[%d] is %x",i, recvbuf[i]);
	}
	
}
//返回的请求内容
void get_receive(char *sock, char *buf)
{
	;
}
/*******************************************************
//NOC协议处理
//参数:sock 为socket number
//     buf 要处理的buffer name
//	   msg_ty 发送数据的类型[这里根据协议查找对应的返回类型]
//	   des 返回内容
//返回值: 返回0为成功
*******************************************************/
int Noc_get_receive(char *sock, char *buf, int msg_ty)
{
	//
	int msg_ty_pro ,i;
	char des[255];
	char result[4] = {0xff,0xff,0xff,0xff};
	int res = 0;
	
	
	//协议类型预期,参考#define定义
	if(msg_ty <4) 
		msg_ty_pro = msg_ty + 1;
	else 
		msg_ty_pro = msg_ty ;

	_receive(sock,buf,msg_ty_pro,des);
	
	//des中结果判断
	if (RelayQueryConnectionResponse == msg_ty_pro) {
		//L1 
		/*
        //NOC_RELAY_MSG_QUERY_CONN_RESP
			struct RelayQueryConnectionResponse
			{
			  uint8_t   version;              //version(high 4its):NocRelayVersion
			  uint8_t   msg_type;             //type(low 4bits):NocRelayMsgType
			  uint16_t  length;               //length of data packet, it is will be set to zero if it is a message
			  uint32_t  host_id;        	  //host id of from
			  uint32_t  target_host_id;       //target host id  
			  uint32_t  result;               //query result
			  uint32_t  host_key_length;      //length of hostkey, unit:bytes
			  uint8_t   host_key[HOST_KEY_MAX_LENGTH]; //host key
			} ALIGN_1_DECLARE ;
        */
		for (i=0;i<4;i++) {
			result[i] = des[i+8] ;//
			lr_output_message("result[%d] is %x", i, result[i]);
			//返回结果判断
			if( result[i]!= 0x00){
				lr_output_message("～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～RelayQueryConnectionResponse 返回存在失败 result[%d] is %x", i, result[i]);
				res = 1;
				}
		}
	}
	if (RelayRegisterResponse == msg_ty_pro) {
		//L1 
		/*
					//NOC_RELAY_MSG_REGI_RESP header
			struct RelayRegiResponse
			{
			  uint8_t   version;              //version(high 4its):NocRelayVersion
			  uint8_t   msg_type;             //type(low 4bits):NocRelayMsgType
			  uint16_t  length;               //length of data packet, it is will be set to zero if it is a message
			  uint32_t  host_id;              //host id of the incoming connection
			  uint32_t  result;               //registration result, refer to NocRelayRegistrationResult
			} ALIGN_1_DECLARE;
		*/
		for (i=0;i<4;i++) {
			result[i] = des[i+4] ;//
			lr_output_message("result[%d] is %x", i, result[i]);
			//返回结果判断
			if( result[i]!= 0x00){
				lr_output_message("～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～RelayRegiResponse 返回存在失败 result[%d] is %x", i, result[i]);
				res = 1;
				}
		}//end for
	}//end if

		
	
	return res;

}


/****************************************************
//msg_ty :Xgw_RelayRegisterRequest  [2]
//参数:
	host_id 发送方的host_id
	len 发送数据长度
	param_name 发送数据
******************************************************/
void gw_RelayRegisterRequest(char host_id[],int *index,char des[])
{
	int i = 0;
	int j = *index;
	//msg_type
	des[j++] = 0x02;

	//length
	des[j++] =0x00;
	des[j++] =0x04;

	for (;i < 4;) {
		des[j++] = host_id[i++];
	}
	*index = j;
}

/****************************************************
//host_target
//参数:
	host_id 发送方的host_id
	target_host_id 为接受方的host_id
	len 发送数据长度
	param_name 发送数据
******************************************************/
void host_target(char host_id[],char target_host_id[],int *index,char des[])
{
	int i = *index;
	int j = *index;
	for(;i < j + 4; i++){
		des[i] = host_id[i - j];
		des[i + 4] = target_host_id[i - j];
	}
	j = j + 8;
	*index = j;

}
/****************************************************
//发送数据封包
//参数:
	host_id 发送方的host_id
	msg_ty 发送数据的类型 RelayRegiRequest 
	param_name 发送数据位置
******************************************************/
void send_data(char host_id[],char target_host_id[],int msg_ty,char param_name[])
{
	int length ,i , index = 0;
	int *size ;
	//version 
	char version = 0X00;
	//msg_type
	char msg_type;
	//length_c
	char c_length[2];
	char des[299];
	char L2_data[20] = {"1"}; //L2 send_data

	des[index++] = version;

	/*****************************************************************
	#define RelayQueryConnectionRequest 	0
	#define RelayRegisterRequest 			2
	#define RelayMsgConnectionRequest 	4
	#define RelayMsgConnectionResponse    5
	#define RelayDataIndication			6
	*****************************************************************/
	if( RelayRegisterRequest == msg_ty ){
		  size = &index;
		gw_RelayRegisterRequest(host_id, size, des );
	}
	if( RelayQueryConnectionRequest == msg_ty ){
		des[index++] = 0x00;
		length = sizeof(host_id) + sizeof(target_host_id);
		if (length < 256) {
			des[index++] = 0x00;
			des[index++] = length;
		}
		else{
			//高字节位移未处理
			;
		}
		size = &index;
		host_target(host_id ,target_host_id, size,des );

	}
	if (RelayMsgConnectionRequest == msg_ty) {
		des[index++] = 0x04;
		length = sizeof(host_id) + sizeof(target_host_id);
		if (length < 256) {
			des[index++] = 0x00;
			des[index++] = length;
		}
		else{
			//高字节位移未处理
			;
		}
		size = &index;
		host_target(host_id ,target_host_id, size,des );
	}
	if (RelayMsgConnectionResponse == msg_ty) {
		des[index++] = 0x05;
		length = sizeof(host_id) + sizeof(target_host_id);
		if (length < 256) {
			des[index++] = 0x00;
			des[index++] = length;
		}
		else{
			//高字节位移未处理
			;
		}
		size = &index;
		host_target(host_id ,target_host_id, size,des );
	}
	if( RelayDataIndication == msg_ty){
		//RelayDataIndication
		;
		des[index++] = 0x06;
		length = sizeof(host_id) + sizeof(target_host_id) + sizeof(L2_data);
		if (length < 256) {
			des[index++] = 0x00;
			des[index++] = length;
		}
		else{
			//高字节位移未处理
			;
		}
		size = &index;
		host_target(host_id ,target_host_id, size,des );
		for (i=0;i<sizeof(L2_data);) {
			des[index++] = L2_data[i++];
		}
	}

    //lr_output_message ("des is %X, index is %d, *size is %d",des, index, *size);
	//for (i=0;i<index;i++) {
	//	lr_output_message ("[%d] is %x", i, des[i]);
	//}

    lrs_save_param_ex("relay-bgw", "user", des, 0, index ,"ascii", param_name); //存储发送字符串
	
}
