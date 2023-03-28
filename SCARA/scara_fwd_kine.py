import math


t1 = 90
t2 = 90


l1 = 10
l2 = 13
dj = 10

a = l1*math.cos(math.radians(t1))
b = l1*math.sin(math.radians(t1))
c = dj+(l1*math.cos(math.radians(t2)))
d = l1*math.sin(math.radians(t2))

#print(c)
#print(d)

k1 = c - a
k2 = d - b
k3 = c + a
k4 = d + b

j1 = c**2 + d**2
j2 = a**2 + b**2

db = math.sqrt(k1**2 +k2**2)

phi = math.acos(db/(2*l2))
phi1 = math.acos((db**2 + l1**2 - j1)/(2*db*l1))

s = l1**2 - 2*l1*l2*math.cos(phi+phi1) + ((j1+j2)/2)

y = (2*k1*s - (k3*(j1-j2)))/(2*(k1*k4-k2*k3))
x = (j1-j2-2*k2*y)/(2*k1)

print(x)
print(y)

