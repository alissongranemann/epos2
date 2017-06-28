from Serial_Manager import Serial_Manager
from Burden_Estimator import Burden_Estimator
from TSTP_WSN import TSTP_WSN
import struct

class TSTP_Admission_Control:

    def __init__(self, test_mode = False):
        self.wsn = TSTP_WSN()
        self.estimator = Burden_Estimator(self.wsn)
        self.test_mode = test_mode
        if(not self.test_mode):
            self.serial_manager = Serial_Manager(self)

    def config(self, sink_radius, mac_period):
        self.estimator.set_mac_period(mac_period)
        self.wsn.set_gateway_range(sink_radius)

    def handle_new_sensor_request(self, sensor):
        isAceptable = self.estimator.is_sensor_aceptable(sensor)
        if(isAceptable):
            self.wsn.add_sensor(sensor)
        #self.notify(isAceptable)
        self.update_interests_without_response()

    def handle_new_interest_request(self, interest):
        print("Admission control recevied a new interest", interest)
        isAceptable = self.estimator.is_interest_aceptable(interest)
        if(isAceptable):
            self.wsn.add_interest(interest)
        self.notify(isAceptable, interest.ref)

    def notify(self, isAceptable, interest_ref):
        if(not self.test_mode):
            print("interest_ref=", interest_ref)
            msg = bytes(str(interest_ref), 'ascii')
            self.serial_manager.write(msg + b'X')
            self.serial_manager.write(bytes(str(1), 'ascii'))

    def update_interests_without_response(self):
        unnaceptable_interests = self.estimator.get_unnaceptable_interests()
        for i in unnaceptable_interests:
            # self.notify(isAceptable = False, i.ref)
            pass
