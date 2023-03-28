import math

a1 = 10
a2 = 13
d = 10
m=10

x = 1
y = 7

c=math.sqrt((x**2)+(y**2))
e=math.sqrt(((d-x)**2)+(y**2))

t1 = math.atan(y/x) + math.acos(((a1**2) + (c**2) - (a2**2))/(2*a1*c))
t2 = (math.atan(y/(d-x)) + math.acos(((a1**2) + (e**2) - (a2**2))/(2*a1*e)))

print(math.degrees(t1))
print(180 - math.degrees(t2))