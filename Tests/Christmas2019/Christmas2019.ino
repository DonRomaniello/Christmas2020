#include <Encoder.h>


#include <FastLED.h>

#include "Timer.h"
#include "ClickButton.h"

Timer t;

#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 100
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
#define LED_TYPE    WS2811
#define COLOR_CORRECTION Candle
#define BRIGHTNESS  192

#define FPS 240

CRGB leds[NUM_LEDS];


const int buttonPin1 = 23;
ClickButton button1(buttonPin1, LOW, CLICKBTN_PULLUP);




// Flags and Toggles

bool hdran = false; // Has hue drops run
bool trainran = false;
bool pdran = false;
bool cdran = false; // Has Color Drops Run
bool whitetrainran = false; //Has Whitey run
bool huetrainran = false; // Has Hue Train Run
bool correctionlatch = false;



// Timers
unsigned long time_now = 0;
unsigned long time_now2 = 0;
unsigned long time_now3 = 0;

// Speed
int period = 20;
int period2 = 1000;
int period3 = 1000;

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



// Chaos Arrays

int rchaos1[NUM_LEDS + 2];
int gchaos1[NUM_LEDS + 2];
int bchaos1[NUM_LEDS + 2];
int rchaos2[NUM_LEDS + 2];
int gchaos2[NUM_LEDS + 2];
int bchaos2[NUM_LEDS + 2];
int hchaos1[NUM_LEDS + 2];
int schaos1[NUM_LEDS + 2];
int vchaos1[NUM_LEDS + 2];
int hchaos2[NUM_LEDS + 2];
int schaos2[NUM_LEDS + 2];
int vchaos2[NUM_LEDS + 2];




// Random starting colors
CRGB cola = CRGB(0,0,0);
CRGB colb = CRGB(0,0,0);
CRGB colc = CRGB(0,0,0);
CRGB cold = CRGB(0,0,0);

// Fraction of blend/etc - interpolate
int fade = 0;


Encoder myEnc(21, 22);
long oldPosition  = 255;

int clickers = 0;


void setup() {



FastLED.addLeds<1, WS2811, 1, RGB>(leds, 0);
FastLED.addLeds<1, WS2811, 17, RGB>(leds, NUM_LEDS_PER_STRIP);
FastLED.addLeds<1, WS2811, 20, RGB>(leds, 2 * NUM_LEDS_PER_STRIP);
FastLED.setMaxRefreshRate(240);
 Serial.begin(9600);




  
      FastLED.setBrightness(  BRIGHTNESS );



random16_set_seed(analogRead(A0));

// Fill chaos arrays
for (int i = 0; i < (NUM_LEDS + 2); i++) {
  rchaos1[i] = random8();
  gchaos1[i] = random8();
  bchaos1[i] = random8();
  rchaos2[i] = random8();
  gchaos2[i] = random8();
  bchaos2[i] = random8();
  hchaos1[i] = random8();
  schaos1[i] = random8();
  vchaos1[i] = random8();
  hchaos2[i] = random8();
  schaos2[i] = random8();
  vchaos2[i] = random8();
  
}

cola = CRGB(random8(),random8(),random8());
colb = CRGB(random8(),random8(),random8());
colc = CRGB(random8(),random8(),random8());
cold = CRGB(random8(),random8(),random8());





// Set Up Loop that runs every frame (FPS)      
 t.every((1000/FPS), showleds, (void*)0);


 
}

void loop() {

   t.update();
  //whitetrain();
  //colordrops();
  //calibrator();
  //train();
  //allblue();
  huetrain();
}

void showleds(void *context)
{
  FastLED.show();
  button1.Update();
  if (button1.clicks != 0) clickers = button1.clicks;
} 




/* Color Drops  Color Drops  Color Drops  Color Drops  Color Drops  Color Drops  Color Drops  Color Drops */

void colordrops(){
trainran = false;
hdran = false;
pdran = false;



  
if (cdran == false) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(random8(),random8(),random8());
  }
  cdran = true;
}

if(millis() > time_now + period){ //check to see if a 'tick' has happened
    time_now = millis();
    i = random16(NUM_LEDS); // Choose a random LED

changer = random8();
changeg = random8();
changeb = random8();


    
    cola = CRGB(changer, changeg, changeb);
    }



    
if (millis() > time_now2 + (period/255)) {
  leds[i] = blend(leds[i], cola, fade);
time_now2 = millis();
  fade++;
}
if (fade == 255) fade = 0;

}



/* Calibrator  Calibrator  Calibrator  Calibrator  Calibrator  Calibrator  Calibrator  Calibrator  Calibrator */

void calibrator() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
if (button1.clicks == 0) {
raw = newPosition;
}
if (button1.clicks == 1) {
gaw = newPosition;
}
if (button1.clicks == 2) {
baw = newPosition;
}
if (button1.clicks == -1) {
newPosition = 255;
}

  
fill_solid(leds, 300, CRGB::White);
  


}



// Train  Train  Train  Train  Train  Train  Train  Train  Train  Train  Train  Train  Train  

void train() {

hdran = false;
cdran = false;


if(millis() > time_now + period){
    time_now = millis();
    fade++;
}


for (int i = 0; i < 300; i++) {
  leds[i] = blend(CRGB(rchaos1[(i+2)], gchaos1[(i+2)], bchaos1[(i+2)]), CRGB(rchaos1[(i+1)], gchaos1[(i+1)], bchaos1[(i+1)]), ease8InOutQuad(fade));
}

if (fade == 255) { 
  fade = 0;
rchaos1[0] = random8();
gchaos1[0] = random8();
bchaos1[0] = random8();
for (int i = 301; i > 0; i--) {
  rchaos1[i] = rchaos1[(i - 1)];
  gchaos1[i] = gchaos1[(i - 1)];
  bchaos1[i] = bchaos1[(i - 1)];
}
}


}


void whitetrain() {

// Fill with Whites for first run
if (whitetrainran == false) {
for (int i = 0; i < (NUM_LEDS + 2); i++) {
  hchaos1[i] = random8();
  schaos1[i] = random8(whiteSatmax);
  vchaos1[i] = random8(whiteValmin, 255);
  hchaos2[i] = random8();
  schaos2[i] = random8(whiteSatmax);
  vchaos2[i] = random8(whiteValmin, 255);
}
for (int i = NUM_LEDS; i > 0; i--) {
leds[i] = CHSV((int)hchaos1,(int)schaos1,(int)vchaos1); 
}
whitetrainran = true;
fade = 0;
}


//Non-blocking time check
if(millis() > time_now + period){
    time_now = millis();
    fade++;
}

for (int i = 0; i < 300; i++) {
  leds[i] = blend(CHSV(hchaos1[(i+2)], schaos1[(i+2)], vchaos1[(i+2)]), CHSV(hchaos1[(i+1)], schaos1[(i+1)], vchaos1[(i+1)]), ease8InOutQuad(fade), SHORTEST_HUES);
}


if (fade == 255) {
fade = 0; // Reset fade
hchaos1[0] = random8();
schaos1[0] = random8(whiteSatmax);
vchaos1[0] = random8(whiteValmin, 255);
for (int i = 301; i > 0; i--) {
  hchaos1[i] = hchaos1[(i - 1)];
  schaos1[i] = schaos1[(i - 1)];
  vchaos1[i] = vchaos1[(i - 1)];
}
}



}




void huetrain() {


// Fill with Hues for first run
if (huetrainran == false) {
for (int i = 0; i < (NUM_LEDS + 2); i++) {
  hchaos1[i] = random8(HueMin, HueMax);
  schaos1[i] = random8(SatMin, SatMax);
  vchaos1[i] = random8(BriMin, BriMax);
  hchaos2[i] = random8(HueMin, HueMax);
  schaos2[i] = random8(SatMin, SatMax);
  vchaos2[i] = random8(BriMin, BriMax);
}
for (int i = NUM_LEDS; i > 0; i--) {
leds[i] = CHSV((int)hchaos1,(int)schaos1,(int)vchaos1); 
}
huetrainran = true;
fade = 0;
}


//Non-blocking time check
if(millis() > time_now + period){
    time_now = millis();
    fade++;
}


// Update each LED to be a fade between two values
for (int i = 0; i < 300; i++) {
  leds[i] = blend(CHSV(hchaos1[(i+2)], schaos1[(i+2)], vchaos1[(i+2)]), CHSV(hchaos1[(i+1)], schaos1[(i+1)], vchaos1[(i+1)]), ease8InOutQuad(fade), SHORTEST_HUES);
}


// When fade is 100%
if (fade == 255) {
fade = 0; // Reset fade

// Create a new random value for the first entry
  hchaos1[i] = random8(HueMin, HueMax);
  schaos1[i] = random8(SatMin, SatMax);
  vchaos1[i] = random8(BriMin, BriMax);

  // Move values over one
for (int i = 301; i > 0; i--) {
  hchaos1[i] = hchaos1[(i - 1)];
  schaos1[i] = schaos1[(i - 1)];
  vchaos1[i] = vchaos1[(i - 1)];
}
}



}



// Allblue is actually just a test loop
void allblue () {
for (int i = 0; i < 100; i++) {
  leds[i] = CRGB(i, 0, 255);
}
for (int i = 0; i < 100; i++) {
  leds[i+100] = CRGB(255, i, 0);
}
for (int i = 0; i < 100; i++) {
  leds[i+200] = CRGB(0, 255, i);
}



}



// Color Correction

void corrections () {

  
if (correctionlatch == false) {
 LEDS.setCorrection( Candle );
}

 
if (correctionlatch == true) {
 LEDS.setCorrection( Halogen );
}

if(millis() > time_now3 + period3){
  time_now3 = millis();
  correctionlatch = !(correctionlatch);   
}



}
