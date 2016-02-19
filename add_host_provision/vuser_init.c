#include "lrs.h"


vuser_init()
{
    int length ,i ,res = 0 ;
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
	char add_host_device[] ={ 
							   0x00,
							   0x08, 				//msg_type :add_host 0x08
							   0x00,0x16,			//data_length :
							   0x01,0x00,0x01,0x1a, //host_id :0100011a
							   0x01,				//host_type :device
							   0x01,				//data_type :host key
							   0x02,				//host_key_len :0x02
							   0x00,0x00,			//host_key	host_key_len * 8 : 
							   0x02,				// data_type:字段数据类型
							   0x00,0x00,0x00,0x02,	// app_server_host_id_num:允许访问的应用主机个数
							   0x01,0x00,0x01,0x1b,	// app_server_host_id_list: 01 00 01 1b  and 01 00 01 1c
							   0x01,0x00,0x01,0x1c,	// app_server_host_id_list:允许访问的应用主机ID列表
	}; //00成功，03重复注册
	char add_host_app_server[] = {
							   0x00,
							   0x08, 				//msg_type :add_host 0x08
							   0x00,0x09,			//data_length :
							   0x00,0x00,0x00,0x02, //host_id :0100011a
							   0x02,				//host_type :app_server
							   0x01,				//data_type :host key
							   0x02,				//host_key_len :0x02
							   0x00,0x00,			//host_key	host_key_len * 8 :
	};
    lrs_create_socket("provision", "TCP", "RemoteHost=192.168.1.99:12201",  LrsLastArg);
	//lrs_create_socket("relay-fgw", "TCP", "RemoteHost=192.168.1.99:12200",  LrsLastArg);
	

	lrs_save_param_ex("provision", "user", add_host_device, 0, sizeof(add_host_device) ,"ascii", "add_host"); //存储发送字符串
	lrs_send("provision", "add_host" , LrsLastArg);
    lrs_receive("provision", "buf2", LrsLastArg);

	lrs_save_param_ex("provision", "user", add_host_app_server, 0, sizeof(add_host_app_server) ,"ascii", "add_host"); //存储发送字符串
	lrs_send("provision", "add_host" , LrsLastArg);
    lrs_receive("provision", "buf2", LrsLastArg);
    return 0;
}

