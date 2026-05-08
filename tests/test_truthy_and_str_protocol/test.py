class WithLen:
    def __init__(self, size):
        self.size = size

    def __len__(self):
        return self.size

class WithBool:
    def __init__(self, flag):
        self.flag = flag

    def __bool__(self):
        return self.flag

    def __str__(self):
        return "WithBool:" + str(self.flag)

print(bool(WithLen(0)))
print(bool(WithLen(3)))
print(bool(WithBool(False)))
print(bool(WithBool(True)))
print(str(None))
print(str(True))
print(str(2))
print(str(2.0))
print(str(type))
print(str(WithBool(True)))
