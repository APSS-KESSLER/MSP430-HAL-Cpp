#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_i2c.hpp"

Pin<P1,3> scl;
Pin<P1,2> sda;
I2cMaster<I2C_B0> i2c;

void main() {
    // Configure GPIO for I2C mode
    scl.function(PinFunction::Primary);
    sda.function(PinFunction::Primary);

    // Note: This method has a default arguments for the stregnth of the glitch filter. You probably don't need to change it.
    i2c.init(I2cClockSource::Smclk, 10); // Smclk ~= 1MHz, so /10 gives ~100kHz

    // Example I2C slave address
    uint8_t slaveAddress = 0x3F;

    // Write 4 bytes to the device at address 0x3F.
    uint8_t bytes[4] = {1,2,3,4};
    i2c.write(slaveAddress, bytes, 4);

    // Read 3 bytes from the device at 0x3F.
    uint8_t buf[3];
    i2c.write(slaveAddress, buf, 3);

    // write_read() writes and then reads, commonly used for reading from a particular register on the slave.
    // Write 1 byte to the device at 0x3F, then send a repeated start and read 3 bytes (all within one I2C transaction).
    uint8_t send[1] = {1};
    uint8_t recv[3];
    i2c.write_read(slaveAddress, send, 1, recv, 3);

    // transaction() allows for arbitrary combinations of operations.
    I2cOperation ops[3] = {
        {I2cDirection::Transmit, bytes, 4}, 
        {I2cDirection::Receive,  recv,  3}, 
        {I2cDirection::Transmit, send,  1}, 
    };
    i2c.transaction(slaveAddress, ops, 3);

    while (1);
}
