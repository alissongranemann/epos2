#! /usr/bin/python3

# Author: Mateus Krepsky Ludwich.
# Contact: mateus@lisha.ufsc.br
# Year: 2015

import sys
sys.path.append('../res')
sys.path.append('.')
import nic_test_receiver_zero_copy_res
from t4exp import Ignore_white_space_tabs_and_new_lines
from t4exp import check_output

import serial
import io


def execute_pyserial():
    device = '/dev/ttyUSB0'
    baud_rate = 115200
    data_bits = 8
    stop_bits = 1
    ser = serial.Serial(port = device, baudrate = baud_rate, bytesize = data_bits, stopbits = stop_bits, timeout = None)
    print(ser)
    assert(ser.isOpen())

    output = ''
    data = None
    str_data = ''
    while 'Bye!' not in str_data:
        try:
            in_waiting = ser.inWaiting()
            if in_waiting:
                data = ser.read(in_waiting)
                str_data = data.decode('utf-8')
                output += str_data

        except serial.serialutil.SerialException:
            pass
        except KeyboardInterrupt:
            break

    ser.close()

    # print('output: ' + output)
    return output


def obtained_output():
    return execute_pyserial()

def _obtained_output():
    return nic_test_receiver_zero_copy_res._obtained_output()


def main():
    print('NIC Zero-Copy Receiver Application (Output Checker)')
    filters = []
    filters.append(Ignore_white_space_tabs_and_new_lines())

    check_output(nic_test_receiver_zero_copy_res.expected_output(), obtained_output(), filters)

    print('All tests passed.')
    print('Bye!')

if __name__ == '__main__':
    main()
