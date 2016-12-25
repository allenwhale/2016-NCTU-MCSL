/*
 * adc.h
 *
 *  Created on: 2016¦~12¤ë24¤é
 *      Author: Administrator
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32l476xx.h"
#include "delay.h"

void ADCResolution(ADC_TypeDef *adc, int resolution);
void ADCContinuousConversion(ADC_TypeDef *adc, int enable);
void ADCDataAlign(ADC_TypeDef *adc, int dir);
void ADCChannel(ADC_TypeDef *adc, int channel, int rank, int sampleTime);
void ADCWakeup(ADC_TypeDef *adc);
void ADCEnable(ADC_TypeDef *adc);
int ADCGetValue(ADC_TypeDef *adc);
void ADCStartConversion(ADC_TypeDef *adc);
void ADCInterrupt(ADC_TypeDef *adc, int it, int enable);
void ADCCommonDualMode(int mode);
void ADCCommonClockMode(int clkMode);
void ADCCommonPrescaler(int psc);
void ADCCommonDMAMode(int mode);
void ADCCommonDelayTwoSampling(int clk);
#endif /* ADC_H_ */
