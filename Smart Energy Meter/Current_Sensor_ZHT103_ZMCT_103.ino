#include <Adafruit_ADS1X15.h> /*including adafruit library for ADS1115 digital to analog converter*/

Adafruit_ADS1115 ads; //

//declaring variables________________________

int16_t  sensorVALue2 = 0; 
int VAL[100]; 
int max_i = 0;
double ImaxD = 0; 
float current = 0; 
double Ieff = 0;

void setup() 
{
  Serial.begin(115200); /*Start the Serial communication */
  ads.setGain(GAIN_ONE); /* set the gain to 1x gain   +/- 4.096V  1 bit = 0.125mV */

  if (!ads.begin()) 
  {Serial.println("Failed to initialize ADS."); while (1);} /*if ads library not started it will print this 
                                                                               message to serial monitor*/
}

void loop() 
{

  for ( int i = 0; i < 100; i++ ) /* in this loop for calculation we tak 100 samples of the
                                                           voltage and store into an array */
  {
    sensorVALue2 = ads.readADC_SingleEnded(1); /*storing the value of analog channel 0 of ads1115*/
    if (sensorVALue2 > 0) 
    { VAL[i] = sensorVALue2; }  /* as the no voltage value or zero crossing value is near 0 so we
                                                         take the value which is greater than 0 */
    else { VAL[i] = 0; } /* else stores 0 */
    delay(1);
    }

    max_i = 0;

  for ( int i = 0; i < 100; i++ ) /* In this loop we find out the maximum value in the array */
  {
    if ( VAL[i] > max_i )
    {max_i = VAL[i];}
     VAL[i] = 0;
  }
  
  if (max_i != 0) /* if maaximum value is not 0 then print the maximum value */
  {
    ImaxD = max_i;
    Serial.print(ImaxD);
    Serial.print("   ");
    Ieff = ImaxD * 0.125; /* converting the analog reading to mili volt */
    current = ((pow(10,(-3))*6.870027753458*sq(Ieff))-(5.38066*Ieff)+1482.40)/1000; 
  }  /* making a hyperbolic funtion to get the actual value of current from the mili volt value */
  
  else {current = 0;} /* else set 0 */
  Serial.print("Current: ");
  Serial.println((current),2); /* print the value upto 2 decimal places */
  ImaxD = 0;
  delay(10);
} // loop end
