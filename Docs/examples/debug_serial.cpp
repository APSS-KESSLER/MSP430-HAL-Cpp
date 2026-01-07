#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_uart.hpp"
#include "hal/debug_serial.hpp"

// Repeatedly prints "Hello, world n!" once per second (starting from n=0), over eUSCI_A0 (P1.7, P1.6) at a baud rate of 9600.

// Define two GPIO pins (transmit and receive), and the UART peripheral we'll use for printing.
Pin<P1,7> tx;
Pin<P1,6> rx;
Uart<UART_A0> uart;

// The print functions internally call putchar_(), which is responsible for printing a single byte, 
// so we must implement it. In our case we 'print' it by sending it over UART.
void putchar_(char c) {
    uart.writeByte(c);
}

void main() {
    // Configure GPIO for UART mode. This allows the UART peripheral to control them.
    tx.function(PinFunction::Primary);
    rx.function(PinFunction::Primary);
    gpioUnlock();

    // Baud rate config from datasheet: For 9600 baud with 1.048576 MHz clock (default)
    BaudConfig baud = {ucos16: true, ucbr: 6, ucbrf: 13, ucbrs: 0x22};
    // Or equivalently, common use cases have functions you can call to return this directly:
    // BaudConfig baud = BaudConfig::defaultSmclk9600Baud();
    
    // You must initialise the UART peripheral before attempting to print anything!
    // Note: This method has three default arguments: StopBits, BitOrder, and PacketLength. You probably don't need to change them.
    uart.init(ClockSource::Smclk, baud, ParityBit::Disabled);

    // Print out "Hello world n!" in a loop.
    uint8_t i = 0;
    while (1) {
        // Note the underscore at the end of the function name - different from 'regular' printing.
        printf_("Hello, world %d!\r\n", i); 
        i++;
        __delay_cycles(1000000); // Wait 1 sec
    }
}
