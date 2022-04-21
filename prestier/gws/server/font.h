

// font.h
// the font support.

#ifndef GWS_FONT_H
#define GWS_FONT_H    1

//
// Font
//

int gfontSize;

//As fontes usadas pelo servidor gws
unsigned long gws_currentfont_address;  // fonte atual.
unsigned long g8x8fontAddress;          // 8×8, 80×25,CGA, EGA
unsigned long g8x14fontAddress;         // 8x14,80×25,EGA
unsigned long g8x16fontAddress;         // ??
unsigned long g9x14fontAddress;         // 9x14,80×25,MDA, Hercules
unsigned long g9x16fontAddress;         // 9x16,80×25,VGA


//unsigned long gws_eye_sprite_address;



//
// == prototypes =============================
//

int gwssrv_init_font(void);

#endif   







