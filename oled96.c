
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "oled96.h"

extern unsigned char ucFont[], ucSmallFont[];
static int iScreenOffset; // current write offset of screen data
static unsigned char ucScreen[1024]; // local copy of the image buffer
static int file_i2c = 0;

static void oledWriteCommand(unsigned char);

int oledInit(char *fileName, int iAddr,int bFlip, int bInvert)
{
	
	const unsigned char oled64_initbuf[]={0x00,0xae,0xa8,0x3f,0xd3,0x00,0x40,0xa1,0xc8,
	      0xda,0x12,0x81,0xff,0xa4,0xa6,0xd5,0x80,0x8d,0x14,
	      0xaf,0x20,0x02};

	unsigned char uc[4];


	if ((file_i2c = open(fileName, O_RDWR)) < 0)
	{
		fprintf(stderr, "Failed to open i2c bus %d\n", fileName);
		file_i2c = 0;
		return 1;
	}


	if (ioctl(file_i2c, I2C_SLAVE, iAddr) < 0)
	{
		fprintf(stderr, "Failed to acquire bus access or talk to slave\n");
		file_i2c = 0;
		return 1;
	}

	write(file_i2c, oled64_initbuf, sizeof(oled64_initbuf));
	

	if (bInvert)
	{
		uc[0] = 0; 		// command
		uc[1] = 0xa7; 	// invert command
		write(file_i2c, uc, 2);
	}

	if (bFlip) // rotate display 180
	{
		uc[0] = 0; // command
		uc[1] = 0xa0;
		write(file_i2c, uc, 2);
		uc[1] = 0xc0;
		write(file_i2c, uc, 2);
	}
	
	return 0;
}


void oledShutdown()
{
	if (file_i2c != 0)
	{
		oledWriteCommand(0xaE); // turn off OLED
		close(file_i2c);
		file_i2c = 0;
	}
}



static void oledWriteCommand(unsigned char c)
{
	int rc;
	unsigned char buf[2];
	
	buf[0] = 0x00; // command introducer
	buf[1] = c;
	rc = write(file_i2c, buf, 2);
	if (rc) {} // suppress warning
} 


static void oledSetPosition(int x, int y)
{
	iScreenOffset = (y*128)+x;
	oledWriteCommand(0xb0 | y); // go to page Y
	oledWriteCommand(0x00 | (x & 0xf)); // // lower col addr
	oledWriteCommand(0x10 | ((x >> 4) & 0xf)); // upper col addr
}


static void oledWriteDataBlock(unsigned char *ucBuf, int iLen)
{
	int rc;
	unsigned char ucTemp[129];

	ucTemp[0] = 0x40; // data command
	memcpy(&ucTemp[1], ucBuf, iLen);

	rc = write(file_i2c, ucTemp, iLen+1);
	if (rc) {} // suppress warning
	
	memcpy(&ucScreen[iScreenOffset], ucBuf, iLen);
	iScreenOffset += iLen;
}


// Set (or clear) an individual pixel
// The local copy of the frame buffer is used to avoid
// reading data from the display controller
int oledSetPixel(int x, int y, unsigned char ucColor)
{
	int i;
	unsigned char uc, ucOld;

	if (file_i2c == 0)
		return -1;

	i = ((y >> 3) * 128) + x;

	if (i < 0 || i > 1023) // off the screen
		return -1;

	uc = ucOld = ucScreen[i];
	uc &= ~(0x1 << (y & 7));
	if (ucColor)
	{
		uc |= (0x1 << (y & 7));
	}
	if (uc != ucOld) // pixel changed
	{
		oledSetPosition(x, y>>3);
		oledWriteDataBlock(&uc, 1);
	}
	return 0;
}


int oledWriteString(int x, int y, char *szMsg, int iSize)
{
	int i, iLen;
	unsigned char *s;

	if (file_i2c == 0) 
		return -1; // not initialized

	if (iSize < FONT_NORMAL || iSize > FONT_SMALL)
		return -1;


	iLen = strlen(szMsg);
	if (iSize == FONT_BIG) // draw 16x24 font
	{
		if (iLen+x > 8) iLen = 8-x;
		if (iLen < 0) return -1;
		x *= 16;
		for (i=0; i<iLen; i++)
		{
			s = &ucFont[9728 + (unsigned char)szMsg[i]*64];
			oledSetPosition(x+(i*16), y);
			oledWriteDataBlock(s, 16);
			oledSetPosition(x+(i*16), y+1);
			oledWriteDataBlock(s+16, 16);	
			oledSetPosition(x+(i*16), y+2);
			oledWriteDataBlock(s+32, 16);	
		}
	}
	else if (iSize == FONT_NORMAL) // draw 8x8 font
	{
		oledSetPosition(x*8, y);
		if (iLen + x > 16) iLen = 16 - x; // can't display it
		if (iLen < 0)return -1;

		for (i=0; i<iLen; i++)
		{
			s = &ucFont[(unsigned char)szMsg[i] * 8];
			oledWriteDataBlock(s, 8); // write character pattern
		}	
	}
	else // 6x8
	{
		oledSetPosition(x*6, y);
		if (iLen + x > 21) iLen = 21 - x;
		if (iLen < 0) return -1;
		for (i=0; i<iLen; i++)
		{
			s = &ucSmallFont[(unsigned char)szMsg[i]*6];
			oledWriteDataBlock(s, 6);
		}
	}
	return 0;
}


// Fill the frame buffer with a byte pattern
// e.g. all off (0x00) or all on (0xff)
int oledFill(unsigned char ucData)
{
	int y;
	unsigned char temp[128];
	int iLines, iCols;

	if (file_i2c == 0) return -1; // not initialized

	iLines = 8;
	iCols = 8;

	memset(temp, ucData, 128);
	for (y=0; y<iLines; y++)
	{
		oledSetPosition(0,y); // set to (0,Y)
		oledWriteDataBlock(temp, iCols*16); // fill with data byte
	}
	return 0;
}