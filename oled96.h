#ifndef OLED96_H
#define OLED96_H


typedef enum
{
   FONT_NORMAL=0,	// 8x8
   FONT_BIG,		// 16x24
   FONT_SMALL		// 6x8
} FONTSIZE;

int oledInit(char *fileName, int iAddress, int bFlip, int bInvert);
void oledShutdown(void);
int oledFill(unsigned char ucPattern);
int oledWriteString(int x, int y, char *szText, int bLarge);
int oledSetPixel(int x, int y, unsigned char ucPixel);
int oledSetContrast(unsigned char ucContrast);

#endif // OLED96_H
