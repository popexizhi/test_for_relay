/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Thu Aug 06 17:39:45
 *********************************************************************/

#include "lrs.h"
//#include "noc_packet.c"

Action()
{
	int i;
	//fgw_RelayDataIndication ------------------------data------------------------------------------
	lr_message("-------------------------------------L2 : send data-----------------------------------------------");
	for (i=0;i<1000;i++) {
		send_data(fgw_host_id,bgw_host_id,RelayDataIndication, "fgw_RelayDataIndication");
		lrs_send("relay-fgw", "fgw_RelayDataIndication" , LrsLastArg);
		Noc_get_receive("relay-bgw","buf_RelayDataIndication", RelayDataIndication);

	}
    return 0;
}

