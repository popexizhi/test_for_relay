#!/usr/bin/python  
import socket  
import struct  
import os  
import time  


def add_host(wait_time = 0.01):
    gw = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    gw.connect(("192.168.1.99", 13201))
    i = 700
    app_server = i - 2
    send_data = send_packet(host_id = app_server, host_type = "app_server", app_server_list=[])
    gw.send(send_data)
    host_id, res = get_packet(gw)
    if (res != 0):
        print "add app_server err"
        return res
    
    for i in xrange(100000 + i):
        send_data = send_packet(host_id = i ,app_server_list=[app_server,16777226,16777227])
        gw.send(send_data)
        host_id, res = get_packet(gw)
        if (res != 0):
                print "add host_id:%d is err" % host_id
                #return res
        time.sleep(wait_time)
    gw.close()

def get_packet(gw):
    rec = gw.recv(4)
    version , msg_type, length = struct.unpack('!2cH',rec)
    rec_data = gw.recv(length)
    host_id , res = struct.unpack('!LL', rec_data)
    print "host_id %d , res %d" % (host_id, res)
    return host_id , res


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
    #print "_body_Format is %s" % _body_Format
    if ("device" == host_type) :
        s_packet_body = struct.pack(_body_Format, i_host_id, p_host_type, p_data_type, p_host_key_length, host_key, p_data_type_ii) + s_app_server
    else:
        _body_Format = '!i3b%ds' % p_host_key_length
        s_packet_body = struct.pack(_body_Format, i_host_id, p_host_type, p_data_type, p_host_key_length, host_key)
    

    length = len(s_packet_body)   
    s_length = struct.pack('!H',length)#H:length

    s_packet = s_packet_header + s_length + s_packet_body
    return s_packet
    
if __name__ == "__main__": 
    add_host()
