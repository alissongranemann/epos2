# from TSTP_Admission_Control import TSTP_Admission_Control as Admission_Control
from Interest import Interest
from enum import IntEnum, unique
from Sensor import Sensor
from Serial import Serial

@unique
class Request_Type(IntEnum):
    SENSOR = 0
    INTEREST = 1

@unique
class Index(IntEnum):
    TYPE = 0
    X = 1
    Y = 2
    Z = 3
    RADIUS = 4
    PERIOD = 5
    EXPIRY = 6

class Serial_Manager:

    def __init__(self, Admission_Control):
        self.admission_control = Admission_Control;
        self.serial = Serial(self)

    def write(self, data):
        if data:
            print("Writing on serial succes")
        else:
            print("Writing on serial fail")

    def handle_serial_request(self, data):
        print(data)
        if(data[Index.TYPE] == Request_Type.SENSOR):
            sensor = self.extract_sensor_data(data)
            self.admission_control.handle_new_sensor_request(sensor)
        else:
            interest = self.extract_interest_data(data)
            self.admission_control.handle_new_interest_request(interest)

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
        return Interest(x, y, radius, period, expiry)

    def write(self, data):
        self.serial.write(data)

    def new_sensor_request(self, x, y):
        print("Serial recevied new sensor request with coordinates", x, y)
        sensor = Sensor(x, y)
        self.admission_control.handle_new_sensor_request(sensor)

    def new_interest_request(self, x, y, radius, period, expiracy):
        print("Serial recevied new interest request with coordinates", x, y)
        interest = Interest(x, y, radius, period, expiracy)
        self.admission_control.handle_new_interest_request(interest)
