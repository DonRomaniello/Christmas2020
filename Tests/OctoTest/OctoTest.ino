#include <OctoWS2811.h>
#include <FastLED.h>
#include <Arduino.h>

#define DATA_PIN_BOTTOM 1 
#define DATA_PIN_MIDDLE 17 
#define DATA_PIN_TOP 20 

const int numPins = 3;
  byte pinList[numPins] = {DATA_PIN_BOTTOM, DATA_PIN_MIDDLE, DATA_PIN_TOP};
  const int ledsPerStrip = 100;
  CRGB leds[numPins * ledsPerStrip];

  // These buffers need to be large enough for all the pixels.
  // The total number of pixels is "ledsPerStrip * numPins".
  // Each pixel needs 3 bytes, so multiply by 3.  An "int" is
  // 4 bytes, so divide by 4.  The array is created using "int"
  // so the compiler will align it to 32 bit memory.
  DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
  int drawingMemory[ledsPerStrip * numPins * 3 / 4];
  OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, numPins, pinList);


template <EOrder RGB_ORDER = RGB,
          uint8_t CHIP = WS2811_800kHz>
class CTeensy4Controller : public CPixelLEDController<RGB_ORDER, 8, 0xFF>
{
    OctoWS2811 *pocto;

public:
    CTeensy4Controller(OctoWS2811 *_pocto)
        : pocto(_pocto){};

    virtual void init() {}
    virtual void showPixels(PixelController<RGB_ORDER, 8, 0xFF> &pixels)
    {

        uint32_t i = 0;
        while (pixels.has(1))
        {
            uint8_t r = pixels.loadAndScale0();
            uint8_t g = pixels.loadAndScale1();
            uint8_t b = pixels.loadAndScale2();
            pocto->setPixel(i++, r, g, b);
            pixels.stepDithering();
            pixels.advanceData();
        }

        pocto->show();
    }
};

 CTeensy4Controller<RGB, WS2811_800kHz> *pcontroller;

void setup() {
    octo.begin();
    pcontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octo);

    FastLED.addLeds(pcontroller, leds, numPins * ledsPerStrip);    
    FastLED.setMaxRefreshRate(240);
    FastLED.setBrightness(64);
  }


void loop() {
//fill_solid(leds, 300, CRGB::Blue);
//FastLED.show();
//FastLED.delay(500);
//fill_solid(leds, 200, CRGB::Green);
//FastLED.show();
//FastLED.delay(500);
//fill_solid(leds, 300, CRGB::Red);

fill_gradient(leds, 0, CHSV(255,255,255), 300, CHSV(128, 255, 255), SHORTEST_HUES);

FastLED.show();
FastLED.delay(500);
}
