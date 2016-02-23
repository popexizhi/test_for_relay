#!/usr/bin/python  
import socket  
import struct  
import os  
import time  

RELAY_SERVER_PORT = 12200
RELAY_SERVER_ADDR = '192.168.1.99'

def RelayRegisterRequest():
    #{0x00,0x02,0x00,0x04,0x01,0x00,0x00,0x0a}; //0100000a fgw_host_id
    version = '\x00'
    msg_type = '\x02'
    #host_id = '\x01\x00\x00\x0a'
    i_host_id = 16777227
    host_id = struct.pack("!i", i_host_id) 
    length = len(host_id)
    p_len = struct.pack("!H", length)
    
    header = version + msg_type + p_len 
    print repr(header)
    return header+host_id

def send_register_request():
    gw = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    gw.connect(("192.168.1.99", 12200))
    gw.send(RelayRegisterRequest())
    rec = gw.recv(4)
    print "rec is" + repr(rec)
    version , msg_type, length = struct.unpack('!2cH',rec)
    print "version, msg_type, length" + repr(version) , repr(msg_type), length
    rec_data = gw.recv(length)
    host_id , res = struct.unpack('!LL', rec_data)
    print "host_id %d , res %d" % (host_id, res)
    gw.close()

def add_host():
    gw = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    gw.connect(("192.168.1.99", 13201))
    
    send_data = send_packet(host_id = 12101, app_server_list=[1200, 1201, 1202])
    gw.send(send_data)
    get_packet(gw)

    gw.close()

def get_packet(gw):
    rec = gw.recv(4)
    print "rec is" + repr(rec)
    version , msg_type, length = struct.unpack('!2cH',rec)
    print "version, msg_type, length" + repr(version) , repr(msg_type), length
    rec_data = gw.recv(length)
    host_id , res = struct.unpack('!LL', rec_data)
    print "host_id %d , res %d" % (host_id, res)


def send_packet(host_id = 121, host_type = "device", app_server_list=[1201,1203]):
    p_version = 0
    p_msg_type = 8
    i_host_id = host_id

    p_host_type = ("device" == host_type) and 1 or 2 #01 -device ;02 - app_server
    p_data_type = 1 #1 data_type
    p_data_type_ii = 2 # 2 data_type 
    
    host_key = "password"
    p_host_key_length = len(host_key)
    #app_server_host_id_num
    p_app_server_host_id_num = len(app_server_list)
    s_app_server = struct.pack("!i", p_app_server_host_id_num)
    for i in xrange(p_app_server_host_id_num):
        s_app_server = s_app_server + struct.pack('!i', app_server_list[i])
    
    #header(no length)
    s_packet_header =struct.pack('!2b', p_version, p_msg_type)

    #body
    _body_Format = '!i3b%dsb' % p_host_key_length
    print "_body_Format is %s" % _body_Format
    s_packet_body = struct.pack(_body_Format, i_host_id, p_host_type, p_data_type, p_host_key_length, host_key, p_data_type_ii) + s_app_server
    

    length = len(s_packet_body)   
    s_length = struct.pack('!H',length)#H:length

    s_packet = s_packet_header + s_length + s_packet_body
    print "i_host_id %x,\n _host_type %x,\n p_data_type %x,\n p_host_key_length %x,\n host_key %s" % (i_host_id, p_host_type, p_data_type, p_host_key_length, host_key)
    print "p_app_server_host_id_num %d" % p_app_server_host_id_num
    print "s_app_server " + repr(s_app_server)
    return s_packet
    
if __name__ == "__main__": 
    #send_register_request()
    add_host()
