#blink program

#importing Pin class from machine module
from machine import Pin
#importing sleep class from time module
from time import sleep

#declaring built in led pin as output
led = Pin(2, Pin.OUT)
#starting a continuous loop
while True:
  #inverting led pin output in each iteration of the loop
  led.value(not led.value()) 
  sleep(0.5) #500ms delay
