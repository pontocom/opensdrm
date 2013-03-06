#ifndef LCD_H
#define LCD_H

// Maximum supported sizes
#define LCD_MAX_WID 256
#define LCD_MAX_WIDTH 256
#define LCD_MAX_HGT 256
#define LCD_MAX_HEIGHT 256


int lcd_init(char *device, char *driver);



/////////////////////////////////////////////////////////////////
// Driver functions / info held here...
//
// Feel free to override any/all functions here with real driver
// functions...
//
typedef struct lcd_logical_driver 
{
  // Size in cells of the LCD
  int wid, hgt;
  // Size of each LCD cell, in pixels
  int cellwid, cellhgt;
  // Frame buffer...
  char *framebuf;

  // Functions which might be the same for all drivers...
  void (*clear)();
  void (*string)(int x, int y, char lcd[]);
  
  void (*chr)(int x, int y, char c);
  void (*vbar)(int x, int len);
  void (*hbar)(int x, int y, int len);
  void (*init_num)();
  void (*num)(int x, int num);
  
  // Functions which should probably be implemented in each driver...
  int (*init)(char *device);
  void (*close)();
  void (*flush)();
  void (*flush_box)(int lft, int top, int rgt, int bot);
  void (*contrast)(int contrast);
  void (*backlight)(int on);
  void (*set_char)(int n, char *dat);
  void (*icon)(int which, char dest);
  void (*init_vbar)();
  void (*init_hbar)();
  void (*draw_frame)(char *dat);

  // Returns 0 for "no key pressed", or (A-Z).
  char (*getkey)();
  
  // more?
  
} lcd_logical_driver;

typedef struct lcd_physical_driver
{
  char *name;
  int (*init)(char *device);
} lcd_physical_driver;


extern lcd_logical_driver lcd;


#endif

