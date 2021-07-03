#include <Adafruit_ADS1X15.h> /*including adafruit library for ADS1115 digital to analog converter*/

Adafruit_ADS1115 ads; //

//declaring variables________________________

int16_t sensorValue1 = 0;
int val[100];
int i = 0;
int max_v = 0;
double VmaxD = 0;
double Veff = 0;

void setup() {
  Serial.begin(115200); /*Start the Serial communication */
  ads.setGain(GAIN_ONE); /* set the gain to 1x gain   +/- 4.096V  1 bit = 0.125mV */

  if (!ads.begin()) 
  {Serial.println("Failed to initialize ADS."); while (1);} /*if ads library not started it will print this 
                                                                               message to serial monitor*/
}

void loop() 
{
  for ( i = 0; i < 100; i++ ) /* in this loop for calculation we tak 100 samples of the
                                                           voltage and store into an array */
  {
    sensorValue1 = ads.readADC_SingleEnded(0); /*storing the value of analog channel 0 of ads1115*/
    if (sensorValue1 > 13200) 
    { val[i] = sensorValue1; } /* as the no voltage value or zero crossing value is near 13200 so we
                                                         take the value which is greater than 13200 */
    else { val[i] = 0;} /* else stores 0 */
    delay(1);
  }

  max_v = 0;

  for ( i = 0; i < 100; i++ ) /* In this loop we find out the maximum value in the array */
  {
    if ( val[i] > max_v )
    { max_v = val[i]; }
      val[i] = 0;
  }
  if (max_v != 0) /* if maaximum value is not 0 then print the maximum value */
  {
    VmaxD = max_v;
    Serial.print(VmaxD);
    Serial.print("   ");
    Veff=map(VmaxD,17660,17740,230,235); /* mapping the maximum value range with mains input voltage*/
  }
  
  else { Veff = 0; } /* else set the voltage value to 0 */
  
  Serial.print("Voltage: "); /* Printing the voltage value to Serial monitor */
  Serial.println(Veff);
  VmaxD = 0;

} // loop end
