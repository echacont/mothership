// Eleonora Chacón Taylor 2024
// neopixel paper lamps project

#include "lamps.h"

Lamps::Lamps(void) { }

void Lamps::configure(void)
  // FastLED config
  {
    delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    //currentPalette = RainbowColors_p;
    //SetupPurpleAndGreenPalette();
    SetupMonochomePallete(CRGB::Fuchsia, CRGB::OrangeRed);
    currentBlending = LINEARBLEND;
    //currentBlending = NOBLEND;
  }

void Lamps::run(uint8_t LFi, uint8_t HFi)
{
  static uint8_t startIndex = 0;
  static uint8_t led = 0;
  static uint8_t prevLFi = 0;
  static uint8_t prevHFi = 0;
  // generate events
  //if (LFi == 2)     startIndex += 8;
  //if (LFi == 4)     startIndex += 1;
  //if (LFi == 1)     startIndex -= 1;
  //if (HFi == 4)     SetupYellowAndBluePalette();
  //else              SetupPurpleAndGreenPalette();
  
  if ((LFi == 0) && (HFi == 2)) {
    //startIndex++;
    led = random8();
  }
  if ((LFi == 0) && (prevLFi == 0) && (HFi == 2))
    SetupMonochomePallete(CRGB::Magenta, CRGB::MediumPurple);
  if ((LFi == 1) && (prevLFi == 0) && (HFi == 4)) 
    SetupMonochomePallete(CRGB::Fuchsia, CRGB::DarkRed);
    //startIndex += 4;
  //else
    //SetupPurpleAndGreenPalette();
  
  if ((LFi == 1) && (prevHFi == 3) && (HFi == 0)) {
    SetupMonochomePallete(CRGB::DarkOrchid, CRGB::Orchid);
    //startIndex -= 2;
  }
  //led = random8();
  
  //if (LFi < 4) led = LFi;
  //else led = 3;
  //FillLEDsStrip(startIndex);
  FillLEDsPattern1(startIndex, led);
  //FillLEDsFromPaletteColors(startIndex);
  //FillLEDsPallete1(startIndex, led);
  FastLED.show();
  prevHFi = HFi; prevLFi = LFi;
}

void Lamps::fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(240); } }

void Lamps::fadeallR() 
{ 
  for(uint8_t i = 0; i < NUM_LEDS/NUM_LEDS_PER_GROUP; i++) 
  { 
    for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
    {
      leds[i*NUM_LEDS_PER_GROUP+j].nscale8(240); 
    } 
  }
}

void Lamps::FillLEDsStrip(uint8_t colorIndex)
{
    for( uint8_t i = 0; i < NUM_LEDS; ++i) 
    {
          leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
          colorIndex += 1;
    }
}

void Lamps::FillLEDsPattern0(uint8_t colorIndex, uint8_t rndm )
{
    for( uint8_t i = 0; i < NUM_LEDS/NUM_LEDS_PER_GROUP; ++i)
      if ((rndm % NUM_LEDS/NUM_LEDS_PER_GROUP) == i)
        for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
          leds[4*i+j] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
    fadeall();
}

void Lamps::FillLEDsPattern1(uint8_t colorIndex, uint8_t led)
{
  for( uint8_t i = 0; i < NUM_LEDS/NUM_LEDS_PER_GROUP; ++i)
  {
    led = random8();
    if ((led % 2))
      for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
      {
        leds[4*i+j] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
        colorIndex += 1;
      }
    //else
    //  for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
    //    leds[4*i+j] = CRGB::Black;
  }
  fadeallR();
}

// This function sets up a palette of purple and green stripes.
void Lamps::SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green , black, green , black,  
                                   purple, black, purple, black,
                                   green , black, green,  black,
                                   purple, purple,black,  black );
}

void Lamps::SetupYellowAndBluePalette(void)
{
    CRGB yellow = CHSV( HUE_YELLOW, 255, 255);
    CRGB blue  = CHSV( HUE_BLUE, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   yellow , yellow ,black,  black,
                                   blue   , blue   ,black,  black,
                                   yellow , yellow ,black,  black,
                                   blue   , blue   ,black,  black );
}

CRGB Lamps::getRandom(CRGB hue1, CRGB hue2)
{
  CRGB black  = CRGB::Black;
  uint8_t rndm = random8() % 3;
  if (rndm == 0)
    return hue2;
  else if (rndm == 1)
    return hue1;
  else return black;
}

void Lamps::SetupMonochomePallete(CRGB hue1, CRGB hue2)
{
  currentPalette = CRGBPalette16( getRandom(hue1, hue2), getRandom(hue1, hue2),
            getRandom(hue1, hue2), getRandom(hue1, hue2),
      getRandom(hue1, hue2), getRandom(hue1, hue2),
     getRandom(hue1, hue2), getRandom(hue1, hue2),
      getRandom(hue1, hue2), getRandom(hue1, hue2),
     getRandom(hue1, hue2), getRandom(hue1, hue2),
      getRandom(hue1, hue2), getRandom(hue1, hue2),
       getRandom(hue1, hue2), getRandom(hue1, hue2) ); 
}