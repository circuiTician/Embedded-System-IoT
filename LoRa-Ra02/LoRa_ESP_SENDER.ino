#include <SPI.h>
#include <LoRa.h>

const int csPin = 15;
const int resetPin = 2;
const int irqPin = 5;
int state=0;
int counter = 0;
int SyncWord = 0x22;

void setup() {
  pinMode(4,OUTPUT);
  Serial.begin(115200);

  Serial.println("LoRa Sender");
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(8);
  LoRa.setSyncWord(SyncWord);
  
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  //LoRa.print("hello ");
  LoRa.print(counter);
  state=!state;
  digitalWrite(4,state);
  LoRa.endPacket();

  counter++;

  delay(500);
}
