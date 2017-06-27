from Sensor_Rtree import Sensor_Rtree
from Sensor_Graph import Sensor_Graph
from Sensor import Sensor

class TSTP_WSN:

    rtree = Sensor_Rtree()
    graph = Sensor_Graph()
    gateway = Sensor(0, 0, 10)

    def add_sensor(self, sensor):
        if(self.graph.contains(sensor)):
            print("Sensor already inserted on TSTP")
            return

        self.rtree.add_sensor(sensor)
        self.graph.add_sensor(sensor)

        if(self.graph.len() == 1):
            return

        reachable_sensors = self.rtree.get_sensors_in_range(sensor);
        self.graph.add_edges(sensor, reachable_sensors)

    def add_interest(self, interest):
        print ("interest added on TSTP")
        self.graph.draw()

    def get_sensors_in_range(self, coordinate_object):
        return self.rtree.get_sensors_in_range(coordinate_object)

    def get_nearest_sensor(self, sensors_in_region):
        return self.rtree.get_nearest_sensor(self.gateway, sensors_in_region)

    def shortest_path(self, coordinate_object):
        return self.graph.shortest_path(self.gateway, coordinate_object)
