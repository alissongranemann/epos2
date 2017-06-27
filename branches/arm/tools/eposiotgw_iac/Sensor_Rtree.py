from rtree import index
from Math_Utils import Math_Utils as math

class Sensor_Rtree:

    rtreeIndex = index.Index()
    id = 0;

    def add_sensor(self, sensor):
        self.rtreeIndex.insert(self.id, self.point2rect(sensor.x , sensor.y), obj = sensor)
        self.id += 1

    def point2rect(self, x, y):
        return [x ,y ,x ,y];

    def get_sensors_in_range(self, sensor):
        intersection = self.rtreeIndex.intersection(self.build_intersection_rectangle(sensor.x, sensor.y, sensor.range), True)
        sensors_in_range = []
        for s in intersection:
            if(s.object != sensor and math.get_distance_into(sensor, s.object) <= s.object.range):
                sensors_in_range.append(s.object)
        return sensors_in_range

    def get_nearest_sensor(self, sensor):
        return list(s.object for s in self.rtreeIndex.nearest(self.point2rect(sensor.x, sensor.y), 1, True))

    def get_nearest_sensor(self, sensor, sensors_in_region):
        distance = 9999999999;
        nearest_sensor = None
        for s in sensors_in_region:
            distance_to_s = math.get_distance_into(sensor, s)
            if(distance_to_s < distance):
                distance = distance_to_s
                nearest_sensor = s
        return nearest_sensor

    # Retorna um retangulo com centro em x,y e extremidades em x,y + r onde r é o raio
    # Esse método constroi um retangulo que contenha uma circunferencia com centro em x,y e raio r
    def build_intersection_rectangle(self, x, y, r):
        return (x-r, y-r, x+r, y + r)
