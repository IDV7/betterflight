/*
    SPI driver for STM32F7xx microcontrollers
 */



#include "spi.h"
#include "stm32f7xx.h"

#include "log.h"

// GPIO macros
#define SCK_HIGH spi_h->sck.port->BSRR = (spi_h->sck.pin)
#define SCK_LOW spi_h->sck.port->BSRR = (spi_h->sck.pin << 16)
#define MOSI_HIGH spi_h->mosi.port->BSRR = (spi_h->mosi.pin)
#define MOSI_LOW spi_h->mosi.port->BSRR = (spi_h->mosi.pin << 16)
#define CS_HIGH spi_h->cs.port->BSRR = (spi_h->cs.pin)
#define CS_LOW spi_h->cs.port->BSRR = (spi_h->cs.pin << 16)

// SPI control macros
#define SPI_ENABLE spi_h->SPIx->CR1 |= SPI_CR1_SPE // Enable SPI peripheral
#define SPI_DISABLE spi_h->SPIx->CR1 &= ~SPI_CR1_SPE // Disable SPI peripheral

// SPI flag read macros
#define SPI_TXE_FLAG (spi_h->SPIx->SR & SPI_SR_TXE) // TX buffer empty flag
#define SPI_RXNE_FLAG (spi_h->SPIx->SR & SPI_SR_RXNE) // RX buffer not empty flag

// SPI data read/write macros
#define SPI_WRITE_DATA(data) spi_h->SPIx->DR = data // Write data to SPI peripheral
#define SPI_READ_DATA spi_h->SPIx->DR // Read data from SPI peripheral


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

    SPI_DISABLE; //make sure spi is disabled when configuring its registers

    // Enable GPIO clocks based on which ports are used
    if (spi_h->sck.port == GPIOA || spi_h->miso.port == GPIOA || spi_h->mosi.port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        LOGD("Found GPIOA");
    }
    if (spi_h->sck.port == GPIOB || spi_h->miso.port == GPIOB || spi_h->mosi.port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        LOGD("Found GPIOA");
    }
    if (spi_h->sck.port == GPIOC || spi_h->miso.port == GPIOC || spi_h->mosi.port == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    if (spi_h->sck.port == GPIOD || spi_h->miso.port == GPIOD || spi_h->mosi.port == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }

    uint32_t gpio_af = 0; //contains the GPIO_AF5_SPIx value

    //"select" HAL GPIO_AF_SPIx based on what SPIx is used
    if (spi_h->SPIx == SPI1) {
        RCC->AHB2ENR |= RCC_APB2ENR_SPI1EN;
        gpio_af = GPIO_AF5_SPI1;
        LOGD("Found SPI1");
    } else if (spi_h->SPIx == SPI2) {
        RCC->AHB1ENR |= RCC_APB1ENR_SPI2EN;
        gpio_af = GPIO_AF5_SPI2;
        LOGD("Found SPI2");
    } else if (spi_h->SPIx == SPI3) {
        RCC->AHB1ENR |= RCC_APB1ENR_SPI3EN;
        gpio_af = GPIO_AF5_SPI3;
        LOGD("Found SPI3");
    }

    //GPIO config for SPI pins (SCK, MISO, MOSI, CS)
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configure SCK pin
    GPIO_InitStruct.Pin = spi_h->sck.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = gpio_af;
    HAL_GPIO_Init(spi_h->sck.port, &GPIO_InitStruct);
    SCK_LOW; // SCK is low in idle state

    // Configure MISO pin
    GPIO_InitStruct.Pin = spi_h->miso.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = gpio_af;
    HAL_GPIO_Init(spi_h->miso.port, &GPIO_InitStruct);


    // Configure MOSI pin
    GPIO_InitStruct.Pin = spi_h->mosi.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = gpio_af;
    HAL_GPIO_Init(spi_h->mosi.port, &GPIO_InitStruct);
    MOSI_LOW; // MOSI is low in idle state

    // Configure CS pin if using software NSS
    GPIO_InitStruct.Pin = spi_h->cs.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(spi_h->cs.port, &GPIO_InitStruct);
    CS_LOW;

    // Clear CR1 & CR2 registers
    spi_h->SPIx->CR1 = 0;
    spi_h->SPIx->CR2 = 0;
    spi_h->SPIx->I2SCFGR = 0;

    // CR1 register config
    spi_h->SPIx->CR1 |= SPI_CR1_BR_2; // BaudRate[2:0] = 100 -> fPCLK/32
    spi_h->SPIx->CR1 &= ~SPI_CR1_BR_1;
    spi_h->SPIx->CR1 &= ~SPI_CR1_BR_0;
    spi_h->SPIx->CR1 &= ~SPI_CR1_CPOL; // Clock polarity = 0
    spi_h->SPIx->CR1 &= ~SPI_CR1_CPHA; // Clock phase = 0
    spi_h->SPIx->CR1 &= ~SPI_CR1_BIDIMODE; // ->(0) 2-line, (1) 1-line
    spi_h->SPIx->CR1 &= ~SPI_CR1_BIDIOE; // (rx vs tx only), not relevant...
    spi_h->SPIx->CR1 &= ~SPI_CR1_RXONLY; // ->(0) full duplex, (1) output disabled
    spi_h->SPIx->CR1 &= ~SPI_CR1_LSBFIRST; // ->(0) MSB first, (1) LSB first
    spi_h->SPIx->CR1 &= ~SPI_CR1_CRCEN; // CRC Enable: ->(0) disabled, (1) enabled
    spi_h->SPIx->CR1 &= ~SPI_CR1_CRCL; // CRC length, not relevant...
    spi_h->SPIx->CR1 &= ~SPI_CR1_CRCNEXT; // CRC next, not relevant...
    spi_h->SPIx->CR1 |= SPI_CR1_SSM; // Software slave management: (0) hardware, ->(1) software
    spi_h->SPIx->CR1 &= ~SPI_CR1_SSI; // Internal slave select, not relevant...
    spi_h->SPIx->CR1 |= SPI_CR1_MSTR; // (0) slave, ->(1) master

    // CR2 register config
    spi_h->SPIx->CR2 &= ~SPI_CR2_DS_3; // DataSize[3:0] = 0111 -> 8 bits
    spi_h->SPIx->CR2 |= SPI_CR2_DS_2;
    spi_h->SPIx->CR2 |= SPI_CR2_DS_1;
    spi_h->SPIx->CR2 |= SPI_CR2_DS_0;
    spi_h->SPIx->CR2 &= ~SPI_CR2_SSOE; // SS output, not relevant... (using software slave management)
    spi_h->SPIx->CR2 &= ~SPI_CR2_FRF; // ->(0) SPI Motorola mode, (1) TI mode
    spi_h->SPIx->CR2 &= ~SPI_CR2_NSSP; // generate NSS pulse, not relevant... (using software slave management)
    spi_h->SPIx->CR2 |= SPI_CR2_FRXTH; // FIFO reception threshold, (0) RXNE event when 16-bit, ->(1) RXNE event when 8-bit
    spi_h->SPIx->CR2 &= ~SPI_CR2_RXDMAEN; // RX buffer DMA enable, ->(0) disabled, (1) enabled
    spi_h->SPIx->CR2 &= ~SPI_CR2_TXDMAEN; // TX buffer DMA enable, ->(0) disabled, (1) enabled
    spi_h->SPIx->CR2 &= ~SPI_CR2_LDMARX; // DMA reception, not relevant...
    spi_h->SPIx->CR2 &= ~SPI_CR2_LDMATX; // DMA transmission, not relevant...
    spi_h->SPIx->CR2 &= ~SPI_CR2_TXEIE; // TX buffer empty interrupt enable, (0) TXEIE interrupt masked, ->(1) TXEIE interrupt not masked (gens interrupt)
    spi_h->SPIx->CR2 &= ~SPI_CR2_RXNEIE; // RX buffer not empty interrupt enable, (0) RXNEIE interrupt masked, ->(1) RXNEIE interrupt not masked (gens interrupt)
    spi_h->SPIx->CR2 &= ~SPI_CR2_ERRIE; // Error interrupt enable, (0) ERRIE interrupt masked, ->(1) ERRIE interrupt enabled

    // I2SCFGR register config
    spi_h->SPIx->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD; // ->(0) SPI mode, (1) I2S mode
    spi_h->SPIx->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // ->(0) I2S peripheral disabled, (1) I2S peripheral enabled


    //print out the CR1, CR2, I2SCFGR registers




    LOGD("CR1: %s", byte_to_binary_str(spi_h->SPIx->CR1));
    LOGD("CR2: %s", byte_to_binary_str(spi_h->SPIx->CR2));
    LOGD("I2SCFGR: %s", byte_to_binary_str(spi_h->SPIx->I2SCFGR));
    LOGD("SPI status register: %s", byte_to_binary_str(spi_h->SPIx->SR));



    SPI_ENABLE;
}


// CMSIS Hardware SPI transmit & receive
void SPI_transmit_rx(SPI_handle_t *spi_h, uint8_t *tx_data, uint8_t *rx_data, uint32_t len) {

    // enable cs
    CS_LOW;

    // Wait for TX buffer to be empty
    while (!SPI_TXE_FLAG);
    LOGD("TXE flag set");
    // Write data
    SPI_WRITE_DATA(*tx_data);

    // Wait for RX buffer to be filled
    while (!SPI_RXNE_FLAG);
    LOGD("RXNE flag set");
    // disable cs
    CS_HIGH;

    rx_data = (uint8_t*) SPI_READ_DATA;
    LOGD("Read rx data");

//    for (uint32_t i = 0; i < len; i++) { //for each byte
//        uint8_t tx_byte = tx_data[i];
//        uint8_t rx_byte = 0;
//
//
//
//
//        // store received byte
//        rx_data[i] = rx_byte;
//    }

}