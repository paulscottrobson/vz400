// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.cpp
//		Purpose:	Processor Emulation.
//		Created:	21st October 2021
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include "sys_processor.h"
#include "sys_debug_system.h"
#include "hardware.h"

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define FRAME_RATE		(60)														// Frames per second (60Hz)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE) 									// Cycles per frame.

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

static BYTE8 A,B,C,D,E,H,L;  														// Standard register
static WORD16 AFalt,BCalt,DEalt,HLalt; 												// Alternate data set.
static WORD16 PC,SP; 																// 16 bit registers
static WORD16 IX,IY; 																// IX IY accessed indirectly.

static char logBuffer[64];

static BYTE8 s_Flag,z_Flag,c_Flag,h_Flag,n_Flag,p_Flag; 							// Flag Registers
static BYTE8 I,R,intEnabled; 														// We don't really bother with these much.

static BYTE8 ramMemory[0x10000];													// Memory at $0000 upwards

static LONG32 temp32;
static WORD16 temp16,temp16a,*pIXY;
static BYTE8 temp8,oldCarry;

static int cycles = 512;															// Cycle Count.
static int cyclesPerFrame = CYCLES_PER_FRAME;										// Cycles per frame
static int highSpeed = 0; 															// Force to 33Mhz

#define CYCLES(n) cycles -= (n)

// *******************************************************************************************************************************
//														ROM Image
// *******************************************************************************************************************************

static const BYTE8 _monitor_rom[] = {
	#include "_v20_rom.h"
};

static const BYTE8 _char_rom[] = {
	#include "_char_rom.h"
};

// *******************************************************************************************************************************
//											 Memory and I/O read and write macros.
// *******************************************************************************************************************************

#define READ8(a) 	_Read(a)														// Basic Read
#define WRITE8(a,d)	_Write(a,d)														// Basic Write

#define READ16(a) 	(READ8(a) | ((READ8((a)+1) << 8)))								// Read 16 bits.
#define WRITE16(a,d) { WRITE8(a,(d) & 0xFF);WRITE8((a)+1,(d) >> 8); } 				// Write 16 bits

#define FETCH8() 	_FastRead(PC++)													// Fetch byte without fiddling.
#define FETCH16()	_Fetch16()	 													// Fetch word

static inline BYTE8 _Read(WORD16 address);											// Need to be forward defined as 
static inline void _Write(WORD16 address,BYTE8 data);								// used in support functions.

#define INPORT(p) 	HWReadPort(p)
#define OUTPORT(p,d) HWWritePort(p,d)

// *******************************************************************************************************************************
//											   Read and Write Inline Functions
// *******************************************************************************************************************************

static inline BYTE8 _Read(WORD16 address) {
	if (address < RAMSIZE) {
		if (address >= 0x6800 && address < 0x7000) {
			BYTE8 kbd = HWReadKeyboardPort(address) | 0xC0;
			if (cycles > cyclesPerFrame*3/4) kbd &= 0x7F;
			return kbd;
		}
		return ramMemory[address];							
	}
	return 0xFF;
}

static inline BYTE8 _FastRead(WORD16 address) {
	return _Read(address);
}

static void _Write(WORD16 address,BYTE8 data) {
	BYTE8 oldByte = ramMemory[address];
	if (address >= 0x4000 && address <= RAMSIZE) {
		ramMemory[address] = data;
	}	
	if (address >= 0x6800 && address < 0x7000) {
		HWWriteControlLatch(data);
	}
	if (address >= 0x7000 && address < 0x7800 && oldByte != data) {
		if (HWISTEXTMODE() && address < 0x7200) {
			int ch = READ8(address);
			int bgr = HWISGREENBACKGROUND() ? 12 : 14;
			int fgr = bgr+1;
			if (ch >= 0x80) fgr = (ch & 0x70) >> 4;
			HWXPlotCharacter((address & 0x1F),(address >> 5) & 0x0F,fgr,bgr,ch);
		} else {
			int step = HWISGREENBACKGROUND() ? 0 : 4;
			for (int px = 0;px < 4;px++) {
				HWXPlotPixel((address & 0x1F)*4+px,(address >> 5) & 0x3F,((data >> 6) & 3)+step);
				data = data << 2;
			}
		}
	}
	//
	// 		Extensions
	//
	if (address < 0x2000) {
		//
		//	 	POKE 4096,1 turns on 33Mhz mode.
		//
		if (address == 0x1000) highSpeed = (data != 0); 		
		//
		// 		POKE 4352... sets palette
		//
		if ((address & 0xFF00) == 0x1100) {
			HWSetPalette((address >> 2) & 0x3F,address & 0x03,data);			
		}
	}
}

static WORD16 _Fetch16(void) {
	WORD16 w = READ16(PC);
	PC += 2;
	return w;
}

BYTE8 CPUReadCharacterROM(int ch,int row) {
	return _char_rom[ch*12+row];
}

// *******************************************************************************************************************************
//											 Support macros and functions
// *******************************************************************************************************************************

#ifdef INCLUDE_DEBUGGING_SUPPORT
#include <stdlib.h>
#define FAILOPCODE(g,n) exit(fprintf(stderr,"Opcode %02x in group %s\n",n,g))
#else
#define FAILOPCODE(g,n) {}
#endif

#include "cpu_support.h"

int CPUGetCycles(void) {
	return cycles;
}

void CPUSetPC(WORD16 newPC) {
	PC = newPC;
}

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

#ifdef INCLUDE_DEBUGGING_SUPPORT
static void CPULoadChunk(FILE *f,BYTE8* memory,int count);
#endif

void CPUReset(void) {
	HWReset();																		// Reset Hardware
	for (int i = 0;i < sizeof(_monitor_rom);i++) { 									// Reset the ROM image
		ramMemory[i] = _monitor_rom[i];
	}
	BuildParityTable();																// Build the parity flag table.
	PC = 0; 																		// Zero PC.
}

// *******************************************************************************************************************************
//					Called on exit, does nothing on ESP32 but required for compilation
// *******************************************************************************************************************************

#ifdef INCLUDE_DEBUGGING_SUPPORT
#include "gfx.h"
void CPUExit(void) {	
	printf("Exited via $FFFF");
	GFXExit();
}
#else
void CPUExit(void) {}
#endif

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

int CPUExecuteInstruction(void) {
	#ifdef INCLUDE_DEBUGGING_SUPPORT
	if (PC == 0xFFFF) CPUExit();
	#endif

	if (PC == 0xFFD0) {
		sprintf(logBuffer,"PC %x %x",PC,HL());
		HWXLog(logBuffer);
		}

	BYTE8 opcode = FETCH8();														// Fetch opcode.
	switch(opcode) {																// Execute it.
		#include "_code_group_0.h"
		default:
			FAILOPCODE("-",opcode);
	}
	if (cycles >= 0 ) return 0;														// Not completed a frame.
	cycles = cycles + (highSpeed ? 33*1000*1000 : cyclesPerFrame);					// Adjust this frame rate, up to x16 on HS
	HWSync();																		// Update any hardware
	 if (intEnabled) { 																// Interrupt enabled.
	  	PUSH(PC);																	// Do RST 38h
	  	CPUSetPC(0x38);
	 	intEnabled = 0;
	 }
	return FRAME_RATE;																// Return frame rate.
}

// *******************************************************************************************************************************
//												Read/Write Memory
// *******************************************************************************************************************************

BYTE8 CPUReadMemory(WORD16 address) {
	return READ8(address);
}

void CPUWriteMemory(WORD16 address,BYTE8 data) {
	WRITE8(address,data);
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//		Execute chunk of code, to either of two break points or frame-out, return non-zero frame rate on frame, breakpoint 0
// *******************************************************************************************************************************

int CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2) { 
	BYTE8 next;
	do {
		int r = CPUExecuteInstruction();											// Execute an instruction
		if (r != 0) return r; 														// Frame out.
		next = CPUReadMemory(PC);
	} while (PC != breakPoint1 && PC != breakPoint2 && next != 0x76);				// Stop on breakpoint or $76 HALT break
	return 0; 
}

// *******************************************************************************************************************************
//									Return address of breakpoint for step-over, or 0 if N/A
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	BYTE8 op = CPUReadMemory(PC); 	
	if (op == 0xCD || (op & 0xC7) == 0xC4) return PC+3; 							// CALL/CALL xx
	if ((op & 0xC7) == 0xC7) return PC+1;											// RST
	return 0;																		// Do a normal single step
}

void CPUEndRun(void) {
	FILE *f = fopen("memory.dump","wb");
	fwrite(ramMemory,1,RAMSIZE,f);
	fclose(f);
}

void CPULoadBinary(char *fileName) {
	// FILE *f = fopen(fileName,"rb");
	// BYTE8 *romSpace = CPUGetUpper8kAddress();
	// for (int i = 0;i < 0x4000;i++) romSpace[i] = 0;
	// if (f != NULL) {
	// 	fread(romSpace,1,0x4000,f);
	// 	fclose(f);
	// }
}

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS st;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	st.AF = AF();
	st.BC = BC();
	st.DE = DE();
	st.HL = HL();
	st.AFalt = AFalt;
	st.BCalt = BCalt;
	st.DEalt = DEalt;
	st.HLalt = HLalt;
	st.PC = PC;
	st.SP = SP;
	st.IX = IX;
	st.IY = IY;
	st.IE = intEnabled;
	st.cycles = cycles;
	return &st;
}

#endif