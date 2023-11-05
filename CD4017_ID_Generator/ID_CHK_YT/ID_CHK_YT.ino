#include "IDlib.h"
const int Pulse_Pin = 3;
const int Read_Pin  = 4;
const int Reset_Pin = 5;

void setup()
  {
    Serial.begin(115200);
    delay(250);
    int chip_ID = getID(Pulse_Pin, Read_Pin, Reset_Pin);
    Serial.print("ID: "); Serial.println(chip_ID);
    if(chip_ID == 23)
      {
        Serial.println("ID matched");
      }
    else 
      {
        Serial.println("ID not matched");
        while (1) 
        {
          Serial.print("-"); delay(1000);
          Serial.print("/"); delay(1000);
        }
      }
    
  }

void loop()
  {
    Serial.print("Sensor Reading: ");
    Serial.println(analogRead(A0));
    delay(250);
  }