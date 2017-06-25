#!/usr/bin/env python3

# To get an unencrypted PEM (without passphrase):
# openssl rsa -in certificate.pem -out certificate_unencrypted.pem

import os
import sys
import time
import serial
import argparse
import struct
from enum import IntEnum, unique

@unique
class Header_Index(IntEnum):
    TYPE = 0
    LENGTH = 1

class Serial:
    # 'EPOSMote III device descriptor file'
    DEV = '/dev/ttyACM1'
    # 'Timeout for reading from mote'
    TIMEOUT = 600

    def __init__(self, Serial_Manager):
        self.serial_manager = Serial_Manager;
        self.mote = self.init_mote()
        while(True):
            self.read_first();

    def init_mote(self):
        ok = False
        while not ok:
            try:
                print("Waiting for", self.DEV, "to appear")
                while not os.path.exists(self.DEV) or not os.access(self.DEV, os.W_OK):
                    pass
                #mote = serial.Serial(DEV, 115200, timeout = TIMEOUT, write_timeout = 10)
                mote=serial.Serial(self.DEV,baudrate=115200, bytesize=8, parity='N', stopbits=1,timeout=self.TIMEOUT)
                ok = True
            except KeyboardInterrupt:
                raise
            except Exception as e:
                print("Exception caught:", e, file=sys.stderr)
                ok = False
                time.sleep(3)

        print("Mote open", file=sys.stderr)
        ts = bytes(str(int(time.time() * 1000000)), 'ascii')
        try:
            mote.write(ts + b'X')
            print("epoch written", file=sys.stderr)
        except KeyboardInterrupt:
            raise
        except serial.serialutil.SerialTimeoutException:
            pass

        print("init_mote() done", file=sys.stderr)
        return mote

    def read_first(self):
        self.read(1, self.read_header)

    def read_header(self, start, args):
        unpack = struct.unpack('=1B', start)
        if unpack[0] == 94:
            self.read(8, self.check_end)

    def check_end(self, header_data, args):
        header = struct.unpack('=2i', header_data)
        self.read(1, self.read_msg, header)
 
    def read_msg(self, end, args):
        unpack = struct.unpack('=1B', end)
        header = args[0];
        if unpack[0] == 36:
            self.read(header[Header_Index.LENGTH], self.send_to_manager, header[Header_Index.TYPE])

    def send_to_manager(self, msg, args):
        self.serial_manager.handle_serial_request(args[0], msg)


    def read(self, length, method, *args):
        try:
            data = self.mote.read(length)
        except KeyboardInterrupt:
            raise
        except Exception as e:
            print("Exception caught read msg:", e, file=sys.stderr)
            self.mote.close()
            self.init_mote()

        if not len(data):
            self.mote.close()
            self.init_mote()
        else:
            method(data, args)

    def write(self, data):
        aceptable = int(data)
        try:
            self.mote.write(bytes(aceptable))
            print("epoch written", file=sys.stderr)
        except KeyboardInterrupt:
            raise
        except serial.serialutil.SerialTimeoutException:
            pass
