/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H__
#define __I2C_H__

#include <errno.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>

#define I2C_MCU_ADDR 0x00

// Error codes defines
#define I2C_OK 0

// MAX116XX defines
#define I2C_MAX11615_ADDR 0x33
#define CMD_MAX116xx_SETUP 0x8A
#define I2C_MAX11613_ADDR 0x34
#define CMD_READ_ALL_AXIS 0x7
#define CMD_READ_A0 0x61
#define CMD_READ_A1 0x63
#define CMD_READ_A2 0x65
#define CMD_READ_A3 0x67

// TCA9535 Defines
#define I2C_TCA9535_ADDR_1 0x21
#define I2C_TCA9535_ADDR_2 0x22
#define CMD_INPUT_PORT_0 0x00
#define CMD_INPUT_PORT_1 0x01

// Project millisecond counter
static volatile uint64_t _millis;

int i2cSetup(void);
int8_t i2c_transfer7_bit(uint32_t i2c, uint8_t addr, uint8_t *w, size_t wn, uint8_t *r, size_t rn, uint16_t delay); 

#endif