from TSTP_Admission_Control import TSTP_Admission_Control
from Sensor import Sensor
from Interest import Interest
import struct
import random

mac_period = 116

admission_control = TSTP_Admission_Control(False)
admission_control.config(20, mac_period)


def create_sensor(x, y):
    return Sensor(x, y, 20)

def create_interest(x, y, radius, period, expiry):
    return Interest(x, y, 10, period, expiry, random.randint(0,200))

def create_sensor_request(x, y):
    admission_control.handle_new_sensor_request(create_sensor(x, y))

def create_interest_request(x, y, radius, period, expiry):
    admission_control.handle_new_interest_request(create_interest(x, y, radius, period, expiry))

def reset_admission_control():
    pass

def test_add_sensors_before_interests():
    create_sensor_request(0, 0)
    create_sensor_request(9, 0)
    create_sensor_request(16, 0)
    create_sensor_request(21, 0)

    create_interest_request(23, 0, 10, 2, 0)

def test_add_interest_before_sensors():
    create_interest_request(23, 0, 10, 1 * mac_period, 0)
    create_interest_request(25, 0, 10, 2 * mac_period, 0)
    create_interest_request(25, 0, 10, 2 * mac_period, 0)

    create_sensor_request(0, 0)
    create_sensor_request(9, 0)
    create_sensor_request(16, 0)
    create_sensor_request(21, 0)

def test_two_way_network():

    create_sensor_request(10, 17)
    create_sensor_request(20, 15)
    create_sensor_request(32, 30)
    create_sensor_request(28, 7)
    create_sensor_request(41, 42)
    create_sensor_request(37, -5)

    create_interest_request(41, 42, 5, 2 * mac_period, 0)
    create_interest_request(37, 0, 5, 4 * mac_period, 0)

    admission_control.wsn.draw()

def test_interest_not_aceptable():

    create_sensor_request(10, 17)
    create_sensor_request(20, 15)
    create_sensor_request(32, 30)
    create_sensor_request(28, 7)
    create_sensor_request(41, 42)
    create_sensor_request(37, -5)

    create_interest_request(41, 42, 5, 2 * mac_period, 0)
    create_interest_request(37, 0, 5, 4 * mac_period, 0)
    create_interest_request(37, 0, 5, 3 * mac_period, 0)

    admission_control.wsn.draw()

def run_tests():
    # test_add_sensors_before_interests()
    test_add_interest_before_sensors()
    # test_two_way_network()
    # test_interest_not_aceptable()

if __name__ == "__main__":
    run_tests()
