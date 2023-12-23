#pragma once
#include <FastLED.h>
#include <api.h>
#include "arduinoFFT.h"

#define TIMEOUT 500

int MAX_LEDS = 0;

unsigned long last_update = 0;
double currvol = 0;
double rolavgvol = 0;
double rolavgweight = 0.62; // weight of new value
CRGB* led_ptr;

//-------------------------------------------------//
//*******************FFT values********************//
//-------------------------------------------------//

arduinoFFT FFT;

#define SCL_FREQUENCY 0x02
const uint16_t samples          = 256;
const double signalFrequency    = 1000;
const double samplingFrequency  = 1000;

int bufPtr = 0;

double vReal[samples];
double vImag[samples];

//-------------------------------------------------//

void build_data()
{

}



void apply_fft()
{
    FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency); /* Create FFT object */
    /* Print the results of the simulated sampling according to time */

    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */

    FFT.Compute(FFT_FORWARD); /* Compute FFT */

    FFT.ComplexToMagnitude(); /* Compute magnitudes */
    double x = FFT.MajorPeak();
}

void init_music(int max_leds, CRGB *_leds)
{
    MAX_LEDS = max_leds;
    led_ptr = _leds;
}

bool music_is_active()
{
    return millis() - last_update < TIMEOUT;
}

void apply_fft_update()//gets called when buffers are filled
{
    unsigned long start = millis();
    apply_fft();
    Serial.print("time: ");
    Serial.println(millis() - start);

    double low_sum = 0;

    for(int i = 0; i < 5 ; i ++)
    {
        low_sum += vReal[i];
    }

    for(int i = 0; i < MAX_LEDS; i ++)
    {
      led_ptr[i] = CRGB((int)low_sum, (int)low_sum, (int)low_sum);

    }
    FastLED.show(); 

}

void update_vol(double vol)
{


    currvol = vol;
    last_update = millis();
    //currvol = min(100, max(0, vol));
    rolavgvol = rolavgvol * (1- rolavgweight) + rolavgweight * currvol;


    //fill buffer
    vReal[bufPtr] =  rolavgvol;
    vImag[bufPtr] = 0; 
    bufPtr ++;
    if(bufPtr >= samples)
    {   
        apply_fft_update();
        bufPtr = 0;
    }
    //-----------

   


}
