#include <Servo.h>
Servo A; //Left side Servo
Servo B; //Right side Servo

int Apin=6; //Left side Servo Pin
int Bpin=7; //Right side Servo Pin

int aA, bA;

double a, b, c, d, db, s;
int y, x;
double k1, k2, k3, k4, j1, j2;
double phi, phi1;
double l1 = 10; // link 1
double l2 = 13; // link 2
double di = 10; // distance between 2 motor shaft

//--------------------------------------------|
  double t1 = 120; // Left side Servo angle   |
  double t2 = 60;  // Right side Servo angle  | 
//____________________________________________|

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  aA=int(t1); 
  bA=int(t2);

  
  
  A.attach(Bpin);
  B.attach(Apin);
   
  delay(2000);
  
}

void loop() {
  
  a = l1*cos(radians(t1));
  b = l1*sin(radians(t1));
  c = di+(l1*cos(radians(t2)));
  d = l1*sin(radians(t2));
  
  Serial.print("C: ");Serial.print(c);
  Serial.print("  D: ");Serial.print(d);
  //Serial.println();
  
  k1 = c - a;
  k2 = d - b;
  k3 = c + a;
  k4 = d + b;
  
  j1 = pow(c,2) + pow(d,2);
  j2 = pow(a,2) + pow(b,2);
  
  db = sqrt(pow(k1,2) + pow(k2,2));
  
  phi = acos(db/(2*l2));
  phi1 = acos((pow(db,2) + pow(l1,2) - j1)/(2*db*l1));
  
  s = pow(l1,2) - 2*l1*l2*cos(phi+phi1) + ((j1+j2)/2);
  
  y = int((2*k1*s - (k3*(j1-j2)))/(2*(k1*k4-k2*k3)));
  x = int((j1-j2-2*k2*y)/(2*k1));

  Serial.print("  X: ");Serial.print(x);
  Serial.print("  Y: ");Serial.println(y);
  
  A.write(aA);
  B.write(bA); 

  
}
