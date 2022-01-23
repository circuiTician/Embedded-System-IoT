import time
from pinpong.board import Board
from pinpong.libs.dfrobot_dri0050 import DRI0050 # Import DRI0050 library from libs 

#Board("RPi").begin()  #RPi Linux platform 
Board("Win").begin() #windows platform

#pwmd = DRI0050(port="/dev/ttyUSB0") #RPi Linux platform
pwmd = DRI0050(port="COM3")  #Windows platform

#print("version=0x{:x}, addr=0x{:x}".format(pwmd.get_version(), pwmd.get_addr()))
#print("pid=0x{:x}, vid=0x{:x}".format(pwmd.get_vid(), pwmd.get_pid()))

pwmres = 0.1
pwmD = 0

while True:
        pwmres = 0.1
        pwmD = 0
        
        for i in range (0,10):
            
                pwmd.set_freq(1000) #(183HZ-46875HZ)
                pwmD = pwmD + pwmres
                pwmd.set_duty(pwmD)#(0%-100%)
              
                pwmd.set_enable(1)
                time.sleep(0.5)
            

        for i in range (0,10):
            
                pwmd.set_freq(1000) #(183HZ-46875HZ)
                pwmD = pwmD - pwmres
                pwmd.set_duty(pwmD)#(0%-100%)
                  
                pwmd.set_enable(1)
                time.sleep(0.5)
            
        pwmd.set_enable(0)
        time.sleep(1.5)
