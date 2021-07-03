/* declaration of variables __________________________________________________________ */

const byte        interruptPin = 14;          /* pin for the interrupt this the D5 pin of node MCU */
volatile boolean  interrupt_occurred = false; 
int cnt = 0;

void setup() 
{
  Serial.begin(115200); /* Start the serial communication */
  pinMode(interruptPin, INPUT_PULLUP); /*Set the pin as input in pull up arrangement */
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, FALLING); /* attaching interrupt in falling mode */
}
 
void loop() 
{
  Serial.println(cnt);   
}
 
ICACHE_RAM_ATTR void ISR ()    // Interrupt Service Routine, come here when an interrupt occurs
{ 
  cnt++;
}
