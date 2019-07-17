#include "FastLED.h"

#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <EEPROM.h>
// Change the next 6 defines to match your matrix type and size

#define LED_PIN        12
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   16
#define MATRIX_HEIGHT  -8
#define MATRIX_TYPE    VERTICAL_ZIGZAG_MATRIX
#define NUM_LEDS 128

uint8_t maxChanges = 24;      // Value for blending between palettes.
 
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;                                // NOBLEND or LINEARBLEND 


// Initialize changeable global variables. Play around with these!!!
#define qsubd(x, b)  ((x>b)?b:0)                   // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                          // Analog Unsigned subtraction macro. if result <0, then => 0
int8_t thisspeed = 8;                                         // You can change the speed of the wave, and use negative values.
uint8_t allfreq = 32;                                         // You can change the frequency, thus distance between bars.
int thisphase = 0;                                            // Phase change value gets calculated.
uint8_t thiscutoff = 192;                                     // You can change the cutoff value to display this wave. Lower value = longer wave.
int thisdelay = 30;                                           // You can change the delay. Also you can change the allspeed variable above. 
uint8_t bgclr = 0;                                            // A rotating background colour.
uint8_t bgbright = 10;                                        // Brightness of background colour


uint16_t Xorig = 0x012;
uint16_t Yorig = 0x015;
uint16_t X;
uint16_t Y;
uint16_t Xn;
uint16_t Yn;
uint8_t serendipity_index;

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

cLEDText ScrollingMsg;

byte mode;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

const unsigned char TxtDemo[] = { 
                                  EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" "    RANGER LAPLACE"
                                  //EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           FREE FOR USE OF BURNERS"
                                  //EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           ADVICE AND ASSISTANCE OBTAINABLE IMMEDIATELY"
                                  //EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           RANGERS RESPOND TO URGENT NEEDS"
                                  //EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           PULL TO OPEN"
                                  };

const unsigned char TxtDemo0[] = { 
                                  EFFECT_HSV_CH "\x00\xff\xff\xff\xff\xff" "    PEE CLEAR"
                                  EFFECT_HSV_AV "\x00\xff\xff\xff\xff\xff" "    SAFETY THIRD"
                                  EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" "    WELCOME HOME <3"
};
const unsigned char TxtDemo1[] = { EFFECT_RGB_AH "\x00\xff\x00\xff\xff\x00" "    FRENCH QUARTER"
                                   EFFECT_RGB_AH "\xff\xff\x00\xff\x00\xff" "    4:30 & ESPLANADE"
};

CRGBPalette16 gPal;
                                  
void setup()
{
  mode = EEPROM.read(0);
  if(mode >= 5) {
    mode = 0;
  } else mode++;
  //mode = 2;
  EEPROM.write(0, mode);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (wat)

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);
  if(mode < 3) FastLED.setBrightness(30);
  else FastLED.setBrightness(19);
  FastLED.clear(true);
  delay(500);
  leds(0) = CRGB::Aqua;
  delay(250);
  FastLED.show();

  currentPalette = PartyColors_p;
  currentBlending = LINEARBLEND;

  X=Xorig;
  Y=Yorig;
  
  gPal = CRGBPalette16( CRGB::Black, CRGB::Green, CRGB::Yellow, CRGB::Purple );
  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 2, 0, 0);
  switch(mode) {
      case 0:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;

      case 1:
      ScrollingMsg.SetText((unsigned char *)TxtDemo0, sizeof(TxtDemo0) - 1);
      break;

      case 2:
      ScrollingMsg.SetText((unsigned char *)TxtDemo1, sizeof(TxtDemo1) - 1);
      break;

      default:
      //ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;
  }
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
}


void loop()
{
  if (ScrollingMsg.UpdateText() == -1 && mode < 3)
    switch(mode) {
      case 0:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;

      case 1:
      ScrollingMsg.SetText((unsigned char *)TxtDemo0, sizeof(TxtDemo0) - 1);
      break;

      case 2:
      ScrollingMsg.SetText((unsigned char *)TxtDemo1, sizeof(TxtDemo1) - 1);
      break;

      default:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;
    }
  else if (mode == 3) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
    serendipitous();
    FastLED.show();
    delay(20);
  } else if (mode == 4) {
    //nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
    one_sine_pal(millis()>>4);
    FastLED.show(); // display this frame
    delay(20);
  } else if (mode == 5) {
    beatwave();
    FastLED.show(); // display this frame
    delay(20);
  }
  else { 
    FastLED.show();
    delay(75);
  }
  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    static uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

void serendipitous () {

  EVERY_N_SECONDS(5) {
    uint8_t baseC = random8();
    targetPalette = CRGBPalette16(CHSV(baseC-3, 255, random8(192,255)), CHSV(baseC+2, 255, random8(192,255)), CHSV(baseC+5, 192, random8(192,255)), CHSV(random8(), 255, random8(192,255)));

    X = Xorig;
    Y = Yorig;    
  }

//  Xn = X-(Y/2); Yn = Y+(Xn/2);
//  Xn = X-Y/2;   Yn = Y+Xn/2;
//  Xn = X-(Y/2); Yn = Y+(X/2.1);
  Xn = X-(Y/3); Yn = Y+(X/1.5);
//  Xn = X-(2*Y); Yn = Y+(X/1.1);
  
  X = Xn;
  Y = Yn;

  //int temp = 0;
  serendipity_index=(sin8(X)+cos8(Y))/2;                            // Guarantees maximum value of 255

  CRGB newcolor = ColorFromPalette(currentPalette, serendipity_index, 255, LINEARBLEND);
  
//  nblend(leds[X%NUM_LEDS-1], newcolor, 224);          // Try and smooth it out a bit. Higher # means less smoothing.
  nblend(leds[0][map(X,0,65535,0,NUM_LEDS)], newcolor, 224); // Try and smooth it out a bit. Higher # means less smoothing.
  
  fadeToBlackBy(leds[0], NUM_LEDS, 16);                    // 8 bit, 1 = slow, 255 = fast

} // serendipitous()

void one_sine_pal(uint8_t colorIndex) {                                       // This is the heart of this program. Sure is short.
  
  thisphase += thisspeed;                                                     // You can change direction and speed individually.
  
  for (int k=0; k<NUM_LEDS-1; k++) {                                          // For each of the LED's in the strand, set a brightness based on a wave as follows:
    int thisbright = qsubd(cubicwave8((k*allfreq)+thisphase), thiscutoff);    // qsub sets a minimum value called thiscutoff. If < thiscutoff, then bright = 0. Otherwise, bright = 128 (as defined in qsub)..
    leds[0][k] = CHSV(bgclr, 255, bgbright);                                     // First set a background colour, but fully saturated.
    leds[0][k] += ColorFromPalette( currentPalette, colorIndex, thisbright, currentBlending);    // Let's now add the foreground colour.
    colorIndex +=3;
  }
  
  bgclr++;
  
} // one_sine_pal()


void beatwave() {
  
  uint8_t wave1 = beatsin8(9, 0, 255);                        // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (int i=0; i<NUM_LEDS; i++) {
    leds[0][i] = ColorFromPalette( currentPalette, i+wave1+wave2+wave3+wave4, 255, currentBlending); 
  }
  
} // beatwave()
