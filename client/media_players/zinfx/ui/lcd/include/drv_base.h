#ifndef LCD_DRV_BASE_H
#define LCD_DRV_BASE_H

/********************************************************************
  How to make a driver for LCDproc

  Note: Insert the name of your driver in place of the phrase "new_driver".
  
  1. Copy drv_base.c and drv_base.h to new_driver.c and new_driver.h.

  2. Remove all functions which you *don't* need to override.
     lcd.h tells which ones you probably don't need to change, but use
     your judgement about which ones you need.

  3. Rename all functions from "drv_base_*" to "new_driver_*".

  4. Write the new driver functions.
     Be sure to call drv_base_close() in your new_driver_close(), to
     free memory used by the frame buffer.

  5. Add the driver to lcd.c, in the physical drivers section, protected
     by an #ifdef like the rest of the drivers are.

  6. Add your driver to the Makefile, in the same style as the existing ones.

******************************************************************/


int drv_base_init(char *device);
void drv_base_close();
void drv_base_clear();
void drv_base_flush();
void drv_base_string(int x, int y, char string[]);
void drv_base_chr(int x, int y, char c);
void drv_base_contrast(int contrast);
void drv_base_backlight(int on);
void drv_base_init_vbar();
void drv_base_init_hbar();
void drv_base_init_num();
void drv_base_vbar(int x, int len);
void drv_base_hbar(int x, int y, int len);
void drv_base_num(int x, int num);
void drv_base_set_char(int n, char *dat);
void drv_base_icon(int which, char dest);
void drv_base_flush_box(int lft, int top, int rgt, int bot);
void drv_base_draw_frame(char *dat);
char drv_base_getkey();


#endif
