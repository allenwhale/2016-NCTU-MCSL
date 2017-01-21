/*
 * main.c
 *
 *  Created on: 2017¦~1¤ë6¤é
 *      Author: Administrator
 */
#include "stm32l476xx.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "usart.h"
#define min(a, b) ((a) < (b) ? (a) : (b))
extern void GPIO_init();
extern void max7219_init();
extern void SegSend(int, int);
unsigned miliseconds10 = 0;
int targetValueA, targetValueB;
int lastCounterA = 0, lastCounterB = 0, lastTime = 0;
double scanDistance[100];
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
void SysTickConfig(int tick){
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	SysTick->LOAD = tick & SysTick_LOAD_RELOAD_Msk;
	SysTick->VAL = 0;
	SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk);
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
void SysTick_Handler(){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		miliseconds10++;
	}
}
double KA = 0.1;
double KB = 0.1;
int minPwmAF = 17, maxPwmAF = 23;
int minPwmBF = 21, maxPwmBF = 30;
int minPwmAB = 21, maxPwmAB = 24;
int minPwmBB = 22, maxPwmBB = 33;
void TIM7_IRQHandler(){
	if(TIM7->SR & TIM_SR_UIF){
		int minPwmA = targetValueA > 0 ? minPwmAF : minPwmAB;
		int minPwmB = targetValueB > 0 ? minPwmBF : minPwmBB;
		int maxPwmA = targetValueA > 0 ? maxPwmAF : maxPwmAB;
		int maxPwmB = targetValueB > 0 ? maxPwmBF : minPwmBB;
		int nowCounterA = TIM1->CNT;
		int nowCounterB = TIM3->CNT;
		int nowTime = miliseconds10;
		int xxx = targetValueB;
		lastTime = nowTime;
		int diffA = abs(nowCounterA - lastCounterA);
		int diffB = abs(nowCounterB - lastCounterB);
		if(diffA > 10000){
			diffA = 0xFFFF - diffA;
		}
		if(diffB > 10000){
			diffB = 0xFFFF - diffB;
		}
		if(targetValueA > 0){
			GPIOB->BSRR |= 1 << 4;
			GPIOB->BRR |= 1 << 5;
			targetValueA -= diffA;
			if(targetValueA < 0){
				int x = 0;
			}
		}else if(targetValueA < 0){
			GPIOB->BSRR |= 1 << 5;
			GPIOB->BRR |= 1 << 4;
			targetValueA += diffA;
			if(targetValueA > 0){
				//targetValueA = 0;
			}
		}
		if(targetValueB > 0){
			GPIOB->BSRR |= 1 << 2;
			GPIOB->BRR |= 1 << 3;
			targetValueB -= diffB;
			if(targetValueB < 0){
				int x = 0;
			}
		}else if(targetValueB < 0){
			GPIOB->BSRR |= 1 << 3;
			GPIOB->BRR |= 1 << 2;
			targetValueB += diffB;
			if(targetValueB > 0){
				//targetValueB = 0;
			}
		}
		double targetPwmA = abs(targetValueA) * KA;
		int pwmA = 0;
		if(targetPwmA < 1e-2)
			pwmA = 0;
		else if(targetPwmA < minPwmA)
			pwmA = minPwmA;
		else if(targetPwmA > maxPwmA)
			pwmA = maxPwmA;
		else
			pwmA = targetPwmA;
		TIM4->CCR1 = pwmA;
		lastCounterA = nowCounterA;
		double targetPwmB = abs(targetValueB) * KB;
		int pwmB = 0;
		if(targetPwmB < 1e-2)
			pwmB = 0;
		else if(targetPwmB < minPwmB)
			pwmB = minPwmB;
		else if(targetPwmB > maxPwmB)
			pwmB = maxPwmB;
		else
			pwmB = targetPwmB;
		TIM5->CCR1 = pwmB;
		lastCounterB = nowCounterB;
		if(pwmA == 0)
			GPIOA->BSRR |= 1 << 1;
		else
			GPIOA->BRR |= 1 << 1;
		if(pwmB == 0)
			GPIOA->BSRR |= 1 << 4;
		else
			GPIOA->BRR |= 1 << 4;
		TIM7->SR &= ~(TIM_SR_UIF);
	}
}
int get_button(){
	static int last = 0;
	int cnt = 0;
	for(int i=200;i--;)
		cnt += (GPIOC->IDR >> 13) & 1;
	cnt = cnt < 100;
	int res = 0;
	if(last == 0 &&cnt == 1)
		res = 1;
	else res = 0;
	last = cnt;
	return res;
}

double scan(){
	int start;
	GPIOC->BSRR |= (1 << 3);
	start = miliseconds10;
	while((miliseconds10 - start) <= 30);
	GPIOC->BRR |= (1 << 3);
	start = miliseconds10;
	while((GPIOC->IDR & (1 << 2)) && (miliseconds10 - start) <= 100);
	start = miliseconds10;
	while(!(GPIOC->IDR & (1 << 2)) && (miliseconds10 - start) <= 100);
	start = miliseconds10;
	while((GPIOC->IDR & (1 << 2)) && (miliseconds10 - start) <= 100);
	int duration = miliseconds10 - start;
	double distance = 17.0 * duration / 10.0;
	return distance;
}

void turn(int degree){
	static int minPwm = 530, maxPwm = 2350;
	int pwm = (double)degree / 180.0 * (maxPwm - minPwm) + minPwm;
	TIM2->CCR1 = pwm;
}

void front(int dist){
	while(dist){
		int go = min(dist, 300);
		targetValueA = targetValueB = go;
		dist -= go;
		lastCounterA = TIM1->CNT;
		lastCounterB = TIM3->CNT;
		NVIC_EnableIRQ(TIM7_IRQn);
		while(targetValueA || targetValueB);
		NVIC_DisableIRQ(TIM7_IRQn);
	}
}
void back(int dist){
	while(dist){
		int go = min(dist, 300);
		targetValueA = targetValueB = -go;
		dist -= go;
		NVIC_EnableIRQ(TIM7_IRQn);
		while(targetValueA || targetValueB);
		NVIC_DisableIRQ(TIM7_IRQn);
	}
}

void right(int degree){
	int go = (double)degree / 45.0 * 115.;
	targetValueA = -go;
	targetValueB = go;
	NVIC_EnableIRQ(TIM7_IRQn);
	while(targetValueA || targetValueB);
	NVIC_DisableIRQ(TIM7_IRQn);
}

void left(int degree){
	int go = (double)degree / 45.0 * 115.;
	targetValueA = go;
	targetValueB = -go;
	NVIC_EnableIRQ(TIM7_IRQn);
	while(targetValueA || targetValueB);
	NVIC_DisableIRQ(TIM7_IRQn);
}

void scanAll(){
	turn(0);
	for(int i=1000000;i--;);
	static int eachDegree = 5;
	for(int i=0;i<180/eachDegree;i++){
		turn(eachDegree * i);
		for(int j=200000;j--;);
		scanDistance[i] = scan();
		double x = scanDistance[i];
	}
}
void transmit(){
	char res[10000] ={0};
	for(int i=0;i<36;i++){
		char cat[10];
		sprintf(cat, "#%.1f", scanDistance[i]);
		strcat(res, cat);
	}
	USARTPrintString(USART3, res + 1);
}

void done(const char *cmd){
	USARTPrintString(USART3, "DONE#");
	USARTPrintString(USART3, cmd);
}

int main(){

	SCB->CPACR |= (0xF << 20);
	__DSB();
	__ISB();
	SysClock();
	GPIO_init();
	max7219_init();
	display(10, 2);
	SysTickConfig(1000);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	GPIOA->MODER &= ~(0b11 << (1 * 2));
	GPIOA->MODER |= (0b01 << (1 * 2));
	GPIOA->MODER &= ~(0b11 << (4 * 2));
	GPIOA->MODER |= (0b01 << (4 * 2));
	GPIOA->MODER &= ~(0b11 << (8 * 2));
	GPIOA->MODER &= ~(0b11 << (9 * 2));
	GPIOA->MODER |= (0b10 << (8 * 2));
	GPIOA->MODER |= (0b10 << (9 * 2));
	GPIOA->PUPDR &= ~(0b11 << (8 * 2));
	GPIOA->PUPDR &= ~(0b11 << (9 * 2));
	GPIOA->PUPDR |= (0b01 << (8 * 2));
	GPIOA->PUPDR |= (0b01 << (9 * 2));
	GPIOA->AFR[8 >> 3] |= (1 << ((8 & 7) * 4));
	GPIOA->AFR[9 >> 3] |= (1 << ((9 & 7) * 4));
	GPIOC->MODER &= ~(0b11 << (6 * 2));
	GPIOC->MODER &= ~(0b11 << (7 * 2));
	GPIOC->MODER |= (0b10 << (6 * 2));
	GPIOC->MODER |= (0b10 << (7 * 2));
	GPIOC->PUPDR &= ~(0b11 << (6 * 2));
	GPIOC->PUPDR &= ~(0b11 << (7 * 2));
	GPIOC->PUPDR |= (0b01 << (6 * 2));
	GPIOC->PUPDR |= (0b01 << (7 * 2));
	GPIOC->AFR[6 >> 3] |= (2 << ((6 & 7) * 4));
	GPIOC->AFR[7 >> 3] |= (2 << ((7 & 7) * 4));
	GPIOA->MODER &= ~(0b11 << (0 * 2));
	GPIOA->MODER |= (0b10 << (0 * 2));
	GPIOA->AFR[0 >> 3] = (2 << ((0 & 7) * 4));
	GPIOA->MODER &= ~(0b11 << (5 * 2));
	GPIOA->MODER |= (0b10 << (5 * 2));
	GPIOA->AFR[5 >> 3] |= (1 << ((5 & 7) * 4));
	TIM1->ARR = 0xFFFF;
	TIM1->PSC = 0;
	TIM1->CCMR1 |= 1;
	TIM1->CCMR1 |= 1 << 8;
	TIM1->SMCR = 3;
	TIM1->EGR |= TIM_EGR_UG;
	TIM1->CR1 |= TIM_CR1_CEN;
	TIM3->ARR = 0xFFFF;
	TIM3->PSC = 0;
	TIM3->CCMR1 |= 1;
	TIM3->CCMR1 |= 1 << 8;
	TIM3->SMCR = 3;
	TIM3->EGR |= TIM_EGR_UG;
	TIM3->CR1 |= TIM_CR1_CEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;
	TIM7->PSC = 20 - 1;
	TIM7->ARR = 20 - 1;
	TIM7->DIER |= TIM_DIER_TIE;
	TIM7->DIER |= TIM_DIER_UIE;
	TIM7->EGR |= TIM_EGR_UG;
	TIM7->CR1 |= TIM_CR1_CEN;
	//NVIC_EnableIRQ(TIM7_IRQn);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= ~(0b11 << (6 * 2));
	GPIOB->MODER |= (0b10 << (6 * 2));
	GPIOB->AFR[6 >> 3] = (2 << ((6 & 7) * 4));;
	GPIOB->MODER &= ~(0b11 << (4 * 2));
	GPIOB->MODER &= ~(0b11 << (5 * 2));
	GPIOB->MODER |= (0b01 << (4 * 2));
	GPIOB->MODER |= (0b01 << (5 * 2));
	GPIOB->MODER &= ~(0b11 << (2 * 2));
	GPIOB->MODER &= ~(0b11 << (3 * 2));
	GPIOB->MODER |= (0b01 << (2 * 2));
	GPIOB->MODER |= (0b01 << (3 * 2));
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	TIM4->PSC = 0;
	TIM4->ARR = 39;
	TIM4->CCMR1 = 0b01101100;
	TIM4->CCER = 1;
	TIM4->EGR |= TIM_EGR_UG;
	TIM4->CR1 |= TIM_CR1_CEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	TIM5->PSC = 0;
	TIM5->ARR = 39;
	TIM5->CCMR1 = 0b01101100;
	TIM5->CCER = 1;
	TIM5->EGR |= TIM_EGR_UG;
	TIM5->CR1 |= TIM_CR1_CEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->PSC = 10 - 1;
	TIM2->ARR = 20000 - 1;
	TIM2->CCMR1 = 0b01101100;
	TIM2->CCER = 1;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	targetValueB = 340;
	targetValueA = 340;
	GPIOC->MODER &= ~(0b11 << (13 * 2));
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= ~(0b11 << (10 * 2));
	GPIOB->MODER &= ~(0b11 << (11 * 2));
	GPIOB->MODER |= (0b10 << (10 * 2));
	GPIOB->MODER |= (0b10 << (11 * 2));
	GPIOB->OTYPER &= ~(1 << 10);
	GPIOB->OTYPER &= ~(1 << 11);
	GPIOB->PUPDR &= ~(0b11 << (10 * 2));
	GPIOB->PUPDR &= ~(0b11 << (11 * 2));
	GPIOB->AFR[10 >> 3] &= ~(0b1111 << ((10 & 7) << 2));
	GPIOB->AFR[10 >> 3] |= (7 << ((10 & 7) << 2));
	GPIOB->AFR[11 >> 3] &= ~(0b1111 << ((11 & 7) << 2));
	GPIOB->AFR[11 >> 3] |= (7 << ((11 & 7) << 2));
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART3RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USART3RST;
	USARTSetBaudRate(USART3, 10, 9600);
	USARTSetWordLength(USART3, 8);
	USARTSetStopBit(USART3, 0);
	USARTEnableRXTX(USART3, 1, 1);
	USARTEnable(USART3);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~(0b11 << (2 * 2));
	GPIOC->MODER &= ~(0b11 << (3 * 2));
	GPIOC->MODER |= (0b01 << (3 * 2));
	//front(1000);
	char cmd[100];
	while(1){
		USARTGetLine(USART3, cmd, 100);
		if(strncmp(cmd, "front", 5) == 0){
			int x;
			sscanf(cmd, "front %d", &x);
			front(x);
			done(cmd);
		}else if(strncmp(cmd, "back", 4) == 0){
			int x;
			sscanf(cmd, "back %d", &x);
			back(x);
			done(cmd);
		}else if(strncmp(cmd, "left", 4) == 0){
			int x;
			sscanf(cmd, "left %d", &x);
			left(x);
			done(cmd);
		}else if(strncmp(cmd, "right", 5) == 0){
			int x;
			sscanf(cmd, "right %d", &x);
			right(x);
			done(cmd);
		}else if(strncmp(cmd, "scan", 4) == 0){
			scanAll();
			transmit();
		}else if(strncmp(cmd, "transmit", 8) == 0){
			transmit();
		}
	}
	//		if(state)TIM2->CCR1 = 2350;
	//		else TIM2->CCR1 = 550;
	return 0;
}

