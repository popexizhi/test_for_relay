/*********************************************************************
 * Created by Mercury Interactive Windows Sockets Recorder
 *
 * Created on: Thu Aug 06 17:39:45
 *********************************************************************/

#include "lrs.h"


Action()
{
    lrs_create_socket("socket0", "TCP", "RemoteHost=192.168.1.41:10086",  LrsLastArg);

    lrs_send("socket0", "buf0", LrsLastArg);

    lrs_receive("socket0", "buf1", LrsLastArg);

    lrs_send("socket0", "buf2", LrsLastArg);

    lrs_receive("socket0", "buf3", LrsLastArg);

    lrs_send("socket0", "buf4", LrsLastArg);

    lrs_receive("socket0", "buf5", LrsLastArg);

    lrs_send("socket0", "buf6", LrsLastArg);

    lrs_receive("socket0", "buf7", LrsLastArg);

    lrs_send("socket0", "buf8", LrsLastArg);

    lrs_receive("socket0", "buf9", LrsLastArg);

    lrs_send("socket0", "buf10", LrsLastArg);

    lrs_receive("socket0", "buf11", LrsLastArg);

    lrs_send("socket0", "buf12", LrsLastArg);

    lrs_receive("socket0", "buf13", LrsLastArg);

    lrs_send("socket0", "buf14", LrsLastArg);

    lrs_receive("socket0", "buf15", LrsLastArg);

    lrs_send("socket0", "buf16", LrsLastArg);

    lrs_receive("socket0", "buf17", LrsLastArg);

    lrs_disable_socket("socket0", DISABLE_SEND_RECV);

    lrs_close_socket("socket0");

    return 0;
}

