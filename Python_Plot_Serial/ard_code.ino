
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = -50; i<50;i++){
  Serial.println(i);
   delay(5);
  }
  for (int i = 50; i>=-25;i--){
  Serial.println(i);
   delay(5);
  }
 

}
