	org 0
	ld hl,$0E08  			; H = $0E, L = $08, C = ?
	sla l 					; H = $0E, L = $10, C = 0
	srl h 					; H = $07, L = $10, C = 0
	rr l  					; H = $07, L = $08, C = 0
	srl h 					; H = $03, L = $08, C = 1
	rr l  					; H = $03, L = $84, C = 0
	srl h  					; H = $01, L = $84, C = 1
	rr l 					; H = $01, L = $C2, C = 0
