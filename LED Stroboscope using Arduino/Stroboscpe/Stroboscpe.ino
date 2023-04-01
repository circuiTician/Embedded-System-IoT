#include "rotary.h"
#include "disp.h"


const int SWR = 3, MODE = 9, HALF = 8, DOUBLE = 7, LED = 10,trig = 6;
int duty_p = 5;

int MUL=1, MIN = 60, MAX = 20000;
int counter_value=60;
unsigned long debounce =250 ,pm, interval, timePeriod, onTime, offTime;
int lastState = 1, Pcounter;


void setup() 
 { 
    Serial.begin (115200);  
    
    setValues(MUL, MIN, MAX);
    setCounter(MIN);
    rotary_init(5, 4);
    //disp_init(); 
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
    pinMode(trig,INPUT_PULLUP);    
    pinMode(SWR,INPUT); 
    pinMode(MODE,INPUT_PULLUP);   
    pinMode(HALF,INPUT_PULLUP);  
    pinMode(DOUBLE,INPUT_PULLUP);  
    pinMode(LED,OUTPUT); digitalWrite(LED, 0);
    
    welcome(); 
    //delay(3000);
    rpm_disp();mult(1);
    
    
  } 

void loop() 
 { 
   getPos();  // get the changed counter value

   if(!digitalRead(SWR)) //Change the increament/decreament factor
      {
        delay(debounce);
        MUL=MUL*10;
        if(MUL>100) MUL = 1;
        else MUL=MUL;
        setInf(MUL);
        mult(MUL);    
         // change mul factor       
      }

   if(!digitalRead(MODE)) //change the mode in between Freq. & RPM
      {
        delay(debounce);
        if(MIN == 60) 
          {
            MIN = 1; MAX = 334;
            Serial.println("Mode: Frequency");
            //mode change
            setMinMax(MIN,MAX); 
            setCounter((getCV()/60));
            freq_disp();   mult(MUL);           
          }
        else if(MIN == 1) 
          {
            MIN = 60; MAX = 20000;
            Serial.println("Mode: RPM");
            //mode change                        
            setMinMax(MIN,MAX); 
            setCounter((getCV()*60));
            rpm_disp(); mult(MUL);  
          }
          
      }

   if(!digitalRead(HALF))
      {
        delay(debounce);
        if(MIN == 60)
          {
            if(getCV()>=180) {setCounter((getCV()/3));}
          }
        else if(MIN == 1)
          {
            if(getCV()>=3) {setCounter((getCV()/3));}
          }
      }

   if(!digitalRead(DOUBLE))
      {
        delay(debounce);
        if(MIN == 60)
          {
            if(getCV()<=6666) {setCounter((getCV()*3));}
          }
        else if(MIN == 1)
          {
            if(getCV()<=111) {setCounter((getCV()*3));}
          }
      }
      
     
   if (getCV() != Pcounter)
    {
      //MUL
      counter_value = getCV();
      disp_var(counter_value);   
    
      timePeriod = calTP(MIN);
      onTime = (timePeriod*duty_p)/100;
      offTime = timePeriod - onTime;
      Pcounter = getCV();      
    }
  

    if (lastState == 1) { interval = onTime;  }
    else                { interval = offTime; }
    if ((micros()-pm)>= interval)
      {
        digitalWrite(LED,((!lastState) and (not(digitalRead(trig))) ));
        lastState=!lastState;
        pm=micros();
      }

 }// loop