#include "i2c.h"

void i2cSetup(void)
{


	i2c_reset(I2C1);
	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	gpio_set_af(GPIOB, GPIO_AF1, GPIO6 | GPIO7);
	i2c_peripheral_disable(I2C1);
	//configure ANFOFF DNF[3:0] in CR1
	i2c_enable_analog_filter(I2C1);
	i2c_set_digital_filter(I2C1, 0);
	/* HSI is at 48Mhz */
	i2c_set_speed(I2C1, i2c_speed_fm_400k, 48);
	//configure No-Stretch CR1 (only relevant in slave mode)
	i2c_disable_stretching(I2C1);
	//addressing mode
	i2c_set_7bit_addr_mode(I2C1);
	i2c_peripheral_enable(I2C1);
	

	//	Wait ~50ms for I2C to set up.
	for (int i = 0; i < 50000; i++) 
		__asm__("nop");

	// Set up MAX11615
	uint8_t buff;
	uint8_t cmd = CMD_MAX116xx_SETUP;
	i2c_transfer7(I2C1, I2C_MAX11615_ADDR, &cmd, 1, &buff, 1);

}

void i2cWriteByte(uint32_t i2c, uint8_t addr, const uint8_t *w, size_t wn)
{
	/*  waiting for busy is unnecessary. read the RM */
        if (wn) 
		{
                i2c_set_7bit_address(i2c, addr);
                i2c_set_write_transfer_dir(i2c);
                i2c_set_bytes_to_transfer(i2c, wn);
                i2c_disable_autoend(i2c);
                i2c_send_start(i2c);
 
                while (wn--) 
				{
                        bool wait = true;
                        while (wait) 
						{
                                if (i2c_transmit_int_status(i2c)) 
								{
                                        wait = false;
                                }
                                while (i2c_nack(i2c)); /* FIXME Some error */
                        }
                        i2c_send_data(i2c, *w++);
                }
        }
}