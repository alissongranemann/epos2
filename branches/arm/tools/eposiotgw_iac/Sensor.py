from Coordinate_Object import Coordinate_Object

class Sensor(Coordinate_Object):

    def __init__(self, x, y, range):
        Coordinate_Object.__init__(self, x, y)
        self.range = range;
        self.burden = 0;
        self.backup_burden = 0

    def set_burden(self, burden):
        self.backup_burden = self.burden
        self.burden = burden

    def restore_burden(self):
        self.burden = self.backup_burden
        self.backup_burden = 0

    def __hash__(self):
        return hash((self.x, self.y))

    def __eq__(self, other):
        if type(other) is type(self):
            return self.x == other.x and self.y == other.y
        return False

    def __str__(self):
     return "("+ str(self.x) + ", " + str(self.y) + ")"
