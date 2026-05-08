class Number:
    def __init__(self, n):
        self.n = n

    def __lt__(self, other):
        return self.n < other.n

    def __eq__(self, other):
        return self.n == other.n

    def __str__(self):
        return str(self.n)

a = Number(2)
b = Number(5)
c = Number(2)

print(a < b)
print(a > b)
print(b > a)
print(a <= b)
print(b <= a)
print(a >= b)
print(b >= a)
print(a != c)
print(a != b)
print(a == c)
