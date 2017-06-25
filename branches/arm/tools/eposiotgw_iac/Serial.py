#!/usr/bin/env python3

# To get an unencrypted PEM (without passphrase):
# openssl rsa -in certificate.pem -out certificate_unencrypted.pem

import os
import sys
import time
import serial
import argparse
import struct

class Serial:
    # 'EPOSMote III device descriptor file'
    DEV = '/dev/ttyACM0'
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
        try:
            first = self.mote.read(1)
            while struct.unpack('=1B', first)[0] != 94:
                first = self.mote.read(1)
                if not len(first):
                    self.mote.close()
                    self.init_mote()
        except KeyboardInterrupt:
            raise
        except Exception as e:
            print("Exception caught read first:", e, file=sys.stderr)
            self.mote.close()
            self.init_mote()
        self.read_msg()

    def read_msg(self):
        try:
            #data = self.mote.read(36)
            data = self.mote.read(8)
            header = struct.unpack('=2i', data)
            data = self.mote.read(header[1])
        except KeyboardInterrupt:
            raise
        except Exception as e:
            print("Exception caught read msg:", e, file=sys.stderr)
            data = b''

        if not len(data):
            self.mote.close()
            self.init_mote()
        else:
            if self.read_end()[0] != 36:
                self.read_first();
            if(header[0] == 0):
                unpacked_data = struct.unpack('=3l1L', data)
            if(header[0] == 1):
                unpacked_data = struct.unpack('=3l1L2Q', data)
            if(header[0] == 2):
                unpacked_data = struct.unpack('=1L1I', data)    
            print("msg=", unpacked_data)
            #self.serial_manager.handle_serial_request(unpacked_data)
            #self.mote.write(bytes(str(1), 'ascii'))

    def read_end(self):
        try:
            end = self.mote.read(1)
        except KeyboardInterrupt:
            raise
        except Exception as e:
            print("Exception caught:", e, file=sys.stderr)
            self.mote.close()
            self.init_mote()

        if not len(end):
            self.mote.close()
            self.init_mote()
        else:
            return struct.unpack('=1B', end)

    def write(self, data):
        aceptable = int(data)
        try:
            self.mote.write(bytes(aceptable))
            print("epoch written", file=sys.stderr)
        except KeyboardInterrupt:
            raise
        except serial.serialutil.SerialTimeoutException:
            pass
