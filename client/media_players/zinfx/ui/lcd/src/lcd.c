#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/errno.h>

#include "lcd.h"

#include "drv_base.h"

#ifdef MTXORB_DRV
#include "MtxOrb.h"
#endif

#ifdef TEXT_DRV
#include "text.h"
#endif

#ifdef DEBUG_DRV
#include "debug.h"
#endif

#ifdef CURSES_DRV
#include "curses_drv.h"
#endif

#ifdef HD44780_DRV
#include "hd44780.h"
#endif

// TODO: Make a Windows server, and clients...?


lcd_logical_driver lcd;

lcd_physical_driver drivers[] =
{
#ifdef MTXORB_DRV
  { "MtxOrb",   MtxOrb_init, },
#endif
#ifdef HD44780_DRV
  { "HD44780",  HD44780_init, },
#endif
#ifdef TEXT_DRV
  { "text",     text_init, },
#endif
#ifdef DEBUG_DRV
  { "debug",    debug_init, },
#endif
#ifdef CURSES_DRV
  { "curses",   curses_drv_init, },
#endif
  { NULL,       NULL, },
  
};



////////////////////////////////////////////////////////////
// This sets up which driver to use and initializes stuff.
// 
// It should not be messed with, except to add new drivers.
//
int lcd_init(char *device, char *driver)
{
  int i;

  lcd.framebuf = NULL;
  
  drv_base_init(device);

  for(i=0; drivers[i].name; i++)
  {
    if(!strcmp(driver, drivers[i].name))
    {
      return drivers[i].init(device);
    }
  }

  printf("Invalid driver: %s\n", driver);
  return -1;
  
}

