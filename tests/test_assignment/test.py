x = 10.5
print(x)

y = True
print(y)

z = None
print(z)

a = 10
b = a / 2
c = 3 * a + b
e = d = c
print(e)

a = 3.14 + (2.71 if False else 1.618)
print(a)

b = False
c = True if b else False
print(c)

d = 10.5 if (1.5 > 2.0) else 7.2
print(d)

e = None if (5.0 != 5.0) else 100
print(e)

f = 0.0
f = f + 5.5
print(f)

g = True
g = False if (1.0 < 0.0) else True
print(g)

h = None
h = 42.0 if (True and False) else None
print(h)

# Augmented Assignment

x = 5
x += 3
print(x)

y = 10
y -= 4
print(y)

z = 3
z *= 2
print(z)

w = 20
w /= 5
print(w)

a = 5
a &= 3
print(a)

b = 5
b |= 3
print(b)

c = 5
c ^= 3
print(c)

d = 1
d <<= 3
print(d)

e = 8
e >>= 2
print(e)

