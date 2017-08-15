#include <FastLED.h>

#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <EEPROM.h>
// Change the next 6 defines to match your matrix type and size

#define LED_PIN        2
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   -8
#define MATRIX_HEIGHT  -24
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX
#define NUM_LEDS 192

#define COOLING  55
#define SPARKING 120

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

cLEDText ScrollingMsg;

byte mode;
byte textSteps = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// Palette definitions
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;

const unsigned char TxtDemo[] = { 
                                  EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" EFFECT_SCROLL_UP "    NICE BUTT"
                                  };
const unsigned char TxtDemo0[] = { 
                                  EFFECT_HSV_CH "\x00\xff\xff\xff\xff\xff" EFFECT_SCROLL_UP "    PEE CLEAR"
                                  EFFECT_HSV_AV "\x00\xff\xff\xff\xff\xff" EFFECT_SCROLL_UP "    SAFETY THIRD"
                                  EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" EFFECT_SCROLL_UP "    WELCOME HOME"
                                  EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" EFFECT_SCROLL_UP "    FUCK YOUR BURN MV"
};

const unsigned char TxtDemo1[] = { EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" EFFECT_SCROLL_UP "    POLEGASM"
                                   EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" EFFECT_SCROLL_UP "    7:30 PORTAL"
};

const unsigned char TxtDemo2[] = { EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" EFFECT_SCROLL_UP "    OZ"
};

const unsigned char TxtDemo3[] = { EFFECT_HSV_CV "\x10\xff\xff\x40\xff\xff" EFFECT_SCROLL_UP "OZ"
};
                                  
void setup()
{
  mode = EEPROM.read(0);
  if(mode >= 4) {
    mode = 0;
  } else mode++;
  EEPROM.write(0, mode);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(15);
  FastLED.clear(true);
  delay(500);
  leds(2) = CRGB::Aqua;
  delay(250);
  FastLED.show();

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  
  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds, ScrollingMsg.FontWidth(), ScrollingMsg.FontHeight()+12, 1, 0);
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

      case 3:
      //ScrollingMsg.Init(&leds, ScrollingMsg.FontWidth(), ScrollingMsg.FontHeight()+28, 1, -24);
      ScrollingMsg.SetText((unsigned char *)TxtDemo2, sizeof(TxtDemo2) - 1);
      break;

      default:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;
  }
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
}


void loop()
{
  if (mode < 3 && ScrollingMsg.UpdateText() == -1)
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
    if(textSteps < 35) {
      ScrollingMsg.UpdateText();
      textSteps++;
    } else {
      ScrollingMsg.Init(&leds, ScrollingMsg.FontWidth(), ScrollingMsg.FontHeight()+8, 1, 6);
      ScrollingMsg.SetText((unsigned char *)TxtDemo3, sizeof(TxtDemo3) - 1);
      ScrollingMsg.UpdateText();
      confetti();
    }
    FastLED.show();
    delay(20);
  } else if (mode == 4) {
    beatwave();
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
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
        leds(i).nscale8( scale);
  }
  int pos = random16(NUM_LEDS);
  leds(pos) += CHSV( gHue + random8(64), 200, 255);
}

void beatwave() {
  
  uint8_t wave1 = beatsin8(9, 0, 255);                        // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (int i=0; i<NUM_LEDS; i++) {
    leds(i) = ColorFromPalette( currentPalette, i+wave1+wave2+wave3+wave4, 255, currentBlending); 
  }
  
} // beatwave()
