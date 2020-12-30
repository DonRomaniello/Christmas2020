#include <Encoder.h>
#include <Entropy.h>
#include <FastLED.h>
#include <OctoWS2811.h>
#include <FastLED.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Timer.h"
#include "ClickButton.h"




#define COLOR_CORRECTION Candle
int BRIGHTNESS = 64;

#define DATA_PIN_BOTTOM 1
#define DATA_PIN_MIDDLE 17
#define DATA_PIN_TOP 20

#define FPS 240
#define OLED_FPU (240 / 30)
#define NUM_LEDS 300


// OLED stuff
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);
unsigned long displayTimeout = (5 * 1000);

// Led stuff
const int numPins = 3;
byte pinList[numPins] = {DATA_PIN_BOTTOM, DATA_PIN_MIDDLE, DATA_PIN_TOP};
const int ledsPerStrip = 100;
CRGB leds[numPins * ledsPerStrip];
CRGB leds_B[numPins * ledsPerStrip]; // Also create non-displayed LED arrays for holding effects
CRGB leds_C[numPins * ledsPerStrip];  // ...
CRGB leds_ChaosRGB[numPins * ledsPerStrip + 2]; // chaos arrays
CRGB leds_ChaosHSV[numPins * ledsPerStrip + 2]; // ...

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
      : pocto(_pocto) {};

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


// Flags and Toggles

bool hdRan = false; // Has hue drops run
bool trainRan = false;
bool pdRan = false;
bool cdRan = false; // Has Color Drops Run
bool whiteTrainRan = false; //Has Whitey run
bool hueTrainRan = false; // Has Hue Train run
bool hsvGradientran = false; // Has the hsvGradient run
bool rgbGradientran = false; //Has the rgbGradient run
bool ledSelectRan = false;

bool paused = false;
bool correctionLatch = false;
bool inputsChanged = true;
bool timedOut = false;
int oled_refresh = 0;
int aMode = 0;
int modes = 7;




// Timers
unsigned long time_now = 0;
unsigned long time_now2 = 0;
unsigned long time_now3 = 0;
unsigned long displayTimeoutTimer = 0;

// Speed
long period = 1000;
long period2 = 1000;
long period3 = 1000;

// Values That Need Be addressed
int changer = 0;
int changeg = 0;
int changeb = 0;

int raw = 255;
int gaw = 255;
int baw = 255;


//Color Limits

int whiteSatmax = 96;
int whiteValmin = 200;

int HueMin = 0;
int HueMax = 255;
int BriMax = 255;
int BriMin = 200;
int SatMax = 255;
int SatMin = 128;

int i = 0;


// Random starting colors, herebelow set at zero
CRGB colA = CRGB(0, 0, 0);
CRGB colB = CRGB(0, 0, 0);
CRGB colC = CRGB(0, 0, 0);
CRGB colD = CRGB(0, 0, 0);

CHSV gradTopA = CHSV(0, 0, 0);
CHSV gradTopB = CHSV(0, 0, 0);
CHSV gradBottomA = CHSV(0, 0, 0);
CHSV gradBottomB = CHSV(0, 0, 0);



// Fraction of blend/etc - interpolate
int fade = 0;

// Buttons and knobs

Encoder topKnob(11, 12);
Encoder bottomKnob(9, 10);

// Write positions to represent hardcoded brightness and period settings

long oldPositionTop  = BRIGHTNESS;
long oldPositionBottom  = period;
long newPositionTop = oldPositionTop;
long newPositionBottom = oldPositionBottom;

// Click buttons
ClickButton buttonTop(14, LOW, CLICKBTN_PULLUP);
ClickButton buttonBottom(15, LOW, CLICKBTN_PULLUP);

// Click button states
int clickerTop = 0;
int clickerBottom = 0;


//Declare that there will be a timer, 't'
Timer t;

//
//  /$$$$$$ /$$$$$$$$/$$$$$$$$/$$   /$$/$$$$$$$
// /$$__  $| $$_____|__  $$__| $$  | $| $$__  $$
//| $$  \__| $$        | $$  | $$  | $| $$  \ $$
//|  $$$$$$| $$$$$     | $$  | $$  | $| $$$$$$$/
// \____  $| $$__/     | $$  | $$  | $| $$____/
// /$$  \ $| $$        | $$  | $$  | $| $$
//|  $$$$$$| $$$$$$$$  | $$  |  $$$$$$| $$
// \______/|________/  |__/   \______/|__/


void setup() {
  //Initialize entropy
  Entropy.Initialize();

  octo.begin();

  pcontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octo);
  FastLED.addLeds(pcontroller, leds, numPins * ledsPerStrip);
  FastLED.setMaxRefreshRate(FPS);
  FastLED.setBrightness(  BRIGHTNESS );

  // Set random seed

  random16_set_seed(Entropy.random(WDT_RETURN_WORD));
  randomSeed(Entropy.random(WDT_RETURN_WORD));

  // Fill chaos arrays
  for (int i = 0; i < (NUM_LEDS + 2); i++) {
    leds_ChaosRGB[i] = CRGB(random8(), random8(), random8());
    leds_ChaosHSV[i] = CRGB(random8(), random8(), random8());

  }

  colA = CRGB(random8(), random8(), random8());
  colB = CRGB(random8(), random8(), random8());
  colC = CRGB(random8(), random8(), random8());
  colD = CRGB(random8(), random8(), random8());

  gradTopA = CHSV(random8(), random8(128, 255), 255);
  gradTopB = CHSV(random8(), random8(128, 255), 255);
  gradBottomA = CHSV(random8(), random8(128, 255), 255);
  gradBottomB = CHSV(random8(), random8(128, 255), 255);

  // Set Up Loop that runs every frame (FPS)
  t.every((1000 / FPS), showleds, (void*)0);

  // Knobs
  topKnob.write(BRIGHTNESS);
  bottomKnob.write(period);



  // Click buttons
  buttonTop.debounceTime   = 20;   // Debounce timer in ms
  buttonTop.multiclickTime = 250;  // Time limit for multi clicks
  buttonTop.longClickTime  = 1000; // time until "held-down clicks" register

  buttonBottom.debounceTime   = 20;   // Debounce timer in ms
  buttonBottom.multiclickTime = 250;  // Time limit for multi clicks
  buttonBottom.longClickTime  = 1000; // time until "held-down clicks" register



  // OLED Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(2);
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
}

//
//  EEEE N   N DDD       SSS  EEEE TTTTTT U   U PPPP
//  E    NN  N D  D     S     E      TT   U   U P   P
//  EEE  N N N D  D      SSS  EEE    TT   U   U PPPP
//  E    N  NN D  D         S E      TT   U   U P
//  EEEE N   N DDD      SSSS  EEEE   TT    UUU  P
//

void loop() {




  inputs();
  settings();
  oled();
  t.update();
  rgbGradient();

  if ((aMode % modes) == 0) rgbGradient();
  else if ((aMode % modes) == 1) hsvGradient();
  else if ((aMode % modes) == 2) whiteTrain();
  else if ((aMode % modes) == 3) colorDrops();
  else if ((aMode % modes) == 4) train();
  else if ((aMode % modes) == 5) hueTrain();
  else colorDrops();
  //ledSelect();
}


void showleds(void *context)
{
  FastLED.show();
}

void inputs() {
  knobs();
  clickers();
}

void settings() {
  FastLED.setBrightness(  oldPositionTop );
  period = oldPositionBottom;
  if (clickerTop == 1){
    aMode++;
    clickerTop = 0;
  }
}

// Color Correction

void corrections () {

  if (correctionLatch == false) {
    FastLED.setCorrection( Candle );
  }


  if (correctionLatch == true) {
    LEDS.setCorrection( Halogen );
  }

  if (millis() > time_now3 + period3) {
    time_now3 = millis();
    correctionLatch = !(correctionLatch);
  }
}


void knobs() {
  newPositionTop  = topKnob.read();
  newPositionBottom  = bottomKnob.read();
  if (newPositionTop != oldPositionTop) {    
    if (newPositionTop < 0) newPositionTop = 0;
    if (newPositionTop > 255) newPositionTop = 255;
    oldPositionTop = newPositionTop;
    inputsChanged = true;
  }
  if (newPositionBottom != oldPositionBottom) {
    if (clickerBottom == 0) { // If the mode is default, make the bottom knob change by twentieths of a second
      oldPositionBottom = (((newPositionBottom - oldPositionBottom) * 12.5) + oldPositionBottom);
      bottomKnob.write(oldPositionBottom);
    }
    else oldPositionBottom = newPositionBottom;
    inputsChanged = true;
  }



}

void clickers() {
  buttonTop.Update();
  buttonBottom.Update();
  // Save click codes in function, as click codes are reset at next Update()
  if (buttonTop.clicks != 0) {
    clickerTop = buttonTop.clicks; 
    inputsChanged = true;
  }
  if (buttonBottom.clicks != 0) {
    clickerBottom = buttonBottom.clicks;
    inputsChanged = true;
  }

}


void timeout() {

  if (inputsChanged == true) {
    displayTimeoutTimer = millis();
    inputsChanged = false;
    timedOut = false;
  }
  else if ((millis() - displayTimeoutTimer) >= displayTimeout) {
    display.clearDisplay();
    display.display();
    timedOut = true;
  }

}



void oled() {
  timeout();
  if (timedOut == false) {
    float brightReport = ((float) oldPositionTop / 2.55);
    float cpsReport = ((float) oldPositionBottom / 1000);
    info("Brightness:", brightReport, "%", "Speed", cpsReport, "CPS");  
  }
  oled_display();

}
 
void oled_display() {
  // To avoid excessive updates of the OLED display, refresh at a lower rate
  if ( OLED_FPU == oled_refresh) {
    display.display();
    oled_refresh = 0;
  }
  else {
    oled_refresh++;
  }
}

void info(const char* fieldA, float valueA, const char* unitA, const char* fieldB, float valueB, const char* unitB) {
  display.clearDisplay(); 
  display.setCursor(0, 0);            // Start at top-left corner
  display.print(fieldA);
  display.setCursor(70, 0);
  display.print(valueA);
  display.print(unitA);
  display.setCursor(0, 12);
  display.print(fieldB);
  display.setCursor(70, 12);
  display.print(valueB);
  display.print(unitB);
}

/* Color Drops  Color Drops  Color Drops  Color Drops  Color Drops  Color Drops  Color Drops  Color Drops */

void colorDrops() {
  trainRan = false;
  hdRan = false;
  pdRan = false;




  if (cdRan == false) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(random8(), random8(), random8());
    }
    cdRan = true;
  }

  if (millis() > time_now + period) { //check to see if a 'tick' has happened
    time_now = millis();
    i = random16(NUM_LEDS); // Choose a random LED

    changer = random8();
    changeg = random8();
    changeb = random8();



    colA = CRGB(changer, changeg, changeb);
  }




  if (millis() > time_now2 + (period / 255)) {
    leds[i] = blend(leds[i], colA, fade);
    time_now2 = millis();
    fade++;
  }
  if (fade == 255) fade = 0;

}





// Train  Train  Train  Train  Train  Train  Train  Train  Train  Train  Train  Train  Train

void train() {

  hdRan = false;
  cdRan = false;


  if (millis() > time_now + period) {
    time_now = millis();
    fade++;
  }


  for (int i = 0; i < 300; i++) {
    leds[i] = blend(leds_ChaosRGB[(i + 2)], leds_ChaosRGB[(i + 1)], ease8InOutQuad(fade));
  }




  if (fade == 255) {
    fade = 0;
    leds_ChaosRGB[0] = CRGB(random8(),random8(),random8());
    for (int i = 301; i > 0; i--) {
      leds_ChaosRGB[i] = leds_ChaosRGB[(i - 1)];
    }
  }


}


void whiteTrain() {

  // Fill with Whites for first run
  if (whiteTrainRan == false) {
    for (int i = 0; i < (NUM_LEDS + 2); i++) {
      leds_ChaosHSV[i] = CHSV(random8(), random8(whiteSatmax), random8(whiteValmin, 255));
    }
    for (int i = NUM_LEDS; i > 0; i--) {
      leds[i] = leds_ChaosHSV[i];
    }
    whiteTrainRan = true;
    fade = 0;
  }


  //Non-blocking time check
  if (millis() > time_now + period) {
    time_now = millis();
    fade++;
  }

  for (int i = 0; i < 300; i++) {
    leds[i] = blend(leds_ChaosHSV[(i + 2)], leds_ChaosHSV[(i + 1)], ease8InOutQuad(fade));
  }

  if (fade == 255) {
    fade = 0; // Reset fade
    leds_ChaosHSV[0] = CHSV(random8(), random8(whiteSatmax), random8(whiteValmin, 255));
    for (int i = 301; i > 0; i--) {
      leds_ChaosHSV[i] = leds_ChaosHSV[(i - 1)];
    }
  }
}




void hueTrain() {


  // Fill with Hues for first run
  if (hueTrainRan == false) {
    for (int i = 0; i < (NUM_LEDS + 2); i++) {
      leds_ChaosHSV[i] = CHSV(random8(), random8(whiteSatmax), random8(whiteValmin, 255));
      
    }
    for (int i = NUM_LEDS; i > 0; i--) {
      leds[i] = leds_ChaosHSV[i];
    }
    hueTrainRan = true;
    fade = 0;
  }


  //Non-blocking time check
  if (millis() > time_now + period) {
    time_now = millis();
    fade++;
  }


  // Update each LED to be a fade between two values
  for (int i = 0; i < 300; i++) {
    leds[i] = blend(leds_ChaosHSV[(i + 2)], leds_ChaosHSV[(i + 1)], ease8InOutQuad(fade));
  }


  // When fade is 100%
  if (fade == 255) {
    fade = 0; // Reset fade

    // Create a new random value for the first entry
    leds_ChaosHSV[0] = CHSV(random8(HueMin, HueMax), random8(SatMin, SatMax), random8(BriMin, BriMax));

    // Move values over one
    for (int i = 301; i > 0; i--) {
      leds_ChaosHSV[i] = leds_ChaosHSV[(i - 1)];
    }
  }
}


// HSV Gradient

void hsvGradient () {

  if (millis() > time_now + period) {
    time_now = millis();
    fade++;
  }


  fill_gradient(leds, 0, blend(gradBottomA, gradBottomB, ease8InOutQuad(fade)), NUM_LEDS, blend(gradTopA, gradTopB, ease8InOutQuad(fade)), FORWARD_HUES);



  if (fade == 255) {
    fade = 0;

    gradBottomA = gradBottomB;
    gradBottomB = CHSV(random8(), random8(128, 255), 255);

    gradTopA = gradTopB;
    gradTopB = CHSV(random8(), random8(128, 255), 255);

  }
}



void rgbGradient () {

  if (rgbGradientran == false) {
    hsv2rgb_rainbow(CHSV(random8(), random8(128, 255), 255), colA);
    hsv2rgb_rainbow(CHSV(random8(), random8(128, 255), 255), colB);
    hsv2rgb_rainbow(CHSV(random8(), random8(128, 255), 255), colC);
    hsv2rgb_rainbow(CHSV(random8(), random8(128, 255), 255), colD);
    rgbGradientran = true;
  }


  if (millis() > time_now + period) {
    time_now = millis();
    fade++;
  }

  
  //fill_gradient_RGB(leds, 0, blend (colA, colC, ease8InOutQuad(fade)), NUM_LEDS, blend (colB, colD, ease8InOutQuad(fade)));



  CRGB layer0_End = blend(blend (colA, colC, ease8InOutQuad(fade)),blend (colB, colD, ease8InOutQuad(fade)), 28);
  CRGB layer1_End = blend(blend (colA, colC, ease8InOutQuad(fade)),blend (colB, colD, ease8InOutQuad(fade)), 57);
  CRGB layer2_End = blend(blend (colA, colC, ease8InOutQuad(fade)),blend (colB, colD, ease8InOutQuad(fade)), 85);
  CRGB layer3_End = blend(blend (colA, colC, ease8InOutQuad(fade)),blend (colB, colD, ease8InOutQuad(fade)), 114);
  
// Layer 0
  fill_gradient_RGB(leds, 0, (blend (colA, colC, ease8InOutQuad(fade))), 48, layer0_End);
// Layer 1
  fill_gradient_RGB(leds, 49, layer0_End, 97, layer1_End);
// Layer 2
  fill_gradient_RGB(leds, 98, layer1_End, 145, layer2_End);
// Layer 3
  fill_gradient_RGB(leds, 146, layer2_End, 193, layer3_End);
// Top
  fill_gradient_RGB(leds, 194, layer3_End, NUM_LEDS, blend (colB, colD, ease8InOutQuad(fade)));

  if (fade == 255) {
    fade = 0;
    colA = colC;
    random16_set_seed(Entropy.random(WDT_RETURN_WORD));
    hsv2rgb_rainbow(CHSV(random8(), random8(128, 255), 255), colC);

    colB = colD;
    hsv2rgb_rainbow(CHSV(random8(), random8(128, 255), 255), colD);

  }
}



void ledSelect () {
  if (ledSelectRan == false) {
    topKnob.write(0);
    ledSelectRan = true;
    fill_solid(leds, 300, CRGB::Black);
  }
  
  
  
  leds[(topKnob.read()/4)] = CHSV(random8(), random8(128, 255), 255);
  display.clearDisplay();
  display.display();
  display.setCursor(0, 0);            // Start at top-left corner
  display.print("Number:");
  display.setCursor(70, 0);
  display.print((topKnob.read()/4));
  display.display();
  
}
