.syntax unified
.cpu cortex-m4
.thumb

.data
	result: .word 0
	max_size: .word 0

.text
	.global main
	m: .word 0x5e
	n: .word 0x60

GCD:
	mov r5, sp
	ldr r0, [sp]
	ldr r1, [sp, #4]
	cbz r0, GCD_RETURN_R1
	cbz r1, GCD_RETURN_R0
	and r2, r0, #1
	and r3, r1, #1
	orr r4, r2, r3
	cbz r4, GCD_RETURN_TYPE1
	cbz r2, GCD_RETURN_TYPE2
	cbz r3, GCD_RETURN_TYPE3
GCD_RETURN_TYPE4:
	cmp r0, r1
	bgt GCD_SWAP_END
	mov r2, r0
	mov r0, r1
	mov r1, r2
GCD_SWAP_END:	// a >= b
	sub r0, r1
	push {r0, r1, lr}
	bl GCD
	pop {r1, r2, pc}
GCD_RETURN_TYPE_4_2:	// a < b
	sub r2, r1, r0
	mov r1, r0
	mov r0, r2
	push {r0, r1, lr}
	bl GCD
	pop {r1, r2, pc}
GCD_RETURN_TYPE1:	// a % 2 == 0 && b % 2 == 0
	lsr r0, #1
	lsr r1, #1
	push {r0, r1, lr}
	bl GCD
	lsl r0, #1
	pop {r1, r2, pc}
GCD_RETURN_TYPE2:	// a % 2 == 0
	lsr r0, #1
	push {r0, r1, lr}
	bl GCD
	pop {r1, r2, pc}
GCD_RETURN_TYPE3:	// b % 2 == 0
	lsr r1, #1
	push {r0, r1, lr}
	bl GCD
	pop {r1, r2, pc}
GCD_RETURN_R0:
	bx lr
GCD_RETURN_R1:
	mov r0, r1
	bx lr

main:
	ldr r0, m
	ldr r1, n
	push {r0, r1}
	bl GCD
	pop {r1, r2}
	ldr r1, =result
	str r0, [r1]
	sub r5, sp, r5
	ldr r1, =max_size
	str r5, [r1]

L: b L
