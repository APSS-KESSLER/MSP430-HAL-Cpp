#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_i2c.hpp"

// Acts as an I2C master, sends various messages to 0x3F. See below for details.

// Diagrams of I2C messages are shown by each message for reference. The symbols below are used:
// S = Start, A = ACK (assumed), N = NACK, P = Stop, R = Repeated Start, W = Write bit, R = read bit, '->' means 'value written to'

Pin<P1,3> scl;
Pin<P1,2> sda;
I2cMaster<I2C_B0> i2c;

void main() {
    // Configure GPIO for I2C mode
    scl.function(PinFunction::Primary);
    sda.function(PinFunction::Primary);
    gpioUnlock();

    // You must initialise I2C before using it!
    // Note: This method has a default arguments for the stregnth of the glitch filter. You probably don't need to change it.
    i2c.init(I2cClockSource::Smclk, 10); // Smclk ~= 1MHz, so /10 gives ~100kHz

    // Example I2C slave address
    uint8_t slaveAddress = 0x3F;

    // Write 4 bytes to the device at address 0x3F:
    // | S | 0x3F+W | A | 1 | A | 2 | A | 3 | A | 4 | A | P |
    uint8_t bytes[4] = {1,2,3,4};
    i2c.write(slaveAddress, bytes, 4);
    // Note: The functions return  -1 if successful, otherwise it returns the byte number that received a NACK (0 = addr, 1 = first data byte, etc).

    // Read 3 bytes from the device at 0x3F.
    // | S | 0x3F+R | A | ->buf[0] | A | ->buf[1] | A | ->buf[2] | N | P |
    uint8_t buf[3];
    i2c.write(slaveAddress, buf, 3);

    // write_read() writes and then reads, commonly used for reading from a particular register on the slave.
    // Write 1 byte to the device at 0x3F, then send a repeated start and read 3 bytes (all within one I2C transaction).
    // | S | 0x3F+W | A | 1 | A 
    // | R | 0x3F+R | A | ->recv[0] | A | ->recv[1] | A | ->recv[2] | N | P |
    uint8_t send[1] = {1};
    uint8_t recv[3];
    i2c.write_read(slaveAddress, send, 1, recv, 3);

    // transaction() allows for arbitrary combinations of operations.
    I2cOperation ops[3] = {
        {I2cDirection::Transmit, bytes, 4}, 
        {I2cDirection::Receive,  recv,  3}, 
        {I2cDirection::Transmit, send,  1}, 
    };
    // | S | 0x3F+W | A | 1 | A | 2 | A | 3 | A | 4 | A 
    // | R | 0x3F+R | A | ->recv[0] | A | ->recv[1] | A | ->recv[2] | N 
    // | R | 0x3F+W | A | 1 | A | P |
    i2c.transaction(slaveAddress, ops, 3);

    while (1);
}
