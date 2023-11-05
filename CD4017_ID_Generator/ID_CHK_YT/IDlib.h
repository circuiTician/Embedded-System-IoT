int reading, ID;

int getID(const int pulsePin, const int readPin, const int resetPin) 
{
  pinMode(pulsePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(readPin, INPUT);

  digitalWrite(resetPin, 0); delay(1);
  digitalWrite(resetPin, 1); 
  delayMicroseconds(10);
  digitalWrite(resetPin, 0);

  for (int i = 1; i < 10; i++) 
    {
      // Generate pulse
      digitalWrite(pulsePin, HIGH);
      delayMicroseconds(50);
      digitalWrite(pulsePin, LOW);

      // Read digital reading
      reading = digitalRead(readPin);

      ID = ID+(i*reading);
      //delay(4000);
      delayMicroseconds(10);  // Delay between pulses (100 micro-seconds)
    }
  //Serial.println("ID: "+ String(ID));
  return(ID);
}