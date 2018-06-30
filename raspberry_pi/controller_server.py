#!/usr/bin/env python3

import inputs_pb2
import socket
import struct
import serial
SERIAL = serial.Serial('/dev/ttyACM0', 9600, timeout=1)

def main():
    try:
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.bind(('', 8081))
        server.listen(5)
        SERIAL.open()
        while True:
            connection, address = server.accept()
            data_length = struct.unpack('H', connection.recv(2))[0]
            data = connection.recv(data_length)
            connection.close()
            inputs = inputs_pb2.Input()
            inputs.ParseFromString(data)
            print(inputs)
            input_bytes = serialise_inputs(inputs)
            print(input_bytes)
            SERIAL.write(input_bytes)
    except KeyboardInterrupt:
        SERIAL.close()



def serialise_inputs(inputs):
    result = b''
    for field in inputs.DESCRIPTOR.fields_by_name:
        value = getattr(inputs, field)
        if field in ['left_x_axis', 'left_y_axis', 'right_x_axis', 'right_y_axis']:
            value += 128
        result += chr(value).encode('ascii', errors='replace')
    return result


if __name__ == '__main__':
    main()
