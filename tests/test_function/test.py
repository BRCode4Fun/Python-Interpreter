def factorial(n):
   if n == 1:
       return n
   else:
       return n * factorial(n-1)

print(factorial(4))
print(factorial(5))

def fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n-1) + fibonacci(n-2)

n = 0
while n <= 7:
    print(fibonacci(n))
    n += 1

def a(m):
    t = 3
    def k(g):
        return g
    def b(n):
        def b(l):
            return l
        def c(p):
            return m + n + p + t
        return c(3) + b(10)
    return b(4) + k(5)
print(a(3))

def count_inc(n, m):
    if n > m:
        return;
    print(n)
    count_inc(n+1, m)

count_inc(1, 10)

def count_dec(n, m):
    if n > m:
        return;
    count_dec(n+1, m)
    print(n)
    
count_dec(1, 10)

def f(n):
    def g(m):
        def h(o):
            return n + m + o
        return h
    return g

print(f(3)(4)(5))
