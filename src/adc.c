/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Modified by Fernando Cortes <fermando.corcam@gmail.com>
 * modified by Guillermo Rivera <memogrg@gmail.com>
 * modified by Frantisek Burian <BuFran@seznam.cz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "adc.h"


uint8_t channel_array[] = { 1, 1, ADC_CHANNEL_TEMP};

void adc_setup(void)
{
	rcc_periph_clock_enable(RCC_ADC);
	rcc_periph_clock_enable(RCC_GPIOA);

	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);

	adc_power_off(ADC1);
	adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
	adc_calibrate(ADC1);
	adc_set_operation_mode(ADC1, ADC_MODE_SCAN);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_071DOT5);
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
	adc_disable_analog_watchdog(ADC1);
	adc_power_on(ADC1);

	/* Wait for ADC to start up. */
	for (int i = 0; i < 800000; i++)
		__asm__("nop");

}

void axisScan(int16_t (*a)[AXIS_MAX])
{
	uint16_t axis_raw[AXIS_MAX];
	//uint8_t data[2];
	//uint8_t cmd = CMD_READ_A0;
	uint8_t data[6];
	uint8_t cmd = CMD_READ_ALL_AXIS;
	//i2cWriteByte(I2C1,I2C_MAX11615_ADDR, &cmd, 1);
	//dma_start(data, 2);


	i2c_transfer7(I2C1, I2C_MAX11615_ADDR, &cmd, 1, data, 6);
	//axis_raw[0] = ((uint16_t)data[0] & 15) << 8 | data[1];
	//*a[0] = axis_raw[0] - 2048;
	for(int i = 3; i < 6; i++)
	{
		axis_raw[i+3] = ((uint16_t)data[(i*2)] & 15) << 8 | data[(i*2)+1];
		*a[i+3] = axis_raw[i+3] - 2048;
	}

	adc_start_conversion_regular(ADC1);
		
	while (!(adc_eoc(ADC1)));

	axis_raw[0] = adc_read_regular(ADC1);
	*a[0] = axis_raw[0] - 2048;
	//usart_print_int(USART1, temp);
	
}