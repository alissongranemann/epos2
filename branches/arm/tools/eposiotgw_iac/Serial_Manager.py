from Interest import Interest
from enum import IntEnum, unique
from Sensor import Sensor
from Serial import Serial
import struct

@unique
class Request_Type(IntEnum):
    SENSOR = 0
    INTEREST = 1
    CONFIG = 2

@unique
class Index(IntEnum):
    X = 0
    Y = 1
    Z = 2
    RADIUS = 3
    PERIOD = 4
    EXPIRY = 5
    REF = 6

class Serial_Manager:

    def __init__(self, Admission_Control):
        self.admission_control = Admission_Control;
        self.serial = Serial(self)

    def handle_serial_request(self, type, data):
        if(type == Request_Type.SENSOR):
            unpacked_data = struct.unpack('=3l1L', data)
            print("msg=", unpacked_data)
            sensor = self.extract_sensor_data(unpacked_data)
            self.admission_control.handle_new_sensor_request(sensor)
        if(type == Request_Type.INTEREST):
            unpacked_data = struct.unpack('=3l1L2Q1i', data)
            print("msg=", unpacked_data)
            interest = self.extract_interest_data(unpacked_data)
            self.admission_control.handle_new_interest_request(interest)
        if(type == Request_Type.CONFIG):
            #do config
            unpacked_data = struct.unpack('=1L1I', data)
            print("msg=", unpacked_data)
            self.admission_control.config(unpacked_data[0], unpacked_data[1])

    def extract_sensor_data(self, data):
        x = data[Index.X]
        y = data[Index.Y]
        radius = data[Index.RADIUS]
        return Sensor(x, y, radius)

    def extract_interest_data(self, data):
        x = data[Index.X]
        y = data[Index.Y]
        radius = data[Index.RADIUS]
        period = data[Index.PERIOD]
        expiry = data[Index.EXPIRY]
        ref = data[Index.REF]
        return Interest(x, y, radius, period, expiry, ref)

    def write(self, data):
        self.serial.write(data)
