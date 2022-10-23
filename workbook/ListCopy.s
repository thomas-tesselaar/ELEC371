.text
.global _start
.org 0x0000
_start:
	movi 	sp, 0x7ffc
	movia r2, LIST1
	movia r3, LIST2
	ldw 	r4,	N(r0)
	
	call ListCopy
	
_end:
	break

ListCopy:
	subi	sp, sp, 20
	stw		ra, 16(sp)
	stw		r2, 12(sp)
	stw		r3, 8(sp)
	stw		r4, 4(sp)
	stw		r5, 0(sp)
	
lc_loop:
	ldw		r5, 0(r2)
	stw		r5, 0(r3)
	addi	r2, r2, 4
	addi	r3, r3, 4
	subi	r4, r4, 1
	
	bne r4, r0, lc_loop
lc_end:
	ldw		ra, 16(sp)
	ldw		r2, 12(sp)
	ldw		r3, 8(sp)
	ldw		r4, 4(sp)
	ldw		r5, 0(sp)
	addi	sp, sp, 20

	ret

.org 0x1000
LIST1:	.word 4, 6, 8, 9
LIST2:	.skip 16
N:	    .word 4
