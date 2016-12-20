.syntax unified
.cpu cortex-m4
.thumb

.text
	.global main
	.include "../src/seg.s"
	.include "../src/pin.s"
	student_id: .word 316320
	.align 4
	.equ SEG_DIN, 	0b1000
	.equ SEG_CS,	0b10000
	.equ SEG_CLK, 	0b100000

SegSend:
	push {r0, r1, r2, r3, r4, r5, r6, lr}
	add r0, r1, r0, lsl 8
	mov r1, 16
	ldr r2, =GPIOB_BRR
	ldr r3, =GPIOB_BSRR
	ldr r4, =SEG_CS
	ldr r5, =SEG_DIN
	ldr r6, =SEG_CLK
	str r4, [r2]
	mov r7, (1<<15)
SEG_SEND_LOOP:
	str r6, [r2]
	tst r0, r7
	beq SEG_NOT_SET
	str r5, [r3]
	b SEG_DONE
SEG_NOT_SET:
	str r5, [r2]
SEG_DONE:
	str r6, [r3]
	//lsr r0, 1
	lsr r7, 1
	subs r1, r1, 1
	bgt SEG_SEND_LOOP
SEG_SEND_END:
	str r4, [r3]
	pop {r0, r1, r2, r3, r4, r5, r6, pc}

Delay:
	push {r0, lr}
	ldr r0, =1000000
DELAY_LOOP:				// 4
	cbz r0, DELAY_END	// 1
	sub r0, 1			// 1
	b DELAY_LOOP		// 3
DELAY_END:
	pop {r0, pc}

main:
	mov r0, 0x2
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	mov r0, 0x540
	ldr r1, =GPIOB_MODER
	ldr r2, [r1]
	and r2, 0XFFFFF03F
	orr r2, r2, r0
	str r2, [r1]

	mov r0, 0XA80
	ldr r1, =GPIOB_OSPEEDR
	ldr r2, [r1]
	and r2, 0XFFFFF03F
	orr r2, r2, r0
	str r2, [r1]

	ldr r0, =SEG_ADDRESS_DECODE_MODE
	mov r1, 0xFF
	bl SegSend
	ldr r0, =SEG_ADDRESS_DISPLAY_TEST
	mov r1, 0
	bl SegSend
	ldr r0, =SEG_ADDRESS_SCAN_LIMIT
	mov r1, 6
	bl SegSend
	ldr r0, =SEG_ADDRESS_ITENSITY
	mov r1, 10
	bl SegSend
	ldr r0, =SEG_ADDRESS_SHUTDOWN
	mov r1, 1
	bl SegSend

	ldr r2, =student_id
	ldr r2, [r2]
	mov r3, 1
	mov r5, 10
LOOP:
	sdiv r4, r2, r5
	mul r4, r5
	sub r1, r2, r4
	mov r0, r3
	bl SegSend
	sdiv r2, r5
	add r3, 1
	cmp r3, 8
	bne LOOP
L: b L

