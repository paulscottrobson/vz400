// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.h
//		Purpose:	Hardware Emulation Header
//		Created:	21st October 2021
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _HARDWARE_H
#define _HARDWARE_H

void HWReset(void);
void HWSync(void);
BYTE8 HWReadPort(WORD16 addr);
void HWWritePort(WORD16 addr,BYTE8 data);
void HWWriteControlLatch(BYTE8 data);
BYTE8 HWGetVideoMode(void);
BYTE8 *HWGetPalette(BYTE8 colour);
BYTE8 HWReadKeyboardPort(WORD16 addr);
void HWLoadProgram(void);
BYTE8 HWGetBackgroundPalette(void);

int HWXGetKeyboardRow(int row);
void HWXSyncImplementation(LONG32 iCount);
int HWXIsKeyPressed(int code);
WORD16 HWXLoadFile(char * fileName,WORD16 *startLoad,WORD16 *endLoad,BYTE8 *type);
void HWXSetFrequency(int frequency);
void HWXLog(char *logText);
void HWXClearScreen(int colour);
void HWXPlotCharacter(int x,int y,int colour,int bgr,BYTE8 ch);
void HWXPlotPixel(int x,int y,int colour);

#define HWISTEXTMODE() ((HWGetVideoMode() & 0x8) == 0)
#define HWISGREENBACKGROUND() ((HWGetVideoMode() & 0x10) == 0)

#define PALETTE_SIZE 		(16)

#ifdef LINUX
#define FILESEP '/'
#else
#define FILESEP '\\'
#endif

#endif
