
ISR:
  subi ea, ea, 4
  subi sp, sp, 12
  stw ra, 8(sp)
  stw r2, 4(sp)
  stw r3, 0(sp)

  rdctl r2, ipending

CheckIntX:
  andi r3, r2, 0x10
  beq r3, r0, CheckIntY
  Call HandleIntX
  
CheckIntY:
  andi r3, r2, 0x20
  beq r3, r0, ISR_end
  Call HandleIntY
  
ISR_end:
  ldw ra, 8(sp)
  ldw r2, 4(sp)
  ldw r3, 0(sp)
  addi sp, sp 12
  
  eret
