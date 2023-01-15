#include "dma.h"

void dma_setup(void) {
    // Enable DMA clock
    rcc_periph_clock_enable(RCC_DMA1);
    // In order to use I2C_RX, we need DMA 1 Channel 3
    dma_channel_reset(DMA1, DMA_CHANNEL3);
    // I2C device address as input
    dma_set_peripheral_address(DMA1, DMA_CHANNEL3, I2C1_RXDR);
    // We will be using the MAX11615 as the source data
    dma_set_read_from_peripheral(DMA1, DMA_CHANNEL3);
    // Memory increment mode needs to be turned on, so that if we're sending
    // multiple bytes the DMA controller actually sends a series of bytes,
    // instead of the same byte multiple times.
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL3);
    // Contrarily, the peripheral does not need to be incremented - the SPI
    // data register doesn't move around as we write to it.
    dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL3);
    // We want to use 16 bit transfers
    dma_set_peripheral_size(DMA1, DMA_CHANNEL3, DMA_CCR_PSIZE_8BIT);
    dma_set_memory_size(DMA1, DMA_CHANNEL3, DMA_CCR_MSIZE_8BIT);
    // We don't have any other DMA transfers going, but if we did we can use
    // priorities to try to ensure time-critical transfers are not interrupted
    // by others. In this case, it is alone.
    dma_set_priority(DMA1, DMA_CHANNEL3, DMA_CCR_PL_LOW);
    // Since we need to pull the register clock high after the transfer is
    // complete, enable transfer complete interrupts.
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL3);
    // We also need to enable the relevant interrupt in the interrupt
    // controller, and assign it a priority.
    nvic_set_priority(NVIC_DMA1_CHANNEL2_3_DMA2_CHANNEL1_2_IRQ, 0);
    nvic_enable_irq(NVIC_DMA1_CHANNEL2_3_DMA2_CHANNEL1_2_IRQ);
}

void dma_start(void *data, size_t data_size) {
    // Note - manipulating the memory address/size of the DMA controller cannot
    // be done while the channel is enabled. Ensure any previous transfer has
    // completed and the channel is disabled before you start another transfer.
    // Tell the DMA controller to start reading memory data from this address
    dma_set_memory_address(DMA1, DMA_CHANNEL3, (uint32_t)data);
    // Configure the number of bytes to transfer
    dma_set_number_of_data(DMA1, DMA_CHANNEL3, data_size);
    // Enable the DMA channel.
    dma_enable_channel(DMA1, DMA_CHANNEL3);

    // Since we're manually controlling our register clock, move it low now
    gpio_clear(GPIOB, GPIO12);

    // Finally, enable i2c DMA recieve. This call is what actually starts the receive
    i2c_enable_rxdma(I2C1);
}

void dma1_channel2_3_dma2_channel1_2_isr() {
    // Check that we got triggered because the transfer is complete, by
    // checking the Transfer Complete Interrupt Flag
    if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL3, DMA_TCIF)) {
        // If that is why we're here, clear the flag for next time
        dma_clear_interrupt_flags(DMA1, DMA_CHANNEL3, DMA_TCIF);

        // Like the non-dma version, we don't want to latch the register clock
        // until the transfer is actually complete, so wait til the busy flag
        // is clear
        while (i2c_busy(I2C1));

        // Turn our DMA channel back off, in preparation of the next transfer
        i2c_disable_rxdma(I2C1);
        dma_disable_channel(DMA1, DMA_CHANNEL3);

        // Bring the register clock high to latch the transferred data
        gpio_set(GPIOB, GPIO12);
    }
}