#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_uart.hpp"
#include "hal/watchdog.hpp"

// Writes "Hello" once per second over eUSCI_A0 (P1.7, P1.6) at a baud rate of 9600

Pin<P1,7> tx;
Pin<P1,6> rx;
Uart<UART_A0> uart;
Watchdog watchdog;

void main() {
    watchdog.disable();
    
    // Configure GPIO for UART mode
    tx.function(PinFunction::Primary);
    rx.function(PinFunction::Primary);
    gpioUnlock();

    // Baud rate config from datasheet: For 9600 baud with 1.048576 MHz clock (default)
    const BaudConfig baud = {ucos16: true, ucbr: 6, ucbrf: 13, ucbrs: 0x22};
    // Or equivalently, common use cases have functions you can call to return this directly:
    // BaudConfig baud = BaudConfig::defaultSmclk9600Baud();
    
    // Note: This method has three default arguments: StopBits, BitOrder, and PacketLength. You probably don't need to change them.
    uart.init(ClockSource::Smclk, baud, ParityBit::Disabled);

    constexpr uint16_t len = 6;
    uint8_t arr[len] = "Hello";
    while (1) {
        uart.write(arr, len);
        __delay_cycles(1000000); // Wait 1 sec
    }
}
