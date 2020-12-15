#include <FastLED.h>
#define NUM_LEDS_PER_STRIP 100 
#define NUM_STRIPS 3
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
#define DATA_PIN_BOTTOM 1 
#define DATA_PIN_MIDDLE 17 
#define DATA_PIN_TOP 20 
CRGB leds_BOTTOM[NUM_LEDS_PER_STRIP];
CRGB leds_MIDDLE[NUM_LEDS_PER_STRIP];
CRGB leds_TOP[NUM_LEDS_PER_STRIP];

CRGB leds_array[NUM_LEDS];

void setup() {
delay(3000);
//FastLED.addLeds<1, WS2811, DATA_PIN_BOTTOM, RGB>(leds_BOTTOM, NUM_LEDS_PER_STRIP);
//FastLED.addLeds<1, WS2811, DATA_PIN_MIDDLE, RGB>(leds_MIDDLE, NUM_LEDS_PER_STRIP);
//FastLED.addLeds<1, WS2811, DATA_PIN_TOP, RGB>(leds_TOP, NUM_LEDS_PER_STRIP);


FastLED.addLeds<1, WS2811, DATA_PIN_BOTTOM, RGB>(leds_array, 0);
FastLED.addLeds<1, WS2811, DATA_PIN_MIDDLE, RGB>(leds_array, NUM_LEDS_PER_STRIP);
FastLED.addLeds<1, WS2811, DATA_PIN_TOP, RGB>(leds_array, 2 * NUM_LEDS_PER_STRIP);


//FastLED.addLeds<1, WS2811, DATA_PIN_BOTTOM, RGB>(leds_array, 0);
//FastLED.addLeds<1, WS2811, DATA_PIN_MIDDLE, RGB>(leds_array, NUM_LEDS_PER_STRIP);
//FastLED.addLeds<1, WS2811, DATA_PIN_TOP, RGB>(leds_array, 2 * NUM_LEDS_PER_STRIP);

FastLED.setMaxRefreshRate(240);
FastLED.setBrightness(32);
}

void loop() {
// whatever you want

fill_solid(leds_BOTTOM, 100, CRGB::Blue);
fill_solid(leds_MIDDLE, 100, CRGB::Blue);
fill_solid(leds_TOP, 100, CRGB::Blue);
FastLED.show();
FastLED.delay(500);

}
