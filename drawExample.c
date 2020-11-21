#include <stdio.h>
#include <unistd.h>
#include "oled96.h"
#include "drawExample.h"

void stringExample()
{
	int i=0,y=0;

	// Small Font Example : 6x8
	for(i=0;i<8;i++)
	{
		if(i==0)
			oledWriteString(2,y,"SMALL FONT EXAMPLE",FONT_SMALL);
		else
			oledWriteString(0,y,"128x64 I2C OLEDScreen",FONT_SMALL);
		y++;
	}
	sleep(2); oledFill(0);y=0;


	// Normal Font Example : 8x8
	for(i=0;i<8;i++)
	{
		if(i==0)
			oledWriteString(1,y,"Normal FONT EX.",FONT_NORMAL);
		else
			oledWriteString(0,y,"0.96 inch I2C",FONT_NORMAL);
		y++;
	}
	sleep(2); oledFill(0);y=1;


	// Big Font Example : 16x24
	for(i=0;i<2;i++)
	{
		if(i==0)
			oledWriteString(0,y,"BIG FONT",FONT_BIG);
		else
			oledWriteString(0,y,"MUHAMMET",FONT_BIG);
		y+=3;
	}
	sleep(2); oledFill(0);
}

void drawTriangle()
{
	int i=0;
	for (i=0; i<32; i++)
	{
		oledSetPixel(64-i,16+i, 0);
		oledSetPixel(64+i,16+i, 0);
	}

	for (i=0; i<64; i++)
	{
		oledSetPixel(32+i,48, 0);
	}
}


void drawX()
{
	int i=0;
	for (i=0; i<32; i++)
	{
		oledSetPixel(48+i,16+i, 0);
		oledSetPixel(80-i,16+i, 0);
	}
}


void drawSquare()
{
	int i=0;
	for (i=0; i<32; i++)
	{
		oledSetPixel(48+i,16, 0);
		oledSetPixel(48+i,48, 0);
		oledSetPixel(48, 16+i, 0);
		oledSetPixel(80, 16+i, 0);
	}
}


void drawRectangle()
{
	int i=0;
	for (i=0; i<64; i++)
	{
		oledSetPixel(32+i,16, 0);
		oledSetPixel(32+i,48, 0);
	}

	for (i=0; i<32; i++)
	{
		oledSetPixel(32, 16+i, 0);
		oledSetPixel(96, 16+i, 0);
	}
}
