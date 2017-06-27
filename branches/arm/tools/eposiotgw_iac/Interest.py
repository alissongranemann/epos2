from Coordinate_Object import Coordinate_Object

class Interest(Coordinate_Object):

    def __init__(self, x, y, r, period, expiry):
        Coordinate_Object.__init__(self, x, y)
        self.range = r
        self.period = period
        self.expiry = expiry
