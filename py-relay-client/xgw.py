#!/usr/bin/python  
import socket  
import struct  
import os  
import time  



if __name__ == "__main__":  
    gw = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    gw.connect(("192.168.1.99", 12200))
    gw.close()