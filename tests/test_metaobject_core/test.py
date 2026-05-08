print(type(object) == type)
print(type(type) == type)
print(issubclass(type, object))
print(issubclass(object, type))
print(isinstance(object, type))
print(isinstance(type, type))
print(isinstance(type, object))
print(isinstance(object, object))

class A:
    pass

a = A()
print(type(a) == A)
print(isinstance(a, A))
print(isinstance(A, type))
print(issubclass(A, object))
