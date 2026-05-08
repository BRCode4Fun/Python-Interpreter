def mark(n):
    print("mark")
    return n

print(True or mark(1))
print(False and mark(2))
print(False or mark(3))
print(True and mark(4))

class Adder:
    def __init__(self, base):
        self.base = base

    def __call__(self, n):
        return self.base + n

class Scale:
    def double(self, n):
        return n * 2

def add(a, b):
    return a + b

print(add(2, 3))

adder = Adder(10)
print(adder(5))

scale = Scale()
fn = scale.double
print(fn(6))
print(scale.double(7))
