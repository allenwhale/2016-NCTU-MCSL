/*
 * asm.s
 *
 *  Created on: 2016¦~11¤ë29¤é
 *      Author: Administrator
 */
.syntax unified
.cpu cortex-m4
.thumb
text:
	.include "../src/pin.s"
	.include "../src/seg.s"
	.global Delay
	.global GPIO_init
	.global max7219_init
	.global SegSend
	.equ SEG_DIN, 	0b1000
	.equ SEG_CS,	0b10000
	.equ SEG_CLK, 	0b100000

Delay:
	push {r0, lr}
	ldr r0, =1000000
DELAY_LOOP:				// 4
	cbz r0, DELAY_END	// 1
	sub r0, 1			// 1
	b DELAY_LOOP		// 3
DELAY_END:
	pop {r0, pc}


SegSend:
	push {r0-r7, lr}
	add r0, r1, r0, lsl 8
	mov r1, 16
	ldr r2, =GPIOC_BRR
	ldr r3, =GPIOC_BSRR
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
	pop {r0-r7, pc}

GPIO_init:
	push {r0, r1, r2, lr}
	mov r0, 0x6
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	mov r0, 0x540
	ldr r1, =GPIOC_MODER
	ldr r2, [r1]
	and r2, 0XFFFFF03F
	orr r2, r2, r0
	str r2, [r1]

	mov r0, 0XA80
	ldr r1, =GPIOC_OSPEEDR
	ldr r2, [r1]
	and r2, 0XFFFFF03F
	orr r2, r2, r0
	str r2, [r1]

	ldr r1, =GPIOC_MODER
	ldr r2, [r1]
	and r2, 0xD3FFFFFF
	str r2, [r1]

	ldr r1, =GPIOC_PUPDR
	ldr r2, =0x04000000
	str r2, [r1]
	pop {r0, r1, r2, pc}

max7219_init:
	push {r0, r1, lr}
	ldr r0, =SEG_ADDRESS_DECODE_MODE
	mov r1, 0xFF
	bl SegSend
	ldr r0, =SEG_ADDRESS_DISPLAY_TEST
	mov r1, 0
	bl SegSend
	ldr r0, =SEG_ADDRESS_SCAN_LIMIT
	mov r1, 7
	bl SegSend
	ldr r0, =SEG_ADDRESS_ITENSITY
	mov r1, 10
	bl SegSend
	ldr r0, =SEG_ADDRESS_SHUTDOWN
	mov r1, 1
	bl SegSend
	pop {r0, r1, pc}
