/**
 * @file main.c
 * @author JustEnoughDucks (you@domain.com)
 * @brief This is the main function for the Libre-MiG-S hardcoded firmware.
 * 			These firmware files are not compatible with FreeJoy Configurator but
 * 			are meant as an early-stange proof of concept and full functionality test.
 * @version 0.1
 * @date 2023-01-03
 *
 * @copyright Copyright (c) 2023 under GPL 3.0 license
 *
 */
#include "main.h"


int main(void) {
    /* code */

    // Simulation Variables
	/*int dir = 1;
	int x = 0;
	uint8_t b = 0;
	uint16_t z = 0;*/

    int err = 0;
    i2cEN = 1;
    startup = 0;
    _millis = 0;
    
    clock_setup();
    adc_setup();
    usart_setup();
    err = i2cSetup();
    if(err != I2C_OK) {
        i2cEN = 0;
        //usart_print_int(USART1, err);
        // usart_send_blocking(USART1, "I2C TIMEOUT");
    }
    button_setup();
    gpio_setup();

    // dma_setup();
    for (int i = 0; i < 14900; i++) { /* Wait a bit. */
        __asm__("nop");
    }
    usb_setup();
    //systick_setup();


    // Main Loop
    while (1) {
		
		//button_simulation(&dir, &x, &b, &z);
        if (err == 0) {
            button_scan();
            axis_scan(&axis, startup);
        } else if (err == ETIMEDOUT) {
            err = i2cSetup(); 
            if(err != I2C_OK) {
                //usart_print_int(USART1, err);
            }
        }
        if (startup == 0){
            startup = 1;
        }

        /* Using API function gpio_toggle(): */
        // gpio_toggle(LEDPORT, LEDPIN);	/* LED on/off */
        // for (int i = 0; i < 1490000; i++) {	/* Wait a bit. */
        //	__asm__("nop");
        // }
        usbd_poll(usbd_dev);
    }
}





void sys_tick_handler(void) {
    
    _millis++;
    // bytes 0-5 are button values, bytes 6-18 are axis values
    uint8_t buf[PACKET_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // uint8_t buf[PACKET_SIZE] = {0, 0, 0, 0, 0, 0, 0};
    // uint8_t buf[4] = {0,0,0,0};

    // Axis Pack
    for (int i = 0; i < AXIS_MAX; i++) {
        buf[(2 * i) + 1] = (axis[i] >> 8) & 0xFF;
        buf[(2 * i)] = axis[i] & 0xFF;
    }

    uint8_t k = 0;
    // Button pack
    for (int i = AXIS_MAX * 2; i < PACKET_SIZE; i++) {
        /*if(i == PACKET_SIZE - 1)
        {
                buf[i] |= button[k].state <<

        }*/
        for (int j = 0; j < 8; j++) {
            if (k == BUTTON_MAX) {
                buf[i] = buf[i] >> 4;
                break;
            }
            if (j < 7) {
                buf[i] |= button[k].state << 7;
                buf[i] = buf[i] >> 1;
            } else if (j == 7) {
                buf[i] |= button[k].state << 7;
            }
            k++;
        }
    }

    usbd_ep_write_packet(usbd_dev, 0x81, buf, PACKET_SIZE);
}

static void clock_setup(void) {
    // rcc_clock_setup_in_hsi_out_48mhz();
    rcc_clock_setup_in_hse_8mhz_out_48mhz();
    // rcc_clock_setup_in_hsi48_out_48mhz();

    // enable GPIOA clock
    rcc_periph_clock_enable(GPIOA);
    rcc_periph_clock_enable(GPIOB);
    

    // enable I2C1 and GPIOB clock
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_USB);

    // Set clock to internal hsi oscillator
    rcc_set_i2c_clock_hsi(I2C1);
    
}

static void systick_setup(void) {
    //Systick enabling
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    STK_CVR = 0;
    systick_set_reload(rcc_ahb_frequency / 8 / 1000);
    systick_clear();
    systick_interrupt_enable();
    systick_counter_enable();

}

static void gpio_setup(void) {
    /* Set pin to 'output push-pull'. */
    /* Using API functions: */
    // gpio_mode_setup(LEDPORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LEDPIN);
    for (int i = 22; i < BUTTON_MAX; i++) {
        if (button[i].type == 0 || button[i].type == 1) {
            if (button[i].port == 0) {
                gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << button[i].gpio_num);
            } else if (button[i].port == 1) {
                gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << button[i].gpio_num);
            }
        }
    }
}

static void button_setup(void) {
    // Trigger Buttons
    button[1].gpio_num = 101;
    button[1].type = 2;
    button[2].gpio_num = 102;
    button[2].type = 2;

    // Face Buttons
    button[0].gpio_num = 100;
    button[0].type = 2;
    button[3].gpio_num = 103;
    button[3].type = 2;
    button[4].gpio_num = 104;
    button[4].type = 2;
    button[5].gpio_num = 105;
    button[5].type = 2;
    button[6].gpio_num = 106;
    button[6].type = 2;
    button[7].gpio_num = 107;
    button[7].type = 2;
    button[8].gpio_num = 108;
    button[8].type = 2;

    // Back side Buttons
    button[9].gpio_num = 109;
    button[9].type = 2;
    button[10].gpio_num = 110;
    button[10].type = 2;
    button[11].gpio_num = 111;
    button[11].type = 2;
    button[12].gpio_num = 112;
    button[12].type = 2;
    button[13].gpio_num = 113;
    button[13].type = 2;
    button[14].gpio_num = 114;
    button[14].type = 2;

    // Encoder Buttons
    button[15].gpio_num = 200;
    button[15].type = 3;
    button[16].gpio_num = 201;
    button[16].type = 3;
    encoder.index_a = 15;
    encoder.index_b = 16;

    button[17].gpio_num = 202;
    button[17].type = 2;
    button[18].gpio_num = 203;
    button[18].type = 2;
    button[19].gpio_num = 204;
    button[19].type = 2;
    button[20].gpio_num = 205;
    button[20].type = 2;
    button[21].gpio_num = 206;
    button[21].type = 2;

    // Toggle 1-3
    button[22].port = 0;
    button[22].gpio_num = 4;
    button[22].type = 1;
    button[23].port = 0;
    button[23].gpio_num = 5;
    button[23].type = 1;
    button[24].port = 0;
    button[24].gpio_num = 6;
    button[24].type = 1;

    // Selector 1-4
    button[25].port = 1;
    button[25].gpio_num = 10;
    button[25].type = 1;
    button[26].port = 1;
    button[26].gpio_num = 11;
    button[26].type = 1;
    button[27].port = 1;
    button[27].gpio_num = 12;
    button[27].type = 1;
    button[28].port = 1;
    button[28].gpio_num = 13;
    button[28].type = 1;

    // Keypad 1-6
    button[29].port = 0;
    button[29].gpio_num = 0;
    button[29].type = 0;
    button[30].port = 1;
    button[30].gpio_num = 14;
    button[30].type = 0;
    button[31].port = 1;
    button[31].gpio_num = 15;
    button[31].type = 0;
    button[32].port = 1;
    button[32].gpio_num = 8;
    button[32].type = 0;
    button[33].port = 1;
    button[33].gpio_num = 9;
    button[33].type = 0;
    button[34].port = 0;
    button[34].gpio_num = 7;
    button[34].type = 0;
}

void button_scan(void) {
    uint8_t tca_raw_1[2] = {0};
    uint8_t tca_raw_2[2] = {0};
    i2c_transfer7_bit(I2C1, I2C_TCA9535_ADDR_1, CMD_INPUT_PORT_0, 1, &tca_raw_1[0], 2, 10);
    i2c_transfer7_bit(I2C1, I2C_TCA9535_ADDR_2, CMD_INPUT_PORT_0, 1, &tca_raw_2[0], 2, 10);
    usart_print_int(USART1, tca_raw_1[1]);
    usart_print_int(USART1, tca_raw_2[0]);
    uint8_t encoderTick = 0;

    for (int i = 0; i < BUTTON_MAX; i++) {
        if (button[i].type == 0 || button[i].type == 1) {
            if (button[i].port == 0) {
                button[i].state_last = button[i].state;
                button[i].state = (gpio_get(GPIOA, 1 << button[i].gpio_num) >> button[i].gpio_num) & 1;
            } else if (button[i].port == 1) {
                button[i].state_last = button[i].state;
                button[i].state = (gpio_get(GPIOB, 1 << button[i].gpio_num) >> button[i].gpio_num) & 1;
            }
        } else if (button[i].type == 2) {
            if (button[i].gpio_num >= 100 && button[i].gpio_num < 200) {
                button[i].state_last = button[i].state;
                button[i].state = (tca_raw_1[(button[i].gpio_num % 100) / 8] >> (button[i].gpio_num % 100 % 8)) & 1;
            } else if (button[i].gpio_num >= 200) {
                button[i].state_last = button[i].state;
                button[i].state = (tca_raw_2[(button[i].gpio_num % 200) / 8] >> (button[i].gpio_num % 200 % 8)) & 1;
            }
        } else if (button[i].type == 3) {
            button[i].state_last = button[i].state;
            button[i].state = (tca_raw_2[(button[i].gpio_num % 200) / 8] >> (button[i].gpio_num % 200 % 8)) & 1;
            encoderTick ^= 1;

            if(!encoderTick && button[encoder.index_a].state_last != button[encoder.index_a].state) {
                
                encoder.dir_last = encoder.dir;
                encoder.dir = button[encoder.index_b].state && button[encoder.index_a].state;

                if(encoder.dir_last == 0 && encoder.dir == 0) {
                    encoder.state = 1;
                    button[encoder.index_a].state = 1;
                    button[encoder.index_b].state = 0;
                }else if(encoder.dir_last == 1 && encoder.dir == 1) {
                    encoder.state = 2;
                    button[encoder.index_a].state = 0;
                    button[encoder.index_b].state = 1;
                }else {
                    encoder.state = 0;
                    button[encoder.index_a].state = 0;
                    button[encoder.index_b].state = 0;
                }

            }

            // TODO: put in enc3der code
        }
    }
}

void button_simulation(int *dir, int *x, uint8_t *b, uint16_t *z) {
    // Simulation variables

    if (*x == 2) {
        if (axis[0] > 32766) {
            if (*b == BUTTON_MAX) {
                *b = 0;
            }
            *dir = -1;
            button[*b].state_last = button[*b].state;
            button[*b].state = 1;
        }
        if (axis[0] < -32766) {
            button[*b].state_last = button[*b].state;
            button[*b].state = 0;
            (*b)++;

            *dir = 1;
        }
        if (*z == 10000) {
            *z = 0;
        }

        axis[0] += *dir;
        axis[1] += -*dir;
        axis[2] += *dir;
        axis[3] += -*dir;
        axis[4] += *dir;
        axis[5] += -*dir;
        *x = 0;
        (*z)++;
    }
    (*x)++;
}

