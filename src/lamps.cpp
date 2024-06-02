// Eleonora Chacón Taylor 2024
// neopixel paper lamps project

#include "lamps.h"

Lamps::Lamps(void) 
{ 
  colorIndex = 0;
  freezeCounter = 0;
  prevLFi = 0;
  prevHFi = 0;
  input = false;
  scene = 0;
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
  alternate1Palette =  SetupMonochomePallete(hue1, hue2); 
  alternate2Palette = SetupMonochomePallete(CRGB::MintCream, CRGB::LimeGreen);
}

void Lamps::toggleScene(bool input)
{
  static bool prevInput = false;

  if (!prevInput && input) // positive edge
  {
    setupPallete();
    scene++;
  }
  switch(scene)
  {
      case 0:
        hue1 = CRGB::LawnGreen; hue2 = CRGB::LimeGreen; break;  // ** warmup
      case 1:
        hue1 = CRGB::Crimson;   hue2 = CRGB::Red; break;   // ** peak
      case 2:
        hue1 = CRGB::LightPink; hue2 = CRGB::Purple; break; // ** B2B
      default:
        scene = 0; 
  } 
  
  prevInput = input;
}

// take FFT results summarized by LFi and HFi and generate events
// mimicking beat detection
void Lamps::run(uint8_t LFi, uint8_t HFi)
{
  static uint8_t led = 0;

  if (freezeCounter > 0) // changes are frozen except for fadeall
  {
    freezeCounter--;
    #ifdef DEBUG_SERIAL
      Serial.print("F "); Serial.print(freezeCounter);
    #endif
  }
  else // freezeCounter is zero (expired)
  {
    int8_t diff = HFi - LFi;
    #ifdef DEBUG_SERIAL
      if (diff < 0) Serial.print("D: "); 
      else Serial.print("D:  ");
      Serial.print(diff);
    #endif
    // generate events 
    if (diff > FFT_BEAT_DIFF) 
    { 
      freezeCounter = FREEZE_TIME;
      led = random8() % NUM_LEDS/NUM_LEDS_PER_GROUP;
      FillLEDsPattern0(led, alternate1Palette);
      #ifdef DEBUG_SERIAL
      Serial.print(" *");
      #endif
    }
    else if (diff < FFT_HIGH_DIFF) {
      colorIndex += C_CHANGE;
      setupPallete();
      FillLEDsPattern0(led, alternate1Palette);
      freezeCounter = FREEZE_TIME;
      #ifdef DEBUG_SERIAL
      Serial.print(" -");
      #endif
    } else  if (diff < FFT_HIGH_DIFF-1) {
      freezeCounter = FREEZE_TIME;
      // generate gliches, sometimes
      if (!(random8()%4)) FillLEDsPattern1(led, alternate2Palette);
      else FillLEDsPattern1(led, alternate1Palette);
      #ifdef DEBUG_SERIAL
      Serial.print(" --");
      #endif
    } else {
      colorIndex++;
    }
    
  }
  fadeall();
  FastLED.show();

  #ifdef DEBUG_SERIAL
    Serial.println();
  #endif
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
  //fadeall();
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