#ifndef MTXORB_H
#define MTXORB_H



int MtxOrb_init(char *device);
void MtxOrb_close();
void MtxOrb_flush_box(int lft, int top, int rgt, int bot);
void MtxOrb_contrast(int contrast);
void MtxOrb_backlight(int on);
void MtxOrb_init_vbar();
void MtxOrb_init_hbar();
void MtxOrb_vbar(int x, int len);
void MtxOrb_hbar(int x, int y, int len);
void MtxOrb_init_num();
void MtxOrb_num(int x, int num);
void MtxOrb_set_char(int n, char *dat);
void MtxOrb_icon(int which, char dest);
void MtxOrb_draw_frame(char *dat);
char MtxOrb_getkey();



#endif
