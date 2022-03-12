#include <Servo.h>
Servo A; //Left side Servo
Servo B; //Right side Servo

int Apin=6; //Left side Servo Pin
int Bpin=7; //Right side Servo Pin

int aA, bA;

double l1 = 10; // link 1
double l2 = 13; // link 2
double di = 10; // distance between 2 motor shaft

double c,e;
double t1,t2; 
double pi = 3.1415926535897932384626433832795;

double x, y;

// square
//double xw[] = {4,4,8,8};
//double yw[] = {15,20,20,15};

// triangle
double xw[] = {4,6,8};
double yw[] = {15,20,15};

void setup()
{
  Serial.begin(115200);
    A.attach(Apin);
    B.attach(Bpin);
    //warm up angles :-)
    A.write(90);
    B.write(90); delay(500);
    A.write(120);
    B.write(60); delay(500);
    A.write(90);
    B.write(90); delay(500);
    
}
void loop()
{
  for (int i = 0; i<3; i++)
  {
    x=xw[i];
    y=yw[i];
   //__________________________________________________  
     c=sqrt(pow(x,2)+pow(y,2));
     e=sqrt(pow((di-x),2)+pow(y,2));
    
     t1 = atan(y/x) + acos((((pow(l1,2) + pow(c,2)) - pow(l2,2))/(2*l1*c)));
     t2 = atan(y/(di-x)) + acos((((pow(l1,2) + pow(e,2)) - pow(l2,2))/(2*l1*e)));
     //aA = 180 - degrees(t2);
     //bA = degrees(t1);
    
     aA = abs(int((t1*180)/pi));
     bA = abs(int(180 - (t2*180)/pi));
   //__________________________________________________
    A.write(aA);
    B.write(bA);   
    Serial.print("Alpha: ");Serial.print(aA);
    Serial.print("  Beta: ");Serial.println(bA);
    delay(2000);
  }
  
  Serial.println("-------------");
  //delay(2000);
}
