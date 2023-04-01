#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void disp_init()
{
    
}

void welcome(){

  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(48, 16);
  display.println(F("LED"));
  display.setCursor(30, 34);
  display.println(F("STROBE"));
  display.display();
  //delay(3000);

  //display.clearDisplay();

  display.setTextSize(1.5); // Draw 2X-scale text
  display.setTextColor(SSD1306_INVERSE);
  display.setCursor(20, 52);
  display.println(F("By circuiTician"));
  display.display();
  display.stopscroll();
  delay(2000);


}

void freq_disp(){
  display.clearDisplay();
  
  display.setTextSize(2); // Draw 2X-scale text
  display.fillRect(35, 8, 52, 18, SSD1306_WHITE);
  display.display(); 
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(38, 10);
  display.println("FREQ");
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(50, 50);
  display.println("Hz");
   display.display(); 
  
  
}

void rpm_disp(){
  display.clearDisplay();
  
  display.setTextSize(2); // Draw 2X-scale text
  display.fillRect(30, 8, 64, 18, SSD1306_WHITE);
  display.display(); 
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(32, 10);
  display.println("SPEED");
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(45, 50);
  display.println("RPM");  
   display.display();   

}

void duty_disp(){
  display.clearDisplay();
  
  display.setTextSize(2); // Draw 2X-scale text
  display.fillRect(35, 8, 52, 18, SSD1306_WHITE);
  display.display(); 
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(38, 10);
  display.println("DUTY");
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(57, 50);
  display.println("%");   
   display.display();      
      
}

void mult(int mul){
  // Serial.print("--");  
  // Serial.println(mul);
  display.setTextSize(1); 
  display.fillRect(95, 54, 55, 10, SSD1306_BLACK);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(97, 56);
  display.println("x"+String(mul)); 
  display.display();   
}

void disp_var(int c_val){
  
  display.setTextSize(2); 
  display.fillRect(30, 30, 64, 18, SSD1306_BLACK);   
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(33, 32);
  display.println(String(c_val));
  display.display();
}