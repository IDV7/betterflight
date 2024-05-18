

#include "spi.h"
#include "stm32f7xx.h"

// GPIO macros
#define SCK_HIGH spi_h->sck.port->BSRR = (spi_h->sck.pin)
#define SCK_LOW spi_h->sck.port->BSRR = (spi_h->sck.pin << 16)
#define MOSI_HIGH spi_h->mosi.port->BSRR = (spi_h->mosi.pin)
#define MOSI_LOW spi_h->mosi.port->BSRR = (spi_h->mosi.pin << 16)
#define CS_HIGH spi_h->cs.port->BSRR = (spi_h->cs.pin)
#define CS_LOW spi_h->cs.port->BSRR = (spi_h->cs.pin << 16)

// SPI control macros
#define SPI_ENABLE spi_h->SPIx->CR1 |= SPI_CR1_SPE
#define SPI_DISABLE spi_h->SPIx->CR1 &= ~SPI_CR1_SPE

// SPI flag read macros
#define SPI_TXE_FLAG (spi_h->SPIx->SR & SPI_SR_TXE)
#define SPI_RXNE_FLAG (spi_h->SPIx->SR & SPI_SR_RXNE)

// SPI data read/write macros
#define SPI_WRITE_DATA(data) spi_h->SPIx->DR = data
#define SPI_READ_DATA spi_h->SPIx->DR


void SPI_init(SPI_handle_t *spi_h) {
    /*

        use of CMSIS to configure and control the hardware SPI peripheral

        notes:
        - enable SPI CS before master sending sck (aka setting -> SPI_CR1_SPE)
          master will start to communicate when SPI enabled AND TXFIFO not empty (or with the next write to TXFIFO)

        registers:
           config: CR1, CR2
           status/data:
              - SR: status register most important bits:
                     FTLVL[1:0]: FIFO transmission level (00: empty, 01: 1/4, 10: 1/2, 11: full)
                     FRLVL[1:0]: FIFO reception level (00: empty, 01: 1/4, 10: 1/2, 11: full)
                     FRE: frame format error flag
                     BSY: busy flag
                     TXE: transmit buffer empty flag
                     RXNE : receive buffer not empty flag
              - DR: data register:
                     16-bit data register, write to send, read to receive


        reminder:
        '&=' -> resetting a bit (LOW, 0)
        '|=' -> setting a bit (HIGH, 1)

    */

    //GPIO config for SPI pins (SCK, MISO, MOSI, CS)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    //mosi
    GPIO_InitStruct.Pin = spi_h->mosi.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->mosi.port, &GPIO_InitStruct);
    MOSI_LOW; //init mosi low

    //miso
    GPIO_InitStruct.Pin = spi_h->miso.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(spi_h->miso.port, &GPIO_InitStruct);

    //sck
    GPIO_InitStruct.Pin = spi_h->sck.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->sck.port, &GPIO_InitStruct);
    SCK_HIGH; //init clock high

    //cs
    GPIO_InitStruct.Pin = spi_h->cs.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->cs.port, &GPIO_InitStruct);
    CS_HIGH; //init cs high


    // Clear CR1 & CR2 registers
    spi_h->SPIx->CR1 = 0;
    spi_h->SPIx->CR2 = 0;

    // CR1 register config
    spi_h->SPIx->CR1 &= SPI_CR1_BR_2; // BaudRate[2:0] = 011 -> fPCLK/16
    spi_h->SPIx->CR1 |= SPI_CR1_BR_1;
    spi_h->SPIx->CR1 |= SPI_CR1_BR_0;
    spi_h->SPIx->CR1 &= SPI_CR1_CPOL; // Clock polarity = 0
    spi_h->SPIx->CR1 &= SPI_CR1_CPHA; // Clock phase = 0
    spi_h->SPIx->CR1 &= SPI_CR1_BIDIMODE; // ->(0) 2-line, (1) 1-line
    spi_h->SPIx->CR1 &= SPI_CR1_BIDIOE; // (rx vs tx only), not relevant...
    spi_h->SPIx->CR1 &= SPI_CR1_RXONLY; // ->(0) full duplex, (1) output disabled
    spi_h->SPIx->CR1 &= SPI_CR1_LSBFIRST; // ->(0) MSB first, (1) LSB first
    spi_h->SPIx->CR1 &= SPI_CR1_CRCEN; // CRC Enable: ->(0) disabled, (1) enabled
    spi_h->SPIx->CR1 &= SPI_CR1_CRCL; // CRC length, not relevant...
    spi_h->SPIx->CR1 &= SPI_CR1_CRCNEXT; // CRC next, not relevant...
    spi_h->SPIx->CR1 |= SPI_CR1_SSM; // Software slave management: (0) hardware, ->(1) software
    spi_h->SPIx->CR1 &= SPI_CR1_SSI; // Internal slave select, not relevant...
    spi_h->SPIx->CR1 |= SPI_CR1_MSTR; // (0) slave, ->(1) master

    // CR2 register config
    spi_h->SPIx->CR2 &= SPI_CR2_DS_3; // DataSize[3:0] = 0111 -> 8 bits
    spi_h->SPIx->CR2 |= SPI_CR2_DS_2;
    spi_h->SPIx->CR2 |= SPI_CR2_DS_1;
    spi_h->SPIx->CR2 |= SPI_CR2_DS_0;
    spi_h->SPIx->CR2 &= SPI_CR2_SSOE; // SS output, not relevant... (using software slave management)
    spi_h->SPIx->CR2 &= SPI_CR2_FRF; // ->(0) SPI Motorola mode, (1) TI mode
    spi_h->SPIx->CR2 &= SPI_CR2_NSSP; // generate NSS pulse, not relevant... (using software slave management)
    spi_h->SPIx->CR2 |= SPI_CR2_FRXTH; // FIFO reception threshold, (0) RXNE event when 16-bit, ->(1) RXNE event when 8-bit
    spi_h->SPIx->CR2 &= SPI_CR2_RXDMAEN; // RX buffer DMA enable, ->(0) disabled, (1) enabled
    spi_h->SPIx->CR2 &= SPI_CR2_LDMARX; // DMA reception, not relevant...
    spi_h->SPIx->CR2 &= SPI_CR2_LDMATX; // DMA transmission, not relevant...
    spi_h->SPIx->CR2 &= SPI_CR2_TXEIE; // TX buffer empty interrupt enable, (0) TXEIE interrupt masked, ->(1) TXEIE interrupt not masked (gens interrupt)
    spi_h->SPIx->CR2 &= SPI_CR2_RXNEIE; // RX buffer not empty interrupt enable, (0) RXNEIE interrupt masked, ->(1) RXNEIE interrupt not masked (gens interrupt)
    spi_h->SPIx->CR2 &= SPI_CR2_ERRIE; // Error interrupt enable, (0) ERRIE interrupt masked, ->(1) ERRIE interrupt enabled

}


// CMSIS Hardware SPI transmit & receive
void SPI_transmit_rx(SPI_handle_t *spi_h, uint8_t *tx_data, uint8_t *rx_data, uint32_t len) {

    // enable cs
    CS_LOW;

    for (uint32_t i = 0; i < len; i++) { //for each byte
        uint8_t tx_byte = tx_data[i];
        uint8_t rx_byte = 0;

        /*
         MAARTEN!!!
         Read the reference manual on how to send and receive data using the SPI peripheral
         its not just writing to the DR register, you also need to wait for the correct flags etc...
         reading is also not super easy you need to check a bunch of things like the SR registers FRLVL and FRE flags (I think)
         read the reference manual on how SPI works (starting on page 953),
         most interesting part for this part of the software is on page (962: Data transmission and reception procedure)
         NOTE: don't use/read the parts about DMA (we won't use it)
         gl :)
        */



        // store received byte
        rx_data[i] = rx_byte;
    }

    // disable cs
    CS_HIGH;
}