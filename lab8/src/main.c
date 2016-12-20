/*
 * main.c
 *
 *  Created on: 2016¦~12¤ë7¤é
 *      Author: Administrator
 */
#include "stm32l476xx.h"
extern void GPIO_init();
extern void max7219_init();
extern void SegSend(int, int);
static int col[] = {5, 6, 7, 8};
static int row[] = {3, 4, 5, 6};
static GPIO_TypeDef* COL_GPIO = GPIOA;
static GPIO_TypeDef* ROW_GPIO = GPIOB;
static int stableKeyValue= 0, keyValue = 0, keyCnt = 0;
static int status = 0;
#define WAIT_FOR_INPUT 0
#define COUNTDOWN 1
#define SINGING 2
static int now_col = 0;
static const int keypad[][4] = {
		{1, 2, 3, 10},
		{4, 5, 6, 11},
		{7, 8, 9, 12},
		{15, 0, 14, 13}
};
int read_gpio(GPIO_TypeDef* gpio, int pin){
	return (gpio->IDR >> pin) & 1;
}
void set_gpio(GPIO_TypeDef* gpio, int pin){
	gpio->BSRR |= (1 << pin);
}
void reset_gpio(GPIO_TypeDef* gpio, int pin){
	gpio->BRR |= (1 << pin);
}
void keypad_init(){
	// PB 3456 ROW 1234 INPUT
	// PA 5678 COL 1234 OUTPUT
	// SET keypad GPIO OUTPUT
	// PA 5678 - COL 1234 OUTPUT
	RCC->AHB2ENR = RCC->AHB2ENR | 0x1;
	//Set PA5 ~ PA8 as output mode
	GPIOA->MODER = GPIOA->MODER & 0xFFFC03FF;
	GPIOA->MODER = GPIOA->MODER | 0x15400;
	// SET keypad GPIO INPUT
	// PB 3456 - ROW 1234 INPUT
	RCC->AHB2ENR = RCC->AHB2ENR | 0x2;
	//Set PB3 ~ PB6 as INPUT mode
	GPIOB->MODER = GPIOB->MODER & 0xFFFFC03F;
	GPIOB->MODER = GPIOB->MODER | 0x0;
	//set PB3 ~ PB6 is Pull-Down
	GPIOB->PUPDR = GPIOB->PUPDR & 0xFFFFC03F;
	GPIOB->PUPDR = GPIOB->PUPDR | 0x2A80;
	//Set PA5 ~ PA8 as high speed mode
	GPIOA->OSPEEDR = GPIOA->OSPEEDR | 261120;

	GPIOA->OTYPER |= 0x1e0;
	for(int i=5;i<=8;i++)
		set_gpio(GPIOA, i);
}
void SysClock(){
	int plln = 20;
	int pllm = 1;
	int pllr = 1;
	plln <<= RCC_PLLCFGR_PLLN_Pos;
	pllm <<= RCC_PLLCFGR_PLLM_Pos;
	pllr <<= RCC_PLLCFGR_PLLR_Pos;
	RCC->CFGR &= ~(RCC_CFGR_SW_Msk);
	RCC->CR &= ~(RCC_CR_PLLON);
	while((RCC->CR & RCC_CR_PLLRDY));
	//config
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN_Msk);
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk);
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLR_Msk);
	RCC->PLLCFGR |= plln;
	RCC->PLLCFGR |= pllm;
	RCC->PLLCFGR |= pllr;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;
	RCC->CR |= RCC_CR_PLLON;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}
void SysTickConfig(int tick){
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	SysTick->LOAD = tick & SysTick_LOAD_RELOAD_Msk;
	SysTick->VAL = 0;
	SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk);
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
int get_user_button(){
	int cnt = 0;
	for(int i=100;i--;){
		cnt += read_gpio(GPIOC, 13);
	}
	return cnt < 50;
}

int display(int data, int num_digs){
	for(int i=1;i<=num_digs;i++){
		SegSend(i, data % 10);
		data /= 10;
	}
	for(int i=num_digs+1;i<=8;i++){
		data /= 10;
		SegSend(i, 15);
	}
	if(data != 0)
		return -1;
	return 0;
}
void SysTick_Handler_lab81(){
	if(GPIOA->ODR & GPIO_ODR_OD4_Msk){
		GPIOA->BRR |= GPIO_BRR_BR4_Msk;
	}else{
		GPIOA->BSRR |= GPIO_BSRR_BS4_Msk;
	}
}
void SysTick_Handler_lab82(){
	GPIOA->OTYPER |= (1 << col[now_col]);
	now_col = (now_col + 1) % 4;
	GPIOA->OTYPER &= ~(1 << col[now_col]);
}
void SysTick_Handler_lab83(){
	if(status == WAIT_FOR_INPUT){
		GPIOA->OTYPER |= (1 << col[now_col]);
		now_col = (now_col + 1) % 4;
		GPIOA->OTYPER &= ~(1 << col[now_col]);
	}else if(status == COUNTDOWN){
		int tmp = stableKeyValue;
		stableKeyValue--;
		display(stableKeyValue, 2);
		if(stableKeyValue == 0){
			status = SINGING;
			while(1){
				if(status != SINGING)break;
				TIM2->PSC = 500;
				TIM2->CCR1 = 10;
			}
			TIM2->CCR1 = 0;
		}
	}
}
void SysTick_Handler(){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		//SysTick_Handler_lab81();
		//SysTick_Handler_lab82();
		SysTick_Handler_lab83();
	}
}
void lab81(){
	SysClock();
	SysTickConfig(5000000);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODE4_Msk);
	GPIOA->MODER |= (1 << GPIO_MODER_MODE4_Pos);
	while(1){
		if(get_user_button()){
			SysTick->CTRL ^= SysTick_CTRL_ENABLE_Msk;
		}
	}
}
void EXTISetup(){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI3_PB;
	SYSCFG->EXTICR[1] = SYSCFG_EXTICR2_EXTI4_PB | SYSCFG_EXTICR2_EXTI5_PB | SYSCFG_EXTICR2_EXTI6_PB;
	SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI13_PC;
	EXTI->IMR1 |= EXTI_IMR1_IM3;
	EXTI->IMR1 |= EXTI_IMR1_IM4;
	EXTI->IMR1 |= EXTI_IMR1_IM5;
	EXTI->IMR1 |= EXTI_IMR1_IM6;
	EXTI->IMR1 |= EXTI_IMR1_IM13;
	EXTI->FTSR1 |= EXTI_FTSR1_FT3;
	EXTI->FTSR1 |= EXTI_FTSR1_FT4;
	EXTI->FTSR1 |= EXTI_FTSR1_FT5;
	EXTI->FTSR1 |= EXTI_FTSR1_FT6;
	EXTI->RTSR1 |= EXTI_RTSR1_RT13;
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
	NVIC_SetPriority(SysTick_IRQn, 5);
	display(NVIC_GetPriority(EXTI15_10_IRQn)*100+NVIC_GetPriority(SysTick_IRQn), 4);
}

void EXTIKeypadHandler(int r){
	if(status != WAIT_FOR_INPUT)return;
	int nowKey = keypad[r][(now_col + 3) % 4];
	if(nowKey == keyValue){
		keyCnt++;
	}else{
		keyCnt = 0;
	}
	keyValue = nowKey;
	if(keyCnt >= 5){
		keyCnt = 5;
		display(stableKeyValue = keyValue, 2);
	}
}
void EXTI15_10_IRQHandler(){
	if(EXTI->PR1 & EXTI_PR1_PIF13){
		if(status == WAIT_FOR_INPUT){
			status = COUNTDOWN;
			SysTickConfig(10000000);
		}else if(status == SINGING){
			status = WAIT_FOR_INPUT;
			SysTickConfig(4000);
			TIM2->CCR1 = 0;
		}
		EXTI->PR1 = EXTI_PR1_PIF13;
	}
}
void EXTI9_5_IRQHandler(){
	if(EXTI->PR1 & EXTI_PR1_PIF5_Msk){
		EXTIKeypadHandler(2);
		EXTI->PR1 = EXTI_PR1_PIF5_Msk;
	}
	if(EXTI->PR1 & EXTI_PR1_PIF6_Msk){
		EXTIKeypadHandler(3);
		EXTI->PR1 = EXTI_PR1_PIF6_Msk;
	}
}
void EXTI3_IRQHandler(){
	if(EXTI->PR1 & EXTI_PR1_PIF3_Msk){
		EXTIKeypadHandler(0);
		EXTI->PR1 = EXTI_PR1_PIF3_Msk;
	}
}
void EXTI4_IRQHandler(){
	if(EXTI->PR1 & EXTI_PR1_PIF4_Msk){
		EXTIKeypadHandler(1);
		EXTI->PR1 = EXTI_PR1_PIF4_Msk;
	}
}
void lab82(){
	SysClock();
	max7219_init();
	keypad_init();
	SysTickConfig(4000);
	EXTISetup();
	while(1){
		//display(keyValue, 2);
	}
}
void pwm_init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(3);
	GPIOA->MODER |= 2;
	GPIOA->AFR[0] |= 1;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->PSC = 500;
	TIM2->ARR = 19;
	TIM2->CCMR1 = 0b01101100;
	TIM2->CCER = 1;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->CCR1 = 0;
}
void lab83(){
	SysClock();
	max7219_init();
	keypad_init();
	SysTickConfig(4000);
	EXTISetup();
	pwm_init();
	status = WAIT_FOR_INPUT;
}
int main(){
	GPIO_init();
	lab83();
	while(1);
	return 0;
}

