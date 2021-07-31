#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//rotary encoder
#define SW 2
#define DT 4
#define CLK 5

//Stepper Driver
#define PUL 6
#define DIR 7
#define EN 8

//System
#define START 10
//#define STOP 11
#define SWEN 12
//LED
#define MS 9
#define MED 13

// variables
float counter, deg=90, dt=3, rpm=5, Pdeg, Pdt, Prpm;
int state,Laststate, check, flag, pntr;
int minR = 1, maxR = 10, minDG = 0, maxDG = 360, minDT = 1, maxDT = 10, mindr = 0, maxdr = 1;
int stp,dir,Pdir, dl;
int SC,fc,i,j,sf;

unsigned long pm,cm;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() 
{
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  pinMode(SW,INPUT); pinMode(DT,INPUT); pinMode(CLK,INPUT);
  pinMode(START,INPUT_PULLUP);  pinMode(3,INPUT_PULLUP); pinMode(SWEN,INPUT_PULLUP);
  pinMode(PUL,OUTPUT); pinMode(DIR,OUTPUT); pinMode(EN,OUTPUT); pinMode(MED,OUTPUT); pinMode(MS,OUTPUT);
  attachInterrupt(1,dam,FALLING);

  lcd.setCursor(2,0); lcd.print("STARTING..."); lcd.blink();
  delay(1500); lcd.noBlink(); lcd.clear();

  lcd.setCursor(1,0); lcd.print("DEG-"); lcd.setCursor(10,0); lcd.print("SD-");
  lcd.setCursor(13,0); lcd.print("CW"); lcd.setCursor(1,1); lcd.print("RPM-"); 
  lcd.setCursor(9,1); lcd.print("DT-"); lcd.setCursor(15,1); lcd.print("S");
  
  lcd.setCursor(5,0); lcd.print(int(deg));
  lcd.setCursor(5,1); lcd.print(int(rpm));
  lcd.setCursor(12,1); lcd.print(int(dt));
  
  
}

void loop() 
{

 if(digitalRead(START) == 1)
 {
  Serial.println("START"); flag = 1;
  digitalWrite(MS,1); 
  digitalWrite(MED,0); 
  digitalWrite(EN,0);
  motorStart(deg,rpm,dt,dir);
 }
 if(sf == 1)
 {
   Serial.println("STOP"); 
   flag = 0;
   sf = 0;
   i=0;
   digitalWrite(MS,0);
 }
/*_____________________________________________Start Flag 0_________________________________________*/
 if(flag == 0)
 {
//_________________________Rotary start
 state = digitalRead(CLK);
  if(state != Laststate)
  {
    
    if(pntr == 3) // rpm
    { 
     if(digitalRead(DT) != state) {
                                  if(rpm>=maxR)  { rpm=rpm; check = rpm*10; }
                                  else { rpm=rpm+0.5; check = rpm*10; }
                                  if ((check%10) == 0) { /*Serial.println((int)rpm);*/ }
                                  }
     else                         {
                                  if(rpm<=minR) { rpm=rpm; check = rpm*10; }
                                  else { rpm=rpm-0.5; check = rpm*10; }
                                  if ((check%10) == 0) { /*Serial.println((int)rpm);*/ }
                                  }
    }
    else if(pntr == 4)// dead time
    {
      if(digitalRead(DT) != state){
                                   if(dt>=maxDT)  { dt=dt; check = dt*10; }
                                   else { dt=dt+0.5; check = dt*10; }
                                   if ((check%10) == 0) { /*Serial.println((int)dt);*/ }
                                  }
      else                        {
                                   if(dt<=minDT) { dt=dt; check = dt*10; }
                                   else { dt=dt-0.5; check = dt*10; }
                                   if ((check%10) == 0) { /*Serial.println((int)dt);*/ }
                                  }
    }
    else if(pntr == 1) // degree
    {
    if(digitalRead(DT) != state){
                                  if(deg>=maxDG)  { deg=deg; check = deg*10; }
                                  else { deg=deg+0.5; check = deg*10; }
                                  if ((check%10) == 0) { /*Serial.println((int)deg);*/ }
                                 }
    else                         {
                                  if(deg<=minDG) { deg=deg; check = deg*10; }
                                  else { deg=deg-0.5; check = deg*10; }
                                  if ((check%10) == 0) { /*Serial.println((int)deg);*/ }
                                 }
    }
   else if(pntr == 2) // direction
    {
    if(digitalRead(DT) != state){
                                  dir=0;
                                }
    else                         {
                                  dir=1;
                                 }
    }
  
  }
  Laststate=state;
//_________________________Rotary end

//_________________________Rotary push start
if(digitalRead(SW) == 0)
{
  if(pntr == 0)      
  { 
    pntr++; // deg
    lcd.setCursor(0,0); lcd.print(">");//deg
    lcd.setCursor(9,0); lcd.print(" ");//SD
    lcd.setCursor(0,1); lcd.print(" ");//rpm
    lcd.setCursor(8,1); lcd.print(" ");//dt
  }
  else if(pntr == 1) 
  { 
    pntr++; // dr
    lcd.setCursor(0,0); lcd.print(" ");//deg
    lcd.setCursor(9,0); lcd.print(">");//SD
    lcd.setCursor(0,1); lcd.print(" ");//rpm
    lcd.setCursor(8,1); lcd.print(" ");//dt
  }
  else if(pntr == 2) 
  { 
    pntr++; // rpm
    lcd.setCursor(0,0); lcd.print(" ");//deg
    lcd.setCursor(9,0); lcd.print(" ");//SD
    lcd.setCursor(0,1); lcd.print(">");//rpm
    lcd.setCursor(8,1); lcd.print(" ");//dt
  }
  else if(pntr == 3) 
  { 
    pntr++; // dt
    lcd.setCursor(0,0); lcd.print(" ");//deg
    lcd.setCursor(9,0); lcd.print(" ");//SD
    lcd.setCursor(0,1); lcd.print(" ");//rpm
    lcd.setCursor(8,1); lcd.print(">");//dt
  }
  else if(pntr == 4) 
  { 
    pntr=1; // dt
    lcd.setCursor(0,0); lcd.print(">");//deg
    lcd.setCursor(9,0); lcd.print(" ");//SD
    lcd.setCursor(0,1); lcd.print(" ");//rpm
    lcd.setCursor(8,1); lcd.print(" ");//dt
  }
  Serial.println(pntr);
  delay(350);
}
//_________________________Rotary push end

//_________________________EN/DIS start
 if (digitalRead(SWEN)== 0) 
 {
  digitalWrite(EN,1); digitalWrite(MED,1);
 }
 else           
 {
  digitalWrite(EN,0); digitalWrite(MED,0);
 }
//_________________________EN/DIS end
 
 }//flag 0 end

prints();

/*Serial.print("deg"); Serial.print(deg); Serial.print("  ");
Serial.print("dt"); Serial.print(dt); Serial.print("  ");
Serial.print("rpm"); Serial.println(rpm);*/

}//loop


void prints()
{
  if(Pdeg != deg)
  {
    lcd.setCursor(5,0); lcd.print("   ");
    lcd.setCursor(5,0); lcd.print(int(deg)); Pdeg=deg;
  }
  /*----------*/
  if(Prpm != rpm)
  {
    lcd.setCursor(5,1); lcd.print("   ");
    lcd.setCursor(5,1); lcd.print(int(rpm)); Prpm=rpm;
  }
  /*----------*/
  if(Pdt != dt)
  {
    lcd.setCursor(12,1); lcd.print("   ");
    lcd.setCursor(12,1); lcd.print(int(dt)); Pdt=dt;
  }
  /*----------*/
  if(Pdir != dir)
  {
    lcd.setCursor(13,0); lcd.print("   ");
    if(dir == 0)
    {
      lcd.setCursor(13,0); lcd.print("CW");
    }
    else
    {
      lcd.setCursor(13,0); lcd.print("CCW");
    }
    Pdir=dir;
  }
  
}// end print


void motorStart(float deg,float rpm,float dt,int dir)
{
  
  dl= 1000/((rpm/60)*800);
  SC = 1.11*deg;
  int mdt = int(dt)*1000;
  Serial.print("mdt: ");Serial.print(mdt);
  
  /*Serial.print(deg); Serial.print(" ");
  Serial.print((int(dt)*1000)); Serial.print(" ");
  Serial.println(SC);*/
  while(1){

  if (sf==1)
  {Serial.println("broked"); break;}
  
  cm = millis();
  if(cm-pm>=mdt){
    Serial.print("Interval: ");Serial.print(cm-pm); Serial.print("  ");
    if(i==0){
   digitalWrite(DIR,dir);
  while(i<SC)
  {
    
    digitalWrite(PUL,1);
    delay(dl);
    digitalWrite(PUL,0);
    delay(dl);
    i++;
    Serial.print(i);Serial.print(" ");Serial.println(dir);
    
    if (sf==1)
    {Serial.println("broked"); break;}
  
  }
  }// cw end
  
 
  
   else if(i==SC){
   digitalWrite(DIR,!dir);
  while(i>0)
  {
    
    digitalWrite(PUL,1);
    delay(dl);
    digitalWrite(PUL,0);
    delay(dl);
    i--;  
    Serial.print(i);Serial.print(" ");Serial.println(!dir);
    
    if (sf==1)
    {Serial.println("broked"); break;}
  
  }
 }// ccw end
 else{Serial.print("betichod  "); Serial.print("i: "); Serial.println(i);}
 pm=millis();
  }// millis
  
  }//while
  
}

void dam()
{
  Serial.print("ISR Called   ");
  sf=1;
  Serial.println(sf);
}
