/*
 * delay.h
 *
 *  Created on: 2016�~12��16��
 *      Author: Administrator
 */

#ifndef DELAY_H_
#define DELAY_H_
#include "stm32l476xx.h"
#define init_timer(x) { \
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; \
	TIM2->PSC = (x) - 1; \
	TIM2->ARR = UINT16_MAX; \
	TIM2->EGR |= TIM_EGR_UG; \
	TIM2->CR1 |= TIM_CR1_CEN; \
}
#define stop_timer() { \
	TIM2->CR1 &= ~TIM_CR1_CEN; \
}
#define delay_ms(ms) { \
	init_timer(10000000 / 1000); \
	while(TIM2->CNT < ms); \
	stop_timer(); \
}
#define delay_us(us) { \
	init_timer(10000000 / 1000000); \
	while(TIM2->CNT < us); \
	stop_timer(); \
}

#endif /* DELAY_H_ */
