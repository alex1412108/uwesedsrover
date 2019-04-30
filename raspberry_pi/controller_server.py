#!/usr/bin/env python3

import inputs_pb2
import socket
import struct
import serial
import pjon_cython as pjon

class ThroughSerial(pjon.ThroughSerial):
    def recieve(self, data, length, packet_info):
        print('recieve ({}): {}'.format(length, data))


SERIAL = ThroughSerial(1, '/dev/serial0', 115200)

def main():
    try:
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.bind(('', 8081))
        server.listen(5)
        SERIAL.begin()
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

            SERIAL.send(input_bytes)
            SERIAL.update()
            SERIAL.recieve()
        SERIAL.end()
    except KeyboardInterrupt:
        pass


def serialise_inputs(inputs):
    result = bytearray()
    for field in map(lambda f: f.name, inputs.DESCRIPTOR.fields):
        value = getattr(inputs, field)
        if field in ['left_x_axis', 'left_y_axis', 'right_x_axis', 'right_y_axis']:
            value += 128
        result.append(min(value, 255))  # Bug where axis are sometimes 255.
    return result


if __name__ == '__main__':
    main()
