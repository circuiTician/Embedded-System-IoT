  #include <esp_now.h>
  #include <WiFi.h>

  // Global copy of slave
  esp_now_peer_info_t slave;
  #define CHANNEL 1
  #define PRINTSCANRESULTS 0
  #define DELETEBEFOREPAIR 0

  //These are needed for MPU
  #include "I2Cdev.h"
  #include "MPU6050_6Axis_MotionApps20.h"
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      #include "Wire.h"
  #endif

  // MPU control/status vars
  MPU6050 mpu;
  bool dmpReady = false;  // set true if DMP init was successful
  uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
  uint8_t fifoBuffer[64]; // FIFO storage buffer
  Quaternion q;           // [w, x, y, z]         quaternion container
  VectorFloat gravity;    // [x, y, z]            gravity vector
  float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

  // RECEIVER MAC Address
  uint8_t receiverMacAddress[] = {0XE8, 0X68, 0XE7, 0X24, 0X04, 0XF0};  //E8:68:E7:24:04:F0

  struct PacketData 
  {
    byte xAxisValue;
    byte yAxisValue;
    byte zAxisValue;  
  };
  PacketData data;

  // Init ESP Now with fallback
  void InitESPNow() {
    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
      Serial.println("ESPNow Init Success");
    }
    else {
      Serial.println("ESPNow Init Failed");
      // Retry InitESPNow, add a counte and then restart?
      // InitESPNow();
      // or Simply Restart
      ESP.restart();
    }
  }  

  void ScanForSlave() {
    int8_t scanResults = WiFi.scanNetworks();
    // reset on each scan
    bool slaveFound = 0;
    memset(&slave, 0, sizeof(slave));

    Serial.println("");
    if (scanResults == 0) {
      Serial.println("No WiFi devices in AP Mode found");
    } else {
      Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
      for (int i = 0; i < scanResults; ++i) {
        // Print SSID and RSSI for each device found
        String SSID = WiFi.SSID(i);
        int32_t RSSI = WiFi.RSSI(i);
        String BSSIDstr = WiFi.BSSIDstr(i);

        if (PRINTSCANRESULTS) {
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(SSID);
          Serial.print(" (");
          Serial.print(RSSI);
          Serial.print(")");
          Serial.println("");
        }
        delay(10);
        // Check if the current device starts with `Slave`
        if (SSID.indexOf("Slave") == 0) {
          // SSID of interest
          Serial.println("Found a Slave.");
          Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
          // Get BSSID => Mac Address of the Slave
          int mac[6];
          if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
            for (int ii = 0; ii < 6; ++ii ) {
              slave.peer_addr[ii] = (uint8_t) mac[ii];
            }
          }

          slave.channel = CHANNEL; // pick a channel
          slave.encrypt = 0; // no encryption

          slaveFound = 1;
          // we are planning to have only one slave in this example;
          // Hence, break after we find one, to be a bit efficient
          break;
        }
      }
    }

    if (slaveFound) {
      Serial.println("Slave Found, processing..");digitalWrite(2,HIGH);
    } else {
      Serial.println("Slave Not Found, trying again.");digitalWrite(2,LOW);
    }

    // clean up ram
    WiFi.scanDelete();
  }

bool manageSlave() {
  if (slave.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer();
    }

    //Serial.print("Slave Status: ");
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if ( exists) {
      // Slave already paired.
      //Serial.println("Already Paired");
      return true;
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(&slave);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW Not Init");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
        return true;
      } else {
        Serial.println("Not sure what happened");
        return false;
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
    return false;
  }
}

void deletePeer() {
  esp_err_t delStatus = esp_now_del_peer(slave.peer_addr);
  Serial.print("Slave Delete Status: ");
  if (delStatus == ESP_OK) {
    // Delete success
    Serial.println("Success");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW Not Init");
  } else if (delStatus == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}

void sendData() {
  const uint8_t *peer_addr = slave.peer_addr;
  //Serial.print("Sending: "); Serial.println(data);
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &data, sizeof(data));
  //Serial.print("Send Status: ");
  if (result == ESP_OK) {
    //Serial.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}
  // callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
  {
    // char macStr[18];
    // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
    //         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    // Serial.print("Last Packet Sent to: "); Serial.println(macStr);
    // Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  }

  void setupMPU()
  {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    mpu.initialize();
    devStatus = mpu.dmpInitialize();
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) 
    {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.setDMPEnabled(true);
        dmpReady = true;
    } 
  }

  void setup()
  {
    Serial.begin(115200);
    pinMode(2,OUTPUT);
    digitalWrite(2,LOW);        
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    InitESPNow();

    esp_now_register_send_cb(OnDataSent);

    //This is to set up MPU6050 sensor
    setupMPU();  
    ScanForSlave();     
  }

  void loop() 
  {
       
    // if programming failed, don't try to do anything
    if (!dmpReady) return;
    // read a packet from FIFO. Get the Latest packet
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) 
    {  
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

      int xAxisValue = constrain(ypr[2] * 180/M_PI, -90, 90);
      int yAxisValue = constrain(ypr[1] * 180/M_PI, -90, 90);
      int zAxisValue = constrain(ypr[0] * 180/M_PI, -90, 90);    
      data.xAxisValue = map(xAxisValue, -90, 90, 0, 254); 
      data.yAxisValue = map(yAxisValue, -90, 90, 0, 254);
      data.zAxisValue = map(zAxisValue, -90, 90, 0, 254);   
      
      bool isPaired = manageSlave();
      if (isPaired) {
        // pair success or already paired
        // Send data to device
        sendData();
      } 
      else {
        // slave pair failed
        Serial.println("Slave pair failed!");
        ScanForSlave(); 
        
      } 
           
      //esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &data, sizeof(data));

      String inputData  = inputData + "values " + xAxisValue + "  " + yAxisValue + "  " + zAxisValue;
      Serial.println(inputData);
      delay(50); 
   }
  }




