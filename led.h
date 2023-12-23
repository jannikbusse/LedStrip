//delete this line. this line was just to test editing
#pragma once

#include <FastLED.h>
#include "music.h"
#include "api.h"

#define NUM_LEDS          53
#define LED_PIN           7
#define MAX_COLORS        10
#define MODE_RGB          1
#define MODE_HSV          2


bool _enable_flickering = true;
bool _enabled = true;

long flicker_timer = 0;
long flicker_delay = 1700; //ms
long flicker_offset = 0;

unsigned long timer_activation_time = 0;

double flicker_speed = 2;
double _max_brightness = 100;
double offset = 0;
bool timer_set = false;
int curr_colors = 3;

int mode = MODE_HSV;

CRGB leds[NUM_LEDS];
CHSV ledsHSV[NUM_LEDS];

Color cBuf(0,0,0);
Color* cols;



double dclamp(double d1)
{
  return max(0, min(1, d1));
}

template<typename T, typename U>
constexpr double dmod (T x, U mod)
{
    return !mod ? x : x - mod * static_cast<long long>(x / mod);
}



void linear_interpolate_color_RGB(double scalar, const Color &c1,const  Color &c2, Color &res)
{
  //scalar between 0 and 1
  res.r = c2.r * scalar + c1.r * (1 - scalar);
  res.g = c2.g * scalar + c1.g * (1 - scalar);
  res.b = c2.b * scalar + c1.b * (1 - scalar);

}

void set_timer(unsigned long milliseconds)
{
  timer_set = true;
  timer_activation_time = millis() + milliseconds;
}

//untested
void interpolate_color_HSV(double scalar, const Color &c1, const Color &c2, Color &res)
{
  if(abs(c2.h - c1.h) < 0.5)
  {
    res.h = c2.h * scalar + c1.h * (1 - scalar);

  }else if(c2.h > c1.h)
  {
    res.h = c2.h * scalar + (c1.h + 1.0) * (1 - scalar);
    res.h = dmod(res.h, 1.0);

 
  }
  else
  {
    res.h = (c2.h+ 1.0) * scalar + (c1.r ) * (1 - scalar);
    res.h = dmod(res.h, 1.0);

  }
  res.s = c2.s * scalar + c1.s * (1 - scalar);
  res.v = c2.v * scalar + c1.v * (1 - scalar);

}

void offset_to_color(const Color *colorsp, int colors_size, double offset, Color &res, void (*interpolation_func)(double, const Color&, const Color&, Color&))
{
  //offset between 0 and 100
  offset = dmod(offset, 100);

  if(colors_size <= 0)
  {
    res.r = 0;
    res.g = 0;
    res.b = 1;
    res.h = 0;
    res.s = 1;
    res.v = 1;
    return;
  }

  double stepSize = 100.0/(double)(colors_size);
  int adjOffset = offset/ stepSize;

  if(adjOffset == colors_size-1)
  {
    double scalar = (dmod(offset, stepSize)) /(double)stepSize;
    (*interpolation_func)(scalar, colorsp[adjOffset], colorsp[0], res);
    
  }
  else
  {
    double scalar = (dmod(offset, stepSize)) /(double)stepSize;
    (*interpolation_func)(scalar, colorsp[adjOffset], colorsp[adjOffset + 1], res);

  }
}



void offset_to_rgb(double offset, Color &c)
{
  //offset between 0 and 100
 

  if(offset < 33.3)
  {
    double clamped = offset / 33.3;
    c.b = 0;
    c.g =  clamped ;
    c.r = (1 - clamped) ;
  }

  else if(offset < 66.6)
  {
    double clamped = (offset - 33.3)/ 33.3;
    c.r = 0;
    c.b = clamped;
    c.g = (1 - clamped);
  }

  else
  {
    double clamped = (offset - 66.6)/ 33.3;
    c.g = 0;
    c.r = clamped;
    c.b = 1 - clamped;
  }

}

void set_brightness(double b)
{
  _max_brightness = b;
}

void apply_noise_modulation(int led, Color &c)
{
  if(millis() > flicker_timer)
  {
    offset += (double)random(0,4)-2.0;
    flicker_offset += random(50,300);
    flicker_timer = millis() + random(60, flicker_delay);
  }

  double scale = 20;
  
  //double flicker_intensity = (double)inoise8(led * scale + offset * offsetscale, millis() + flicker_offset) / (255.0) + 0.5;
  double flicker_intensity = ((double)inoise8(led * scale + flicker_speed, millis()/5 + flicker_offset) / (255.0)); //between 0 an 1
  flicker_intensity = flicker_intensity * 1.0 + 0.5;
  c.r = dclamp(c.r * flicker_intensity);
  c.g = dclamp(c.g * flicker_intensity);
  c.b = dclamp(c.b * flicker_intensity);

}

void set_led_to_color(int led, Color &c)
{
  leds[led].r = c.r * _max_brightness;
  leds[led].g = c.g * _max_brightness;
  leds[led].b = c.b * _max_brightness;
}
void set_led_to_colorHSV(int led, Color &c)
{
  leds[led] = CHSV(c.h * 255, c.s * 255, c.v *_max_brightness);
}

void clear_colors()
{
  curr_colors = 0;
  for(int i = 0; i < MAX_COLORS; i ++)
  {
    cols[i].r = 1;
    cols[i].g = 0;
    cols[i].b = 0;
  }
}

void add_color(double r, double g, double b, double h, double s, double v)
{
  if(curr_colors >= MAX_COLORS)
  {
    return;
  }

  curr_colors ++;
  cols[curr_colors-1].r = r;
  cols[curr_colors-1].g = g;
  cols[curr_colors-1].b = b;
  cols[curr_colors-1].h = h;
  cols[curr_colors-1].s = s;
  cols[curr_colors-1].v = v;
}

void activate_timer()
{
  timer_set = false;
  for(int a = 0; a < 30; a ++)
  {
    for(int i = 0; i < NUM_LEDS; i ++)
    {
      leds[i] = CRGB(255, 255, 255);

    }
    FastLED.show(); 
    delay(200);
    for(int i = 0; i < NUM_LEDS; i ++)
    {
      leds[i] = CRGB(0, 0, 0);

    }
    FastLED.show(); 
    delay(150);

  }
}

void update_leds()
{
  offset = dmod(offset, 100.0);

  if(timer_set && millis() > timer_activation_time)
  {
    activate_timer();
  }

  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(250,255,0);
  }
  FastLED.show();
  return;

  if(!_enabled || music_is_active())
  {
    return;
  }
  if(mode == MODE_RGB)
  {
    for(int i = 0; i < NUM_LEDS; i ++)
    {
      offset_to_color(cols, curr_colors, i + offset, cBuf, linear_interpolate_color_RGB);
      //cBuf.normalize_to_brightness(1.1);
      if(_enable_flickering)
        apply_noise_modulation(i, cBuf);
      set_led_to_color(i, cBuf);
    } 
  }
  else if(mode == MODE_HSV)
  {
    for(int i = 0; i < NUM_LEDS; i ++)
    {
      offset_to_color(cols, curr_colors, i + offset, cBuf, interpolate_color_HSV);
      //cBuf.normalize_to_brightness(1.1);
      set_led_to_colorHSV(i , cBuf);
      //leds[i] = CHSV(i*5, 255, _max_brightness);

    }
    
  }


      

  FastLED.show();
}

void enable()
{
  _enabled = true;
  offset = 0;
}

void disable()
{
  _enabled = false;

  cBuf.r = 0;
  cBuf.g = 0;
  cBuf.b = 0;

  for(int i = 0; i < NUM_LEDS; i ++)
  {
    set_led_to_color(i, cBuf);
  }  
  FastLED.show();
}

void update_music(double amp)
{
  if(_enabled)
    update_vol(amp);
}

void init_LEDS(double max_brightness)
{
  _max_brightness = max_brightness;

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  init_music(NUM_LEDS, leds);
  delay(100);
  FastLED.setBrightness(255);
  if(mode == MODE_RGB)
  {
    for(int i = 0; i < NUM_LEDS; i ++)
    {
      leds[i] = CRGB(255,0,0);

    }
  }  
  if(mode == MODE_HSV)
  {
    for(int i = 0; i < NUM_LEDS; i ++)
    {
      leds[i] = CHSV(128,255,255);

    }
  }  
  delay(100);
  FastLED.show();
  cols = (Color *) malloc(sizeof(Color) * MAX_COLORS); 
  cols[0].r = 1;
  cols[0].g = 0;
  cols[0].b = 0;
  cols[0].h  = 0;
  cols[0].s  = 1;
  cols[0].v  = 1;

  cols[1].r = 0;
  cols[1].g = 1;
  cols[1].b = 0;
  cols[1].h  = 0.5;
  cols[1].s  = 1;
  cols[1].v  = 1;

  cols[2].r = 0;
  cols[2].g = 0;
  cols[2].b = 1;
  cols[2].h  = 1;
  cols[2].s  = 1;
  cols[2].v  = 1;
  curr_colors = 3;
 
}
