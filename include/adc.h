/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "i2c.h"
#include "usart.h"
//#include "main.h"

#define AXIS_MAX 6


void adc_setup(void);
void axisScan(int16_t (*a) [AXIS_MAX]);

#endif