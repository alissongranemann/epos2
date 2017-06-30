import math
from networkx.exception import NetworkXNoPath
from TSTP_Exception import NoSensorInRegionException
from copy import deepcopy


class Burden_Estimator:

    def __init__(self, wsn):
        self.wsn = wsn;
        self.interests_without_response = []

    def set_mac_period(self, mac_period):
        self.mac_period = mac_period

    def is_sensor_aceptable(self, sensor):
        return True

    def is_interest_aceptable(self, interest):
        try:
            shortest_path = self.get_shortest_path(interest)
            aceptable = self.estimate_path_burden(shortest_path, interest.period)
            return aceptable
        except (NoSensorInRegionException, NetworkXNoPath):
            print("There are no path to", interest, "addind interest to interests_without_response list")
            self.interests_without_response.append(interest)
            return True

    def get_shortest_path(self, interest):
        sensors_in_region = self.wsn.get_sensors_in_range(interest)
        print("Sensors in region of interest: ", end = '')
        for s in sensors_in_region:
            print(s, end = '')
        print()

        nearest_sensor = self.wsn.get_nearest_sensor(sensors_in_region)
        print("The nearest sensor of gateway in region is: ", nearest_sensor)

        shortest_path = self.wsn.shortest_path(nearest_sensor)
        print("The shortest_path to nearest sensor is: ", end = ''),

        for s in shortest_path:
            print("->",s, end = '')
        print()

        return shortest_path

    def estimate_path_burden(self, path, period):
        # Itera do sensor destino até o gateway
        for i in range(len(path), 0, -1):
            sensor = path[i - 1]
            burden = self.estimate_sensor_burden(sensor, period)
            if(burden <= 100):
                sensor.set_burden(burden)
            else:
                from_sensor = i
                self.restore_path_estimation(path, from_sensor)
                return False
        return True

    def estimate_sensor_burden(self, sensor, period):
        sensor_burden = math.ceil((self.mac_period / period) * 100) + sensor.burden
        print('The burden on sensor', sensor, 'is ', sensor_burden)
        return sensor_burden

    def restore_path_estimation(self, path, init):
        for i in range(init, len(path)):
            sensor = path[i]
            print('The burden on sensor', sensor, 'is ', sensor.burden, "will be restored to", sensor.backup_burden)
            sensor.restore_burden()

    def get_unnaceptable_interests(self):
        unnaceptable_interests = []
        iteration_set = deepcopy(self.interests_without_response)
        if(len(self.interests_without_response) > 0):
            print('checking if unnaceptable interests can be admited')
        for interest in iteration_set:
            try:
                shortest_path = self.get_shortest_path(interest)
                aceptable = self.estimate_path_burden(shortest_path, interest.period)
                if(not aceptable):
                    unnaceptable_interests.append(interest)
                self.interests_without_response.remove(interest)
            except (NoSensorInRegionException, NetworkXNoPath):
                print("The interest", interest, "cant be admited yet")
        if(len(unnaceptable_interests) > 0):
            print('the following interests worent admited', end = '')
            for i in unnaceptable_interests:
                print(i, end = '')
            print()
        return unnaceptable_interests
