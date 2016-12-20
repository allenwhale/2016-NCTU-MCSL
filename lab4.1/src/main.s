.syntax unified
.cpu cortex-m4
.thumb
.data
	leds: .byte	0
.text
	.global main
	.equ RCC_AHB2ENR, 	0x4002104C
	.equ GPIOB_MODER,	0x48000400
	.equ GPIOB_OTYPER,	0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR, 	0x4800040C
	.equ GPIOB_IDR, 	0x48000410
	.equ GPIOB_ODR, 	0x48000414
	.equ GPIOB_BSRR,	0x48000418
	.equ GPIOB_LCKR, 	0x4800041C
	.equ GPIOB_AFRL, 	0x48000420
	.equ GPIOB_AFRH, 	0x48000424
	.equ GPIOB_BRR, 	0x48000428
	.equ GPIOB_ASCR, 	0x4800042C

	.equ GPIOC_MODER,	0x48000800
	.equ GPIOC_OTYPER,	0x48000804
	.equ GPIOC_OSPEEDR, 0x48000808
	.equ GPIOC_PUPDR, 	0x4800080C
	.equ GPIOC_IDR, 	0x48000810
	.equ GPIOC_ODR, 	0x48000814
	.equ GPIOC_BSRR,	0x48000818
	.equ GPIOC_LCKR, 	0x4800081C
	.equ GPIOC_AFRL, 	0x48000820
	.equ GPIOC_AFRH, 	0x48000824
	.equ GPIOC_BRR, 	0x48000828
	.equ GPIOC_ASCR, 	0x4800082C

Delay:
	push {r0, lr}
	ldr r0, =1000
	//ldr r0, =2
DELAY_LOOP:				// 4
	cbz r0, DELAY_END	// 1
	sub r0, 1			// 1
	mov r4, r0
	bl GetButton
	cbnz r3, CHECK_BUTTON_END
	cbz r0, CHECK_BUTTON_END
	eor r2, 1
CHECK_BUTTON_END:
	mov r3, r0
	mov r0, r4
	b DELAY_LOOP		// 3
DELAY_END:
	pop {r0, pc}

DisplayLed:
	push {r1, r2, lr}
	mov r1, r0
	eor r1, 0xFFFFFFFF
	ldr r2, =GPIOB_ODR
	str r1, [r2]
	pop {r1, r2, pc}

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

main:

	mov r0, 0x6
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]

	mov r0, 5440
	ldr r1, =GPIOB_MODER
	ldr r2, [r1]
	and r2, 0xFFFFC03F
	orr r2, r2, r0
	str r2, [r1]

	mov r0, 10880
	ldr r1, =GPIOB_OSPEEDR
	str r0, [r1]

	ldr r1, =GPIOC_MODER
	ldr r2, [r1]
	and r2, 0xD3FFFFFF
	str r2, [r1]

	ldr r1, =GPIOC_PUPDR
	ldr r2, =0x04000000
	str r2, [r1]

	mov r0, 12	// led
	mov r1, 0	// dir( 0-> left, 1 -> right)
	mov r2, 1 	// enable
	mov r3, 1	// last button state
LOOP:
	bl Delay
	cmp r2, 0
	beq LOOP
	bl DisplayLed
	cbz r1, MOVE_LEFT
MOVE_RIGHT:
	lsr r0, 1
	cmp r0, 12
	bne LOOP
	mov r1, 0
	b LOOP
MOVE_LEFT:
	lsl r0, 1
	cmp r0, 192
	bne LOOP
	mov r1, 1
	b LOOP


L: b L
