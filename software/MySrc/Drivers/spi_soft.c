


#include "spi_soft.h"

// HAL is too slow, macros for controlling the spi related gpio pins via the BSRR register
#define MOSI_HIGH spi_h->mosi.port->BSRR = (spi_h->mosi.pin)
#define MOSI_LOW spi_h->mosi.port->BSRR = (spi_h->mosi.pin << 16)

#define SCK_HIGH spi_h->sck.port->BSRR = (spi_h->sck.pin)
#define SCK_LOW spi_h->sck.port->BSRR = (spi_h->sck.pin << 16)

#define CS_HIGH spi_h->cs.port->BSRR = (spi_h->cs.pin)
#define CS_LOW spi_h->cs.port->BSRR = (spi_h->cs.pin << 16)


// Initialize the SPI pins (fill in the SPI_handle_t struct with the correct GPIO pins)
void SPI_soft_init(SPI_handle_t *spi_h) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //init mosi
    GPIO_InitStruct.Pin = spi_h->mosi.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->mosi.port, &GPIO_InitStruct);
    MOSI_LOW;

    //init miso
    GPIO_InitStruct.Pin = spi_h->miso.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(spi_h->miso.port, &GPIO_InitStruct);

    //init sck
    GPIO_InitStruct.Pin = spi_h->sck.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->sck.port, &GPIO_InitStruct);
    SCK_HIGH;

    //init cs
    GPIO_InitStruct.Pin = spi_h->cs.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->cs.port, &GPIO_InitStruct);
    CS_HIGH;

}

// transmits tx_data (of length len) and receives rx_data (of length len) over SPI (tx and rx happens in the same cycle)
void SPI_soft_trx(SPI_handle_t *spi_h, const uint8_t *tx_data, uint8_t *rx_data, uint32_t len) {

    // turn off interrupts
    __disable_irq();

    // enable cs
    CS_LOW;
    __NOP();

    for (uint32_t i = 0; i < len; i++) { //for each byte
        uint8_t tx_byte = tx_data[i];
        uint8_t rx_byte = 0;

        for (uint8_t j = 0; j < 8; j++) { //for each bit

            // set mosi
            if (tx_byte & 0x80) {
                MOSI_HIGH;
            } else {
                MOSI_LOW;
            }

            // shift tx_byte
            tx_byte <<= 1;

            SCK_LOW;
            //SPI_Delay(NOP_CNT);

            // read miso
            rx_byte <<= 1;
            if (spi_h->miso.port->IDR & spi_h->miso.pin) { // if miso is high
                rx_byte |= 0x01; // set the last bit
            }

            SCK_HIGH;
            //SPI_Delay(NOP_CNT);
        }
        MOSI_LOW;

        rx_data[i] = rx_byte;
    }

    // disable cs
    CS_HIGH;

    // enable interrupts again
    __enable_irq();
}

// transmits tx_data (of length len) over SPI
void SPI_soft_tx(SPI_handle_t *spi_h, uint8_t *tx_data, uint32_t len) {

    // turn off interrupts
    __disable_irq();

    // enable cs
    CS_LOW;
    __NOP();

    for (uint32_t i = 0; i < len; i++) { //for each byte
        uint8_t tx_byte = tx_data[i];

        for (uint8_t j = 0; j < 8; j++) { //for each bit

            // set mosi
            if (tx_byte & 0x80) {
                MOSI_HIGH;
            } else {
                MOSI_LOW;
            }

            // shift tx_byte
            tx_byte <<= 1;

            SCK_LOW;
            __NOP();


            SCK_HIGH;
            __NOP();

        }
        MOSI_LOW;
    }

    // disable cs
    CS_HIGH;

    // enable interrupts again
    __enable_irq();
}