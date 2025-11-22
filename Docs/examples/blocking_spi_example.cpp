#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_spi.hpp"

Pin<P1,7> mosi;
Pin<P1,6> miso;
Pin<P1,5> sclk;
Pin<P1,4> cs;
SpiMaster<SPI_A0> spi;

void main() {
    // Configure GPIO for SPI mode
    mosi.function(PinFunction::Primary);
    miso.function(PinFunction::Primary);
    sclk.function(PinFunction::Primary);
    // Chip select
    cs.toOutput().setHigh();

    // Note: This method has two default arguments: BitOrder and PacketLength. You probably don't need to change them.
    spi.init(SpiMode::MODE_0(), ClockSource::Smclk, 1);

    // There are two levels of abstractions on offer: A high level that sends buffers and manages the chip select pin automatically,
    // and a lower level that allows for manual chip select control and bytes sent individually. 

    /* High level: transfer(), transferInPlace(), write() and receive() all accept a chip select pin to automatically toggle. */
    constexpr uint16_t buf_len = 2; 
    uint8_t sendBuf[buf_len] = {1, 2};
    uint8_t recvBuf[buf_len];
    spi.transfer(sendBuf, buf_len, recvBuf, buf_len, cs);

    
    /* Low level: SPI transactions can be performed manually via writeByte(), readByte() or transferByte() */
    cs.setLow();

    // Send two bytes and ignore the received bytes, then send a dummy byte so the slave can send us a byte.
    spi.writeByte(1);
    spi.writeByte(2);
    uint8_t result = spi.readByte();

    spi.flush(); // Always flush before deasserting CS 
    cs.setHigh();

    while (1);
}
