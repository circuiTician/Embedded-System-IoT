
#include <ESP32Servo.h>

const double VCC = 3.3;             // NodeMCU on board 3.3v vcc
const double R2 = 10000;            // 10k ohm series resistor
const double adc_resolution = 4095; // 10-bit adc

const double A = 0.001129148;   // thermistor equation parameters
const double B = 0.000234125;
const double C = 0.0000000876741; 
float ptc,tc;


Servo myservo;
int pos = 0;
int servoPin = 5;
void setup() {
  Serial.begin(9600);  /* Define baud rate for serial communication */
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400);
  Serial.println();
   myservo.write(0); delay(800);
   myservo.write(180); delay(800);
   myservo.write(0); delay(800);
   Serial.println("sets");
   myservo.write(180); delay(5000);
   Serial.println("set");
}

void loop() {
  double Vout, Rth, temperature, adc_value; 
for ( int i = 0; i < 100; i++ )
{
  adc_value = analogRead(35);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;

/*  Steinhart-Hart Thermistor Equation:
 *  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
 *  where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8  */
  temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));   // Temperature in kelvin

  temperature = temperature - 273.15;  // Temperature in degree celsius
  
  tc=temperature;
  ptc=ptc+tc;
  //Serial.print(temperature);Serial.print("      ");Serial.println(ptc); 
  delayMicroseconds(500);
  }

  Serial.print("Temperature = ");
  Serial.print(ptc/100);
  Serial.print(" degree celsius");
  if((ptc/100)>100)     { pos=0;}
  else if ((ptc/100)<0) { pos=180;}
  else { pos = map((ptc/100),0,100, 180,0);}
 
  Serial.print("  Servo pos: "); Serial.print(pos);Serial.println("*");
  myservo.write(pos);
   delay(1000);
   ptc=0;

}
