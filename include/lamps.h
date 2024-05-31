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
  CRGBPalette16 currentPalette;
  TBlendType    currentBlending;

  void fadeall(void);
  void fadeallR(void);

  public:
  Lamps();
  void configure(void);
  void run(uint8_t, uint8_t);
  void FillLEDsStrip(uint8_t colorIndex);
  void FillLEDsPattern0(uint8_t colorIndex, uint8_t rndm );
  void FillLEDsPattern1(uint8_t colorIndex, uint8_t led);
  void SetupPurpleAndGreenPalette(void);
  void SetupYellowAndBluePalette(void);
  CRGB getRandom(CRGB hue1, CRGB hue2);
  void SetupMonochomePallete(CRGB hue1, CRGB hue2);


};



#endif // LAMPS_H