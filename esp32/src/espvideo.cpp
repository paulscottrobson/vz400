// ****************************************************************************
// ****************************************************************************
//
//		Name:		espvideo.cpp
//		Purpose:	Video Routines
//		Created:	21st October 2021
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// ****************************************************************************
// ****************************************************************************

#include "espinclude.h"

// ****************************************************************************
//
//								Static objects
//
// ****************************************************************************

fabgl::VGAController DisplayController;
fabgl::Canvas        Canvas(&DisplayController);

static RGB888 pColours[PALETTE_SIZE];
static uint8_t rawPixels[PALETTE_SIZE];

#define DWIDTH 320
#define DHEIGHT 240 
#define XOFFSET 32
#define YOFFSET 24

void HWXClearScreen(int colour);

// ****************************************************************************
//
//						Initialise video subsystem
//
// ****************************************************************************

#define CONVCOL(n) 	(((n) == 0) ? 0 : 255)

void HWESPVideoInitialise(void) {
	#if USE_8_COLORS
	DisplayController.begin(VGA_RED, VGA_GREEN, VGA_BLUE, VGA_HSYNC, VGA_VSYNC);
	#elif USE_64_COLORS
	DisplayController.begin(VGA_RED1, VGA_RED0, VGA_GREEN1, VGA_GREEN0, VGA_BLUE1, VGA_BLUE0, VGA_HSYNC, VGA_VSYNC);
	#endif
	DisplayController.setResolution(QVGA_320x240_60Hz);
	DisplayController.enableBackgroundPrimitiveExecution(false);

	HWXClearScreen(0);
}

void HWXClearScreen(int colour) { 

	for (int i = 0;i < PALETTE_SIZE;i++) {
		BYTE8 *col = HWGetPalette(i);
		pColours[i].R = col[0];
		pColours[i].G = col[1];
		pColours[i].B = col[2];
	 	rawPixels[i] = DisplayController.createRawPixel(RGB222(pColours[i].R>>6,pColours[i].G>>6,pColours[i].B>>6));
	}

	for (int x = 0;x < DWIDTH;x++)
		for (int y = 0;y < DHEIGHT;y++)
			DisplayController.setRawPixel(x,y,rawPixels[colour]);
}

void HWXPlotCharacter(int x,int y,int colour,int bgr,BYTE8 ch) { 
	x = x * 8 + XOFFSET;
	y = y * 12 + YOFFSET;
	for (int y1 = 0;y1 < 12;y1++) {
		BYTE8 b = CPUReadCharacterROM(ch,y1);
		for (int x1 = 0;x1 < 8;x1++) {
			DisplayController.setRawPixel(x+x1,y+y1,rawPixels[(b & 0x80) ? colour:bgr]);
			b = b << 1;
		}
	}
}

void HWXPlotPixel(int x,int y,int colour) {
	x = x * 2 + XOFFSET;
	y = y * 3 + YOFFSET;
	uint8_t col = rawPixels[colour];
	DisplayController.setRawPixel(x+0,y+0,col);
	DisplayController.setRawPixel(x+1,y+0,col);
	DisplayController.setRawPixel(x+0,y+1,col);
	DisplayController.setRawPixel(x+1,y+1,col);
	DisplayController.setRawPixel(x+0,y+2,col);
	DisplayController.setRawPixel(x+1,y+2,col);
}

