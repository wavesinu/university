import math


# Vector에 관련된 실질적인 속성과 기능들만 있음
class MyVector:

    def __init__(self, x, y):
        self.x = x
        self.y = y

        norm = 0

    def __add__(self, other):
        return MyVector(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return MyVector(self.x - other.x, self.y - other.y)

    def __mul__(self, scalar):
        return MyVector(scalar * self.x, scalar * self.y)

    def setPos(self, x, y):
        self.x = x
        self.y = y

    def normalize(self):
        self.norm = self.getMagnitude()
        self.x = self.x / self.norm
        self.y = self.y / self.norm

    def getMagnitude(self):
        return math.sqrt(math.pow(self.x, 2) + math.pow(self.y, 2))

    def getState(self):
        return self.x, self.y

    def vec(self):
        return [self.x, self.y]

# a = MyVector(1, 2)
# b = MyVector(4, 5)
# c = (b + a)
# print(c.vec())
