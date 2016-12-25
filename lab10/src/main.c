/*
 * main.c
 *
 *  Created on: 2016年12月23日
 *      Author: Administrator
 */
#include "stm32l476xx.h"
#include "usart.h"
#include "delay.h"
#include "adc.h"
#include <stdio.h>
#include <string.h>
double resistor = 0;
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
		ADCStartConversion(ADC1);
	}
}
void USARTInit(){
	//PA9 PA10
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(0b11 << (9 * 2));
	GPIOA->MODER &= ~(0b11 << (10 * 2));
	GPIOA->MODER |= (0b10 << (9 * 2));
	GPIOA->MODER |= (0b10 << (10 * 2));
	GPIOA->OTYPER &= ~(1 << 9);
	GPIOA->OTYPER &= ~(1 << 10);
	GPIOA->PUPDR &= ~(0b11 << (9 * 2));
	GPIOA->PUPDR &= ~(0b11 << (10 * 2));
	GPIOA->AFR[9 >> 3] &= ~(0b1111 << ((9 & 7) << 2));
	GPIOA->AFR[9 >> 3] |= (7 << ((9 & 7) << 2));
	GPIOA->AFR[10 >> 3] &= ~(0b1111 << ((10 & 7) << 2));
	GPIOA->AFR[10 >> 3] |= (7 << ((10 & 7) << 2));
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
	USARTSetBaudRate(USART1, 4, 9600);
	USARTSetWordLength(USART1, 8);
	USARTSetStopBit(USART1, 0);
	USARTEnableRXTX(USART1, 1, 1);
	USARTEnable(USART1);
}
void ADCInit(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER |= 0b11;
	GPIOC->ASCR |= 1;
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	ADCResolution(ADC1, 0); // 12 bits
	ADCContinuousConversion(ADC1, 0); // enable continuous conversion
	ADCDataAlign(ADC1, 0); // set right align
	ADCCommonDualMode(0); // independent mode
	ADCCommonClockMode(1); // hclk / 1
	ADCCommonPrescaler(0); //div 1
	ADCCommonDMAMode(0); // disable dma
	ADCCommonDelayTwoSampling(0b0100); // 5 adc clk cycle
	ADCChannel(ADC1, 1, 1, 2); // channel 1, rank 1, 12.5 adc clock cycle
	ADCWakeup(ADC1);
	ADCInterrupt(ADC1, ADC_IER_EOCIE, 1);
	NVIC_EnableIRQ(ADC1_2_IRQn);
	ADCEnable(ADC1);
}
int get_user_button(){
	int cnt = 0;
	for(int i=200;i--;)
		cnt += (GPIOC->IDR >> 13) & 1;
	return cnt < 100;
}
void lab101(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~(0b11 << (13 * 2));
	int last = 0;
	while(1){
		int x = get_user_button();
		if(x == 1 && last == 0){
			USARTPrintString(USART1, "Hello World!");
		}
		last = x;
	}
}
void lab102(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~(0b11 << (13 * 2));
	int last = 0;
	while(1){
		int x = get_user_button();
		if(x == 1 && last == 0){
			char s[20];
			sprintf(s, "%f\n", resistor);
			USARTPrintString(USART1, s);
		}
		last = x;
	}
}
void PrintResistorValue(){
	char s[20];
	USARTPrintString(USART1, "\33[2K\r");
	sprintf(s, "%f", resistor);
	USARTPrintString(USART1, s);
}
void lab103(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(0b11 << (5 * 2));
	GPIOA->MODER |= (0b01 << (5 * 2));
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->PSC = 2000 - 1;
	TIM2->ARR = 1000 - 1;
	TIM2->DIER |= TIM_DIER_TIE;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	char s[200];
	while(1){
		USARTPrintString(USART1, ">");
		int len = USARTGetLine(USART1, s, 200);
		if(!strcmp(s, "showid")){
			USARTPrintString(USART1, "0316320\r\n");
		}else if(!strcmp(s, "light")){
			NVIC_EnableIRQ(TIM2_IRQn);
			while(USARTGetData(USART1) != 'q');
			NVIC_DisableIRQ(TIM2_IRQn);
			USARTPrintString(USART1, "\r\n");
		}else if(!strcmp(s, "led on")){
			GPIOA->BSRR = 1 << 5;
		}else if(!strcmp(s, "led off")){
			GPIOA->BRR = 1 << 5;
		}else if(len){
			USARTPrintString(USART1, "Unknown Command: ");
			USARTPrintString(USART1, s);
			USARTPrintString(USART1, "\r\n");
		}
	}
}
void TIM2_IRQHandler(){
	if(TIM2->SR & TIM_SR_UIF){
		PrintResistorValue();
		TIM2->SR &= ~(TIM_SR_UIF);
	}
}
double ADCValueToVoltage(int value){
	return (5. / 4095.) * value;
}
void ADC1_2_IRQHandler(){
	if(ADC1->ISR & ADC_ISR_EOC){
		ADC1->ISR &= ~ADC_ISR_EOC;
		resistor = ADCValueToVoltage(ADCGetValue(ADC1));
		resistor = (5. - resistor) * 2000 / resistor;
	}
}

int main(){
	SCB->CPACR |= (0xF << 20);
	__DSB();
	__ISB();
	USARTInit();
	ADCInit();
	SysTickConfig(40000);
	lab103();
	while(1);
	return 0;
}
