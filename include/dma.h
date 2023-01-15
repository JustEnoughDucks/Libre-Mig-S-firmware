/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMA_H__
#define __DMA_H__


//#include <errno.h>
#include <stdio.h>
#include "i2c.h"
//#include <unistd.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>
//#include <libopencm3/cm3/systick.h>

// Function  prototypes
void dma_setup(void);
void dma_start(void *data, size_t data_size);

#endif