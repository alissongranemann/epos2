import math
from math import hypot

class Math_Utils:

    def get_distance_into(one, other):
        return math.hypot(one.x - other.x, one.y - other.y)
