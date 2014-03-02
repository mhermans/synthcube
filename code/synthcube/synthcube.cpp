// Do not remove the include below
#include "synthcube.h"
 /* Example using waveshaping to modify the spectrum of an audio signal
using Mozzi sonification library.
Demonstrates the use of WaveShaper(), EventDelay(), Smooth(),
rand(), and fixed-point numbers.
Circuit: Audio output on digital pin 9 (on a Uno or similar), or
check the README or http://sensorium.github.com/Mozzi/
Mozzi help/discussion/announcements:
https://groups.google.com/forum/#!forum/mozzi-users
Tim Barrass 2012.
This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <WaveShaper.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Smooth.h>
#include <tables/sin8192_int8.h>
#include <tables/waveshape_chebyshev_3rd_256_int8.h>
#include <tables/waveshape_chebyshev_6th_256_int8.h>
#include <tables/waveshape_compress_512_to_488_int16.h>


// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aSin(SIN8192_DATA); // sine wave sound source
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aGain1(SIN8192_DATA); // to fade sine wave in and out before waveshaping
//Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aGain2(SIN8192_DATA); // to fade sine wave in and out before waveshaping

// Chebyshev polynomial curves, The nth curve produces the n+2th harmonic component.
WaveShaper <char> aCheby3rd(CHEBYSHEV_3RD_256_DATA); // 5th harmonic
WaveShaper <char> aCheby6th(CHEBYSHEV_6TH_256_DATA); // 8th harmonic
WaveShaper <int> aCompress(WAVESHAPE_COMPRESS_512_TO_488_DATA); // to compress instead of dividing by 2 after adding signals

// for scheduling note changes
EventDelay kChangeNoteDelay;

// for random notes

Q8n0 octave_start_note = 28;

// Q24n8 carrier_freq; // unsigned long with 24 integer bits and 8 fractional bits

// smooth transitions between notes
Smooth <unsigned int> kSmoothFreq(0.85f);
int target_freq, smoothed_freq;

#define INPUT_PIN1 0

void setup(){
  Serial.begin(9600);
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
  randSeed(); // reseed the random generator for different results each time the sketch runs
  aSin.setFreq(octave_start_note); // set the frequency
  aGain1.setFreq(2.f); // use a float for low frequencies, in setup it doesn't need to be fast
//  aGain2.setFreq(.4f);
   kChangeNoteDelay.set(2000); // note duration ms, within resolution of CONTROL_RATE
}

int buffer[20];
int32_t lastInput = 0;

void debugOutput(int diff);

void updateControl() {

  //int input1 = mozziAnalogRead(INPUT_PIN1);

  int inputAccelerometer = mozziAnalogRead(INPUT_PIN1)
  int mappedInput = map(inputAccelerometer, 0, 1024, 0, 100);
  int inputDiff = abs(lastInput - mappedInput);

  //int freq = aCheby3rd.next(aSin.next());
  //char awaveshaped1 = aCheby3rd(aSin.next());
  lastInput = mappedInput;

 // octave_start_note = 36;
//  if (inputDiff > 0) {
//	//octave_start_note = (rand((byte)3)*2)+36; // ranges between 36 and 84
//  } else

   if ((inputDiff  < 2)) {
	kChangeNoteDelay.set(6000); // note duration ms, within resolution of CONTROL_RATE)
   } else if ((inputDiff > 4) &&  (inputDiff < 6)) {

//	  float freq = (float) (rand((byte)20)/10);
	//aGain2.setFreq(freq);
	kChangeNoteDelay.set(6000);//kChangeNoteDelay.set(rand((byte) 2000) + 1000);
	aSin.setFreq(octave_start_note*2); // sine wave source/2); // then scale it back down after it's smoothed
   } else if (inputDiff > 6) {
	   kChangeNoteDelay.set(6000);//kChangeNoteDelay.set(rand((byte) 2000) + 1000);
	   	aSin.setFreq(octave_start_note*3)

	   //aSin.next = aSin.next() + awaveshaped1;
   }

//  } else if (inputDiff > 3) {
//	  float freq = (float) (rand((byte)20)/10);
//       aGain1.setFreq(freq);

  debugOutput(inputDiff);

//  if(kChangeNoteDelay.ready()){
////      Q16n16 midi_note = Q8n0_to_Q16n16(octave_start_note);
////      target_freq = Q16n16_to_Q16n0(Q16n16_mtof(midi_note)); // has to be 16 bits for Smooth
//      kChangeNoteDelay.start();
// }
  smoothed_freq = kSmoothFreq.next(octave_start_note*2); // temporarily scale up target_freq to get better int smoothing at low values

   }

void debugOutput(int diff) {
	int pktSize;
	buffer[0] = 0xCDAB;             //SimPlot packet header. Indicates start of data packet
	buffer[1] = 4*sizeof(int);      //Size of data in bytes. Does not include the header and size fields
	buffer[2] = diff;

	pktSize = 2 + 2 + (4*sizeof(int)); //Header bytes + size field bytes + data
	//IMPORTANT: Change to serial port that is connected to PC
	Serial.write((uint8_t * )buffer, pktSize);
}


//int updateAudio(){
//  char asig0 = aSin.next(); // sine wave source
//  // make 2 signals fading in and out to show effect of amplitude when waveshaping with Chebyshev polynomial curves
//  // offset the signals by 128 to fit in the 0-255 range for the waveshaping table lookups
//  unsigned char asig1 = (unsigned char)128+((asig0*((unsigned char)128+aGain1.next()))>>8);
////  unsigned char asig2 = (unsigned char)128+((asig0*((unsigned char)128+aGain2.next()))>>8);
//  // get the waveshaped signals
//  char awaveshaped1 = aCheby3rd.next(asig0);
////  char awaveshaped2 = aCheby6th.next(asig2);
//  // use a waveshaping table to squeeze 2 summed 8 bit signals into the range -244 to 243
//
//  int awaveshaped3 = aCompress.next(256u + awaveshaped1);
//  return awaveshaped3;
//}


void loop(){
  audioHook(); // required here
}




