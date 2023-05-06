#include "i2c.h"

int i2cSetup(void) {
    i2c_reset(I2C1);
    /* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
    gpio_set_af(GPIOB, GPIO_AF1, GPIO6 | GPIO7);
    i2c_peripheral_disable(I2C1);
    // configure ANFOFF DNF[3:0] in CR1
    i2c_enable_analog_filter(I2C1);
    i2c_set_digital_filter(I2C1, 0);
    /* HSI is at 48Mhz */
    i2c_set_speed(I2C1, i2c_speed_fm_400k, 48);
    // configure No-Stretch CR1 (only relevant in slave mode)
    i2c_disable_stretching(I2C1);
    // addressing mode
    i2c_set_7bit_addr_mode(I2C1);
    i2c_peripheral_enable(I2C1);

    //	Wait ~50ms for I4C to set up.
    for (int i = 0; i < 50000; i++) __asm__("nop");

    // Set up MAX11615
    uint8_t buff;
    uint8_t cmd = CMD_MAX116xx_SETUP;
    if ((i2c_transfer7_bit(I2C1, I2C_MAX11613_ADDR, &cmd, 1, &buff, 1, 10)) == 0) {
        return 0;
    } else {
        return -1;
    }

}

/**
 * Run a write/read transaction to a given 7bit i2c address
 * If both write & read are provided, the read will use repeated start.
 * Both write and read are optional
 * This modification actually includes a timeout - Benjamin
 * @param i2c peripheral of choice, eg I2C1
 * @param addr 7 bit i2c device address
 * @param w buffer of data to write
 * @param wn length of w
 * @param r destination buffer to read into
 * @param rn number of bytes to read (r should be at least this long)
 */
int8_t i2c_transfer7_bit(uint32_t i2c, uint8_t addr, uint8_t *w, size_t wn, uint8_t *r, size_t rn, uint16_t delay) {
    /*  waiting for busy is unnecessary. read the RM */
    if(!delay){
        delay = 10;
    }
    if (wn) {
        i2c_set_7bit_address(i2c, addr);
        i2c_set_write_transfer_dir(i2c);
        i2c_set_bytes_to_transfer(i2c, wn);
        if (rn) {
            i2c_disable_autoend(i2c);
        } else {
            i2c_enable_autoend(i2c);
        }
        i2c_send_start(i2c);
        uint32_t startMillis = _millis;

        while (wn--) {
            bool wait = true;
            while (wait) {
                if (i2c_transmit_int_status(i2c)) {
                    wait = false;
                }
                while (i2c_nack(i2c)) {
                    if((_millis - startMillis) > delay){
                        return ETIMEDOUT;
                    }
                }
                
            }
            i2c_send_data(i2c, *w++);
        }
        /* not entirely sure this is really necessary.
         * RM implies it will stall until it can write out the later bits
         */
        if (rn) {
            while (!i2c_transfer_complete(i2c))
                ;
        }
    }

    if (rn) {
        /* Setting transfer properties */
        i2c_set_7bit_address(i2c, addr);
        i2c_set_read_transfer_dir(i2c);
        i2c_set_bytes_to_transfer(i2c, rn);
        /* start transfer */
        i2c_send_start(i2c);
        /* important to do it afterwards to do a proper repeated start! */
        i2c_enable_autoend(i2c);

        for (size_t i = 0; i < rn; i++) {
            while (i2c_received_data(i2c) == 0)
                ;
            r[i] = i2c_get_data(i2c);
        }
    }
    return 0;
}