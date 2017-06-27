from TSTP_Admission_Control import TSTP_Admission_Control
from Sensor import Sensor
from Interest import Interest
import struct

admission_control = TSTP_Admission_Control(True)
admission_control.config(10, 116)

def create_sensor(x, y):
    return Sensor(x, y, 10)

def create_interest(x, y, period, expiry):
    return Interest(x, y, 10, period, expiry)

def create_sensor_request(x, y):
    admission_control.handle_new_sensor_request(create_sensor(x, y))

def create_interest_request(x, y, period, expiry):
    admission_control.handle_new_interest_request(create_interest(x, y, period, expiry))

def reset_admission_control():
    pass

def test_sensors_one_interest():
    create_sensor_request(0, 0)
    create_sensor_request(9, 0)
    create_sensor_request(16, 0)
    create_sensor_request(21, 0)

    create_interest_request(23, 0, 2, 0)

def test_add_interest():
    create_interest_request(23, 0, 2, 0)

def run_tests():
    test_sensors_one_interest()
    test_add_interest()

if __name__ == "__main__":
    run_tests()
