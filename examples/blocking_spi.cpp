#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_spi.hpp"

// Acts as an SPI master, sending two SPI transactions and then does nothing. See below for details.

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
    cs.toOutput().setHigh(); // Chip select
    gpioUnlock();

    // You must initialise SPI before using it!
    // Note: This method has two default arguments: BitOrder and PacketLength. You probably don't need to change them.
    spi.init(SpiMode::MODE_0(), ClockSource::Smclk, 1);

    // There are two levels of abstractions on offer: A high level that sends buffers and manages the chip select pin automatically,
    // and a lower level that allows for manual chip select control and bytes sent individually. 
    
    /* Low level: SPI transactions can be performed manually via writeByte(), readByte() or transferByte() */
    // Note, you must manage the chip select pin yourself!
    
    // Send two bytes and ignores the received bytes, then sends a dummy byte so the slave can send us a byte:
    // CS:   ‾‾‾‾|_________________________|‾‾‾‾
    // MOSI:        |  1  |  2  |    0   |      
    // MISO:        | N/A | N/A | result |      
    cs.setLow();
        spi.writeByte(1);
        spi.writeByte(2);
        uint8_t result = spi.readByte();
        spi.flush(); // Always flush before deasserting CS to ensure sending has finished
    cs.setHigh();
    
    /* High level: transfer(), transferInPlace(), write() and receive() all accept a chip select pin to automatically toggle. */
    // Send two bytes with value 1, 2, and simultaneously store the received bytes in recvBuf:
    // CS:   ‾‾‾‾|_______________________________|‾‾‾‾
    // MOSI:        |     1      |     2      |       
    // MISO:        | recvBuf[0] | recvBuf[1] |       
    #define BUF_LEN 2 // If you know C++ you could use a constexpr for this
    uint8_t sendBuf[BUF_LEN] = {1, 2};
    uint8_t recvBuf[BUF_LEN];
    spi.transfer(sendBuf, BUF_LEN, recvBuf, BUF_LEN, cs);
    
    // Then do nothing.
    while (1);
}
