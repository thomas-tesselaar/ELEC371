
GetDataFromPort:
  subi  sp, sp, 24
  stw   ra, 20(sp)
  stw   r2, 16(sp)
  stw   r3, 12(sp)
  stw   r4, 8(sp)
  stw   r5, 4(sp)
  stw   r6, 0(sp)
  
  movia r4, PORT_DATA
  movia r5, PORT_STATUS
  
GetDataLoop:
  ldwio r6, 0(r5)
  andi  r6, r6, 0x20
  beq   r6, r0, GetDataLoop
  
  ldwio r6, 0(r4)
  stw   r6, 0(r2)
  
  addi  r2, r2, 4
  subi  r3, r3, 1
  
  bne   r3, r0, GetDataLoop

GetDataEnd:
  stw   ra, 20(sp)
  stw   r2, 16(sp)
  stw   r3, 12(sp)
  stw   r4, 8(sp)
  stw   r5, 4(sp)
  stw   r6, 0(sp)
  addi  sp, sp, 24
  
  ret
  
