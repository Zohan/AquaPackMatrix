#include <FastLED.h>

#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <EEPROM.h>
// Change the next 6 defines to match your matrix type and size

#define LED_PIN        A3
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   16
#define MATRIX_HEIGHT  -8
#define MATRIX_TYPE    VERTICAL_ZIGZAG_MATRIX
#define NUM_LEDS 128

#define COOLING  55
#define SPARKING 120

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
const unsigned char TxtDemo1[] = { EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" "    FUCK YOUR BURN"
                                   EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" "    I <3 YOU"
};

CRGBPalette16 gPal;
                                  
void setup()
{
  mode = EEPROM.read(0);
  if(mode >= 4) {
    mode = 0;
  } else mode++;
  EEPROM.write(0, mode);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);
  if(mode < 3) FastLED.setBrightness(30);
  else FastLED.setBrightness(15);
  FastLED.clear(true);
  delay(500);
  leds(0) = CRGB::Aqua;
  delay(250);
  FastLED.show();
  
  gPal = CRGBPalette16( CRGB::Black, CRGB::Green, CRGB::Yellow, CRGB::Purple );
  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
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
    confetti();
    FastLED.show();
    delay(20);
  } else if (mode == 4) {
    random16_add_entropy( random());
    Fire2012WithPalette(); // run simulation frame, using palette colors
    FastLED.show(); // display this frame
    delay(20);
  }
  else { 
    FastLED.show();
    delay(75);
  }
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  uint8_t scale = 255-10;
  for( uint16_t i = 0; i < 128; i++) {
        leds(i).nscale8( scale);
  }
  int pos = random16(128);
  leds(pos) += CHSV( gHue + random8(64), 200, 255);
}

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      pixelnumber = j;
      leds(pixelnumber) = color;
    }
}
