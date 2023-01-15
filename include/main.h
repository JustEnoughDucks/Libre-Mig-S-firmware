/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include "i2c.h"
#include "adc.h"
#include "usart.h"
#include "dma.h"
#include "usbhid.h"

// Button Struct. Includes variations on button types for use in logic
typedef struct
{
    uint16_t gpio_num;
    uint8_t port :2;
    uint8_t state :1;
    uint8_t state_last :1;
    uint8_t is_inverted :1;
    uint8_t type :2; // 0: Normal, 1: Toggle, 2: i2c, 3: Encode
} button_t;

// Encoder Struct, has relative states for eaasy logic
typedef struct
{
    uint32_t time_last;
    int32_t cnt;
    uint8_t state :3;
    uint8_t pin_a;
    uint8_t pin_b;
    uint8_t dir :1;
    uint8_t dir_last :1;

} encoder_t;


// Function Prototypes
static void clock_setup(void);
static void gpio_setup(void);
//static void button_setup(button_t (*b)[35]);
static void button_setup(void);
void buttonScan(void);


// User defined variables
button_t button[35] = {0};
int16_t axis[AXIS_MAX];

#endif