setup:
  movia r2, IO_X_CONTROL
  movi  r2, 0x20  #bit 5 for local IE
  stwio r3, 0(r2)
  movia r2, IO_Y_CONTROL
  movi  r2, 0x  #bit for local IE
  stwio r3, 0(r2)
  movia r2, IO_Z_CONTROL
  movi  r2, 0x  #bit for local IE
  stwio r3, 0(r2)
  movia r2, 0x80402000
  wrctl ienable, r2
  movi  r2, 1
  wrctl status, r2


#--------------------------------------

isr:
  subi  ea, ea, 4
  subi  sp, sp, 12
  stw   ra, 8(sp)
  stw   r2, 4(sp)
  stw   r3, 0(sp)
  rdctl r2, ipending
check_handle_X:
  movia r3, 0x80000000
  and   r3, r2, r3
  beq   r3, r0, check_handle_Y
  call HandleX
check_handle_X:
  movia r3, 0x00400000
  and   r3, r2, r3
  beq   r3, r0, check_handle_Z
  call HandleY
check_handle_Z:
  andi  r3, r2, 0x2000
  beq   r3, r0, check_handle_Y
  call HandleZ
isr_end:
  ldw   ra, 8(sp)
  ldw   r2, 4(sp)
  ldw   r3, 0(sp)
  addi  sp, sp, 12
  eret
  
  
