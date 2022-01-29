#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#include <Wire.h>
#include <RtcDS3231.h>  //RTC library
#include <EEPROM.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define WIFI_SSID "****************"
#define WIFI_PASSWORD "****************"
#define API_KEY "****************e"
#define DATABASE_URL "****************" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define MD1 D3 
#define MD2 D4 
#define MD3 D5 
#define MD4 D6 

//RtcDS3231 rtcObject;              //Uncomment for version 1.0.1 of the rtc library
RtcDS3231<TwoWire> rtcObject(Wire); //Uncomment for version 2.0.0 of the rtc library

SoftwareSerial mySoftwareSerial(13, 15); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int AV1, AV2, AV3, pDay;
int AVR1, AVR2, AVR3;
int Years, Months, Days, Hours, Min, Sec,timeStamp, date;

String k, m1, m2, m3, iv;
int MS1[] = {1,3,0};
int MS2[] = {1,2,0};
int MS3[] = {1,3,4};
int M1, M2, M3;
int ind1, ind2, ind3;


int AH1, AM1, AH2, AM2, AH3, AM3, W;
int AHR1, AMR1, AHR2, AMR2, AHR3, AMR3;
int onv=1, ofv=0;

// value to be adjust according to your choice
int rt=1;// repeat time gap in min
unsigned long lm, HT = 5000; // time is in milisecond//// **It is the time how much the led will glow after alarm

void setup() 
{
 
 Serial.begin(115200);
 mySoftwareSerial.begin(9600);
 EEPROM.begin(4096);
 pinMode(MD1,OUTPUT); pinMode(MD2,OUTPUT); pinMode(MD3,OUTPUT); pinMode(MD4,OUTPUT);
 digitalWrite(MD1,ofv); digitalWrite(MD2,ofv); digitalWrite(MD3,ofv); digitalWrite(MD4,ofv); Serial.println("All off");
 Serial.println();
 delay(5000);
 
//_____________________________________DF player begin_________________________________________________
   
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
  
  myDFPlayer.volume(28);  //Set volume value. From 0 to 30
  
//_____________________________________RTC begin_________________________________________________
   
  rtcObject.Begin();     //Starts I2C
  
  /*RtcDateTime currentTime = RtcDateTime(2022, 01, 29, 20,35, 0); //define date and time object
  rtcObject.SetDateTime(currentTime); //configure the RTC with object*/

  //______________________________________Checking the date once____________________________________________
  delay(1000);
 RtcDateTime currentTime = rtcObject.GetDateTime(); 
 pDay = currentTime.Day(), DEC;

 Serial.print(currentTime.Year());Serial.print("/");Serial.print(currentTime.Month());Serial.print("/");Serial.print(currentTime.Day());Serial.print("-----");
 Serial.print(currentTime.Hour());Serial.print(":");Serial.print(currentTime.Minute());Serial.print(":");Serial.print(currentTime.Second());Serial.println();
 if(currentTime.Year() == 2000){digitalWrite(MD2,1); delay(200);digitalWrite(MD2,0); delay(200); digitalWrite(MD2,1); delay(200);digitalWrite(MD2,0);}
 else { myDFPlayer.play(5); delay(3050); } 
//_______________________________________EEPROM set______________________________________________
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  delay(5000);
  
  if (WiFi.status() == WL_CONNECTED)
 {
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(DATABASE_URL, API_KEY);
  Firebase.setDoubleDigits(5);
  delay(2000);
  
 if (Firebase.ready()) 
  { 
//_________________For Medicine set 1_____________________
    Firebase.getString(fbdo, "MS/MS1");
    k = fbdo.to<String>();
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS1[0]=M1; EEPROM.write(40,MS1[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS1[1]=M2; EEPROM.write(41,MS1[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS1[2]=M3; EEPROM.write(42,MS1[2]);
    //Serial.print("MS1 - "); Serial.print(MS1[0]); Serial.print(" - "); Serial.print(MS1[1]); Serial.print(" - "); Serial.println(MS1[2]);
     
//_________________For Medicine set 2_____________________
   Firebase.getString(fbdo, "MS/MS2");
    k = fbdo.to<String>();
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS2[0]=M1; EEPROM.write(20,MS2[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS2[1]=M2; EEPROM.write(21,MS2[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS2[2]=M3; EEPROM.write(22,MS2[2]); 
    //Serial.print("MS2 - "); Serial.print(MS2[0]); Serial.print(" - "); Serial.print(MS2[1]); Serial.print(" - "); Serial.println(MS2[2]);
     

//_________________For Medicine set 3_____________________
   Firebase.getString(fbdo, "MS/MS3");
    k = fbdo.to<String>();
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS3[0]=M1; EEPROM.write(30,MS3[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS3[1]=M2; EEPROM.write(31,MS3[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS3[2]=M3; EEPROM.write(32,MS3[2]);
    //Serial.print("MS3 - "); Serial.print(MS3[0]); Serial.print(" - "); Serial.print(MS3[1]); Serial.print(" - "); Serial.println(MS3[2]);
     
    //_________________For Alarm 1____________________
    Firebase.getString(fbdo, "T/AH1"); iv = fbdo.to<String>();
    AH1 = iv.toInt(); EEPROM.write(10,AH1);
    Firebase.getString(fbdo, "T/AM1"); iv = fbdo.to<String>();
    AM1 = iv.toInt(); EEPROM.write(11,AM1);
    if((AM1+rt) == 60) { AHR1=AH1+1; AMR1=0; }
    else               { AHR1=AH1; AMR1=AM1+rt; }
    
//_________________For Alarm 2____________________
    Firebase.getString(fbdo, "T/AH2"); iv = fbdo.to<String>();
    AH2 = iv.toInt(); EEPROM.write(12,AH2);
    Firebase.getString(fbdo, "T/AM2"); iv = fbdo.to<String>();
    AM2 = iv.toInt(); EEPROM.write(13,AM2);
    if((AM2+rt) == 60) { AHR2=AH2+1; AMR2=0; }
    else               { AHR2=AH2; AMR2=AM2+rt; }

//_________________For Alarm 3____________________
    Firebase.getString(fbdo, "T/AH3"); iv = fbdo.to<String>();
    AH3 = iv.toInt(); EEPROM.write(14,AH3);
    Firebase.getString(fbdo, "T/AM3"); iv = fbdo.to<String>();
    AM3 = iv.toInt(); EEPROM.write(15,AM3);
    if((AM3+rt) == 60) { AHR3=AH3+1; AMR3=0; }
    else               { AHR3=AH3; AMR3=AM3+rt; }
    
  }//fb ready
 }// have network

//_______________________________________no network_________________________________________
else{
  digitalWrite(MD3,1); delay(200);digitalWrite(MD3,0); delay(200); digitalWrite(MD3,1); delay(200);digitalWrite(MD3,0);
  Serial.println("No network detected");
  if(EEPROM.read(10) == 77 && EEPROM.read(11) == 77)   
   {
  //A1  
    AH1=00; AM1=00;  // sokal 8 ta
    if((AM1+rt) == 60) { AHR1=AH1+1; AMR1=0; }
    else               { AHR1=AH1; AMR1=AM1+rt; }
   }
   else 
   { 
    Serial.println("Reading from EEPROM 1");
    AH1 = EEPROM.read(10); AM1 = EEPROM.read(11); 
    if((AM1+rt) == 60) { AHR1=AH1+1; AMR1=0; }
    else               { AHR1=AH1; AMR1=AM1+rt; }
   }
   
  //A2
   if(EEPROM.read(12) == 77 && EEPROM.read(13) == 77)   
   {
    AH2=00; AM2=05; // dupur 1 ta
    if((AM2+rt) == 60) { AHR2=AH2+1; AMR2=0; }
    else               { AHR2=AH2; AMR2=AM2+rt; }
   }
   else 
   { 
    Serial.println("Reading from EEPROM 2");
    AH2 = EEPROM.read(12); AM2 = EEPROM.read(13); 
    if((AM2+rt) == 60) { AHR2=AH2+1; AMR2=0; }
    else               { AHR2=AH2; AMR2=AM2+rt; }
   }
  
  //A3
   if(EEPROM.read(14) == 77 && EEPROM.read(15) == 77)   
   { 
    AH3=00; AM3=10; // rat 9 ta
    if((AM3+rt) == 60) { AHR3=AH3+1; AMR3=0; }
    else               { AHR3=AH3; AMR3=AM3+rt; }
   }
   else 
   { 
    Serial.println("Reading from EEPROM 3");
    AH3 = EEPROM.read(14); AM3 = EEPROM.read(15); 
    if((AM3+rt) == 60) { AHR3=AH3+1; AMR3=0; }
    else               { AHR3=AH3; AMR3=AM3+rt; }
   }
   
  //________________________________Medicine set______________________________________ 
  
  //MS1
   if(EEPROM.read(40) == 77 && EEPROM.read(41) == 77 && EEPROM.read(42) == 77)  {}
   else { Serial.println("Reading from EEPROM 4"); MS1[0]= EEPROM.read(40); MS1[1]= EEPROM.read(41); MS1[2]= EEPROM.read(42); }
  //MS2
   if(EEPROM.read(20) == 77 && EEPROM.read(21) == 77 && EEPROM.read(22) == 77)  {}
   else { Serial.println("Reading from EEPROM 5"); MS2[0]= EEPROM.read(20); MS2[1]= EEPROM.read(21); MS2[2]= EEPROM.read(22); } 
  //MS3
   if(EEPROM.read(30) == 77 && EEPROM.read(31) == 77 && EEPROM.read(32) == 77)  {}
   else { Serial.println("Reading from EEPROM 6"); MS3[0]= EEPROM.read(30); MS3[1]= EEPROM.read(31); MS3[2]= EEPROM.read(32); } 
  }// no net end
EEPROM.commit();

//   Serial.print("Alarm1-  "); Serial.print(AH1); Serial.print(":"); Serial.println(AM1);
//   Serial.print("Alarm2-  "); Serial.print(AH2); Serial.print(":"); Serial.println(AM2);
//   Serial.print("Alarm3-  "); Serial.print(AH3); Serial.print(":"); Serial.println(AM3);
//   

//   
//   Serial.print("MS1 - "); Serial.print(MS1[0]); Serial.print(" - "); Serial.print(MS1[1]); Serial.print(" - "); Serial.println(MS1[2]);
//   Serial.print("MS2 - "); Serial.print(MS2[0]); Serial.print(" - "); Serial.print(MS2[1]); Serial.print(" - "); Serial.println(MS2[2]);
//   Serial.print("MS3 - "); Serial.print(MS3[0]); Serial.print(" - "); Serial.print(MS3[1]); Serial.print(" - "); Serial.println(MS3[2]);

Serial.print(EEPROM.read(10)); Serial.print(":"); Serial.println(EEPROM.read(11));
Serial.print(EEPROM.read(12)); Serial.print(":"); Serial.println(EEPROM.read(13));
Serial.print(EEPROM.read(14)); Serial.print(":"); Serial.println(EEPROM.read(15));

Serial.print(EEPROM.read(40)); Serial.print(" - "); Serial.print(EEPROM.read(41)); Serial.print(" - "); Serial.println(EEPROM.read(42));
Serial.print(EEPROM.read(20)); Serial.print(" - "); Serial.print(EEPROM.read(21)); Serial.print(" - "); Serial.println(EEPROM.read(22));
Serial.print(EEPROM.read(30)); Serial.print(" - "); Serial.print(EEPROM.read(31)); Serial.print(" - "); Serial.println(EEPROM.read(32));

   Serial.print("Repeat Alarm1-  "); Serial.print(AHR1); Serial.print(":"); Serial.println(AMR1);
   Serial.print("Repeat Alarm2-  "); Serial.print(AHR2); Serial.print(":"); Serial.println(AMR2);
   Serial.print("Repeat Alarm3-  "); Serial.print(AHR3); Serial.print(":"); Serial.println(AMR3);
 

 Serial.println("wait for some moment...");
 delay(5000);
 
}//setup

void loop() 
{
    RtcDateTime currentTime = rtcObject.GetDateTime();    //get the time from the RTC
 
   Years= currentTime.Year(); Months= currentTime.Month(); Days= currentTime.Day();
   Hours= currentTime.Hour(); Min= currentTime.Minute(); Sec= currentTime.Second();
    
  
    if (Hours == AH1 && Min == AM1 && AV1 == 0) // for alarm1
    {
      kaj(); // Medicine alert
      for(int i = 0; i<sizeof(MS1) / sizeof(MS1[0]); i++)
      {
       Serial.print("  Medicine No. ");
       Serial.print(MS1[i]);
       W = MS1[i];
       work(); // medicine no.
      }
      AV1=1;
      lastKaj(); // dont forget
    }
    
   else if (Hours == AH2 && Min == AM2 && AV2 == 0)
    {
      kaj();
      for(int i = 0; i<sizeof(MS2) / sizeof(MS2[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS2[i]);
        W = MS2[i];
       work();
      }
      AV2=1;
      lastKaj();
    }

   else if (Hours == AH3 && Min == AM3 && AV3 == 0)
    {
      kaj();
      for(int i = 0; i<sizeof(MS3) / sizeof(MS3[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS3[i]);
        W = MS3[i];
        work();
      }
      AV3=1;
      lastKaj();
    }
//_________________________________________________Repeat alarm____________________________________________________

 else if (Hours == AHR1 && Min == AMR1 && AVR1 == 0) // for alarm1
    {
      kaj(); // Medicine alert
      for(int i = 0; i<sizeof(MS1) / sizeof(MS1[0]); i++)
      {
       Serial.print("  Medicine No. ");
       Serial.print(MS1[i]);
       W = MS1[i];
       work(); // medicine no.
      }
      AVR1=1;
      lastKaj1(); // dont forget
    }
    
   else if (Hours == AHR2 && Min == AMR2 && AVR2 == 0)
    {
      kaj();
      for(int i = 0; i<sizeof(MS2) / sizeof(MS2[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS2[i]);
        W = MS2[i];
       work();
      }
      AVR2=1;
      lastKaj1();
    }

   else if (Hours == AHR3 && Min == AMR3 && AVR3 == 0)
    {
      kaj();
      for(int i = 0; i<sizeof(MS3) / sizeof(MS3[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS3[i]);
        W = MS3[i];
        work();
      }
      AVR3=1;
      lastKaj1();
    }

//_____________________________________________________day reset & light off_______________________________________
    
   if(Days != pDay)
   {
     AV1=0; AV2=0; AV3=0;
     pDay=Days; 
   }
   else{}
   if ( millis()-lm>= HT)
   {
    digitalWrite(MD1,ofv); digitalWrite(MD2,ofv); digitalWrite(MD3,ofv); digitalWrite(MD4,ofv); Serial.println("All off");
   }
   else{}

}// loop

//__________________________________________void functions________________________________
void work()
{
  
  if (W == 0){}
  else if (W == 1)
  { digitalWrite(MD1,onv); myDFPlayer.play(6); delay(1500); Serial.println(" Medicine1"); }
  else if(W == 2)
  { digitalWrite(MD2,onv); myDFPlayer.play(7); delay(1500); Serial.println(" Medicine2"); }
  else if(W == 3)
  { digitalWrite(MD3,onv); myDFPlayer.play(8); delay(1500); Serial.println(" Medicine3"); }
  else if(W == 4)
  { digitalWrite(MD4,onv); myDFPlayer.play(9); delay(1500); Serial.println(" Medicine4"); }
  
}

void kaj()
{
  Serial.println("Medicine Alert");
  myDFPlayer.play(4);
  delay(2050);
  myDFPlayer.play(1); // Tring Medicine Alert It's the time for
  delay(4050);
}

void lastKaj()
{
  myDFPlayer.play(2); //don't forget to take your medicine 
  delay(3200);
  Serial.println();
  Serial.println("-------------");
  lm=millis();
}

void lastKaj1()
{
  myDFPlayer.play(3); // This is the last reminder for the medicine don't forget to take your medicine 
  delay(6050);
  Serial.println();
  Serial.println("F-------------");
  lm=millis();
}
