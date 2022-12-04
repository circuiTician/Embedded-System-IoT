int val = 0;

void setup() {
  Serial.begin(115200);
}

// put your main code here, to run repeatedly
void loop() {
  // read hall effect sensor value
  val = hallRead();
  // print the results to the serial monitor
  Serial.println(val); 
  delay(1000);
}