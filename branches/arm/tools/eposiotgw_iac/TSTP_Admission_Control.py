from Serial_Manager import Serial_Manager
from Burden_Estimator import Burden_Estimator
from TSTP_WSN import TSTP_WSN

class TSTP_Admission_Control:

    def __init__(self, test_mode = False):
        self.wsn = TSTP_WSN()
        self.estimator = Burden_Estimator(self.wsn)
        self.test_mode = test_mode
        if(not test_mode):
            self.serial_manager = Serial_Manager(self)

    def handle_new_sensor_request(self, sensor):
        isAceptable = self.estimator.is_sensor_aceptable(sensor)
        if(isAceptable):
            self.wsn.add_sensor(sensor)
        self.notify(isAceptable)

    def handle_new_interest_request(self, interest):
        isAceptable = self.estimator.is_interest_aceptable(interest)
        if(isAceptable):
            self.wsn.add_interest(interest)
        self.notify(isAceptable)

    def notify(self, isAceptable):
        if(not self.test_mode):
            self.serial_manager.write(isAceptable)
