// ****************************************************************************
// ****************************************************************************
//
//		Name:		main.cpp
//		Purpose:	Main Program (esp version)
//		Created:	21st October 2020
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// ****************************************************************************
// ****************************************************************************

#include "espinclude.h"

int nextFrameTime;

// ****************************************************************************
//
//								Set up code
//
// ****************************************************************************

void setup()
{
	Serial.begin(115200);delay(500);
	int fsOpen = SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);
	HWESPVideoInitialise();
	HWESPSoundInitialise();
	HWESPKeyboardInitialise();
	CPUReset();
	nextFrameTime = millis();
}

// ****************************************************************************
//
//									Execution
//
// ****************************************************************************

void loop()
{
    unsigned long frameRate = CPUExecuteInstruction();
    if (frameRate != 0) {
		while (millis() < nextFrameTime) {}
		nextFrameTime = nextFrameTime + 1000 / frameRate;
	}
}	

