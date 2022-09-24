#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "RFID ACS"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "RFID ACS"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "acs.h"

//device id
#define DID "RF002"

#define red 32
#define green 33
int cid;

// WiFi AP SSID
#define WIFI_SSID "Programmer Bose 2.4"
// WiFi password
#define WIFI_PASSWORD "Pneucis@202226"
// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://ap-southeast-2-1.aws.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> Generate API Token)
#define INFLUXDB_TOKEN "LIujCqYIepYq99GaiXOV7SJ1f-jJ2PGgqAngjKBGYsjEdZcF6ZnlysDoPAwZEc5Zzsk-jj5Sg_EYAUUIQa2IQw=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "f098fb3ba7967658"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "RFID_LOG"

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "IST-5:30" //kolkata

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("rfid_log");

void setup() {
  Serial.begin(115200);
  rfid_start();
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  digitalWrite(red, 0);
  digitalWrite(green, 0);
  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Add tags
  sensor.addTag("Device", DEVICE);
  sensor.addTag("ID", DID);

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}


void loop()
{
  mySerial.write(detect_command_iso14443a, 4);
  String ncd = chk_res();
  Serial.println(ncd);
  //delay(20);
  if (ncd == "1285404000")
  {

    mySerial.write(atc, 5);
    String ncid = chk_res();
    String ID = ncid.substring(4, 17);
    Serial.println(ncid);
    Serial.println(ID);

    if (ID == "8119842100217" ) {
      cid = (ID.substring(0, 4).toInt());
      Serial.println(cid);
      writeInflux();
      Serial.println("Access Granted--------------->");
    }
    else
    {
      Serial.println("Access Denied--------------->");
      digitalWrite(green, 0);
      digitalWrite(red, 1);
      delay(2000);
      digitalWrite(red, 0);
    }

    //delay(20);
  }
  //  else
  //  {
  //    Serial.println("Access Denied--------------->");
  //  }
}

void writeInflux()
{
  digitalWrite(green, 1);
  digitalWrite(red, 0);
  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();

  // Store measured value into point
  // Report RSSI of currently connected network
  sensor.addField("Card ID", cid);

  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  // Check WiFi connection and reconnect if needed
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  digitalWrite(green, 1);
  digitalWrite(red, 0);
  //Serial.println("Wait 10s");
  delay(2000);
  digitalWrite(green, 0);
}
