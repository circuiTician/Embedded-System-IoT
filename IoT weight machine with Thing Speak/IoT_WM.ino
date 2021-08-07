#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LiquidCrystal_I2C library

#include <ESP8266WiFi.h>
#include <Servo.h>

#include "ThingSpeak.h" 

HX711_ADC LoadCell(12, 14); // parameters: dt pin, sck pin //D6,D5

LiquidCrystal_I2C lcd(0x3f, 20, 4); //SCL D1, SDA D2



#define tare_button  D7 
#define START  D3
#define GO  D4
#define intr  D8
int motor;
int si,i, pdd, pdl, RP, pd, C,fg=0,fl=0;
int TP=15000;
unsigned long TTS=0,TO=0;
Servo myservo;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 1234567890;
const char * myWriteAPIKey = "*************";

void setup() {
  Serial.begin(115200);
 
  myservo.attach(D0); 
  myservo.write(115);
  pinMode (tare_button, INPUT_PULLUP);
  pinMode (START, INPUT_PULLUP);
  pinMode (GO, INPUT_PULLUP);
  pinMode(intr, INPUT);
  lcd.begin(); // begins connection to the LCD module
  lcd.backlight(); // turns on the backlight
  lcd.setCursor(0, 0);lcd.print("Not connected..");
  lcd.setCursor(0, 1);lcd.print("Plz connect to WiFi");
/*_______________________________________________________________*/

  WiFi.mode(WIFI_STA);  WiFi.beginSmartConfig();
  Serial.println("Waiting for SmartConfig.");
  
  while (!WiFi.smartConfigDone()) { delay(500); Serial.print("."); }
  Serial.println(""); Serial.println("SmartConfig done."); Serial.println("Waiting for WiFi");
  
  //delay(500);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print(".");}
  lcd.clear();
  Serial.println("WiFi Connected.");  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
  lcd.setCursor(0, 0);lcd.print("WiFi Connected.");
  lcd.setCursor(0, 1);lcd.print("IP Address: ");
  lcd.setCursor(2, 2);lcd.print(WiFi.localIP());delay(1300);
  
  ThingSpeak.begin(client);  // Initialize 
  
/*_______________________________________________________________*/  
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(231.51); //231.51 calibration factor for load cell => strongly dependent on your individual setup
  
  lcd.clear();
  lcd.setCursor(1, 1); // set cursor to first row
  lcd.print("IoT Weight Machine"); // print out to LCD
  lcd.setCursor(3, 2); // set cursor to first row
  lcd.print("Initiating..."); // print out to LCD
  lcd.blink();
  delay(1500);
  lcd.noBlink();
  lcd.clear();
  
 lcd.setCursor(1,0); lcd.print("IoT Weight Machine");
 lcd.setCursor(1,1); lcd.print("Weight: ");
 lcd.setCursor(0,2); lcd.print("No.PDL:"); lcd.setCursor(11,2); lcd.print("No.PDD:");
 lcd.setCursor(0,3); lcd.print("RP:"); lcd.setCursor(7,3); lcd.print("TPD:");
 lcd.setCursor(7,2); lcd.print(pdl);
 lcd.setCursor(18,2); lcd.print(pdd);
 lcd.setCursor(3,3); lcd.print(RP);
 lcd.setCursor(11,3); lcd.print(C);

}

void loop() 
{ 
//Serial.print("tare- ");Serial.print(digitalRead(tare_button));
//Serial.print("  start- ");Serial.print(digitalRead(START));
//Serial.print("  go- ");Serial.println(digitalRead(GO));
  
  lcd.setCursor(0,3); lcd.print("RP:");
  LoadCell.update(); // retrieves data from the load cell
 i = LoadCell.getData(); // get output value
  
 if (i<0)
 {
  i = i * (-1);
  lcd.setCursor(8, 1);
  lcd.print(" ");
 }
 else
 {
  if(i == 0)
  { lcd.setCursor(8, 1); lcd.print(" "); }
  else
  { lcd.setCursor(8, 1); lcd.print("-"); }
 }
 
  
  lcd.setCursor(9, 1); // set cursor to secon row
  lcd.print(i,1); // print out the retrieved value to the second row
  lcd.print("Gram.     ");
  
  if (i>10000)
  {
    //LoadCell.start(2000);
    lcd.setCursor(8, 1); // set cursor to secon row
    lcd.print("OVER LOAD!! ");
  }
  
  if (digitalRead (tare_button) == 0)
  {
    lcd.setCursor(8, 1); // set cursor to secon row
    lcd.print("Taring...");
    LoadCell.start(2000);
    lcd.setCursor(8, 1);
    lcd.print("          ");
  }
/*____________________________weight end && buttons start______________________________________*/



if (digitalRead(START) == 0)
 {
  if(i>=100){
  fl=1;
  TTS=millis();
  si=i;
  Serial.println("start");
  pdl=map(i,100,10000,1,50) ;//dispencible plastic count
  RP= map(i,100,10000,10,100);//reward point calculator
  Serial.print("pdl: ");
  Serial.print(pdl); Serial.print(" pdd: ");
  Serial.print(pdd); Serial.print(" RP: "); 
  Serial.println(RP);
  lcd.setCursor(7,2); lcd.print(pdl);
  lcd.setCursor(18,2); lcd.print(pdd);
  lcd.setCursor(3,3); lcd.print(RP);

  //call thingspeak
  
  /*int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){ Serial.println("Channel update successful1."); }
  else{ Serial.println("Problem updating channel. HTTP error code1 " + String(x)); }*/
  
  }
 else{
   lcd.setCursor(3,3); lcd.print("0");
 }
  
  pd=1;
 }
if (digitalRead(GO) == 0 && pd == 1)
 {
  Serial.println("go");
  TTS=millis();
  TO=TP*pdl;
  myservo.write(0);
  Serial.print("pdd:");Serial.println(pdd);
  plaDis();
  Serial.println(C);
  //call things speak to store total plastic count
  ThingSpeak.setField(1, si);
  ThingSpeak.setField(2, RP);
  ThingSpeak.setField(3, C);
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){ Serial.println("Channel update successful2."); }
  else{ Serial.println("Problem updating channel. HTTP error code2 " + String(x)); }
  
  pdd=00; pdl=00, RP=00, si=0;
  lcd.setCursor(7,2); lcd.print("   "); lcd.setCursor(7,2); lcd.print(pdl);
  lcd.setCursor(18,2); lcd.print("   "); lcd.setCursor(18,2); lcd.print(pdd);
  lcd.setCursor(3,3); lcd.print("   "); lcd.setCursor(3,3); lcd.print(RP);
  
 }

if((millis()-TTS >= 10000) && fl == 1)
{
  pdd=00; pdl=00, RP=00, si=0;
  lcd.setCursor(7,2); lcd.print("   "); lcd.setCursor(7,2); lcd.print(pdl);
  lcd.setCursor(18,2); lcd.print("   "); lcd.setCursor(18,2); lcd.print(pdd);
  lcd.setCursor(3,3); lcd.print("   "); lcd.setCursor(3,3); lcd.print(RP);
  fl=0;
  TTS=0;
}

}//loop



void plaDis()
{
  Serial.print("pddf:");Serial.println(pdd);
  while(true)
  {
    lcd.setCursor(0,3); lcd.print("RP:");
   Serial.println((millis()-TTS));
   //interrupt
      if (digitalRead(intr)==1)
      {
        fg = 1;
      }
      else if (fg==1 && (digitalRead(intr)==0))
      {
       pdd++;
       C++;
       fg=0;
      }
   //********************************
   lcd.setCursor(18,2); lcd.print(pdd);
   lcd.setCursor(11,3); lcd.print(C);
   if (pdd==pdl || (millis()-TTS)>TO)
   {
    Serial.println("breaked");
    pd=0;
    myservo.write(115); 
    delay(500);
    break;
   }
   
  }
}
