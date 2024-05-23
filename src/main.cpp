#include <Arduino.h>
#include "defines.h"

ISR (TIMER2_COMPA_vect);
ISR (ADC_vect);

void configureTC2(void);
void enableTC2int(void);
void disableTC2int(void);
void configureADC(void);
void configure(void);

#include "sampler.h"
sampler smplr0;
easyFFT fft0;

uint8_t getLowFreqIndex(float f_peaks[]);
uint8_t getHighFreqIndex(float f_peaks[]);

// cochinadilla de FastLED
#include <FastLED.h>

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(220); } }

void FillLEDsStrip(uint8_t colorIndex)
{
    for( uint8_t i = 0; i < NUM_LEDS_STRIP; ++i) 
    {
          leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
          colorIndex += 1;
    }
}

void FillLEDsPattern0(uint8_t colorIndex, uint8_t rndm )
{
    for( uint8_t i = 0; i < NUM_LEDS/NUM_LEDS_PER_GROUP; ++i)
      if ((rndm % 4) == i)
        for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
          leds[4*i+j] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
    fadeall();
}

void FillLEDsPallete1(uint8_t colorIndex, uint8_t led)
{
  for( uint8_t i = 0; i < NUM_LEDS/NUM_LEDS_PER_GROUP; ++i)
      if ((led % (NUM_LEDS/NUM_LEDS_PER_GROUP)) == i)
        for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
          leds[4*i+j] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
      //else
      //  for (uint8_t j = 0; j < NUM_LEDS_PER_GROUP; j++)
      //    leds[4*i+j] = CRGB::Black;
  fadeall();
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
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

void SetupYellowAndBluePalette()
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

CRGB getRandom(CRGB hue1, CRGB hue2)
{
  CRGB black  = CRGB::Black;
  uint8_t rndm = random8() % 3;
  if (rndm == 0)
    return hue2;
  else if (rndm == 1)
    return hue1;
  else return black;
}

void SetupMonochomePallete(CRGB hue1, CRGB hue2)
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

void configure()
{
  // configure OC2A digital pin as output
  pinMode(OC2A_PIN, OUTPUT);
  // configure debug digital pin as output
  pinMode(DEBUG_PIN, OUTPUT);
  digitalWrite(DEBUG_PIN, HIGH);
  configureADC();
  configureTC2();

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
  //Serial.begin(9600);
  //Serial.println("sampler: configuration done");
}


uint8_t getLowFreqIndex(float f_peaks[])
{
  uint8_t lowFreqIndex = 0;
  int16_t lowFreq = 2000; // max possible freq given sample rate
  for (uint8_t i = 0; i<NUM_PEAKS; i++)
  {
    if (f_peaks[i] < lowFreq)
    {
      lowFreq = f_peaks[i];
      lowFreqIndex = i;
    }
  }
  return lowFreqIndex;
}

uint8_t getHighFreqIndex(float f_peaks[])
{
  uint8_t highFreqIndex = 0;
  int16_t highFreq = 0;
  for (uint8_t i = 0; i<NUM_PEAKS; i++)
  {
    if (f_peaks[i] > highFreq)
    {
      highFreq = f_peaks[i];
      highFreqIndex = i;
    }
  }
  return highFreqIndex;
}

void configureTC2()
{
  // configure Timer/Counter 2
  // TCCR2A: toggle OC2A on compare match, WGM21:WGM20: CTC mode
  TCCR2A = 0x42;
  // TCCR2B: set WGM22 to enable OC2A, clock select to fclk/128
  TCCR2B = 0x0B;
  // OCR2A: adjusted to trigger ADC at 4 kHz
  // 4000 Hz was measured using instrumentation on DEBUG_PIN
  OCR2A = 124;
  // TIMSK2: compare match A interrupt enable
  TIMSK2 = 0x02;
}

void disableTC2int()
{
  TIMSK2 = 0;
}

void enableTC2int()
{
  TIMSK2 = 0x02;
}

void configureADC()
{
  // select internal bandgap reference AVcc, ADLAR bit, ADC0 input pin
  ADMUX = 0xE0;
  // free running
  ADCSRB = 0;
  // ADC enable, ADC prescaler (fclk/32)
  ADCSRA = 0x8F; 
}

ISR (ADC_vect)
{
  disableTC2int();
  smplr0.pushSample(int8_t(ADCH-SAMPLE_ADJ));
  if(!smplr0.isBufferReady())
     enableTC2int();
}

// start ADC conversion
ISR (TIMER2_COMPA_vect) 
{ ADCSRA |= 0x40; } 

void setup() 
{
  configure();
}


void loop() 
{
  if (smplr0.isBufferReady()) 
  {
    //unsigned long time = millis();
    // 4000 Hz was measured using instrumentation on DEBUG_PIN
    fft0.FFT(smplr0.buffer, NSAMPLES, 4000.0);
    uint8_t LFi = getLowFreqIndex(fft0.f_peaks);
    uint8_t HFi = getHighFreqIndex(fft0.f_peaks);
    static uint8_t prevLFi = 0;
    static uint8_t prevHFi = 0;

    /*{
      Serial.print(time); Serial.print(": "); 
      for (uint8_t i = 0; i<NUM_PEAKS; i++)
      {
        Serial.print(int(fft0.f_peaks[i]), DEC); Serial.print(" ");
      }
      Serial.println();
      Serial.print("sampler:  lower freq FFT index: "); Serial.println(LFi);
      Serial.print("sampler: higher freq FFT index: "); Serial.println(HFi);
    }*/
    
    // FastLED operations
    {
      static uint8_t startIndex = 0;
      static uint8_t led = 0;
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
        startIndex -= 2;
      }
      //led = random8();
      
      //if (LFi < 4) led = LFi;
      //else led = 3;
      //FillLEDsStrip(startIndex);
      FillLEDsPattern0(startIndex, led);
      //FillLEDsFromPaletteColors(startIndex);
      //FillLEDsPallete1(startIndex, led);
      FastLED.show();
    }
    // debug
    { 
      static bool pin = 0;
      pin = !pin;
      digitalWrite(DEBUG_PIN, pin);
    }
    prevHFi = HFi; prevLFi = LFi;

    // clear bufferReady flag to enabling sampling
    // resets buffer pointer
    smplr0.clearBufferReady();
    // Enable timer 2 interrupts to resume sampling
    enableTC2int();
  }

}