#!/usr/bin/env python3

import inputs_pb2
import socket
import struct

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('', 8081))
server.listen(5)
while True:
    connection, address = server.accept()
    data_length = struct.unpack('H', connection.recv(2))[0]
    data = connection.recv(data_length)
    connection.close()
    inputs = inputs_pb2.Input()
    inputs.ParseFromString(data)
    print(inputs)

    
