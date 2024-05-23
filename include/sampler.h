#ifndef SAMPLER_H
#define SAMPLER_H

#include <Arduino.h>
#include "defines.h"

// let's write a class to handle stuff
struct sampler
{
  private:
  uint8_t  pointer;
  bool     bufferReady;
  
  public:
  int8_t   buffer[NSAMPLES];
  sampler();
  void pushSample(int8_t sample);
  void clearBuffer(void);
  //void processBuffer(void);
  void printBuffer(void);
  bool isBufferReady(void);
  void clearBufferReady(void);
};

struct easyFFT
{
  /*
  example
    FFT(data,64,100);        //to  get top five value of frequencies of X having 64 sample at 100Hz sampling
    Serial.println(f_peaks[0]);
    Serial.println(f_peaks[1]);
    delay(99999);
  after ruing above FFT(), frequencies available at f_peaks[0],f_peaks[1],f_peaks[2],f_peaks[3],f_peaks[4],
  */           
     
  private:
  //---------------------------------------------------------------------------//
  byte  sine_data [91]=
  {
  0,  
  4,    9,    13,   18,   22,   27,   31,   35,   40,   44, 
  49,   53,   57,   62,   66,   70,   75,   79,   83,   87, 
  91,   96,   100,  104,  108,  112,  116,  120,  124,  127,  
  131,  135,  139,  143,  146,  150,  153,  157,  160,  164,  
  167,  171,  174,  177,  180,  183,  186,  189,  192,  195,       //Paste this at top of program
  198,  201,  204,  206,  209,  211,  214,  216,  219,  221,  
  223,  225,  227,  229,  231,  233,  235,  236,  238,  240,  
  241,  243,  244,  245,  246,  247,  248,  249,  250,  251,  
  252,  253,  253,  254,  254,  254,  255,  255,  255,  255
    };
  
  //---------------------------------------------------------------------------//

  #define NUM_PEAKS 5

  public:
  float f_peaks[NUM_PEAKS];  // top 5 frequencies peaks in descending order
  easyFFT();
  float FFT(int8_t in[],uint8_t N,float Frequency);
  float sine(int i);
  float cosine(int i);

};


#endif