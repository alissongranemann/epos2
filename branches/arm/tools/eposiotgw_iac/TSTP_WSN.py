from Sensor_Rtree import Sensor_Rtree
from Sensor_Graph import Sensor_Graph
from Sensor import Sensor
from TSTP_Exception import NoSensorInRegionException

class TSTP_WSN:

    def __init__(self):
        self.rtree = Sensor_Rtree()
        self.graph = Sensor_Graph()
        self.gateway = Sensor(0, 0, 10)
        self.add_sensor(self.gateway)

    def add_sensor(self, sensor):
        if(self.graph.contains(sensor)):
            return

        self.rtree.add_sensor(sensor)
        self.graph.add_sensor(sensor)

        if(self.graph.len() == 1):
            return

        reachable_sensors = self.rtree.get_sensors_in_range(sensor);
        self.graph.add_edges(sensor, reachable_sensors)

    def add_interest(self, interest):
        pass

    def get_sensors_in_range(self, coordinate_object):
        sensors_in_range = self.rtree.get_sensors_in_range(coordinate_object)
        if(len(sensors_in_range) == 0):
            raise NoSensorInRegionException
        return sensors_in_range

    def get_nearest_sensor(self, sensors_in_region):
        return self.rtree.get_nearest_sensor(self.gateway, sensors_in_region)

    def shortest_path(self, coordinate_object):
        return self.graph.shortest_path(self.gateway, coordinate_object)

    def draw(self):
        self.graph.draw()

    def set_gateway_range(self, range):
        self.gateway.range = range;
