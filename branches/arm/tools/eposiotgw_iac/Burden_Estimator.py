class Burden_Estimator:

    def __init__(self, wsn):
        self.wsn = wsn;

    def is_sensor_aceptable(self, sensor):
        return True

    def is_interest_aceptable(self, interest):
        sensors_in_region = self.wsn.get_sensors_in_range(interest)
        print("Sensors in region of interest:")
        for s in sensors_in_region:
            print(s)

        nearest_sensor = self.wsn.get_nearest_sensor(sensors_in_region)
        print("The nearest sensor of gateway in region is: ", nearest_sensor)

        shortest_path = self.wsn.shortest_path(nearest_sensor)
        print("The shortest_path to nearest sensor is:")

        for s in shortest_path:
            print(s)

        print("The number of hops to nearest sensor is:", len(shortest_path))

        aceptable = self.estimate_path_burden(shortest_path)

        return aceptable

    def estimate_path_burden(self, path):
        # Itera do sensor que destino até o gateway
        for i in range(len(path), 0, -1):
            sensor = path[i - 1]
            burden = self.estimate_sensor_burden(sensor)
            if(burden < 100):
                sensor.set_burden(burden)
            else:
                from_sensor = i - 1
                self.restore_path_estimation(path, from_sensor)
                return False
        return True

    def estimate_sensor_burden(self, sensor):
        print('The burden on sensor', sensor, 'will be calculated soon')
        return 1

    def restore_path_estimation(self, path, init):
        for i in range(init, len(path)):
            sensor = path[i]
            sensor.restore_burden
