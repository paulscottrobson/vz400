ifeq ($(OS),Windows_NT)
include ..\documents\common.make
else
include ../documents/common.make
endif

EMUROOT = ..$(S)emulator$(S)
EMUSRC = $(EMUROOT)src$(S)

.PHONY: all serial

all: update

serial: update
	pio device monitor -b 115200
	
update:
	make -C ..$(S)emulator
	$(CCOPY) $(EMUSRC)hardware.cpp src	
	$(CCOPY) $(EMUSRC)hardware_esp.cpp src	
	$(CCOPY) $(EMUSRC)sys_processor.cpp src	
	$(CCOPY) $(EMUROOT)include/*.* include
	$(CCOPY) $(EMUROOT)roms/*.* include
	$(CCOPY) $(EMUROOT)cpu/*.* include
	$(CCOPY) $(EMUROOT)framework/gfxkeys.h include
	$(CCOPY) $(EMUROOT)storage/*.* data
	pio lib install 6143 	
	pio run -t upload
	pio run -t uploadfs	