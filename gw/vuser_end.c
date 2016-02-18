/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Thu Aug 06 17:39:45
 *********************************************************************/

#include "lrs.h"


vuser_end()
{
	

	lrs_send("socket0", "buf16", LrsLastArg);

    lrs_receive("socket0", "buf17", LrsLastArg);

    lrs_disable_socket("socket0", DISABLE_SEND_RECV);

    lrs_close_socket("socket0");
    lrs_cleanup();

    return 0;
}

