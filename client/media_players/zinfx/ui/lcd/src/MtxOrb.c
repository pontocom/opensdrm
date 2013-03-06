#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "lcd.h"
#include "MtxOrb.h"
#include "drv_base.h"



static int fd;

static void MtxOrb_linewrap(int on);
static void MtxOrb_autoscroll(int on);


/////////////////////////////////////////////////////////////////
// Opens com port and sets baud correctly...
//
int MtxOrb_init(char *device) 
{
  struct termios portset;

  // Set up io port correctly, and open it...
  fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY); 
  if (fd == -1) 
  {
    fprintf(stderr, "MtxOrb_init: failed (%s)\n", strerror(errno)); 
  }
  tcgetattr(fd, &portset);
  // This is necessary in Linux, but does not exist in irix.
#ifndef IRIX
  cfmakeraw(&portset);
#endif
  cfsetospeed(&portset, B19200);
  tcsetattr(fd, TCSANOW, &portset);


  // Set display-specific stuff..
  MtxOrb_linewrap(1);
  MtxOrb_autoscroll(1);


  if(!lcd.framebuf)
    {
      lcd.close();
      return -1;
    }


  // Set the functions the driver supports...

  lcd.init =       MtxOrb_init;
  lcd.close =      MtxOrb_close;
  lcd.flush_box =  MtxOrb_flush_box;
  lcd.contrast =   MtxOrb_contrast;
  lcd.backlight =  MtxOrb_backlight;
  lcd.set_char =   MtxOrb_set_char;
  lcd.icon =       MtxOrb_icon;
  lcd.init_vbar =  MtxOrb_init_vbar;
  lcd.init_hbar =  MtxOrb_init_hbar;
  lcd.init_num =   MtxOrb_init_num;
  lcd.vbar =       MtxOrb_vbar;
  lcd.hbar =       MtxOrb_hbar;
  lcd.num =        MtxOrb_num;
  lcd.draw_frame = MtxOrb_draw_frame;
  lcd.getkey =     MtxOrb_getkey;

  return fd;
}



/////////////////////////////////////////////////////////////////
// Clean-up
//
void MtxOrb_close() 
{
  close (fd); 
  drv_base_close();
}


void MtxOrb_flush_box(int lft, int top, int rgt, int bot)
{
  int y;
  char out[LCD_MAX_WIDTH];
  
  
//  printf("Flush (%i,%i)-(%i,%i)\n", lft, top, rgt, bot);

  for(y=top; y<=bot; y++)
    {
      sprintf(out, "%cG%c%c", 254, lft, y);
      write(fd, out, 4);
      write(fd, lcd.framebuf+(y*lcd.wid)+lft, rgt-lft+1);
      
    }
  
  
}


/////////////////////////////////////////////////////////////////
// Changes screen contrast (0-255; 140 seems good)
//
void MtxOrb_contrast(int contrast) 
{
  char out[4];
  sprintf(out, "%cP%c", 254, contrast);
  write(fd, out, 3);
}

/////////////////////////////////////////////////////////////////
// Sets the backlight on or off -- can be done quickly for
// an intermediate brightness...
//
void MtxOrb_backlight(int on)
{
  char out[4];
  if(on)
  {
    sprintf(out, "%cB%c", 254, 0);
    write(fd, out, 3);
  }
  else
  {
    sprintf(out, "%cF", 254);
    write(fd, out, 2);
  }
}


/////////////////////////////////////////////////////////////////
// Toggle the built-in linewrapping feature
//
static void MtxOrb_linewrap(int on)
{
  char out[4];
  if(on)
    sprintf(out, "%cC", 254);
  else
    sprintf(out, "%cD", 254);
  write(fd, out, 2);
}

/////////////////////////////////////////////////////////////////
// Toggle the built-in automatic scrolling feature
//
static void MtxOrb_autoscroll(int on)
{
  char out[4];
  if(on)
    sprintf(out, "%cQ", 254);
  else
    sprintf(out, "%cR", 254);
  write(fd, out, 2);
}


/////////////////////////////////////////////////////////////////
// Sets up for vertical bars.  Call before lcd.vbar()
//
void MtxOrb_init_vbar() 
{
  char a[] = {
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,1,1,1,1,
  };
  char b[] = {
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,1,1,1,1,
    1,1,1,1,1,
  };
  char c[] = {
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
  };
  char d[] = {
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
  };
  char e[] = {
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
  };
  char f[] = {
    0,0,0,0,0,
    0,0,0,0,0,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
  };
  char g[] = {
    0,0,0,0,0,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
  };

  lcd.set_char(1,a);
  lcd.set_char(2,b);
  lcd.set_char(3,c);
  lcd.set_char(4,d);
  lcd.set_char(5,e);
  lcd.set_char(6,f);
  lcd.set_char(7,g);
  
}

/////////////////////////////////////////////////////////////////
// Inits horizontal bars...
//
void MtxOrb_init_hbar() 
{

  char a[] = {
    1,0,0,0,0,
    1,0,0,0,0,
    1,0,0,0,0,
    1,0,0,0,0,
    1,0,0,0,0,
    1,0,0,0,0,
    1,0,0,0,0,
    1,0,0,0,0,
  };
  char b[] = {
    1,1,0,0,0,
    1,1,0,0,0,
    1,1,0,0,0,
    1,1,0,0,0,
    1,1,0,0,0,
    1,1,0,0,0,
    1,1,0,0,0,
    1,1,0,0,0,
  };
  char c[] = {
    1,1,1,0,0,
    1,1,1,0,0,
    1,1,1,0,0,
    1,1,1,0,0,
    1,1,1,0,0,
    1,1,1,0,0,
    1,1,1,0,0,
    1,1,1,0,0,
  };
  char d[] = {
    1,1,1,1,0,
    1,1,1,1,0,
    1,1,1,1,0,
    1,1,1,1,0,
    1,1,1,1,0,
    1,1,1,1,0,
    1,1,1,1,0,
    1,1,1,1,0,
  };

  lcd.set_char(1,a);
  lcd.set_char(2,b);
  lcd.set_char(3,c);
  lcd.set_char(4,d);

}

/////////////////////////////////////////////////////////////////
// Draws a vertical bar...
//
void MtxOrb_vbar(int x, int len) 
{
  char map[9] = {32, 1, 2, 3, 4, 5, 6, 7, 255 };
  

  int y;
  for(y=lcd.hgt; y > 0 && len>0; y--)
    {
      if(len >= lcd.cellhgt) lcd.chr(x, y, 255);
      else lcd.chr(x, y, map[len]);

      len -= lcd.cellhgt;
    }
  
}

/////////////////////////////////////////////////////////////////
// Draws a horizontal bar to the right.
//
void MtxOrb_hbar(int x, int y, int len) 
{
  char map[6] = { 32, 1, 2, 3, 4, 255  };
  

  for(; x<lcd.wid && len>0; x++)
    {
      if(len >= lcd.cellwid) lcd.chr(x,y,255);
      else lcd.chr(x, y, map[len]);
      
      len -= lcd.cellwid;
      
    }
  
}


/////////////////////////////////////////////////////////////////
// Sets up for big numbers.
//
void MtxOrb_init_num() 
{
  char out[3];
  sprintf(out, "%cn", 254);
  write(fd, out, 2);
}


/////////////////////////////////////////////////////////////////
// Writes a big number.
//
void MtxOrb_num(int x, int num) 
{
  char out[5];
  sprintf(out, "%c#%c%c", 254, x, num);
  write(fd, out, 4);
}


/////////////////////////////////////////////////////////////////
// Sets a custom character from 0-7...
//
// For input, values > 0 mean "on" and values <= 0 are "off".
//
// The input is just an array of characters...
//
void MtxOrb_set_char(int n, char *dat)
{
  char out[4];
  int row, col;
  int letter;

  if(n < 0 || n > 7) return;
  if(!dat) return;

  sprintf(out, "%cN%c", 254, n);
  write(fd, out, 3);

  
  for(row=0; row<lcd.cellhgt; row++)
  {
    letter = 0;
    for(col=0; col<lcd.cellwid; col++)
    {
      letter <<= 1;
      letter |= (dat[(row*lcd.cellwid) + col] > 0);
    }
    write(fd, &letter, 1);
  }
}


void MtxOrb_icon(int which, char dest)
{
  char icons[3][5*8] = {
   {
     1,1,1,1,1,  // Empty Heart
     1,0,1,0,1,
     0,0,0,0,0,
     0,0,0,0,0,
     0,0,0,0,0,
     1,0,0,0,1,
     1,1,0,1,1,
     1,1,1,1,1,
   },   

   {
     1,1,1,1,1,  // Filled Heart
     1,0,1,0,1,
     0,1,0,1,0,
     0,1,1,1,0,
     0,1,1,1,0,
     1,0,1,0,1,
     1,1,0,1,1,
     1,1,1,1,1,
   },
   
   {
     0,0,0,0,0,  // Ellipsis
     0,0,0,0,0,
     0,0,0,0,0,
     0,0,0,0,0,
     0,0,0,0,0,
     0,0,0,0,0,
     0,0,0,0,0,
     1,0,1,0,1,
   },
   
  };

  lcd.set_char(dest, &icons[which][0]);
  
}


/////////////////////////////////////////////////////////////
// Blasts a single frame onscreen, to the lcd...
//
// Input is a character array, sized lcd.wid*lcd.hgt
//
void MtxOrb_draw_frame(char *dat)
{
  char out[LCD_MAX_WIDTH * LCD_MAX_HEIGHT];
  
  if(!dat) return;

  sprintf(out, "%cG%c%c", 254, 1, 1);
  write(fd, out, 4);
  write(fd, dat, lcd.wid*lcd.hgt);

}



/////////////////////////////////////////////////////////////
// returns one character from the keypad...
// (A-Z) on success, 0 on failure...
//
char MtxOrb_getkey()
{
  char in = 0;
  read(fd, &in, 1);
  return in;
}
