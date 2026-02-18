#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_spi.hpp"
#include "hal/watchdog.hpp"
#include "hal/ext/mx25v.hpp"

Pin<P1,7> mosi;
Pin<P1,6> miso;
Pin<P1,5> sclk;
Pin<P1,0> led;

typedef Pin<P1,4> FlashChipSel;
FlashChipSel cs;

typedef SpiMaster<SPI_A0> Spi;
Spi spi;

FlashMemory<_16Mbit, Spi, FlashChipSel> flash;
Watchdog watchdog;

void main() {
    watchdog.disable();
    
    // Configure GPIO for SPI mode
    mosi.function(PinFunction::Primary);
    miso.function(PinFunction::Primary);
    sclk.function(PinFunction::Primary);
    cs.toOutput().setHigh(); // Chip select
    led.toOutput().setLow();
    gpioUnlock();

    spi.init(SpiMode::MODE_0(), ClockSource::Smclk, 1);
    flash.init(spi, cs);
    
    flash.eraseAll();
    flash.waitUntilReady();
    #define SLEN 10
    uint8_t send[SLEN] = {0,1,2,3,4,5,6,7,8,9};
    flash.writeBytes(0x1234, send, SLEN);
    
    #define RLEN 3
    uint8_t recv[RLEN];
    flash.readBytes(0x1234, recv, RLEN);
    if (recv[0] == 0 && recv[1] == 1 && recv[2] == 2) {
        led.setHigh();
    }
    
    while (1);
}
