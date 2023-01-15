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

#define RCCLEDPORT (RCC_GPIOA)
#define LEDPORT GPIOA
#define LEDPIN GPIO5

int dir;
int x;


int main(void) 
{
	// Simulation variables
	//dir = 1;
	//x = 0;
	//int z = 0;
	
    /* code */
	clock_setup();
	adc_setup();
	usart_setup();
	i2cSetup();
	button_setup();
	gpio_setup();
	//dma_setup();
	for (int i = 0; i < 14900; i++) {	/* Wait a bit. */
		__asm__("nop");
	}
	usb_setup();

    // Main Loop
    while(1)
    {
		buttonScan();
		axisScan(&axis);
		//usart_print_int(USART1, axis[0]);
		//usart_print_int(USART1, axis[1])

		// Input simulator
		/*
		if(x == 10)
		{
			if(axis[0] > 32600)
			{
				dir = -1;
				for(int i = 0; i < 35; i++)
				{
					button[i].state_last = button[i].state;
					button[i].state = 1; 
					//usart_print_int(USART1,i);
				}
			}
			if(axis[0] < -32600)
			{

				for(int i = 0; i < 35; i++)
				{
					button[i].state_last = button[i].state;
					button[i].state = 0; 
					//usart_print_int(USART1,i);
				}
				dir = 1;
			}
			if(z == 10000)
			{
				z = 0;
			}

			axis[0] += dir;
			axis[1] += -dir;
			axis[2] += dir;
			axis[3] += -dir;
			axis[4] += dir;
			axis[5] += -dir;
			x=0;
			z++;
		}
		x++;*/



		//my_usart_print_int(USART1, axis_raw[0]);

        /* Using API function gpio_toggle(): */
		//gpio_toggle(LEDPORT, LEDPIN);	/* LED on/off */
		//for (int i = 0; i < 1490000; i++) {	/* Wait a bit. */
		//	__asm__("nop");
		//}
		usbd_poll(usbd_dev);
    }
}

void sys_tick_handler(void)
{

	// bytes 0-5 are button values, bytes 6-18 are axis values
	uint8_t buf[PACKET_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//uint8_t buf[PACKET_SIZE] = {0, 0, 0, 0, 0, 0, 0};
	//uint8_t buf[4] = {0,0,0,0};

	// Axis Pack
	for(int i = 0; i < AXIS_MAX; i++)
	{
		buf[2*i+1] = (axis[i] >> 8) & 0xFF;
		buf[2*i] = axis[i] & 0xFF;
	}

	uint8_t k = 0;
	// Button pack
	for(int i = AXIS_MAX*2; i < PACKET_SIZE; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(k == 35)
			{
				buf[i] = buf[i] >> 1;
				break;
			}
			if(j < 7)
			{
				buf[i] |= button[k].state;
				buf[i] = buf[i] << 1;
			}else if(j == 7)
			{
				buf[i] |= button[k].state;
			}
			k++;
		}
	}



	usbd_ep_write_packet(usbd_dev, 0x81, buf, PACKET_SIZE);
}

static void clock_setup(void)
{
	//rcc_clock_setup_in_hsi_out_48mhz();
	rcc_clock_setup_in_hsi48_out_48mhz();
	
	// enable GPIOA clock
	rcc_periph_clock_enable(GPIOA);
	rcc_periph_clock_enable(GPIOB);

	// enable I2C1 and GPIOB clock
	rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_USB);

	// Set clock to internal hsi oscillator
	rcc_set_i2c_clock_hsi(I2C1);
}

static void gpio_setup(void)
{
	/* Set pin to 'output push-pull'. */
	/* Using API functions: */
	gpio_mode_setup(LEDPORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LEDPIN);
	for(int i = 22; i < 32; i++)
	{
		if(button[i].type == 0 || button[i].type == 1)
		{
			if(button[i].port == 1)
			{
				gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, button[i].port);
			}else if(button[i].port == 2)
			{
				gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, button[i].port);
			}
		}
	}

}


static void button_setup(void)
{
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
	button[22].port = 1;
	button[22].gpio_num = GPIO4;
	button[22].type = 1;
	button[23].port = 1;
	button[23].gpio_num = GPIO5;
	button[23].type = 1;
	button[23].port = 1;
	button[24].gpio_num = GPIO6;
	button[24].type = 1;

	// Selector 1-4
	button[25].port = 2;
	button[25].gpio_num = GPIO10;
	button[25].type = 1;
	button[26].port = 2;
	button[26].gpio_num = GPIO11;
	button[26].type = 1;
	button[27].port = 2;
	button[27].gpio_num = GPIO12;
	button[27].type = 1;
	button[28].port = 2;
	button[28].gpio_num = GPIO13;
	button[28].type = 1;

	// Keypad 1-6
	button[29].port = 1;
	button[29].gpio_num = GPIO0;
	button[29].type = 0;
	button[30].port = 2;
	button[30].gpio_num = GPIO14;
	button[30].type = 0;
	button[31].port = 2;
	button[31].gpio_num = GPIO15;
	button[31].type = 0;
	button[32].port = 2;
	button[32].gpio_num = GPIO8;
	button[32].type = 0;
	button[33].port = 2;
	button[33].gpio_num = GPIO9;
	button[33].type = 0;
	button[34].port = 1;
	button[34].gpio_num = GPIO7;
	button[34].type = 0;
	
}


void buttonScan(void)
{
	uint8_t tca_raw_1[2] = {0};
	uint8_t tca_raw_2[2] = {0};
	i2c_transfer7(I2C1, I2C_TCA9535_ADDR_1, CMD_INPUT_PORT_0, 1, &tca_raw_1[0], 2);
	i2c_transfer7(I2C1, I2C_TCA9535_ADDR_2, CMD_INPUT_PORT_0, 1, &tca_raw_2[0], 2);
	usart_print_int(USART1, tca_raw_1[1]);
	usart_print_int(USART1, tca_raw_2[0]);

	for(int i = 0; i < 32; i++)
	{
		if(button[i].type == 0 || button[i].type == 1)
		{
			if(button[i].port == 1)
			{
				button[i].state_last = button[i].state;
				button[i].state = gpio_get(GPIOA,button[i].gpio_num);
			}else if(button[i].port == 2)
			{
				button[i].state_last = button[i].state;
				button[i].state = gpio_get(GPIOB,button[i].gpio_num);
			}
		} else if(button[i].type == 2)
		{	
			if(button[i].gpio_num >= 100 && button[i].gpio_num < 200)
			{
				button[i].state_last = button[i].state;
				button[i].state = (tca_raw_1[(button[i].gpio_num % 100) / 8] >> (button[i].gpio_num % 100 % 8)) & 1;
			} else if(button[i].gpio_num >= 200)
			{
				button[i].state_last = button[i].state;
				button[i].state = (tca_raw_2[(button[i].gpio_num % 200) / 8] >> (button[i].gpio_num % 200 % 8)) & 1;
			}
		} else if(button[i].type == 3)
		{
			// TODO: put in encoder code
		}
	}
}