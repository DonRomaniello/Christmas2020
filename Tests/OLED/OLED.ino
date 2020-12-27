
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define FPS 60
float value = 92.8;
int cps = (1000 / 1000);

void setup() {
  // OLED setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(2);
  display.clearDisplay();


//  display.drawLine(2, 10, 2, (display.height()-1), SSD1306_WHITE);
//  display.drawLine(14, 10, 14, (display.height()-1), SSD1306_WHITE);
//  display.drawLine(25, 10, 25, (display.height()-1), SSD1306_WHITE);

 
  


}

void loop() {
  // Format for drawRect: (originX, originY, width, height, color)

//  display.drawRect(2, 2, (display.width() - 2), (display.height()-2)/2, SSD1306_WHITE);
//  display.fillRect(2, 2, ((display.width() - 2)/2), (display.height()-2)/2, SSD1306_WHITE);



  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  showTime("Brightness:", 81.2, "%", "Speed:", 2,"cps");
  display.display();

  
  

}


void showTime(const char* fieldA, float valueA, const char* unitA, const char* fieldB, float valueB, const char* unitB){
  display.setCursor(0,0);             // Start at top-left corner
  display.print(fieldA);
  display.setCursor(70,0);
  display.print(valueA);
  display.print(unitA);

  display.setCursor(0,12);
  display.print(fieldB);
  display.setCursor(70,12);
  display.print(valueB);
  display.print(unitB);
}
