// Eleonora Chacón Taylor 2024
// neopixel paper lamps project

#include "lamps.h"

Lamps::Lamps(void) 
{ 
  colorIndex = 0;
  freezeCounter = 0;
  prevLFi = 0;
  prevHFi = 0;
}

void Lamps::configure(void)
{
    delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    setupPallete();
    currentBlending = LINEARBLEND;
}

void Lamps::setupPallete(void)
{
  alternate1Palette =  SetupMonochomePallete(CRGB::DarkMagenta, CRGB::Violet);
  //alternate2Palette =  SetupMonochomePallete(CRGB::MediumTurquoise, CRGB::OrangeRed);
}

void Lamps::run(uint8_t LFi, uint8_t HFi)
{
  static uint8_t led = 0;

  if (freezeCounter > 0) // changes are frozen except for fadeall
  {
    freezeCounter--;
  }
  else // freezeCounter is zero (expired)
  {
    // generate events for next loop
    if ((LFi == 0) && (HFi == 2)) 
    { 
      freezeCounter = FREEZE_TIME*2;
      led = random8() % NUM_LEDS/NUM_LEDS_PER_GROUP;
      FillLEDsPattern1(led, alternate1Palette);
    }
    else if ((LFi == 1) && (prevLFi == 0) && (HFi == 4)) {
      //colorIndex += C_CHANGE*8;
      setupPallete();
      freezeCounter = FREEZE_TIME;
    } else  if ((LFi == 0) && (prevLFi == 0) && (HFi == 2)) {
      freezeCounter = FREEZE_TIME;
      FillLEDsPattern1(led, alternate1Palette);
    }
    FillLEDsPattern0(led, alternate1Palette);
  }
  fadeall();
  FastLED.show();
  prevHFi = HFi; prevLFi = LFi;
}

void Lamps::fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(247); } }

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


void Lamps::FillLEDsPattern0(uint8_t led , CRGBPalette16 palette)
{
  for( uint8_t i = 0; i < NUM_LEDS/NUM_LEDS_PER_GROUP; ++i)
    //if ((led % (NUM_LEDS/NUM_LEDS_PER_GROUP)) == i)
    if (led == i)
      for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
      {
        leds[4*i+j] = ColorFromPalette(palette, colorIndex, BRIGHTNESS, currentBlending);
        colorIndex += C_CHANGE;
      }
  fadeall();
}

void Lamps::FillLEDsPattern1(uint8_t led, CRGBPalette16 palette)
{
  for( uint8_t i = 0; i < NUM_LEDS/NUM_LEDS_PER_GROUP; ++i)
  {
    if (!(led % P_CHANGE))
      for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
      {
        leds[NUM_LEDS_PER_GROUP*i+j] = ColorFromPalette(palette, colorIndex, BRIGHTNESS, currentBlending);
        colorIndex -= C_CHANGE;
      }
  }
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

CRGBPalette16 Lamps::SetupMonochomePallete(CRGB hue1, CRGB hue2)
{
  return CRGBPalette16( getRandom(hue1, hue2), getRandom(hue1, hue2),
                        getRandom(hue1, hue2), getRandom(hue1, hue2),
                        getRandom(hue1, hue2), getRandom(hue1, hue2),
                        getRandom(hue1, hue2), getRandom(hue1, hue2),
                        getRandom(hue1, hue2), getRandom(hue1, hue2),
                        getRandom(hue1, hue2), getRandom(hue1, hue2),
                        getRandom(hue1, hue2), getRandom(hue1, hue2),
                        getRandom(hue1, hue2), getRandom(hue1, hue2) ); 
}