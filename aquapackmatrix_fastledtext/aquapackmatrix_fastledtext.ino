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

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

cLEDText ScrollingMsg;

byte mode;

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
const unsigned char TxtDemo2[] = { 
                                  };


                                  
void setup()
{
  mode = EEPROM.read(0);
  mode = !mode;
  EEPROM.write(0, mode);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);;
  FastLED.setBrightness(30);
  FastLED.clear(true);
  delay(500);
  FastLED.showColor(CRGB::Aqua);
  delay(250);
  FastLED.show();

  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
  switch(mode) {
      case 0:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;

      case 1:
      ScrollingMsg.SetText((unsigned char *)TxtDemo0, sizeof(TxtDemo0) - 1);
      break;
  }
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
}


void loop()
{
  if (ScrollingMsg.UpdateText() == -1)
    switch(mode) {
      case 0:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;

      case 1:
      ScrollingMsg.SetText((unsigned char *)TxtDemo0, sizeof(TxtDemo0) - 1);
      break;

      case 2:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;

      default:
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
      break;
    }
  else
    FastLED.show();
  delay(75);
}
