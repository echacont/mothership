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

#include "lamps.h"
Lamps lmps0;

void configure()
{
  // configure OC2A digital pin as output
  pinMode(OC2A_PIN, OUTPUT);
  // configure debug digital pin as output
  pinMode(DEBUG_PIN, OUTPUT);
  digitalWrite(DEBUG_PIN, HIGH);
  configureADC();
  configureTC2();
  lmps0.configure();

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

void disableTC2int() { TIMSK2 = 0; }

void enableTC2int() { TIMSK2 = 0x02; }

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
  if(!smplr0.isBufferReady()) enableTC2int();
}

// start ADC conversion
ISR (TIMER2_COMPA_vect) { ADCSRA |= 0x40; } 

void setup()  { configure(); }

void loop() 
{
  if (smplr0.isBufferReady()) 
  {
    //unsigned long time = millis();
    // 4000 Hz was measured using instrumentation on DEBUG_PIN
    fft0.FFT(smplr0.buffer, NSAMPLES, 4000.0);
    uint8_t LFi = getLowFreqIndex(fft0.f_peaks);
    uint8_t HFi = getHighFreqIndex(fft0.f_peaks);

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
    lmps0.run(LFi, HFi);

    // debug
    { 
      static bool pin = 0;
      pin = !pin;
      digitalWrite(DEBUG_PIN, pin);
    }


    // clear bufferReady flag to enabling sampling
    // resets buffer pointer
    smplr0.clearBufferReady();
    // Enable timer 2 interrupts to resume sampling
    enableTC2int();
  }

}