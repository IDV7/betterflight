
#include "stm32f7xx.h"

#ifndef BETTERFLIGHT_SPI_H
#define BETTERFLIGHT_SPI_H

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} SPI_GPIO;


typedef struct {
    SPI_TypeDef *SPIx; // CMSIS SPI peripheral "registers"
    SPI_GPIO mosi;
    SPI_GPIO miso;
    SPI_GPIO sck;
    SPI_GPIO cs;
}SPI_handle_t;

void SPI_init(SPI_handle_t *hspi);
void SPI_transmit_rx(SPI_handle_t *spi_h, uint8_t *tx_data, uint8_t *rx_data, uint32_t len);

#endif //BETTERFLIGHT_SPI_H
