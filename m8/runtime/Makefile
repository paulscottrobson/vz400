ifeq ($(OS),Windows_NT)
include ..\..\documents\common.make
else
include ../../documents/common.make
endif

all: 
	ruby builder.rb
	zasm  -u runtime.asm -b m8_8000.lib
	$(CCOPY) m8*.lib ..
	

	

