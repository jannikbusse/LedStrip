#pragma once

struct speed_struct
{
    float speed = 0;
};

struct offset_struct
{
    float offset_add = 0;

};

struct brightness_struct
{
    float brightness = 0;

};

struct color_struct
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

struct timer_struct
{
    unsigned long timedelta = 0; 
};

struct music_struct
{
    float amp = 0;
};

struct Color
{
  double r, g, b;
  double h, s, v;
  Color(double _r,double _g,double _b)
  {
    r=r;
    b=b;
    g=g;
  }

  void cpy(const Color &_c)
  {
    r = _c.r;
    g = _c.g;
    b = _c.b;
    h = _c.h;
    s = _c.s;
    v = _c.v;
  }
  /**
   * Normalizes brightness of rgb component
  */
  void normalize_to_brightness(double brightness)
  {
    double sum = r + g + b;
    r = min(1, r/sum * brightness);
    g = min(1, g/sum * brightness);
    b = min(1, b/sum * brightness);
  }
};