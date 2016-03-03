#!/usr/bin/python  
import socket  
import struct  
import os  
import time  

RELAY_SERVER_PORT = 13201
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
class Magw(Gw):
    """manage xgw """
    def add_host(self, ihost_id = 121, host_type = "device", app_server_list=[1201,1203], host_key = "password"):
        """add host_id """
        bmsg_type = 8 #
        p_msg_type = struct.pack("!b", bmsg_type)

        p_host_type = ("device" == host_type) and 1 or 2 #01 -device ;02 - app_server
        p_data_type = 1 #1 data_type
        p_data_type_ii = 2 # 2 data_type 
        
        p_host_key_length = len(host_key)
        #app_server_host_id_num
        p_app_server_host_id_num = len(app_server_list)
        s_app_server = struct.pack("!i", p_app_server_host_id_num)
        for i in xrange(p_app_server_host_id_num):
            s_app_server = s_app_server + struct.pack('!i', app_server_list[i])
    
        #body
        _body_Format = '!i3b%dsb' % p_host_key_length
        #print "_body_Format is %s" % _body_Format
        if ("device" == host_type) :
            p_data =  struct.pack(_body_Format, ihost_id, p_host_type, p_data_type, p_host_key_length, host_key, p_data_type_ii) + s_app_server
        else:
            _body_Format = '!i3b%ds' % p_host_key_length
            p_data = struct.pack(_body_Format, ihost_id, p_host_type, p_data_type, p_host_key_length, host_key)
        
    
        
        self.send_(p_msg_type, p_data)
        bres_msg_type , p_data_res = self.get_()
        i_host, i_res = struct.unpack("!2L", p_data_res)
        print "Lhost_id is %d \t add res is %d" % (ihost_id, i_res)

    def del_host(self, ihost_id= 121, idelres=1):
        """del host_id """
        bmsg_type = 10 #
        p_msg_type = struct.pack("!b", bmsg_type)
        p_data = struct.pack("!2i", ihost_id, idelres)
        self.send_(p_msg_type, p_data)
        bres_msg_type , p_data_res = self.get_()
        i_host, i_res = struct.unpack("!2L", p_data_res)
        print "Lhost_id is %d \t del res is %d" % (ihost_id, i_res)
if __name__=="__main__":
    a = Magw(123)
    for i in xrange(20):
        a.add_host(i, app_server_list=[16777226,16777227])
        #a.add_host(i+1000)
        #a.del_host(i)
        #a.del_host(i+1000)

