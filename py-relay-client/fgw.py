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

    def get_(self, length_set = None):
        """return bmsg_type,p_res_data """
        p_rec = self.relay.recv(4)
        p_last_data = None
        version , bmsg_type, length = struct.unpack('!cbH',p_rec)
        if (None == length_set):
            p_res_data = self.relay.recv(length)
        else:
            p_res_data = self.relay.recv(length_set)
            p_last_data = self.relay.recv(length - length_set)
        return bmsg_type, p_res_data

class Fgw(Gw):
    """fgw """
    def __init__(self, ihost_id = 167815360, itarget_host_id = 16777226):
        Gw.__init__(self, ihost_id)
        self.itarget_host_id = itarget_host_id
        self.set_target_host_id(self.itarget_host_id)
        self.p_data = '\xff\xff'
        
    def set_target_host_id(self, target_host_id):
        """target_host_id """
        self.p_target_host_id = struct.pack("!i", target_host_id)
    
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
    def RelayQueryConnectionRequest(self):
        """msg_type :0 ;returen msg_type:1 res:0"""
        bmsg_type = 0 #
        bPre_res_msg_type = bmsg_type + 1
        p_msg_type = struct.pack("!b", bmsg_type)
        p_send_data = self.p_host_id + self.p_target_host_id
        self.send_(p_msg_type, p_send_data)
        bres_msg_type, p_res= self.get_(length_set=12)
        if (bPre_res_msg_type == bres_msg_type):
            #p_res : Lhost_id + Lres
            Lhost_id, Ltarget_host_id, Lres = struct.unpack("!3L",p_res)
            print "Lhost_id is %d \tLtarget_host_id is %d\tres is %d" %(Lhost_id, Ltarget_host_id, Lres)
            return Lres

    def L2_RelayMsgConnectionRequest(self):
        """L2 RelayMsgConnectionRequest """
        bmsg_type = 4 #
        p_msg_type = struct.pack("!b", bmsg_type)
        p_data = self.p_target_host_id + self.p_host_id
        self.send_(p_msg_type, p_data)
        print "L2 RelayMsgConnectionRequest *************"

    def L2_send_data(self, data = 6553):
        """L2 send_data """
        bmsg_type = 6 #
        p_msg_type = struct.pack("!b", bmsg_type)
        p_data = self.p_target_host_id + self.p_host_id + self.p_data + '\x00' * data
        self.send_(p_msg_type, p_data)

    
if __name__=="__main__":
    a = Fgw()

    a.RelayQueryConnectionRequest()
    a.RelayRegisterRequest()
    a.L2_RelayMsgConnectionRequest()
    for i in xrange(1000):
        a.L2_send_data()
    #a.L2_send_data()
