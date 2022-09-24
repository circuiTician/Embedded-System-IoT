
HardwareSerial mySerial(2);

uint8_t echo_command[1] = {0x55};
uint8_t info_command[2] = {0x01, 0x00};
uint8_t proto_sel[6] =  {0x02, 0x04, 0x02, 0x00, 0x03, 0x00};//protocol selection
uint8_t tim[6] =  {0x09, 0x04, 0x3A, 0x00, 0x58, 0x04};//synchronization between digital and analog inputs by adjusting TimerW value
uint8_t mg[6] =  {0x09, 0x04, 0x68, 0x01, 0x01, 0xD3};//modulation gain

uint8_t detect_command_iso14443a[4] =  {0x04, 0x02, 0x26, 0x07};//new card detect command
uint8_t atc[5] = {0x04, 0x03, 0x93, 0x20, 0x08};//card id check command



String chk_res()
{
String resp = "";
delay(50);
  while (mySerial.available()) {
    int k = mySerial.read();
    //Serial.print(k, HEX); Serial.print(" ");
    resp += String(k);
  }
  
  return resp;
  //Serial.println(resp);
}

void rfid_start()
{
  mySerial.begin(57600);
  delay(2000);
  Serial.println("RYRR10S");
  Serial.println();
  mySerial.flush();
  delay(1000);


  mySerial.write(echo_command, 1);
  chk_res();
  delay(100);
  mySerial.write(info_command, 2);
  chk_res();
  delay(100);
  mySerial.write(proto_sel, 6);
  chk_res();
  delay(100);
  mySerial.write(tim, 6);
  chk_res();
  delay(100);
  mySerial.write(mg, 6);
  chk_res();
  delay(100);
}
