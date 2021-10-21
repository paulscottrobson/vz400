// ****************************************************************************
// ****************************************************************************
//
//		Name:		hardware_esp.c
//		Purpose:	Hardware Emulation (ESP32 Specific)
//		Created:	21st October 2021
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// ****************************************************************************
// ****************************************************************************

#include <Arduino.h>
#include "sys_processor.h"
#include "gfxkeys.h"
#include "hardware.h"
#include "espinclude.h"

int HWESPGetScanCode(void);
void HWESPSetFrequency(int freq);

static int scan_to_gfx[256] = {
	#include "_scan_to_gfx.h"
};

// ****************************************************************************
//							  Sync Hardware
// ****************************************************************************

static BYTE8 keyStatus[256] = {0};
static int shift = 0;
static int release = 0;
static int lastKey = 0;

void HWXSyncImplementation(LONG32 iCount) {
	int scanCode = HWESPGetScanCode();
	while (scanCode > 0) {
		if (scanCode == 0xE0) {
			shift = 0x80;
		} 
		else if (scanCode == 0xF0) {
			release = 1;
		}
		else {
			int isDown = (release == 0);
			scanCode = (scanCode & 0x7F) | shift;
			Serial.println(scanCode);
			scanCode = scan_to_gfx[scanCode];
			//writeCharacter(scanCode & 0x0F,(scanCode >> 4)+2,isDown ? '*' : '.');
			keyStatus[scanCode] = isDown;
			lastKey = isDown ? scanCode : -scanCode;
			shift = 0x00;
			release = 0x00;
		}
		scanCode = HWESPGetScanCode();
		keyStatus[GFXKEY_SHIFT] = keyStatus[GFXKEY_LSHIFT]|keyStatus[GFXKEY_RSHIFT];
		keyStatus[GFXKEY_CONTROL] = keyStatus[GFXKEY_LCTRL]|keyStatus[GFXKEY_RCTRL];
	}
	if (keyStatus[GFXKEY_CONTROL] && keyStatus[GFXKEY_ESCAPE]) CPUReset();			/* Ctrl+ESC is Reset */
}

void HWXLog(char *logText) {
	Serial.println(logText);
}

// ****************************************************************************
//					Check if key pressed (GFXKEY values)
// ****************************************************************************

int  HWXIsKeyPressed(int key) {
	return keyStatus[key] != 0;
}

// ****************************************************************************
//						Get System time in 1/100s
// ****************************************************************************

WORD16 HWGetSystemClock(void) {
	return (millis() / 10) & 0xFFFF;
}

// ****************************************************************************
//	 						Set sound pitch, 0 = off
// ****************************************************************************

void HWXSetFrequency(int frequency) {
	HWESPSetFrequency(frequency);
}

// ****************************************************************************
// 									Load file
// ****************************************************************************

WORD16 HWXLoadFile(char * fileName,WORD16 *startLoad,WORD16 *endLoad,BYTE8 *type) {
	char fullName[128];
	if (fileName[0] == 0) return 1;
	sprintf(fullName,"/%s",fileName);
	WORD16 exists = SPIFFS.exists(fullName);		// If file exitst
	if (exists != 0) {
		File file = SPIFFS.open(fullName);			// Open it
		for (int i = 0;i < 21;i++) file.read(); 	// Don't care (21 bytes)
		int fType = file.read(); 					// File type (1 byte)
		int start = file.read(); 					// Load Address (2 bytes)
		start = start + (file.read() << 8);
		*startLoad = start; 						// Return start address
		while (file.available()) { 					// Rest is data
			BYTE8 data = file.read();
			CPUWriteMemory(start++,data);
		}
		file.close();
		*type = fType & 0xFF; 						// Return type
		*endLoad = start & 0xFFFF; 					// And location
	}
	return exists == 0;
}


