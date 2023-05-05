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
#define PCB_REV 2


void adc_setup(void);
void axis_scan(int16_t (*a) [AXIS_MAX], uint8_t startup);
//extern uint16_t offset_table[6];

#endif