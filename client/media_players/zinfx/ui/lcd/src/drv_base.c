#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/errno.h>

#include "lcd.h"
#include "drv_base.h"


//////////////////////////////////////////////////////////////////////////
////////////////////// Base "class" to derive from ///////////////////////
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// init() should set up any device-specific stuff, and
// point all the function pointers.
int drv_base_init(char *device) 
{
//  printf("drv_base_init()\n");
  
  lcd.wid = 20;
  lcd.hgt = 4;

  if(!lcd.framebuf) 
    lcd.framebuf = malloc(lcd.wid * lcd.hgt);

  if(!lcd.framebuf)
    {
      drv_base_close();
      return -1;
    }
// Debugging...
//  if(lcd.framebuf) printf("Frame buffer: %i\n", (int)lcd.framebuf);

  drv_base_clear();

  lcd.cellwid = 5;
  lcd.cellhgt = 8;
  
  lcd.clear = drv_base_clear;
  lcd.string = drv_base_string;
  lcd.chr = drv_base_chr;
  lcd.vbar = drv_base_vbar;
  lcd.hbar = drv_base_hbar;
  lcd.init_num = drv_base_init_num;
  lcd.num = drv_base_num;
  
  lcd.init = drv_base_init;
  lcd.close = drv_base_close;
  lcd.flush = drv_base_flush;
  lcd.flush_box = drv_base_flush_box;
  lcd.contrast = drv_base_contrast;
  lcd.backlight = drv_base_backlight;
  lcd.set_char = drv_base_set_char;
  lcd.icon = drv_base_icon;
  lcd.init_vbar = drv_base_init_vbar;
  lcd.init_hbar = drv_base_init_hbar;
  lcd.draw_frame = drv_base_draw_frame;

  lcd.getkey = drv_base_getkey;
  
  
  return 200;  // 200 is arbitrary.  (must be 1 or more)
}

void drv_base_close() 
{
  if(lcd.framebuf != NULL) free(lcd.framebuf);

  lcd.framebuf = NULL;
}

/////////////////////////////////////////////////////////////////
// Clears the LCD screen
//
void drv_base_clear() 
{
  memset(lcd.framebuf, ' ', lcd.wid*lcd.hgt);
  
}


//////////////////////////////////////////////////////////////////
// Flushes all output to the lcd...
//
void drv_base_flush()
{
  lcd.draw_frame(lcd.framebuf);
}


/////////////////////////////////////////////////////////////////
// Prints a string on the lcd display, at position (x,y).  The
// upper-left is (1,1), and the lower right should be (20,4).
//
void drv_base_string(int x, int y, char string[]) 
{
  int i;

  x -= 1;  // Convert 1-based coords to 0-based...
  y -= 1;
  
  for(i=0; string[i]; i++)
    {
      lcd.framebuf[(y*lcd.wid) + x + i] = string[i];
    }
}

/////////////////////////////////////////////////////////////////
// Prints a character on the lcd display, at position (x,y).  The
// upper-left is (1,1), and the lower right should be (20,4).
//
void drv_base_chr(int x, int y, char c) 
{
  y--;
  x--;
  
  lcd.framebuf[(y*lcd.wid) + x] = c;
}



void drv_base_contrast(int contrast) 
{
//  printf("Contrast: %i\n", contrast);
}

void drv_base_backlight(int on)
{
  /*
  if(on)
  {
    printf("Backlight ON\n");
  }
  else
  {
    printf("Backlight OFF\n");
  }
  */
}

void drv_base_init_vbar() 
{
//  printf("Vertical bars.\n");
}

void drv_base_init_hbar() 
{
//  printf("Horizontal bars.\n");
}

void drv_base_init_num() 
{
//  printf("Big Numbers.\n");
}

void drv_base_num(int x, int num) 
{
//  printf("BigNum(%i, %i)\n", x, num);
}

void drv_base_set_char(int n, char *dat)
{
//  printf("Set Character %i\n", n);
}

/////////////////////////////////////////////////////////////////
// Draws a vertical bar; erases entire column onscreen.
//
void drv_base_vbar(int x, int len) 
{
  int y;
  for(y=lcd.hgt; y > 0 && len>0; y--)
    {
      drv_base_chr(x, y, '|');

      len -= lcd.cellhgt;
    }
  
}

/////////////////////////////////////////////////////////////////
// Draws a horizontal bar to the right.
//
void drv_base_hbar(int x, int y, int len) 
{
  for(; x<lcd.wid && len>0; x++)
    {
      drv_base_chr(x,y,'-');
      
      len -= lcd.cellwid;
    }
  
}


/////////////////////////////////////////////////////////////////
// Sets character 0 to an icon...
//
void drv_base_icon(int which, char dest)
{
//  printf("Char %i set to icon %i\n", dest, which);
}


void drv_base_flush_box(int lft, int top, int rgt, int bot)
{
  drv_base_flush();
  
}


void drv_base_draw_frame(char *dat)
{
  int i, j;

  char out[LCD_MAX_WIDTH];

  if(!dat) return;

//  printf("Frame (%ix%i): \n%s\n", lcd.wid, lcd.hgt, dat);

  for(i=0; i<lcd.wid; i++)
    {
      out[i] = '-';
    }
  out[lcd.wid] = 0;
  printf("+%s+\n", out);
  
  
  for(i=0; i<lcd.hgt; i++)
    {
      for(j=0; j<lcd.wid; j++)
	{
	  out[j] = dat[j+(i*lcd.wid)];
	}
      out[lcd.wid] = 0;
      printf("|%s|\n", out);
      
    }
  
  for(i=0; i<lcd.wid; i++)
    {
      out[i] = '-';
    }
  out[lcd.wid] = 0;
  printf("+%s+\n", out);

}


char drv_base_getkey()
{
  return 0;
}
