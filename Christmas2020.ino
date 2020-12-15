/* Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */


// FASTled works on pin 0-23
// Encoder works on 0-4,!NOT 5!,6-23
//22,23


#include <Encoder.h>
#include <FastLED.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knobRight(19,20);
Encoder knobLeft(9, 10);
//   avoid using pins with LEDs attached

#define DATA_PIN_BOTTOM 7
#define NUM_LEDS 100


#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 100
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
#define LED_TYPE    WS2811
#define COLOR_CORRECTION Candle
#define BRIGHTNESS  192

CRGB leds[NUM_LEDS];



void setup() {
  // tell FastLED there's 60 NEOPIXEL leds on pin 10, starting at index 0 in the led array
   FastLED.addLeds<LED_TYPE, DATA_PIN_BOTTOM, RGB>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection( COLOR_CORRECTION );
   FastLED.setMaxRefreshRate(240);

//FastLED.addLeds<1, LED_TYPE, DATA_PIN_BOTTOM, RGB>(leds, NUM_LEDS_PER_STRIP);


  // tell FastLED there's 60 NEOPIXEL leds on pin 11, starting at index 60 in the led array
  //FastLED.addLeds<LED_TYPE, 16, RGB>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection( COLOR_CORRECTION );

  // tell FastLED there's 60 NEOPIXEL leds on pin 12, starting at index 120 in the led array
 // FastLED.addLeds<LED_TYPE, 17, RGB>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection( COLOR_CORRECTION );


  
}

long positionLeft  = -999;
long positionRight = -999;

void loop() {
  long newLeft, newRight;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  if (newLeft != positionLeft || newRight != positionRight) {
   
    positionLeft = newLeft;
    positionRight = newRight;
  }
  fill_solid( leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  FastLED.delay(1000);
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  FastLED.delay(1000);
  
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
  }
}
