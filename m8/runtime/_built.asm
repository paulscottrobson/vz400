

 .dw end1000-$
 .db '<',0
 .db $FF
start1000:
 ld   a,h           ; check if signs different.
 xor  d
 add  a,a          ; CS if different
 jr   nc,__less_samesign
 ld   a,d          ; different. set CS to sign of B
 add  a,a          ; if set (negative) B must be < A as A is +ve
 jr   __less_returnc
__less_samesign:
 push  de           ; save DE
 ex   de,hl          ; -1 if B < A
 sbc  hl,de          ; calculate B - A , hencs CS if < (Carry clear by add a,a)
 pop  de           ; restore DE
__less_returnc:
 ld   a,0          ; A 0
 sbc  a,0          ; A $FF if CS.
 ld   l,a          ; put in HL
 ld   h,a
 ret



end1000:
 .dw end1001-$
 .db '=',0
 .db $FF
start1001:
 ld   a,h          ; H = H ^ D
 xor  d
 ld   h,a
 ld   a,l          ; A = (L ^ E) | (H ^ D)
 xor  e
 or   h           ; if A == 0 they are the same.
 ld   hl,$0000         ; return 0 if different
 ret  nz
 dec  hl           ; return -1
 ret



end1001:
 .dw end1002-$
 .db '-',0
 .db $FF
start1002:
 push  de           ; save DE
 ex   de,hl          ; HL = B, DE = A
 xor  a            ; clear carry
 sbc  hl,de          ; calculate B-A
 pop  de           ; restore DE
 ret



end1002:
 .dw end1003-$
 .db '+',0
 .db end1003-$
start1003:
 add  hl,de



end1003:
 .dw end1004-$
 .db 'and',0
 .db $FF
start1004:
 ld   a,h
 and  d
 ld   h,a
 ld   a,l
 and  e
 ld   l,a
 ret



end1004:
 .dw end1005-$
 .db 'or',0
 .db $FF
start1005:
 ld   a,h
 or   d
 ld   h,a
 ld   a,l
 or   e
 ld   l,a
 ret



end1005:
 .dw end1006-$
 .db 'xor',0
 .db $FF
start1006:
 ld   a,h
 xor  d
 ld   h,a
 ld   a,l
 xor  e
 ld   l,a
 ret





end1006:
 .dw end1007-$
 .db '/',0
 .db $FF
start1007:
 push  de
 call  DIVDivideMod16
 ex   de,hl
 pop  de
 ret



end1007:
 .dw end1008-$
 .db 'mod',0
 .db $FF
start1008:
 push  de
 call  DIVDivideMod16
 pop  de
 ret



DIVDivideMod16:
 push  bc
 ld   b,d     ; DE
 ld   c,e
 ex   de,hl
 ld   hl,0
 ld   a,b
 ld   b,8
Div16_Loop1:
 rla
 adc  hl,hl
 sbc  hl,de
 jr   nc,Div16_NoAdd1
 add  hl,de
Div16_NoAdd1:
 djnz  Div16_Loop1
 rla
 cpl
 ld   b,a
 ld   a,c
 ld   c,b
 ld   b,8
Div16_Loop2:
 rla
 adc  hl,hl
 sbc  hl,de
 jr   nc,Div16_NoAdd2
 add  hl,de
Div16_NoAdd2:
 djnz  Div16_Loop2
 rla
 cpl
 ld   d,c
 ld   e,a
 pop  bc
 ret



end1008:
 .dw end1009-$
 .db '!',0
 .db end1009-$
start1009:
  ld   (hl),e
  inc  hl
  ld   (hl),d
  dec  hl


end1009:
 .dw end1010-$
 .db '@',0
 .db end1010-$
start1010:
  ld   a,(hl)
  inc  hl
  ld  h,(hl)
  ld  l,a



end1010:
 .dw end1011-$
 .db '+!',0
 .db $FF
start1011:
  ld   a,(hl)
  add  a,e
  ld   (hl),a
  inc  hl
  ld   a,(hl)
  adc  a,d
  ld   (hl),a
  dec  hl
  ret



end1011:
 .dw end1012-$
 .db 'c!',0
 .db end1012-$
start1012:
  ld   (hl),e



end1012:
 .dw end1013-$
 .db 'c@',0
 .db end1013-$
start1013:
  ld   l,(hl)
  ld   h,0



end1013:
 .dw end1014-$
 .db 'p@',0
 .db $FF
start1014:
  push  bc
  ld  b,h
  ld   c,l
  in   l,(c)
  ld   h,0
  pop  bc
  ret



end1014:
 .dw end1015-$
 .db 'p!',0
 .db $FF
start1015:
  push  bc
  push  hl
  ld   a,e
  ld  b,h
  ld   c,l
  out  (c),a
  pop  hl
  pop  bc
  ret




end1015:
 .dw end1016-$
 .db ';',0
 .db end1016-$
start1016:
  ret


end1016:
 .dw end1017-$
 .db 'copy',0
 .db $FF
start1017:
  ld   a,b         ; exit if C = 0
  or   c
  ret  z

  push  bc          ; BC count
  push  de          ; DE target
  push  hl          ; HL source

  xor  a          ; Clear C
  sbc  hl,de         ; check overlap ?
  jr   nc,__copy_gt_count      ; if source after target
  add  hl,de         ; undo subtract

  add  hl,bc         ; add count to HL + DE
  ex   de,hl
  add  hl,bc
  ex   de,hl
  dec  de          ; dec them, so now at the last byte to copy
  dec  hl
  lddr           ; do it backwards
  jr   __copy_exit

__copy_gt_count:
  add  hl,de         ; undo subtract
  ldir          ; do the copy
__copy_exit:
  pop  hl          ; restore registers
  pop  de
  pop  bc
  ret



end1017:
 .dw end1018-$
 .db 'fill',0
 .db $FF
start1018:
  ld   a,b         ; exit if C = 0
  or   c
  ret  z

  push  bc          ; BC count
  push  de          ; DE target, L byte
__fill_loop:
  ld   a,l         ; copy a byte
  ld   (de),a
  inc  de          ; bump pointer
  dec  bc          ; dec counter and loop
  ld   a,b
  or   c
  jr   nz,__fill_loop
  pop  de          ; restore
  pop  bc
  ret



end1018:
 .dw end1019-$
 .db 'halt',0
 .db $FF
start1019:
__halt_loop:
  di
  halt
  jr   __halt_loop



end1019:
 .dw end1020-$
 .db 'break',0
 .db end1020-$
start1020:
  db   $76




end1020:
 .dw end1021-$
 .db '*',0
 .db $FF
start1021:
 jp   MULTMultiply16


MULTMultiply16:
  push  bc
  push  de
  ld   b,h        ; get multipliers in DE/BC
  ld   c,l
  ld   hl,0        ; zero total
__Core__Mult_Loop:
  bit  0,c        ; lsb of shifter is non-zero
  jr   z,__Core__Mult_Shift
  add  hl,de        ; add adder to total
__Core__Mult_Shift:
  srl  b         ; shift BC right.
  rr   c
  ex   de,hl        ; shift DE left
  add  hl,hl
  ex   de,hl
  ld   a,b        ; loop back if BC is nonzero
  or   c
  jr   nz,__Core__Mult_Loop
  pop  de
  pop  bc
  ret


end1021:
 .dw end1022-$
 .db 'swap',0
 .db end1022-$
start1022:
  ex   de,hl

end1022:
 .dw end1023-$
 .db '.',0
 .db end1023-$
start1023:
  ex   de,hl


end1023:
 .dw end1024-$
 .db 'a>b',0
 .db end1024-$
start1024:
  ld   d,h
  ld   e,l


end1024:
 .dw end1025-$
 .db 'a>c',0
 .db end1025-$
start1025:
  ld   b,h
  ld   c,l



end1025:
 .dw end1026-$
 .db 'b>a',0
 .db end1026-$
start1026:
  ld   h,d
  ld   l,e


end1026:
 .dw end1027-$
 .db 'b>c',0
 .db end1027-$
start1027:
  ld   b,d
  ld   c,e



end1027:
 .dw end1028-$
 .db 'c>a',0
 .db end1028-$
start1028:
  ld   h,b
  ld   l,c


end1028:
 .dw end1029-$
 .db 'c>b',0
 .db end1029-$
start1029:
  ld   d,b
  ld   e,c



end1029:
 .dw end1030-$
 .db 'push',0
 .db end1030-$
start1030:
 push  hl


end1030:
 .dw end1031-$
 .db 'pop',0
 .db end1031-$
start1031:
 ex   de,hl
 pop  hl



end1031:
 .dw end1032-$
 .db 'a>r',0
 .db end1032-$
start1032:
 push  hl


end1032:
 .dw end1033-$
 .db 'r>a',0
 .db end1033-$
start1033:
 pop  hl



end1033:
 .dw end1034-$
 .db 'b>r',0
 .db end1034-$
start1034:
 push  de


end1034:
 .dw end1035-$
 .db 'r>b',0
 .db end1035-$
start1035:
 pop  de



end1035:
 .dw end1036-$
 .db 'c>r',0
 .db end1036-$
start1036:
 push  bc


end1036:
 .dw end1037-$
 .db 'r>c',0
 .db end1037-$
start1037:
 pop  bc




end1037:
 .dw end1038-$
 .db 'ab>r',0
 .db end1038-$
start1038:
 push  de
 push  hl


end1038:
 .dw end1039-$
 .db 'r>ab',0
 .db end1039-$
start1039:
 pop  hl
 pop  de



end1039:
 .dw end1040-$
 .db 'bc>r',0
 .db end1040-$
start1040:
 push  de
 push  bc


end1040:
 .dw end1041-$
 .db 'r>bc',0
 .db end1041-$
start1041:
 pop  bc
 pop  de



end1041:
 .dw end1042-$
 .db 'abc>r',0
 .db end1042-$
start1042:
 push  bc
 push  de
 push  hl


end1042:
 .dw end1043-$
 .db 'r>abc',0
 .db end1043-$
start1043:
 pop  hl
 pop  de
 pop  bc



end1043:
 .dw end1044-$
 .db '---',0
 .db end1044-$
start1044:
  dec  hl
  dec  hl


end1044:
 .dw end1045-$
 .db '--',0
 .db end1045-$
start1045:
  dec  hl


end1045:
 .dw end1046-$
 .db '++',0
 .db end1046-$
start1046:
  inc  hl


end1046:
 .dw end1047-$
 .db '+++',0
 .db end1047-$
start1047:
  inc  hl
  inc  hl


end1047:
 .dw end1048-$
 .db '0<',0
 .db $FF
start1048:
  bit  7,h
  ld   hl,$0000
  ret  z
  dec  hl
  ret


end1048:
 .dw end1049-$
 .db '0=',0
 .db $FF
start1049:
  ld   a,h
  or   l
  ld   hl,$0000
  ret  nz
  dec  hl
  ret


end1049:
 .dw end1050-$
 .db '2*',0
 .db end1050-$
start1050:
  add  hl,hl

end1050:
 .dw end1051-$
 .db '4*',0
 .db end1051-$
start1051:
  add  hl,hl
  add  hl,hl

end1051:
 .dw end1052-$
 .db '8*',0
 .db end1052-$
start1052:
  add  hl,hl
  add  hl,hl
  add  hl,hl

end1052:
 .dw end1053-$
 .db '16*',0
 .db end1053-$
start1053:
  add  hl,hl
  add  hl,hl
  add  hl,hl
  add  hl,hl


end1053:
 .dw end1054-$
 .db '2/',0
 .db end1054-$
start1054:
  sra  h
  rr   l

end1054:
 .dw end1055-$
 .db '4/',0
 .db $FF
start1055:
  sra  h
  rr   l
  sra  h
  rr   l
  ret

end1055:
 .dw end1056-$
 .db '8/',0
 .db $FF
start1056:
  sra  h
  rr   l
  sra  h
  rr   l
  sra  h
  rr   l
  ret

end1056:
 .dw end1057-$
 .db '16/',0
 .db $FF
start1057:
  sra  h
  rr   l
  sra  h
  rr   l
  sra  h
  rr   l
  sra  h
  rr   l
  ret


end1057:
 .dw end1058-$
 .db 'abs',0
 .db $FF
start1058:
  bit  7,h
  ret  z
  jr   __negate


end1058:
 .dw end1059-$
 .db '0-',0
 .db $FF
start1059:
__negate:
  ld   a,h
  cpl
  ld   h,a
  ld   a,l
  cpl
  ld   l,a
  inc  hl
  ret


end1059:
 .dw end1060-$
 .db 'bswap',0
 .db end1060-$
start1060:
  ld   a,l
  ld   l,h
  ld   h,a


end1060:
 .dw end1061-$
 .db 'not',0
 .db $FF
start1061:
  ld   a,h
  cpl
  ld   h,a
  ld   a,l
  cpl
  ld   l,a
  ret


end1061:
 .dw end1062-$
 .db 'strlen',0
 .db $FF
start1062:
  push  de
  ex   de,hl
  ld   hl,0
_SLNLoop:
  ld   a,(de)
  or   a
  jr   z,_SLNExit
  inc  de
  inc  hl
  jr   _SLNLoop
_SLNExit:
  pop  de
  ret


end1062:
 .dw end1063-$
 .db 'random',0
 .db $FF
start1063:
 ex   de,hl
 push  bc
    ld   hl,(seed1)
    ld   b,h
    ld   c,l
    add  hl,hl
    add  hl,hl
    inc  l
    add  hl,bc
    ld   (seed1),hl
    ld   hl,(seed2)
    add  hl,hl
    sbc  a,a
    and  %00101101
    xor  l
    ld   l,a
    ld   (seed2),hl
    add  hl,bc
    pop  bc
    ret

end1063:
 .dw 0

