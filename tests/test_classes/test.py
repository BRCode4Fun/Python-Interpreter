class Number:
    def __init__(self, n):
        self._n = n
    
    def __add__(self, other):
        return Number(self._n + other.getN())
    
    def getN(self):
        return self._n
    
n1 = Number(3)
n2 = Number(5)
n3 = Number(6)
n4 = Number(9)

n5 = n1.__add__(n2).__add__(n3).__add__(n4)

print(n5.getN())


