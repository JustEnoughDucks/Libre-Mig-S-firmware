/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H__
#define __I2C_H__

#include <errno.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

#define I2C_MCU_ADDR 0x00

// MAX116XX defines
#define I2C_MAX11615_ADDR 0x33
#define CMD_MAX116xx_SETUP 0x8A
#define CMD_READ_ALL_AXIS 0x5
#define CMD_READ_A0 0x61
#define CMD_READ_A1 0x63
#define CMD_READ_A3 0x65

// TCA9535 Defines
#define I2C_TCA9535_ADDR_1 0x21
#define I2C_TCA9535_ADDR_2 0x22
#define CMD_INPUT_PORT_0 0x00
#define CMD_INPUT_PORT_1 0x01


void i2cSetup(void);
void i2cWriteByte(uint32_t i2c, uint8_t addr, const uint8_t *w, size_t wn); 

#endif