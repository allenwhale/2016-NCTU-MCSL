	.equ GPIOA_MODER,	0x48000000
	.equ GPIOA_OTYPER,	0x48000004
	.equ GPIOA_OSPEEDR, 0x48000008
	.equ GPIOA_PUPDR, 	0x4800000C
	.equ GPIOA_IDR, 	0x48000010
	.equ GPIOA_ODR, 	0x48000014
	.equ GPIOA_BSRR,	0x48000018
	.equ GPIOA_LCKR, 	0x4800001C
	.equ GPIOA_AFRL, 	0x48000020
	.equ GPIOA_AFRH, 	0x48000024
	.equ GPIOA_BRR, 	0x48000028
	.equ GPIOA_ASCR, 	0x4800002C

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

	.equ GPIOD_MODER,	0x48000C00
	.equ GPIOD_OTYPER,	0x48000C04
	.equ GPIOD_OSPEEDR, 0x48000C08
	.equ GPIOD_PUPDR, 	0x48000C0C
	.equ GPIOD_IDR, 	0x48000C10
	.equ GPIOD_ODR, 	0x48000C14
	.equ GPIOD_BSRR,	0x48000C18
	.equ GPIOD_LCKR, 	0x48000C1C
	.equ GPIOD_AFRL, 	0x48000C20
	.equ GPIOD_AFRH, 	0x48000C24
	.equ GPIOD_BRR, 	0x48000C28
	.equ GPIOD_ASCR, 	0x48000C2C

	.equ GPIOE_MODER,	0x48001000
	.equ GPIOE_OTYPER,	0x48001004
	.equ GPIOE_OSPEEDR, 0x48001008
	.equ GPIOE_PUPDR, 	0x4800100C
	.equ GPIOE_IDR, 	0x48001010
	.equ GPIOE_ODR, 	0x48001014
	.equ GPIOE_BSRR,	0x48001018
	.equ GPIOE_LCKR, 	0x4800101C
	.equ GPIOE_AFRL, 	0x48001020
	.equ GPIOE_AFRH, 	0x48001024
	.equ GPIOE_BRR, 	0x48001028
	.equ GPIOE_ASCR, 	0x4800102C

	.equ GPIOF_MODER,	0x48001400
	.equ GPIOF_OTYPER,	0x48001404
	.equ GPIOF_OSPEEDR, 0x48001408
	.equ GPIOF_PUPDR, 	0x4800140C
	.equ GPIOF_IDR, 	0x48001410
	.equ GPIOF_ODR, 	0x48001414
	.equ GPIOF_BSRR,	0x48001418
	.equ GPIOF_LCKR, 	0x4800141C
	.equ GPIOF_AFRL, 	0x48001420
	.equ GPIOF_AFRH, 	0x48001424
	.equ GPIOF_BRR, 	0x48001428
	.equ GPIOF_ASCR, 	0x4800142C

	.equ GPIOG_MODER,	0x48001800
	.equ GPIOG_OTYPER,	0x48001804
	.equ GPIOG_OSPEEDR, 0x48001808
	.equ GPIOG_PUPDR, 	0x4800180C
	.equ GPIOG_IDR, 	0x48001810
	.equ GPIOG_ODR, 	0x48001814
	.equ GPIOG_BSRR,	0x48001818
	.equ GPIOG_LCKR, 	0x4800181C
	.equ GPIOG_AFRL, 	0x48001820
	.equ GPIOG_AFRH, 	0x48001824
	.equ GPIOG_BRR, 	0x48001828
	.equ GPIOG_ASCR, 	0x4800182C

	.equ GPIOH_MODER,	0x48001C00
	.equ GPIOH_OTYPER,	0x48001C04
	.equ GPIOH_OSPEEDR, 0x48001C08
	.equ GPIOH_PUPDR, 	0x48001C0C
	.equ GPIOH_IDR, 	0x48001C10
	.equ GPIOH_ODR, 	0x48001C14
	.equ GPIOH_BSRR,	0x48001C18
	.equ GPIOH_LCKR, 	0x48001C1C
	.equ GPIOH_AFRL, 	0x48001C20
	.equ GPIOH_AFRH, 	0x48001C24
	.equ GPIOH_BRR, 	0x48001C28
	.equ GPIOH_ASCR, 	0x48001C2C

	.equ RCC_CR, 		0x40021000
	.equ RCC_ICSCR, 	0x40021004
	.equ RCC_CFGR, 		0x40021008
	.equ RCC_PLLCFGR, 	0x4002100C
	.equ RCC_PLLSAI1CFGR, 0x40021010
	.equ RCC_PLLSAI2CFGR, 0x40021014
	.equ RCC_CIER, 		0x40021018
	.equ RCC_CIFR, 		0x4002101C
	.equ RCC_CICR, 		0x40021020
	.equ RCC_AHB1RSTR, 	0x40021024
	.equ RCC_AHB2RSTR, 	0x4002102C
	.equ RCC_AHB3RSTR, 	0x40021030
	.equ RCC_APB1RSTR1,	0x40021038
	.equ RCC_APB1RSTR2, 0x4002103C
	.equ RCC_APB2RSTR,	0x40021040
	.equ RCC_AHB1ENR, 	0x40021048
	.equ RCC_AHB2ENR, 	0x4002104C
	.equ RCC_AHB3ENR, 	0x40021050
	.equ RCC_APB1ENR1, 	0x40021058
	.equ RCC_APB1ENR2, 	0x4002105C
	.equ RCC_APB2ENR, 	0x40021060
	.equ RCC_AHB1SMENR, 0x40021068
	.equ RCC_AHB2SMENR, 0x4002106C
	.equ RCC_AHB3SMENR, 0x40021070
	.equ RCC_APB1SMENR1,0x40021078
	.equ RCC_APB1SMENR2,0x4002107C
	.equ RCC_APB2SMENR, 0x40021080
	.equ RCC_CCIPR, 	0x40021088
	.equ RCC_BDCR, 		0x40021090
	.equ RCC_CSR, 		0x40021094
