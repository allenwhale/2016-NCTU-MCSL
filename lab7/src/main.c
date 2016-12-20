/*
 * main.c
 *
 *  Created on: 2016¦~11¤ë29¤é
 *      Author: Administrator
 */
#include "stm32l476xx.h"
extern void Delay();
extern void GPIO_init();
extern void max7219_init();
extern void SegSend(int, int);
static int col[] = {5, 6, 7, 8};
static int row[] = {3, 4, 5, 6};
static GPIO_TypeDef* COL_GPIO = GPIOA;
static GPIO_TypeDef* ROW_GPIO = GPIOB;
int read_gpio(GPIO_TypeDef* gpio, int pin){
	return (gpio->IDR >> pin) & 1;
}
void set_gpio(GPIO_TypeDef* gpio, int pin){
	gpio->BSRR |= (1 << pin);
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

	//GPIOA->MODER &= 0xFFFC03FF;
	//GPIOA->PUPDR &= ~(((1<<8)-1)<<5);
	//GPIOA->PUPDR |= 0x15400;
}
void SysClock(int f){ // 1, 6, 10, 16, 40
	int plln, pllm, pllr;
	if(f == 0){
		plln = 8;
		pllm = 7;
		pllr = 1;
	}else if(f == 1){
		plln = 9;
		pllm = 2;
		pllr = 1;
	}else if(f == 2){
		plln = 10;
		pllm = 1;
		pllr = 1;
	}else if(f == 3){
		plln = 8;
		pllm = 0;
		pllr = 0;
	}else if(f == 4){
		plln = 20;
		pllm = 0;
		pllr = 0;
	}
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
	//while(!(((RCC->CFGR & RCC_CFGR_SWS_Msk)>>RCC_CFGR_SWS_Pos) == 3));
}
void SysClockToPLL(){
	RCC->CFGR &= ~(RCC_CFGR_SW_Msk);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}
int get_button(){
	int cnt = 0;
	for(int i=100;i--;)
		cnt += (GPIOC->IDR >> 13) ^ 1;
	return cnt > 50;
}
void lab71(){
	SysClockToPLL();
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(3 << (5 * 2));
	GPIOA->MODER |= (1 << (5 * 2));
	int f = 0;
	SysClock(f);
	while(1){
		if(get_button()){
			f = (f + 1) % 5;
			SysClock(f);
		}
		GPIOA->BSRR |= (1 << 5);
		Delay();
		GPIOA->BRR |= (1 << 5);
		Delay();
	}
}
void timer_init(){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
	TIM6->PSC = 999;
	TIM6->ARR = 3999;
	TIM6->EGR |= TIM_EGR_UG;
}
void timer_start(){
	TIM6->CR1 |= TIM_CR1_CEN;
}
void timer_stop(){
	TIM6->CR1 &= ~TIM_CR1_CEN;
}

void display(int data){
	SegSend(1, data % 10);
	data /= 10;
	SegSend(2, data % 10);
	data /= 10;
	SegSend(3, (data % 10) | (1 << 7));
	data /= 10;
	for(int i=4;i<=8;i++){
		if(data != 0){
			SegSend(i, data % 10);
			data /= 10;
		}else{
			SegSend(i, 15);
		}
	}
}
void lab72(){
#define TIME_SEC 127
	if(1 <= TIME_SEC && TIME_SEC <= 1000000){
		timer_init();
		timer_start();
		int time = 0;
		display(0);
		while(1){
			if(TIM6->CNT % 40 == 0){
				time++;
				if(time > TIME_SEC){
					break;
				}
				display(time);
			}

		}
		timer_stop();
	}else{
		display(0);
	}
	while(1);
}
int get_keypad_input_one_raw(int x, int y){
	GPIOA->OTYPER &= ~(1 << col[y]);
	int res = read_gpio(ROW_GPIO, row[x]);
	GPIOA->OTYPER |= (1 << col[y]);
	return res;
}
void lab73(){
	int psc_table[][3] = {
			{1529/2, 1361/2, 1213/2},
			{1145/2, 1020/2, 909/2},
			{809/2, 764/2}
	};
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(3);
	GPIOA->MODER |= 2;
	GPIOA->AFR[0] |= 1;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->PSC = 0;
	TIM2->ARR = 19;
	//TIM2->EGR |= TIM_EGR_UG;
	TIM2->CCMR1 = 0b01101100;
	TIM2->CCER = 1;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->CCR1 = 10;

	int last_x = -1, last_y = -1;
	int ccr1 = 10;
	while(1){
		int x = -1, y = -1;
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				if(get_keypad_input_one_raw(i, j)){
					x = i, y = j;
				}
			}
		}
		if(get_keypad_input_one_raw(0, 3)){
			if(ccr1 < 18) ccr1++;
		}
		if(get_keypad_input_one_raw(1, 3)){
			if(ccr1 > 2) ccr1--;
		}
		if(x != -1 && y != -1){
			if(x != last_x || y != last_y){
				//TIM2->CR1 &= ~TIM_CR1_CEN;
				TIM2->PSC = psc_table[x][y];
				//TIM2->EGR |= TIM_EGR_UG;
				//TIM2->CR1 |= TIM_CR1_CEN;
				TIM2->CCR1 = ccr1;
				last_x = x, last_y = y;
			}
		}else{
			last_x = last_y = -1;
			TIM2->CCR1 = 0;
		}
	}

}
int main(){
	GPIO_init();
	max7219_init();
	keypad_init();
	lab73();
	return 0;
}
