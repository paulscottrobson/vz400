; ***************************************************************************************
; ***************************************************************************************
;
;		Name : 		runtime.asm
;		Author :	Paul Robson (paul@robsons.org.uk)
;		Date : 		21st October 2021
;		Purpose :	Binary operators (A ? B -> A)
;
; ***************************************************************************************
; ***************************************************************************************


		.org 	$8000
		;
		; 		This is patch to kickstart
		;
start:		
		jp 		$8000  						; +0 	vector to main program (patched)
		.dw 	Dictionary-start 			; +3 	offset to dictionary list.
		;
		; 		Data area
		;
seed1:
		.dw 	12345
seed2:
		.dw 	54321		

		;
		; 		Dictionary included here.
		;
Dictionary:		
		.include "_built.asm"