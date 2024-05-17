


#include "spi_soft.h"
#include "misc.h"
#include "log.h"

static void SPI_Delay(uint32_t cycles);
static uint32_t calc_nops_spi_half_cycle(uint32_t mcu_clk_speed_hz, uint32_t spi_clock_speed_hz);


// Initialize the SPI pins (requires the user to have configured the pins a bit already... (like clocks and stuff))
void SPI_Init(SPI_handle_t *spi_h, uint32_t spi_speed_hz) {

    // calc number of NOPs to delay for half of the SPI clock cycle
    spi_h->nop_cnt = calc_nops_spi_half_cycle(get_mcu_clock_speed(), spi_speed_hz);

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //init mosi
    GPIO_InitStruct.Pin = spi_h->mosi.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->mosi.port, &GPIO_InitStruct);
    //init mosi low
    spi_h->mosi.port->BSRR = spi_h->mosi.pin << 16;

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
    //init clock high
    spi_h->sck.port->BSRR = spi_h->sck.pin;

    //init cs
    if (spi_h->cs.port != NULL) {
        GPIO_InitStruct.Pin = spi_h->cs.pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(spi_h->cs.port, &GPIO_InitStruct);
        //init cs high
        spi_h->cs.port->BSRR = spi_h->cs.pin << 16;
    }
}

void SPI_transmit_receive(SPI_handle_t *spi_h, uint8_t *tx_data, uint8_t *rx_data, uint32_t len) {

    // turn off interrupts
    __disable_irq();

    // enable cs
    spi_h->cs.port->BSRR = spi_h->cs.pin;

    for (uint32_t i = 0; i < len; i++) {
        uint8_t tx_byte = tx_data[i];
        uint8_t rx_byte = 0;

        for (uint8_t j = 0; j < 8; j++) {
            // set mosi
            if (tx_byte & 0x80) {
                spi_h->mosi.port->BSRR = (spi_h->mosi.pin);
            } else {
                spi_h->mosi.port->BSRR = (spi_h->mosi.pin << 16);
            }

            // shift tx_byte
            tx_byte <<= 1;

            // read miso
            if (spi_h->miso.port->IDR & spi_h->miso.pin) {
                rx_byte |= 0x80;
            }

            // shift rx_byte
            rx_byte <<= 1;

            // toggle sck
            spi_h->sck.port->BSRR = spi_h->sck.pin;
            SPI_Delay(spi_h->nop_cnt);
            spi_h->sck.port->BSRR = spi_h->sck.pin << 16;
            SPI_Delay(spi_h->nop_cnt);
        }

        rx_data[i] = rx_byte;
    }

    // disable cs
    spi_h->cs.port->BSRR = spi_h->cs.pin << 16;

    // enable interrupts again
    __enable_irq();

}



static void SPI_Delay(uint32_t cycles) {
    while (cycles--) {
        __NOP();
    }
}

// calc delay in nop cycles for given spi speed and mcu speed
uint32_t calc_nops_spi_half_cycle(uint32_t mcu_clk_speed_hz, uint32_t spi_clock_speed_hz) {
    //calc period of spi clock in ns
    uint32_t spiPeriodNs = 1000000000 / spi_clock_speed_hz;

    //calc period of mcu clock in ns
    uint32_t mcuCycleDurationNs = 1000000000 / mcu_clk_speed_hz;

    //calc number of mcu cycles in one spi clock period
    uint32_t nopCount = spiPeriodNs / mcuCycleDurationNs / 2;

    return nopCount;
}
