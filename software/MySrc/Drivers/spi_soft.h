#ifndef BETTERFLIGHT_SPI_SOFT_H
#define BETTERFLIGHT_SPI_SOFT_H

#include "stm32f7xx_hal.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} SPI_GPIO;

typedef struct {
    SPI_GPIO mosi;
    SPI_GPIO miso;
    SPI_GPIO sck;
    SPI_GPIO cs;
    uint32_t nop_cnt;  // amount of NOPs to delay for half of the SPI clock cycle

} SPI_handle_t;



// Initialize the SPI pins (fill in the SPI_handle_t struct with the correct GPIO pins)
void SPI_soft_init(SPI_handle_t *spi_h);

// transmits tx_data (of length len) and receives rx_data (of length len) over SPI (tx and rx happens in the same cycle)
void SPI_soft_trx(SPI_handle_t *spi_h, const uint8_t *tx_data, uint8_t *rx_data, uint32_t len);

// transmits tx_data (of length len) over SPI
void SPI_soft_tx(SPI_handle_t *spi_h, uint8_t *tx_data, uint32_t len);




#endif //BETTERFLIGHT_SPI_SOFT_H