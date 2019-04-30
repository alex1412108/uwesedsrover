#!/usr/bin/env python3

import inputs_pb2
import numpy
import socket
import struct
import usb
import yaml


def main():
    # Load configuration and mappings.
    config, error = get_yaml('configuration.yaml')
    if error:
        print('Unable to load configuration file.')
        raise error
    mappings, error = get_yaml('input_mappings.yaml')
    if error:
        print('Unable to load input mappings.')
        raise error

    # Find the first connected Xbox controller.
    device = usb.core.find(idVendor=int(config['vendor_id']),
                           idProduct=int(config['product_id']))
    if device is None:
        raise Exception('Unable to find an Xbox controller.')

    # Reclaim device from other driver.
    device.reset()
    if device.is_kernel_driver_active(config['interface']['index']):
        device.detach_kernel_driver(config['interface']['index'])

    # Retrieve endpoint.
    device.set_configuration()
    configuration = device[config['configuration']]
    interface = configuration[(config['interface']['index'],
                               config['interface']['setting'])]
    endpoint = interface[config['endpoint']]

    # Handle device input.
    interrupted = False
    inputs_previous = {}
    while not interrupted:
        try:
            inputs = None
            data = device.read(endpoint.bEndpointAddress,
                               endpoint.wMaxPacketSize)
            if len(data) == config['data']['length']:
                inputs = process_input(data, mappings)
                message = create_message(inputs)
                error = send_message(message, config['address'],
                                     config['port'])
                if error:
                    print('Encountered issue sending message to server.')
            if inputs and inputs != inputs_previous:
                inputs_previous = inputs
        except KeyboardInterrupt:
            interrupted = True
        except usb.core.USBError as error:
            if error.args == ('Operation timed out',):
                continue


def get_yaml(path):
    content, error = None, None
    with open(path) as stream:
        try:
            content = yaml.safe_load(stream)
        except yaml.YAMLError as exception:
            error = exception
    return content, error


def process_input(data, mappings):
    inputs = {}
    for mapping in mappings:
        value = int(data[mapping['index']])
        if 'mapping' in mapping:
            value = (mapping['mapping'] == value & mapping['mapping'])
        if 'interpolation' in mapping:
            for interpolation in mapping['interpolation']:
                if value in range(interpolation['from_start'],
                                  interpolation['from_end']):
                    from_ = [interpolation['from_start'],
                             interpolation['from_end']]
                    to = [interpolation['to_start'], interpolation['to_end']]
                    value = int(numpy.interp(value, from_, to))
        inputs[mapping['name']] = value
    return inputs


def create_message(inputs):
    message = inputs_pb2.Input()
    for name, value in inputs.items():
        setattr(message, name.replace(' ', '_'), value)
    return message


def send_message(message, address, port):
    error = None
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
            server.connect((address, port))
            data = message.SerializeToString()
            server.send(struct.pack('H', len(data)))
            server.send(data)
    except Exception as exception:
        error = exception
    return error


if __name__ == '__main__':
    main()
