/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Thu Aug 06 17:39:45
 *********************************************************************/

#include "lrs.h"
#include "noc_packet.c"
#define RelayQueryConnectionRequest 0
#define RelayRegisterRequest 		2
#define RelayMsgConnectionRequest 	4
#define RelayMsgConnectionResponse  5
#define RelayDataIndication 		6

char fgw_host_id[] = {0x01,0x00,0x00,0x0a}; 
char bgw_host_id[] = {0x0a,0x00,0xa8,0xc0};

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
	
    lrs_create_socket("relay-bgw", "TCP", "RemoteHost=192.168.1.99:12200",  LrsLastArg);
	lrs_create_socket("relay-fgw", "TCP", "RemoteHost=192.168.1.99:12200",  LrsLastArg);


	//-------------------------------------L1-------------------------------------------------
	//bgw_RelayRegiRequest
	lr_message("-------------------------------------L1 : bgw_RelayRegiRequest-----------------------------------------------");
	send_data(fgw_host_id, null,RelayRegisterRequest, "bgw_RelayRegiRequest");//bgw_RelayRegiRequest
	lrs_send("relay-bgw", "bgw_RelayRegiRequest" , LrsLastArg);
	res = Noc_get_receive("relay-bgw", "buf1", RelayRegisterRequest);
	if (0 != res ) {
		lr_error_message("bgw_RelayRegiRequest is err");
		return res;
	}

	
	//fgw_RelayQueryConnectionRequest
	lr_message("-------------------------------------L1 : fgw_RelayQueryConnectionRequest-----------------------------------------------");
	send_data(bgw_host_id,fgw_host_id,RelayQueryConnectionRequest, "fgw_RelayQueryConnectionRe");//RelayQueryConnectionRequest
	lrs_send("relay-fgw", "fgw_RelayQueryConnectionRe" , LrsLastArg);
	res = Noc_get_receive("relay-fgw", "buf1", RelayQueryConnectionRequest);
	if (0 != res ) {
		lr_error_message("fgw_RelayQueryConnectionRequest is err");
		return res;
	}

	//fgw_RelayRegiRequest
	lr_message("-------------------------------------L1 : fgw_RelayRegiRequest-----------------------------------------------");
	send_data(bgw_host_id, null,RelayRegisterRequest, "fgw_RelayRegiRequest");//RelayRegiRequest
	lrs_send("relay-fgw", "fgw_RelayRegiRequest" , LrsLastArg);
    res = Noc_get_receive("relay-fgw","buf1",RelayRegisterRequest);
	if (0 != res ) {
		lr_error_message("fgw_RelayRegiRequest is err");
		return res;
	}

	//-------------------------------------L2-------------------------------------------------
	//fgw_RelayMsgConnectionRequest
	// L2  0100000a -> 0a00a8c0 fgw_host_id ->bgw_host_id
	lr_message("-------------------------------------L2 : fgw_RelayMsgConnectionRequest-----------------------------------------------");
	send_data(fgw_host_id, bgw_host_id,RelayMsgConnectionRequest, "fgw_RelayMsgConnectionReq");//RelayRegiRequest
	lrs_send("relay-fgw", "fgw_RelayMsgConnectionReq" , LrsLastArg);
    res = Noc_get_receive("relay-bgw", "buf_RelayMsgConnectionReq", RelayMsgConnectionRequest);
	if (0 != res ) {
		lr_error_message("L2 :: fgw_RelayMsgConnectionRequest is err");
		return res;
	}
	//bgw_RelayMsgConnectionResponse
	//L2 0a00a8c0 -> 0100000a bgw_host_id ->fgw_host_id
	lr_message("-------------------------------------L2 : bgw_RelayMsgConnectionResponse-----------------------------------------------");
	send_data(bgw_host_id,fgw_host_id,RelayMsgConnectionResponse, "bgw_RelayMsgConnectionRes");//bgw_RelayMsgConnectionRes
	lrs_send("relay-bgw", "bgw_RelayMsgConnectionRes" , LrsLastArg);
    res = Noc_get_receive("relay-fgw", "buf_RelayMsgConnectionRes", RelayMsgConnectionResponse);
	if (0 != res ) {
		lr_error_message("L2 :: bgw_RelayMsgConnectionResponse is err");
		return res;
	}

    return res;
}

