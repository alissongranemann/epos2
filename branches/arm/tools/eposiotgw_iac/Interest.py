from Coordinate_Object import Coordinate_Object

class Interest(Coordinate_Object):

    def __init__(self, x, y, r, period, expiry, ref):
        Coordinate_Object.__init__(self, x, y)
        self.range = r
        self.period = period
        self.expiry = expiry
        self.ref = ref

    def __str__(self):
     return "("+ str(self.x) + ", " + str(self.y) + ", " + str(self.range) + ", " + str(self.period) + ", " + str(self.ref) + ")"

    def __hash__(self):
        return hash((self.ref))

    def __eq__(self, other):
        if type(other) is type(self):
            return self.ref == other.ref
        return False
