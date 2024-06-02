// Eleonora Chacón Taylor 2024
// neopixel paper lamps project

#ifndef LAMPS_H
#define LAMPS_H

#include "defines.h"

// cochinadilla de FastLED
#include <FastLED.h>

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

struct Lamps
{
  private:
  CRGB leds[NUM_LEDS];
  //CRGBPalette16 currentPalette;
  CRGBPalette16 alternate1Palette;
  CRGBPalette16 alternate2Palette;
  TBlendType    currentBlending;
  uint16_t freezeCounter;
  uint8_t colorIndex;
  uint8_t prevLFi ;
  uint8_t prevHFi ;
  bool input; // user interface input (push-button)
  uint8_t scene;
  CRGB hue1, hue2; // hue used to compose current pallete

  public:
  Lamps();
  void configure(void);
  void setupPallete(void);
  void run(uint8_t, uint8_t);
  void FillLEDsPattern0(uint8_t led, CRGBPalette16 palette);
  void FillLEDsPattern1(uint8_t led, CRGBPalette16 palette);
  void fadeall(void);
  void fadeallR(void);
  CRGB getRandom(CRGB hue1, CRGB hue2);
  CRGBPalette16 SetupMonochomePallete(CRGB hue1, CRGB hue2);
  // user interface
  void toggleScene(bool input); // positive-edge activated
};



#endif // LAMPS_H