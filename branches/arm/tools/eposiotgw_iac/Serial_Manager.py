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

    def __init__(self):
        self.admission_control = None;
        self.serial = None

    def set_admission_control(self, admission_control):
        self.admission_control = admission_control
        self.serial = Serial()
        self.serial.set_serial_manager(self)

    def handle_serial_request(self, type, data):
        if(type == Request_Type.SENSOR):
            unpacked_data = struct.unpack('=3l1L', data)
            sensor = self.extract_sensor_data(unpacked_data)
            self.admission_control.handle_new_sensor_request(sensor)
        if(type == Request_Type.INTEREST):
            unpacked_data = struct.unpack('=3l1L2Q1i', data)
            interest = self.extract_interest_data(unpacked_data)
            self.admission_control.handle_new_interest_request(interest)
        if(type == Request_Type.CONFIG):
            unpacked_data = struct.unpack('=1L1I', data)
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

    def write_response_message(self, result, interest_ref):
        self.write_header(1);
        self.write_msg(result, interest_ref)

    def write_header(self, times):
        header = bytes('^', 'ascii') + bytes(str(times), 'ascii') + bytes('$', 'ascii')
        self.serial.write(header)

    def write_msg(self, result, interest_ref):
        ref = bytes(str(interest_ref), 'ascii')
        msg = ref + b'X' + bytes(str(int(result)), 'ascii')
        self.serial.write(msg)


