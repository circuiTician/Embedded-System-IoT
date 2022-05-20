#include <Servo.h>

Servo servoLF; Servo servoRF; Servo servoLB; Servo servoRB;

//----CT6B Receiver--------
const int ch1 = 7;
const int ch2 = 8;
const int ch3 = 9;
const int ch4 = 10;
const int ch5 = 11;
const int ch6 = 12;


//-------Variables---------
int cv1, cv2, cv3, cv4, cv5, cv6; 
int lf_A, rf_A, lb_A, rb_A; 

// UpSideUp_DownPosition___________UpSideUp_UpPosition
int usuDP_lf=120,                  usuUP_lf=160;
int usuDP_rf=70,                   usuUP_rf=20;
int usuDP_lb=80,                   usuUP_lb=20;
int usuDP_rb=90,                   usuUP_rb=160;

int preServoVal1, preServoVal2, preServoVal3;

void setup() 
{
 Serial.begin(115200);
 
 servoLF.attach(6); servoRF.attach(5); servoLB.attach(4); servoRB.attach(3);

 //Down limit position
 lf_A = usuDP_lf , rf_A = usuDP_rf, lb_A = usuDP_lb, rb_A = usuDP_rb; 
 servoLF.write(lf_A); servoRF.write(rf_A); servoLB.write(lb_A); servoRB.write(rb_A); // down dead angles when up side
 delay(1500);

 //Up limit position
 lf_A = usuUP_lf , rf_A = usuUP_rf, lb_A = usuUP_lb, rb_A = usuUP_rb;
 servoLF.write(lf_A); servoRF.write(rf_A); servoLB.write(lb_A); servoRB.write(rb_A);//up dead angles when up side
 delay(1500);

 pinMode(ch1, INPUT); pinMode(ch2, INPUT); pinMode(ch3, INPUT); pinMode(ch4, INPUT); pinMode(ch5, INPUT); pinMode(ch6, INPUT);

}

void loop() 
{
  cv1 = pulseIn(ch1,HIGH); cv2 = pulseIn(ch2,HIGH); 
  cv3 = pulseIn(ch3,HIGH); cv4 = pulseIn(ch4,HIGH); 
  cv5 = pulseIn(ch5,HIGH); cv6 = pulseIn(ch6,HIGH);

  Serial.print("Ch1: ");   Serial.print(cv1); Serial.print("  Ch2: "); Serial.print(cv2);
  Serial.print("  Ch3: "); Serial.print(cv3); Serial.print("  Ch4: "); Serial.print(cv4); 
  Serial.print("  Ch5: "); Serial.print(cv5); Serial.print("  Ch6: "); Serial.println(cv6);

  
  //for up and down action
  if (cv6>1000 && cv6<1900 && (abs(preServoVal1 - cv6) >=50) && cv3>1450 && cv3<1550 && cv4>1450 && cv4<1550)//recomended 80
  {
     lf_A=map(cv6,1000,1900, usuDP_lf,usuUP_lf); rf_A=map(cv6,1000,1900, usuDP_rf,usuUP_rf);
     lb_A=map(cv6,1000,1900, usuDP_lb,usuUP_lb); rb_A=map(cv6,1000,1900, usuDP_rb,usuUP_rb);
     servoLF.write(lf_A); servoRF.write(rf_A); servoLB.write(lb_A); servoRB.write(rb_A);
     preServoVal1=cv6;
  }

 //for rolling action
 if (cv4>1000 && cv4<2000 && cv6>1900 && cv3>1300 && cv3<1700 && (abs(preServoVal2 - cv4) >=50)) 
  {
    lf_A=map(cv4,2000,1000, usuUP_lf,usuDP_lf); rf_A=map(cv4,2000,1000, usuDP_rf,usuUP_rf);
    lb_A=map(cv4,2000,1000, usuUP_lb,usuDP_lb); rb_A=map(cv4,2000,1000, usuDP_rb,usuUP_rb);
    servoLF.write(lf_A); servoRF.write(rf_A); servoLB.write(lb_A); servoRB.write(rb_A);
    preServoVal2=cv4;
  }

 //for pitching action
 if (cv3>1000 && cv3<2000 && cv6>1900 && cv4>1450 && cv4<1550 && (abs(preServoVal3 - cv3) >=50)) 
  {
    lf_A=map(cv3,1000,2000, usuUP_lf,usuDP_lf); rf_A=map(cv3,1000,2000, usuUP_rf,usuDP_rf); 
    lb_A=map(cv3,1000,2000, usuDP_lb,usuUP_lb); rb_A=map(cv3,1000,2000, usuDP_rb,usuUP_rb);
    servoLF.write(lf_A); servoRF.write(rf_A); servoLB.write(lb_A); servoRB.write(rb_A);
    preServoVal3=cv3;
  }

}//loop
