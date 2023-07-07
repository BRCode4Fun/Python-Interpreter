# Left shift operator
x = 10
print(x << 1)  # Equivalent to multiplying x by 2^1
print(x << 2)  # Equivalent to multiplying x by 2^2
print(x << 3)  # Equivalent to multiplying x by 2^3

# Right shift operator
y = 16
print(y >> 1)  # Equivalent to dividing y by 2^1
print(y >> 2)  # Equivalent to dividing y by 2^2
print(y >> 3)  # Equivalent to dividing y by 2^3

# Shift an Odd number
z = 15
print(z >> 0) # Shifting by 0 has no effect
print(z >> 1)
print(z >> 2)
#print(z >> -1) # Shifting by negative value

# Left shift edge case
z = 1
print(z << 31)  # Shifting left by the number of bits in an integer (32 bits)

# Right shift edge case
w = -2147483648  # Smallest 32-bit signed integer
print(w >> 31)  # Shifting right by the number of bits in an integer (32 bits)

# Right shift in while loop
count = 16
while count > 0:
    print(count)
    count = count >> 1

