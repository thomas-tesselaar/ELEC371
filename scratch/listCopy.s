.text
.global _start
.org 0x1000

_start:
	movia	sp, 0x007FFFFC
	movia r2, list1
	movia r3, list2
	ldw r4, n(r0)
	
	call ListCopy


_end:
	break


ListCopy:
	subi sp, sp, 20
	stw ra, 16(sp)
	stw r2, 12(sp) # input list
	stw r3, 8(sp)  # output list
	stw r4, 4(sp)  # number of elements in list
	stw r5, 0(sp)  # iterator

	
lc_loop:
	ldw r5, 0(r2)
	stw r5, 0(r3)
	
	addi r2, r2, 4
	addi r3, r3, 4
	subi r4, r4, 1
	bgt r4, r0, lc_loop
	
	ldw ra, 16(sp)
	ldw r2, 12(sp)
	ldw r3, 8(sp)
	ldw r4, 4(sp)
	ldw r5, 0(sp)
	addi sp, sp, 20
	
	ret


_subtraction:
	subi sp, sp, 12
	stw ra, 8(sp)
	stw r2, 4(sp)
	stw r3, 0(sp)
	
	sub r2, r2, r3
	
	ldw ra, 8(sp)
	ldw r2, 4(sp)
	ldw r3, 0(sp)
	addi sp, sp, 12
	
	ret
	
_multiplication:
	subi sp, sp, 12
	stw ra, 8(sp)
	stw r2, 4(sp)
	stw r3, 0(sp)
	
	mul r2, r2, r3
	
	ldw ra, 8(sp)
	ldw r2, 4(sp)
	ldw r3, 0(sp)
	addi sp, sp, 12
	
	ret
	
.org 0x2000
	
	
X: .word 0
Y: .word 3
Z: .word 4
n: .word 5
list1: .word -1, 1, 2, 7, 23
list2: .skip 20

	
	

