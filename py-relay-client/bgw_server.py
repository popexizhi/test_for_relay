#!/usr/bin/python  
import socket  
import struct  
import os  
import time  

RELAY_SERVER_PORT = 13200
RELAY_SERVER_ADDR = '192.168.1.99'
class Gw():
    """send data,get rec"""
    def __init__(self, ihost_id):
        self.server = RELAY_SERVER_ADDR
        self.port = RELAY_SERVER_PORT
        self.relay = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
        self.relay.connect((self.server, self.port)) 

        self.p_version = '\x00'
        self.p_host_id = struct.pack("!i", ihost_id)

    def send_(self, p_msg_type, p_send_data):
        """ """
        Hlength = len(p_send_data)
        p_length = struct.pack("!H", Hlength)
        p_header = self.p_version + p_msg_type + p_length 

        p_data = p_header + p_send_data
        self.relay.send(p_data)

    def get_(self):
        """return bmsg_type,p_res_data """
        p_rec = self.relay.recv(4)
        version , bmsg_type, length = struct.unpack('!cbH',p_rec)
        p_res_data = self.relay.recv(length)
        
        return bmsg_type, p_res_data

class Bgw(Gw):
    """app_server """
    def __init__(self, ihost_id = 16777226):
        Gw.__init__(self, ihost_id)

    def RelayRegisterRequest(self):
        """ msg_type :2 ;returen msg_type:3 res:0"""
        bmsg_type = 2 #
        bPre_res_msg_type = bmsg_type + 1
        p_msg_type = struct.pack("!b", bmsg_type)
        self.send_(p_msg_type, self.p_host_id)
        bres_msg_type, p_res = self.get_()
        if (bPre_res_msg_type == bres_msg_type):
            #p_res : Lhost_id + Lres
            Lhost_id, Lres = struct.unpack("!2L",p_res)
            print "Lhost_id is %d \tres is %d" %(Lhost_id, Lres)
            return Lres

    def run_test(self):
        """get rec data """
        i = 1
        f_log = open("bwg.log","w")
        while(i):
            bmsg_type, p_res_data = self.get_()
            if ( 1 == self.L2_RelayMsgConnectionResponse(bmsg_type, p_res_data)):
                i = i + 1
            #data_log = "msg_type is " + str(bmsg_type) + "**** res_data is" + repr(p_res_data)
            #f_log.writelines(data_log)
            print i -1

    def L2_RelayMsgConnectionResponse(self,bres_msg_type, p_res):
        """L2 relayMsgConnectionResponse """
        bmsg_type = 5 #
        bPre_res_msg_type = 4 #L2 RelayMsgConnectionRequest
        if (bPre_res_msg_type == bres_msg_type):
            #get fgw host_id
            Lhost_id, Ltarget_id = struct.unpack("!2L", p_res)
            p_target_id = struct.pack("!L", Ltarget_id)
            p_data = p_target_id + self.p_host_id
            p_msg_type = struct.pack("!b", bmsg_type)
            self.send_(p_msg_type, p_data)
            print "L2 relayMsgConnectionResponse fgw_host_id is "+str(Ltarget_id)
            return 0
        else:
            return 1

if __name__=="__main__":
    a = Bgw()
    a.RelayRegisterRequest()
    a.run_test()
