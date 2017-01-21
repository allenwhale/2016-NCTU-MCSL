/*
 * delay.h
 *
 *  Created on: 2016¦~12¤ë16¤é
 *      Author: Administrator
 */

#ifndef DELAY_H_
#define DELAY_H_
#include "stm32l476xx.h"
#define init_timer(x) { \
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN; \
	TIM3->PSC = (x) - 1; \
	TIM3->ARR = UINT16_MAX; \
	TIM3->EGR |= TIM_EGR_UG; \
	TIM3->CR1 |= TIM_CR1_CEN; \
}
#define stop_timer() { \
	TIM3->CR1 &= ~TIM_CR1_CEN; \
}
#define delay_ms(ms) { \
	init_timer(10000000 / 1000); \
	while(TIM3->CNT < ms); \
	stop_timer(); \
}
#define delay_us(us) { \
	init_timer(10000000 / 1000000); \
	while(TIM3->CNT < us); \
	stop_timer(); \
}

#endif /* DELAY_H_ */
