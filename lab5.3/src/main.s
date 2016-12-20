.syntax unified
.cpu cortex-m4
.thumb

.text
	.global main
	.include "../src/seg.s"
	.include "../src/pin.s"
	.align 4
	.equ SEG_DIN, 	0b1000
	.equ SEG_CS,	0b10000
	.equ SEG_CLK, 	0b100000

SegSend:
	push {r0, r1, r2, r3, r4, r5, r6, lr}
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

GetButton:
	push {r1, r2, r3, lr}	// 5
	mov r0, 0				// 1
	mov r1, 400				// 1
	ldr r2, =GPIOC_IDR		// 2
GET_BUTTON_LOOP:
	cbz r1, GET_BUTTON_END	// 1
	sub r1, 1				// 1
	ldr r3, [r2]			// 2
	tst r3, 0x2000			// 1
	beq GET_BUTTON_LOOP		// 3
	add r0, 1				// 1
	b GET_BUTTON_LOOP		// 3
GET_BUTTON_END:
	cmp r0, 200				// 1
	bgt GET_BUTTON_HIGH		// 3
	mov r0, 0				// 1
	pop {r1, r2, r3, pc}	// 5
GET_BUTTON_HIGH:
	mov r0, 1				// 1
	pop {r1, r2, r3, pc}	// 5

DisplayNum:
	push {r0, r1, r2, r3, r4, r5, lr}
	cmp r0, -1
	bne NORMAL_DISPLAY
	mov r0, 1
	mov r1, SEG_DATA_DECODE_1
	bl SegSend
	mov r0, 2
	mov r1, SEG_DATA_DECODE_DASH
	bl SegSend
	mov r0, 3
	b DISPLAY_BLANK_LOOP
NORMAL_DISPLAY:
	mov r2, r0
	mov r3, 10
	mov r0, 1
DISPLAY_LOOP:
	sdiv r4, r2, r3
	mul r4, r3
	sub r1, r2, r4
	bl SegSend
	sdiv r2, r3
	add r0, 1
	cmp r0, 9
	beq DISPLAY_LOOP_END
	cbz r2, DISPLAY_LOOP_END
	b DISPLAY_LOOP
DISPLAY_LOOP_END:
DISPLAY_BLANK_LOOP:
	cmp r0, 9
	beq DISPLAY_BLANK_LOOP_END
	ldr r1, =SEG_DATA_DECODE_BLANK
	bl SegSend
	add r0, 1
	b DISPLAY_BLANK_LOOP
DISPLAY_BLANK_LOOP_END:
	pop {r0, r1, r2, r3, r4, r5, pc}

main:
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
	mov r0, 0
	bl DisplayNum

	mov r2, 0
	mov r3, 1
	mov r4, 1 	// last button
	mov r5, 0 	// high count
	mov r7, 0 	// just reset
LOOP:
	bl GetButton
	cbnz r4, CHECK_BUTTON_HIGH
	cbz r0, CHECK_BUTTON_HIGH
	mov r4, r0
	cbnz r7, SKIP
	b DISPLAY_NEXT
SKIP:
	mov r7, 0
	b LOOP
CHECK_BUTTON_HIGH:
	cmp r0, 1
	beq CHECK_BUTTON_END
	cmp r4, 1
	beq CHECK_BUTTON_END
	add r5, 1
	cmp r5, 1000
	bge RESET
	b LOOP
CHECK_BUTTON_END:
	mov r4, r0
	mov r5, 0
	b LOOP
DISPLAY_NEXT:
	cmp r2, -1
	beq LOOP
	add r6, r2, r3
	mov r2, r3
	mov r3, r6
	ldr r6, =100000000
	cmp r2, r6
	blt CHECK_NUM_LIMIT_END
	mov r2, -1
CHECK_NUM_LIMIT_END:
	//display
	mov r0, r2
	bl DisplayNum
	b LOOP
RESET:
	mov r5, 0
	mov r2, 0
	mov r3, 1
	//display
	mov r0, r2
	bl DisplayNum
	mov r7, 1
	b LOOP

L: b L

