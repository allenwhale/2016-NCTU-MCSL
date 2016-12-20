.syntax unified
.cpu cortex-m4
.thumb
.data
	result: .byte 0

.text
	.global main
	.equ X, 0x55AB
	.equ Y, 0xAA55



hamm:
	movs r3, #1
	movs r4, #0
	eor r0, r1
hamm_loop:
	cmp r0, #0
	beq hamm_end
	movs r2, r0
	and r2, r3
	cmp r2, #0
	beq not_increase_result
	add r4, #1
not_increase_result:
	asr r0, #1
	b hamm_loop
hamm_end:
	bx lr

main:
	/*
	movs r0, #X => error
	X cant exceed 255
	use `ldr r0, =X`
	*/
	ldr r0, =X
	ldr r1, =Y
	bl hamm
	ldr r0, =result
	str r4, [r0]

L: b L
