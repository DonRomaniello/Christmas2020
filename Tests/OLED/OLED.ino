
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  // OLED setup
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Rotations... odd numbers are vertical
  display.setRotation(3);
  display.clearDisplay();



}

void loop() {
  // Format for drawRect: (originX, originY, width, height, color)

//  display.drawRect(2, 2, (display.width() - 2), (display.height()-2)/2, SSD1306_WHITE);
//  display.fillRect(2, 2, ((display.width() - 2)/2), (display.height()-2)/2, SSD1306_WHITE);




  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(1,0);             // Start at top-left corner
  display.println(F("R|G|B"));
  
  display.display();
  delay(1000);

}
