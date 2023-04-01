#include "Arduino.h"

 int CLOCK_PIN, DATA_PIN;
 int counter = 0,  inf, minV, maxV;  
 //bool st;
 int aState,  aLastState;

void rotary_init(const int clock, const int data) // Set the clock_PIN & data_PIN pin
 { 
   CLOCK_PIN=clock; DATA_PIN=data;
   Serial.print("Clock_PIN pin: "); Serial.print(CLOCK_PIN); Serial.print("  Data_PIN pin: "); Serial.println(DATA_PIN);
   pinMode (CLOCK_PIN,INPUT);
   pinMode (DATA_PIN,INPUT);
   aLastState = digitalRead(CLOCK_PIN); 
 } 

void setInf(int INF) // Set the increament/decreament factor
  { 
    inf = INF; Serial.print("Incremental factor set to: "); Serial.println(inf);
  }
void setMinMax(int MINv, int MAXv) // Set the Minimum & Maximum value to count
  { 
    minV = MINv; maxV = MAXv; 
    Serial.print("Min value set to: "); Serial.print(minV); Serial.print("  Max value set to: "); Serial.println(maxV);
  }

void setValues(int INF, int MINv, int MAXv) //Set the Minimum, Maximum value of counter & increament/decreament factor at a time
  {
    setInf(INF); setMinMax(MINv, MAXv);
    Serial.print("Incremental factor set to: "); Serial.println(inf);
    Serial.print("Min value set to: ");          Serial.println(minV);
    Serial.print("Max value set to: ");          Serial.println(maxV);
  }

void setCounter(int NCV) // Mannual overwrite the counter value
  {
    if(NCV>20000) NCV=20000;
    counter = NCV;
    Serial.print("  New Counter: "); Serial.println(counter);
  }

int getPos() //return the changed counter value 
  {
    aState = digitalRead(CLOCK_PIN); 
    if (aState != aLastState)
      {     
        if (digitalRead(DATA_PIN) != aState) 
          { 
            counter= counter+inf;
            if(counter > maxV) counter=counter-inf;
            else               counter=counter;
          } 
        else 
          {
            counter=counter-inf;
            if(counter < minV) counter= counter+inf;
            else               counter=counter;
          }
         Serial.print("Position: ");
         Serial.println(counter); Serial.println();
      } 
    aLastState = aState;
    return counter;
  }

int getCV() //return the current counter value
 {
   //Serial.print("--Previous Counter: "); Serial.print(counter);
   return counter;
 }

unsigned long  calTP(int mIn)
  {
    unsigned long totalTime;
    if(mIn == 60) //it is in RPM mode
      { totalTime = 1000000/(counter/60); }
    else if (mIn == 1) // it is in Freq. mode
      { totalTime = 1000000/counter;}
    
    Serial.print("Time period: "); Serial.println(totalTime);
    return totalTime;
  }