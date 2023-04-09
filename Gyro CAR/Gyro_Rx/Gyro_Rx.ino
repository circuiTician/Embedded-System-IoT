#include <esp_now.h>
#include <WiFi.h>
#include <vector>

#define FORWARD 1
#define BACKWARD 2
#define LEFT 3
#define RIGHT 4
#define FORWARD_LEFT 5
#define FORWARD_RIGHT 6
#define BACKWARD_LEFT 7
#define BACKWARD_RIGHT 8
#define TURN_LEFT 9
#define TURN_RIGHT 10
#define STOP 0

#define BACK_RIGHT_MOTOR 0
#define BACK_LEFT_MOTOR 1
#define FRONT_RIGHT_MOTOR 2
#define FRONT_LEFT_MOTOR 3

#define CHANNEL 1

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;
  int pinEn; 
  int pwmSpeedChannel;
};

std::vector<MOTOR_PINS> motorPins = 
{
  {18, 19,  5, 4},  //BACK_RIGHT_MOTOR
  {21, 22, 23, 5},  //BACK_LEFT_MOTOR
  {14, 12, 13, 6},  //FRONT_RIGHT_MOTOR
  {26, 27, 25, 7},  //FRONT_LEFT_MOTOR   
};

#define MAX_MOTOR_SPEED 200

const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;

#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal
unsigned long lastRecvTime = 0;

struct PacketData
{
  byte xAxisValue;
  byte yAxisValue;
  byte zAxisValue;
};
PacketData receiverData;


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

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  if (len == 0)
  {
    return;
  }
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  String inputData ;
  inputData = inputData + "values " + receiverData.xAxisValue + "  " + receiverData.yAxisValue + "  " + receiverData.zAxisValue;
  Serial.println(inputData);

  // if ( receiverData.xAxisValue < 75 && receiverData.yAxisValue < 75)//fwd left
  // {
  //    //  
  //   processCarMovement(FORWARD_LEFT); 
  // }
  // else if ( receiverData.xAxisValue > 175 && receiverData.yAxisValue < 75)//bkd lt
  // {
  //      processCarMovement(BACKWARD_LEFT); 
  // } 
  // else if ( receiverData.xAxisValue < 75 && receiverData.yAxisValue > 175)//fwd rt
  // {
  //   processCarMovement(FORWARD_RIGHT);    
  // }
  // else if ( receiverData.xAxisValue > 175 && receiverData.yAxisValue > 175)//bkd rt
  // {
  //   processCarMovement(BACKWARD_RIGHT);    //
  // }  
  if (receiverData.zAxisValue > 175  && (receiverData.xAxisValue>75 || receiverData.xAxisValue<175) && (receiverData.yAxisValue>75 || receiverData.yAxisValue<175))
  {
    processCarMovement(TURN_RIGHT);
  }
  else if (receiverData.zAxisValue < 75  && (receiverData.xAxisValue>75 || receiverData.xAxisValue<175) && (receiverData.yAxisValue>75 || receiverData.yAxisValue<175))
  {
    processCarMovement(TURN_LEFT);
  }
  else if (receiverData.yAxisValue < 75  && (receiverData.xAxisValue>75 || receiverData.xAxisValue<175) && (receiverData.zAxisValue>75 || receiverData.zAxisValue<175))
  {
    processCarMovement(RIGHT);  
  }
  else if (receiverData.yAxisValue > 175 && (receiverData.xAxisValue>75 || receiverData.xAxisValue<175) && (receiverData.zAxisValue>75 || receiverData.zAxisValue<175))
  {
    processCarMovement(LEFT);    
  }
  else if (receiverData.xAxisValue > 175 && (receiverData.yAxisValue>75 || receiverData.yAxisValue<175) && (receiverData.zAxisValue>75 || receiverData.zAxisValue<175))
  {
    processCarMovement(BACKWARD);   
  }
  else if (receiverData.xAxisValue < 75 && (receiverData.yAxisValue>75 || receiverData.yAxisValue<175) && (receiverData.zAxisValue>75 || receiverData.zAxisValue<175))
  {
    processCarMovement(FORWARD);    
  } 
  else
  {
    processCarMovement(STOP);     
  }

  lastRecvTime = millis();   
}

void processCarMovement(int inputValue)
{
  switch(inputValue)
  {
    case BACKWARD:
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);                  
      break;
  
    case FORWARD:
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);   
      break;
  
    case LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);   
      break;
  
    case RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);  
      break;
  
    case FORWARD_LEFT://OK
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);  
      break;
  
    case FORWARD_RIGHT://ok
       rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, STOP);   
      break;
  
    case BACKWARD_LEFT://ok
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, STOP);   
      break;
  
    case BACKWARD_RIGHT://OK
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);  
      break;
  
    case TURN_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, MAX_MOTOR_SPEED);  
      break;
  
    case TURN_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_RIGHT_MOTOR, -MAX_MOTOR_SPEED);
      rotateMotor(FRONT_LEFT_MOTOR, MAX_MOTOR_SPEED);
      rotateMotor(BACK_LEFT_MOTOR, -MAX_MOTOR_SPEED);   
      break;
  
    case STOP:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  
    default:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  }
}

void rotateMotor(int motorNumber, int motorSpeed)
{
  if (motorSpeed < 0)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);    
  }
  else if (motorSpeed > 0)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
  }
  else
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);      
  }
  
  ledcWrite(motorPins[motorNumber].pwmSpeedChannel, abs(motorSpeed));
}

void setUpPinModes()
{
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  
    //Set up PWM for motor speed
    ledcSetup(motorPins[i].pwmSpeedChannel, PWMFreq, PWMResolution);  
    ledcAttachPin(motorPins[i].pinEn, motorPins[i].pwmSpeedChannel);     
    rotateMotor(i, STOP);  
  }
}

void setup() 
{
  setUpPinModes();
  
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  configDeviceAP();

  // Init ESP-NOW
  InitESPNow();

  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() 
{
  //Check Signal lost.
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    processCarMovement(STOP);
  }
}