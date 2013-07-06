Path: accuvax.nwu.edu!tank!shamash!com50!pwcs!minnow!lee
From: lee@minnow.sp.unisys.com (Gene Lee)
Newsgroups: comp.graphics,comp.sys.ibm.pc
Subject: How to use VGA Cards (source included)
Date: 12 Apr 89 11:55:15 GMT
Keywords: VGA

/* I have had several requests for information on how to write software
    which can make use of the 256 color modes of VGA.  Rather than mail
    everyone a copy I am posting these routines as an example.  They 
    should at least give the concepts behind programming the card.

/* ----------------------------------------------------------------------- */

/*

		        VGA.C

     The following are a few routines written in Turbo-C which plot
     pixels and set palette in VGA mode for Paradise VGA cards.
     
     Hack them as you wish
     
*/     

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>

#include "\drvd\progs\vga\vga.h"


/* global variables */

/* values used for the rainbow palette routine */
int W=75;
int RC=64;
int GC=128;
int BC=192;
int H=63;

int XSIZE,YSIZE;       /* number of pixels in x and y direction */
int MAX_COLOR,NUM_COLORS;   /* max color value and maxcolor value +1 */

int __current_mode__=TEXT_MODE;
static unsigned int lasthio=-1;
p palette[256];

/* _______________________________________________________________________ */


void vga_dot(x,y, color)
int x,y,color;
{
   union REGS in, out;

   static char *p=MK_FP(0xA000,0);
   static unsigned long offset;   /* offset into VGA memory */
   static unsigned int hio,lowo;  /* hi and low portions of offset */

#ifndef USE_BIOS

	switch (__current_mode__) {

	case VGA320200:
			p[XSIZE*y+x] = color;

			break;

	case VGA640400:
	   offset =((long)XSIZE*y)+x;

           /* in order to access all pixels in a 640x400 display, the offset */
	   /* generated can be greater than 64K.  To get around this problem */
	   /* they have chosen to break up the offset as follows. */
	   /* Break into hi and low parts, ie. 0x1A2B3 give hi:0x1A low:0x2B3 */
	   /* hi will be used put into the VGA register PR0A and */ 
	   /* and low will be the offset */

	   hio = (unsigned int)(offset >>12);
	   lowo = (unsigned int)(offset & 0xFFF);


	   /* only set PR0A if its changed from last time */
	   if (hio != lasthio) {
		   lasthio = hio;

		   /* unlock PR0A*/
		   outport (0x3CE, 0x50F);

		   /* set PR0A */
		   hio = (hio<<8) | 9;
		   outport(0x3CE,hio);

		   /* lock PR0A again*/
		   outport (0x3CE, 0x00F);

	   }

	   p[lowo] = color;

	   break;

	default:         /* any other mode */
	   in.h.ah =0xc;
	   in.h.al =color;
	   in.x.dx =y;
	   in.x.cx =x;
	   in.h.bh =0;     /* page zero */

	   int86(VIDEO_INT, &in, &out);

	   break;
	}


#else
	   in.h.ah =0xc;
	   in.h.al =color;
	   in.x.dx =y;
	   in.x.cx =x;
	   in.h.bh =0;     /* page zero */
	   int86(VIDEO_INT, &in, &out);
#endif
}

/* _____________________________________________________________________ */


void vga_mode(mode)
int mode;
{
   union REGS in, out;

   lasthio = -1;    /* initialize variables for plotting */

   __current_mode__ = mode;   /* know the current mode */

   switch (mode) {

   case VGA640400:
	   XSIZE = 640;
	   YSIZE = 400;

	   MAX_COLOR = 255;
	   NUM_COLORS = MAX_COLOR +1;

	   break;

   case VGA320200:
	   XSIZE = 320;
	   YSIZE = 200;

	   MAX_COLOR = 255;
	   NUM_COLORS = MAX_COLOR +1;

	   break;

   case VGA800600:
	   XSIZE = 800;
	   YSIZE = 600;

	   MAX_COLOR = 15;
	   NUM_COLORS = MAX_COLOR +1;

	   break;


   default:
	   printf("vga_mode: Invalid graphics mode\n");
	   exit(-1);

	   break;
   }

   in.x.ax = mode;

   int86(VIDEO_INT, &in, &out);
}
/* _____________________________________________________________________ */


void text_mode()
{
   union REGS in, out;

   __current_mode__ = TEXT_MODE;   /* know the current mode */


   in.x.ax = TEXT_MODE;
   int86(VIDEO_INT, &in, &out);
}

/* _______________________________________________________________________ */

/*
		set the whole vga palette with one call
*/

void set_vga_palette(palette)
p *palette;
{
   union REGS in, out;
   struct SREGS sregs;

   in.x.ax =0x1012;
   in.x.bx =0;
   in.x.cx =256;

   sregs.es = FP_SEG(palette);
   in.x.dx = FP_OFF(palette);


   int86x(VIDEO_INT, &in, &out, &sregs);
}


/* _______________________________________________________________________ */

/*
		set one index in the vga palette
*/

void set_vga_pal(index,r,g,b)
int index;
int r,g,b;
{
   union REGS in, out;

   in.x.ax =0x1010;

   in.x.bx =index;
   in.h.dh =r;
   in.h.ch =g;
   in.h.cl =b;

   int86(VIDEO_INT, &in, &out);
}


/* --------------------------------------------------------------------- */

/* just for fun, this routine will setup the VGA palette with the colors
   of the rainbow.  The basic idea is to slightly overlap three (red,green,
   blue) normal shaped curves.  The height of each curve is the intensity
   of the red,green or blue color for that palette index.
   
          ^  |
          |  |    ..  ..   ..
    Intensity|   .  ..  . .  .
             |  .   ..   .    .
             | .   .  . . .    .
             |__________________
                Palette Index ->
                
*/
                
void rainbow_palette()
{
   union REGS in, out;
   struct SREGS sregs;

	int i;

	for (i=1 ; i < 256 ; i++) {
		palette[i].red = H * pow(M_E,-1.0*pow(((double)i-RC)/W,2.0));
		palette[i].green = H * pow(M_E,-1.0*pow(((double)i-GC)/(W),2.0));
		palette[i].blue = H * pow(M_E,-1.0*pow(((double)i-BC)/W,2.0));

	}

   in.x.ax =0x1012;
   in.x.bx =0;
   in.x.cx =256;

   sregs.es = FP_SEG(palette);
   in.x.dx = FP_OFF(palette);


   int86x(VIDEO_INT, &in, &out, &sregs);
}

--------------------- CUT HERE -----------------------------------------

/*
	                        VGA.H
*/	                        

typedef struct {
		unsigned char red,green,blue;
		} p;


#define ESC 0x1B
#define VIDEO_INT 0x10
#define EGA_MODE  0x13
#define TEXT_MODE 0x3
#define VGA640400 0x5E
#define VGA320200 0x13
#define VGA800600 0x58


#ifndef W
extern int W;
extern int RC;
extern int GC;
extern int BC;
extern int H;

extern int XSIZE,YSIZE;       /* number of pixels in x and y direction */
extern int MAX_COLOR,NUM_COLORS;   /* max color value and maxcolor value +1 */

extern int __current_mode__;
extern p palette[256];

#endif


/* proto types */

void vga_dot(int x, int y, int color);
void vga_mode(int mode);
void text_mode(void);
void set_vga_palette(p *palette);
void set_vga_pal(int index, int red, int green, int blue);
void rainbow_palette(void);



-- 
Gene Lee               UUCP: ...!amdahl!ems!minnow!lee
Unisys Corporation             
Phone: +1 612 635 3993    CSNET: lee@minnow.SP.Unisys.Com
If not for the courage of the fearless manager, the paycheck would be lost.
