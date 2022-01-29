#include <Wire.h>       //I2C library
#include <RtcDS3231.h>  //RTC library
  
//RtcDS3231 rtcObject;              //Uncomment for version 1.0.1 of the rtc library
RtcDS3231<TwoWire> rtcObject(Wire); //Uncomment for version 2.0.0 of the rtc library
int Years, Months, Days, Hours, Min, Sec,timeStamp, date;  
void setup() {
  
  Serial.begin(115200);  //Starts serial connection
  rtcObject.Begin();     //Starts I2C
  
  /*RtcDateTime currentTime = RtcDateTime(2022, 01, 29, 21,36, 30); //define date and time object
  rtcObject.SetDateTime(currentTime); //configure the RTC with object*/
  
}
  
void loop() {
  
  RtcDateTime currentTime = rtcObject.GetDateTime();    //get the time from the RTC
  

Years= currentTime.Year(); Months= currentTime.Month(); Days= currentTime.Day();
Hours= currentTime.Hour(); Min= currentTime.Minute(); Sec= currentTime.Second();
  

  Serial.print("Date- "); Serial.print(Days); Serial.print("/"); Serial.print(Months); Serial.print("/"); Serial.println(Years); 
  Serial.print("Time- "); Serial.print(Hours); Serial.print(":"); Serial.print(Min); Serial.print(":"); Serial.println(Sec); 

  
  delay(1000); //1 seconds delay
  
}
