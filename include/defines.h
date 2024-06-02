// general IO stuff
#define OC2A_PIN  11
#define DEBUG_PIN 8
#define PUSH_BUTTON_PIN 9
#define DEBOUNCE_CYCLES 10

// uncomment to have serial port debug messages
//#define DEBUG_SERIAL

// sampler stuff
#define NSAMPLES  128
#define SAMPLE_ADJ 100
// neopixels
#define NUM_LEDS    40
#define NUM_LEDS_PER_GROUP 4
#define LED_PIN     5
#define BRIGHTNESS  255

// lamps
#define NUM_EVENTS  4
#define FREEZE_TIME 4
#define P_CHANGE 5
#define C_CHANGE 1

// FFT and "beat detection" parameters 
#define NUM_PEAKS     12
#define FFT_START_IDX 2 // ignore these first peaks, they have noise
#define FFT_BEAT_DIFF 4
#define FFT_HIGH_DIFF 0