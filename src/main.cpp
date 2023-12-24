#include "arduinoFFT.h"
#include "led.h"
#include "api.h"
#define MAX_BRIGHTNESS    222.0

#define CMD_FLICKER       'F'
#define CMD_TURN_ON       'N'
#define CMD_TURN_SWITCH   'S'
#define CMD_MOVE_OFFSET   'M'
#define CMD_BRIGHTNESS    'B'
#define CMD_CLEAR_COLS    'C'
#define CMD_ADD_COLOR     'A'
#define CMD_SET_SPEED     'O'
#define CMD_SET_TIMER     'T'
#define CMD_MUSIC         'V'

double offset_speed = 0.1;

bool cleared = false;

void setup() 
{
  Serial.begin(9600);
  delay(100);
  init_LEDS(MAX_BRIGHTNESS);
  while(Serial.available())
    Serial.read();
}
void cmd_switch()
{
  if(_enabled)
  {
    disable();
  }else
  {
    enable();
  }
}

void cmd_flicker()
{
    _enable_flickering = !_enable_flickering;
}

void handle_serial()
{
  size_t bytesRead = 0;
  char cmd = "";
  int av = Serial.available();

  
  if (av)
  {
    //clear_LEDS();
    cmd = Serial.read();
    switch(cmd)
    {
      case CMD_FLICKER:
      {
        cmd_flicker();
      }
      break;
      case CMD_TURN_SWITCH:
        cmd_switch();
      break;

      case CMD_CLEAR_COLS:
        clear_colors();
      break;

      case CMD_MOVE_OFFSET:
      {
        offset_struct os;
        bytesRead = Serial.readBytes((char *)&os, sizeof(struct offset_struct));
        if(bytesRead == sizeof(struct offset_struct))
        {
          offset += (double)os.offset_add;
        }
      }
      break;

      case CMD_MUSIC:
      {
        music_struct os;
        bytesRead = Serial.readBytes((char *)&os, sizeof(struct offset_struct));
        if(bytesRead == sizeof(struct offset_struct))
        {
           update_music((double)os.amp);
           Serial.print("Read value:");
           Serial.println(os.amp);
        }
      }
      break;

      case CMD_SET_TIMER:
      {
        timer_struct os;
        bytesRead = Serial.readBytes((char *)&os, sizeof(struct timer_struct));
        if(bytesRead == sizeof(struct timer_struct))
        {
          set_timer(os.timedelta * 1000);
        }
      }
        break;

      case CMD_BRIGHTNESS:
      {
	      brightness_struct bs;
        bytesRead = Serial.readBytes((char *)&bs, sizeof(struct brightness_struct));
        if(bytesRead == sizeof(struct brightness_struct))
        {
          set_brightness((double)bs.brightness);
          
        }
      }
      break;

      case CMD_SET_SPEED:
      {
        speed_struct ss;
        bytesRead = Serial.readBytes((char *)&ss, sizeof(struct speed_struct));
        if(bytesRead == sizeof(struct speed_struct))
        {
          offset_speed = ss.speed;

        }
      }
      break;
      case CMD_ADD_COLOR:
      {
	      color_struct cs;
        bytesRead = Serial.readBytes((char *)&cs, sizeof(struct color_struct));
	      if(bytesRead == sizeof(struct color_struct))
        {
          double r = (double)cs.r/255.0;
          double g = (double)cs.g/255.0;
          double b = (double)cs.b/255.0;
        
          CHSV hsv;
          hsv = rgb2hsv_approximate(CRGB(cs.r, cs.g, cs.b));
          add_color(r, g, b, (double)hsv.h/255.0,  (double)hsv.s/255.0, (double)hsv.v/255.0);
        }
      }
      break;

      
      default:
        while(Serial.available())
          Serial.read();
      break;
    }
  
  }
}

void loop()
{

  handle_serial();
  offset += offset_speed;
  //update_leds();
}
