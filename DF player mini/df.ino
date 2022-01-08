

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#define pp 3
#define vu 4
#define vd 5
#define nxt 6
#define prv 7


int v= 20, pv=0;

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  
  pinMode(pp,INPUT_PULLUP);
  pinMode(nxt,INPUT_PULLUP);
  pinMode(prv,INPUT_PULLUP);
  pinMode(vu,INPUT_PULLUP);
  pinMode(vd,INPUT_PULLUP);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(v);  //Set volume value. From 0 to 30
  //myDFPlayer.play(1);  //Play the first mp3
}

void loop()
{
  Serial.println(pv);
  if(digitalRead(pp)==0){
    if(pv == 0){
    myDFPlayer.start();
    pv=1;
    }
    else if(pv == 1){
    myDFPlayer.pause();
    pv=0;
    }
  }
  if(digitalRead(nxt)==0){
    myDFPlayer.next();
  }
  if(digitalRead(prv)==0){
    myDFPlayer.previous();
    
  }
  if(digitalRead(vu)==0 && v<30){
    v=v+1;
    myDFPlayer.volume(v);
  }
  if(digitalRead(vd)==0 && v>0){
    v=v-1;
    myDFPlayer.volume(v);
  }
  delay(200);
  
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
