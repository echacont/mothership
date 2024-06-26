#include <Arduino.h>
#include "defines.h"
#include "sampler.h"

sampler::sampler()
{
  time = 0;
  pointer = 0;
  bufferReady = false;
  for (uint8_t i = 0; i<NSAMPLES; i++)
    buffer[i] = 0;
}

void sampler::printBuffer(void)
{
  {
    Serial.println("sampler: buffer data dump");
    for (uint8_t i = 0; i<NSAMPLES; i++) 
    {
      Serial.print(buffer[i]);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
}

void sampler::pushSample(int8_t sample)
{
  if (!bufferReady)
  {
    buffer[pointer] = sample;
    pointer++;
    if (pointer > NSAMPLES-1) 
    {
      pointer = 0;
      bufferReady = true;
    }
  }
}

void sampler::clearBuffer()
{
  for (uint8_t i = 0; i<NSAMPLES; i++) buffer[i] = 0;
  pointer = 0;
  bufferReady = false;
}

/*
void sampler::processBuffer()
{
    // calculate average to offset
    //Serial.println("sampler: buffer processing:");
    int16_t avg = 0;
    for (uint8_t i = 0; i<NSAMPLES; i++) 
    {
      avg += buffer[i];
      //Serial.print(avg); Serial.print(" ");
    }
    avg = avg/(NSAMPLES);
    //Serial.print("\navg: "); Serial.println(avg);
    // apply average to offset signal
    //Serial.println("sampler: writing proc samples");
    for (uint8_t i = 0; i<NSAMPLES; i++)  
    {
      int16_t diff = buffer[i] - avg;
      //Serial.print(diff, DEC); Serial.print(" -> ");
      buffer[i] = diff;
      //Serial.print(buffer[i], DEC); Serial.print(" ");
    }
    //Serial.print("\n");
}
*/

bool sampler::isBufferReady(void)
{ return bufferReady; }

void sampler::clearBufferReady(void)
{ 
  bufferReady = false; 
  pointer = 0;
}

// easy FFT 
// https://projecthub.arduino.cc/abhilashpatel121/easyfft-fast-fourier-transform-fft-for-arduino-03724d

//-----------------------------FFT Function----------------------------------------------//

easyFFT::easyFFT() { }

void easyFFT::FFT(int8_t in[],uint8_t N,float Frequency)
{
/*
Code to perform FFT on arduino,
setup:
paste  sine_data [91] at top of program [global variable], paste FFT function at end of  program
Term:
1. in[]     : Data array, 
2. N        : Number of sample  (recommended sample size 2,4,8,16,32,64,128...)
3. Frequency: sampling frequency  required as input (Hz)

If sample size is not in power of 2 it will be clipped  to lower side of number. 
i.e, for 150 number of samples, code will consider  first 128 sample, remaining sample  will be omitted.
For Arduino nano, FFT of  more than 128 sample not possible due to mamory limitation (64 recomended)
For  higher Number of sample may arise Mamory related issue,
Code by ABHILASH
Contact:  abhilashpatel121@gmail.com 
Documentation:https://www.instructables.com/member/abhilash_patel/instructables/
2/3/2021:  change data type of N from float to int for >=256 samples
*/

unsigned  int data[13]={1,2,4,8,16,32,64,128,256,512,1024,2048};
int c1,f,o,x;
uint8_t a;
a=N;  
                                 
      for(int8_t i=0;i<12;i++)                 //calculating  the levels
         { if(data[i]<=a){o=i;} }
      
int in_ps[data[o]]={};     //input for sequencing
float out_r[data[o]]={};   //real part of transform
float  out_im[data[o]]={};  //imaginory part of transform
           
x=0;  
      for(int b=0;b<o;b++)                     // bit reversal
         {
          c1=data[b];
          f=data[o]/(c1+c1);
                for(int  j=0;j<c1;j++)
                    { 
                     x=x+1;
                     in_ps[x]=in_ps[j]+f;
                    }
         }

 
      for(uint8_t i=0;i<data[o];i++)            // update input array as per bit reverse order
         {
          if(in_ps[i]<a)
          {out_r[i]=in[in_ps[i]];}
          if(in_ps[i]>a)
          {out_r[i]=in[in_ps[i]-a];}      
         }


int i10,i11,n1;
float e,c,s,tr,ti;

    for(int  i=0;i<o;i++)                                    //fft
    {
     i10=data[i];              // overall values of sine/cosine  :
     i11=data[o]/data[i+1];    // loop with similar sine cosine:
     e=360/data[i+1];
     e=0-e;
     n1=0;

          for(int j=0;j<i10;j++)
          {
          c=cosine(e*j);
          s=sine(e*j);    
          n1=j;
          
                for(int  k=0;k<i11;k++)
                 {
                 tr=c*out_r[i10+n1]-s*out_im[i10+n1];
                 ti=s*out_r[i10+n1]+c*out_im[i10+n1];
          
                 out_r[n1+i10]=out_r[n1]-tr;
                 out_r[n1]=out_r[n1]+tr;
          
                 out_im[n1+i10]=out_im[n1]-ti;
                 out_im[n1]=out_im[n1]+ti;          
          
                 n1=n1+i10+i10;
                  }       
             }
     }

/*
for(int i=0;i<data[o];i++)
{
Serial.print(out_r[i]);
Serial.print("\	");                                     // un comment to print RAW o/p    
Serial.print(out_im[i]);  Serial.println("i");      
}
*/


//---> here onward out_r contains  amplitude and our_in conntains frequency (Hz)
    for(uint8_t i=0;i<data[o-1];i++)               // getting amplitude from compex number
        {
         out_r[i]=sqrt(out_r[i]*out_r[i]+out_im[i]*out_im[i]);  // to  increase the speed delete sqrt
         out_im[i]=i*Frequency/N;
         /*
         Serial.print(out_im[i]); Serial.print("Hz");
         Serial.print("\	");                            // un comment to print freuency bin    
         Serial.println(out_r[i]);  
         */    
        }




x=0;       // peak detection
   for(uint8_t i=1;i<data[o-1]-1;i++)
      {
      if(out_r[i]>out_r[i-1] &&  out_r[i]>out_r[i+1]) 
      {in_ps[x]=i;    //in_ps array used for storage of  peak number
      x=x+1;}    
      }


s=0;
c=0;
    for(int  i=0;i<x;i++)             // re arraange as per magnitude
    {
        for(int  j=c;j<x;j++)
        {
            if(out_r[in_ps[i]]<out_r[in_ps[j]]) 
                {s=in_ps[i];
                in_ps[i]=in_ps[j];
                in_ps[j]=s;}
        }
    c=c+1;
    }



    for(int i=0;i<NUM_PEAKS;i++)     //  updating f_peak array (global variable)with descending order
    {
    f_peaks[i]=out_im[in_ps[i]];
    }



}
    
float easyFFT::sine(int i)
{
  int j=i;
  float  out;
  while(j<0){j=j+360;}
  while(j>360){j=j-360;}
  if(j>-1   && j<91){out=  sine_data[j];}
  else if(j>90  && j<181){out= sine_data[180-j];}
  else if(j>180  && j<271){out= -sine_data[j-180];}
  else if(j>270 && j<361){out= -sine_data[360-j];}
  return (out/255);
}

float easyFFT::cosine(int i)
{
  int j=i;
  float  out;
  while(j<0){j=j+360;}
  while(j>360){j=j-360;}
  if(j>-1   && j<91){out=  sine_data[90-j];}
  else if(j>90  && j<181){out= -sine_data[j-90];}
  else  if(j>180 && j<271){out= -sine_data[270-j];}
  else if(j>270 && j<361){out= sine_data[j-270];}
  return (out/255);
}
// end of easyFFT
//------------------------------------------------------------------------------------//
 
// aditional methods to get insight from f_peaks

void easyFFT::getFreqIndexes(void)
{
  uint8_t lowFreqIndex = 0;
  int16_t lowFreq = 2000; // max possible freq given sample rate
  uint8_t highFreqIndex = 0;
  int16_t highFreq = 0;
  for (uint8_t i = FFT_START_IDX; i<NUM_PEAKS; i++)
  {
    if (f_peaks[i] < lowFreq)
    {
      lowFreq = f_peaks[i];
      lowFreqIndex = i;
    }
    if (f_peaks[i] > highFreq)
    {
      highFreq = f_peaks[i];
      highFreqIndex = i;
    }
  }
  LFi = lowFreqIndex;
  HFi = highFreqIndex;
  
}