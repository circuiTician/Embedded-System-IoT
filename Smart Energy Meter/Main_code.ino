/*Declaring libraries for Firebase_______________________________*/

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

/*Declaring libraries for network time from internet_______________________________*/

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_ADS1X15.h>

#define API_KEY "****************************************"  /*put your firebase api key of the project*/
#define DATABASE_URL "***********************************"  /*put your firebase real time data base url of the project*/


const char *ssid     = "**********"; /*put your wifi SSID (wifi name)*/
const char *password = "**********"; /*put your wifi password*/


FirebaseData fbdo;     /*creating data object for firebase */
FirebaseAuth auth;     /*firebase authentication*/
FirebaseConfig config; /*firebase configuration*/

Adafruit_ADS1115 ads;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "in.pool.ntp.org"); /* "in.pool.ntp.org" -- this link is for india only 
                            check your country link in this website "https://www.pool.ntp.org/zone/@" */

/*Declaring variables_____________________________________________________________*/

//time
unsigned long epochTime;
int monthDay, currentMonth, currentYear, currentHour, currentMinute, currentSecond,pc;  

// voltage
int16_t sensorValue1 = 0;
int val[100]; int max_v = 0;
double VmaxD = 0, Veff = 0;


// energy
const byte interruptPin = 14; // Or other pins that support an interrupt
volatile boolean  interrupt_occurred = false; 
const int LED_pin = 2;
int cnt = 0;

//Cureent
int16_t  sensorVALue2 = 0; int VAL[100]; int max_i = 0;
double ImaxD = 0; float current = 0; double Ieff = 0;


void setup()
{

  Serial.begin(115200); /* Start Serial communication */

  pinMode(LED_pin, OUTPUT); digitalWrite(LED_pin, 1); 
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, FALLING);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {Serial.print(".");delay(300);}

  Serial.println();
  Serial.print("Connected with IP: ");Serial.println(WiFi.localIP());Serial.println();
  digitalWrite(LED_pin, 0); /*turn on the LED if connected with wifi */

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION); /* Print the firebase version*/

/* configuring firebase with url and api key*/
  config.api_key = API_KEY;      
  config.database_url = DATABASE_URL;

/* Starting the ntp server*/
  timeClient.begin();
  timeClient.setTimeOffset(19800); /*as INDIA time is GMT+5:30 so converting 5:30 into
                                            seconds  give this result 5:30*60 = 19800 */

  ads.setGain(GAIN_ONE); // 1x gain   +/- 4.096V   0.125mV
  if (!ads.begin()) 
  {Serial.println("Failed to initialize ADS."); while (1);}
  
  Firebase.begin(DATABASE_URL, API_KEY); /* Starting the firebase */
  
}

void loop()
{
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime(); /* this function is called epochTime
                                      epoch Time contains the date & time of a certain region from 
                                       which we can extract indivitual values of date & time*/
  struct tm *ptm = gmtime ((time_t *)&epochTime);

  int monthDay = ptm->tm_mday;                  /* Storing date */
  int currentMonth = ptm->tm_mon+1;             /* Storing month */
  int currentYear = ptm->tm_year+1900;          /* Storing year */

  int currentHour = timeClient.getHours();      /* Storing hour */
  int currentMinute = timeClient.getMinutes();  /* Storing minute */
  int currentSecond = timeClient.getSeconds();  /* Storing seconds */

  voltage(); /* calling the voltage function to get the value of voltage */
  CURRENt(); /* calling the current function to get the value of current */

  /* Serial printing all values which we will send to firebase*/
  
  Serial.println("Voltage: " + String(Veff)+ " volt");
  Serial.println("EnergyImpulse: " + String(cnt));
  Serial.println("Current: " + String(current)+ " amp");
  Serial.println(String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear));
  Serial.println(String(currentHour) + "-" + String(currentMinute) + "-" + String(currentSecond));
  Serial.println(".........");
  
/*in each 1 min interval we updating the value in fire base*/

 if(currentMinute-pc == 1 || pc == 0)
  {
  
  Firebase.setInt(fbdo, "/SEM/EIMP", cnt);          /* send the energy count value to firebase*/
  Firebase.setInt(fbdo, "/SEM/VOLTAGE", Veff);      /* send the voltage value to firebase*/
  Firebase.setFloat(fbdo, "/SEM/CURRENT", current); /* send the current value to firebase*/

  Firebase.setInt(fbdo, "/SEM/DD", monthDay);      /* send the date to firebase*/
  Firebase.setInt(fbdo, "/SEM/MO", currentMonth);  /* send the month to firebase*/
  Firebase.setInt(fbdo, "/SEM/YY", currentYear);   /* send the year to firebase*/

  Firebase.setInt(fbdo, "/SEM/HH", currentHour);   /* send the hour to firebase*/
  Firebase.setInt(fbdo, "/SEM/MI", currentMinute); /* send the minute to firebase*/

  pc=currentMinute;

  }
  
   delay(500);
}

ICACHE_RAM_ATTR void ISR () /* ISR for Energy meter*/
{                
  cnt++;
  //Serial.println(cnt);
}


void voltage() /* function for voltage sensor */
{
  for ( int i = 0; i < 100; i++ ) 
  {
    sensorValue1 = ads.readADC_SingleEnded(0); 
    if (sensorValue1 > 13200) 
    { val[i] = sensorValue1; }
    
    else { val[i] = 0; }
    delay(1);
  }

  max_v = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( val[i] > max_v )
    { max_v = val[i]; }
    val[i] = 0;
    }
    
  if (max_v != 0) 
  {
    VmaxD = max_v;
    //Serial.println(VmaxD);
    Veff=map(VmaxD,17660,17740,230,235);
    if(Veff>=230 && Veff<=235)
    {Veff = Veff;}
    else {Veff = 0;}
  }
  else {Veff = 0; }
  VmaxD = 0;
}

void CURRENt() /* function for current sensor */
{
    for ( int i = 0; i < 100; i++ ) {
    sensorVALue2 = ads.readADC_SingleEnded(1); 
    if (sensorVALue2 > 0) {
      VAL[i] = sensorVALue2;
    }
    else {
      VAL[i] = 0;
    }
    delay(1);
  }

  max_i = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( VAL[i] > max_i )
    {max_i = VAL[i];}
    VAL[i] = 0;
  }
  if (max_i != 0) 
  {
    ImaxD = max_i;
    //Serial.print(ImaxD);
    //Serial.print("   ");
    Ieff = ImaxD * 0.125;
    current = ((pow(10,(-3))*6.870027753458*sq(Ieff))-(5.38066*Ieff)+1482.40)/1000;
  }
  else {current = 0;}
  //Serial.print("Current: ");
  //Serial.println((current),2);
  ImaxD = 0;

  delay(10);
}
