.syntax unified
.cpu cortex-m4
.thumb
.data
	password: .byte 0
.text
	.global main
	.include "../src/pin.s"
Delay:
	push {r0, lr}
	ldr r0, =500000
DELAY_LOOP:				// 4
	cbz r0, DELAY_END	// 1
	sub r0, 1			// 1
	b DELAY_LOOP		// 3
DELAY_END:
	pop {r0, pc}


GetButton:
	push {r1, r2, r3, lr}	// 5
	mov r0, 0				// 1
	mov r1, 4				// 1
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
	cmp r0, 2				// 1
	bgt GET_BUTTON_HIGH		// 3
	mov r0, 0				// 1
	pop {r1, r2, r3, pc}	// 5
GET_BUTTON_HIGH:
	mov r0, 1				// 1
	pop {r1, r2, r3, pc}	// 5

GetPassword:
	push {lr}
	ldr r0, =GPIOA_IDR
	ldr r0, [r0]
	lsr r0, 6
	and r0, 0xF
	pop {pc}

DisplayLed:
	push {r1, r2, lr}
	mov r1, r0
	eor r1, 0xFFFFFFFF
	ldr r2, =GPIOB_ODR
	str r1, [r2]
	pop {r1, r2, pc}

main:
	mov r0, 0x7
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

	mov r0, 0xFFF00FFF
	ldr r1, =GPIOA_MODER
	ldr r2, [r1]
	and r2, r0
	str r2, [r1]

	mov r0, 0x55000
	ldr r1, =GPIOA_PUPDR
	ldr r2, [r1]
	and r2, 0xFFF00FFF
	orr r2, r0
	str r2, [r1]
	mov r0, 0x0
	bl DisplayLed
	ldr r1, =password
	ldr r1, [r1]
LOOP:
	bl GetButton
	cmp r0, 0
	bne LOOP
	bl GetPassword
	cmp r0, r1
	bne PASSWORD_WRONG
	mov r0, 0x78
	bl DisplayLed
	bl Delay
	mov r0, 0x0
	bl DisplayLed
	bl Delay
	mov r0, 0x78
	bl DisplayLed
	bl Delay
	mov r0, 0x0
	bl DisplayLed
	bl Delay
	mov r0, 0x78
	bl DisplayLed
	bl Delay
	mov r0, 0x0
	bl DisplayLed
	b LOOP
PASSWORD_WRONG:
	mov r0, 0x78
	bl DisplayLed
	bl Delay
	mov r0, 0x0
	bl DisplayLed
	b LOOP
L: b L
