/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Thu Aug 06 17:39:45
 *********************************************************************/

#include "lrs.h"

#define RelayQueryConnectionRequest 0
#define RelayRegisterRequest 		2
#define RelayMsgConnectionRequest 	4
#define RelayMsgConnectionResponse  5
#define RelayDataIndication 		6

char fgw_host_id[] = {0x01,0x00,0x00,0x0a}; 
char bgw_host_id[] = {0x0a,0x00,0xa8,0xc0};

vuser_init()
{

	int length ,i ;
	char *null;
	char *data;
/*
	char bgw_RelayRegisterRequest[] = {0x00,0x02,0x00,0x04,0x01,0x00,0x00,0x0a}; //0100000a fgw_host_id
	char fgw_RelayQueryConnectionRequest[] = {0x00,0x00,0x00,0x08,0x0a,0x00,0xa8,0xc0,0x01,0x00,0x00,0x0a}; //L1 0a00a8c0 -> 0100000a
	char fgw_RelayRegiRequest[] = {0x00,0x02,0x00,0x04,0x0a,0x00,0xa8,0xc0}; //0a00a8c0 bgw_host_id
	char fgw_RelayMsgConnectionRequest[] = {0x00,0x04,0x00,0x08,0x01,0x00,0x00,0x0a,0x0a,0x00,0xa8,0xc0}; //L2  0100000a -> 0a00a8c0 fgw_host_id ->bgw_host_id
	char bgw_RelayMsgConnectionResponse[] = {0x00,0x05,0x00,0x08,0x0a,0x00,0xa8,0xc0,0x01,0x00,0x00,0x0a}; //L2 0a00a8c0 -> 0100000a bgw_host_id ->fgw_host_id
	char fgw_RelayDataIndication[] = {0x00,0x06,0x00,0x0a,0x01,0x00,0x00,0x0a,0x0a,0x00,0xa8,0xc0,0x11,0x11}; //data  0100000a -> 0a00a8c0
*/
	
    lrs_create_socket("relay-bgw", "TCP", "RemoteHost=192.168.1.99:12200",  LrsLastArg);
	lrs_create_socket("relay-fgw", "TCP", "RemoteHost=192.168.1.99:12200",  LrsLastArg);


	//-------------------------------------L1-------------------------------------------------
	//bgw_RelayRegiRequest
	send_data(fgw_host_id, null,RelayRegisterRequest, "bgw_RelayRegiRequest");//bgw_RelayRegiRequest
	lrs_send("relay-bgw", "bgw_RelayRegiRequest" , LrsLastArg);
	get_receive("relay-bgw", "buf1", LrsLastArg);


	
	//fgw_RelayQueryConnectionRequest
	send_data(bgw_host_id,fgw_host_id,RelayQueryConnectionRequest, "fgw_RelayQueryConnectionRe");//RelayQueryConnectionRequest
	lrs_send("relay-fgw", "fgw_RelayQueryConnectionRe" , LrsLastArg);
	get_receive("relay-fgw", "buf1", LrsLastArg);

	//fgw_RelayRegiRequest
	send_data(bgw_host_id, null,RelayRegisterRequest, "fgw_RelayRegiRequest");//RelayRegiRequest
	lrs_send("relay-fgw", "fgw_RelayRegiRequest" , LrsLastArg);
    get_receive("relay-fgw", "buf1", LrsLastArg);

	//-------------------------------------L2-------------------------------------------------
	//fgw_RelayMsgConnectionRequest
	// L2  0100000a -> 0a00a8c0 fgw_host_id ->bgw_host_id
	send_data(fgw_host_id, bgw_host_id,RelayMsgConnectionRequest, "fgw_RelayMsgConnectionReq");//RelayRegiRequest
	lrs_send("relay-fgw", "fgw_RelayMsgConnectionReq" , LrsLastArg);
    get_receive("relay-bgw", "buf_RelayMsgConnectionReq", LrsLastArg);

	//bgw_RelayMsgConnectionResponse
	//L2 0a00a8c0 -> 0100000a bgw_host_id ->fgw_host_id
	send_data(bgw_host_id,fgw_host_id,RelayMsgConnectionResponse, "bgw_RelayMsgConnectionRes");//bgw_RelayMsgConnectionRes
	lrs_send("relay-bgw", "bgw_RelayMsgConnectionRes" , LrsLastArg);
    get_receive("relay-fgw", "buf_RelayMsgConnectionRes", LrsLastArg);



    return 0;
}
/***************************
Ĭ��ʹ�õ�Э���ļ����ݣ���λ�ÿ�����չΪtestcase
*****************************/
enum NocRelayVersion
{
  NOC_RELAY_VER_01 = 0,
  NOC_RELAY_VER_MAX
};
/******************************************************************************************/
//�������ر�������
//����:sock Ϊsocket number
//     buf Ҫ������buffer name
// ************************************************************************************
void get_receive(char *sock, char *buf)
{	
	char *recvbuf;
	char *reclen;	
	int i,recvlen= 0;

    //lrs_set_recv_timeout(600,0);//���ó�ʱʱ��300s
	//���ذ�����
	lrs_receive_ex(sock,buf, "NumberOfBytesToRecv=4", LrsLastArg);//���ذ�ͷ�еĳ���λ��
	lrs_get_last_received_buffer(sock,&recvbuf,&recvlen); //��ñ�������
	lr_output_message("$$$$$$$$$$$$$$$$$$$$$$$$$$$$ len is %x %x %x %x", recvbuf[0],recvbuf[1],recvbuf[2],recvbuf[3]);
	if(0x00 == recvbuf[2]) 
		recvlen = recvbuf[3];
	else {
		//���ֽ�δ����
		;
	}
	lr_output_message("len is %d", recvlen);

	//���ܷ��ذ�
	lr_save_int(recvlen,"recvlen");
	reclen = lr_eval_string("NumberOfBytesToRecv=<recvlen>");
	lrs_receive_ex(sock,buf,reclen, LrsLastArg); //����ָ�����ȵı���
	lrs_get_last_received_buffer(sock,&recvbuf,&recvlen); //��ñ�������
	
	for (i=0 ; i <recvlen; i++) {
		lr_output_message("receive[%d] is %x",i, recvbuf[i]);
	}

};
/****************************************************
//msg_ty :Xgw_RelayRegisterRequest  [2]
//����:
	host_id ���ͷ���host_id
	len �������ݳ���
	param_name ��������
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
//����:
	host_id ���ͷ���host_id
	target_host_id Ϊ���ܷ���host_id
	len �������ݳ���
	param_name ��������
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
//�������ݷ��
//����:
	host_id ���ͷ���host_id
	msg_ty �������ݵ����� RelayRegiRequest 
	param_name ��������λ��
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
			//���ֽ�λ��δ����
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
			//���ֽ�λ��δ����
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
			//���ֽ�λ��δ����
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
			//���ֽ�λ��δ����
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

    lrs_save_param_ex("relay-bgw", "user", des, 0, index ,"ascii", param_name); //�洢�����ַ���
	
}