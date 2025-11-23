#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_uart.hpp"
#include "hal/debug_serial.hpp"

// To use the print functions we must tell them how to write a single byte by implementing this function:
void putchar_(char c) {
    uart.writeByte(c);
}

Pin<P1,7> tx;
Pin<P1,6> rx;
Uart<UART_A0> uart;

void main() {
    // Configure GPIO for UART mode
    tx.function(PinFunction::Primary);
    rx.function(PinFunction::Primary);

    // Baud rate config from datasheet: For 9600 baud with 1.048576 MHz clock (default)
    BaudConfig baud = {ucos16: true, ucbr: 6, ucbrf: 13, ucbrs: 0x22};
    // Note: This method has three default arguments: StopBits, BitOrder, and PacketLength. You probably don't need to change them.
    uart.init(ClockSource::Smclk, baud, ParityBit::Disabled);

    uint8_t i = 0;
    while (1) {
        printf_("Hello, world %d!\r\n", i);
        i++;
        __delay_cycles(1000000); // Wait 1 sec
    }
}
